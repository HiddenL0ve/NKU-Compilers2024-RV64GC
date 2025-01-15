#include "mem2reg.h"
#include <tuple>

static std::set<Instruction> EraseSet;
static std::map<int, int> mem2reg_map;
static std::set<int> common_allocas;    // alloca of common situations
static std::map<PhiInstruction *, int> phi_map;
// 检查该条alloca指令是否可以被mem2reg
// eg. 数组不可以mem2reg
// eg. 如果该指针直接被使用不可以mem2reg(在SysY一般不可能发生,SysY不支持指针语法)
void Mem2RegPass::IsPromotable(CFG *C, Instruction AllocaInst) {}
/*
    int a1 = 5,a2 = 3,a3 = 11,b = 4
    return b // a1,a2,a3 is useless
-----------------------------------------------
pseudo IR is:
    %r0 = alloca i32 ;a1
    %r1 = alloca i32 ;a2
    %r2 = alloca i32 ;a3
    %r3 = alloca i32 ;b
    store 5 -> %r0 ;a1 = 5
    store 3 -> %r1 ;a2 = 3
    store 11 -> %r2 ;a3 = 11
    store 4 -> %r3 ;b = 4
    %r4 = load i32 %r3
    ret i32 %r4
--------------------------------------------------
%r0,%r1,%r2只有store, 但没有load,所以可以删去
优化后的IR(pseudo)为:
    %r3 = alloca i32
    store 4 -> %r3
    %r4 - load i32 %r3
    ret i32 %r4
*/

bool HandleStore(StoreInstruction *StoreI, int regnum, std::set<int> &vset) {
    int ty = StoreI->GetPointer()->GetOperandType();
    if (ty != BasicOperand::REG) {
        return false;
    }
    if (vset.end() == vset.find(regnum)) {
        return false;
    }
    return true;
}

bool HandleLoad(LoadInstruction *LoadI, int regnum, std::set<int> &vset) {
    int ty = LoadI->GetPointer()->GetOperandType();
    if (ty != BasicOperand::REG) {
        return false;
    }
    if (vset.end() == vset.find(regnum)) {
        return false;
    }
    return true;
}

// vset is the set of alloca regno that only store but not load
// 该函数对你的时间复杂度有一定要求, 你需要保证你的时间复杂度小于等于O(nlognlogn), n为该函数的指令数
// 提示:deque直接在中间删除是O(n)的, 可以先标记要删除的指令, 最后想一个快速的方法统一删除
void Mem2RegPass::Mem2RegNoUseAlloca(CFG *C, std::set<int> &vset) {
    // TODO("Mem2RegNoUseAlloca");
    // this function is used in InsertPhi
    for (auto [id, b] : *C->block_map) {
        for (auto I : b->Instruction_list) {
            if (I->GetOpcode() == BasicInstruction::LLVMIROpcode::STORE) {
                StoreInstruction *StoreI = (StoreInstruction *)I;
                int regnum = StoreI->GetDefRegNum();
                if(HandleStore(StoreI, regnum, vset)){
                    EraseSet.insert(I);
                }
            }
        }
    }

}

/*
    int b = getint();
    b = b + 10
    return b // def and use of b are in same block
-----------------------------------------------
pseudo IR is:
    %r0 = alloca i32 ;b
    %r1 = call getint()
    store %r1 -> %r0
    %r2 = load i32 %r0
    %r3 = %r2 + 10
    store %r3 -> %r0
    %r4 = load i32 %r0
    ret i32 %r4
--------------------------------------------------
%r0的所有load和store都在同一个基本块内
优化后的IR(pseudo)为:
    %r1 = call getint()
    %r3 = %r1 + 10
    ret %r3

对于每一个load，我们只需要找到最近的store,然后用store的值替换之后load的结果即可
*/

// vset is the set of alloca regno that load and store are all in the BB block_id
// 该函数对你的时间复杂度有一定要求，你需要保证你的时间复杂度小于等于O(nlognlogn), n为该函数的指令数

