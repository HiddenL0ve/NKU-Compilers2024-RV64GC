#include "riscv64_instSelect.h"
#include <sstream>

Register RiscV64Selector::GetllvmReg(int ir_reg, MachineDataType type) {
    if (llvm_rv_regtable.find(ir_reg) == llvm_rv_regtable.end()) {
        llvm_rv_regtable[ir_reg] = GetNewReg(type);
    }
    Assert(llvm_rv_regtable[ir_reg].type == type);
    return llvm_rv_regtable[ir_reg];
}
Register RiscV64Selector::GetNewReg(MachineDataType type, bool save_across_call) {
    return cur_func->GetNewRegister(type.data_type, type.data_length);
}
template <> void RiscV64Selector::ConvertAndAppend<LoadInstruction *>(LoadInstruction *ins) {
    if (auto *regOp = dynamic_cast<RegOperand *>(ins->GetPointer())) {
        auto *rdOp = dynamic_cast<RegOperand *>(ins->GetResult());

        Register rd;
        auto lwOpcode = RISCV_LW;
        switch (ins->GetDataType()) {
        case BasicInstruction::LLVMType::I32:
        case BasicInstruction::LLVMType::PTR:
            rd = GetllvmReg(rdOp->GetRegNo(), INT64);
            lwOpcode = ins->GetDataType() == BasicInstruction::LLVMType::PTR ? RISCV_LD : RISCV_LW;
            break;
        default:
            ERROR("Unexpected data type");
            return;
        }

        if (llvm_rv_allocas.find(regOp->GetRegNo()) == llvm_rv_allocas.end()) {
            Register ptr = GetllvmReg(regOp->GetRegNo(), INT64);
            auto lwInstr = rvconstructor->ConstructIImm(lwOpcode, rd, ptr, 0);
            cur_block->push_back(lwInstr);
        } else {
            auto spOffset = llvm_rv_allocas[regOp->GetRegNo()];
            auto lwInstr = rvconstructor->ConstructIImm(lwOpcode, rd, GetPhysicalReg(RISCV_sp), spOffset);
            ((RiscV64Function *)cur_func)->AddAllocaIns(lwInstr);
            cur_block->push_back(lwInstr);
        }
    } else if (auto *globalOp = dynamic_cast<GlobalOperand *>(ins->GetPointer())) {
        auto *rdOp = dynamic_cast<RegOperand *>(ins->GetResult());
        auto addrHi = GetNewReg(INT64);
        auto lwOpcode = RISCV_LW;
        switch (ins->GetDataType()) {
        case BasicInstruction::LLVMType::I32:
        case BasicInstruction::LLVMType::PTR:
            lwOpcode = ins->GetDataType() == BasicInstruction::LLVMType::PTR ? RISCV_LD : RISCV_LW;
            break;
        case BasicInstruction::LLVMType::FLOAT32:
            lwOpcode = RISCV_FLW;
            break;
        default:
            ERROR("Unexpected data type");
            return;
        }

        auto rd = GetllvmReg(rdOp->GetRegNo(), INT64);
        auto luiInstr = rvconstructor->ConstructULabel(RISCV_LUI, addrHi, RiscVLabel(globalOp->GetName(), true));
        auto lwInstr = rvconstructor->ConstructILabel(lwOpcode, rd, addrHi, RiscVLabel(globalOp->GetName(), false));

        cur_block->push_back(luiInstr);
        cur_block->push_back(lwInstr);
    }
}

template <> void RiscV64Selector::ConvertAndAppend<StoreInstruction *>(StoreInstruction *ins) {
    Register value_reg;
    auto operandType = ins->GetValue()->GetOperandType();

    switch (operandType) {
    case BasicOperand::IMMI32: {
        auto val_imm = static_cast<ImmI32Operand *>(ins->GetValue());
        value_reg = GetNewReg(INT64);
        auto imm_copy_ins = rvconstructor->ConstructCopyRegImmI(value_reg, val_imm->GetIntImmVal(), INT64);
        cur_block->push_back(imm_copy_ins);
        break;
    }
    case BasicOperand::REG: {
        auto val_reg = static_cast<RegOperand *>(ins->GetValue());
        switch (ins->GetDataType()) {
        case BasicInstruction::LLVMType::I32:
            value_reg = GetllvmReg(val_reg->GetRegNo(), INT64);
            break;
        default:
            ERROR("Unexpected data type");
            return;
        }
        break;
    }
    default:
        ERROR("Unexpected or unimplemented operand type");
        return;
    }

    auto pointerType = ins->GetPointer()->GetOperandType();
    switch (pointerType) {
    case BasicOperand::REG: {
        auto reg_ptr_op = static_cast<RegOperand *>(ins->GetPointer());
        auto dataType = ins->GetDataType();
        if (dataType == BasicInstruction::LLVMType::I32 || dataType == BasicInstruction::LLVMType::FLOAT32) {
            auto storeOpcode = dataType == BasicInstruction::LLVMType::FLOAT32 ? RISCV_FSW : RISCV_SW;
            auto ptr_reg = GetllvmReg(reg_ptr_op->GetRegNo(), INT64);
            auto sp_offset = llvm_rv_allocas.find(reg_ptr_op->GetRegNo()) == llvm_rv_allocas.end()
                             ? 0
                             : llvm_rv_allocas[reg_ptr_op->GetRegNo()];
            auto store_instruction = rvconstructor->ConstructSImm(
            storeOpcode, value_reg, sp_offset ? GetPhysicalReg(RISCV_sp) : ptr_reg, sp_offset);
            if (sp_offset) {
                ((RiscV64Function *)cur_func)->AddAllocaIns(store_instruction);
            }
            cur_block->push_back(store_instruction);
        } else {
            ERROR("Unexpected data type");
        }
        break;
    }
    case BasicOperand::GLOBAL: {
        auto global_op = static_cast<GlobalOperand *>(ins->GetPointer());
        auto addr_hi = GetNewReg(INT64);
        auto lui_instruction =
        rvconstructor->ConstructULabel(RISCV_LUI, addr_hi, RiscVLabel(global_op->GetName(), true));
        cur_block->push_back(lui_instruction);

        auto storeOpcode = ins->GetDataType() == BasicInstruction::LLVMType::FLOAT32 ? RISCV_FSW : RISCV_SW;
        auto store_instruction =
        rvconstructor->ConstructSLabel(storeOpcode, value_reg, addr_hi, RiscVLabel(global_op->GetName(), false));
        cur_block->push_back(store_instruction);
        break;
    }
    default:
        ERROR("Unexpected operand type for pointer");
        return;
    }
}
Register RiscV64Selector::ExtractOp2Reg(BasicOperand *op, MachineDataType type) {
    auto operandType = op->GetOperandType();
    Register ret;

    switch (operandType) {
    case BasicOperand::IMMI32:
        Assert(type == INT64);
        ret = GetNewReg(INT64);
        cur_block->push_back(
        rvconstructor->ConstructCopyRegImmI(ret, static_cast<ImmI32Operand *>(op)->GetIntImmVal(), INT64));
        break;
    case BasicOperand::REG:
        ret = GetllvmReg(static_cast<RegOperand *>(op)->GetRegNo(), type);
        break;
    default:
        ERROR("Unexpected op type");
        return Register();
    }

    return ret;
}
int RiscV64Selector::ExtractOp2ImmI32(BasicOperand *op) {
    //("%d",5);
    Assert(op->GetOperandType() == BasicOperand::IMMI32);
    return ((ImmI32Operand *)op)->GetIntImmVal();
}

