#ifndef CODEGEN_H
#define CODEGEN_H

#include "parser.h"
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

class CodeGen {
public:
    llvm::LLVMContext context;
    llvm::IRBuilder<> builder;
    llvm::Module module;

    CodeGen() : builder(context), module("sviroxk", context) {}

    void generate(const Node& node);
    void print();
};

#endif
