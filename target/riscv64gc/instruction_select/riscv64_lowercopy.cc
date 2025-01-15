#include "riscv64_lowercopy.h"
#include "../riscv64.h"
void RiscV64LowerCopy::Execute() {
    for (auto& function : unit->functions) {
        auto block_it = function->getMachineCFG()->getSeqScanIterator();
        block_it->open();

        while (block_it->hasNext()) {
            auto block = block_it->next()->Mblock;

            for (auto it = block->begin(); it != block->end(); ++it) {
                auto& ins = *it;

                if (ins->arch != MachineBaseInstruction::COPY) {
                    continue;
                }

                auto m_copy = static_cast<MachineCopyInstruction*>(ins);
                auto dst_reg = static_cast<MachineRegister*>(m_copy->GetDst());

                Assert(dst_reg->op_type == MachineBaseOperand::REG);

                // 处理立即数类型拷贝 (IMMI)
                if (m_copy->GetSrc()->op_type == MachineBaseOperand::IMMI) {
                    auto src_immi = static_cast<MachineImmediateInt*>(m_copy->GetSrc());
                    auto mid_reg = function->GetNewReg(INT64);

                    it = block->erase(it);
                    it = block->insert(it, rvconstructor->ConstructUImm(
                        RISCV_LUI, mid_reg, src_immi->imm32 + 0x800));
                    it = block->insert(it, rvconstructor->ConstructIImm(
                        RISCV_ADDI, dst_reg->reg, mid_reg, ((src_immi->imm32) << 20) >> 20));

                    --it;  // 回到刚插入的指令上继续
                } 
                // 处理浮点立即数类型拷贝 (IMMF)
                else if (m_copy->GetSrc()->op_type == MachineBaseOperand::IMMF) {
                    ERROR("Floating-point immediate copy not yet implemented");
                    TODO("Implement RiscV Float Immediate Copy");
                } 
                // 处理寄存器到寄存器拷贝 (REG)
                else if (m_copy->GetSrc()->op_type == MachineBaseOperand::REG) {
                    auto src_reg = static_cast<MachineRegister*>(m_copy->GetSrc());
                    auto dst_phys_reg = dst_reg->reg;
                    auto src_phys_reg = src_reg->reg;

                    it = block->erase(it);

                    if (src_reg != dst_reg) {
                        if (src_phys_reg.type.data_type == MachineDataType::INT) {
                            block->insert(it, rvconstructor->ConstructR(
                                RISCV_ADD, dst_phys_reg, src_phys_reg, GetPhysicalReg(RISCV_x0)));
                        } else if (src_phys_reg.type.data_type == MachineDataType::FLOAT) {
                            block->insert(it, rvconstructor->ConstructR2(
                                RISCV_FMV_S, dst_phys_reg, src_phys_reg));
                        } else {
                            ERROR("Unknown Machine Data Type");
                        }
                    }
                    --it;  // 回退到刚插入的指令上
                } 
                // 未知的拷贝类型
                else {
                    ERROR("Unknown Machine Operand Type");
                }
            }
        }
    }
}

void RiscV64LowerIImmCopy::Execute() {
    for (auto& function : unit->functions) {
        auto block_it = function->getMachineCFG()->getSeqScanIterator();
        block_it->open();

        while (block_it->hasNext()) {
            auto block = block_it->next()->Mblock;

            for (auto it = block->begin(); it != block->end(); ++it) {
                auto& ins = *it;

                // 仅处理 COPY 指令
                if (ins->arch != MachineBaseInstruction::COPY) {
                    continue;
                }

                auto m_copy = static_cast<MachineCopyInstruction*>(ins);
                Assert(m_copy->GetDst()->op_type == MachineBaseOperand::REG);

                // 仅处理立即数类型的 COPY
                if (m_copy->GetSrc()->op_type != MachineBaseOperand::IMMI) {
                    continue;
                }

                // 禁用指令调度（根据条件）
                if (!ins->CanSchedule()) {
                    rvconstructor->DisableSchedule();
                }

                auto src_immi = static_cast<MachineImmediateInt*>(m_copy->GetSrc());
                auto dst_reg = static_cast<MachineRegister*>(m_copy->GetDst());

                // 处理立即数为 0 的特殊情况
                if (src_immi->imm32 == 0) {
                    it = block->erase(it);
                    block->insert(it, rvconstructor->ConstructCopyReg(dst_reg->reg, GetPhysicalReg(RISCV_x0), INT64));
                    --it;
                }
                // 处理可用12位立即数直接加载的情况
                else if (src_immi->imm32 <= 2047 && src_immi->imm32 >= -2048) {
                    it = block->erase(it);
                    block->insert(it, rvconstructor->ConstructIImm(RISCV_ADDIW, dst_reg->reg, GetPhysicalReg(RISCV_x0), src_immi->imm32));
                    --it;
                }
                // 处理高12位可直接加载的情况
                else if ((src_immi->imm32 & 0xFFF) == 0) {
                    it = block->erase(it);
                    block->insert(it, rvconstructor->ConstructUImm(RISCV_LUI, dst_reg->reg, (static_cast<uint32_t>(src_immi->imm32)) >> 12));
                    --it;
                }
                // 处理需要两条指令加载的大数
                else {
                    auto mid_reg = function->GetNewReg(INT64);
                    it = block->erase(it);

                    // 加载高20位
                    block->insert(it, rvconstructor->ConstructUImm(RISCV_LUI, mid_reg, (static_cast<uint32_t>(src_immi->imm32 + 0x800)) >> 12));
                    
                    // 加载低12位
                    block->insert(it, rvconstructor->ConstructIImm(RISCV_ADDIW, dst_reg->reg, mid_reg, (src_immi->imm32 << 20) >> 20));
                    --it;
                }

                // 重新启用指令调度
                rvconstructor->EnableSchedule();
            }
        }
    }
}