float RiscV64Selector::ExtractOp2ImmF32(BasicOperand *op) {
    Assert(op->GetOperandType() == BasicOperand::IMMF32);
    return ((ImmF32Operand *)op)->GetFloatVal();
}

// Reference: https://github.com/yuhuifishash/NKU-Compilers2024-RV64GC.git/target/riscv64gc/instruction_select/riscv64_instSelect.cc line 233-314
template <> void RiscV64Selector::ConvertAndAppend<ArithmeticInstruction *>(ArithmeticInstruction *ins) {
    // TODO("Implement this if you need");
    // 加法
    // printf("%d",5);
    if (ins->GetOpcode() == BasicInstruction::ADD) {
        auto dataType = ins->GetDataType();
        auto operand1Type = ins->GetOperand1()->GetOperandType();
        auto operand2Type = ins->GetOperand2()->GetOperandType();
        auto resultType = ins->GetResultOperand()->GetOperandType();

        if (dataType == BasicInstruction::I32) {
            auto *rd_op = static_cast<RegOperand *>(ins->GetResultOperand());

            if (operand1Type == BasicOperand::IMMI32 && operand2Type == BasicOperand::IMMI32) {
                auto *imm1_op = static_cast<ImmI32Operand *>(ins->GetOperand1());
                auto *imm2_op = static_cast<ImmI32Operand *>(ins->GetOperand2());
                auto imm1 = imm1_op->GetIntImmVal();
                auto imm2 = imm2_op->GetIntImmVal();
                auto rd = GetllvmReg(rd_op->GetRegNo(), INT64);
                auto copy_imm_instr = rvconstructor->ConstructCopyRegImmI(rd, imm1 + imm2, INT64);
                cur_block->push_back(copy_imm_instr);
            } else if (operand1Type == BasicOperand::REG && operand2Type == BasicOperand::IMMI32) {
                Assert(resultType == BasicOperand::REG);
                auto *rs_op = static_cast<RegOperand *>(ins->GetOperand1());
                auto *i_op = static_cast<ImmI32Operand *>(ins->GetOperand2());
                auto rd = GetllvmReg(rd_op->GetRegNo(), INT64);
                auto rs = GetllvmReg(rs_op->GetRegNo(), INT64);
                auto imm = i_op->GetIntImmVal();
                if (imm != 0) {
                    auto addiw_instr = rvconstructor->ConstructIImm(RISCV_ADDIW, rd, rs, imm);
                    cur_block->push_back(addiw_instr);
                } else {
                    auto copy_instr = rvconstructor->ConstructCopyReg(rd, rs, INT64);
                    cur_block->push_back(copy_instr);
                }
            } else if (operand1Type == BasicOperand::REG && operand2Type == BasicOperand::REG) {
                Assert(resultType == BasicOperand::REG);
                auto *rs_op = static_cast<RegOperand *>(ins->GetOperand1());
                auto *rt_op = static_cast<RegOperand *>(ins->GetOperand2());
                auto rd = GetllvmReg(rd_op->GetRegNo(), INT64);
                auto rs = GetllvmReg(rs_op->GetRegNo(), INT64);
                auto rt = GetllvmReg(rt_op->GetRegNo(), INT64);
                auto addw_instr = rvconstructor->ConstructR(RISCV_ADDW, rd, rs, rt);
                cur_block->push_back(addw_instr);
            } else if (operand2Type == BasicOperand::REG && operand1Type == BasicOperand::IMMI32) {
                Assert(resultType == BasicOperand::REG);
                auto *rs_op = static_cast<RegOperand *>(ins->GetOperand2());
                auto *i_op = static_cast<ImmI32Operand *>(ins->GetOperand1());
                auto rd = GetllvmReg(rd_op->GetRegNo(), INT64);
                auto rs = GetllvmReg(rs_op->GetRegNo(), INT64);
                auto imm = i_op->GetIntImmVal();
                if (imm != 0) {
                    auto addiw_instr = rvconstructor->ConstructIImm(RISCV_ADDIW, rd, rs, imm);
                    cur_block->push_back(addiw_instr);
                } else {
                    auto copy_instr = rvconstructor->ConstructCopyReg(rd, rs, INT64);
                    cur_block->push_back(copy_instr);
                }
            }
        }
    }
    // 减法
    if (ins->GetOpcode() == BasicInstruction::SUB) {
        auto operand1Type = ins->GetOperand1()->GetOperandType();
        auto operand2Type = ins->GetOperand2()->GetOperandType();
        auto resultType = ins->GetResultOperand()->GetOperandType();

        if (operand1Type == BasicOperand::IMMI32 && operand2Type == BasicOperand::IMMI32) {
            Assert(resultType == BasicOperand::REG);
            auto *rd_op = static_cast<RegOperand *>(ins->GetResultOperand());
            auto *imm1_op = static_cast<ImmI32Operand *>(ins->GetOperand1());
            auto *imm2_op = static_cast<ImmI32Operand *>(ins->GetOperand2());
            auto imm1 = imm1_op->GetIntImmVal();
            auto imm2 = imm2_op->GetIntImmVal();
            auto rd = GetllvmReg(rd_op->GetRegNo(), INT64);
            auto copy_imm_instr = rvconstructor->ConstructCopyRegImmI(rd, imm1 - imm2, INT64);
            cur_block->push_back(copy_imm_instr);
        } else if (operand1Type == BasicOperand::REG && operand2Type == BasicOperand::IMMI32) {
            Assert(resultType == BasicOperand::REG);
            auto *rd_op = static_cast<RegOperand *>(ins->GetResultOperand());
            auto *reg1_op = static_cast<RegOperand *>(ins->GetOperand1());
            auto *imm_op = static_cast<ImmI32Operand *>(ins->GetOperand2());
            auto rd = GetllvmReg(rd_op->GetRegNo(), INT64);
            auto reg1 = GetllvmReg(reg1_op->GetRegNo(), INT64);
            auto imm = imm_op->GetIntImmVal();
            auto addiw_instr = rvconstructor->ConstructIImm(RISCV_ADDIW, rd, reg1, -imm);
            cur_block->push_back(addiw_instr);
        } else {
            Assert(resultType == BasicOperand::REG);
            auto *rd_op = static_cast<RegOperand *>(ins->GetResultOperand());
            Register reg1 = ExtractOp2Reg(ins->GetOperand1(), INT64);
            Register reg2 = ExtractOp2Reg(ins->GetOperand2(), INT64);
            auto rd = GetllvmReg(rd_op->GetRegNo(), INT64);
            auto sub_instr = rvconstructor->ConstructR(RISCV_SUBW, rd, reg1, reg2);
            cur_block->push_back(sub_instr);
        }
    }

    // 乘法
    else if (ins->GetOpcode() == BasicInstruction::MUL) {
        auto operand1Type = ins->GetOperand1()->GetOperandType();
        auto operand2Type = ins->GetOperand2()->GetOperandType();
        auto resultType = ins->GetResultOperand()->GetOperandType();

        if (operand1Type == BasicOperand::IMMI32 && operand2Type == BasicOperand::IMMI32) {
            Assert(resultType == BasicOperand::REG);
            auto *rd_op = static_cast<RegOperand *>(ins->GetResultOperand());
            auto *imm1_op = static_cast<ImmI32Operand *>(ins->GetOperand1());
            auto *imm2_op = static_cast<ImmI32Operand *>(ins->GetOperand2());
            auto imm1 = imm1_op->GetIntImmVal();
            auto imm2 = imm2_op->GetIntImmVal();
            auto rd = GetllvmReg(rd_op->GetRegNo(), INT64);
            auto copy_imm_instr = rvconstructor->ConstructCopyRegImmI(rd, imm1 * imm2, INT64);
            cur_block->push_back(copy_imm_instr);
        } else {
            Assert(resultType == BasicOperand::REG);
            auto *rd_op = static_cast<RegOperand *>(ins->GetResultOperand());
            auto rd = GetllvmReg(rd_op->GetRegNo(), INT64);
            Register mul1, mul2;

            if (operand1Type == BasicOperand::IMMI32) {
                auto *imm1_op = static_cast<ImmI32Operand *>(ins->GetOperand1());
                mul1 = GetNewReg(INT64);
                auto copy_imm_instr = rvconstructor->ConstructCopyRegImmI(mul1, imm1_op->GetIntImmVal(), INT64);
                cur_block->push_back(copy_imm_instr);
            } else if (operand1Type == BasicOperand::REG) {
                auto *reg1_op = static_cast<RegOperand *>(ins->GetOperand1());
                mul1 = GetllvmReg(reg1_op->GetRegNo(), INT64);
            } else {
                ERROR("Unexpected op type");
            }

            if (operand2Type == BasicOperand::IMMI32) {
                auto *imm2_op = static_cast<ImmI32Operand *>(ins->GetOperand2());
                mul2 = GetNewReg(INT64);
                auto copy_imm_instr = rvconstructor->ConstructCopyRegImmI(mul2, imm2_op->GetIntImmVal(), INT64);
                cur_block->push_back(copy_imm_instr);
            } else if (operand2Type == BasicOperand::REG) {
                auto *reg2_op = static_cast<RegOperand *>(ins->GetOperand2());
                mul2 = GetllvmReg(reg2_op->GetRegNo(), INT64);
            } else {
                ERROR("Unexpected op type");
            }

            auto mul_instr = rvconstructor->ConstructR(RISCV_MULW, rd, mul1, mul2);
            cur_block->push_back(mul_instr);
        }
    }

    // 除法
    else if (ins->GetOpcode() == BasicInstruction::DIV) {
        auto operand1Type = ins->GetOperand1()->GetOperandType();
        auto operand2Type = ins->GetOperand2()->GetOperandType();
        auto resultType = ins->GetResultOperand()->GetOperandType();

        if (operand1Type == BasicOperand::IMMI32 && operand2Type == BasicOperand::IMMI32) {
            Assert(resultType == BasicOperand::REG);
            auto *rd_op = static_cast<RegOperand *>(ins->GetResultOperand());
            auto *imm1_op = static_cast<ImmI32Operand *>(ins->GetOperand1());
            auto *imm2_op = static_cast<ImmI32Operand *>(ins->GetOperand2());
            auto imm1 = imm1_op->GetIntImmVal();
            auto imm2 = imm2_op->GetIntImmVal();
            auto rd = GetllvmReg(rd_op->GetRegNo(), INT64);

            Assert(imm2 != 0);    // Ensure the divisor is not zero
            auto copy_imm_instr = rvconstructor->ConstructCopyRegImmI(rd, imm1 / imm2, INT64);
            cur_block->push_back(copy_imm_instr);
        } else {
            Assert(resultType == BasicOperand::REG);
            auto *rd_op = static_cast<RegOperand *>(ins->GetResultOperand());
            auto rd = GetllvmReg(rd_op->GetRegNo(), INT64);
            Register div1, div2;

            if (operand1Type == BasicOperand::IMMI32) {
                auto *imm1_op = static_cast<ImmI32Operand *>(ins->GetOperand1());
                div1 = GetNewReg(INT64);
                auto copy_imm_instr = rvconstructor->ConstructCopyRegImmI(div1, imm1_op->GetIntImmVal(), INT64);
                cur_block->push_back(copy_imm_instr);
            } else if (operand1Type == BasicOperand::REG) {
                auto *reg1_op = static_cast<RegOperand *>(ins->GetOperand1());
                div1 = GetllvmReg(reg1_op->GetRegNo(), INT64);
            } else {
                ERROR("Unexpected op type");
            }

            if (operand2Type == BasicOperand::IMMI32) {
                auto *imm2_op = static_cast<ImmI32Operand *>(ins->GetOperand2());
                div2 = GetNewReg(INT64);
                auto copy_imm_instr = rvconstructor->ConstructCopyRegImmI(div2, imm2_op->GetIntImmVal(), INT64);
                cur_block->push_back(copy_imm_instr);
            } else if (operand2Type == BasicOperand::REG) {
                auto *reg2_op = static_cast<RegOperand *>(ins->GetOperand2());
                div2 = GetllvmReg(reg2_op->GetRegNo(), INT64);
            } else {
                ERROR("Unexpected op type");
            }

            auto div_instr = rvconstructor->ConstructR(RISCV_DIVW, rd, div1, div2);
            cur_block->push_back(div_instr);
        }
    }
    // 模
    else if (ins->GetOpcode() == BasicInstruction::MOD) {
        auto operand1Type = ins->GetOperand1()->GetOperandType();
        auto operand2Type = ins->GetOperand2()->GetOperandType();
        auto resultType = ins->GetResultOperand()->GetOperandType();

        if (operand1Type == BasicOperand::IMMI32 && operand2Type == BasicOperand::IMMI32) {
            Assert(resultType == BasicOperand::REG);
            auto *rd_op = static_cast<RegOperand *>(ins->GetResultOperand());
            auto *imm1_op = static_cast<ImmI32Operand *>(ins->GetOperand1());
            auto *imm2_op = static_cast<ImmI32Operand *>(ins->GetOperand2());
            auto imm1 = imm1_op->GetIntImmVal();
            auto imm2 = imm2_op->GetIntImmVal();
            auto rd = GetllvmReg(rd_op->GetRegNo(), INT64);

            Assert(imm2 != 0);    // Ensure the divisor is not zero
            auto copy_imm_instr = rvconstructor->ConstructCopyRegImmI(rd, imm1 % imm2, INT64);
            cur_block->push_back(copy_imm_instr);
        } else {
            Assert(resultType == BasicOperand::REG);
            auto *rd_op = static_cast<RegOperand *>(ins->GetResultOperand());
            auto rd = GetllvmReg(rd_op->GetRegNo(), INT64);
            Register rem1, rem2;

            if (operand1Type == BasicOperand::IMMI32) {
                auto *imm1_op = static_cast<ImmI32Operand *>(ins->GetOperand1());
                rem1 = GetNewReg(INT64);
                auto copy_imm_instr = rvconstructor->ConstructCopyRegImmI(rem1, imm1_op->GetIntImmVal(), INT64);
                cur_block->push_back(copy_imm_instr);
            } else if (operand1Type == BasicOperand::REG) {
                auto *reg1_op = static_cast<RegOperand *>(ins->GetOperand1());
                rem1 = GetllvmReg(reg1_op->GetRegNo(), INT64);
            } else {
                ERROR("Unexpected op type");
            }

            if (operand2Type == BasicOperand::IMMI32) {
                auto *imm2_op = static_cast<ImmI32Operand *>(ins->GetOperand2());
                rem2 = GetNewReg(INT64);
                auto copy_imm_instr = rvconstructor->ConstructCopyRegImmI(rem2, imm2_op->GetIntImmVal(), INT64);
                cur_block->push_back(copy_imm_instr);
            } else if (operand2Type == BasicOperand::REG) {
                auto *reg2_op = static_cast<RegOperand *>(ins->GetOperand2());
                rem2 = GetllvmReg(reg2_op->GetRegNo(), INT64);
            } else {
                ERROR("Unexpected op type");
            }

            auto rem_instr = rvconstructor->ConstructR(RISCV_REMW, rd, rem1, rem2);
            cur_block->push_back(rem_instr);
        }
    }
    
}

