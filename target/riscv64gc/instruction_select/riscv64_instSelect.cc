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
    if(ins->GetPointer()->GetOperandType()==BasicOperand::REG){
    auto ptr_op = (RegOperand *)ins->GetPointer();
    auto rd_op = (RegOperand *)ins->GetResult();
 
    if (ins->GetDataType() == BasicInstruction::LLVMType::I32||ins->GetDataType() == BasicInstruction::LLVMType::PTR) {
            Register rd = GetllvmReg(rd_op->GetRegNo(), INT64);
            if (llvm_rv_allocas.find(ptr_op->GetRegNo()) == llvm_rv_allocas.end()) {
                Register ptr = GetllvmReg(ptr_op->GetRegNo(), INT64);  
                auto lw_instr = rvconstructor->ConstructIImm(RISCV_LW, rd, ptr, 0);

                if(ins->GetDataType() == BasicInstruction::LLVMType::PTR)
                auto lw_instr = rvconstructor->ConstructIImm(RISCV_LD, rd, ptr, 0);

                cur_block->push_back(lw_instr);
            } else {
                auto sp_offset = llvm_rv_allocas[ptr_op->GetRegNo()];
                auto lw_instr = rvconstructor->ConstructIImm(RISCV_LW, rd, GetPhysicalReg(RISCV_sp), sp_offset);

                if(ins->GetDataType() == BasicInstruction::LLVMType::PTR)
                auto lw_instr = rvconstructor->ConstructIImm(RISCV_LD, rd, GetPhysicalReg(RISCV_sp), sp_offset);

                ((RiscV64Function *)cur_func)->AddAllocaIns(lw_instr);
                cur_block->push_back(lw_instr);
            }
        } else if (ins->GetDataType() == BasicInstruction::LLVMType::FLOAT32) {
            Register rd = GetllvmReg(rd_op->GetRegNo(), FLOAT64);
            if (llvm_rv_allocas.find(ptr_op->GetRegNo()) == llvm_rv_allocas.end()) {
                Register ptr = GetllvmReg(ptr_op->GetRegNo(), INT64);   
                auto lw_instr = rvconstructor->ConstructIImm(RISCV_FLW, rd, ptr, 0);
                cur_block->push_back(lw_instr);
            } else {
                auto sp_offset = llvm_rv_allocas[ptr_op->GetRegNo()];
                auto lw_instr = rvconstructor->ConstructIImm(RISCV_FLW, rd, GetPhysicalReg(RISCV_sp), sp_offset);
                ((RiscV64Function *)cur_func)->AddAllocaIns(lw_instr);
                cur_block->push_back(lw_instr);
            }
        } else {
            ERROR("Unexpected data type");
        }

    }else if(ins->GetPointer()->GetOperandType()==BasicOperand::GLOBAL){
        // lui %r0, %hi(x)  加载x的高位，并且左移，将低16位清零
        // lw  %rd, %lo(x)(%r0) 低16位清零后加上低位的十六位得到地址
        auto global_op = (GlobalOperand *)ins->GetPointer();
        auto rd_op = (RegOperand *)ins->GetResult();

        Register addr_hi = GetNewReg(INT64);

        if (ins->GetDataType() == BasicInstruction::LLVMType::I32||ins->GetDataType() == BasicInstruction::LLVMType::PTR) {
            Register rd = GetllvmReg(rd_op->GetRegNo(), INT64);

            auto lui_instr = rvconstructor->ConstructULabel(RISCV_LUI, addr_hi, RiscVLabel(global_op->GetName(), true));
            auto lw_instr =
            rvconstructor->ConstructILabel(RISCV_LW, rd, addr_hi, RiscVLabel(global_op->GetName(), false));
            
            if(ins->GetDataType() == BasicInstruction::LLVMType::PTR)
            auto lw_instr =rvconstructor->ConstructILabel(RISCV_LD, rd, addr_hi, RiscVLabel(global_op->GetName(), false));

            cur_block->push_back(lui_instr);
            cur_block->push_back(lw_instr);
        } else if (ins->GetDataType() == BasicInstruction::LLVMType::FLOAT32) {
            Register rd = GetllvmReg(rd_op->GetRegNo(), FLOAT64);

            auto lui_instr = rvconstructor->ConstructULabel(RISCV_LUI, addr_hi, RiscVLabel(global_op->GetName(), true));
            auto lw_instr =
            rvconstructor->ConstructILabel(RISCV_FLW, rd, addr_hi, RiscVLabel(global_op->GetName(), false));

            cur_block->push_back(lui_instr);
            cur_block->push_back(lw_instr);
        }  else {
            ERROR("Unexpected data type");
        }
    }
}

