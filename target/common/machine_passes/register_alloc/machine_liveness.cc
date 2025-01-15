#include "../../machine_instruction_structures/machine.h"
#include "liveinterval.h"

// 为了实现方便，这里直接使用set进行活跃变量分析，如果你不满意，可以自行更换更高效的数据结构(例如bitset)
template <class T> std::set<T> SetIntersect(const std::set<T> &a, const std::set<T> &b) {
    std::set<T> ret;
    for (auto x : b) {
        if (a.count(x) != 0) {
            ret.insert(x);
        }
    }
    return ret;
}

template <class T> std::set<T> SetUnion(const std::set<T> &a, const std::set<T> &b) {
    std::set<T> ret(a);
    for (auto x : b) {
        ret.insert(x);
    }
    return ret;
}

// a-b
template <class T> std::set<T> SetDiff(const std::set<T> &a, const std::set<T> &b) {
    std::set<T> ret(a);
    for (auto x : b) {
        ret.erase(x);
    }
    return ret;
}

std::vector<Register *> MachinePhiInstruction::GetReadReg() {
    std::vector<Register *> ret;
    for (auto [label, op] : phi_list) {
        if (op->op_type == MachineBaseOperand::REG) {
            ret.push_back(&(((MachineRegister *)op)->reg));
        }
    }
    return ret;
}
std::vector<Register *> MachinePhiInstruction::GetWriteReg() { return std::vector<Register *>({&result}); }
#ifdef DEBUG
#define DEBUG_PRINT(...) fprintf(stderr, __VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#endif

void Liveness::UpdateDefUse() {

    DEF.clear();
    USE.clear();

    auto mcfg = current_func->getMachineCFG();
    // 顺序遍历每个基本块
    auto seq_it = mcfg->getSeqScanIterator();
    seq_it->open();
    while (seq_it->hasNext()) {
        auto node = seq_it->next();

        // DEF[B]: 在基本块B中定义，并且定义前在B中没有被使用的变量集合
        // USE[B]: 在基本块B中使用，并且使用前在B中没有被定义的变量集合
        DEF[node->Mblock->getLabelId()].clear();
        USE[node->Mblock->getLabelId()].clear();

        auto &cur_def = DEF[node->Mblock->getLabelId()];
        auto &cur_use = USE[node->Mblock->getLabelId()];

        for (auto ins : *(node->Mblock)) {
            for (auto reg_r : ins->GetReadReg()) {
                if (cur_def.find(*reg_r) == cur_def.end()) {
                    cur_use.insert(*reg_r);
                }
            }
            for (auto reg_w : ins->GetWriteReg()) {
                if (cur_use.find(*reg_w) == cur_use.end()) {
                    cur_def.insert(*reg_w);
                }
            }
        }
    }
}
template <size_t N> 
std::bitset<N> BitsetUnion(const std::bitset<N> &a, const std::bitset<N> &b) {
    return a | b;
}

template <size_t N> 
std::bitset<N> BitsetDiff(const std::bitset<N> &a, const std::bitset<N> &b) {
    return a & ~b;
}

void Liveness::Execute() {
    UpdateDefUse();

    OUT.clear();
    IN.clear();

    auto mcfg = current_func->getMachineCFG();
    bool changed = true;

    // 使用逆后序遍历优化收敛速度
    auto seq_it = mcfg->getReversePostorderIterator();
    seq_it->open();

    while (changed) {
        changed = false;
        while (seq_it->hasNext()) {
            auto node = seq_it->next();
            int cur_id = node->Mblock->getLabelId();

            // 计算 OUT[B]
            auto old_out = OUT[cur_id];
            OUT[cur_id].reset();
            for (auto succ : mcfg->GetSuccessorsByBlockId(cur_id)) {
                OUT[cur_id] |= IN[succ->Mblock->getLabelId()];
            }
            if (OUT[cur_id] != old_out) {
                changed = true;
            }

            // 计算 IN[B]
            auto old_in = IN[cur_id];
            IN[cur_id] = USE[cur_id] | (OUT[cur_id] & ~DEF[cur_id]);
            if (IN[cur_id] != old_in) {
                changed = true;
            }
        }
    }
}
