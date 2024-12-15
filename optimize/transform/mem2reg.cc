#include "mem2reg.h"
#include <tuple>

static std::set<Instruction> EraseSet;
static std::map<int, int> mem2reg_map;
static std::set<int> common_allocas;    // alloca of common situations
static std::map<PhiInstruction *, int> phi_map;
// 检查该条alloca指令是否可以被mem2reg
// eg. 数组不可以mem2reg
// eg. 如果该指针直接被使用不可以mem2reg(在SysY一般不可能发生,SysY不支持指针语法)
void Mem2RegPass::IsPromotable(CFG *C, Instruction AllocaInst) { TODO("IsPromotable"); }
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

// vset is the set of alloca regno that only store but not load
// 该函数对你的时间复杂度有一定要求, 你需要保证你的时间复杂度小于等于O(nlognlogn), n为该函数的指令数
// 提示:deque直接在中间删除是O(n)的, 可以先标记要删除的指令, 最后想一个快速的方法统一删除
void Mem2RegPass::Mem2RegNoUseAlloca(CFG *C, std::set<int> &vset) {
    // this function is used in InsertPhi
    for (auto [id, b] : *C->block_map) {
        for (auto Ins : b->Instruction_list) {
            if (Ins->GetOpcode() == BasicInstruction::LLVMIROpcode::STORE) {
                auto S = (StoreInstruction *)Ins;
                int ty = S->GetPointer()->GetOperandType();
                if (ty != BasicOperand::REG) {
                    continue;
                }
                int regnum = S->GetDefRegNum();
                if (vset.end() == vset.find(regnum)) {
                    continue;
                }
                EraseSet.insert(Ins);
            }
        }
    }
    // for (auto [id, bb] : *C->block_map) {
    //     auto tmp_Instruction_list = bb->Instruction_list;
    //     bb->Instruction_list.clear();
    //     for (auto I : tmp_Instruction_list) {
    //         if (EraseSet.find(I) != EraseSet.end()) {
    //             continue;
    //         }
    //         bb->InsertInstruction(1, I);
    //     }
    // }
    // TODO("Mem2RegNoUseAlloca");
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
            auto StoreI = (StoreInstruction *)I;
            int ty = StoreI->GetPointer()->GetOperandType();
            if (ty != BasicOperand::REG) {
                continue;
            }
            int regnum = StoreI->GetDefRegNum();
            if (vset.end() == vset.find(regnum)) {
                continue;
            }
            curr_reg_map[regnum] = ((RegOperand *)(StoreI->GetValue()))->GetRegNo();    // store进的目标寄存器
            EraseSet.insert(I);
        }
        if (I->GetOpcode() == BasicInstruction::LLVMIROpcode::LOAD) {
            auto LoadI = (LoadInstruction *)I;
            int ty = LoadI->GetPointer()->GetOperandType();
            if (ty != BasicOperand::REG) {
                continue;
            }
            int regnum = LoadI->GetUseRegNum();
            if (vset.end() == vset.find(regnum)) {
                continue;
            }
            mem2reg_map[LoadI->GetResultRegNo()] = curr_reg_map[regnum];    // 得到之前进行store的目标寄存器
            //%r0 = alloca i32 ;b
            // %r1 = call getint()
            // store %r1 -> %r0
            //%r4 = load i32 %r0   GetResultRegNo:%r4 curr_reg_map[regnum]:%r0
            EraseSet.insert(I);
        }
    }
    // for (auto [id, bb] : *C->block_map) {
    //     auto tmp_Instruction_list = bb->Instruction_list;
    //     bb->Instruction_list.clear();
    //     for (auto I : tmp_Instruction_list) {
    //         if (EraseSet.find(I) != EraseSet.end()) {
    //             continue;
    //         }
    //         bb->InsertInstruction(1, I);
    //     }
    // }
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
                auto StoreI = (StoreInstruction *)I;
                int ty = StoreI->GetPointer()->GetOperandType();
                if (ty != BasicOperand::REG) {
                    continue;
                }
                int regnum = StoreI->GetDefRegNum();
                if (vset.end() == vset.find(regnum)) {
                    continue;
                }
                curr_reg_map[regnum] = ((RegOperand *)(StoreI->GetValue()))->GetRegNo();
                EraseSet.insert(I);
            }
        }
    }    // TODO("Mem2RegOneDefDomAllUses");
    for (auto [id, b] : *C->block_map) {
        for (auto I : b->Instruction_list) {
            if (I->GetOpcode() == BasicInstruction::LLVMIROpcode::LOAD) {
                auto LoadI = (LoadInstruction *)I;
                int ty = LoadI->GetPointer()->GetOperandType();
                if (ty != BasicOperand::REG) {
                    continue;
                }
                int regnum = LoadI->GetUseRegNum();
                if (vset.end() == vset.find(regnum)) {
                    continue;
                }
                mem2reg_map[LoadI->GetResultRegNo()] = curr_reg_map[regnum];
                EraseSet.insert(I);
            }
        }
    }
    // for (auto [id, bb] : *C->block_map) {
    //     auto tmp_Instruction_list = bb->Instruction_list;
    //     bb->Instruction_list.clear();
    //     for (auto I : tmp_Instruction_list) {
    //         if (EraseSet.find(I) != EraseSet.end()) {
    //             continue;
    //         }
    //         bb->InsertInstruction(1, I);
    //     }
    // }
    // TODO("Mem2RegOneDefDomAllUses");
}
auto CalculatedDefAndUse(CFG *C) {
    // set of basic blocks id that contain definitions(uses) of key
    std::map<int, std::set<int>> defs, uses;
    // key is the alloca register, value is the definitions number of this register
    std::map<int, int> def_num;
    for (auto [id, BB] : *C->block_map) {
        for (auto I : BB->Instruction_list) {
            if (I->GetOpcode() == BasicInstruction::LLVMIROpcode::STORE) {
                auto StoreI = (StoreInstruction *)I;
                if (StoreI->GetPointer()->GetOperandType() == BasicOperand::GLOBAL) {
                    continue;
                }
                defs[StoreI->GetDefRegNum()].insert(id);
                def_num[StoreI->GetDefRegNum()]++;
            } else if (I->GetOpcode() == BasicInstruction::LLVMIROpcode::LOAD) {
                auto LoadI = (LoadInstruction *)I;
                if (LoadI->GetPointer()->GetOperandType() == BasicOperand::GLOBAL) {
                    continue;
                }
                uses[LoadI->GetUseRegNum()].insert(id);
            }
        }
    }
    return std::tuple(defs, uses, def_num);
}
void Mem2RegPass::InsertPhi(CFG *C) {
    auto [defs, uses, def_num] = CalculatedDefAndUse(C);
    LLVMBlock entry_BB = (*C->block_map)[0];
    std::set<int> no_use_vset, onedom_vset;
    std::map<int, std::set<int>> sameblock_vset_map;
    DominatorTree *tree = domtrees->GetDomTree(C);

    printf("%d", *(defs[0].begin()));
    for (auto I : entry_BB->Instruction_list) {
        if (I->GetOpcode() != BasicInstruction::LLVMIROpcode::ALLOCA) {
            continue;
        }

        auto AllocaI = (AllocaInstruction *)I;
        if (!(AllocaI->GetDims().empty())) {
            continue;
        }    // array can not be promoted
        int v = AllocaI->GetResultRegNo();
        BasicInstruction::LLVMType type = AllocaI->GetDataType();

        auto alloca_defs = defs[v];
        auto alloca_uses = uses[v];
        if (alloca_uses.size() == 0) {    // not use
            EraseSet.insert(I);
            no_use_vset.insert(v);
            continue;
        }

        if (alloca_defs.size() == 1) {
            int block_id = *(alloca_defs.begin());
            if (alloca_uses.size() == 1 && *(alloca_uses.begin()) == block_id) {    // def and use in the same block
                EraseSet.insert(I);
                sameblock_vset_map[block_id].insert(v);
                continue;
            }
        }
        if (def_num[v] == 1) {    // only def once
            int block_id = *(alloca_defs.begin());
            int dom_flag = 1;
            for (auto load_BBid : alloca_uses) {
                if (tree->IsDominate(block_id, load_BBid) == false) {
                    dom_flag = 0;
                    break;
                }
            }
            if (dom_flag) {    // one def dominate all uses
                EraseSet.insert(I);
                onedom_vset.insert(v);
                continue;
            }
        }
        common_allocas.insert(v);
        EraseSet.insert(I);
        std::set<int> F{};            // set of blocks where phi is added
        std::set<int> W = defs[v];    // set of blocks that contain the def of regno

        while (!W.empty()) {
            int BB_X = *W.begin();
            W.erase(BB_X);
            for (auto BB_Y : tree->GetDF(BB_X)) {
                // std::cout<<v<<" "<<BB_X<<" "<<BB_Y<<"\n";
                if (F.find(BB_Y) == F.end()) {
                    PhiInstruction *PhiI = new PhiInstruction(type, GetNewRegOperand(++C->max_reg));
                    (*C->block_map)[BB_Y]->InsertInstruction(0, PhiI);
                    phi_map[PhiI] = v;
                    F.insert(BB_Y);
                    if (defs[v].find(BB_Y) == defs[v].end()) {
                        W.insert(BB_Y);
                    }
                }
            }
        }
    }
    Mem2RegNoUseAlloca(C, no_use_vset);
    Mem2RegOneDefDomAllUses(C, onedom_vset);
    for (auto [id, vset] : sameblock_vset_map) {
        Mem2RegUseDefInSameBlock(C, vset, id);
    }
    // for (auto [id, bb] : *C->block_map) {
    //     auto tmp_Instruction_list = bb->Instruction_list;
    //     bb->Instruction_list.clear();
    //     for (auto I : tmp_Instruction_list) {
    //         if (EraseSet.find(I) != EraseSet.end()) {
    //             continue;
    //         }
    //         bb->InsertInstruction(1, I);
    //     }
    // }

    // TODO("InsertPhi");
}
int Mem2RegPass::in_allocas(std::set<int> &S, Instruction I) {
    if (I->GetOpcode() == BasicInstruction::LLVMIROpcode::LOAD) {
        auto LoadI = (LoadInstruction *)I;
        if (LoadI->GetPointer()->GetOperandType() != BasicOperand::REG) {
            return -1;
        }
        int pointer = LoadI->GetUseRegNum();
        if (S.find(pointer) != S.end()) {
            return pointer;
        }
    }
    if (I->GetOpcode() == BasicInstruction::LLVMIROpcode::STORE) {
        auto StoreI = (StoreInstruction *)I;
        if (StoreI->GetPointer()->GetOperandType() != BasicOperand::REG) {
            return -1;
        }
        int pointer = StoreI->GetDefRegNum();
        if (S.find(pointer) != S.end()) {
            return pointer;
        }
    }
    return -1;
}
void Mem2RegPass::VarRename(CFG *C) {
    // TODO("VarRename");
    std::map<int, std::map<int, int>> WorkList;    //< BB, <alloca_reg,val_reg> >
    WorkList.insert({0, std::map<int, int>{}});
    std::vector<int> BBvis;
    BBvis.resize(C->max_label + 1);
    while (!WorkList.empty()) {
        int BB = (*WorkList.begin()).first;
        auto IncomingVals = (*WorkList.begin()).second;
        WorkList.erase(BB);
        if (BBvis[BB]) {
            continue;
        }
        BBvis[BB] = 1;
        for (auto &I : (*C->block_map)[BB]->Instruction_list) {
            if (I->GetOpcode() == BasicInstruction::LLVMIROpcode::LOAD) {
                auto LoadI = (LoadInstruction *)I;
                int v = in_allocas(common_allocas, I);
                if (v >= 0) {    // load instruction is in common_allocas
                    // 如果当前指令是 load，找到对应的 alloca v，将用到 load 结果的地方都替换成
                    // IncomingVals[v]
                    EraseSet.insert(LoadI);
                    mem2reg_map[LoadI->GetResultRegNo()] = IncomingVals[v];
                }
            }
            if (I->GetOpcode() == BasicInstruction::LLVMIROpcode::STORE) {
                auto StoreI = (StoreInstruction *)I;
                int v = in_allocas(common_allocas, I);
                if (v >= 0) {    // store instruction is in common_allocas
                    // 如果当前指令是 store，找到对应的 alloca v，更新IncomingVals[v] = val,并删除store
                    EraseSet.insert(StoreI);
                    IncomingVals[v] = ((RegOperand *)(StoreI->GetValue()))->GetRegNo();
                }
            }
            if (I->GetOpcode() == BasicInstruction::LLVMIROpcode::PHI) {
                auto PhiI = (PhiInstruction *)I;
                if (EraseSet.find(PhiI) != EraseSet.end()) {
                    continue;
                }
                auto it = phi_map.find(PhiI);
                if (it != phi_map.end()) {    // phi instruction is in allocas
                    // 更新IncomingVals[v] = val
                    IncomingVals[it->second] = PhiI->GetResultRegNo();
                }
            }
        }
        for (auto succ : C->G[BB]) {
            int BBv = succ->block_id;
            WorkList.insert({BBv, IncomingVals});
            for (auto I : (*C->block_map)[BBv]->Instruction_list) {
                if (I->GetOpcode() != BasicInstruction::LLVMIROpcode::PHI) {
                    break;
                }
                auto PhiI = (PhiInstruction *)I;
                // 找到 phi 对应的 alloca
                auto it = phi_map.find(PhiI);
                if (it != phi_map.end()) {
                    int v = it->second;
                    if (IncomingVals.find(v) == IncomingVals.end()) {
                        EraseSet.insert(PhiI);
                        continue;
                    }
                    // 为 phi 添加前驱块到当前块的边
                    PhiI->InsertPhi(GetNewRegOperand(IncomingVals[v]), GetNewLabelOperand(BB));
                }
            }
        }
    }

    for (auto [id, bb] : *C->block_map) {
        for (auto I : bb->Instruction_list) {
            if (I->GetOpcode() == BasicInstruction::LLVMIROpcode::LOAD &&
                ((LoadInstruction *)I)->GetPointer()->GetOperandType() == BasicOperand::REG) {

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

    for (auto [id, bb] : *C->block_map) {
        auto tmp_Instruction_list = bb->Instruction_list;
        bb->Instruction_list.clear();
        for (auto I : tmp_Instruction_list) {
            if (EraseSet.find(I) != EraseSet.end()) {
                continue;
            }
            bb->InsertInstruction(1, I);
        }
    }

    for (auto B1 : *C->block_map) {
        for (auto I : B1.second->Instruction_list) {
            // replace mem2reg_map
            I->ReplaceRegByMap(mem2reg_map);
        }
    }

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