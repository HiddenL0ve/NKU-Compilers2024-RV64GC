#include "../../include/Instruction.h"
#include "../../include/ir.h"
#include <assert.h>
extern std::map<FuncDefInstruction, int> max_label_map;
extern std::map<FuncDefInstruction, int> max_reg_map;

void LLVMIR::EraseUnreachInsAndBlocks() {
    for (auto &[FunIns, blocks] : function_block_map) {
        std::map<int, int> reachmap;
        std::stack<int> s;
        s.push(0);
        // 移除返回和跳转后续指令
        while (!s.empty()) {
            int cur = s.top();
            s.pop();
            reachmap[cur] = 1;
            auto &block = blocks[cur];
            auto &blocklist = block->Instruction_list;
            int pos = blocklist.size();
            for (int i = 0; i < blocklist.size(); i++) {
                if (blocklist[i]->GetOpcode() == BasicInstruction::LLVMIROpcode::RET) {
                    pos = i;
                    break;
                }
            }
            while (blocklist.size() > pos + 1) {
                blocklist.pop_back();
            }

            // 跳转指令
            Instruction blocklast = blocklist[blocklist.size() - 1];
            if (blocklast->GetOpcode() == BasicInstruction::LLVMIROpcode::BR_UNCOND) {
                BrUncondInstruction *br = (BrUncondInstruction *)blocklast;
                int target_num = ((LabelOperand *)br->GetDestLabel())->GetLabelNo();
                if (reachmap[target_num] == 0) {
                    reachmap[target_num] = 1;
                    s.push(target_num);
                }
            }
            if (blocklast->GetOpcode() == BasicInstruction::LLVMIROpcode::BR_COND) {
                BrCondInstruction *br = (BrCondInstruction *)blocklast;
                int target_true_num = ((LabelOperand *)br->GetTrueLabel())->GetLabelNo();
                int target_false_num = ((LabelOperand *)br->GetFalseLabel())->GetLabelNo();
                if (reachmap[target_false_num] == 0) {
                    reachmap[target_false_num] = 1;
                    s.push(target_false_num);
                }
                if (reachmap[target_true_num] == 0) {
                    reachmap[target_true_num] = 1;
                    s.push(target_true_num);
                }
            }
        }
        std::set<int> badblocks;

        for (auto &[i, b] : blocks) {
            if (reachmap[i] == 0) {
                badblocks.insert(i);
            }
        }
        for (int bad_id : badblocks) {
            blocks.erase(bad_id);
        }

        // 更新PHI指令
        for (auto &[i, b] : blocks) {
            for (auto Ins : b->Instruction_list) {
                if (Ins->GetOpcode() == BasicInstruction::LLVMIROpcode::PHI) {
                    auto P = (PhiInstruction *)Ins;
                    for (auto badb : badblocks) {
                        P->ErasePhi(badb);
                    }
                } else {
                    break;
                }
            }
        }
    }
}

void LLVMIR::CFGInit() {
     EraseUnreachInsAndBlocks();
    for (auto &[defI, bb_map] : function_block_map) {
        CFG *cfg = new CFG();
        cfg->block_map = &bb_map;
        cfg->function_def = defI;
        cfg->max_label = max_label_map[defI];
        //printf("%d",cfg->max_label);
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
        auto list = b->Instruction_list;
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
    //TODO("BuildCFG");
     }

std::vector<LLVMBlock> CFG::GetPredecessor(LLVMBlock B) { return invG[B->block_id]; }

std::vector<LLVMBlock> CFG::GetPredecessor(int bbid) { return invG[bbid]; }

std::vector<LLVMBlock> CFG::GetSuccessor(LLVMBlock B) { return G[B->block_id]; }

std::vector<LLVMBlock> CFG::GetSuccessor(int bbid) { return G[bbid]; }