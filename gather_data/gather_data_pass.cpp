// /* 
// ===-- Heuristic Based Static Branch Prediction ------------------------===
// Gather Data Pass Information
// 1. Given a benchmark, iterate over all branches. For each branch, record the true branch probability 
//    (probability that a branch is taken, between 0 and 1) along with that branch id
// 2. Additionally for each branch, apply each heuristic on the branch and record a 1 if it is true, and a 0 if it is false

// Output Formats:
// true_probability.csv
//   - Rows: One for each branch
//   - Columns: branch_id, branch_probability (0 to 1)
// heuristics.csv
//   - Rows: One for each branch
//   - Columns: branch_id, loop, pointer, opcode, guard, loop_header, call, store, return
//    -1: N/A
//    0: taken
//    1: not taken

// Ball and Larus Heuristics:
//   - Loop: If the branch is a loop (backedge to loop header), then predict taken.
//   - Pointer: If comparing a pointer against null or comparing two pointers with each other, then predict not taken
//   - Opcode: If comparing an int variable < 0, <= 0, or = any constant, then predict not taken
//   - Guard: If operand is a register that is used before define in the successor, and the successor does not post-dominate the current block, predict branch taken to the successor
//   - Loop Header: If successor is a loop header or loop pre-header and does not post dominate, then the branch will be taken
//   - Call: If successor contains a function call and does not post dominate, predict branch not taken
//   - Store: If succeessor contains a store and does not post dominate, predict branch not taken
//   - Return: If successor contains a return, predict branch not taken

// */

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

namespace {
struct CalcHeuristics : public FunctionPass {
  static char ID;
  CalcHeuristics() : FunctionPass(ID) {}

  void getAnalysisUsage(AnalysisUsage &AU) const {
    AU.addRequired<BlockFrequencyInfoWrapperPass>(); // Analysis pass to load block execution count
    AU.addRequired<BranchProbabilityInfoWrapperPass>(); // Analysis pass to load branch probability
  }

  bool runOnFunction(Function &F) override {
    errs() << "Pass is running finally!!!\n";

    BranchProbabilityInfo &bpi = getAnalysis<BranchProbabilityInfoWrapperPass>().getBPI(); 
    const unsigned int taken_idx = 0;
    const unsigned int not_taken_idx = 1;

    FILE *heuristic_data;
    FILE *true_prob_data;
    heuristic_data = fopen("heuristics.csv", "w+");
    true_prob_data = fopen("true_probability.csv", "w+");

    std::set<int> branch_cmds = {llvm::Instruction::Br, llvm::Instruction::Switch, llvm::Instruction::IndirectBr};
    
    for(Function::iterator bb = F.begin(), e = F.end(); bb != e; ++bb) {
      BasicBlock *curr_bb = dyn_cast<BasicBlock>(bb);
      Instruction *instr = curr_bb->getTerminator();
      BranchInst *branch_instr = dyn_cast<BranchInst>(instr);
      if(branch_cmds.find(instr->getOpcode()) != branch_cmds.end() && branch_instr->getNumSuccessors() == 2){
        
        // Branch's BB heuristics 
        int loop = h_loop(curr_bb);
        int pointer = h_pointer(curr_bb);
        int opcode = h_opcode(curr_bb);
        int guard = h_guard(curr_bb);

        // Successor's BB heuristics
        BasicBlock *taken = branch_instr->getSuccessor(taken_idx);
        BasicBlock *not_taken = branch_instr->getSuccessor(not_taken_idx);

        int loopheader = h_loopheader(taken, not_taken);
        int call = h_call(taken, not_taken);
        int store = h_store(taken, not_taken);
        int ret = h_return(taken, not_taken);

        // Write to csv
        // True edge probabilities
        BranchProbability taken_br_prob = bpi.getEdgeProbability(curr_bb, taken_idx);
        double taken_prob = (double) taken_br_prob.getNumerator() / taken_br_prob.getDenominator();
        double not_taken_prob = 1 - taken_prob;
        fprintf(true_prob_data, "%f, %f\n", taken_prob, not_taken_prob);

        // Heuristics
        fprintf(heuristic_data, "%d, %d, %d, %d, %d, %d, %d, %d\n", loop, pointer, opcode, guard, loopheader, call, store, ret);
      }

    }

    return false;
  }

  int h_loop(BasicBlock *branch_bb){
    return -1;
  }

  int h_pointer(BasicBlock *branch_bb){
    return -1;
  }

  int h_opcode(BasicBlock *branch_bb){
    return -1;
  }

  int h_guard(BasicBlock *branch_bb){
    return -1;
  }

  int h_loopheader(BasicBlock *taken_successor_bb, BasicBlock *not_taken_successor_bb){
    return -1;
  }

  int h_call(BasicBlock *taken_successor_bb, BasicBlock *not_taken_successor_bb){
    return -1;
  }

  int h_store(BasicBlock *taken_successor_bb, BasicBlock *not_taken_successor_bb){
    return -1;
  }

  int h_return(BasicBlock *taken_successor_bb, BasicBlock *not_taken_successor_bb){
    return -1;
  }
};
};

char CalcHeuristics::ID = 0;
static RegisterPass<CalcHeuristics> X("CalcHeuristics", "Calculates heuristic applicability on branches.",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);