template <> void RiscV64Selector::ConvertAndAppend<IcmpInstruction *>(IcmpInstruction *ins) {
    auto resultType = ins->GetResult()->GetOperandType();
    Assert(resultType == BasicOperand::REG);
    auto *res_op = static_cast<RegOperand *>(ins->GetResult());
    auto res_reg = GetllvmReg(res_op->GetRegNo(), INT64);
    cmp_context[res_reg] = ins;
}

template <> void RiscV64Selector::ConvertAndAppend<FcmpInstruction *>(FcmpInstruction *ins) {
    auto resultType = ins->GetResult()->GetOperandType();
    Assert(resultType == BasicOperand::REG);
    auto *res_op = static_cast<RegOperand *>(ins->GetResult());
    auto res_reg = GetllvmReg(res_op->GetRegNo(), INT64);
    cmp_context[res_reg] = ins;
}

template <> void RiscV64Selector::ConvertAndAppend<AllocaInstruction *>(AllocaInstruction *ins) {
    auto resultType = ins->GetResultOp()->GetOperandType();
    Assert(resultType == BasicOperand::REG);
    auto *reg_op = static_cast<RegOperand *>(ins->GetResultOp());
    int byte_size = ins->GetAllocaSize() << 2;
    llvm_rv_allocas[reg_op->GetRegNo()] = cur_offset;
    cur_offset += byte_size;
}

