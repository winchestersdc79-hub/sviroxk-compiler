#include "codegen.h"
#include <llvm/Support/raw_ostream.h>
#include <iostream>

llvm::Value* CodeGen::genExpr(const Node& node) {
    if (node.type == NODE_NUMBER)
        return builder.getInt32(std::stoi(node.value));
    if (node.type == NODE_BINOP) {
        llvm::Value* L = genExpr(*node.left);
        llvm::Value* R = genExpr(*node.right);
        if (node.op == "+") return builder.CreateAdd(L, R);
        if (node.op == "-") return builder.CreateSub(L, R);
        if (node.op == "*") return builder.CreateMul(L, R);
        if (node.op == "/") return builder.CreateSDiv(L, R);
    }
    return builder.getInt32(0);
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
    llvm::Function* putsFunc =
        llvm::Function::Create(putsType,
            llvm::Function::ExternalLinkage,
            "puts", module);

    for (const Node& node : program.children) {
        if (node.type == NODE_SLOV) {
            llvm::Value* str =
                builder.CreateGlobalStringPtr(node.value);
            builder.CreateCall(putsFunc, {str});
        }
        else if (node.type == NODE_VAR_DECL) {
            llvm::AllocaInst* alloca =
                builder.CreateAlloca(builder.getInt32Ty(),
                    nullptr, node.varName);
            llvm::Value* val = genExpr(*node.left);
            builder.CreateStore(val, alloca);
        }
    }

    builder.CreateRet(builder.getInt32(0));
}

void CodeGen::print() {
    module.print(llvm::outs(), nullptr);
}
