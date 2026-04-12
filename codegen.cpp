#include "codegen.h"
#include <llvm/Support/raw_ostream.h>
#include <iostream>

llvm::Value* CodeGen::getStringPtr(const std::string& str) {
    llvm::GlobalVariable* gv = builder.CreateGlobalString(str);
    return builder.CreateInBoundsGEP(gv->getValueType(), gv,
        {builder.getInt32(0), builder.getInt32(0)});
}

llvm::Value* CodeGen::genExpr(const Node& node) {
    if (node.type == NODE_NUMBER)
        return builder.getInt32(std::stoi(node.value));
    if (node.type == NODE_FUNC_CALL) {
        // встроенная математика
        if (node.varName == "sqrt" || node.varName == "abs" ||
            node.varName == "pow"  || node.varName == "max" ||
            node.varName == "min") {
            std::string name = node.varName;
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
                return builder.CreateSelect(
                    builder.CreateICmpSGT(a, b), a, b);
            }
            if (name == "min") {
                llvm::Value* a = genExpr(node.args[0]);
                llvm::Value* b = genExpr(node.args[1]);
                return builder.CreateSelect(
                    builder.CreateICmpSLT(a, b), a, b);
            }
        }
        if (funcs.count(node.varName)) {
            std::vector<llvm::Value*> argVals;
            for (const Node& a : node.args)
                argVals.push_back(genExpr(a));
            return builder.CreateCall(funcs[node.varName], argVals);
        }
        return builder.getInt32(0);
    }
    // встроенные математические функции
    if (node.type == NODE_FUNC_CALL &&
        (node.varName == "sqrt" || node.varName == "abs" ||
         node.varName == "pow"  || node.varName == "max" ||
         node.varName == "min")) {
        std::string name = node.varName;
        if (name == "sqrt") {
            llvm::FunctionType* ft =
                llvm::FunctionType::get(builder.getDoubleTy(),
                    {builder.getDoubleTy()}, false);
            llvm::Function* f = module.getFunction("sqrt");
            if (!f) f = llvm::Function::Create(ft,
                llvm::Function::ExternalLinkage, "sqrt", module);
            llvm::Value* arg = genExpr(node.args[0]);
            arg = builder.CreateSIToFP(arg, builder.getDoubleTy());
            llvm::Value* res = builder.CreateCall(f, {arg});
            return builder.CreateFPToSI(res, builder.getInt32Ty());
        }
        if (name == "abs") {
            llvm::Value* arg = genExpr(node.args[0]);
            llvm::Value* neg = builder.CreateNeg(arg);
            llvm::Value* cmp = builder.CreateICmpSLT(arg, builder.getInt32(0));
            return builder.CreateSelect(cmp, neg, arg);
        }
        if (name == "pow") {
            llvm::FunctionType* ft =
                llvm::FunctionType::get(builder.getDoubleTy(),
                    {builder.getDoubleTy(), builder.getDoubleTy()}, false);
            llvm::Function* f = module.getFunction("pow");
            if (!f) f = llvm::Function::Create(ft,
                llvm::Function::ExternalLinkage, "pow", module);
            llvm::Value* a = builder.CreateSIToFP(genExpr(node.args[0]), builder.getDoubleTy());
            llvm::Value* b = builder.CreateSIToFP(genExpr(node.args[1]), builder.getDoubleTy());
            llvm::Value* res = builder.CreateCall(f, {a, b});
            return builder.CreateFPToSI(res, builder.getInt32Ty());
        }
        if (name == "max") {
            llvm::Value* a = genExpr(node.args[0]);
            llvm::Value* b = genExpr(node.args[1]);
            llvm::Value* cmp = builder.CreateICmpSGT(a, b);
            return builder.CreateSelect(cmp, a, b);
        }
        if (name == "min") {
            llvm::Value* a = genExpr(node.args[0]);
            llvm::Value* b = genExpr(node.args[1]);
            llvm::Value* cmp = builder.CreateICmpSLT(a, b);
            return builder.CreateSelect(cmp, a, b);
        }
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
    if (node.type == NODE_FILE_OPEN) {
        llvm::FunctionType* fopenType =
            llvm::FunctionType::get(builder.getPtrTy(),
                {builder.getPtrTy(), builder.getPtrTy()}, false);
        llvm::Function* fopenFunc = module.getFunction("fopen");
        if (!fopenFunc) {
            fopenFunc = llvm::Function::Create(fopenType,
                llvm::Function::ExternalLinkage, "fopen", module);
        }
        llvm::Value* fname = getStringPtr(node.value);
        llvm::Value* mode = getStringPtr("w");
        return builder.CreateCall(fopenFunc, {fname, mode});
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
    if (node.type == NODE_ARRAY_ACCESS) {
        int idx = std::stoi(node.left->value);
        std::string name = node.varName + "_" + std::to_string(idx);
        llvm::AllocaInst* alloca = vars[name];
        if (alloca) return builder.CreateLoad(builder.getInt32Ty(), alloca);
        return builder.getInt32(0);
    }
    if (node.type == NODE_IDENTIFIER) {
        llvm::AllocaInst* alloca = vars[node.value];
        if (alloca) return builder.CreateLoad(builder.getInt32Ty(), alloca);
        return builder.getInt32(0);
    }
    if (node.type == NODE_BINOP) {
        llvm::Value* L = genExpr(*node.left);
        llvm::Value* R = genExpr(*node.right);
        if (node.op == "+") return builder.CreateAdd(L, R);
        if (node.op == "-") return builder.CreateSub(L, R);
        if (node.op == "*") return builder.CreateMul(L, R);
        if (node.op == "/") return builder.CreateSDiv(L, R);
        if (node.op == ">") return builder.CreateICmpSGT(L, R);
        if (node.op == "<") return builder.CreateICmpSLT(L, R);
        if (node.op == "==") return builder.CreateICmpEQ(L, R);
        if (node.op == "!=") return builder.CreateICmpNE(L, R);
    }
    return builder.getInt32(0);
}

void CodeGen::genNode(const Node& node) {
    if (node.type == NODE_SLOV) {
        if (node.left && node.left->type == NODE_STRING) {
            llvm::Value* str = getStringPtr(node.left->value);
            builder.CreateCall(putsFunc, {str});
        } else if (node.left && (
            node.left->type == NODE_IDENTIFIER ||
            node.left->type == NODE_ARRAY_ACCESS ||
            node.left->type == NODE_FUNC_CALL ||
            node.left->type == NODE_DEREF)) {
            llvm::Value* fmt = getStringPtr("%d\n");
            llvm::Value* val = genExpr(*node.left);
            builder.CreateCall(printfFunc, {fmt, val});
        } else {
            llvm::Value* str = getStringPtr(node.value);
            builder.CreateCall(putsFunc, {str});
        }
    }
    else if (node.type == NODE_VAR_DECL) {
        llvm::Type* ty = (node.varType == "ptr" || node.varType == "file" || node.varType == "cos") ?
            (llvm::Type*)builder.getPtrTy() :
            (llvm::Type*)builder.getInt32Ty();
        llvm::AllocaInst* alloca =
            builder.CreateAlloca(ty, nullptr, node.varName);
        llvm::Value* val = genExpr(*node.left);
        builder.CreateStore(val, alloca);
        vars[node.varName] = alloca;
    }
    else if (node.type == NODE_FILE_WRITE) {
        llvm::FunctionType* fputsType =
            llvm::FunctionType::get(builder.getInt32Ty(),
                {builder.getPtrTy(), builder.getPtrTy()}, false);
        llvm::Function* fputsFunc =
            llvm::Function::Create(fputsType,
                llvm::Function::ExternalLinkage, "fputs", module);
        llvm::FunctionType* fcloseType =
            llvm::FunctionType::get(builder.getInt32Ty(),
                {builder.getPtrTy()}, false);
        llvm::Function* fcloseFunc =
            llvm::Function::Create(fcloseType,
                llvm::Function::ExternalLinkage, "fclose", module);
        llvm::AllocaInst* fileAlloca = vars[node.varName];
        if (fileAlloca) {
            llvm::Value* filePtr =
                builder.CreateLoad(builder.getPtrTy(), fileAlloca);
            llvm::Value* str = getStringPtr(node.left->value);
            builder.CreateCall(fputsFunc, {str, filePtr});
        }
    }
    else if (node.type == NODE_FILE_CLOSE) {
        llvm::FunctionType* fcloseType =
            llvm::FunctionType::get(builder.getInt32Ty(),
                {builder.getPtrTy()}, false);
        llvm::Function* fcloseFunc =
            module.getFunction("fclose");
        if (!fcloseFunc) {
            fcloseFunc = llvm::Function::Create(fcloseType,
                llvm::Function::ExternalLinkage, "fclose", module);
        }
        llvm::AllocaInst* fileAlloca = vars[node.varName];
        if (fileAlloca) {
            llvm::Value* filePtr =
                builder.CreateLoad(builder.getPtrTy(), fileAlloca);
            builder.CreateCall(fcloseFunc, {filePtr});
        }
    }
    else if (node.type == NODE_DEREF_ASSIGN) {
        llvm::AllocaInst* alloca = vars[node.varName];
        if (alloca) {
            llvm::Value* ptr = builder.CreateLoad(builder.getPtrTy(), alloca);
            builder.CreateStore(genExpr(*node.left), ptr);
        }
    }
    else if (node.type == NODE_ASSIGN) {
        llvm::AllocaInst* alloca = vars[node.varName];
        if (alloca) builder.CreateStore(genExpr(*node.left), alloca);
    }
    else if (node.type == NODE_RETURN) {
        builder.CreateRet(genExpr(*node.left));
    }
    else if (node.type == NODE_FUNC_DEF) {
        std::vector<llvm::Type*> paramTypes(
            node.params.size(), builder.getInt32Ty());
        llvm::FunctionType* ft =
            llvm::FunctionType::get(builder.getInt32Ty(), paramTypes, false);
        llvm::Function* func =
            llvm::Function::Create(ft,
                llvm::Function::ExternalLinkage, node.varName, module);
        llvm::BasicBlock* block =
            llvm::BasicBlock::Create(context, "entry", func);
        llvm::BasicBlock* savedBlock = builder.GetInsertBlock();
        auto savedVars = vars;
        builder.SetInsertPoint(block);
        int idx = 0;
        for (auto& arg : func->args()) {
            std::string pname = node.params[idx++];
            pname = pname.substr(pname.find(":") + 1);
            arg.setName(pname);
            llvm::AllocaInst* alloca =
                builder.CreateAlloca(builder.getInt32Ty(), nullptr, pname);
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
        funcs[node.varName] = func;
    }
    else if (node.type == NODE_FUNC_CALL) {
        if (funcs.count(node.varName)) {
            std::vector<llvm::Value*> argVals;
            for (const Node& a : node.args)
                argVals.push_back(genExpr(a));
            builder.CreateCall(funcs[node.varName], argVals);
        }
    }
    else if (node.type == NODE_ARRAY_DECL) {
        int idx = 0;
        for (const Node& elem : node.args) {
            std::string name = node.varName + "_" + std::to_string(idx++);
            llvm::AllocaInst* alloca =
                builder.CreateAlloca(builder.getInt32Ty(), nullptr, name);
            builder.CreateStore(genExpr(elem), alloca);
            vars[name] = alloca;
        }
    }
    else if (node.type == NODE_STRUCT_DEF) {
        for (const Node& field : node.children) {
            std::string fullName = node.varName + "_" + field.varName;
            llvm::AllocaInst* alloca =
                builder.CreateAlloca(builder.getInt32Ty(), nullptr, fullName);
            llvm::Value* val = genExpr(*field.left);
            builder.CreateStore(val, alloca);
            vars[fullName] = alloca;
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
        builder.CreateBr(mergeBlock);
        builder.SetInsertPoint(elseBlock);
        for (const Node& n : node.elseChildren) genNode(n);
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
        builder.CreateBr(loopBlock);
        builder.SetInsertPoint(loopBlock);
        llvm::Value* cur =
            builder.CreateLoad(builder.getInt32Ty(), i);
        llvm::Value* cond = builder.CreateICmpSLT(cur, count);
        llvm::BasicBlock* bodyBlock =
            llvm::BasicBlock::Create(context, "body", func);
        builder.CreateCondBr(cond, bodyBlock, afterBlock);
        builder.SetInsertPoint(bodyBlock);
        for (const Node& n : node.children) genNode(n);
        llvm::Value* next =
            builder.CreateAdd(cur, builder.getInt32(1));
        builder.CreateStore(next, i);
        builder.CreateBr(loopBlock);
        builder.SetInsertPoint(afterBlock);
    }
    else if (node.type == NODE_LOOP_W) {
        llvm::Function* func = builder.GetInsertBlock()->getParent();
        llvm::BasicBlock* condBlock =
            llvm::BasicBlock::Create(context, "cond", func);
        llvm::BasicBlock* bodyBlock =
            llvm::BasicBlock::Create(context, "body", func);
        llvm::BasicBlock* afterBlock =
            llvm::BasicBlock::Create(context, "after", func);
        builder.CreateBr(condBlock);
        builder.SetInsertPoint(condBlock);
        llvm::Value* cond = genExpr(*node.left);
        if (cond->getType() == builder.getInt32Ty())
            cond = builder.CreateICmpNE(cond, builder.getInt32(0));
        builder.CreateCondBr(cond, bodyBlock, afterBlock);
        builder.SetInsertPoint(bodyBlock);
        for (const Node& n : node.children) genNode(n);
        builder.CreateBr(condBlock);
        builder.SetInsertPoint(afterBlock);
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

    // сначала все функции
    for (const Node& node : program.children)
        if (node.type == NODE_FUNC_DEF) genNode(node);

    // потом main
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