template <> void RiscV64Selector::ConvertAndAppend<BrCondInstruction *>(BrCondInstruction *ins) {
rvconstructor->DisableSchedule();

Assert(ins->GetCond()->GetOperandType() == BasicOperand::REG);
auto cond_reg = (RegOperand *)ins->GetCond();
auto br_reg = GetllvmReg(cond_reg->GetRegNo(), INT64);
auto cmp_ins = cmp_context[br_reg];

int opcode;
Register cmp_rd, cmp_op1, cmp_op2;
if (cmp_ins->GetOpcode() == BasicInstruction::ICMP) {
    auto icmp_ins = (IcmpInstruction *)cmp_ins;

    // Handle ICMP op1
    if (icmp_ins->GetOp1()->GetOperandType() == BasicOperand::REG) {
        cmp_op1 = GetllvmReg(((RegOperand *)icmp_ins->GetOp1())->GetRegNo(), INT64);
    } else if (icmp_ins->GetOp1()->GetOperandType() == BasicOperand::IMMI32) {
        auto imm_val = ((ImmI32Operand *)icmp_ins->GetOp1())->GetIntImmVal();
        if (imm_val != 0) {
            cmp_op1 = GetNewReg(INT64);
            auto copy_imm_instr = rvconstructor->ConstructCopyRegImmI(
                cmp_op1, imm_val, INT64);
            cur_block->push_back(copy_imm_instr);
        } else {
            cmp_op1 = GetPhysicalReg(RISCV_x0);
        }
    } else {
        ERROR("Unexpected ICMP op1 type");
    }

    // Handle ICMP op2
    if (icmp_ins->GetOp2()->GetOperandType() == BasicOperand::REG) {
        cmp_op2 = GetllvmReg(((RegOperand *)icmp_ins->GetOp2())->GetRegNo(), INT64);
    } else if (icmp_ins->GetOp2()->GetOperandType() == BasicOperand::IMMI32) {
        auto imm_val = ((ImmI32Operand *)icmp_ins->GetOp2())->GetIntImmVal();
        if (imm_val != 0) {
            cmp_op2 = GetNewReg(INT64);
            auto copy_imm_instr = rvconstructor->ConstructCopyRegImmI(
                cmp_op2, imm_val, INT64);
            cur_block->push_back(copy_imm_instr);
        } else {
            cmp_op2 = GetPhysicalReg(RISCV_x0);
        }
    } else {
        ERROR("Unexpected ICMP op2 type");
    }
        switch (icmp_ins->GetCond()) {
        case BasicInstruction::IcmpCond::eq:    // beq
            opcode = RISCV_BEQ;
            break;
        case BasicInstruction::IcmpCond::ne:    // bne
            opcode = RISCV_BNE;
            break;
        case BasicInstruction::IcmpCond::ugt:    // bgtu --p
            opcode = RISCV_BGTU;
            break;
        case BasicInstruction::IcmpCond::uge:    // bgeu
            opcode = RISCV_BGEU;
            break;
        case BasicInstruction::IcmpCond::ult:    // bltu
            opcode = RISCV_BLTU;
            break;
        case BasicInstruction::IcmpCond::ule:    // bleu --p
            opcode = RISCV_BLEU;
            break;
        case BasicInstruction::IcmpCond::sgt:    // bgt --p
            opcode = RISCV_BGT;
            break;
        case BasicInstruction::IcmpCond::sge:    // bge
            opcode = RISCV_BGE;
            break;
        case BasicInstruction::IcmpCond::slt:    // blt
            opcode = RISCV_BLT;
            break;
        case BasicInstruction::IcmpCond::sle:    // ble --p
            opcode = RISCV_BLE;
            break;
        default:
            ERROR("Unexpected ICMP cond");
        }
    } else {
        ERROR("No Cmp Before Br");
    }
    Assert(ins->GetTrueLabel()->GetOperandType() == BasicOperand::LABEL);
    Assert(ins->GetFalseLabel()->GetOperandType() == BasicOperand::LABEL);
    auto true_label = (LabelOperand *)ins->GetTrueLabel();
    auto false_label = (LabelOperand *)ins->GetFalseLabel();

    auto br_ins = rvconstructor->ConstructBLabel(opcode, cmp_op1, cmp_op2,
                                                 RiscVLabel(true_label->GetLabelNo(), false_label->GetLabelNo()));

    cur_block->push_back(br_ins);
    auto br_else_ins =
    rvconstructor->ConstructJLabel(RISCV_JAL, GetPhysicalReg(RISCV_x0), RiscVLabel(false_label->GetLabelNo()));
    cur_block->push_back(br_else_ins);
}