void Mem2RegPass::Mem2RegUseDefInSameBlock(CFG *C, std::set<int> &vset, int block_id) {
    // this function is used in InsertPhi
    std::map<int, int> curr_reg_map;
    for (auto I : (*C->block_map)[block_id]->Instruction_list) {
        if (I->GetOpcode() == BasicInstruction::LLVMIROpcode::STORE) {
            StoreInstruction *StoreI = (StoreInstruction *)I;
            int regnum = StoreI->GetDefRegNum();
            if(HandleStore(StoreI, regnum, vset)){
                int r=((RegOperand *)(StoreI->GetValue()))->GetRegNo();
                    if(curr_reg_map[r]>0)
                    {curr_reg_map[regnum]=curr_reg_map[r];}
                    else
                    curr_reg_map[regnum] = ((RegOperand *)(StoreI->GetValue()))->GetRegNo();
                    
                    EraseSet.insert(I);
                    
                    //printf(" %d %d \n",regnum,curr_reg_map[regnum]);
            }
        }

        if (I->GetOpcode() == BasicInstruction::LLVMIROpcode::LOAD) {
            LoadInstruction *LoadI = (LoadInstruction *)I;
            int regnum = LoadI->GetUseRegNum();
            if(HandleLoad(LoadI, regnum, vset)){
                mem2reg_map[LoadI->GetResultRegNo()] = curr_reg_map[regnum];
                curr_reg_map[LoadI->GetResultRegNo()] =curr_reg_map[regnum];
                
                EraseSet.insert(I);
            }
            //%r0 = alloca i32 ;b
            // %r1 = call getint()
            // store %r1 -> %r0
            //%r4 = load i32 %r0   GetResultRegNo:%r4 curr_reg_map[regnum]:%r0
        }
    }
    // TODO("Mem2RegUseDefInSameBlock");
}

