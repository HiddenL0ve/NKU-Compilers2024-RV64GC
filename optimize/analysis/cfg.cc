#include "../../include/Instruction.h"
#include "../../include/ir.h"
#include <assert.h>
extern std::map<FuncDefInstruction, int> max_label_map;
extern std::map<FuncDefInstruction, int> max_reg_map;

void LLVMIR::CFGInit() {

    for (auto &[defI, bb_map] : function_block_map) {
        CFG *cfg = new CFG();
        cfg->block_map = &bb_map;
        cfg->function_def = defI;
        cfg->max_label = max_label_map[defI];
        // printf("%d",cfg->max_label);
        cfg->max_reg = max_reg_map[defI];
        cfg->BuildCFG();
        // TODO("init your members in class CFG if you need");
        llvm_cfg[defI] = cfg;
    }
}

void LLVMIR::BuildCFG() {
    for (auto [defI, cfg] : llvm_cfg) {
        cfg->BuildCFG();
    }
}

void CFG::BuildCFG() {
    G.clear();
    G.resize(max_label + 1);

    invG.clear();
    invG.resize(max_label + 1);

    for (auto [id, b] : *block_map) {
        auto &list = b->Instruction_list;
        int pos=list.size();
        for (int i = 0; i < list.size(); i++) {
                if (list[i]->GetOpcode() == BasicInstruction::LLVMIROpcode::RET) {
                    pos = i;
                    break;
                }
         }
        while (list.size() > pos + 1) {
                list.pop_back();
        }
        Instruction ins = list[list.size() - 1];
        int opcode = ins->GetOpcode();
        if (opcode == BasicInstruction::LLVMIROpcode::BR_UNCOND) {
            BrUncondInstruction *br = (BrUncondInstruction *)ins;
            int target_num = ((LabelOperand *)br->GetDestLabel())->GetLabelNo();
            G[id].push_back((*block_map)[target_num]);
            invG[target_num].push_back(b);
        } else if (opcode == BasicInstruction::LLVMIROpcode::BR_COND) {
            BrCondInstruction *br = (BrCondInstruction *)ins;
            int target_true_num = ((LabelOperand *)br->GetTrueLabel())->GetLabelNo();
            int target_false_num = ((LabelOperand *)br->GetFalseLabel())->GetLabelNo();
            G[id].push_back((*block_map)[target_true_num]);
            G[id].push_back((*block_map)[target_false_num]);
            invG[target_true_num].push_back(b);
            invG[target_false_num].push_back(b);
        } else if (opcode == BasicInstruction::LLVMIROpcode::RET) {
            ret_block = b;
        }
    }
    // TODO("BuildCFG");
}

std::vector<LLVMBlock> CFG::GetPredecessor(LLVMBlock B) { return invG[B->block_id]; }

std::vector<LLVMBlock> CFG::GetPredecessor(int bbid) { return invG[bbid]; }

std::vector<LLVMBlock> CFG::GetSuccessor(LLVMBlock B) { return G[B->block_id]; }

std::vector<LLVMBlock> CFG::GetSuccessor(int bbid) { return G[bbid]; }