template <> void RiscV64Selector::ConvertAndAppend<BrUncondInstruction *>(BrUncondInstruction *ins) {
    rvconstructor->DisableSchedule();
    auto dest_label = RiscVLabel(((LabelOperand *)ins->GetDestLabel())->GetLabelNo());

    auto jal_instr = rvconstructor->ConstructJLabel(RISCV_JAL, GetPhysicalReg(RISCV_x0), dest_label);

    cur_block->push_back(jal_instr);
}

template <> void RiscV64Selector::ConvertAndAppend<CallInstruction *>(CallInstruction *ins) {
rvconstructor->DisableSchedule();
Assert(ins->GetRetType() == BasicInstruction::VOID || ins->GetResult()->GetOperandType() == BasicOperand::REG);

int ireg_cnt = 0;
int freg_cnt = 0;
int stkpara_cnt = 0;
    // Parameters
for (auto [type, arg_op] : ins->GetParameterList()) {
    if (type == BasicInstruction::I32 || type == BasicInstruction::PTR) {
        if (ireg_cnt < 8) {
            if (arg_op->GetOperandType() == BasicOperand::REG) {
                auto arg_regop = (RegOperand *)arg_op;
                auto arg_reg = GetllvmReg(arg_regop->GetRegNo(), INT64);
                if (llvm_rv_allocas.find(arg_regop->GetRegNo()) == llvm_rv_allocas.end()) {
                    auto arg_copy_instr = rvconstructor->ConstructCopyReg(
                        GetPhysicalReg(RISCV_a0 + ireg_cnt), arg_reg, INT64);
                    cur_block->push_back(arg_copy_instr);
                } else {
                    auto sp_offset = llvm_rv_allocas[arg_regop->GetRegNo()];
                    cur_block->push_back(rvconstructor->ConstructIImm(
                        RISCV_ADDI, GetPhysicalReg(RISCV_a0 + ireg_cnt), GetPhysicalReg(RISCV_sp), sp_offset));
                }
            } else if (arg_op->GetOperandType() == BasicOperand::IMMI32) {
                auto arg_immop = (ImmI32Operand *)arg_op;
                auto arg_copy_instr = rvconstructor->ConstructCopyRegImmI(
                    GetPhysicalReg(RISCV_a0 + ireg_cnt), arg_immop->GetIntImmVal(), INT64);
                cur_block->push_back(arg_copy_instr);
            } else if (arg_op->GetOperandType() == BasicOperand::GLOBAL) {
                auto mid_reg = GetNewReg(INT64);
                auto arg_global = (GlobalOperand *)arg_op;
                auto global_name = arg_global->GetName();
                cur_block->push_back(rvconstructor->ConstructULabel(
                    RISCV_LUI, mid_reg, RiscVLabel(global_name, true)));
                cur_block->push_back(rvconstructor->ConstructILabel(
                    RISCV_ADDI, GetPhysicalReg(RISCV_a0 + ireg_cnt), mid_reg, RiscVLabel(global_name, false)));
            } else {
                ERROR("Unexpected Operand type");
            }
        }
        ireg_cnt++;
    }  else {
            ERROR("Unexpected parameter type %d", type);
        }
    }

    if (ireg_cnt - 8 > 0)
        stkpara_cnt += (ireg_cnt - 8);
    if (freg_cnt - 8 > 0)
        stkpara_cnt += (freg_cnt - 8);
    // int sub_sz = ((stkpara_cnt * 8 + 15)/16)*16;

    if (stkpara_cnt != 0) {
        ireg_cnt = freg_cnt = 0;
        int arg_off = 0;
        // cur_block->push_back(rvconstructor->ConstructIImm(RISCV_ADDI,GetPhysicalReg(RISCV_sp),GetPhysicalReg(RISCV_sp),-sub_sz));
        for (auto [type, arg_op] : ins->GetParameterList()) {
            if (type == BasicInstruction::I32 || type == BasicInstruction::PTR) {
                if (ireg_cnt < 8) {
                } else {
                    if (arg_op->GetOperandType() == BasicOperand::REG) {
                        auto arg_regop = (RegOperand *)arg_op;
                        auto arg_reg = GetllvmReg(arg_regop->GetRegNo(), INT64);
                        if (llvm_rv_allocas.find(arg_regop->GetRegNo()) == llvm_rv_allocas.end()) {
                            cur_block->push_back(
                            rvconstructor->ConstructSImm(RISCV_SD, arg_reg, GetPhysicalReg(RISCV_sp), arg_off));
                        } else {
                            auto sp_offset = llvm_rv_allocas[arg_regop->GetRegNo()];
                            auto mid_reg = GetNewReg(INT64);
                            cur_block->push_back(
                            rvconstructor->ConstructIImm(RISCV_ADDI, mid_reg, GetPhysicalReg(RISCV_sp), sp_offset));
                            cur_block->push_back(
                            rvconstructor->ConstructSImm(RISCV_SD, mid_reg, GetPhysicalReg(RISCV_sp), arg_off));
                        }
                    } else if (arg_op->GetOperandType() == BasicOperand::IMMI32) {
                        auto arg_immop = (ImmI32Operand *)arg_op;
                        auto arg_imm = arg_immop->GetIntImmVal();
                        auto imm_reg = GetNewReg(INT64);
                        cur_block->push_back(rvconstructor->ConstructCopyRegImmI(imm_reg, arg_imm, INT64));
                        cur_block->push_back(
                        rvconstructor->ConstructSImm(RISCV_SD, imm_reg, GetPhysicalReg(RISCV_sp), arg_off));
                    } else if (arg_op->GetOperandType() == BasicOperand::GLOBAL) {
                        auto glb_reg1 = GetNewReg(INT64);
                        auto glb_reg2 = GetNewReg(INT64);
                        auto arg_glbop = (GlobalOperand *)arg_op;
                        cur_block->push_back(
                        rvconstructor->ConstructULabel(RISCV_LUI, glb_reg1, RiscVLabel(arg_glbop->GetName(), true)));
                        cur_block->push_back(rvconstructor->ConstructILabel(RISCV_ADDI, glb_reg2, glb_reg1,
                                                                            RiscVLabel(arg_glbop->GetName(), false)));
                        cur_block->push_back(
                        rvconstructor->ConstructSImm(RISCV_SD, glb_reg2, GetPhysicalReg(RISCV_sp), arg_off));
                    }
                    arg_off += 8;
                }
                ireg_cnt++;
            } else {
                ERROR("Unexpected parameter type %d", type);
            }
        }
    }

    // Call Label
    auto call_funcname = ins->GetFunctionName();
    if (ireg_cnt > 8) {
        ireg_cnt = 8;
    }
    if (freg_cnt > 8) {
        freg_cnt = 8;
    }
    // Return Value
    auto return_type = ins->GetRetType();
    auto result_op = (RegOperand *)ins->GetResult();
    cur_block->push_back(rvconstructor->ConstructCall(RISCV_CALL, call_funcname, ireg_cnt, freg_cnt));
    cur_func->UpdateParaSize(stkpara_cnt * 8);
    // if(stkpara_cnt != 0){
    //     cur_block->push_back(rvconstructor->ConstructIImm(RISCV_ADDI,GetPhysicalReg(RISCV_sp),GetPhysicalReg(RISCV_sp),sub_sz));
    // }
    if (return_type == BasicInstruction::I32) {
        auto copy_ret_ins =
        rvconstructor->ConstructCopyReg(GetllvmReg(result_op->GetRegNo(), INT64), GetPhysicalReg(RISCV_a0), INT64);
        cur_block->push_back(copy_ret_ins);
    } 
    else if (return_type == BasicInstruction::VOID) {
    } else {
        ERROR("Unexpected return type %d", return_type);
    }

    // TODO("Implement this if you need");
}

