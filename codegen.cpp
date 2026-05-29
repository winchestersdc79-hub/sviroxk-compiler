#include "codegen.h"
#include <llvm/Support/raw_ostream.h>
#include <iostream>

llvm::Value* CodeGen::getStringPtr(const std::string& str) {
    llvm::GlobalVariable* gv = builder.CreateGlobalString(str);
    return builder.CreateInBoundsGEP(gv->getValueType(), gv,
        {builder.getInt32(0), builder.getInt32(0)});
}

llvm::Type* CodeGen::svType(const std::string& name) {
    if (name == "ptr" || name == "file" || name == "cos")
        return builder.getPtrTy();
    if (name == "dor" || name == "udor")
        return builder.getDoubleTy();
    if (name == "chr")
        return builder.getInt8Ty();
    if (name == "rox64")
        return builder.getInt64Ty();
    return builder.getInt32Ty(); // rox, bue
}

llvm::Value* CodeGen::castTo(llvm::Value* val, llvm::Type* ty) {
    if (!val || !ty) return val;
    if (val->getType() == ty) return val;
    if (ty->isDoubleTy() && val->getType()->isIntegerTy())
        return builder.CreateSIToFP(val, ty);
    if (ty->isIntegerTy(32) && val->getType()->isDoubleTy())
        return builder.CreateFPToSI(val, ty);
    if (ty->isIntegerTy(64) && val->getType()->isIntegerTy(32))
        return builder.CreateSExt(val, ty);
    if (ty->isIntegerTy(32) && val->getType()->isIntegerTy(64))
        return builder.CreateTrunc(val, ty);
    if (ty->isPointerTy() && !val->getType()->isPointerTy())
        return val;
    return val;
}

