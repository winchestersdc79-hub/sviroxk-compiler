#include "codegen.h"
#include <llvm/Support/raw_ostream.h>
#include <iostream>

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
        if (node.left && node.left->type == NODE_IDENTIFIER
            && vars.count(node.left->value)) {
            llvm::Value* fmt = builder.CreateGlobalStringPtr("%d\n");
            llvm::Value* val = genExpr(*node.left);
            builder.CreateCall(printfFunc, {fmt, val});
        } else {
            llvm::Value* str = builder.CreateGlobalStringPtr(node.value);
            builder.CreateCall(putsFunc, {str});
        }
    }
    else if (node.type == NODE_VAR_DECL) {
        llvm::AllocaInst* alloca =
            builder.CreateAlloca(builder.getInt32Ty(),
                nullptr, node.varName);
        llvm::Value* val = genExpr(*node.left);
        builder.CreateStore(val, alloca);
        vars[node.varName] = alloca;
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
}

void CodeGen::generate(const Node& program) {
    llvm::FunctionType* funcType =
        llvm::FunctionType::get(builder.getInt32Ty(), false);
    llvm::Function* mainFunc =
        llvm::Function::Create(funcType,
            llvm::Function::ExternalLinkage,
            "main", module);
    llvm::BasicBlock* block =
        llvm::BasicBlock::Create(context, "entry", mainFunc);
    builder.SetInsertPoint(block);

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

    for (const Node& node : program.children) genNode(node);

    builder.CreateRet(builder.getInt32(0));
}

void CodeGen::print() {
    module.print(llvm::outs(), nullptr);
}
