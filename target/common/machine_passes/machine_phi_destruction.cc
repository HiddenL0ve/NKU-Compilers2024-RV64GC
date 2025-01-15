#include "machine_phi_destruction.h"
void MachinePhiDestruction::Execute() {
    for (auto func : unit->functions) {
        current_func = func;
        PhiDestructionInCurrentFunction();
    }
}

void MachinePhiDestruction::PhiDestructionInCurrentFunction() {
    // 第一次扫描处理Phi指令
    auto processPhiInstructions = [&](MachineBlock *blk) {
        for (auto it = blk->begin(); it != blk->end();) {
            auto ins = *it;
            if (ins->arch == MachineBaseInstruction::COMMENT) {
                ++it;
                continue;
            }

            if (ins->arch != MachineBaseInstruction::PHI)
                break;

            auto phi_ins = static_cast<MachinePhiInstruction *>(ins);
            it = blk->erase(it);

            for (const auto &[phi_blk_id, phi_op] : phi_ins->GetPhiList()) {
                auto pred_blk = current_func->getMachineCFG()->GetNodeByBlockId(phi_blk_id)->Mblock;

                if (phi_op->op_type == MachineBaseOperand::REG &&
                    static_cast<MachineRegister *>(phi_op)->reg == phi_ins->GetResult()) {
                    continue;
                }

                pred_blk->InsertParallelCopyList(phi_ins->GetResult(), phi_op);
            }
        }
    };

    auto insertIntermediateBlocks = [&](MachineBlock *blk) {
        for (auto pred : current_func->getMachineCFG()->GetPredecessorsByBlockId(blk->getLabelId())) {
            auto pred_id = pred->Mblock->getLabelId();
            if (current_func->getMachineCFG()->GetSuccessorsByBlockId(pred_id).size() > 1) {
                current_func->InsertNewBranchOnlyBlockBetweenEdge(pred_id, blk->getLabelId());
            }
        }
    };

    auto block_it = current_func->getMachineCFG()->getSeqScanIterator();
    block_it->open();
    while (block_it->hasNext()) {
        auto blk = block_it->next()->Mblock;

        bool has_phi =
        std::any_of(blk->begin(), blk->end(), [](auto ins) { return ins->arch == MachineBaseInstruction::PHI; });
        if (!has_phi)
            continue;

        insertIntermediateBlocks(blk);
        processPhiInstructions(blk);
    }

    // 第二次扫描处理ParallelCopyList
    block_it = current_func->getMachineCFG()->getSeqScanIterator();
    block_it->open();

    while (block_it->hasNext()) {
        auto blk = block_it->next()->Mblock;
        auto &copylist = blk->GetParallelCopyList();
        if (copylist.empty())
            continue;

        std::map<Register, int> ref_count;
        std::queue<Register> ready_queue;

        for (const auto &[dst, src] : copylist) {
            ref_count[dst] = 0;
            if (src->op_type == MachineBaseOperand::REG) {
                auto src_reg = static_cast<MachineRegister *>(src)->reg;
                Assert(src_reg.is_virtual);
                ref_count[src_reg]++;
            }
        }

        for (const auto &[reg, count] : ref_count) {
            if (count == 0) {
                ready_queue.push(reg);
            }
        }

        auto insert_pos = blk->getInsertBeforeBrIt();
        while (!copylist.empty()) {
            if (!ready_queue.empty()) {
                auto dst = ready_queue.front();
                ready_queue.pop();

                auto it = copylist.find(dst);
                if (it != copylist.end()) {
                    auto src = it->second;
                    copylist.erase(it);

                    if (src->op_type == MachineBaseOperand::REG) {
                        auto src_reg = static_cast<MachineRegister *>(src)->reg;
                        if (--ref_count[src_reg] == 0) {
                            ready_queue.push(src_reg);
                        }
                    }

                    auto copy_instr = new MachineCopyInstruction(src, new MachineRegister(dst), dst.type);
                    copy_instr->SetNoSchedule(true);
                    blk->insert(insert_pos, copy_instr);
                }
            } else {
                auto [dst, src] = *copylist.begin();
                auto tmp_reg = current_func->GetNewRegister(dst.type.data_type, dst.type.data_length);

                auto copy_instr = new MachineCopyInstruction(src, new MachineRegister(dst), dst.type);
                copy_instr->SetNoSchedule(true);
                blk->insert(insert_pos, copy_instr);

                if (src->op_type == MachineBaseOperand::REG) {
                    auto src_reg = static_cast<MachineRegister *>(src)->reg;
                    if (--ref_count[src_reg] == 0) {
                        ready_queue.push(src_reg);
                    }
                }

                ref_count[tmp_reg]++;
                blk->InsertParallelCopyList(dst, new MachineRegister(tmp_reg));
                copylist.erase(dst);
            }
        }
    }
}