template <> void RiscV64Selector::ConvertAndAppend<StoreInstruction *>(StoreInstruction *ins) {
    Register value_reg;
    if (ins->GetValue()->GetOperandType() == BasicOperand::IMMI32) {
        auto val_imm = (ImmI32Operand *)ins->GetValue();

        value_reg = GetNewReg(INT64);

        auto imm_copy_ins = rvconstructor->ConstructCopyRegImmI(value_reg, val_imm->GetIntImmVal(), INT64);
        cur_block->push_back(imm_copy_ins);

    } else if (ins->GetValue()->GetOperandType() == BasicOperand::REG) {
        auto val_reg = (RegOperand *)ins->GetValue();
        if (ins->GetDataType() == BasicInstruction::LLVMType::I32) {
            value_reg = GetllvmReg(val_reg->GetRegNo(), INT64);
        } else if (ins->GetDataType() == BasicInstruction::LLVMType::FLOAT32) {
            value_reg = GetllvmReg(val_reg->GetRegNo(), FLOAT64);
        } else {
            ERROR("Unexpected data type");
        }
    } else if (ins->GetValue()->GetOperandType() == BasicOperand::IMMF32) {
        
        auto val_imm = (ImmF32Operand *)ins->GetValue();

        value_reg = GetNewReg(FLOAT64);

        auto imm_copy_ins = rvconstructor->ConstructCopyRegImmF(value_reg, val_imm->GetFloatVal(), FLOAT64);
        float val = val_imm->GetFloatVal();

        
        cur_block->push_back(imm_copy_ins);
    } else {
        ERROR("Unexpected or unimplemented operand type");
    }


     if (ins->GetPointer()->GetOperandType() == BasicOperand::REG) {
        // Lazy("Deal with alloca later");
        auto reg_ptr_op = (RegOperand *)ins->GetPointer();

        if (ins->GetDataType() == BasicInstruction::LLVMType::I32||ins->GetDataType() == BasicInstruction::LLVMType::FLOAT32) {
            if (llvm_rv_allocas.find(reg_ptr_op->GetRegNo()) == llvm_rv_allocas.end()) {
                auto ptr_reg = GetllvmReg(reg_ptr_op->GetRegNo(), INT64);

                auto store_instruction = rvconstructor->ConstructSImm(RISCV_SW, value_reg, ptr_reg, 0);

                if(ins->GetDataType() == BasicInstruction::LLVMType::FLOAT32)
                auto store_instruction = rvconstructor->ConstructSImm(RISCV_FSW, value_reg, ptr_reg, 0);
                
                cur_block->push_back(store_instruction);
            } else {
                auto sp_offset = llvm_rv_allocas[reg_ptr_op->GetRegNo()];

                auto store_instruction =
                rvconstructor->ConstructSImm(RISCV_SW, value_reg, GetPhysicalReg(RISCV_sp), sp_offset);

                if(ins->GetDataType() == BasicInstruction::LLVMType::FLOAT32)
                auto store_instruction =rvconstructor->ConstructSImm(RISCV_FSW, value_reg, GetPhysicalReg(RISCV_sp), sp_offset);

                ((RiscV64Function *)cur_func)->AddAllocaIns(store_instruction);
                cur_block->push_back(store_instruction);
            }
        }  else {
            ERROR("Unexpected data type");
        }

    } else if (ins->GetPointer()->GetOperandType() == BasicOperand::GLOBAL) {
        auto global_op = (GlobalOperand *)ins->GetPointer();

        auto addr_hi = GetNewReg(INT64);

        auto lui_instruction =
        rvconstructor->ConstructULabel(RISCV_LUI, addr_hi, RiscVLabel(global_op->GetName(), true));
        cur_block->push_back(lui_instruction);

        if (ins->GetDataType() == BasicInstruction::LLVMType::I32) {
            auto store_instruction =
            rvconstructor->ConstructSLabel(RISCV_SW, value_reg, addr_hi, RiscVLabel(global_op->GetName(), false));
            cur_block->push_back(store_instruction);
        } else if (ins->GetDataType() == BasicInstruction::LLVMType::FLOAT32) {
            auto store_instruction =
            rvconstructor->ConstructSLabel(RISCV_FSW, value_reg, addr_hi, RiscVLabel(global_op->GetName(), false));
            cur_block->push_back(store_instruction);
        } else {
            ERROR("Unexpected data type");
        }
    }
}
Register RiscV64Selector::ExtractOp2Reg(BasicOperand *op, MachineDataType type) {
    if (op->GetOperandType() == BasicOperand::IMMI32) {
        Assert(type == INT64);
        Register ret = GetNewReg(INT64);
        cur_block->push_back(rvconstructor->ConstructCopyRegImmI(ret, ((ImmI32Operand *)op)->GetIntImmVal(), INT64));
        return ret;
    } else if (op->GetOperandType() == BasicOperand::IMMF32) {
        Assert(type == FLOAT64);
        Register ret = GetNewReg(FLOAT64);
        cur_block->push_back(rvconstructor->ConstructCopyRegImmF(ret, ((ImmF32Operand *)op)->GetFloatVal(), FLOAT64));
        return ret;
    } else if (op->GetOperandType() == BasicOperand::REG) {
        return GetllvmReg(((RegOperand *)op)->GetRegNo(), type);
    } else {
        ERROR("Unexpected op type");
    }
    return Register();
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

template <> void RiscV64Selector::ConvertAndAppend<ArithmeticInstruction *>(ArithmeticInstruction *ins) {
    // TODO("Implement this if you need");
    //加法
    //printf("%d",5);
    if(ins->GetOpcode() == BasicInstruction::ADD) {
        //整數
        
        if (ins->GetDataType() == BasicInstruction::I32) {
            //立即數+立即數
            //printf("%d",9);
            if (ins->GetOperand1()->GetOperandType() == BasicOperand::IMMI32 &&
                ins->GetOperand2()->GetOperandType() == BasicOperand::IMMI32) {
                // auto *imm1_op = (ImmI32Operand *)ins->GetOperand1();
                // auto *imm2_op = (ImmI32Operand *)ins->GetOperand2();
                // auto imm1 = imm1_op->GetIntImmVal();
                // auto imm2 = imm2_op->GetIntImmVal();
                // printf("%d,%d",imm1,imm2);
                // auto *rd_op = (RegOperand *)ins->GetResultOperand();
                // auto r1=GetllvmReg(-1, INT64);
                // auto r2=GetllvmReg(-1, INT64);
                // auto rd = GetllvmReg(rd_op->GetRegNo(), INT64);
                // auto r1_instr = rvconstructor->ConstructUImm(RISCV_LI, r1, imm1);
                // auto r2_instr = rvconstructor->ConstructUImm(RISCV_LI, r2, imm2);
                // auto retcopy_instr = rvconstructor->ConstructR(RISCV_ADD, GetPhysicalReg(RISCV_a0), r1, r2);
                // //auto copy_imm_instr = rvconstructor->ConstructCopyRegImmI(rd, imm1 + imm2, INT64);
                // cur_block->push_back(r1_instr);
                // cur_block->push_back(r2_instr);
                // cur_block->push_back(retcopy_instr);
                auto *imm1_op = (ImmI32Operand *)ins->GetOperand1();
                auto *imm2_op = (ImmI32Operand *)ins->GetOperand2();
                auto *rd_op = (RegOperand *)ins->GetResultOperand();

                auto imm1 = imm1_op->GetIntImmVal();
                auto imm2 = imm2_op->GetIntImmVal();
                auto rd = GetllvmReg(rd_op->GetRegNo(), INT64);

                auto copy_imm_instr = rvconstructor->ConstructCopyRegImmI(rd, imm1 + imm2, INT64);
                cur_block->push_back(copy_imm_instr);
            // } else if (ins->GetOperand1()->GetOperandType() == BasicOperand::REG &&
            //            ins->GetOperand2()->GetOperandType() == BasicOperand::REG) {
            //     auto *reg1_op = (RegOperand *)ins->GetOperand1();
            //     auto *reg2_op = (RegOperand *)ins->GetOperand2();
            //     auto reg1 = reg1_op->GetRegVal();
            //     auto reg2 = reg2_op->GetRegVal();
            //     auto retcopy_instr = rvconstructor->ConstructIImm(RISCV_ADDI, GetPhysicalReg(RISCV_a0), reg1, reg2);
            //     cur_block->push_back(retcopy_instr);
            // }
            } 
            //寄存器+立即数
            if (ins->GetOperand1()->GetOperandType() == BasicOperand::REG &&
                ins->GetOperand2()->GetOperandType() == BasicOperand::IMMI32) {
                Assert(ins->GetResultOperand()->GetOperandType() == BasicOperand::REG);

                auto *rd_op = (RegOperand *)ins->GetResultOperand();
                auto *rs_op = (RegOperand *)ins->GetOperand1();
                auto *i_op = (ImmI32Operand *)ins->GetOperand2();

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
            //寄存器+寄存器
            if (ins->GetOperand1()->GetOperandType() == BasicOperand::REG &&
                ins->GetOperand2()->GetOperandType() == BasicOperand::REG) {
                //printf("%d",9);
                Assert(ins->GetResultOperand()->GetOperandType() == BasicOperand::REG);
                auto *rd_op = (RegOperand *)ins->GetResultOperand();
                auto *rs_op = (RegOperand *)ins->GetOperand1();
                auto *rt_op = (RegOperand *)ins->GetOperand2();

                auto rd = GetllvmReg(rd_op->GetRegNo(), INT64);
                auto rs = GetllvmReg(rs_op->GetRegNo(), INT64);
                auto rt = GetllvmReg(rt_op->GetRegNo(), INT64);

                auto addw_instr = rvconstructor->ConstructR(RISCV_ADDW, rd, rs, rt);

                cur_block->push_back(addw_instr);
            }
            //立即数+寄存器
            if (ins->GetOperand2()->GetOperandType() == BasicOperand::REG &&
                ins->GetOperand1()->GetOperandType() == BasicOperand::IMMI32) {
                Assert(ins->GetResultOperand()->GetOperandType() == BasicOperand::REG);

                auto *rd_op = (RegOperand *)ins->GetResultOperand();
                auto *rs_op = (RegOperand *)ins->GetOperand2();
                auto *i_op = (ImmI32Operand *)ins->GetOperand1();

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
        else {

        }
    }

    //减法
    if(ins->GetOpcode() == BasicInstruction::SUB) {
        //立即数-立即数
        if (ins->GetOperand1()->GetOperandType() == BasicOperand::IMMI32 &&
            ins->GetOperand2()->GetOperandType() == BasicOperand::IMMI32) {
            Assert(ins->GetResultOperand()->GetOperandType() == BasicOperand::REG);
            auto *rd_op = (RegOperand *)ins->GetResultOperand();
            auto *imm1_op = (ImmI32Operand *)ins->GetOperand1();
            auto *imm2_op = (ImmI32Operand *)ins->GetOperand2();

            auto imm1 = imm1_op->GetIntImmVal();
            auto imm2 = imm2_op->GetIntImmVal();
            auto rd = GetllvmReg(rd_op->GetRegNo(), INT64);

            auto copy_imm_instr = rvconstructor->ConstructCopyRegImmI(rd, imm1 - imm2, INT64);
            cur_block->push_back(copy_imm_instr);
        } 
        //寄存器-立即数，使用 ADDIW
        else if (ins->GetOperand1()->GetOperandType() == BasicOperand::REG &&
                   ins->GetOperand2()->GetOperandType() == BasicOperand::IMMI32) {
            Assert(ins->GetResultOperand()->GetOperandType() == BasicOperand::REG);
            Register rd = GetllvmReg(((RegOperand *)ins->GetResultOperand())->GetRegNo(), INT64);
            Register sub1 = GetllvmReg(((RegOperand *)ins->GetOperand1())->GetRegNo(), INT64);
            int imm = ((ImmI32Operand *)ins->GetOperand2())->GetIntImmVal();
            cur_block->push_back(rvconstructor->ConstructIImm(RISCV_ADDIW, rd, sub1, -imm));
        } 
        //SUBW
        else {
            Assert(ins->GetResultOperand()->GetOperandType() == BasicOperand::REG);
            Register rd = GetllvmReg(((RegOperand *)ins->GetResultOperand())->GetRegNo(), INT64);
            Register sub1 = ExtractOp2Reg(ins->GetOperand1(), INT64);
            Register sub2 = ExtractOp2Reg(ins->GetOperand2(), INT64);
            auto sub_instr = rvconstructor->ConstructR(RISCV_SUBW, rd, sub1, sub2);
            cur_block->push_back(sub_instr);
        }
    }

    //乘法
    else if (ins->GetOpcode() == BasicInstruction::MUL) {
        if (ins->GetOperand1()->GetOperandType() == BasicOperand::IMMI32 &&
            ins->GetOperand2()->GetOperandType() == BasicOperand::IMMI32) {
            Assert(ins->GetResultOperand()->GetOperandType() == BasicOperand::REG);
            auto *rd_op = (RegOperand *)ins->GetResultOperand();
            auto *imm1_op = (ImmI32Operand *)ins->GetOperand1();
            auto *imm2_op = (ImmI32Operand *)ins->GetOperand2();

            auto imm1 = imm1_op->GetIntImmVal();
            auto imm2 = imm2_op->GetIntImmVal();
            auto rd = GetllvmReg(rd_op->GetRegNo(), INT64);

            auto copy_imm_instr = rvconstructor->ConstructCopyRegImmI(rd, imm1 * imm2, INT64);
            cur_block->push_back(copy_imm_instr);
        } 
        //将立即数转换为寄存器
        else {
            Assert(ins->GetResultOperand()->GetOperandType() == BasicOperand::REG);
            Register rd = GetllvmReg(((RegOperand *)ins->GetResultOperand())->GetRegNo(), INT64);
            Register mul1, mul2;
            if (ins->GetOperand1()->GetOperandType() == BasicOperand::IMMI32) {
                mul1 = GetNewReg(INT64);
                auto copy_imm_instr =
                rvconstructor->ConstructCopyRegImmI(mul1, ((ImmI32Operand *)ins->GetOperand1())->GetIntImmVal(), INT64);
                cur_block->push_back(copy_imm_instr);
            } else if (ins->GetOperand1()->GetOperandType() == BasicOperand::REG) {
                mul1 = GetllvmReg(((RegOperand *)ins->GetOperand1())->GetRegNo(), INT64);
            } else {
                ERROR("Unexpected op type");
            }

            if (ins->GetOperand2()->GetOperandType() == BasicOperand::IMMI32) {
                mul2 = GetNewReg(INT64);
                auto copy_imm_instr =
                rvconstructor->ConstructCopyRegImmI(mul2, ((ImmI32Operand *)ins->GetOperand2())->GetIntImmVal(), INT64);
                cur_block->push_back(copy_imm_instr);
            } else if (ins->GetOperand2()->GetOperandType() == BasicOperand::REG) {
                mul2 = GetllvmReg(((RegOperand *)ins->GetOperand2())->GetRegNo(), INT64);
            } else {
                ERROR("Unexpected op type");
            }
            auto mul_instr = rvconstructor->ConstructR(RISCV_MULW, rd, mul1, mul2);
            cur_block->push_back(mul_instr);
        }
    }

    //除法
    else if (ins->GetOpcode() == BasicInstruction::DIV) {
        if (ins->GetOperand1()->GetOperandType() == BasicOperand::IMMI32 &&
            ins->GetOperand2()->GetOperandType() == BasicOperand::IMMI32) {
            Assert(ins->GetResultOperand()->GetOperandType() == BasicOperand::REG);
            auto *rd_op = (RegOperand *)ins->GetResultOperand();
            auto *imm1_op = (ImmI32Operand *)ins->GetOperand1();
            auto *imm2_op = (ImmI32Operand *)ins->GetOperand2();

            auto imm1 = imm1_op->GetIntImmVal();
            auto imm2 = imm2_op->GetIntImmVal();
            auto rd = GetllvmReg(rd_op->GetRegNo(), INT64);

            Assert(imm2 != 0);//除数不为零
            auto copy_imm_instr = rvconstructor->ConstructCopyRegImmI(rd, imm1 / imm2, INT64);
            cur_block->push_back(copy_imm_instr);
        } 
        else {
            Assert(ins->GetResultOperand()->GetOperandType() == BasicOperand::REG);
            Register rd = GetllvmReg(((RegOperand *)ins->GetResultOperand())->GetRegNo(), INT64);
            Register div1, div2;
            if (ins->GetOperand1()->GetOperandType() == BasicOperand::IMMI32) {
                div1 = GetNewReg(INT64);
                auto copy_imm_instr =
                rvconstructor->ConstructCopyRegImmI(div1, ((ImmI32Operand *)ins->GetOperand1())->GetIntImmVal(), INT64);
                cur_block->push_back(copy_imm_instr);
            } else if (ins->GetOperand1()->GetOperandType() == BasicOperand::REG) {
                div1 = GetllvmReg(((RegOperand *)ins->GetOperand1())->GetRegNo(), INT64);
            } else {
                ERROR("Unexpected op type");
            }

            if (ins->GetOperand2()->GetOperandType() == BasicOperand::IMMI32) {
                div2 = GetNewReg(INT64);
                auto copy_imm_instr =
                rvconstructor->ConstructCopyRegImmI(div2, ((ImmI32Operand *)ins->GetOperand2())->GetIntImmVal(), INT64);
                cur_block->push_back(copy_imm_instr);
            } else if (ins->GetOperand2()->GetOperandType() == BasicOperand::REG) {
                div2 = GetllvmReg(((RegOperand *)ins->GetOperand2())->GetRegNo(), INT64);
            } else {
                ERROR("Unexpected op type");
            }
            auto div_instr = rvconstructor->ConstructR(RISCV_DIVW, rd, div1, div2);
            cur_block->push_back(div_instr);
        }
    }

    //模
    else if (ins->GetOpcode() == BasicInstruction::MOD) {
        if (ins->GetOperand1()->GetOperandType() == BasicOperand::IMMI32 &&
            ins->GetOperand2()->GetOperandType() == BasicOperand::IMMI32) {
            Assert(ins->GetResultOperand()->GetOperandType() == BasicOperand::REG);
            auto *rd_op = (RegOperand *)ins->GetResultOperand();
            auto *imm1_op = (ImmI32Operand *)ins->GetOperand1();
            auto *imm2_op = (ImmI32Operand *)ins->GetOperand2();

            auto imm1 = imm1_op->GetIntImmVal();
            auto imm2 = imm2_op->GetIntImmVal();
            auto rd = GetllvmReg(rd_op->GetRegNo(), INT64);

            Assert(imm2 != 0);
            auto copy_imm_instr = rvconstructor->ConstructCopyRegImmI(rd, imm1 % imm2, INT64);
            cur_block->push_back(copy_imm_instr);
        } else {
            Assert(ins->GetResultOperand()->GetOperandType() == BasicOperand::REG);
            Register rd = GetllvmReg(((RegOperand *)ins->GetResultOperand())->GetRegNo(), INT64);
            Register rem1, rem2;
            if (ins->GetOperand1()->GetOperandType() == BasicOperand::IMMI32) {
                rem1 = GetNewReg(INT64);
                auto copy_imm_instr =
                rvconstructor->ConstructCopyRegImmI(rem1, ((ImmI32Operand *)ins->GetOperand1())->GetIntImmVal(), INT64);
                cur_block->push_back(copy_imm_instr);
            } else if (ins->GetOperand1()->GetOperandType() == BasicOperand::REG) {
                rem1 = GetllvmReg(((RegOperand *)ins->GetOperand1())->GetRegNo(), INT64);
            } else {
                ERROR("Unexpected op type");
            }
            if (ins->GetOperand2()->GetOperandType() == BasicOperand::IMMI32) {
                rem2 = GetNewReg(INT64);
                auto copy_imm_instr =
                rvconstructor->ConstructCopyRegImmI(rem2, ((ImmI32Operand *)ins->GetOperand2())->GetIntImmVal(), INT64);
                cur_block->push_back(copy_imm_instr);
            } else if (ins->GetOperand2()->GetOperandType() == BasicOperand::REG) {
                rem2 = GetllvmReg(((RegOperand *)ins->GetOperand2())->GetRegNo(), INT64);
            } else {
                ERROR("Unexpected op type");
            }
            auto rem_instr = rvconstructor->ConstructR(RISCV_REMW, rd, rem1, rem2);
            cur_block->push_back(rem_instr);
        }
    }
    
    //浮点加法
    else if (ins->GetOpcode() == BasicInstruction::FADD) {
        if (ins->GetOperand1()->GetOperandType() == BasicOperand::IMMF32 &&
            ins->GetOperand2()->GetOperandType() == BasicOperand::IMMF32) {
            Assert(ins->GetResultOperand()->GetOperandType() == BasicOperand::REG);
            auto *rd_op = (RegOperand *)ins->GetResultOperand();
            auto *imm1_op = (ImmF32Operand *)ins->GetOperand1();
            auto *imm2_op = (ImmF32Operand *)ins->GetOperand2();

            auto imm1 = imm1_op->GetFloatVal();
            auto imm2 = imm2_op->GetFloatVal();
            auto rd = GetllvmReg(rd_op->GetRegNo(), FLOAT64);

            auto copy_imm_instr = rvconstructor->ConstructCopyRegImmF(rd, imm1 + imm2, FLOAT64);
            cur_block->push_back(copy_imm_instr);
            // float val = imm1 + imm2;

            // auto inter_reg = GetNewReg(INT64);
            // cur_block->push_back(rvconstructor->ConstructCopyRegImmI(inter_reg, *(int *)&val, INT64));
            // cur_block->push_back(rvconstructor->ConstructR2(RISCV_FMV_W_X, rd, inter_reg));
        } else {
            Assert(ins->GetResultOperand()->GetOperandType() == BasicOperand::REG);
            Register rd = GetllvmReg(((RegOperand *)ins->GetResultOperand())->GetRegNo(), FLOAT64);
            Register fadd1 = ExtractOp2Reg(ins->GetOperand1(), FLOAT64);
            Register fadd2 = ExtractOp2Reg(ins->GetOperand2(), FLOAT64);
            auto fadd_instr = rvconstructor->ConstructR(RISCV_FADD_S, rd, fadd1, fadd2);
            cur_block->push_back(fadd_instr);
        }
    } 
    //浮点减法
    else if (ins->GetOpcode() == BasicInstruction::FSUB) {
        if (ins->GetOperand1()->GetOperandType() == BasicOperand::IMMF32 &&
            ins->GetOperand2()->GetOperandType() == BasicOperand::IMMF32) {
            Assert(ins->GetResultOperand()->GetOperandType() == BasicOperand::REG);
            auto *rd_op = (RegOperand *)ins->GetResultOperand();
            auto *imm1_op = (ImmF32Operand *)ins->GetOperand1();
            auto *imm2_op = (ImmF32Operand *)ins->GetOperand2();

            auto imm1 = imm1_op->GetFloatVal();
            auto imm2 = imm2_op->GetFloatVal();
            auto rd = GetllvmReg(rd_op->GetRegNo(), FLOAT64);

            auto copy_imm_instr = rvconstructor->ConstructCopyRegImmF(rd, imm1 - imm2, FLOAT64);
            cur_block->push_back(copy_imm_instr);
            // float val = imm1 - imm2;

            // auto inter_reg = GetNewReg(INT64);
            // cur_block->push_back(rvconstructor->ConstructCopyRegImmI(inter_reg, *(int *)&val, INT64));
            // cur_block->push_back(rvconstructor->ConstructR2(RISCV_FMV_W_X, rd, inter_reg));
        } else {
            Assert(ins->GetResultOperand()->GetOperandType() == BasicOperand::REG);
            Register rd = GetllvmReg(((RegOperand *)ins->GetResultOperand())->GetRegNo(), FLOAT64);
            if (ins->GetOperand1()->GetOperandType() == BasicOperand::IMMF32) {
                float op1_val = ExtractOp2ImmF32(ins->GetOperand1());
                //第一个数为0，使用第二个数取负指令RISCV_FNEG_S
                if (op1_val == 0) {
                    // Log("Neg");
                    Assert(ins->GetOperand2()->GetOperandType() == BasicOperand::REG);
                    cur_block->push_back(
                    rvconstructor->ConstructR2(RISCV_FNEG_S, rd, ExtractOp2Reg(ins->GetOperand2(), FLOAT64)));
                    return;
                }
            } else if (ins->GetOperand2()->GetOperandType() == BasicOperand::IMMF32) {
                float op2_val = ExtractOp2ImmF32(ins->GetOperand2());
                //第二个数为0，拷贝第一个数的值
                if (op2_val == 0) {
                    // Log("sub 0");
                    Assert(ins->GetOperand1()->GetOperandType() == BasicOperand::REG);
                    cur_block->push_back(
                    rvconstructor->ConstructCopyReg(rd, ExtractOp2Reg(ins->GetOperand1(), FLOAT64), FLOAT64));
                    return;
                }
            }
            Register fsub1, fsub2;
            if (ins->GetOperand1()->GetOperandType() == BasicOperand::IMMF32) {
                fsub1 = GetNewReg(FLOAT64);
                auto copy_imm_instr = rvconstructor->ConstructCopyRegImmF(
                fsub1, ((ImmF32Operand *)ins->GetOperand1())->GetFloatVal(), FLOAT64);
                // float val = ((ImmF32Operand *)ins->GetOperand1())->GetFloatVal();

                // auto inter_reg = GetNewReg(INT64);
                // cur_block->push_back(rvconstructor->ConstructCopyRegImmI(inter_reg, *(int *)&val, INT64));
                // cur_block->push_back(rvconstructor->ConstructR2(RISCV_FMV_W_X, fsub1, inter_reg));
                cur_block->push_back(copy_imm_instr);
            } else if (ins->GetOperand1()->GetOperandType() == BasicOperand::REG) {
                fsub1 = GetllvmReg(((RegOperand *)ins->GetOperand1())->GetRegNo(), FLOAT64);
            } else {
                ERROR("Unexpected op type");
            }
            if (ins->GetOperand2()->GetOperandType() == BasicOperand::IMMF32) {
                fsub2 = GetNewReg(FLOAT64);
                auto copy_imm_instr = rvconstructor->ConstructCopyRegImmF(
                fsub2, ((ImmF32Operand *)ins->GetOperand2())->GetFloatVal(), FLOAT64);
                // float val = ((ImmF32Operand *)ins->GetOperand2())->GetFloatVal();

                // auto inter_reg = GetNewReg(INT64);
                // cur_block->push_back(rvconstructor->ConstructCopyRegImmI(inter_reg, *(int *)&val, INT64));
                // cur_block->push_back(rvconstructor->ConstructR2(RISCV_FMV_W_X, fsub2, inter_reg));
                cur_block->push_back(copy_imm_instr);
            } else if (ins->GetOperand2()->GetOperandType() == BasicOperand::REG) {
                fsub2 = GetllvmReg(((RegOperand *)ins->GetOperand2())->GetRegNo(), FLOAT64);
            } else {
                ERROR("Unexpected op type");
            }
            auto fsub_instr = rvconstructor->ConstructR(RISCV_FSUB_S, rd, fsub1, fsub2);
            cur_block->push_back(fsub_instr);
        }
    }
    //浮点乘法
    else if (ins->GetOpcode() == BasicInstruction::FMUL) {
        if (ins->GetOperand1()->GetOperandType() == BasicOperand::IMMF32 &&
            ins->GetOperand2()->GetOperandType() == BasicOperand::IMMF32) {
            Assert(ins->GetResultOperand()->GetOperandType() == BasicOperand::REG);
            auto *rd_op = (RegOperand *)ins->GetResultOperand();
            auto *imm1_op = (ImmF32Operand *)ins->GetOperand1();
            auto *imm2_op = (ImmF32Operand *)ins->GetOperand2();

            auto imm1 = imm1_op->GetFloatVal();
            auto imm2 = imm2_op->GetFloatVal();
            auto rd = GetllvmReg(rd_op->GetRegNo(), FLOAT64);

            auto copy_imm_instr = rvconstructor->ConstructCopyRegImmF(rd, imm1 * imm2, FLOAT64);
            cur_block->push_back(copy_imm_instr);
            // float val = imm1 * imm2;

            // auto inter_reg = GetNewReg(INT64);
            // cur_block->push_back(rvconstructor->ConstructCopyRegImmI(inter_reg, *(int *)&val, INT64));
            // cur_block->push_back(rvconstructor->ConstructR2(RISCV_FMV_W_X, rd, inter_reg));
        } else {
            Assert(ins->GetResultOperand()->GetOperandType() == BasicOperand::REG);
            Register rd = GetllvmReg(((RegOperand *)ins->GetResultOperand())->GetRegNo(), FLOAT64);
            Register fsub1, fsub2;
            if (ins->GetOperand1()->GetOperandType() == BasicOperand::IMMF32) {
                fsub1 = GetNewReg(FLOAT64);
                auto copy_imm_instr = rvconstructor->ConstructCopyRegImmF(
                fsub1, ((ImmF32Operand *)ins->GetOperand1())->GetFloatVal(), FLOAT64);
                // float val = ((ImmF32Operand *)ins->GetOperand1())->GetFloatVal();

                // auto inter_reg = GetNewReg(INT64);
                // cur_block->push_back(rvconstructor->ConstructCopyRegImmI(inter_reg, *(int *)&val, INT64));
                // cur_block->push_back(rvconstructor->ConstructR2(RISCV_FMV_W_X, fsub1, inter_reg));
                cur_block->push_back(copy_imm_instr);
            } else if (ins->GetOperand1()->GetOperandType() == BasicOperand::REG) {
                fsub1 = GetllvmReg(((RegOperand *)ins->GetOperand1())->GetRegNo(), FLOAT64);
            } else {
                ERROR("Unexpected op type");
            }
            if (ins->GetOperand2()->GetOperandType() == BasicOperand::IMMF32) {
                fsub2 = GetNewReg(FLOAT64);
                auto copy_imm_instr = rvconstructor->ConstructCopyRegImmF(
                fsub2, ((ImmF32Operand *)ins->GetOperand2())->GetFloatVal(), FLOAT64);
                // float val = ((ImmF32Operand *)ins->GetOperand2())->GetFloatVal();

                // auto inter_reg = GetNewReg(INT64);
                // cur_block->push_back(rvconstructor->ConstructCopyRegImmI(inter_reg, *(int *)&val, INT64));
                // cur_block->push_back(rvconstructor->ConstructR2(RISCV_FMV_W_X, fsub2, inter_reg));
                cur_block->push_back(copy_imm_instr);
            } else if (ins->GetOperand2()->GetOperandType() == BasicOperand::REG) {
                fsub2 = GetllvmReg(((RegOperand *)ins->GetOperand2())->GetRegNo(), FLOAT64);
            } else {
                ERROR("Unexpected op type");
            }
            auto fsub_instr = rvconstructor->ConstructR(RISCV_FMUL_S, rd, fsub1, fsub2);
            cur_block->push_back(fsub_instr);
        }
    } 
    //浮点除法
    else if (ins->GetOpcode() == BasicInstruction::FDIV) {
        if (ins->GetOperand1()->GetOperandType() == BasicOperand::IMMF32 &&
            ins->GetOperand2()->GetOperandType() == BasicOperand::IMMF32) {
            Assert(ins->GetResultOperand()->GetOperandType() == BasicOperand::REG);
            auto *rd_op = (RegOperand *)ins->GetResultOperand();
            auto *imm1_op = (ImmF32Operand *)ins->GetOperand1();
            auto *imm2_op = (ImmF32Operand *)ins->GetOperand2();

            auto imm1 = imm1_op->GetFloatVal();
            auto imm2 = imm2_op->GetFloatVal();
            // Assert(imm2 != 0);
            auto rd = GetllvmReg(rd_op->GetRegNo(), FLOAT64);

            auto copy_imm_instr = rvconstructor->ConstructCopyRegImmF(rd, imm1 / imm2, FLOAT64);
            cur_block->push_back(copy_imm_instr);
            // float val = imm1 / imm2;

            // auto inter_reg = GetNewReg(INT64);
            // cur_block->push_back(rvconstructor->ConstructCopyRegImmI(inter_reg, *(int *)&val, INT64));
            // cur_block->push_back(rvconstructor->ConstructR2(RISCV_FMV_W_X, rd, inter_reg));
        } else {
            Assert(ins->GetResultOperand()->GetOperandType() == BasicOperand::REG);
            Register rd = GetllvmReg(((RegOperand *)ins->GetResultOperand())->GetRegNo(), FLOAT64);
            Register fsub1, fsub2;
            if (ins->GetOperand1()->GetOperandType() == BasicOperand::IMMF32) {
                fsub1 = GetNewReg(FLOAT64);
                auto copy_imm_instr = rvconstructor->ConstructCopyRegImmF(
                fsub1, ((ImmF32Operand *)ins->GetOperand1())->GetFloatVal(), FLOAT64);
                // float val = ((ImmF32Operand *)ins->GetOperand1())->GetFloatVal();

                // auto inter_reg = GetNewReg(INT64);
                // cur_block->push_back(rvconstructor->ConstructCopyRegImmI(inter_reg, *(int *)&val, INT64));
                // cur_block->push_back(rvconstructor->ConstructR2(RISCV_FMV_W_X, fsub1, inter_reg));
                cur_block->push_back(copy_imm_instr);
            } else if (ins->GetOperand1()->GetOperandType() == BasicOperand::REG) {
                fsub1 = GetllvmReg(((RegOperand *)ins->GetOperand1())->GetRegNo(), FLOAT64);
            } else {
                ERROR("Unexpected op type");
            }
            if (ins->GetOperand2()->GetOperandType() == BasicOperand::IMMF32) {
                fsub2 = GetNewReg(FLOAT64);
                auto copy_imm_instr = rvconstructor->ConstructCopyRegImmF(
                fsub2, ((ImmF32Operand *)ins->GetOperand2())->GetFloatVal(), FLOAT64);
                // float val = ((ImmF32Operand *)ins->GetOperand2())->GetFloatVal();

                // auto inter_reg = GetNewReg(INT64);
                // cur_block->push_back(rvconstructor->ConstructCopyRegImmI(inter_reg, *(int *)&val, INT64));
                // cur_block->push_back(rvconstructor->ConstructR2(RISCV_FMV_W_X, fsub2, inter_reg));
                cur_block->push_back(copy_imm_instr);
            } else if (ins->GetOperand2()->GetOperandType() == BasicOperand::REG) {
                fsub2 = GetllvmReg(((RegOperand *)ins->GetOperand2())->GetRegNo(), FLOAT64);
            } else {
                ERROR("Unexpected op type");
            }
            auto fsub_instr = rvconstructor->ConstructR(RISCV_FDIV_S, rd, fsub1, fsub2);
            cur_block->push_back(fsub_instr);
        }
    }
    // else if (ins->GetOpcode() == BasicInstruction::BITXOR) {
    //     Register rd = GetllvmReg(((RegOperand *)ins->GetResultOperand())->GetRegNo(), INT64);
    //     if (ins->GetOperand1()->GetOperandType() == BasicOperand::IMMI32 &&
    //         ins->GetOperand2()->GetOperandType() == BasicOperand::IMMI32) {
    //         int imm1 = ExtractOp2ImmI32(ins->GetOperand1());
    //         int imm2 = ExtractOp2ImmI32(ins->GetOperand2());
    //         cur_block->push_back(rvconstructor->ConstructCopyRegImmI(rd, imm1 ^ imm2, INT64));
    //     } else if (ins->GetOperand1()->GetOperandType() == BasicOperand::IMMI32 ||
    //                ins->GetOperand2()->GetOperandType() == BasicOperand::IMMI32) {
    //         int imm = ins->GetOperand1()->GetOperandType() == BasicOperand::IMMI32
    //                   ? ExtractOp2ImmI32(ins->GetOperand1())
    //                   : ExtractOp2ImmI32(ins->GetOperand2());
    //         Register rs = ins->GetOperand1()->GetOperandType() == BasicOperand::REG
    //                       ? ExtractOp2Reg(ins->GetOperand1(), INT64)
    //                       : ExtractOp2Reg(ins->GetOperand2(), INT64);
    //         cur_block->push_back(rvconstructor->ConstructIImm(RISCV_XORI, rd, rs, imm));
    //     } else {
    //         cur_block->push_back(rvconstructor->ConstructR(RISCV_XOR, rd, ExtractOp2Reg(ins->GetOperand1(), INT64),
    //                                                        ExtractOp2Reg(ins->GetOperand2(), INT64)));
    //     }
    // } 
    else {
       // Log("RV InstSelect For Opcode %d", ins->GetOpcode());
    }
}

template <> void RiscV64Selector::ConvertAndAppend<IcmpInstruction *>(IcmpInstruction *ins) {
    Assert(ins->GetResult()->GetOperandType() == BasicOperand::REG);
    auto res_op = (RegOperand *)ins->GetResult();
    auto res_reg = GetllvmReg(res_op->GetRegNo(), INT64);
    cmp_context[res_reg] = ins;
}

template <> void RiscV64Selector::ConvertAndAppend<FcmpInstruction *>(FcmpInstruction *ins) {
    Assert(ins->GetResult()->GetOperandType() == BasicOperand::REG);
    auto res_op = (RegOperand *)ins->GetResult();
    auto res_reg = GetllvmReg(res_op->GetRegNo(), INT64);
    cmp_context[res_reg] = ins;
}

template <> void RiscV64Selector::ConvertAndAppend<AllocaInstruction *>(AllocaInstruction *ins) {
    Assert(ins->GetResultOp()->GetOperandType() == BasicOperand::REG);
    auto reg_op = (RegOperand *)ins->GetResultOp();
    int byte_size = ins->GetAllocaSize() << 2;
    // Log("Alloca size %d", byte_size);
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
        if (icmp_ins->GetOp1()->GetOperandType() == BasicOperand::REG) {
            cmp_op1 = GetllvmReg(((RegOperand *)icmp_ins->GetOp1())->GetRegNo(), INT64);
        } else if (icmp_ins->GetOp1()->GetOperandType() == BasicOperand::IMMI32) {
            
            if (((ImmI32Operand *)icmp_ins->GetOp1())->GetIntImmVal() != 0) {
                cmp_op1 = GetNewReg(INT64);
                auto copy_imm_instr = rvconstructor->ConstructCopyRegImmI(
                cmp_op1, ((ImmI32Operand *)icmp_ins->GetOp1())->GetIntImmVal(), INT64);
                cur_block->push_back(copy_imm_instr);
            } else {
                cmp_op1 = GetPhysicalReg(RISCV_x0);
            }
        } else {
            ERROR("Unexpected ICMP op1 type");
        }
        if (icmp_ins->GetOp2()->GetOperandType() == BasicOperand::REG) {
            cmp_op2 = GetllvmReg(((RegOperand *)icmp_ins->GetOp2())->GetRegNo(), INT64);
        } else if (icmp_ins->GetOp2()->GetOperandType() == BasicOperand::IMMI32) {
            if (((ImmI32Operand *)icmp_ins->GetOp2())->GetIntImmVal() != 0) {
                cmp_op2 = GetNewReg(INT64);
                auto copy_imm_instr = rvconstructor->ConstructCopyRegImmI(
                cmp_op2, ((ImmI32Operand *)icmp_ins->GetOp2())->GetIntImmVal(), INT64);
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
    } else if (cmp_ins->GetOpcode() == BasicInstruction::FCMP) {
        auto fcmp_ins = (FcmpInstruction *)cmp_ins;
        if (fcmp_ins->GetOp1()->GetOperandType() == BasicOperand::REG) {
            cmp_op1 = GetllvmReg(((RegOperand *)fcmp_ins->GetOp1())->GetRegNo(), FLOAT64);
        } else if (fcmp_ins->GetOp1()->GetOperandType() == BasicOperand::IMMF32) {
            cmp_op1 = GetNewReg(FLOAT64);
            auto cmp_oppre = GetNewReg(INT64);
            
            auto copy_imm_instr =
            rvconstructor->ConstructCopyRegImmF(cmp_op1, ((ImmF32Operand *)fcmp_ins->GetOp1())->GetFloatVal(), FLOAT64);
            cur_block->push_back(copy_imm_instr);
        } else {
            ERROR("Unexpected FCMP op1 type");
        }
        if (fcmp_ins->GetOp2()->GetOperandType() == BasicOperand::REG) {
            cmp_op2 = GetllvmReg(((RegOperand *)fcmp_ins->GetOp2())->GetRegNo(), FLOAT64);
        } else if (fcmp_ins->GetOp2()->GetOperandType() == BasicOperand::IMMF32) {
            cmp_op2 = GetNewReg(FLOAT64);
            auto cmp_oppre = GetNewReg(INT64);

            auto copy_imm_instr =
            rvconstructor->ConstructCopyRegImmF(cmp_op2, ((ImmF32Operand *)fcmp_ins->GetOp2())->GetFloatVal(), FLOAT64);
            cur_block->push_back(copy_imm_instr);
        } else {
            ERROR("Unexpected FCMP op2 type");
        }
        cmp_rd = GetNewReg(INT64);
        switch (fcmp_ins->GetCond()) {
        case BasicInstruction::FcmpCond::OEQ:
        case BasicInstruction::FcmpCond::UEQ:
            // FEQ.S
            cur_block->push_back(rvconstructor->ConstructR(RISCV_FEQ_S, cmp_rd, cmp_op1, cmp_op2));
            opcode = RISCV_BNE;
            break;
        case BasicInstruction::FcmpCond::OGT:
        case BasicInstruction::FcmpCond::UGT:
            cur_block->push_back(rvconstructor->ConstructR(RISCV_FLT_S, cmp_rd, cmp_op2, cmp_op1));
            opcode = RISCV_BNE;
            break;
        case BasicInstruction::FcmpCond::OGE:
        case BasicInstruction::FcmpCond::UGE:
            cur_block->push_back(rvconstructor->ConstructR(RISCV_FLE_S, cmp_rd, cmp_op2, cmp_op1));
            opcode = RISCV_BNE;
            break;
        case BasicInstruction::FcmpCond::OLT:
        case BasicInstruction::FcmpCond::ULT:
            cur_block->push_back(rvconstructor->ConstructR(RISCV_FLT_S, cmp_rd, cmp_op1, cmp_op2));
            opcode = RISCV_BNE;
            break;
        case BasicInstruction::FcmpCond::OLE:
        case BasicInstruction::FcmpCond::ULE:
            cur_block->push_back(rvconstructor->ConstructR(RISCV_FLE_S, cmp_rd, cmp_op1, cmp_op2));
            opcode = RISCV_BNE;
            break;
        case BasicInstruction::FcmpCond::ONE:
        case BasicInstruction::FcmpCond::UNE:
            cur_block->push_back(rvconstructor->ConstructR(RISCV_FEQ_S, cmp_rd, cmp_op1, cmp_op2));
            opcode = RISCV_BEQ;
            break;
        case BasicInstruction::FcmpCond::ORD:
        case BasicInstruction::FcmpCond::UNO:
        case BasicInstruction::FcmpCond::TRUE:
        case BasicInstruction::FcmpCond::FALSE:
        default:
            ERROR("Unexpected FCMP cond");
        }
        cmp_op1 = cmp_rd;
        cmp_op2 = GetPhysicalReg(RISCV_x0);
    } else {
        ERROR("No Cmp Before Br");
    }
    Assert(ins->GetTrueLabel()->GetOperandType() == BasicOperand::LABEL);
    Assert(ins->GetFalseLabel()->GetOperandType() == BasicOperand::LABEL);
    auto true_label = (LabelOperand *)ins->GetTrueLabel();
    auto false_label = (LabelOperand *)ins->GetFalseLabel();

    auto br_ins = rvconstructor->ConstructBLabel(opcode, cmp_op1, cmp_op2,
                                                 RiscVLabel(true_label->GetLabelNo()));
    
    cur_block->push_back(br_ins);
    auto br_else_ins =rvconstructor->ConstructJLabel(RISCV_JAL, GetPhysicalReg(RISCV_x0), RiscVLabel(false_label->GetLabelNo()));
    cur_block->push_back(br_else_ins);
}

template <> void RiscV64Selector::ConvertAndAppend<BrUncondInstruction *>(BrUncondInstruction *ins) {
    rvconstructor->DisableSchedule();
    auto dest_label = RiscVLabel(((LabelOperand *)ins->GetDestLabel())->GetLabelNo());

    auto jal_instr = rvconstructor->ConstructJLabel(RISCV_JAL, GetPhysicalReg(RISCV_x0), dest_label);

    cur_block->push_back(jal_instr);
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

    if (ins->GetFunctionName() == std::string("llvm.memset.p0.i32")) {
        Assert(ins->GetParameterList().size() == 4);
        Assert(ins->GetParameterList()[0].second->GetOperandType() == BasicOperand::REG);
        Assert(ins->GetParameterList()[1].second->GetOperandType() == BasicOperand::IMMI32);
        // Assert(ins->GetParameterList()[2].second->GetOperandType() == BasicOperand::IMMI32);
        Assert(ins->GetParameterList()[3].second->GetOperandType() == BasicOperand::IMMI32);

        // parameter 0
        {
            int ptrreg_no = ((RegOperand *)ins->GetParameterList()[0].second)->GetRegNo();
            if (llvm_rv_allocas.find(ptrreg_no) == llvm_rv_allocas.end()) {
                cur_block->push_back(
                rvconstructor->ConstructCopyReg(GetPhysicalReg(RISCV_a0), GetllvmReg(ptrreg_no, INT64), INT64));
            } else {
                auto sp_offset = llvm_rv_allocas[ptrreg_no];
                auto ld_alloca =
                rvconstructor->ConstructIImm(RISCV_ADDI, GetPhysicalReg(RISCV_a0), GetPhysicalReg(RISCV_sp), sp_offset);
                ((RiscV64Function *)cur_func)->AddAllocaIns(ld_alloca);
                cur_block->push_back(ld_alloca);
            }
        }
        // parameters 1
        {
            auto imm_op = (ImmI32Operand *)(ins->GetParameterList()[1].second);
            cur_block->push_back(
            rvconstructor->ConstructCopyRegImmI(GetPhysicalReg(RISCV_a1), imm_op->GetIntImmVal(), INT64));
        }
        // paramters 2
        {
            if (ins->GetParameterList()[2].second->GetOperandType() == BasicOperand::IMMI32) {
                int arr_sz = ((ImmI32Operand *)ins->GetParameterList()[2].second)->GetIntImmVal();
                cur_block->push_back(rvconstructor->ConstructCopyRegImmI(GetPhysicalReg(RISCV_a2), arr_sz, INT64));
            } else {
                int sizereg_no = ((RegOperand *)ins->GetParameterList()[2].second)->GetRegNo();
                if (llvm_rv_allocas.find(sizereg_no) == llvm_rv_allocas.end()) {
                    cur_block->push_back(
                    rvconstructor->ConstructCopyReg(GetPhysicalReg(RISCV_a2), GetllvmReg(sizereg_no, INT64), INT64));
                } else {
                    auto sp_offset = llvm_rv_allocas[sizereg_no];
                    auto ld_alloca = rvconstructor->ConstructIImm(RISCV_ADDI, GetPhysicalReg(RISCV_a2),
                                                                  GetPhysicalReg(RISCV_sp), sp_offset);
                    ((RiscV64Function *)cur_func)->AddAllocaIns(ld_alloca);
                    cur_block->push_back(ld_alloca);
                }
            }
        }
        // call
        cur_block->push_back(rvconstructor->ConstructCall(RISCV_CALL, "memset", 3, 0));
        return;
    }
    // Parameters
    for (auto [type, arg_op] : ins->GetParameterList()) {
        if (type == BasicInstruction::I32 || type == BasicInstruction::PTR) {
            if (ireg_cnt < 8) {
                if (arg_op->GetOperandType() == BasicOperand::REG) {
                    auto arg_regop = (RegOperand *)arg_op;
                    auto arg_reg = GetllvmReg(arg_regop->GetRegNo(), INT64);
                    if (llvm_rv_allocas.find(arg_regop->GetRegNo()) == llvm_rv_allocas.end()) {
                        auto arg_copy_instr =
                        rvconstructor->ConstructCopyReg(GetPhysicalReg(RISCV_a0 + ireg_cnt), arg_reg, INT64);
                        cur_block->push_back(arg_copy_instr);
                    } else {
                        auto sp_offset = llvm_rv_allocas[arg_regop->GetRegNo()];
                        cur_block->push_back(rvconstructor->ConstructIImm(
                        RISCV_ADDI, GetPhysicalReg(RISCV_a0 + ireg_cnt), GetPhysicalReg(RISCV_sp), sp_offset));
                    }
                } else if (arg_op->GetOperandType() == BasicOperand::IMMI32) {
                    auto arg_immop = (ImmI32Operand *)arg_op;
                    auto arg_imm = arg_immop->GetIntImmVal();
                    auto arg_copy_instr =
                    rvconstructor->ConstructCopyRegImmI(GetPhysicalReg(RISCV_a0 + ireg_cnt), arg_imm, INT64);
                    cur_block->push_back(arg_copy_instr);
                } else if (arg_op->GetOperandType() == BasicOperand::GLOBAL) {
                    auto mid_reg = GetNewReg(INT64);
                    auto arg_global = (GlobalOperand *)arg_op;
                    cur_block->push_back(
                    rvconstructor->ConstructULabel(RISCV_LUI, mid_reg, RiscVLabel(arg_global->GetName(), true)));
                    cur_block->push_back(rvconstructor->ConstructILabel(RISCV_ADDI, GetPhysicalReg(RISCV_a0 + ireg_cnt),
                                                                        mid_reg,
                                                                        RiscVLabel(arg_global->GetName(), false)));
                } else {
                    ERROR("Unexpected Operand type");
                }
            } else {
            }
            ireg_cnt++;
        } else if (type == BasicInstruction::FLOAT32) {
            if (freg_cnt < 8) {
                if (arg_op->GetOperandType() == BasicOperand::REG) {
                    auto arg_regop = (RegOperand *)arg_op;
                    auto arg_reg = GetllvmReg(arg_regop->GetRegNo(), FLOAT64);
                    auto arg_copy_instr =
                    rvconstructor->ConstructCopyReg(GetPhysicalReg(RISCV_fa0 + freg_cnt), arg_reg, FLOAT64);
                    cur_block->push_back(arg_copy_instr);
                } else if (arg_op->GetOperandType() == BasicOperand::IMMF32) {
                    auto arg_immop = (ImmF32Operand *)arg_op;
                    auto arg_imm = arg_immop->GetFloatVal();
                    auto arg_copy_instr =
                    rvconstructor->ConstructCopyRegImmF(GetPhysicalReg(RISCV_fa0 + freg_cnt), arg_imm, FLOAT64);
                    cur_block->push_back(arg_copy_instr);
                } else {
                    ERROR("Unexpected Operand type");
                }
            } else {
            }
            freg_cnt++;
        } else if (type == BasicInstruction::DOUBLE) {
            if (ireg_cnt < 8) {
                if (arg_op->GetOperandType() == BasicOperand::REG) {
                    auto arg_regop = (RegOperand *)arg_op;
                    auto arg_reg = GetllvmReg(arg_regop->GetRegNo(), FLOAT64);
                    cur_block->push_back(
                    rvconstructor->ConstructR2(RISCV_FMV_X_D, GetPhysicalReg(RISCV_a0 + ireg_cnt), arg_reg));
                } else {
                    ERROR("Unexpected Operand Type");
                }
            } else {
            }
            ireg_cnt++;
        } else {
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
            } else if (type == BasicInstruction::FLOAT32) {
                if (freg_cnt < 8) {
                } else {
                    if (arg_op->GetOperandType() == BasicOperand::REG) {
                        auto arg_regop = (RegOperand *)arg_op;
                        auto arg_reg = GetllvmReg(arg_regop->GetRegNo(), FLOAT64);
                        cur_block->push_back(
                        rvconstructor->ConstructSImm(RISCV_FSD, arg_reg, GetPhysicalReg(RISCV_sp), arg_off));
                    } else if (arg_op->GetOperandType() == BasicOperand::IMMF32) {
                        auto arg_immop = (ImmF32Operand *)arg_op;
                        auto arg_imm = arg_immop->GetFloatVal();
                        auto imm_reg = GetNewReg(INT64);
                        cur_block->push_back(rvconstructor->ConstructCopyRegImmI(imm_reg, *(int *)&arg_imm, INT64));
                        cur_block->push_back(
                        rvconstructor->ConstructSImm(RISCV_SD, imm_reg, GetPhysicalReg(RISCV_sp), arg_off));
                    } else {
                        ERROR("Unexpected Operand type");
                    }
                    arg_off += 8;
                }
                freg_cnt++;
            } else if (type == BasicInstruction::DOUBLE) {
                if (ireg_cnt < 8) {
                } else {
                    if (arg_op->GetOperandType() == BasicOperand::REG) {
                        auto arg_regop = (RegOperand *)arg_op;
                        auto arg_reg = GetllvmReg(arg_regop->GetRegNo(), FLOAT64);
                        cur_block->push_back(
                        rvconstructor->ConstructSImm(RISCV_FSD, arg_reg, GetPhysicalReg(RISCV_sp), arg_off));
                    } else {
                        ERROR("Unexpected Operand type");
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
    } else if (return_type == BasicInstruction::FLOAT32) {
        // Lazy("Not tested");
        auto copy_ret_ins =
        rvconstructor->ConstructCopyReg(GetllvmReg(result_op->GetRegNo(), FLOAT64), GetPhysicalReg(RISCV_fa0), FLOAT64);
        cur_block->push_back(copy_ret_ins);
    } else if (return_type == BasicInstruction::VOID) {
    } else {
        ERROR("Unexpected return type %d", return_type);
    }
    
    //TODO("Implement this if you need");
}

template <> void RiscV64Selector::ConvertAndAppend<RetInstruction *>(RetInstruction *ins) {
    //printf("%d",7);
    rvconstructor->DisableSchedule();
    if (ins->GetRetVal() != NULL) {
        //printf("%d",6);
        if (ins->GetRetVal()->GetOperandType() == BasicOperand::IMMI32) {
            auto retimm_op = (ImmI32Operand *)ins->GetRetVal();
            auto imm = retimm_op->GetIntImmVal();
           //printf("%d",00);
            auto retcopy_instr = rvconstructor->ConstructUImm(RISCV_LI, GetPhysicalReg(RISCV_a0), imm);
            cur_block->push_back(retcopy_instr);
        } else if (ins->GetRetVal()->GetOperandType() == BasicOperand::IMMF32) {
            auto retimm_op = (ImmF32Operand *)ins->GetRetVal();
            auto imm = retimm_op->GetFloatVal();

            auto retcopy_instr = rvconstructor->ConstructCopyRegImmF(GetPhysicalReg(RISCV_fa0), imm, FLOAT64);
            cur_block->push_back(retcopy_instr);
        } else if (ins->GetRetVal()->GetOperandType() == BasicOperand::REG) {
            //printf("%d",0);
            if (ins->GetType() == BasicInstruction::LLVMType::FLOAT32) {

                auto retreg_val = (RegOperand *)ins->GetRetVal();
                auto reg = GetllvmReg(retreg_val->GetRegNo(), FLOAT64);

                auto retcopy_instr = rvconstructor->ConstructCopyReg(GetPhysicalReg(RISCV_fa0), reg, FLOAT64);
                cur_block->push_back(retcopy_instr);

            } else if (ins->GetType() == BasicInstruction::LLVMType::I32) {

                // auto retreg_val = (RegOperand *)ins->GetRetVal();
                // auto reg = GetllvmReg(retreg_val->GetRegNo(), INT64);

                // //auto retcopy_instr = rvconstructor->ConstructCopyReg(GetPhysicalReg(RISCV_a0), reg, INT64);
                // auto retcopy_instr=rvconstructor->ConstructR(RISCV_ADD, GetPhysicalReg(RISCV_a0), reg,
                //                                                     GetPhysicalReg(RISCV_x0));
                // cur_block->push_back(retcopy_instr);
                auto retreg_val = (RegOperand *)ins->GetRetVal();
                auto reg = GetllvmReg(retreg_val->GetRegNo(), INT64);

                auto retcopy_instr = rvconstructor->ConstructCopyReg(GetPhysicalReg(RISCV_a0), reg, INT64);
                cur_block->push_back(retcopy_instr);
            }
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

//浮点转整数
template <> void RiscV64Selector::ConvertAndAppend<FptosiInstruction *>(FptosiInstruction *ins) {
    // TODO("Implement this if you need");
    auto src_op = ins->GetSrc();
    auto dst_op = (RegOperand *)ins->GetResultReg();
    if (src_op->GetOperandType() == BasicOperand::REG) {
        auto regf = (RegOperand *)src_op;
        // FCVT.W.S 从浮点寄存器 regf 中获取值，转换为整数，并存储到目标寄存器 dst_op 中
        auto fmv = rvconstructor->ConstructR2(RISCV_FCVT_W_S, GetllvmReg(dst_op->GetRegNo(), INT64),
                                              GetllvmReg(regf->GetRegNo(), FLOAT64));
        cur_block->push_back(fmv);
    } 
    else if (src_op->GetOperandType() == BasicOperand::IMMF32) {
        auto immf = (ImmF32Operand *)src_op;
        auto copyI =
        rvconstructor->ConstructCopyRegImmI(GetllvmReg(dst_op->GetRegNo(), INT64), immf->GetFloatVal(), INT64);
        cur_block->push_back(copyI);
    } 
    else {
        ERROR("Unexpected Fptosi src type");
    }
}

template <> void RiscV64Selector::ConvertAndAppend<SitofpInstruction *>(SitofpInstruction *ins) {
    //TODO("Implement this if you need");
    auto src_op = ins->GetSrc();
    auto dst_op = (RegOperand *)ins->GetResultReg();
    if (src_op->GetOperandType() == BasicOperand::REG) {
        auto regi = (RegOperand *)src_op;
        // FCVT.S.W
        auto fcvt = rvconstructor->ConstructR2(RISCV_FCVT_S_W, GetllvmReg(dst_op->GetRegNo(), FLOAT64),
                                               GetllvmReg(regi->GetRegNo(), INT64));
        cur_block->push_back(fcvt);
    } else if (src_op->GetOperandType() == BasicOperand::IMMI32) {
        auto immi = (ImmI32Operand *)src_op;
        auto inter_reg = GetNewReg(INT64);
        cur_block->push_back(rvconstructor->ConstructCopyRegImmI(inter_reg, immi->GetIntImmVal(), INT64));
        cur_block->push_back(
        rvconstructor->ConstructR2(RISCV_FCVT_S_W, GetllvmReg(dst_op->GetRegNo(), FLOAT64), inter_reg));
    } else {
        ERROR("Unexpected Sitofp src type");
    }
}

//零扩展操作
template <> void RiscV64Selector::ConvertAndAppend<ZextInstruction *>(ZextInstruction *ins) {
    //TODO("Implement this if you need");
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
    } else if (cmp_ins->GetOpcode() == BasicInstruction::FCMP) {
        auto fcmp_ins = (FcmpInstruction *)cmp_ins;
        Register cmp_op1, cmp_op2;
        if (fcmp_ins->GetOp1()->GetOperandType() == BasicOperand::REG) {
            cmp_op1 = GetllvmReg(((RegOperand *)fcmp_ins->GetOp1())->GetRegNo(), FLOAT64);
        } else if (fcmp_ins->GetOp1()->GetOperandType() == BasicOperand::IMMF32) {
            cmp_op1 = GetNewReg(FLOAT64);
            // auto cmp_oppre = GetNewReg(INT64);
            float float_val = ((ImmF32Operand *)fcmp_ins->GetOp1())->GetFloatVal();
            cur_block->push_back(rvconstructor->ConstructCopyRegImmF(cmp_op1, float_val, FLOAT64));
            // cur_block->push_back(rvconstructor->ConstructCopyRegImmI(cmp_oppre, *(int *)&float_val, INT64));
            // cur_block->push_back(rvconstructor->ConstructR2(RISCV_FMV_W_X, cmp_op1, cmp_oppre));
        } else {
            ERROR("Unexpected FCMP op1 type");
        }
        if (fcmp_ins->GetOp2()->GetOperandType() == BasicOperand::REG) {
            cmp_op2 = GetllvmReg(((RegOperand *)fcmp_ins->GetOp2())->GetRegNo(), FLOAT64);
        } else if (fcmp_ins->GetOp2()->GetOperandType() == BasicOperand::IMMF32) {
            cmp_op2 = GetNewReg(FLOAT64);
            // auto cmp_oppre = GetNewReg(INT64);
            float float_val = ((ImmF32Operand *)fcmp_ins->GetOp2())->GetFloatVal();
            cur_block->push_back(rvconstructor->ConstructCopyRegImmF(cmp_op2, float_val, FLOAT64));
            // cur_block->push_back(rvconstructor->ConstructCopyRegImmI(cmp_oppre, *(int *)&float_val, INT64));
            // cur_block->push_back(rvconstructor->ConstructR2(RISCV_FMV_W_X, cmp_op2, cmp_oppre));
        } else {
            ERROR("Unexpected FCMP op2 type");
        }
        switch (fcmp_ins->GetCond()) {
        case BasicInstruction::OEQ:
        case BasicInstruction::UEQ:
            // FEQ.S
            cur_block->push_back(rvconstructor->ConstructR(RISCV_FEQ_S, result_reg, cmp_op1, cmp_op2));
            break;
        case BasicInstruction::OGT:
        case BasicInstruction::UGT:
            cur_block->push_back(rvconstructor->ConstructR(RISCV_FLT_S, result_reg, cmp_op2, cmp_op1));
            break;
        case BasicInstruction::OGE:
        case BasicInstruction::UGE:
            cur_block->push_back(rvconstructor->ConstructR(RISCV_FLE_S, result_reg, cmp_op2, cmp_op1));
            break;
        case BasicInstruction::OLT:
        case BasicInstruction::ULT:
            cur_block->push_back(rvconstructor->ConstructR(RISCV_FLT_S, result_reg, cmp_op1, cmp_op2));
            break;
        case BasicInstruction::OLE:
        case BasicInstruction::ULE:
            cur_block->push_back(rvconstructor->ConstructR(RISCV_FLE_S, result_reg, cmp_op1, cmp_op2));
            break;
        case BasicInstruction::ONE:
        case BasicInstruction::UNE:
            cur_block->push_back(rvconstructor->ConstructR(RISCV_FEQ_S, result_reg, cmp_op1, cmp_op2));
            break;
        case BasicInstruction::ORD:
        case BasicInstruction::UNO:
        case BasicInstruction::TRUE:
        case BasicInstruction::FALSE:
        default:
            ERROR("Unexpected FCMP cond");
        }
    } else {
        ERROR("Unexpected ins Before zext");
    }
}

template <> void RiscV64Selector::ConvertAndAppend<GetElementptrInstruction *>(GetElementptrInstruction *ins) {
    auto global_op = (GlobalOperand *)ins->GetPtrVal();
    auto result_op = (RegOperand *)ins->GetResult();

    int product = 1;
    for (auto size : ins->GetDims()) {
        product *= size;
    }
    int const_offset = 0;
    auto offset_reg = GetNewReg(INT64);
    auto result_reg = GetllvmReg(result_op->GetRegNo(), INT64);

    int offset_reg_assigned = 0;
    for (int i = 0; i < ins->GetIndexes().size(); i++) {
        if (ins->GetIndexes()[i]->GetOperandType() == BasicOperand::IMMI32) {
            const_offset += (((ImmI32Operand *)ins->GetIndexes()[i])->GetIntImmVal()) * product;
        } else {
            auto index_op = (RegOperand *)ins->GetIndexes()[i];
            auto index_reg = GetllvmReg(index_op->GetRegNo(), INT64);
            if (product != 1) {
                auto this_inc = GetNewReg(INT64);
                // this_inc = indexes[i] * product
                auto product_reg = GetNewReg(INT64);
                cur_block->push_back(rvconstructor->ConstructCopyRegImmI(product_reg, product, INT64));
                cur_block->push_back(rvconstructor->ConstructR(RISCV_MUL, this_inc, index_reg, product_reg));
                if (offset_reg_assigned == 0) {
                    offset_reg_assigned = 1;
                    // offset = this_inc
                    cur_block->push_back(rvconstructor->ConstructCopyReg(offset_reg, this_inc, INT64));
                } else {
                    auto new_offset = GetNewReg(INT64);
                    // offset += this_inc
                    cur_block->push_back(rvconstructor->ConstructR(RISCV_ADD, new_offset, offset_reg, this_inc));
                    offset_reg = new_offset;
                }
            } else {
                if (offset_reg_assigned == 0) {
                    offset_reg_assigned = 1;
                    // offset_reg = indexes[i]
                    auto offset_reg_set_instr = rvconstructor->ConstructCopyReg(offset_reg, index_reg, INT64);
                    cur_block->push_back(offset_reg_set_instr);
                } else {
                    auto new_offset = GetNewReg(INT64);
                    // offset += indexes[i]
                    cur_block->push_back(rvconstructor->ConstructR(RISCV_ADD, new_offset, offset_reg, index_reg));
                    offset_reg = new_offset;
                }
            }
        }
        if (i < ins->GetDims().size()) {
            product /= ins->GetDims()[i];
        }
    }
    // ins->PrintIR(std::cerr);
    // Log("const_offset = %d",const_offset);
    bool all_imm = false;
    if (const_offset != 0) {
        if (offset_reg_assigned == 0) {
            offset_reg_assigned = 1;
            all_imm = true;

            auto li_instr = rvconstructor->ConstructCopyRegImmI(offset_reg, const_offset * 4, INT64);

            cur_block->push_back(li_instr);
        } else {
            auto new_offset = GetNewReg(INT64);
            cur_block->push_back(rvconstructor->ConstructIImm(RISCV_ADDI, new_offset, offset_reg, const_offset));
            offset_reg = new_offset;
        }
    }
    if (ins->GetPtrVal()->GetOperandType() == BasicOperand::REG) {
        // Lazy("Not tested");
        auto ptr_op = (RegOperand *)ins->GetPtrVal();
        auto offsetfull_reg = GetNewReg(INT64);
        if (offset_reg_assigned) {
            auto sll_instr = rvconstructor->ConstructIImm(RISCV_SLLI, offsetfull_reg, offset_reg, 2);
            if (all_imm) {
                offsetfull_reg = offset_reg;
            }
            if (llvm_rv_allocas.find(ptr_op->GetRegNo()) == llvm_rv_allocas.end()) {
                auto base_reg = GetllvmReg(ptr_op->GetRegNo(), INT64);
                if (!all_imm) {
                    cur_block->push_back(sll_instr);
                }
                auto addoffset_instr = rvconstructor->ConstructR(RISCV_ADD, result_reg, base_reg, offsetfull_reg);
                cur_block->push_back(addoffset_instr);
            } else {
                auto sp_offset = llvm_rv_allocas[ptr_op->GetRegNo()];
                auto base_reg = GetNewReg(INT64);
                auto load_basereg_instr =
                rvconstructor->ConstructIImm(RISCV_ADDI, base_reg, GetPhysicalReg(RISCV_sp), sp_offset);
                ((RiscV64Function *)cur_func)->AddAllocaIns(load_basereg_instr);
                cur_block->push_back(load_basereg_instr);
                if (!all_imm) {
                    cur_block->push_back(sll_instr);
                }
                auto addoffset_instr = rvconstructor->ConstructR(RISCV_ADD, result_reg, base_reg, offsetfull_reg);
                cur_block->push_back(addoffset_instr);
            }
        } else {
            if (llvm_rv_allocas.find(ptr_op->GetRegNo()) == llvm_rv_allocas.end()) {
                cur_block->push_back(
                rvconstructor->ConstructCopyReg(result_reg, GetllvmReg(ptr_op->GetRegNo(), INT64), INT64));
            } else {
                auto sp_offset = llvm_rv_allocas[ptr_op->GetRegNo()];
                auto load_basereg_instr =
                rvconstructor->ConstructIImm(RISCV_ADDI, result_reg, GetPhysicalReg(RISCV_sp), sp_offset);
                ((RiscV64Function *)cur_func)->AddAllocaIns(load_basereg_instr);
                cur_block->push_back(load_basereg_instr);
            }
        }
    } else if (ins->GetPtrVal()->GetOperandType() == BasicOperand::GLOBAL) {
        if (offset_reg_assigned) {
            auto basehi_reg = GetNewReg(INT64);
            auto basefull_reg = GetNewReg(INT64);
            auto offsetfull_reg = GetNewReg(INT64);

            auto lui_instr =
            rvconstructor->ConstructULabel(RISCV_LUI, basehi_reg, RiscVLabel(global_op->GetName(), true));
            auto addi_instr = rvconstructor->ConstructILabel(RISCV_ADDI, basefull_reg, basehi_reg,
                                                             RiscVLabel(global_op->GetName(), false));
            auto sll_instr = rvconstructor->ConstructIImm(RISCV_SLLI, offsetfull_reg, offset_reg, 2);
            if (all_imm) {
                offsetfull_reg = offset_reg;
            }
            auto addoffset_instr = rvconstructor->ConstructR(RISCV_ADD, result_reg, basefull_reg, offsetfull_reg);

            cur_block->push_back(lui_instr);
            cur_block->push_back(addi_instr);
            if (!all_imm) {
                cur_block->push_back(sll_instr);
            }
            cur_block->push_back(addoffset_instr);
        } else {
            auto result_hi_reg = GetNewReg(INT64);

            auto lui_instr =
            rvconstructor->ConstructULabel(RISCV_LUI, result_hi_reg, RiscVLabel(global_op->GetName(), true));
            auto addi_instr = rvconstructor->ConstructILabel(RISCV_ADDI, result_reg, result_hi_reg,
                                                             RiscVLabel(global_op->GetName(), false));

            cur_block->push_back(lui_instr);
            cur_block->push_back(addi_instr);
        }
    } else {
        ERROR("Unexpected OperandType");
    }
    //TODO("Implement this if you need");
}

template <> void RiscV64Selector::ConvertAndAppend<PhiInstruction *>(PhiInstruction *ins) {
    Assert(ins->GetResultOp()->GetOperandType() == BasicOperand::REG);
    auto res_op = (RegOperand *)ins->GetResultOp();
    Register result_reg;
    if (ins->GetDataType() == BasicInstruction::LLVMType::I32 || ins->GetDataType() == BasicInstruction::LLVMType::PTR) {
        result_reg = GetllvmReg(res_op->GetRegNo(), INT64);
    } else if (ins->GetDataType() == BasicInstruction::LLVMType::FLOAT32) {
        result_reg = GetllvmReg(res_op->GetRegNo(), FLOAT64);
    }
    auto m_phi = new MachinePhiInstruction(result_reg);
    for (auto [label, val] : ins->GetPhiList()) {
        Assert(label->GetOperandType() == BasicOperand::LABEL);
        auto label_op = (LabelOperand *)label;
        if (val->GetOperandType() == BasicOperand::REG && ins->GetDataType() == BasicInstruction::LLVMType::I32) {
            auto reg_op = (RegOperand *)val;
            auto val_reg = GetllvmReg(reg_op->GetRegNo(), INT64);
            m_phi->pushPhiList(label_op->GetLabelNo(), val_reg);
        } else if (val->GetOperandType() == BasicOperand::REG && ins->GetDataType() == BasicInstruction::LLVMType::FLOAT32) {
            auto reg_op = (RegOperand *)val;
            auto val_reg = GetllvmReg(reg_op->GetRegNo(), FLOAT64);
            m_phi->pushPhiList(label_op->GetLabelNo(), val_reg);
        } else if (val->GetOperandType() == BasicOperand::IMMI32) {
            auto immi_op = (ImmI32Operand *)val;
            m_phi->pushPhiList(label_op->GetLabelNo(), immi_op->GetIntImmVal());
        } else if (val->GetOperandType() == BasicOperand::IMMF32) {
            auto immf_op = (ImmF32Operand *)val;
            m_phi->pushPhiList(label_op->GetLabelNo(), immf_op->GetFloatVal());
        } else if (val->GetOperandType() == BasicOperand::REG && ins->GetDataType() == BasicInstruction::LLVMType::PTR) {
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

        // TODO: 添加函数参数(推荐先阅读一下riscv64_lowerframe.cc中的代码和注释)
        // See MachineFunction::AddParameter()
        //TODO("Add function parameter if you need");
         for (int i = 0; i < defI->GetFormalSize(); i++) {
            Assert(defI->formals_reg[i]->GetOperandType() == BasicOperand::REG);
            MachineDataType type;
            Assert(defI->formals[i] != BasicInstruction::LLVMType::DOUBLE);
            Assert(defI->formals[i] != BasicInstruction::LLVMType::I64);
            Assert(defI->formals[i] != BasicInstruction::LLVMType::I1);
            Assert(defI->formals[i] != BasicInstruction::LLVMType::I8);
            if (defI->formals[i] == BasicInstruction::LLVMType::I32 || defI->formals[i] == BasicInstruction::LLVMType::PTR) {
                type = INT64;
            } else if (defI->formals[i] == BasicInstruction::LLVMType::FLOAT32) {
                type = FLOAT64;
            } else {
                ERROR("Unknown llvm type");
            }
            cur_func->AddParameter(GetllvmReg(((RegOperand *)defI->formals_reg[i])->GetRegNo(), type));
        }
        // 遍历每个LLVM IR基本块
        for (auto [id, block] : *(cfg->block_map)) {
            cur_block = new RiscV64Block(id);
            // 将新块添加到Machine CFG中
            cur_mcfg->AssignEmptyNode(id, cur_block);
            cur_func->UpdateMaxLabel(id);

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
