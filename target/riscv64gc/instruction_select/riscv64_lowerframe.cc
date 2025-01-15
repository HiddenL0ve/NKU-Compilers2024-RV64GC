#include "riscv64_lowerframe.h"

/**
 * 执行LowerFrame操作，主要用于在函数入口插入参数获取的相关指令。
 */
void RiscV64LowerFrame::Execute() {
    // 遍历所有函数
    for (auto func : unit->functions) {
        current_func = func;

        // 遍历函数的所有基本块
        for (auto &b : func->blocks) {
            cur_block = b;

            // 处理函数入口
            if (b->getLabelId() == 0) { 
                int i32_cnt = 0;
                int f32_cnt = 0;
                int para_offset = 0;

                // 遍历函数的参数
                for (auto para : func->GetParameters()) {
                    if (para.type.data_type == INT64.data_type) {
                        if (i32_cnt < 8) {
                            b->push_front(rvconstructor->ConstructR(
                                RISCV_ADD, para, GetPhysicalReg(RISCV_a0 + i32_cnt), GetPhysicalReg(RISCV_x0)));
                        } else {
                            b->push_front(rvconstructor->ConstructIImm(
                                RISCV_LD, para, GetPhysicalReg(RISCV_sp), para_offset));
                            para_offset += 8;
                        }
                        i32_cnt++;
                    } else if (para.type.data_type == FLOAT64.data_type) {
                        if (f32_cnt < 8) {
                            b->push_front(rvconstructor->ConstructCopyReg(
                                para, GetPhysicalReg(RISCV_fa0 + f32_cnt), FLOAT64));
                        } else {
                            b->push_front(rvconstructor->ConstructIImm(
                                RISCV_FLD, para, GetPhysicalReg(RISCV_fp), para_offset));
                            para_offset += 8;
                        }
                        f32_cnt++;
                    } else {
                        ERROR("Unknown type");
                    }
                }
            }
        }
    }
}

/**
 * 收集需要保存的寄存器及其定义和读写位置。
 */
void GatherUseSregs(MachineFunction *func, 
                    std::vector<std::vector<int>> &reg_defblockids, 
                    std::vector<std::vector<int>> &reg_rwblockids) {
    reg_defblockids.resize(64);
    reg_rwblockids.resize(64);

    for (auto &b : func->blocks) {
        int RegNeedSaved[64] = {
            [RISCV_s0] = 1,  [RISCV_s1] = 1,  [RISCV_s2] = 1,   [RISCV_s3] = 1,   [RISCV_s4] = 1,
            [RISCV_s5] = 1,  [RISCV_s6] = 1,  [RISCV_s7] = 1,   [RISCV_s8] = 1,   [RISCV_s9] = 1,
            [RISCV_s10] = 1, [RISCV_s11] = 1, [RISCV_fs0] = 1,  [RISCV_fs1] = 1,  [RISCV_fs2] = 1,
            [RISCV_fs3] = 1, [RISCV_fs4] = 1, [RISCV_fs5] = 1,  [RISCV_fs6] = 1,  [RISCV_fs7] = 1,
            [RISCV_fs8] = 1, [RISCV_fs9] = 1, [RISCV_fs10] = 1, [RISCV_fs11] = 1, [RISCV_ra] = 1,
        };

        for (auto ins : *b) {
            for (auto reg : ins->GetWriteReg()) {
                if (!reg->is_virtual && RegNeedSaved[reg->reg_no]) {
                    RegNeedSaved[reg->reg_no] = 0;
                    reg_defblockids[reg->reg_no].push_back(b->getLabelId());
                    reg_rwblockids[reg->reg_no].push_back(b->getLabelId());
                }
            }
            for (auto reg : ins->GetReadReg()) {
                if (!reg->is_virtual && RegNeedSaved[reg->reg_no]) {
                    RegNeedSaved[reg->reg_no] = 0;
                    reg_rwblockids[reg->reg_no].push_back(b->getLabelId());
                }
            }
        }
    }

    if (func->HasInParaInStack()) {
        reg_defblockids[RISCV_fp].push_back(0);
        reg_rwblockids[RISCV_fp].push_back(0);
    }
}

/**
 * 执行LowerStack操作，主要用于函数栈空间的分配和寄存器的保存与恢复。
 */
void RiscV64LowerStack::Execute() {
    for (auto func : unit->functions) {
        current_func = func;
        std::vector<std::vector<int>> saveregs_occurblockids, saveregs_rwblockids;
        GatherUseSregs(func, saveregs_occurblockids, saveregs_rwblockids);

        int saveregnum = 0;
        for (auto &vld : saveregs_rwblockids) {
            if (!vld.empty()) saveregnum++;
        }
        func->AddStackSize(saveregnum * 8);

        for (auto &b : func->blocks) {
            cur_block = b;

            if (b->getLabelId() == 0) {
                if (func->GetStackSize() <= 2032) {
                    b->push_front(rvconstructor->ConstructIImm(
                        RISCV_ADDI, GetPhysicalReg(RISCV_sp), GetPhysicalReg(RISCV_sp), -func->GetStackSize()));
                } else {
                    auto stacksz_reg = GetPhysicalReg(RISCV_t0);
                    b->push_front(rvconstructor->ConstructR(
                        RISCV_SUB, GetPhysicalReg(RISCV_sp), GetPhysicalReg(RISCV_sp), stacksz_reg));
                    b->push_front(rvconstructor->ConstructUImm(
                        RISCV_LI, stacksz_reg, func->GetStackSize()));
                }
            }
        }
    }
}