llvm::Value* CodeGen::genExpr(const Node& node) {
    if (node.type == NODE_NUMBER) {
        if (node.value.find('.') != std::string::npos)
            return llvm::ConstantFP::get(builder.getDoubleTy(), std::stod(node.value));
        try {
            return builder.getInt32(std::stoi(node.value));
        } catch (...) {
            return builder.getInt64(std::stoll(node.value));
        }
    }
    if (node.type == NODE_STRING)
        return getStringPtr(node.value);
    if (node.type == NODE_STRUCT_FIELD) {
        std::string key = node.varName + "_" + node.op;
        llvm::AllocaInst* alloca = vars[key];
        if (alloca) {
            llvm::Type* ty = alloca->getAllocatedType();
            if (ty->isPointerTy())
                return builder.CreateLoad(builder.getPtrTy(), alloca);
            if (ty->isDoubleTy())
                return builder.CreateLoad(builder.getDoubleTy(), alloca);
            if (ty->isIntegerTy(64))
                return builder.CreateLoad(builder.getInt64Ty(), alloca);
            return builder.CreateLoad(builder.getInt32Ty(), alloca);
        }
        return builder.getInt32(0);
    }
    if (node.type == NODE_IDENTIFIER) {
        llvm::AllocaInst* alloca = vars[node.value];
        if (alloca) {
            llvm::Type* ty = alloca->getAllocatedType();
            if (ty->isPointerTy())
                return builder.CreateLoad(builder.getPtrTy(), alloca);
            if (ty->isDoubleTy())
                return builder.CreateLoad(builder.getDoubleTy(), alloca);
            if (ty->isIntegerTy(8))
                return builder.CreateLoad(builder.getInt8Ty(), alloca);
            if (ty->isIntegerTy(64))
                return builder.CreateLoad(builder.getInt64Ty(), alloca);
            return builder.CreateLoad(builder.getInt32Ty(), alloca);
        }
        return builder.getInt32(0);
    }
    if (node.type == NODE_ARRAY_ACCESS) {
        if (vars.count(node.varName) &&
            vars[node.varName]->getAllocatedType()->isPointerTy()) {
            llvm::Value* ptr = builder.CreateLoad(
                builder.getPtrTy(), vars[node.varName]);
            llvm::Value* idx = genExpr(*node.left);
            llvm::Value* gep = builder.CreateGEP(
                builder.getInt32Ty(), ptr, {idx});
            return builder.CreateLoad(builder.getInt32Ty(), gep);
        }
        if (arrays.count(node.varName)) {
            llvm::AllocaInst* arr = arrays[node.varName];
            unsigned n = arrayLengths[node.varName];
            llvm::ArrayType* at =
                llvm::ArrayType::get(builder.getInt32Ty(), n);
            llvm::Value* idx = genExpr(*node.left);
            llvm::Value* gep = builder.CreateInBoundsGEP(
                at, arr, {builder.getInt32(0), idx});
            return builder.CreateLoad(builder.getInt32Ty(), gep);
        }
        return builder.getInt32(0);
    }
    if (node.type == NODE_ADDR) {
        llvm::AllocaInst* alloca = vars[node.varName];
        if (alloca) return alloca;
        return builder.getInt32(0);
    }
    if (node.type == NODE_DEREF) {
        llvm::AllocaInst* alloca = vars[node.varName];
        if (alloca) {
            llvm::Value* ptr = builder.CreateLoad(builder.getPtrTy(), alloca);
            return builder.CreateLoad(builder.getInt32Ty(), ptr);
        }
        return builder.getInt32(0);
    }
    if (node.type == NODE_DYN_ARRAY) {
        llvm::FunctionType* mallocType =
            llvm::FunctionType::get(builder.getPtrTy(),
                {builder.getInt64Ty()}, false);
        llvm::Function* mallocF = module.getFunction("malloc");
        if (!mallocF) mallocF = llvm::Function::Create(mallocType,
            llvm::Function::ExternalLinkage, "malloc", module);
        llvm::Value* size = genExpr(*node.left);
        size = builder.CreateSExt(size, builder.getInt64Ty());
        llvm::Value* bytes = builder.CreateMul(size,
            builder.getInt64(4)); // 4 байта на int
        return builder.CreateCall(mallocF, {bytes});
    }
    if (node.type == NODE_CAST) {
        llvm::Value* val = genExpr(*node.left);
        if (node.varType == "dor") {
            if (val->getType()->isIntegerTy())
                return builder.CreateSIToFP(val, builder.getDoubleTy());
            return val;
        }
        if (node.varType == "rox") {
            if (val->getType()->isDoubleTy())
                return builder.CreateFPToSI(val, builder.getInt32Ty());
            return val;
        }
        if (node.varType == "rox64") {
            if (val->getType()->isDoubleTy())
                return builder.CreateFPToSI(val, builder.getInt64Ty());
            if (val->getType()->isIntegerTy(32))
                return builder.CreateSExt(val, builder.getInt64Ty());
            return val;
        }
        if (node.varType == "chr") {
            if (val->getType()->isIntegerTy(32))
                return builder.CreateTrunc(val, builder.getInt8Ty());
            return val;
        }
        return val;
    }
    if (node.type == NODE_FILE_READ) {
        llvm::AllocaInst* fileAlloca = vars[node.varName];
        if (!fileAlloca) return getStringPtr("");
        llvm::Value* fp = builder.CreateLoad(builder.getPtrTy(), fileAlloca);
        llvm::FunctionType* fgetsType = llvm::FunctionType::get(
            builder.getPtrTy(),
            {builder.getPtrTy(), builder.getInt32Ty(), builder.getPtrTy()}, false);
        llvm::Function* fgetsF = module.getFunction("fgets");
        if (!fgetsF) fgetsF = llvm::Function::Create(fgetsType,
            llvm::Function::ExternalLinkage, "fgets", module);
        llvm::AllocaInst* buf = builder.CreateAlloca(
            llvm::ArrayType::get(builder.getInt8Ty(), 4096), nullptr, "filebuf");
        llvm::Value* bufPtr = builder.CreateInBoundsGEP(
            llvm::ArrayType::get(builder.getInt8Ty(), 4096), buf,
            {builder.getInt32(0), builder.getInt32(0)});
        builder.CreateCall(fgetsF,
            {bufPtr, builder.getInt32(4096), fp});
        return bufPtr;
    }
    if (node.type == NODE_FILE_OPEN) {
        llvm::FunctionType* fopenType =
            llvm::FunctionType::get(builder.getPtrTy(),
                {builder.getPtrTy(), builder.getPtrTy()}, false);
        llvm::Function* fopenF = module.getFunction("fopen");
        if (!fopenF) fopenF = llvm::Function::Create(fopenType,
            llvm::Function::ExternalLinkage, "fopen", module);
        llvm::Value* fname = getStringPtr(node.value);
        llvm::Value* mode = getStringPtr("w");
        return builder.CreateCall(fopenF, {fname, mode});
    }
    if (node.type == NODE_INPUT && node.varType == "cos") {
        llvm::FunctionType* printfType =
            llvm::FunctionType::get(builder.getInt32Ty(),
                {builder.getPtrTy()}, true);
        llvm::Function* printfF = module.getFunction("printf");
        if (!printfF) printfF = llvm::Function::Create(printfType,
            llvm::Function::ExternalLinkage, "printf", module);
        llvm::FunctionType* scanfType =
            llvm::FunctionType::get(builder.getInt32Ty(),
                {builder.getPtrTy()}, true);
        llvm::Function* scanfF = module.getFunction("scanf");
        if (!scanfF) scanfF = llvm::Function::Create(scanfType,
            llvm::Function::ExternalLinkage, "scanf", module);
        llvm::Value* prompt = getStringPtr(node.value);
        builder.CreateCall(printfF, {prompt});
        llvm::AllocaInst* buf = builder.CreateAlloca(
            llvm::ArrayType::get(builder.getInt8Ty(), 256), nullptr, "strinput");
        llvm::Value* bufPtr = builder.CreateInBoundsGEP(
            llvm::ArrayType::get(builder.getInt8Ty(), 256), buf,
            {builder.getInt32(0), builder.getInt32(0)});
        llvm::Value* fmt = getStringPtr("%255s");
        builder.CreateCall(scanfF, {fmt, bufPtr});
        return bufPtr;
    }
    if (node.type == NODE_INPUT) {
        llvm::FunctionType* printfType =
            llvm::FunctionType::get(builder.getInt32Ty(),
                {builder.getPtrTy()}, true);
        llvm::Function* printfF = module.getFunction("printf");
        if (!printfF) printfF = llvm::Function::Create(printfType,
            llvm::Function::ExternalLinkage, "printf", module);
        llvm::FunctionType* scanfType =
            llvm::FunctionType::get(builder.getInt32Ty(),
                {builder.getPtrTy()}, true);
        llvm::Function* scanfF = module.getFunction("scanf");
        if (!scanfF) scanfF = llvm::Function::Create(scanfType,
            llvm::Function::ExternalLinkage, "scanf", module);
        llvm::Value* prompt = getStringPtr(node.value);
        builder.CreateCall(printfF, {prompt});
        llvm::AllocaInst* tmp =
            builder.CreateAlloca(builder.getInt32Ty(), nullptr, "input_tmp");
        llvm::Value* fmt = getStringPtr("%d");
        builder.CreateCall(scanfF, {fmt, tmp});
        return builder.CreateLoad(builder.getInt32Ty(), tmp);
    }
    if (node.type == NODE_BINOP) {
        llvm::Value* L = genExpr(*node.left);
        llvm::Value* R = genExpr(*node.right);
        bool isFloat = L->getType()->isDoubleTy() || R->getType()->isDoubleTy();
        if (isFloat) {
            if (L->getType()->isIntegerTy())
                L = builder.CreateSIToFP(L, builder.getDoubleTy());
            if (R->getType()->isIntegerTy())
                R = builder.CreateSIToFP(R, builder.getDoubleTy());
            if (node.op == "+") return builder.CreateFAdd(L, R);
            if (node.op == "-") return builder.CreateFSub(L, R);
            if (node.op == "*") return builder.CreateFMul(L, R);
            if (node.op == "/") return builder.CreateFDiv(L, R);
            if (node.op == ">") return builder.CreateFCmpOGT(L, R);
            if (node.op == "<") return builder.CreateFCmpOLT(L, R);
            if (node.op == "==") return builder.CreateFCmpOEQ(L, R);
            if (node.op == "!=") return builder.CreateFCmpONE(L, R);
        }
        if (node.op == "+") return builder.CreateAdd(L, R);
        if (node.op == "-") return builder.CreateSub(L, R);
        if (node.op == "*") return builder.CreateMul(L, R);
        if (node.op == "/") return builder.CreateSDiv(L, R);
        if (node.op == ">") return builder.CreateICmpSGT(L, R);
        if (node.op == "<") return builder.CreateICmpSLT(L, R);
        if (node.op == "==") return builder.CreateICmpEQ(L, R);
        if (node.op == "!=") return builder.CreateICmpNE(L, R);
    }
    if (node.type == NODE_FUNC_CALL) {
        std::string name = node.varName;
        // математика
        if (name == "sqrt") {
            llvm::FunctionType* ft = llvm::FunctionType::get(
                builder.getDoubleTy(), {builder.getDoubleTy()}, false);
            llvm::Function* f = module.getFunction("sqrt");
            if (!f) f = llvm::Function::Create(ft,
                llvm::Function::ExternalLinkage, "sqrt", module);
            llvm::Value* arg = builder.CreateSIToFP(
                genExpr(node.args[0]), builder.getDoubleTy());
            return builder.CreateFPToSI(
                builder.CreateCall(f, {arg}), builder.getInt32Ty());
        }
        if (name == "abs") {
            llvm::Value* arg = genExpr(node.args[0]);
            llvm::Value* neg = builder.CreateNeg(arg);
            llvm::Value* cmp = builder.CreateICmpSLT(arg, builder.getInt32(0));
            return builder.CreateSelect(cmp, neg, arg);
        }
        if (name == "pow") {
            llvm::FunctionType* ft = llvm::FunctionType::get(
                builder.getDoubleTy(),
                {builder.getDoubleTy(), builder.getDoubleTy()}, false);
            llvm::Function* f = module.getFunction("pow");
            if (!f) f = llvm::Function::Create(ft,
                llvm::Function::ExternalLinkage, "pow", module);
            llvm::Value* a = builder.CreateSIToFP(
                genExpr(node.args[0]), builder.getDoubleTy());
            llvm::Value* b = builder.CreateSIToFP(
                genExpr(node.args[1]), builder.getDoubleTy());
            return builder.CreateFPToSI(
                builder.CreateCall(f, {a, b}), builder.getInt32Ty());
        }
        if (name == "max") {
            llvm::Value* a = genExpr(node.args[0]);
            llvm::Value* b = genExpr(node.args[1]);
            return builder.CreateSelect(builder.CreateICmpSGT(a, b), a, b);
        }
        if (name == "min") {
            llvm::Value* a = genExpr(node.args[0]);
            llvm::Value* b = genExpr(node.args[1]);
            return builder.CreateSelect(builder.CreateICmpSLT(a, b), a, b);
        }
        if (name == "str") {
            llvm::FunctionType* sprintfType =
                llvm::FunctionType::get(builder.getInt32Ty(),
                    {builder.getPtrTy(), builder.getPtrTy()}, true);
            llvm::Function* sprintfF = module.getFunction("sprintf");
            if (!sprintfF) sprintfF = llvm::Function::Create(sprintfType,
                llvm::Function::ExternalLinkage, "sprintf", module);
            llvm::AllocaInst* buf = builder.CreateAlloca(
                llvm::ArrayType::get(builder.getInt8Ty(), 64), nullptr, "strbuf");
            llvm::Value* bufPtr = builder.CreateInBoundsGEP(
                llvm::ArrayType::get(builder.getInt8Ty(), 64), buf,
                {builder.getInt32(0), builder.getInt32(0)});
            llvm::Value* val = genExpr(node.args[0]);
            llvm::Value* fmt = getStringPtr("%d");
            builder.CreateCall(sprintfF, {bufPtr, fmt, val});
            return bufPtr;
        }
        if (name == "ser") {
            llvm::FunctionType* strcmpType =
                llvm::FunctionType::get(builder.getInt32Ty(),
                    {builder.getPtrTy(), builder.getPtrTy()}, false);
            llvm::Function* strcmpF = module.getFunction("strcmp");
            if (!strcmpF) strcmpF = llvm::Function::Create(strcmpType,
                llvm::Function::ExternalLinkage, "strcmp", module);
            llvm::Value* s1 = genExpr(node.args[0]);
            llvm::Value* s2 = genExpr(node.args[1]);
            llvm::Value* res = builder.CreateCall(strcmpF, {s1, s2});
            return builder.CreateICmpEQ(res, builder.getInt32(0));
        }
        if (name == "rep" && node.args.size() >= 3) {
            llvm::FunctionType* strstrType = llvm::FunctionType::get(
                builder.getPtrTy(),
                {builder.getPtrTy(), builder.getPtrTy()}, false);
            llvm::Function* strstrF = module.getFunction("strstr");
            if (!strstrF) strstrF = llvm::Function::Create(strstrType,
                llvm::Function::ExternalLinkage, "strstr", module);
            llvm::FunctionType* strcpyType = llvm::FunctionType::get(
                builder.getPtrTy(),
                {builder.getPtrTy(), builder.getPtrTy()}, false);
            llvm::Function* strcpyF = module.getFunction("strcpy");
            if (!strcpyF) strcpyF = llvm::Function::Create(strcpyType,
                llvm::Function::ExternalLinkage, "strcpy", module);
            llvm::FunctionType* strcatType = llvm::FunctionType::get(
                builder.getPtrTy(),
                {builder.getPtrTy(), builder.getPtrTy()}, false);
            llvm::Function* strcatF = module.getFunction("strcat");
            if (!strcatF) strcatF = llvm::Function::Create(strcatType,
                llvm::Function::ExternalLinkage, "strcat", module);
            llvm::Value* hay = genExpr(node.args[0]);
            llvm::Value* oldv = genExpr(node.args[1]);
            llvm::Value* newv = genExpr(node.args[2]);
            llvm::AllocaInst* out = builder.CreateAlloca(
                llvm::ArrayType::get(builder.getInt8Ty(), 512), nullptr, "repbuf");
            llvm::Value* outPtr = builder.CreateInBoundsGEP(
                llvm::ArrayType::get(builder.getInt8Ty(), 512), out,
                {builder.getInt32(0), builder.getInt32(0)});
            builder.CreateCall(strcpyF, {outPtr, hay});
            llvm::Value* pos = builder.CreateCall(strstrF, {outPtr, oldv});
            llvm::Value* found = builder.CreateICmpNE(
                pos, llvm::ConstantPointerNull::get(builder.getPtrTy()));
            llvm::Function* fn = builder.GetInsertBlock()->getParent();
            llvm::BasicBlock* repl = llvm::BasicBlock::Create(context, "rep_yes", fn);
            llvm::BasicBlock* done = llvm::BasicBlock::Create(context, "rep_done", fn);
            builder.CreateCondBr(found, repl, done);
            builder.SetInsertPoint(repl);
            builder.CreateCall(strcatF, {outPtr, newv});
            builder.CreateBr(done);
            builder.SetInsertPoint(done);
            return outPtr;
        }
        if (name == "flo" || name == "cel" || name == "ron") {
            llvm::FunctionType* ft = llvm::FunctionType::get(
                builder.getDoubleTy(), {builder.getDoubleTy()}, false);
            llvm::Function* f = module.getFunction(
                name == "flo" ? "floor" : name == "cel" ? "ceil" : "round");
            std::string cname = name == "flo" ? "floor" : name == "cel" ? "ceil" : "round";
            f = module.getFunction(cname);
            if (!f) f = llvm::Function::Create(ft,
                llvm::Function::ExternalLinkage, cname, module);
            llvm::Value* arg = genExpr(node.args[0]);
            if (arg->getType()->isIntegerTy())
                arg = builder.CreateSIToFP(arg, builder.getDoubleTy());
            llvm::Value* res = builder.CreateCall(f, {arg});
            return builder.CreateFPToSI(res, builder.getInt32Ty());
        }
        if (name == "ran") {
            llvm::FunctionType* ft = llvm::FunctionType::get(
                builder.getInt32Ty(), {}, false);
            llvm::Function* f = module.getFunction("rand");
            if (!f) f = llvm::Function::Create(ft,
                llvm::Function::ExternalLinkage, "rand", module);
            return builder.CreateCall(f, {});
        }
        if (name == "ar") {
            llvm::FunctionType* strlenType =
                llvm::FunctionType::get(builder.getInt32Ty(),
                    {builder.getPtrTy()}, false);
            llvm::Function* strlenF = module.getFunction("strlen");
            if (!strlenF) strlenF = llvm::Function::Create(strlenType,
                llvm::Function::ExternalLinkage, "strlen", module);
            llvm::Value* str = genExpr(node.args[0]);
            return builder.CreateCall(strlenF, {str});
        }
        // пользовательские функции
        if (funcs.count(name)) {
            std::vector<llvm::Value*> argVals;
            llvm::Function* fn = funcs[name];
            const auto& ptypes = funcParamTypes[name];
            for (size_t i = 0; i < node.args.size(); i++) {
                llvm::Value* v = genExpr(node.args[i]);
                if (i < ptypes.size())
                    v = castTo(v, svType(ptypes[i]));
                argVals.push_back(v);
            }
            llvm::Value* ret = builder.CreateCall(fn, argVals);
            return castTo(ret, builder.getInt32Ty());
        }
        return builder.getInt32(0);
    }
    return builder.getInt32(0);
}

