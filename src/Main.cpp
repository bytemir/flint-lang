#include "llvm/Support/TargetSelect.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include <iostream>

int main() {
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();

    auto Context = std::make_unique<llvm::LLVMContext>();
    auto Module = std::make_unique<llvm::Module>("FlintTestModule", *Context);

    std::cout << "Successfully initialized LLVM Context and Native Targets." << std::endl;
    std::cout << "Module ID: " << Module->getModuleIdentifier() << std::endl;

    return 0;
}