template <>
void RiscV64Selector::ConvertAndAppend<RetInstruction *>(RetInstruction *ins) {
    rvconstructor->DisableSchedule(); // 禁用调度器

    // 如果返回值不为空，处理返回值
    if (ins->GetRetVal() != nullptr) {
        auto ret_val = ins->GetRetVal();
        switch (ret_val->GetOperandType()) {
            case BasicOperand::IMMI32: {
                // 返回值为整数立即数
                auto imm_val = static_cast<ImmI32Operand *>(ret_val)->GetIntImmVal();
                auto retcopy_instr = rvconstructor->ConstructUImm(RISCV_LI, GetPhysicalReg(RISCV_a0), imm_val);
                cur_block->push_back(retcopy_instr);
                break;
            }
            case BasicOperand::REG: {
                // 返回值为寄存器
                auto retreg_val = static_cast<RegOperand *>(ret_val);
                if (ins->GetType() == BasicInstruction::LLVMType::I32) {
                    // 整数返回值
                    auto reg = GetllvmReg(retreg_val->GetRegNo(), INT64);
                    auto retcopy_instr = rvconstructor->ConstructCopyReg(GetPhysicalReg(RISCV_a0), reg, INT64);
                    cur_block->push_back(retcopy_instr);
                }
                break;
            }
            default:
                ERROR("Unsupported operand type for return value");
        }
    }

    // 构造返回指令
    auto ret_instr = rvconstructor->ConstructIImm(RISCV_JALR, GetPhysicalReg(RISCV_x0), GetPhysicalReg(RISCV_ra), 0);

    // 设置返回类型
    if (ins->GetType() == BasicInstruction::LLVMType::I32) {
        ret_instr->setRetType(1); // 整数类型返回
    } else if (ins->GetType() == BasicInstruction::LLVMType::FLOAT32) {
        ret_instr->setRetType(2); // 浮点类型返回
    } else {
        ret_instr->setRetType(0); // 默认类型返回
    }

    // 将返回指令添加到当前块
    cur_block->push_back(ret_instr);
}


// 浮点转整数
template <> void RiscV64Selector::ConvertAndAppend<FptosiInstruction *>(FptosiInstruction *ins) {
    // TODO("Implement this if you need");
    
}

template <> void RiscV64Selector::ConvertAndAppend<SitofpInstruction *>(SitofpInstruction *ins) {
    // TODO("Implement this if you need");
    
}

