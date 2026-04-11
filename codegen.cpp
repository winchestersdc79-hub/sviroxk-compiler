#include "codegen.h"
#include <llvm/Support/raw_ostream.h>
#include <stdexcept>

void CodeGen::generate(const Node& node) {
    if (node.type == NODE_VAR_DECL) {

        // Создаём функцию main
        llvm::FunctionType* funcType =
            llvm::FunctionType::get(builder.getInt32Ty(), false);
        llvm::Function* mainFunc =
            llvm::Function::Create(funcType,
                llvm::Function::ExternalLinkage,
                "main", module);

        // Создаём блок кода
        llvm::BasicBlock* block =
            llvm::BasicBlock::Create(context, "entry", mainFunc);
        builder.SetInsertPoint(block);

        // Генерируем переменную
        if (node.varType == "rox") {
            // целое число
            llvm::AllocaInst* alloca =
                builder.CreateAlloca(builder.getInt32Ty(),
                    nullptr, node.varName);
            llvm::Value* val =
                builder.getInt32(std::stoi(node.value));
            builder.CreateStore(val, alloca);
        }
        else if (node.varType == "dor") {
            // дробное число
            llvm::AllocaInst* alloca =
                builder.CreateAlloca(builder.getDoubleTy(),
                    nullptr, node.varName);
            llvm::Value* val =
                llvm::ConstantFP::get(builder.getDoubleTy(),
                    std::stod(node.value));
            builder.CreateStore(val, alloca);
        }

        // Возврат 0
        builder.CreateRet(builder.getInt32(0));
    }
}

void CodeGen::print() {
    module.print(llvm::outs(), nullptr);
}
