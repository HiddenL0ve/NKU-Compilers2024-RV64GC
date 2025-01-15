#include "physical_register.h"
// Reference: https://github.com/yuhuifishash/NKU-Compilers2024-RV64GC.git/target/common/machine_passes/register_alloc/physical_register.cc line 2-36
bool PhysicalRegistersAllocTools::OccupyReg(int phy_id, LiveInterval interval) {

    phy_occupied[phy_id].push_back(interval);
    return true;
}

// 释放指定物理寄存器的占用，如果该寄存器包含给定的生命周期区间，则将其移除并返回 true。
bool PhysicalRegistersAllocTools::ReleaseReg(int phy_id, LiveInterval interval) {
    auto it = phy_occupied[phy_id].begin();
    for (; it != phy_occupied[phy_id].end(); ++it) {
        if (*it == interval) { // 找到与给定生命周期区间匹配的记录
            phy_occupied[phy_id].erase(it); // 从寄存器占用列表中移除该区间
            return true;
        }
    }
    return false; // 未找到匹配的生命周期区间，返回 false
}

// 将给定的内存区域标记为占用，用于记录内存被指定生命周期区间所使用。
bool PhysicalRegistersAllocTools::OccupyMem(int offset, int size, LiveInterval interval) {
    size /= 4; // 将字节大小转换为内存单元大小
    for (int i = offset; i < offset + size; i++) {
        while (i >= mem_occupied.size()) { // 如果内存占用列表尚未覆盖到目标地址，扩展其大小
            mem_occupied.push_back({});
        }
        mem_occupied[i].push_back(interval); // 将生命周期区间添加到对应的内存地址列表中
    }
    return true; 
}

// 释放指定内存区域的占用，从给定的内存区间中移除特定的生命周期区间。
bool PhysicalRegistersAllocTools::ReleaseMem(int offset, int size, LiveInterval interval) {
    size /= 4; // 将字节大小转换为内存单元大小
    for (int i = offset; i < offset + size; i++) {
        auto it = mem_occupied[i].begin();
        for (; it != mem_occupied[i].end(); ++it) {
            if (*it == interval) { // 找到与给定生命周期区间匹配的记录
                mem_occupied[i].erase(it); // 从内存占用列表中移除该区间
                break; // 匹配成功后无需继续遍历
            }
        }
    }
    return true;
}


int PhysicalRegistersAllocTools::getIdleReg(LiveInterval interval, std::vector<int> preferd_regs,
                                            std::vector<int> noprefer_regs) {
    PRINT("\nVreg: ");
    interval.Print();

    // 检查优先寄存器
    auto tryRegister = [&](int reg) -> bool {
        for (auto conflict_j : getAliasRegs(reg)) {
            for (auto other_interval : phy_occupied[conflict_j]) {
                PRINT("\nTry Phy %d", reg);
                PRINT("Othe: ");
                other_interval.Print();
                if (interval & other_interval) {
                    PRINT("\n->Fail\n");
                    return false;
                } else {
                    PRINT("\n->Success\n");
                }
            }
        }
        return true;
    };

    // 优先检查 preferd_regs 中的寄存器
    for (auto reg : preferd_regs) {
        if (tryRegister(reg)) {
            return reg;
        }
    }

    // 标记已尝试的寄存器
    std::map<int, int> reg_tried, reg_valid;
    for (auto reg : preferd_regs) {
        reg_tried[reg] = 1;
    }
    for (auto reg : noprefer_regs) {
        reg_tried[reg] = 1;
    }

    // 检查所有有效寄存器
    for (auto reg : getValidRegs(interval)) {
        reg_valid[reg] = 1;
        if (reg_tried[reg]) continue;
        if (tryRegister(reg)) {
            return reg;
        }
    }

    // 检查非优先寄存器
    for (auto reg : noprefer_regs) {
        if (!reg_valid[reg]) continue;
        if (tryRegister(reg)) {
            return reg;
        }
    }

    // 未找到可用寄存器，返回 -1
    return -1;
}

int PhysicalRegistersAllocTools::getIdleMem(LiveInterval interval) {
    std::vector<bool> ok;
    ok.resize(mem_occupied.size(), true);
    for (int i = 0; i < mem_occupied.size(); i++) {
        ok[i] = true;
        for (auto other_interval : mem_occupied[i]) {
            if (interval & other_interval) {
                ok[i] = false;
                break;
            }
        }
    }
    int free_cnt = 0;
    for (int offset = 0; offset < ok.size(); offset++) {
        if (ok[offset]) {
            free_cnt++;
        } else {
            free_cnt = 0;
        }
        if (free_cnt == interval.getReg().getDataWidth() / 4) {
            return offset - free_cnt + 1;
        }
    }
    return mem_occupied.size() - free_cnt;
}

int PhysicalRegistersAllocTools::swapRegspill(int p_reg1, LiveInterval interval1, int offset_spill2, int size,
                                    LiveInterval interval2) {

    // 1. 释放寄存器p_reg1
    ReleaseReg(p_reg1, interval1);
    // 2. 释放内存offset_spill2
    ReleaseMem(offset_spill2, size, interval2);
    // 3. 分配寄存器p_reg1;
    OccupyReg(p_reg1, interval2);
    // 4. 分配内存offset_spill2
    // OccupyMem(getIdleMem(interval1), size, interval1);
    return 0;
}

std::vector<LiveInterval> PhysicalRegistersAllocTools::getConflictIntervals(LiveInterval interval) {
    std::vector<LiveInterval> result;
    for (auto phy_intervals : phy_occupied) {
        for (auto other_interval : phy_intervals) {
            if (interval.getReg().type == other_interval.getReg().type && (interval & other_interval)) {
                result.push_back(other_interval);
            }
        }
    }
    return result;
}
std::vector<int> PhysicalRegistersAllocTools::getAliasRegs(int phy_id) { return std::vector<int>({phy_id}); }
