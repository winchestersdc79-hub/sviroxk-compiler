#include "codegen.h"
#include <llvm/Support/raw_ostream.h>
#include <iostream>
#include <string>

llvm::Value* CodeGen::getStringPtr(const std::string& str) {
    llvm::GlobalVariable* gv = builder.CreateGlobalString(str);
    return builder.CreateInBoundsGEP(
        gv->getValueType(), gv,
        {builder.getInt32(0), builder.getInt32(0)}
    );
}

llvm::Value* CodeGen::genExpr(const Node& node) {
    if (node.type == NODE_NUMBER)
        return builder.getInt32(std::stoi(node.value));
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
        if (node.left && node.left->type == NODE_IDENTIFIER && vars.count(node.left->value)) {
            llvm::Value* fmt = getStringPtr("%d\n");
            llvm::Value* val = genExpr(*node.left);
            builder.CreateCall(printfFunc, {fmt, val});
        } else {
            llvm::Value* str = getStringPtr(node.value);
            builder.CreateCall(putsFunc, {str});
        }
    }
    else if (node.type == NODE_VAR_DECL) {
        llvm::AllocaInst* alloca = builder.CreateAlloca(builder.getInt32Ty(), nullptr, node.varName);
        llvm::Value* val = genExpr(*node.left);
        builder.CreateStore(val, alloca);
        vars[node.varName] = alloca;
    }
    else if (node.type == NODE_STRUCT_DEF) {
        for (const Node& field : node.children) {
            std::string fullName = node.varName + "_" + field.varName;
            llvm::AllocaInst* alloca = builder.CreateAlloca(builder.getInt32Ty(), nullptr, fullName);
            llvm::Value* val = genExpr(*field.left);
            builder.CreateStore(val, alloca);
            vars[fullName] = alloca;
        }
    }
    else if (node.type == NODE_FUNC_DEF) {
        llvm::FunctionType* ft = llvm::FunctionType::get(builder.getInt32Ty(), false);
        llvm::Function* func = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, node.varName, module);
        llvm::BasicBlock* block = llvm::BasicBlock::Create(context, "entry", func);
        llvm::BasicBlock* savedBlock = builder.GetInsertBlock();
        builder.SetInsertPoint(block);
        for (const Node& n : node.children) genNode(n);
        builder.CreateRet(builder.getInt32(0));
        builder.SetInsertPoint(savedBlock);
        funcs[node.varName] = func;
    }
    else if (node.type == NODE_FUNC_CALL) {
        if (funcs.count(node.varName)) {
            builder.CreateCall(funcs[node.varName], {});
        }
    }
    else if (node.type == NODE_ASSIGN) {
        llvm::AllocaInst* alloca = vars[node.varName];
        if (alloca) {
            llvm::Value* val = genExpr(*node.left);
            builder.CreateStore(val, alloca);
        }
    }
    else if (node.type == NODE_LOOP_N || node.type == NODE_LOOP_W || node.type == NODE_IF) {
        // Пропускаем пока (чтобы не было ошибок компиляции)
        // Можно добавить позже
    }
}

void CodeGen::generate(const Node& program) {
    llvm::FunctionType* funcType = llvm::FunctionType::get(builder.getInt32Ty(), false);
    llvm::Function* mainFunc = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, "main", module);
    llvm::BasicBlock* block = llvm::BasicBlock::Create(context, "entry", mainFunc);
    builder.SetInsertPoint(block);

    llvm::FunctionType* putsType = llvm::FunctionType::get(builder.getInt32Ty(), {builder.getPtrTy()}, false);
    putsFunc = llvm::Function::Create(putsType, llvm::Function::ExternalLinkage, "puts", module);

    llvm::FunctionType* printfType = llvm::FunctionType::get(builder.getInt32Ty(), {builder.getPtrTy()}, true);
    printfFunc = llvm::Function::Create(printfType, llvm::Function::ExternalLinkage, "printf", module);

    for (const Node& node : program.children) genNode(node);

    builder.CreateRet(builder.getInt32(0));
}

void CodeGen::print() {
    module.print(llvm::outs(), nullptr);
}
