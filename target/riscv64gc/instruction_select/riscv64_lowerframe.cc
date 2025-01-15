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
                for (auto para : func->GetParameters()) {    // 你需要在指令选择阶段正确设置parameters的值
                    if (para.type.data_type == INT64.data_type) {
                        if (i32_cnt < 8) {    // 插入使用寄存器传参的指令
                            b->push_front(rvconstructor->ConstructR(RISCV_ADD, para, GetPhysicalReg(RISCV_a0 + i32_cnt),
                                                                    GetPhysicalReg(RISCV_x0)));
                        }
                        if (i32_cnt >= 8) {    // 插入使用内存传参的指令
                            //  b->push_front(
                            // rvconstructor->ConstructCopyReg(para, GetPhysicalReg(RISCV_a0 + i32_cnt), INT64));
                             b->push_front(rvconstructor->ConstructIImm(RISCV_LD, para, GetPhysicalReg(RISCV_sp), para_offset));
                             para_offset += 8;
                        }
                        i32_cnt++;
                    } else if (para.type.data_type == FLOAT64.data_type) {    // 处理浮点数
                         if (f32_cnt < 8) {
                            b->push_front(
                            rvconstructor->ConstructCopyReg(para, GetPhysicalReg(RISCV_fa0 + f32_cnt), FLOAT64));
                        }
                        if (f32_cnt >= 8) {
            
                            b->push_front(rvconstructor->ConstructIImm(RISCV_FLD, para, GetPhysicalReg(RISCV_fp), para_offset));
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
void GatherUseSregs(MachineFunction *func, std::vector<std::vector<int>> &reg_defblockids,
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
                if (reg->is_virtual == false) {
                    if (RegNeedSaved[reg->reg_no]) {
                        RegNeedSaved[reg->reg_no] = 0;
                        reg_defblockids[reg->reg_no].push_back(b->getLabelId());
                        reg_rwblockids[reg->reg_no].push_back(b->getLabelId());
                    }
                }
            }
            for (auto reg : ins->GetReadReg()) {
                if (reg->is_virtual == false) {
                    if (RegNeedSaved[reg->reg_no]) {
                        RegNeedSaved[reg->reg_no] = 0;
                        reg_rwblockids[reg->reg_no].push_back(b->getLabelId());
                    }
                }
            }
        }
    }
    if (func->HasInParaInStack()) {
        reg_defblockids[RISCV_fp].push_back(0);
        reg_rwblockids[RISCV_fp].push_back(0);
    }
}

void GatherUseSregs (MachineFunction* func, std::vector<int>& saveregs) {
    int RegNeedSaved[64] = {
        [RISCV_s0] = 1,
        [RISCV_s1] = 1,
        [RISCV_s2] = 1,
        [RISCV_s3] = 1,
        [RISCV_s4] = 1,
        [RISCV_s5] = 1,
        [RISCV_s6] = 1,
        [RISCV_s7] = 1,
        [RISCV_s8] = 1,
        [RISCV_s9] = 1,
        [RISCV_s10] = 1,
        [RISCV_s11] = 1,
        [RISCV_fs0] = 1,
        [RISCV_fs1] = 1,
        [RISCV_fs2] = 1,
        [RISCV_fs3] = 1,
        [RISCV_fs4] = 1,
        [RISCV_fs5] = 1,
        [RISCV_fs6] = 1,
        [RISCV_fs7] = 1,
        [RISCV_fs8] = 1,
        [RISCV_fs9] = 1,
        [RISCV_fs10] = 1,
        [RISCV_fs11] = 1,
        [RISCV_ra] = 1,
    };
    for (auto &b : func->blocks) {
        for (auto ins : *b) {
            for (auto reg : ins->GetWriteReg()) {
                if (reg->is_virtual == false) {
                    if (RegNeedSaved[reg->reg_no]) {
                        RegNeedSaved[reg->reg_no] = 0;
                        saveregs.push_back(reg->reg_no);
                    }
                }
            }
        }
    }
    // save fp
    if (func->HasInParaInStack()) {
        if (RegNeedSaved[RISCV_fp]) {
            RegNeedSaved[RISCV_fp] = 0;
            saveregs.push_back(RISCV_fp);
        }
    }
}

