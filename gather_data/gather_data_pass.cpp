#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {
struct Hello : public FunctionPass {
  static char ID;
  Hello() : FunctionPass(ID) {}

  bool runOnFunction(Function &F) override {
    errs() << "Hello: ";
    errs().write_escaped(F.getName()) << '\n';
    return false;
  }
}; // end of struct Hello
}  // end of anonymous namespace

char Hello::ID = 0;
static RegisterPass<Hello> X("hello", "Hello World Pass",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);


/* 
Gather Data Pass Information
1. Given a benchmark, iterate over all branches. For each branch, record the true branch probability 
   (probability that a branch is taken, between 0 and 1) along with that branch id
2. Additionally for each branch, apply each heuristic on the branch and record a 1 if it is true, and a 0 if it is false

Output Formats:
true_probability.csv
  - Rows: One for each branch
  - Columns: branch_id, branch_probability (0 to 1)
heuristics.csv
  - Rows: One for each branch
  - Columns: branch_id, loop, pointer, opcode, guard, loop_header, call, store, return
*/