#ifndef CODEGEN_H
#define CODEGEN_H

#include "parser.h"
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <map>

class CodeGen {
public:
    llvm::LLVMContext context;
    llvm::IRBuilder<> builder;
    llvm::Module module;
    std::map<std::string, llvm::AllocaInst*> vars;
    llvm::Function* putsFunc = nullptr;
    llvm::Function* printfFunc = nullptr;

    CodeGen() : builder(context), module("sviroxk", context) {}

    llvm::Value* genExpr(const Node& node);
    void genNode(const Node& node);
    void generate(const Node& program);
    void print();
};

#endif