void RiscV64LowerStack::Execute() {
    // 在函数在寄存器分配后执行
    // TODO: 在函数开头保存 函数被调者需要保存的寄存器，并开辟栈空间
    // TODO: 在函数结尾恢复 函数被调者需要保存的寄存器，并收回栈空间
    // TODO: 开辟和回收栈空间
    // 具体需要保存/恢复哪些可以查阅RISC-V函数调用约定
    //Log("RiscV64LowerStack");
    for (auto func : unit->functions) {
        current_func = func;
        std::vector<std::vector<int>> saveregs_occurblockids, saveregs_rwblockids;
        GatherUseSregs(func, saveregs_occurblockids, saveregs_rwblockids);
        std::vector<int> sd_blocks;
        std::vector<int> ld_blocks;
        std::vector<int> restore_offset;
        sd_blocks.resize(64);
        ld_blocks.resize(64);
        restore_offset.resize(64);
        int saveregnum = 0, cur_restore_offset = 0;
        for (int i = 0; i < saveregs_occurblockids.size(); i++) {
            auto &vld = saveregs_rwblockids[i];
            if (!vld.empty()) {
                saveregnum++;
            }
        }
        func->AddStackSize(saveregnum * 8);
        auto mcfg = func->getMachineCFG();
        bool restore_at_beginning = (-8 + func->GetStackSize()) >= 2048;
        if (!restore_at_beginning) {
            for (int i = 0; i < saveregs_occurblockids.size(); i++) {
                if (!saveregs_occurblockids[i].empty()) {
                    int regno = i;
                    int sp_offset = cur_restore_offset;
                    cur_restore_offset -= 8;
                    restore_offset[i] = sp_offset;
                    sd_blocks[i] = saveregs_occurblockids[i].front();
                    ld_blocks[i] = saveregs_occurblockids[i].back();

                    // 保存寄存器到栈
                    auto save_block = mcfg->GetNodeByBlockId(sd_blocks[i])->Mblock;
                    int sd_op = (regno >= RISCV_x0 && regno <= RISCV_x31) ? RISCV_SD : RISCV_FSD;
                    save_block->push_front(
                        rvconstructor->ConstructSImm(sd_op, GetPhysicalReg(i), GetPhysicalReg(RISCV_sp), sp_offset));

                    // 恢复寄存器
                    auto restore_block = mcfg->GetNodeByBlockId(ld_blocks[i])->Mblock;
                    auto it = restore_block->getInsertBeforeBrIt();
                    int ld_op = (regno >= RISCV_x0 && regno <= RISCV_x31) ? RISCV_LD : RISCV_FLD;
                    restore_block->insert(
                        it, rvconstructor->ConstructIImm(ld_op, GetPhysicalReg(i), GetPhysicalReg(RISCV_sp), sp_offset));
                }
            }
        }

        for (auto &b : func->blocks) {
            cur_block = b;
            if (b->getLabelId() == 0) {
                if (func->GetStackSize() <= 2032) {
                    b->push_front(rvconstructor->ConstructIImm(RISCV_ADDI, GetPhysicalReg(RISCV_sp),
                                                               GetPhysicalReg(RISCV_sp),
                                                               -func->GetStackSize()));    // sub sp
                } else {
                    auto stacksz_reg = GetPhysicalReg(RISCV_t0);
                    b->push_front(rvconstructor->ConstructR(RISCV_SUB, GetPhysicalReg(RISCV_sp),
                                                            GetPhysicalReg(RISCV_sp), stacksz_reg));
                    b->push_front(rvconstructor->ConstructUImm(RISCV_LI, stacksz_reg, func->GetStackSize()));
                }
                if (func->HasInParaInStack()){
                    b->push_front(rvconstructor->ConstructR(RISCV_ADD, GetPhysicalReg(RISCV_fp), GetPhysicalReg(RISCV_sp), GetPhysicalReg(RISCV_x0)));
                }
                if (restore_at_beginning) {
                    int offset = 0;
                    for (int i = 0; i < 64; i++) {
                        if (!saveregs_occurblockids[i].empty()) {
                            int regno = i;
                            offset -= 8;
                            int sd_op = (regno >= RISCV_x0 && regno <= RISCV_x31) ? RISCV_SD : RISCV_FSD;
                            b->push_front(rvconstructor->ConstructSImm(sd_op, GetPhysicalReg(regno),
                                                                       GetPhysicalReg(RISCV_sp), offset));
                        }
                    }
                } else if (func->HasInParaInStack()) {
                    b->push_front(rvconstructor->ConstructSImm(RISCV_SD, GetPhysicalReg(RISCV_fp), GetPhysicalReg(RISCV_sp), restore_offset[RISCV_fp]));
                }
            }
            auto last_ins = *(b->ReverseBegin());
            Assert(last_ins->arch == MachineBaseInstruction::RiscV);
            auto riscv_last_ins = (RiscV64Instruction *)last_ins;
            if (riscv_last_ins->getOpcode() == RISCV_JALR) {
                if (riscv_last_ins->getRd() == GetPhysicalReg(RISCV_x0)) {
                    if (riscv_last_ins->getRs1() == GetPhysicalReg(RISCV_ra)) {
                        Assert(riscv_last_ins->getImm() == 0);
                        b->pop_back();
                        if (func->GetStackSize() <= 2032) {
                            b->push_back(rvconstructor->ConstructIImm(RISCV_ADDI, GetPhysicalReg(RISCV_sp),
                                                                      GetPhysicalReg(RISCV_sp), func->GetStackSize()));
                        } else {
                            auto stacksz_reg = GetPhysicalReg(RISCV_t0);
                            b->push_back(rvconstructor->ConstructUImm(RISCV_LI, stacksz_reg, func->GetStackSize()));
                            b->push_back(rvconstructor->ConstructR(RISCV_ADD, GetPhysicalReg(RISCV_sp),
                                                                   GetPhysicalReg(RISCV_sp), stacksz_reg));
                        }
                        if (restore_at_beginning) {
                            int offset = 0;
                            for (int i = 0; i < 64; i++) {
                                if (!saveregs_occurblockids[i].empty()) {
                                    int regno = i;
                                    offset -= 8;
                                    int ld_op = (regno >= RISCV_x0 && regno <= RISCV_x31) ? RISCV_LD : RISCV_FLD;
                                    b->push_back(rvconstructor->ConstructIImm(ld_op, GetPhysicalReg(regno),
                                                                              GetPhysicalReg(RISCV_sp), offset));
                                }
                            }
                        }
                        b->push_back(riscv_last_ins);
                    }
                }
            }
        }
    }
}
