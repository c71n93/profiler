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
                   name == "funcStartLogger" || name == "funcEndLogger" ||
                   name == "operationLogger";
        }

        void insertFuncStartLogger(Function &F, IRBuilder<> &builder) {
            // Prepare funcStartLogger function
            ArrayRef<Type *> funcStartLoggerParamTypes = {
                    builder.getInt8Ty()->getPointerTo()
            };
            FunctionType *funcStartLoggerFuncType =
                    FunctionType::get(Type::getVoidTy(builder.getContext()), funcStartLoggerParamTypes, false);
            FunctionCallee funcStartLoggerFunc = F.getParent()->getOrInsertFunction(
                    "funcStartLogger", funcStartLoggerFuncType);

            // Insert a call of funcStartLogger function in the function begin
            BasicBlock &entryBB = F.getEntryBlock();
            builder.SetInsertPoint(&entryBB.front());
            Value *funcName = builder.CreateGlobalStringPtr(F.getName());
            Value *args[] = {funcName};
            builder.CreateCall(funcStartLoggerFunc, args);
        }

        void insertOperationsLogger(Function &F, IRBuilder<> &builder) {
            ArrayRef<Type *> operationParamLoggerTypes = {
                    builder.getInt8Ty()->getPointerTo()
            };
            FunctionType *operationParamLoggerType =
                    FunctionType::get(Type::getVoidTy(builder.getContext()), operationParamLoggerTypes, false);
            FunctionCallee operationParamLoggerFunc = F.getParent()->getOrInsertFunction(
                    "operationLogger", operationParamLoggerType);

            for (auto &B : F) {
                for (auto &I : B) {
                    if (auto *phi = dyn_cast<PHINode>(&I)) {
                        continue;
                    }
                    builder.SetInsertPoint(&I);
                    Value *instrName =
                            builder.CreateGlobalStringPtr(I.getOpcodeName());
                    Value *args[] = {instrName};
                    builder.CreateCall(operationParamLoggerFunc, args);
                }
            }
        }

        virtual bool runOnFunction(Function &F) {
            if (isFuncLogger(F.getName())) {
                return false;
            }
            // Prepare builder for IR modification
            LLVMContext &Ctx = F.getContext();
            IRBuilder<> builder(Ctx);

            insertFuncStartLogger(F, builder);
            insertOperationsLogger(F, builder);

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
RegisterMyPass(PassManagerBuilder::EP_OptimizerLast,
               registerMyPass);
