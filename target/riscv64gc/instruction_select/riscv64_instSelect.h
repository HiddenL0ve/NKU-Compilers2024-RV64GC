#ifndef RISCV64_INSTSELECT_H
#define RISCV64_INSTSELECT_H
#include "../../common/machine_passes/machine_selector.h"
#include "../riscv64.h"
class RiscV64Selector : public MachineSelector {
private:
    std::map<int, Register> llvm_rv_regtable;
    std::map<int, int> llvm_rv_allocas;
    std::map<Uint64, bool> global_imm_vsd;
    int cur_offset;    // 局部变量在栈中的偏移
    // 你需要保证在每个函数的指令选择结束后, cur_offset的值为局部变量所占栈空间的大小
    Register GetllvmReg(int, MachineDataType);
    Register ExtractOp2Reg(BasicOperand *, MachineDataType);
    Register GetNewReg(MachineDataType, bool save_across_call = false);
    int ExtractOp2ImmI32(BasicOperand *);
    float ExtractOp2ImmF32(BasicOperand *);
    // TODO(): 添加更多你需要的成员变量和函数
    std::map<Register, Instruction> cmp_context;
public:
    RiscV64Selector(MachineUnit *dest, LLVMIR *IR) : MachineSelector(dest, IR) {}
    void SelectInstructionAndBuildCFG();
    void ClearFunctionSelectState();
    template <class INSPTR> void ConvertAndAppend(INSPTR);
};
#endif