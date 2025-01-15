#include "basic_register_allocation.h"

void RegisterAllocation::Execute() {
    // 初始化未分配的函数队列
    for (auto func : unit->functions) {
        not_allocated_funcs.push(func);
    }
    int iterations = 0;
    while (!not_allocated_funcs.empty()) {
        // 获取当前处理的函数
        current_func = not_allocated_funcs.front();
        numbertoins.clear();

        // 为指令编号
        InstructionNumber(unit, numbertoins).ExecuteInFunc(current_func);

        // 清除之前的分配结果并从队列移除
        alloc_result[current_func].clear();
        not_allocated_funcs.pop();

        // 更新活跃区间并尝试寄存器分配
        UpdateIntervalsInCurrentFunc();
        CoalesceInCurrentFunc();
        if (DoAllocInCurrentFunc()) {    // 尝试进行分配
            // 如果发生溢出，插入spill指令后将所有物理寄存器退回到虚拟寄存器，重新分配
        spiller->ExecuteInFunc(current_func, &alloc_result[current_func]);    // 生成溢出代码
        current_func->AddStackSize(phy_regs_tools->getSpillSize());                 // 调整栈的大小
         not_allocated_funcs.push(current_func);                               // 重新分配直到不再spill
         iterations++;
        }
       
    }

    // 将虚拟寄存器重写为物理寄存器
    VirtualRegisterRewrite(unit, alloc_result).Execute();
}

void InstructionNumber::Execute() {
    for (auto func : unit->functions) {
        ExecuteInFunc(func);
    }
}

void InstructionNumber::ExecuteInFunc(MachineFunction *func) {
    int count_begin = 0;
    current_func = func;

    auto it = func->getMachineCFG()->getBFSIterator();
    it->open();

    while (it->hasNext()) {
        auto mcfg_node = it->next();
        auto mblock = mcfg_node->Mblock;

        // 为基本块分配编号
        this->numbertoins[count_begin] = InstructionNumberEntry(nullptr, true);
        count_begin++;

        for (auto ins : *mblock) {
           if (ins->arch != MachineBaseInstruction::COMMENT) {
                this->numbertoins[count_begin] = InstructionNumberEntry(ins, false);
                ins->setNumber(count_begin++);
            }
        }
    }
}

void RegisterAllocation::UpdateIntervalsInCurrentFunc() {
    intervals.clear();
    copy_sources.clear();
    auto mfun = current_func;
    auto mcfg = mfun->getMachineCFG();
    Liveness liveness(mfun);

    auto it = mcfg->getReverseIterator(mcfg->getBFSIterator());
    it->open();

    std::map<Register, int> last_def, last_use;

    while (it->hasNext()) {
        auto mcfg_node = it->next();
        auto mblock = mcfg_node->Mblock;
        auto cur_id = mcfg_node->Mblock->getLabelId();

        // 更新出度集合的活跃区间
        for (auto reg : liveness.GetOUT(cur_id)) {
            if (intervals.find(reg) == intervals.end()) {
                intervals[reg] = LiveInterval(reg);
            }

            intervals[reg].PushFront(mblock->getBlockInNumber(), mblock->getBlockOutNumber());
            last_use[reg] = mblock->getBlockOutNumber();
        }

        // 遍历基本块中的指令
        for (auto reverse_it = mcfg_node->Mblock->ReverseBegin();
             reverse_it != mcfg_node->Mblock->ReverseEnd(); ++reverse_it) {
            auto ins = *reverse_it;
            if (ins->arch == MachineBaseInstruction::COPY) {
                // Update copy_sources
                // Log("COPY");
                //std::cout<<"999"<<std::endl;
                for (auto reg_w : ins->GetWriteReg()) {
                    for (auto reg_r : ins->GetReadReg()) {
                        copy_sources[*reg_w].push_back(*reg_r);
                        copy_sources[*reg_r].push_back(*reg_w);
                    }
                }
            } else if (ins->arch == MachineBaseInstruction::RiscV) {
                // Log("RV");
            }
            for (auto reg : ins->GetWriteReg()) {
                last_def[*reg] = ins->getNumber();

                if (intervals.find(*reg) == intervals.end()) {
                    intervals[*reg] = LiveInterval(*reg);
                }

                if (last_use.find(*reg) != last_use.end()) {
                    last_use.erase(*reg);
                    intervals[*reg].SetMostBegin(ins->getNumber());
                } else {
                    intervals[*reg].PushFront(ins->getNumber(), ins->getNumber());
                }
                intervals[*reg].IncreaseReferenceCount(1);
            }

            for (auto reg : ins->GetReadReg()) {
                if (intervals.find(*reg) == intervals.end()) {
                    intervals[*reg] = LiveInterval(*reg);
                }

                if (last_use.find(*reg) == last_use.end()) {
                    intervals[*reg].PushFront(mblock->getBlockInNumber(), ins->getNumber());
                }
                last_use[*reg] = ins->getNumber();

                intervals[*reg].IncreaseReferenceCount(1);
            }
        }

        // 清除当前基本块的活跃信息
        last_use.clear();
        last_def.clear();
    }

}