void CodeGen::genNode(const Node& node) {
    if (node.type == NODE_SLOV) {
        if (node.left && node.left->type == NODE_STRING) {
            llvm::Value* str = getStringPtr(node.left->value);
            builder.CreateCall(putsFunc, {str});
        } else if (node.left) {
            llvm::Value* val = genExpr(*node.left);
            if (val->getType()->isPointerTy()) {
                // строка — выводим через puts
                builder.CreateCall(putsFunc, {val});
            } else if (val->getType()->isDoubleTy()) {
                llvm::Value* fmt = getStringPtr("%f\n");
                builder.CreateCall(printfFunc, {fmt, val});
            } else if (val->getType()->isIntegerTy(8)) {
                llvm::Value* fmt = getStringPtr("%c\n");
                builder.CreateCall(printfFunc, {fmt, val});
            } else if (val->getType()->isIntegerTy(64)) {
                llvm::Value* fmt = getStringPtr("%lld\n");
                builder.CreateCall(printfFunc, {fmt, val});
            } else {
                llvm::Value* fmt = getStringPtr("%d\n");
                builder.CreateCall(printfFunc, {fmt, val});
            }
        } else {
            llvm::Value* str = getStringPtr(node.value);
            builder.CreateCall(putsFunc, {str});
        }
    }
    else if (node.type == NODE_VAR_DECL) {
        llvm::Type* ty = svType(node.varType);
        llvm::AllocaInst* alloca =
            builder.CreateAlloca(ty, nullptr, node.varName);
        llvm::Value* val = genExpr(*node.left);
        if (node.varType == "dor" || node.varType == "udor") {
            if (val->getType()->isIntegerTy())
                val = builder.CreateSIToFP(val, builder.getDoubleTy());
        }
        builder.CreateStore(val, alloca);
        vars[node.varName] = alloca;
    }
    else if (node.type == NODE_ASSIGN) {
        llvm::AllocaInst* alloca = vars[node.varName];
        if (alloca) {
            llvm::Value* val = genExpr(*node.left);
            llvm::Type* ty = alloca->getAllocatedType();
            if (ty->isDoubleTy() && val->getType()->isIntegerTy())
                val = builder.CreateSIToFP(val, builder.getDoubleTy());
            if (ty->isIntegerTy(32) && val->getType()->isDoubleTy())
                val = builder.CreateFPToSI(val, builder.getInt32Ty());
            builder.CreateStore(val, alloca);
        }
    }
    else if (node.type == NODE_ARRAY_ASSIGN) {
        if (node.left && node.right) {
            // динамический массив
            if (vars.count(node.varName) &&
                vars[node.varName]->getAllocatedType()->isPointerTy()) {
                llvm::Value* ptr = builder.CreateLoad(
                    builder.getPtrTy(), vars[node.varName]);
                llvm::Value* idx = genExpr(*node.left);
                llvm::Value* gep = builder.CreateGEP(
                    builder.getInt32Ty(), ptr, {idx});
                builder.CreateStore(genExpr(*node.right), gep);
            } else if (arrays.count(node.varName)) {
                llvm::AllocaInst* arr = arrays[node.varName];
                unsigned n = arrayLengths[node.varName];
                llvm::ArrayType* at =
                    llvm::ArrayType::get(builder.getInt32Ty(), n);
                llvm::Value* idx = genExpr(*node.left);
                llvm::Value* gep = builder.CreateInBoundsGEP(
                    at, arr, {builder.getInt32(0), idx});
                builder.CreateStore(genExpr(*node.right), gep);
            }
        }
    }
    else if (node.type == NODE_DEREF_ASSIGN) {
        llvm::AllocaInst* alloca = vars[node.varName];
        if (alloca) {
            llvm::Value* ptr = builder.CreateLoad(builder.getPtrTy(), alloca);
            builder.CreateStore(genExpr(*node.left), ptr);
        }
    }
    else if (node.type == NODE_RETURN) {
        builder.CreateRet(genExpr(*node.left));
    }
    else if (node.type == NODE_BREAK) {
        if (breakBlock) builder.CreateBr(breakBlock);
    }
    else if (node.type == NODE_CONTINUE) {
        if (continueBlock) builder.CreateBr(continueBlock);
    }
    else if (node.type == NODE_ARRAY_DECL) {
        unsigned n = (unsigned)node.args.size();
        llvm::ArrayType* at =
            llvm::ArrayType::get(builder.getInt32Ty(), n);
        llvm::AllocaInst* arr =
            builder.CreateAlloca(at, nullptr, node.varName);
        arrays[node.varName] = arr;
        arrayLengths[node.varName] = n;
        for (unsigned i = 0; i < n; i++) {
            llvm::Value* gep = builder.CreateInBoundsGEP(
                at, arr, {builder.getInt32(0), builder.getInt32(i)});
            builder.CreateStore(genExpr(node.args[i]), gep);
        }
    }
    else if (node.type == NODE_STRUCT_DEF) {
        std::vector<StructField> fields;
        for (const Node& field : node.children) {
            StructField sf;
            sf.type = field.varType;
            sf.name = field.varName;
            sf.defaultVal = *field.left;
            fields.push_back(sf);
        }
        structDefs[node.varName] = fields;
    }
    else if (node.type == NODE_STRUCT_INSTANCE) {
        if (!structDefs.count(node.varType)) {
            std::cerr << "Неизвестная структура: " << node.varType << std::endl;
            exit(1);
        }
        for (const StructField& f : structDefs[node.varType]) {
            std::string full = node.varName + "_" + f.name;
            llvm::Type* ty = svType(f.type);
            llvm::AllocaInst* alloca =
                builder.CreateAlloca(ty, nullptr, full);
            llvm::Value* val = genExpr(f.defaultVal);
            val = castTo(val, ty);
            builder.CreateStore(val, alloca);
            vars[full] = alloca;
        }
    }
    else if (node.type == NODE_FUNC_DEF) {
        std::vector<llvm::Type*> paramTypes;
        for (const std::string& p : node.params) {
            std::string ptype = p.substr(0, p.find(':'));
            paramTypes.push_back(svType(ptype));
        }
        std::vector<std::string> ptypes;
        for (const std::string& p : node.params)
            ptypes.push_back(p.substr(0, p.find(':')));
        funcParamTypes[node.varName] = ptypes;
        llvm::FunctionType* ft =
            llvm::FunctionType::get(builder.getInt32Ty(), paramTypes, false);
        llvm::Function* func =
            llvm::Function::Create(ft,
                llvm::Function::ExternalLinkage, node.varName, module);
        llvm::BasicBlock* block =
            llvm::BasicBlock::Create(context, "entry", func);
        llvm::BasicBlock* savedBlock = builder.GetInsertBlock();
        auto savedVars = vars;
        auto savedArrays = arrays;
        auto savedArrayLengths = arrayLengths;
        builder.SetInsertPoint(block);
        int idx = 0;
        for (auto& arg : func->args()) {
            std::string entry = node.params[idx];
            std::string ptype = entry.substr(0, entry.find(':'));
            std::string pname = entry.substr(entry.find(':') + 1);
            idx++;
            arg.setName(pname);
            llvm::Type* ty = svType(ptype);
            llvm::AllocaInst* alloca =
                builder.CreateAlloca(ty, nullptr, pname);
            builder.CreateStore(&arg, alloca);
            vars[pname] = alloca;
        }
        bool hasRet = false;
        for (const Node& n : node.children) {
            genNode(n);
            if (n.type == NODE_RETURN) { hasRet = true; break; }
        }
        if (!hasRet) builder.CreateRet(builder.getInt32(0));
        builder.SetInsertPoint(savedBlock);
        vars = savedVars;
        arrays = savedArrays;
        arrayLengths = savedArrayLengths;
        funcs[node.varName] = func;
    }
    else if (node.type == NODE_FUNC_CALL) {
        if (funcs.count(node.varName)) {
            std::vector<llvm::Value*> argVals;
            const auto& ptypes = funcParamTypes[node.varName];
            for (size_t i = 0; i < node.args.size(); i++) {
                llvm::Value* v = genExpr(node.args[i]);
                if (i < ptypes.size())
                    v = castTo(v, svType(ptypes[i]));
                argVals.push_back(v);
            }
            builder.CreateCall(funcs[node.varName], argVals);
        } else {
            genExpr(node);
        }
    }
    else if (node.type == NODE_DELETE) {
        llvm::FunctionType* freeType =
            llvm::FunctionType::get(builder.getVoidTy(),
                {builder.getPtrTy()}, false);
        llvm::Function* freeF = module.getFunction("free");
        if (!freeF) freeF = llvm::Function::Create(freeType,
            llvm::Function::ExternalLinkage, "free", module);
        llvm::AllocaInst* alloca = vars[node.varName];
        if (alloca) {
            llvm::Value* ptr = builder.CreateLoad(builder.getPtrTy(), alloca);
            builder.CreateCall(freeF, {ptr});
        }
    }
    else if (node.type == NODE_FILE_WRITE) {
        llvm::FunctionType* fputsType =
            llvm::FunctionType::get(builder.getInt32Ty(),
                {builder.getPtrTy(), builder.getPtrTy()}, false);
        llvm::Function* fputsF = module.getFunction("fputs");
        if (!fputsF) fputsF = llvm::Function::Create(fputsType,
            llvm::Function::ExternalLinkage, "fputs", module);
        llvm::FunctionType* fcloseType =
            llvm::FunctionType::get(builder.getInt32Ty(),
                {builder.getPtrTy()}, false);
        llvm::AllocaInst* fileAlloca = vars[node.varName];
        if (fileAlloca) {
            llvm::Value* filePtr =
                builder.CreateLoad(builder.getPtrTy(), fileAlloca);
            llvm::Value* str = genExpr(*node.left);
            builder.CreateCall(fputsF, {str, filePtr});
        }
    }
    else if (node.type == NODE_INPUT) {
        genExpr(node);
    }
    else if (node.type == NODE_FILE_CLOSE) {
        llvm::FunctionType* fcloseType =
            llvm::FunctionType::get(builder.getInt32Ty(),
                {builder.getPtrTy()}, false);
        llvm::Function* fcloseF = module.getFunction("fclose");
        if (!fcloseF) fcloseF = llvm::Function::Create(fcloseType,
            llvm::Function::ExternalLinkage, "fclose", module);
        llvm::AllocaInst* fileAlloca = vars[node.varName];
        if (fileAlloca) {
            llvm::Value* filePtr =
                builder.CreateLoad(builder.getPtrTy(), fileAlloca);
            builder.CreateCall(fcloseF, {filePtr});
        }
    }
    else if (node.type == NODE_IF) {
        llvm::Value* cond = genExpr(*node.left);
        if (cond->getType() == builder.getInt32Ty())
            cond = builder.CreateICmpNE(cond, builder.getInt32(0));
        llvm::Function* func = builder.GetInsertBlock()->getParent();
        llvm::BasicBlock* thenBlock =
            llvm::BasicBlock::Create(context, "then", func);
        llvm::BasicBlock* elseBlock =
            llvm::BasicBlock::Create(context, "else", func);
        llvm::BasicBlock* mergeBlock =
            llvm::BasicBlock::Create(context, "merge", func);
        builder.CreateCondBr(cond, thenBlock, elseBlock);
        builder.SetInsertPoint(thenBlock);
        for (const Node& n : node.children) genNode(n);
        if (!builder.GetInsertBlock()->getTerminator())
            builder.CreateBr(mergeBlock);
        builder.SetInsertPoint(elseBlock);
        for (const Node& n : node.elseChildren) genNode(n);
        if (!builder.GetInsertBlock()->getTerminator())
            builder.CreateBr(mergeBlock);
        builder.SetInsertPoint(mergeBlock);
    }
    else if (node.type == NODE_LOOP_N) {
        llvm::Value* count = genExpr(*node.left);
        llvm::Function* func = builder.GetInsertBlock()->getParent();
        llvm::AllocaInst* i =
            builder.CreateAlloca(builder.getInt32Ty(), nullptr, "i");
        builder.CreateStore(builder.getInt32(0), i);
        llvm::BasicBlock* loopBlock =
            llvm::BasicBlock::Create(context, "loop", func);
        llvm::BasicBlock* afterBlock =
            llvm::BasicBlock::Create(context, "after", func);
        llvm::BasicBlock* bodyBlock =
            llvm::BasicBlock::Create(context, "body", func);
        auto savedBreak = breakBlock;
        auto savedContinue = continueBlock;
        breakBlock = afterBlock;
        continueBlock = loopBlock;
        builder.CreateBr(loopBlock);
        builder.SetInsertPoint(loopBlock);
        llvm::Value* cur = builder.CreateLoad(builder.getInt32Ty(), i);
        llvm::Value* cond = builder.CreateICmpSLT(cur, count);
        builder.CreateCondBr(cond, bodyBlock, afterBlock);
        builder.SetInsertPoint(bodyBlock);
        for (const Node& n : node.children) genNode(n);
        if (!builder.GetInsertBlock()->getTerminator()) {
            llvm::Value* next = builder.CreateAdd(cur, builder.getInt32(1));
            builder.CreateStore(next, i);
            builder.CreateBr(loopBlock);
        }
        builder.SetInsertPoint(afterBlock);
        breakBlock = savedBreak;
        continueBlock = savedContinue;
    }
    else if (node.type == NODE_LOOP_W) {
        llvm::Function* func = builder.GetInsertBlock()->getParent();
        llvm::BasicBlock* condBlock =
            llvm::BasicBlock::Create(context, "cond", func);
        llvm::BasicBlock* bodyBlock =
            llvm::BasicBlock::Create(context, "body", func);
        llvm::BasicBlock* afterBlock =
            llvm::BasicBlock::Create(context, "after", func);
        auto savedBreak = breakBlock;
        auto savedContinue = continueBlock;
        breakBlock = afterBlock;
        continueBlock = condBlock;
        builder.CreateBr(condBlock);
        builder.SetInsertPoint(condBlock);
        llvm::Value* cond = genExpr(*node.left);
        if (cond->getType() == builder.getInt32Ty())
            cond = builder.CreateICmpNE(cond, builder.getInt32(0));
        builder.CreateCondBr(cond, bodyBlock, afterBlock);
        builder.SetInsertPoint(bodyBlock);
        for (const Node& n : node.children) genNode(n);
        if (!builder.GetInsertBlock()->getTerminator())
            builder.CreateBr(condBlock);
        builder.SetInsertPoint(afterBlock);
        breakBlock = savedBreak;
        continueBlock = savedContinue;
    }
}

