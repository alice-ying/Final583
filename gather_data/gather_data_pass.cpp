/* 
===-- Heuristic Based Static Branch Prediction ------------------------===
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

Ball and Larus Heuristics:
  - Loop: If the branch is a loop (backedge to loop header), then predict taken.
  - Pointer: If comparing a pointer against null or comparing two pointers with each other, then predict not taken
  - Opcode: If comparing an int variable < 0, <= 0, or = any constant, then predict not taken
  - Guard: If operand is a register that is used before define in the successor, and the successor does not post-dominate the current block, predict branch taken to the successor
  - Loop Header: If successor is a loop header or loop pre-header and does not post dominate, then the branch will be taken
  - Call: If successor contains a function call and does not post dominate, predict branch not taken
  - Store: If succeessor contains a store and does not post dominate, predict branch not taken
  - Return: If successor contains a return, predict branch not taken

*/
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/LoopIterator.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Scalar/LoopPassManager.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Transforms/Utils/LoopUtils.h"
#include "llvm/Analysis/BranchProbabilityInfo.h"
#include "llvm/Analysis/BlockFrequencyInfo.h"
#include <vector>
#include <set>
#include <iostream>
#include <fstream>
#include <unordered_map>

using namespace llvm;

namespace Heuristics{
struct HeuristicsPass : public FunctionPass {
  static char ID;
  HeuristicsPass() : FunctionPass(ID) {}  

  bool runOnFunction(Function &F) {
    errs() << "Pass is running finally!!!\n";
    std::ofstream heuristics_ofs;
    std::ofstream true_probability_ofs;
    heuristics_ofs.open("heuristics.csv");
    true_probability_ofs.open("true_probability.csv");
    std::set<int> branch_cmds = {llvm::Instruction::Br, llvm::Instruction::Switch, llvm::Instruction::IndirectBr};

    for(Function::iterator bb = F.begin(), e = F.end(); bb != e; ++bb) {
      BasicBlock *curr_bb = bb;
      Instruction *instr = curr_bb->getTerminator();
      BranchInst *branch_instr = dyn_cast<BranchInst>(instr);
      if(branch_cmds.find(i->getOpcode()) != branch_cmds.end()){
        errs() << "Found a branch instruction!\n";
        BasicBlock *taken = branch_instr->getSuccessor(0);
        BasicBlock *not_taken = branch_instr->getSuccessor(1);



      }

    }
    return false;
  }

  void h_loop(ofstream& heuristics_ofs, ofstream& true_probability_ofs){
    
  }

  void h_pointer(ofstream& heuristics_ofs, ofstream& true_probability_ofs){

  }

  void h_opcode(ofstream& heuristics_ofs, ofstream& true_probability_ofs){

  }

  void h_guard(ofstream& heuristics_ofs, ofstream& true_probability_ofs){

  }

  void h_loopheader(ofstream& heuristics_ofs, ofstream& true_probability_ofs){

  }

  void h_call(ofstream& heuristics_ofs, ofstream& true_probability_ofs){

  }

  void h_store(ofstream& heuristics_ofs, ofstream& true_probability_ofs){

  }

  void h_return(ofstream& heuristics_ofs, ofstream& true_probability_ofs){

  }


  void getAnalysisUsage(AnalysisUsage &AU) const {
    AU.addRequired<BlockFrequencyInfoWrapperPass>(); // Analysis pass to load block execution count
    AU.addRequired<BranchProbabilityInfoWrapperPass>(); // Analysis pass to load branch probability
  }


};
}

char Heuristics::HeuristicsPass::ID = 0;
static RegisterPass<Heuristics::HeuristicsPass> X("Heuristics Pass", "Checking branches on heuristics", false, false);