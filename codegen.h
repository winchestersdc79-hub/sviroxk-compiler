#ifndef CODEGEN_H
#define CODEGEN_H

#include "parser.h"
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <map>
#include <string>

class CodeGen {
public:
    llvm::LLVMContext context;
    llvm::IRBuilder<> builder;
    llvm::Module module;
    std::map<std::string, llvm::AllocaInst*> vars;
    std::map<std::string, llvm::AllocaInst*> arrays;
    std::map<std::string, unsigned> arrayLengths;
    std::map<std::string, llvm::Function*> funcs;
    std::map<std::string, std::vector<std::string>> funcParamTypes;
    struct StructField {
        std::string type;
        std::string name;
        Node defaultVal;
    };
    std::map<std::string, std::vector<StructField>> structDefs;
    llvm::BasicBlock* breakBlock = nullptr;
    llvm::BasicBlock* continueBlock = nullptr;
    llvm::Function* putsFunc = nullptr;
    llvm::Function* printfFunc = nullptr;

    CodeGen() : builder(context), module("sviroxk", context) {}

    llvm::Value* getStringPtr(const std::string& str);

    llvm::Type* svType(const std::string& name);
    llvm::Value* castTo(llvm::Value* val, llvm::Type* ty);
    llvm::Value* genExpr(const Node& node);
    void genNode(const Node& node);
    void generate(const Node& program);
    void print();
};

#endif
