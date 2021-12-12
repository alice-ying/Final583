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
//    0.5: N/A
//    1: taken
//    0: not taken

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
#include "llvm/Analysis/PostDominators.h"
#include "llvm/Analysis/LoopInfo.h"
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
    AU.addRequired<PostDominatorTreeWrapperPass>();
    AU.addRequired<LoopInfoWrapperPass>();
    AU.setPreservesAll();
  }

  bool runOnFunction(Function &F) override {
    BranchProbabilityInfo &bpi = getAnalysis<BranchProbabilityInfoWrapperPass>().getBPI(); 
    const unsigned int taken_idx = 0;
    const unsigned int not_taken_idx = 1;

    // Open CSV files
    FILE *heuristic_data;
    FILE *true_prob_data;
    heuristic_data = fopen("heuristics.csv", "a+");
    true_prob_data = fopen("true_probability.csv", "a+");

    // Run through all branches
    int num_branches = 0;
    for(Function::iterator bb = F.begin(), e = F.end(); bb != e; ++bb) {
      BasicBlock *curr_bb = dyn_cast<BasicBlock>(bb);
      Instruction *instr = curr_bb->getTerminator();

      if(isa<BranchInst>(instr)){
        BranchInst *branch_instr = dyn_cast<BranchInst>(instr);
        BasicBlock *taken = branch_instr->getSuccessor(taken_idx);
        BasicBlock *not_taken = nullptr;

        if(branch_instr->getNumSuccessors() == 2){
          not_taken = branch_instr->getSuccessor(not_taken_idx);
        }

        // Write to csv
        // True edge probabilities
        BranchProbability taken_br_prob = bpi.getEdgeProbability(curr_bb, taken_idx);
        double taken_prob = (double) taken_br_prob.getNumerator() / taken_br_prob.getDenominator();
        double not_taken_prob = 1 - taken_prob;
        fprintf(true_prob_data, "%f, %f\n", taken_prob, not_taken_prob);

        // Heuristics
        double loop_val = h_loop(curr_bb, taken);
        double pointer_val = h_pointer(curr_bb);
        double opcode_val = h_opcode(curr_bb);
        double guard_val = h_guard(curr_bb, taken, not_taken);
        double loopheader_val = h_loopheader(curr_bb, taken, not_taken);
        double call_val = h_call(curr_bb, taken, not_taken);
        double store_val = h_store(curr_bb, taken, not_taken);
        double return_val = h_return(taken, not_taken);
        fprintf(heuristic_data, "%.1f, %.1f, %.1f, %.1f, %.1f, %.1f, %.1f, %.1f\n",
          loop_val,
          pointer_val,
          opcode_val,
          guard_val,
          loopheader_val,
          call_val,
          store_val,
          return_val
        );
      }
    }

    return false;
  }

  // Loop: If the branch is a loop (backedge to loop header), then predict taken.
  double h_loop(BasicBlock *branch_bb, BasicBlock *taken_successor_bb){
    LoopInfo *LI = &getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
    Loop* L = LI->getLoopFor(branch_bb);
    if(L != nullptr){
      BasicBlock* header = L->getHeader();
      BasicBlock* preheader = L->getLoopPreheader();
      if(header == taken_successor_bb || preheader == taken_successor_bb){
        return 1;
      }
    }
    return 0.5;
  }

  // Pointer: If comparing a pointer against null or comparing two pointers with each other, then predict not taken
  double h_pointer(BasicBlock *branch_bb){
    Instruction *instr = branch_bb->getTerminator();
    BranchInst *branch_instr = dyn_cast<BranchInst>(instr);

    if(branch_instr->isConditional()){
      Value *cond = branch_instr->getCondition();
      Instruction* i = dyn_cast<Instruction>(&*cond);

      Type *opr0_ty = i->getOperand(0)->getType();
      Type *opr1_ty = i->getOperand(1)->getType();

      if(opr0_ty->isPointerTy() && opr1_ty->isPointerTy()){
        return 0;
      }
    }
    return 0.5;
  }
 
  // Opcode: If comparing an int variable < 0, <= 0, or = any constant, then predict not taken
  double h_opcode(BasicBlock *branch_bb){
    Instruction *instr = branch_bb->getTerminator();
    BranchInst *branch_instr = dyn_cast<BranchInst>(instr);

    if(branch_instr->isConditional()){
      Value *cond = branch_instr->getCondition();
      Instruction *i = dyn_cast<Instruction>(&*cond);
      
      Value *opr0 = i->getOperand(0);
      Value *opr1 = i->getOperand(1);

      if(isa<CmpInst>(i) && opr0->getType()->isIntegerTy() && opr1->getType()->isIntegerTy()){
        CmpInst *cmpInst = dyn_cast<CmpInst>(&*i);
        CmpInst::Predicate pred = cmpInst->getPredicate();

        if(isa<Constant>(opr0) && !isa<Constant>(opr1)){
          int const_val = dyn_cast<llvm::ConstantInt>(opr0)->getSExtValue();
          if((const_val == 0 && (pred == CmpInst::Predicate::ICMP_SGT || pred == CmpInst::Predicate::ICMP_SGE))
            || (pred == CmpInst::Predicate::ICMP_EQ)){
            return 0;
          }
        }
        else if(!isa<Constant>(opr0) && isa<Constant>(opr1)){
          int const_val = dyn_cast<llvm::ConstantInt>(opr1)->getSExtValue();
          if((const_val == 0 && (pred == CmpInst::Predicate::ICMP_SLT || pred == CmpInst::Predicate::ICMP_SLE))
            || (pred == CmpInst::Predicate::ICMP_EQ)){
            return 0;
          }
        }
      }
    }
    return 0.5;
  }

  Value* check_load(Instruction *inst){
    if(inst != nullptr && inst->getOpcode() == llvm::Instruction::Load){
      return inst->getOperand(0);
    }
    return nullptr;
  }

  Value* check_store(Instruction *inst){
    if(inst != nullptr && inst->getOpcode() == llvm::Instruction::Store){
      return inst->getOperand(1);
    }
    return nullptr;
  }

  // Guard: If operand is a register that is used before define in the successor, and the successor does not post-dominate the current block, predict branch taken to the successor
  double h_guard(BasicBlock *branch_bb, BasicBlock *taken_successor_bb, BasicBlock *not_taken_successor_bb){
    auto PDT = &getAnalysis<PostDominatorTreeWrapperPass>().getPostDomTree();
    Instruction *instr = branch_bb->getTerminator();
    BranchInst *branch_instr = dyn_cast<BranchInst>(instr);

    if(branch_instr->isConditional()){
      Value *cond = branch_instr->getCondition();
      Instruction *i = dyn_cast<Instruction>(&*cond);
      Instruction *instopr1 = dyn_cast<Instruction>(i->getOperand(0));
      Instruction *instopr2 = dyn_cast<Instruction>(i->getOperand(1));
      Value* opr1 = check_load(instopr1);
      Value* opr2 = check_load(instopr2);

      if(!PDT->dominates(taken_successor_bb, branch_bb)){
        bool opr1_def_before_use = false;
        bool opr2_def_before_use = false;
        // Loop until both operands are defined before used, or one is used before defined. If the latter is the case, predict taken!
        for(Instruction &i : *taken_successor_bb) {
          // Only check opr1 if it is not already defined before used
          if(!opr1_def_before_use && opr1 != nullptr){
            // Check if opr1 is used as an operand
            if(check_load(&i) == opr1){
              return 1;
            }
            // Check if opr1 is the destination
            if(check_store(&i) == opr1){
              opr1_def_before_use = true;
            }
          }
          if(!opr2_def_before_use && opr2 != nullptr){
            // Check if opr2 is used as an operand
            if(check_load(&i) == opr2){
              return 1;
            }
            // Check if opr2 is the destination
            if(check_store(&i) == opr2){
              opr2_def_before_use = true;
            }
          }
          // If both have been defined before use, we can break
          if(opr1_def_before_use && opr2_def_before_use){
            break;
          }
        }
      }

      if(not_taken_successor_bb != nullptr && !PDT->dominates(not_taken_successor_bb, branch_bb)){
        bool opr1_def_before_use = false;
        bool opr2_def_before_use = false;
        // Loop until both operands are defined before used, or one is used before defined. If the latter is the case, predict taken!
        for(Instruction &i : *not_taken_successor_bb) {
          // Only check opr1 if it is not already defined before used
          if(!opr1_def_before_use){
            // Check if opr1 is used as an operand
            if(check_load(&i) == opr1){
              return 0;
            }
            // Check if opr1 is the destination
            if(check_store(&i) == opr1){
              opr1_def_before_use = true;
            }
          }
          if(!opr2_def_before_use && opr2 != nullptr){
            // Check if opr1 is used as an operand
            if(check_load(&i) == opr2){
              return 0;
            }
            // Check if opr2 is the destination
            if(check_store(&i) == opr2){
              opr2_def_before_use = true;
            }
          }
          // If both have been defined before use, we can break
          if(opr1_def_before_use && opr2_def_before_use){
            break;
          }
        }
      }
      return 0.5;
    }
    return 0.5;
  }

  // Loop Header: If successor is a loop header or loop pre-header and does not post dominate, then the branch will be taken
  double h_loopheader(BasicBlock *branch_bb, BasicBlock *taken_successor_bb, BasicBlock *not_taken_successor_bb){
    auto PDT = &getAnalysis<PostDominatorTreeWrapperPass>().getPostDomTree();
    LoopInfo *LI = &getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
    if(!PDT->dominates(taken_successor_bb, branch_bb) && (LI->isLoopHeader(taken_successor_bb) /* or loop pre-header */)){
      return 1;
    }

    if(not_taken_successor_bb != nullptr && !PDT->dominates(not_taken_successor_bb, branch_bb) && (LI->isLoopHeader(not_taken_successor_bb) /* or loop pre-header */)){
      return 0;
    }
    return 0.5;
  }

  // Call: If successor contains a function call and does not post dominate, predict branch not taken
  double h_call(BasicBlock *branch_bb, BasicBlock *taken_successor_bb, BasicBlock *not_taken_successor_bb){
    auto PDT = &getAnalysis<PostDominatorTreeWrapperPass>().getPostDomTree();
    if(!PDT->dominates(taken_successor_bb, branch_bb)){
      for(Instruction &i : *taken_successor_bb) {
        if(isa<CallInst>(&i)){
          return 0;
        }
      }
    }

    if(not_taken_successor_bb != nullptr && !PDT->dominates(not_taken_successor_bb, branch_bb)){
      for(Instruction &i : *not_taken_successor_bb) {
        int opc = i.getOpcode();
        if(isa<CallInst>(&i)){
          return 1;
        }
      }
    }
    return 0.5;
  }

  // Store: If succeessor contains a store and does not post dominate, predict branch not taken
  double h_store(BasicBlock *branch_bb, BasicBlock *taken_successor_bb, BasicBlock *not_taken_successor_bb){
    auto PDT = &getAnalysis<PostDominatorTreeWrapperPass>().getPostDomTree();
    if(!PDT->dominates(taken_successor_bb, branch_bb)){
      for(Instruction &i : *taken_successor_bb) {
        int opc = i.getOpcode();
        if(opc == llvm::Instruction::Store){
          return 0;
        }
      }
    }

    if(not_taken_successor_bb != nullptr && !PDT->dominates(not_taken_successor_bb, branch_bb)){
      for(Instruction &i : *not_taken_successor_bb) {
        int opc = i.getOpcode();
        if(opc == llvm::Instruction::Store){
          return 1;
        }
      }
    }
    return 0.5;
  }

  // Return: If successor contains a return, predict branch not taken
  double h_return(BasicBlock *taken_successor_bb, BasicBlock *not_taken_successor_bb){
    for(Instruction &i : *taken_successor_bb) {
        int opc = i.getOpcode();
        if(opc == llvm::Instruction::Ret){
            return 0;
        }
    }

    if(not_taken_successor_bb != nullptr){
      for(Instruction &i : *not_taken_successor_bb) {
        int opc = i.getOpcode();
        if(opc == llvm::Instruction::Ret){
          return 1;
        }
      }
    }
    return 0.5;
  }
};
};

char CalcHeuristics::ID = 0;
static RegisterPass<CalcHeuristics> X("CalcHeuristics", "Calculates heuristic applicability on branches.",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);