#include "llvm/Pass.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
using namespace llvm;

namespace {
    struct ProfilerPass : public FunctionPass {
        static char ID;
        ProfilerPass() : FunctionPass(ID) {}

        bool isFuncLogger(StringRef name) {
            return name == "binOptLogger" || name == "callLogger" ||
                   name == "funcStartLogger" || name == "funcEndLogger";
        }

        virtual bool runOnFunction(Function &F) {
            if (isFuncLogger(F.getName())) {
                return false;
            }
            // Prepare builder for IR modification
            LLVMContext &Ctx = F.getContext();
            IRBuilder<> builder(Ctx);
            Type *retType = Type::getVoidTy(Ctx);

            // Prepare funcStartLogger function
            ArrayRef<Type *> funcStartParamTypes = {
                    builder.getInt8Ty()->getPointerTo()};
            FunctionType *funcStartLogFuncType =
                    FunctionType::get(retType, funcStartParamTypes, false);
            FunctionCallee funcStartLogFunc = F.getParent()->getOrInsertFunction(
                    "funcStartLogger", funcStartLogFuncType);

            // Insert a call of funcStartLogger function in the function begin
            BasicBlock &entryBB = F.getEntryBlock();
            builder.SetInsertPoint(&entryBB.front());
            Value *funcName = builder.CreateGlobalStringPtr(F.getName());
            Value *args[] = {funcName};
            builder.CreateCall(funcStartLogFunc, args);

            return false;
        }
    };
}

char ProfilerPass::ID = 0;

// Automatically enable the pass.
// http://adriansampson.net/blog/clangpass.html
static void registerMyPass(const PassManagerBuilder &,
                           legacy::PassManagerBase &PM) {
    PM.add(new ProfilerPass());
}
static RegisterStandardPasses
RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible,
               registerMyPass);