void CodeGen::generate(const Node& program) {
    llvm::FunctionType* putsType =
        llvm::FunctionType::get(builder.getInt32Ty(),
            {builder.getPtrTy()}, false);
    putsFunc = llvm::Function::Create(putsType,
        llvm::Function::ExternalLinkage, "puts", module);

    llvm::FunctionType* printfType =
        llvm::FunctionType::get(builder.getInt32Ty(),
            {builder.getPtrTy()}, true);
    printfFunc = llvm::Function::Create(printfType,
        llvm::Function::ExternalLinkage, "printf", module);

    for (const Node& node : program.children)
        if (node.type == NODE_FUNC_DEF) genNode(node);

    llvm::FunctionType* funcType =
        llvm::FunctionType::get(builder.getInt32Ty(), false);
    llvm::Function* mainFunc =
        llvm::Function::Create(funcType,
            llvm::Function::ExternalLinkage, "main", module);
    llvm::BasicBlock* block =
        llvm::BasicBlock::Create(context, "entry", mainFunc);
    builder.SetInsertPoint(block);

    for (const Node& node : program.children)
        if (node.type != NODE_FUNC_DEF) genNode(node);

    builder.CreateRet(builder.getInt32(0));
}

void CodeGen::print() {
    module.print(llvm::outs(), nullptr);
}