// vset is the set of alloca regno that one store dominators all load instructions
// 该函数对你的时间复杂度有一定要求，你需要保证你的时间复杂度小于等于O(nlognlogn)
void Mem2RegPass::Mem2RegOneDefDomAllUses(CFG *C, std::set<int> &vset) {
    // this function is used in InsertPhi
    std::map<int, int> curr_reg_map;
    for (auto [id, b] : *C->block_map) {
        for (auto I : b->Instruction_list) {
            if (I->GetOpcode() == BasicInstruction::LLVMIROpcode::STORE) {
                StoreInstruction *StoreI = (StoreInstruction *)I;
                int regnum = StoreI->GetDefRegNum();
                if(HandleStore(StoreI, regnum, vset)){
                    int r=((RegOperand *)(StoreI->GetValue()))->GetRegNo();
                    if(curr_reg_map[r]>0)
                    curr_reg_map[regnum]=curr_reg_map[r];
                    else
                    curr_reg_map[regnum] = ((RegOperand *)(StoreI->GetValue()))->GetRegNo();
                    EraseSet.insert(I);
                }
            }
        }
    }    // TODO("Mem2RegOneDefDomAllUses");
    for (auto [id, b] : *C->block_map) {
        for (auto I : b->Instruction_list) {
            if (I->GetOpcode() == BasicInstruction::LLVMIROpcode::LOAD) {
                LoadInstruction *LoadI = (LoadInstruction *)I;
                int regnum = LoadI->GetUseRegNum();
                if(HandleLoad(LoadI, regnum, vset)){
                    mem2reg_map[LoadI->GetResultRegNo()] = curr_reg_map[regnum];
                    curr_reg_map[LoadI->GetResultRegNo()] =curr_reg_map[regnum];
                    EraseSet.insert(I);
                }
            }
        }
    }
    
    // TODO("Mem2RegOneDefDomAllUses");
}
void Mem2RegPass::InsertPhi(CFG *cfg) {
    // 获取变量的定义、使用以及定义次数
    std::map<int, std::set<int>> defs, uses;
    std::map<int, int> def_counts;

    for (const auto &[block_id, block] : *cfg->block_map) {
        for (auto instruction : block->Instruction_list) {
            if (instruction->GetOpcode() == BasicInstruction::LLVMIROpcode::STORE) {
                auto store_inst = static_cast<StoreInstruction *>(instruction);

                // 忽略全局变量的存储操作
                if (store_inst->GetPointer()->GetOperandType() == BasicOperand::GLOBAL) {
                    continue;
                }

                int def_reg_num = store_inst->GetDefRegNum();
                defs[def_reg_num].insert(block_id);
                def_counts[def_reg_num]++;
            } else if (instruction->GetOpcode() == BasicInstruction::LLVMIROpcode::LOAD) {
                auto load_inst = static_cast<LoadInstruction *>(instruction);

                // 忽略全局变量的加载操作
                if (load_inst->GetPointer()->GetOperandType() == BasicOperand::GLOBAL) {
                    continue;
                }

                int use_reg_num = load_inst->GetUseRegNum();
                uses[use_reg_num].insert(block_id);
            }
        }
    }

    // 获取入口基本块
    LLVMBlock entry_block = (*cfg->block_map)[0];

    // 用于分类Alloca的集合
    std::set<int> unused_vars;       // 无使用的变量
    std::set<int> single_dom_vars;   // 单次定义且所有使用都在其支配范围内的变量
    std::map<int, std::set<int>> same_block_defs; // 定义和使用在同一基本块的变量
    DominatorTree *dom_tree = domtrees->GetDomTree(cfg);

    // 遍历入口基本块中的所有指令，找到Alloca指令
    for (auto instruction : entry_block->Instruction_list) {
        if (instruction->GetOpcode() != BasicInstruction::LLVMIROpcode::ALLOCA) {
            continue;
        }

        auto alloca_inst = static_cast<AllocaInstruction *>(instruction);

        // 忽略数组类型的Alloca指令
        if (!alloca_inst->GetDims().empty()) {
            continue;
        }

        int reg_num = alloca_inst->GetResultRegNo();
        auto def_blocks = defs[reg_num];
        auto use_blocks = uses[reg_num];

        // 情况1：无使用的变量，直接删除Alloca
        if (use_blocks.empty()) {
            EraseSet.insert(instruction);
            unused_vars.insert(reg_num);
            continue;
        }

        // 情况2：定义和使用都在同一基本块
        if (def_blocks.size() == 1 && use_blocks.size() == 1 && *use_blocks.begin() == *def_blocks.begin()) {
            EraseSet.insert(instruction);
            same_block_defs[*def_blocks.begin()].insert(reg_num);
            continue;
        }

        // 情况3：定义次数为1且所有使用都在定义的支配范围内
        if (def_counts[reg_num] == 1) {
            int def_block_id = *def_blocks.begin();
            bool all_dominated = true;

            for (int use_block_id : use_blocks) {
                if (!dom_tree->IsDominate(def_block_id, use_block_id)) {
                    all_dominated = false;
                    break;
                }
            }

            if (all_dominated) {
                EraseSet.insert(instruction);
                single_dom_vars.insert(reg_num);
                continue;
            }
        }

        // 情况4：需要插入Phi指令的普通变量
        common_allocas.insert(reg_num);
        EraseSet.insert(instruction);

        // 插入Phi指令的过程
        std::set<int> inserted_phi_blocks; // 已插入Phi指令的基本块集合
        std::set<int> worklist = defs[reg_num]; // 初始化工作列表为所有定义所在的基本块

        while (!worklist.empty()) {
            int current_block = *worklist.begin();
            worklist.erase(current_block);
           // printf(" %d%d ",100,current_block);
            for (int frontier_block : dom_tree->GetDF(current_block)) {
                // 如果该基本块还没有插入Phi指令
                if (inserted_phi_blocks.find(frontier_block) == inserted_phi_blocks.end()) {
                    // 创建Phi指令并插入到基本块开头
                    BasicInstruction::LLVMType var_type = alloca_inst->GetDataType();
                    PhiInstruction *phi_inst = new PhiInstruction(var_type, GetNewRegOperand(++cfg->max_reg));
                    (*cfg->block_map)[frontier_block]->InsertInstruction(0, phi_inst);
                    //printf(" %d ",frontier_block);
                    // 将Phi指令与变量reg_num映射
                    phi_map[phi_inst] = reg_num;

                    // 将该基本块加入已插入Phi指令的集合
                    inserted_phi_blocks.insert(frontier_block);

                    // 如果该基本块没有定义reg_num，则加入工作列表
                    if (defs[reg_num].find(frontier_block) == defs[reg_num].end()) {
                        worklist.insert(frontier_block);
                    }
                }
            }
        }
    }

    // 后续处理不同分类的Alloca变量
    Mem2RegNoUseAlloca(cfg, unused_vars);
    for (auto &[block_id, var_set] : same_block_defs) {
        Mem2RegUseDefInSameBlock(cfg, var_set, block_id);
    }
    Mem2RegOneDefDomAllUses(cfg, single_dom_vars);
   
}
int Mem2RegPass::getAllocaRegIfUsed(std::set<int> &allocasSet, Instruction inst) {
    // 检查是否是对 allocas 中变量的操作
    if (inst->GetOpcode() == BasicInstruction::LLVMIROpcode::LOAD) {
        auto loadInst = static_cast<LoadInstruction *>(inst);
        if (loadInst->GetPointer()->GetOperandType() != BasicOperand::REG) {
            return -1;
        }
        int pointerReg = loadInst->GetUseRegNum();
        if (allocasSet.find(pointerReg) != allocasSet.end()) {
            return pointerReg;
        }
    } else if (inst->GetOpcode() == BasicInstruction::LLVMIROpcode::STORE) {
        auto storeInst = static_cast<StoreInstruction *>(inst);//store <ty> <value>, ptr<pointer> 得到pointer
        if (storeInst->GetPointer()->GetOperandType() != BasicOperand::REG) {
            return -1;
        }
        int pointerReg = storeInst->GetDefRegNum();
        if (allocasSet.find(pointerReg) != allocasSet.end()) {
            return pointerReg;
        }
    }
    return -1;
}
void Mem2RegPass::VarRename(CFG *C) {
    // 初始化工作队列 WorkList，key 是基本块 ID，value 是当前基本块的 alloca 到寄存器的映射
    std::map<int, std::map<int, int>> WorkList;  
    WorkList[0] = {};  // 初始块（入口块）默认没有任何值
    std::vector<int> BBVisited(C->max_label + 1, 0);  // 标记基本块是否已经访问

    // 遍历工作队列，直到所有基本块都处理完成
    while (!WorkList.empty()) {
        // 取出当前处理的基本块和对应的 incoming 值
        auto [BB, IncomingVals] = *WorkList.begin();
        WorkList.erase(WorkList.begin());

        if (BBVisited[BB]) {
            continue;  // 如果已经访问过这个基本块，则跳过
        }
        BBVisited[BB] = 1;  // 标记当前基本块为已访问

        // 遍历当前基本块中的指令
        for (auto &I : (*C->block_map)[BB]->Instruction_list) {
            if (I->GetOpcode() == BasicInstruction::LLVMIROpcode::LOAD) {
                // 处理 LOAD 指令
                auto LoadI = static_cast<LoadInstruction *>(I);
                int regNum = getAllocaRegIfUsed(common_allocas, I);  // 判断是否是对 alloca 的操作
                if (regNum >= 0) {
                    EraseSet.insert(LoadI);  // 标记 LOAD 指令为待删除
                    mem2reg_map[LoadI->GetResultRegNo()] = IncomingVals[regNum];  // 替换结果寄存器
                    IncomingVals[LoadI->GetResultRegNo()]=IncomingVals[regNum];
                }
            } else if (I->GetOpcode() == BasicInstruction::LLVMIROpcode::STORE) {
                // 处理 STORE 指令
                auto StoreI = static_cast<StoreInstruction *>(I);
                int regNum = getAllocaRegIfUsed(common_allocas, I);  // 判断是否是对 alloca 的操作
                if (regNum >= 0) {
                    EraseSet.insert(StoreI);  // 标记 STORE 指令为待删除
                    int r=static_cast<RegOperand *>(StoreI->GetValue())->GetRegNo();
                    //printf("%d",r);
                    if(IncomingVals[r]>0)
                    IncomingVals[regNum] = IncomingVals[r];  // 更新 incoming 值
                    else 
                    IncomingVals[regNum]=r;
                }
            } else if (I->GetOpcode() == BasicInstruction::LLVMIROpcode::PHI) {
                // 处理 PHI 指令
                auto PhiI = static_cast<PhiInstruction *>(I);
                if (EraseSet.find(PhiI) != EraseSet.end()) {
                    continue;  // 如果 PHI 指令已标记为删除，则跳过
                }
                auto it = phi_map.find(PhiI);
                if (it != phi_map.end()) {  // 如果 PHI 指令是针对 alloca 的
                    IncomingVals[it->second] = PhiI->GetResultRegNo();  // 更新 IncomingVals
                }
            }
        }

        // 更新后继基本块
        for (auto succ : C->G[BB]) {
            int SuccBB = succ->block_id;
            WorkList[SuccBB] = IncomingVals;  // 将当前基本块的 IncomingVals 传递给后继块

            // 更新后继块中的 PHI 指令
            for (auto I : (*C->block_map)[SuccBB]->Instruction_list) {
                if (I->GetOpcode() != BasicInstruction::LLVMIROpcode::PHI) {
                    break;  // 只处理 PHI 指令，遇到非 PHI 指令停止
                }
                auto PhiI = static_cast<PhiInstruction *>(I);
                auto it = phi_map.find(PhiI);
                if (it != phi_map.end()) {  // 如果 PHI 指令是针对 alloca 的
                    int regNum = it->second;
                    if (IncomingVals.find(regNum) == IncomingVals.end()) {
                        EraseSet.insert(PhiI);  // 如果没有值可用，则删除 PHI 指令
                        continue;
                    }
                    // 为 PHI 添加前驱块的值和标签
                    PhiI->InsertPhi(GetNewRegOperand(IncomingVals[regNum]), GetNewLabelOperand(BB));
                }
            }
        }
    }
    for (auto [id, bb] : *C->block_map) {
        for (auto I : bb->Instruction_list) {
            if (I->GetOpcode() == BasicInstruction::LLVMIROpcode::LOAD && ((LoadInstruction *)I)->GetPointer()->GetOperandType() == BasicOperand::REG) {
                int result = ((LoadInstruction *)I)->GetResultRegNo();
                if (mem2reg_map.find(result) != mem2reg_map.end()) {
                    int result2 = mem2reg_map[result];
                    while (mem2reg_map.find(result2) != mem2reg_map.end()) {
                        mem2reg_map[result] = mem2reg_map[result2];
                        result2 = mem2reg_map[result];
                    }
                }
            }
        }
    } 
    // 删除标记为待删除的指令
    for (auto &[id, bb] : *C->block_map) {
        auto tmpInstructionList = bb->Instruction_list;
        bb->Instruction_list.clear();
        for (auto I : tmpInstructionList) {
            if (EraseSet.find(I) != EraseSet.end()) {
                continue;  // 跳过待删除的指令
            }
            bb->InsertInstruction(1, I);  // 保留未删除的指令
        }
    }

    // 替换指令中的寄存器
    for (auto &[id, bb] : *C->block_map) {
        for (auto I : bb->Instruction_list) {
            I->Renamereg(mem2reg_map);
        }
    }

    // 清理临时数据结构
    EraseSet.clear();
    mem2reg_map.clear();
    common_allocas.clear();
    phi_map.clear();
}

void Mem2RegPass::Mem2Reg(CFG *C) {
    InsertPhi(C);
    VarRename(C);
}

void Mem2RegPass::Execute() {
    for (auto [defI, cfg] : llvmIR->llvm_cfg) {
        Mem2Reg(cfg);
    }
}