// 零扩展操作
template <> void RiscV64Selector::ConvertAndAppend<ZextInstruction *>(ZextInstruction *ins) {
    // TODO("Implement this if you need");
    Assert(ins->GetSrc()->GetOperandType() == BasicOperand::REG);
    Assert(ins->GetResult()->GetOperandType() == BasicOperand::REG);

    auto ext_reg = GetllvmReg(((RegOperand *)ins->GetSrc())->GetRegNo(), INT64);
    auto cmp_ins = cmp_context[ext_reg];

    Assert(cmp_ins != nullptr);
    auto result_reg = GetllvmReg(((RegOperand *)ins->GetResult())->GetRegNo(), INT64);

    if (cmp_ins->GetOpcode() == BasicInstruction::ICMP) {
        auto icmp_ins = (IcmpInstruction *)cmp_ins;
        auto op1 = icmp_ins->GetOp1();
        auto op2 = icmp_ins->GetOp2();
        auto cur_cond = icmp_ins->GetCond();
        Register reg_1, reg_2;
        if (op1->GetOperandType() == BasicOperand::IMMI32) {
            auto t = op1;
            op1 = op2;
            op2 = t;
            switch (cur_cond) {
            case BasicInstruction::eq:
            case BasicInstruction::ne:
                break;
            case BasicInstruction::sgt:
                cur_cond = BasicInstruction::slt;
                break;
            case BasicInstruction::sge:
                cur_cond = BasicInstruction::sle;
                break;
            case BasicInstruction::slt:
                cur_cond = BasicInstruction::sgt;
                break;
            case BasicInstruction::sle:
                cur_cond = BasicInstruction::sge;
                break;
            case BasicInstruction::ugt:
            case BasicInstruction::uge:
            case BasicInstruction::ult:
            case BasicInstruction::ule:
                ERROR("Unexpected ICMP cond");
            }
        }
        if (op1->GetOperandType() == BasicOperand::IMMI32) {
            Assert(op2->GetOperandType() == BasicOperand::IMMI32);
            int rval = 0;
            int op1_val = ((ImmI32Operand *)op1)->GetIntImmVal();
            int op2_val = ((ImmI32Operand *)op2)->GetIntImmVal();
            switch (cur_cond) {
            case BasicInstruction::eq:
                rval = (op1_val == op2_val);
            case BasicInstruction::ne:
                rval = (op1_val != op2_val);
                break;
            case BasicInstruction::sgt:
                rval = (op1_val > op2_val);
                break;
            case BasicInstruction::sge:
                rval = (op1_val >= op2_val);
                break;
            case BasicInstruction::slt:
                rval = (op1_val < op2_val);
                break;
            case BasicInstruction::sle:
                rval = (op1_val <= op2_val);
                break;
            case BasicInstruction::ugt:
            case BasicInstruction::uge:
            case BasicInstruction::ult:
            case BasicInstruction::ule:
                ERROR("Unexpected ICMP cond");
            }
            cur_block->push_back(rvconstructor->ConstructCopyRegImmI(result_reg, rval, INT64));
            return;
        }
        if (op2->GetOperandType() == BasicOperand::IMMI32) {
            Assert(op1->GetOperandType() == BasicOperand::REG);
            auto op1_reg = GetllvmReg(((RegOperand *)op1)->GetRegNo(), INT64);
            if (((ImmI32Operand *)op2)->GetIntImmVal() == 0) {
                auto not_reg = GetNewReg(INT64);
                switch (cur_cond) {
                case BasicInstruction::eq:
                    cur_block->push_back(rvconstructor->ConstructIImm(RISCV_SLTIU, result_reg, op1_reg, 1));
                    return;
                case BasicInstruction::ne:
                    cur_block->push_back(
                    rvconstructor->ConstructR(RISCV_SLTU, result_reg, GetPhysicalReg(RISCV_x0), op1_reg));
                    return;
                case BasicInstruction::sgt:
                    cur_block->push_back(
                    rvconstructor->ConstructR(RISCV_SLT, result_reg, GetPhysicalReg(RISCV_x0), op1_reg));
                    return;
                case BasicInstruction::sge:    // sgez ~ not sltz
                    cur_block->push_back(
                    rvconstructor->ConstructR(RISCV_SLT, not_reg, op1_reg, GetPhysicalReg(RISCV_x0)));
                    cur_block->push_back(rvconstructor->ConstructIImm(RISCV_XORI, result_reg, not_reg, 1));
                    return;
                case BasicInstruction::slt:
                    cur_block->push_back(
                    rvconstructor->ConstructR(RISCV_SLT, result_reg, op1_reg, GetPhysicalReg(RISCV_x0)));
                    return;
                case BasicInstruction::sle:    // slez ~ not sgtz
                    cur_block->push_back(
                    rvconstructor->ConstructR(RISCV_SLT, not_reg, GetPhysicalReg(RISCV_x0), op1_reg));
                    cur_block->push_back(rvconstructor->ConstructIImm(RISCV_XORI, result_reg, not_reg, 1));
                    return;
                case BasicInstruction::ugt:
                case BasicInstruction::uge:
                case BasicInstruction::ult:
                case BasicInstruction::ule:
                default:
                    ERROR("Unexpected ICMP cond");
                }
            } else if (cur_cond == BasicInstruction::slt) {
                int op2_imm = ((ImmI32Operand *)op2)->GetIntImmVal();
                cur_block->push_back(rvconstructor->ConstructIImm(RISCV_SLTI, result_reg, op1_reg, op2_imm));
                return;
            } else if (cur_cond == BasicInstruction::ult) {
                int op2_imm = ((ImmI32Operand *)op2)->GetIntImmVal();
                cur_block->push_back(rvconstructor->ConstructIImm(RISCV_SLTIU, result_reg, op1_reg, op2_imm));
                return;
            }
            reg_2 = GetNewReg(INT64);
            cur_block->push_back(
            rvconstructor->ConstructCopyRegImmI(reg_2, ((ImmI32Operand *)op2)->GetIntImmVal(), INT64));
        }
        Assert(op1->GetOperandType() == BasicOperand::REG);
        reg_1 = GetllvmReg(((RegOperand *)op1)->GetRegNo(), INT64);
        if (op2->GetOperandType() == BasicOperand::REG) {
            reg_2 = GetllvmReg(((RegOperand *)op2)->GetRegNo(), INT64);
        }
        auto mid_reg = GetNewReg(INT64);
        switch (cur_cond) {
        case BasicInstruction::eq:
            cur_block->push_back(rvconstructor->ConstructR(RISCV_SUBW, mid_reg, reg_1, reg_2));
            cur_block->push_back(rvconstructor->ConstructIImm(RISCV_SLTIU, result_reg, mid_reg, 1));
            return;
        case BasicInstruction::ne:
            cur_block->push_back(rvconstructor->ConstructR(RISCV_SUBW, mid_reg, reg_1, reg_2));
            cur_block->push_back(rvconstructor->ConstructR(RISCV_SLTU, result_reg, GetPhysicalReg(RISCV_x0), mid_reg));
            return;
        case BasicInstruction::sgt:
            cur_block->push_back(rvconstructor->ConstructR(RISCV_SLT, result_reg, reg_2, reg_1));
            return;
        case BasicInstruction::sge:    // reg_1 >= reg_2 <==> not reg_1 < reg_2
            cur_block->push_back(rvconstructor->ConstructR(RISCV_SLT, mid_reg, reg_1, reg_2));
            cur_block->push_back(rvconstructor->ConstructIImm(RISCV_XORI, result_reg, mid_reg, 1));
            return;
        case BasicInstruction::slt:
            cur_block->push_back(rvconstructor->ConstructR(RISCV_SLT, result_reg, reg_1, reg_2));
            return;
        case BasicInstruction::sle:    // 2 < 1  2 >= 1 1 <= 2
            cur_block->push_back(rvconstructor->ConstructR(RISCV_SLT, mid_reg, reg_2, reg_1));
            cur_block->push_back(rvconstructor->ConstructIImm(RISCV_XORI, result_reg, mid_reg, 1));
            return;
        case BasicInstruction::ugt:
        case BasicInstruction::uge:
        case BasicInstruction::ult:
        case BasicInstruction::ule:
        default:
            ERROR("Unexpected ICMP cond");
        }
    } else {
        ERROR("Unexpected ins Before zext");
    }
}

template <>
void RiscV64Selector::ConvertAndAppend<GetElementptrInstruction *>(GetElementptrInstruction *ins) {
    
}


