#include "codegen.h"
#include <llvm/Support/raw_ostream.h>
#include <iostream>

void CodeGen::generate(const Node& node) {
    llvm::FunctionType* funcType =
        llvm::FunctionType::get(builder.getInt32Ty(), false);
    llvm::Function* mainFunc =
        llvm::Function::Create(funcType,
            llvm::Function::ExternalLinkage,
            "main", module);
    llvm::BasicBlock* block =
        llvm::BasicBlock::Create(context, "entry", mainFunc);
    builder.SetInsertPoint(block);

    if (node.type == NODE_SLOV) {
        llvm::FunctionType* putsType =
            llvm::FunctionType::get(builder.getInt32Ty(),
                {builder.getPtrTy()}, false);
        llvm::Function* putsFunc =
            llvm::Function::Create(putsType,
                llvm::Function::ExternalLinkage,
                "puts", module);
        llvm::Value* str =
            builder.CreateGlobalStringPtr(node.value);
        builder.CreateCall(putsFunc, {str});
    }
    else if (node.type == NODE_VAR_DECL) {
        llvm::AllocaInst* alloca =
            builder.CreateAlloca(builder.getInt32Ty(),
                nullptr, node.varName);
        llvm::Value* val = builder.getInt32(0);
        if (node.left) {
            if (node.left->type == NODE_NUMBER) {
                val = builder.getInt32(std::stoi(node.left->value));
            }
            else if (node.left->type == NODE_BINOP) {
                llvm::Value* L = builder.getInt32(std::stoi(node.left->left->value));
                llvm::Value* R = builder.getInt32(std::stoi(node.left->right->value));
                if (node.left->op == "+") val = builder.CreateAdd(L, R, "add");
                else if (node.left->op == "-") val = builder.CreateSub(L, R, "sub");
                else if (node.left->op == "*") val = builder.CreateMul(L, R, "mul");
                else if (node.left->op == "/") val = builder.CreateSDiv(L, R, "div");
            }
        }
        builder.CreateStore(val, alloca);
    }

    builder.CreateRet(builder.getInt32(0));
}

void CodeGen::print() {
    module.print(llvm::outs(), nullptr);
}