template <> void RiscV64Selector::ConvertAndAppend<PhiInstruction *>(PhiInstruction *ins) {
    Assert(ins->GetResultOp()->GetOperandType() == BasicOperand::REG);
    auto res_op = (RegOperand *)ins->GetResultOp();
    Register result_reg;
    if (ins->GetDataType() == BasicInstruction::LLVMType::I32 ||
        ins->GetDataType() == BasicInstruction::LLVMType::PTR) {
        result_reg = GetllvmReg(res_op->GetRegNo(), INT64);
    } 
    auto m_phi = new MachinePhiInstruction(result_reg);
    for (auto [label, val] : ins->GetPhiList()) {
        Assert(label->GetOperandType() == BasicOperand::LABEL);
        auto label_op = (LabelOperand *)label;
        if (val->GetOperandType() == BasicOperand::REG && ins->GetDataType() == BasicInstruction::LLVMType::I32) {
            auto reg_op = (RegOperand *)val;
            auto val_reg = GetllvmReg(reg_op->GetRegNo(), INT64);
            m_phi->pushPhiList(label_op->GetLabelNo(), val_reg);
        } else if (val->GetOperandType() == BasicOperand::IMMI32) {
            auto immi_op = (ImmI32Operand *)val;
            m_phi->pushPhiList(label_op->GetLabelNo(), immi_op->GetIntImmVal());
        } else if (val->GetOperandType() == BasicOperand::IMMF32) {
            auto immf_op = (ImmF32Operand *)val;
            m_phi->pushPhiList(label_op->GetLabelNo(), immf_op->GetFloatVal());
        } else if (val->GetOperandType() == BasicOperand::REG &&
                   ins->GetDataType() == BasicInstruction::LLVMType::PTR) {
            auto reg_op = (RegOperand *)val;
            auto val_reg = GetllvmReg(reg_op->GetRegNo(), INT64);
            m_phi->pushPhiList(label_op->GetLabelNo(), val_reg);
        } else {
            ERROR("Unexpected OperandType");
        }
    }
    cur_block->push_back(m_phi);
}

template <> void RiscV64Selector::ConvertAndAppend<Instruction>(Instruction inst) {
#ifdef ENABLE_COMMENT
    std::ostringstream oss;
    inst->PrintIR(oss);
    cur_block->push_back(rvconstructor->ConstructComment(oss.str()));
#endif

    switch (inst->GetOpcode()) {
    case BasicInstruction::LOAD:
        ConvertAndAppend<LoadInstruction *>((LoadInstruction *)inst);
        break;
    case BasicInstruction::STORE:
        ConvertAndAppend<StoreInstruction *>((StoreInstruction *)inst);
        break;
    case BasicInstruction::ADD:
    case BasicInstruction::SUB:
    case BasicInstruction::MUL:
    case BasicInstruction::DIV:
    case BasicInstruction::FADD:
    case BasicInstruction::FSUB:
    case BasicInstruction::FMUL:
    case BasicInstruction::FDIV:
    case BasicInstruction::MOD:
    case BasicInstruction::SHL:
    case BasicInstruction::BITXOR:
        ConvertAndAppend<ArithmeticInstruction *>((ArithmeticInstruction *)inst);
        break;
    case BasicInstruction::ICMP:
        ConvertAndAppend<IcmpInstruction *>((IcmpInstruction *)inst);
        break;
    case BasicInstruction::FCMP:
        ConvertAndAppend<FcmpInstruction *>((FcmpInstruction *)inst);
        break;
    case BasicInstruction::ALLOCA:
        ConvertAndAppend<AllocaInstruction *>((AllocaInstruction *)inst);
        break;
    case BasicInstruction::BR_COND:
        ConvertAndAppend<BrCondInstruction *>((BrCondInstruction *)inst);
        break;
    case BasicInstruction::BR_UNCOND:
        ConvertAndAppend<BrUncondInstruction *>((BrUncondInstruction *)inst);
        break;
    case BasicInstruction::RET:
        ConvertAndAppend<RetInstruction *>((RetInstruction *)inst);
        break;
    case BasicInstruction::ZEXT:
        ConvertAndAppend<ZextInstruction *>((ZextInstruction *)inst);
        break;
    case BasicInstruction::FPTOSI:
        ConvertAndAppend<FptosiInstruction *>((FptosiInstruction *)inst);
        break;
    case BasicInstruction::SITOFP:
        ConvertAndAppend<SitofpInstruction *>((SitofpInstruction *)inst);
        break;
    case BasicInstruction::GETELEMENTPTR:
        ConvertAndAppend<GetElementptrInstruction *>((GetElementptrInstruction *)inst);
        break;
    case BasicInstruction::CALL:
        ConvertAndAppend<CallInstruction *>((CallInstruction *)inst);
        break;
    case BasicInstruction::PHI:
        ConvertAndAppend<PhiInstruction *>((PhiInstruction *)inst);
        break;
    default:
        ERROR("Unknown LLVM IR instruction");
    }
}
// Reference: https://github.com/yuhuifishash/NKU-Compilers2024-RV64GC.git/target/riscv64gc/instruction_select/riscv64_instSelect.cc line 1778-1852
void RiscV64Selector::SelectInstructionAndBuildCFG() {
    // 将全局定义从中间表示复制到目标
    dest->global_def = IR->global_def;

    // 遍历每个LLVM IR函数
    for (auto &[defI, cfg] : IR->llvm_cfg) {
        if (!cfg) {
            ERROR("LLVMIR CFG is Empty, you should implement BuildCFG in MidEnd first");
        }

        // 初始化函数
        std::string function_name = cfg->function_def->GetFunctionName();
        cur_func = new RiscV64Function(function_name);
        cur_func->SetParent(dest);
        dest->functions.push_back(cur_func);

        // 初始化Machine CFG
        auto cur_mcfg = new MachineCFG;
        cur_func->SetMachineCFG(cur_mcfg);

        // 清空指令选择状态
        ClearFunctionSelectState();

        // 添加函数参数
        for (int i = 0; i < defI->GetFormalSize(); i++) {
            auto formal_reg = defI->formals_reg[i];
            Assert(formal_reg->GetOperandType() == BasicOperand::REG);

            MachineDataType type;
            switch (defI->formals[i]) {
                case BasicInstruction::LLVMType::I32:
                case BasicInstruction::LLVMType::PTR:
                    type = INT64;
                    break;
                default:
                    ERROR("Unsupported LLVM type for function parameter");
            }

            cur_func->AddParameter(GetllvmReg(static_cast<RegOperand *>(formal_reg)->GetRegNo(), type));
        }

        // 遍历基本块
        for (const auto &[block_id, block] : *(cfg->block_map)) {
            cur_block = new RiscV64Block(block_id);
            cur_mcfg->AssignEmptyNode(block_id, cur_block);
            cur_func->UpdateMaxLabel(block_id);

            cur_block->setParent(cur_func);
            cur_func->blocks.push_back(cur_block);

            for (auto instruction : block->Instruction_list) {
                ConvertAndAppend<Instruction>(instruction);
            }
        }

        // 对齐栈帧到8字节边界
        if (cur_offset % 8 != 0) {
            cur_offset = ((cur_offset + 7) / 8) * 8;
        }

        // 设置函数的堆栈大小
        cur_func->SetStackSize(cur_offset + cur_func->GetParaSize());

        // 构建控制流图边
        for (size_t i = 0; i < cfg->G.size(); i++) {
            for (const auto &arc : cfg->G[i]) {
                cur_mcfg->MakeEdge(i, arc->block_id);
            }
        }
    }
}

void RiscV64Selector::ClearFunctionSelectState() {
    llvm_rv_regtable.clear();
    llvm_rv_allocas.clear();
    cmp_context.clear();
    cur_offset = 0;
}
