#ifndef MACHINE
#define MACHINE

#include "../../../include/ir.h"
#include "../MachineBaseInstruction.h"
#include "../../include/dynamic_bitset.h"
#include "../../include/dynamic_bitset.h"
class MachineFunction;
class MachineBlock;
class MachineUnit;
class MachineCFG;
#include <list>

class MachineBlock {
private:
    int label_id;

protected:
    std::list<MachineBaseInstruction *> instructions;

private:
    std::map<Register, MachineBaseOperand *> parallel_copy_list;    // mov second to first
private:
    std::map<Register, MachineBaseOperand *> parallel_copy_list;    // mov second to first
private:
    MachineFunction *parent;

public:
    int loop_depth = 0;
    std::vector<float> branch_predictor;

    // virtual std::vector<int> getAllBranch() = 0;    // [0]-false, [1]-true
    // virtual void ReverseBranch() = 0;
    virtual std::list<MachineBaseInstruction *>::iterator getInsertBeforeBrIt() = 0;
    void InsertParallelCopyList(Register dst, MachineBaseOperand *src) { parallel_copy_list[dst] = src; }
    decltype(parallel_copy_list) &GetParallelCopyList() { return parallel_copy_list; }
    decltype(instructions) &GetInsList() { return instructions; }
    void clear() { instructions.clear(); }
    auto erase(decltype(instructions.begin()) it) { return instructions.erase(it); }
    auto insert(decltype(instructions.begin()) it, MachineBaseInstruction *ins) { return instructions.insert(it, ins); }
    auto getParent() { return parent; }
    void setParent(MachineFunction *parent) { this->parent = parent; }
    auto getLabelId() { return label_id; }
    auto ReverseBegin() { return instructions.rbegin(); }
    auto ReverseEnd() { return instructions.rend(); }
    auto begin() { return instructions.begin(); }
    auto end() { return instructions.end(); }
    auto size() { return instructions.size(); }
    void push_back(MachineBaseInstruction *ins) { instructions.push_back(ins); }
    void push_front(MachineBaseInstruction *ins) { instructions.push_front(ins); }
    void pop_back() { instructions.pop_back(); }
    void pop_front() { instructions.pop_front(); }
    int getBlockInNumber() {
        for (auto ins : instructions) {
            if (ins->arch != MachineBaseInstruction::COMMENT) {
                 return ins->getNumber() - 1;
            } 
            if (ins->arch != MachineBaseInstruction::COMMENT) {
                 return ins->getNumber() - 1;
            } 
        }
        ERROR("Empty block");
        // return (*(instructions.begin()))->getNumber();
    }
    int getBlockOutNumber() {
        for (auto it = instructions.rbegin(); it != instructions.rend(); ++it) {
            auto ins = *it;
            if (ins->arch != MachineBaseInstruction::COMMENT) {
                return ins->getNumber();
            }
            if (ins->arch != MachineBaseInstruction::COMMENT) {
                return ins->getNumber();
            }
        }
        ERROR("Empty block");
        // return (*(instructions.rbegin()))->getNumber();
    }
    MachineBlock(int id) : label_id(id) {}
};

class MachineBlockFactory {
public:
    virtual MachineBlock *CreateBlock(int id) = 0;
};

class MachineFunction {
private:
    std::string func_name;
    MachineUnit *parent;
    std::vector<Register> parameters;
    int max_exist_label;
    MachineBlockFactory *block_factory;

protected:
    int stack_sz;
    int para_sz;
    bool has_inpara_instack;
    bool has_inpara_instack;
    MachineCFG *mcfg;

public:
    bool HasInParaInStack() { return has_inpara_instack; }
    void SetHasInParaInStack(bool has) { has_inpara_instack = has; }
    bool HasInParaInStack() { return has_inpara_instack; }
    void SetHasInParaInStack(bool has) { has_inpara_instack = has; }
    void UpdateMaxLabel(int labelid) { max_exist_label = max_exist_label > labelid ? max_exist_label : labelid; }
    const decltype(parameters) &GetParameters() { return parameters; }
    void AddParameter(Register reg) { parameters.push_back(reg); }
    void SetStackSize(int sz) { stack_sz = sz; }
    void UpdateParaSize(int parasz) {
        if (parasz > para_sz) {
            para_sz = parasz;
        }
    }
    int GetParaSize() { return para_sz; }
    virtual void AddStackSize(int sz) { stack_sz += sz; }
    int GetStackSize() { return ((stack_sz + 15) / 16) * 16; }
    int GetRaOffsetToSp() { return stack_sz - 8; }
    int GetRaOffsetToSp() { return stack_sz - 8; }
    int GetStackOffset() { return stack_sz; }
    MachineCFG *getMachineCFG() { return mcfg; }
    MachineUnit *getParentMachineUnit() { return parent; }
    std::string getFunctionName() { return func_name; }
    
    
    void SetParent(MachineUnit *parent) { this->parent = parent; }
    void SetMachineCFG(MachineCFG *mcfg) { this->mcfg = mcfg; }

    // private:
    // May be removed in future (?), or become private
    // You can also iterate blocks in MachineCFG
    // private:
    // May be removed in future (?), or become private
    // You can also iterate blocks in MachineCFG
    std::vector<MachineBlock *> blocks{};

public:
    Register GetNewRegister(int regtype, int regwidth, bool save_across_call = false);
    Register GetNewRegister(int regtype, int regwidth, bool save_across_call = false);
    Register GetNewReg(MachineDataType type);

protected:
    // Only change branch instruction, don't change cfg or phi instruction
    // Arch-relevant, should be abstract

    // br_cond is still br_cond, br_uncond is still br_uncond
    virtual void MoveAllPredecessorsBranchTargetToNewBlock(int original_target, int new_target) = 0;
    virtual void MoveOnePredecessorBranchTargetToNewBlock(int pre, int original_target, int new_target) = 0;

    // Branch instruction in block[id] will change to br_uncond,
    // passing original branch instruction on to block[new_block]
    virtual void YankBranchInstructionToNewBlock(int original_block_id, int new_block) = 0;
    virtual void AppendUncondBranchInstructionToNewBlock(int new_block, int br_target) = 0;

    // Only change phi instruction, don't change cfg or branch instruction
    // Arch-irrelevant
    void RedirectPhiNodePredecessor(int phi_block, int old_predecessor, int new_predecessor);

    // Only change branch instruction, don't change cfg or phi instruction
    // Arch-relevant, should be abstract

    // br_cond is still br_cond, br_uncond is still br_uncond
    virtual void MoveAllPredecessorsBranchTargetToNewBlock(int original_target, int new_target) = 0;
    virtual void MoveOnePredecessorBranchTargetToNewBlock(int pre, int original_target, int new_target) = 0;

    // Branch instruction in block[id] will change to br_uncond,
    // passing original branch instruction on to block[new_block]
    virtual void YankBranchInstructionToNewBlock(int original_block_id, int new_block) = 0;
    virtual void AppendUncondBranchInstructionToNewBlock(int new_block, int br_target) = 0;

    // Only change phi instruction, don't change cfg or branch instruction
    // Arch-irrelevant
    void RedirectPhiNodePredecessor(int phi_block, int old_predecessor, int new_predecessor);

    MachineBlock *InitNewBlock();

public:
    // Not implemented by now
    // Not only CFG but also instructions need to be changed
    // There are helper functions (declared above) for you to change branch instructions and phi instructions
    MachineBlock *CreateNewEmptyBlock(std::vector<int> pre, std::vector<int> succ);
    MachineBlock *InsertNewBranchOnlyBlockBetweenEdge(int begin, int end);

    // Not sure if it'll be used
    MachineBlock *InsertNewBranchOnlyPreheader(int id, std::vector<int> pres);
    MachineBlock *InsertNewBranchOnlySuccessorBetweenThisAndAllSuccessors(int id);

public:
    // Not implemented by now
    // Not only CFG but also instructions need to be changed
    // There are helper functions (declared above) for you to change branch instructions and phi instructions
    MachineBlock *CreateNewEmptyBlock(std::vector<int> pre, std::vector<int> succ);
    MachineBlock *InsertNewBranchOnlyBlockBetweenEdge(int begin, int end);

    // Not sure if it'll be used
    MachineBlock *InsertNewBranchOnlyPreheader(int id, std::vector<int> pres);
    MachineBlock *InsertNewBranchOnlySuccessorBetweenThisAndAllSuccessors(int id);

public:
    MachineFunction(std::string name, MachineBlockFactory *blockfactory)
        : func_name(name), stack_sz(0), para_sz(0), block_factory(blockfactory), max_exist_label(0),
          has_inpara_instack(false) {}
        : func_name(name), stack_sz(0), para_sz(0), block_factory(blockfactory), max_exist_label(0),
          has_inpara_instack(false) {}
};

class MachineUnit {
public:
    std::vector<Instruction> global_def{};
    std::vector<MachineFunction *> functions;
};

class MachineNaturalLoop {
public:
    int loop_id;
    std::set<MachineBlock *> loop_nodes;
    std::set<MachineBlock *> exits;
    std::set<MachineBlock *> exitings;
    std::set<MachineBlock *> latches;
    MachineBlock *header;
    MachineBlock *preheader;
    MachineNaturalLoop *fa_loop;
    void FindExitNodes(MachineCFG *C);
};

class MachineNaturalLoopForest {
public:
    int loop_cnt = 0;
    MachineCFG *C;
    std::set<MachineNaturalLoop *> loop_set;
    std::map<MachineBlock *, MachineNaturalLoop *> header_loop_map;
    std::vector<std::vector<MachineNaturalLoop *>> loopG;
    void BuildLoopForest();

private:
    void CombineSameHeadLoop();
};

class MachineDominatorTree {
public:
    MachineCFG *C;
    std::vector<std::vector<MachineBlock *>> dom_tree{};
    std::vector<MachineBlock *> idom{};


    std::vector<DynamicBitset> atdom;

    void BuildDominatorTree(bool reverse = false);
    void BuildPostDominatorTree();
    bool IsDominate(int id1, int id2) {    // if blockid1 dominate blockid2, return true, else return false
        return atdom[id2].getbit(id1);
    }
};

class MachineNaturalLoop {
public:
    int loop_id;
    std::set<MachineBlock *> loop_nodes;
    std::set<MachineBlock *> exits;
    std::set<MachineBlock *> exitings;
    std::set<MachineBlock *> latches;
    MachineBlock *header;
    MachineBlock *preheader;
    MachineNaturalLoop *fa_loop;
    void FindExitNodes(MachineCFG *C);
};

class MachineNaturalLoopForest {
public:
    int loop_cnt = 0;
    MachineCFG *C;
    std::set<MachineNaturalLoop *> loop_set;
    std::map<MachineBlock *, MachineNaturalLoop *> header_loop_map;
    std::vector<std::vector<MachineNaturalLoop *>> loopG;
    void BuildLoopForest();

private:
    void CombineSameHeadLoop();
};

class MachineDominatorTree {
public:
    MachineCFG *C;
    std::vector<std::vector<MachineBlock *>> dom_tree{};
    std::vector<MachineBlock *> idom{};


    std::vector<DynamicBitset> atdom;

    void BuildDominatorTree(bool reverse = false);
    void BuildPostDominatorTree();
    bool IsDominate(int id1, int id2) {    // if blockid1 dominate blockid2, return true, else return false
        return atdom[id2].getbit(id1);
    }
};

class MachineCFG {
    friend class MachineDominatorTree;

    friend class MachineDominatorTree;

public:
    class MachineCFGNode {
    public:
        MachineBlock *Mblock;
    };

// private:
// private:
    std::map<int, MachineCFGNode *> block_map{};
    std::vector<std::vector<MachineCFGNode *>> G{}, invG{};
    int max_label;

public:
    MachineCFG() : max_label(0){};
    MachineCFGNode *ret_block;
    MachineCFGNode *ret_block;
    void AssignEmptyNode(int id, MachineBlock *Mblk);

    // Just modify CFG edge, no change on branch instructions
    void MakeEdge(int edg_begin, int edg_end);
    void RemoveEdge(int edg_begin, int edg_end);

    MachineCFGNode *GetNodeByBlockId(int id) { return block_map[id]; }
    std::vector<MachineCFGNode *> GetSuccessorsByBlockId(int id) { return G[id]; }
    std::vector<MachineCFGNode *> GetPredecessorsByBlockId(int id) { return invG[id]; }

    MachineDominatorTree DomTree, PostDomTree;
    void BuildDominatoorTree(bool buildPost = true) {
        DomTree.C = this;
        DomTree.BuildDominatorTree();

        PostDomTree.C = this;
        if (buildPost) {
            PostDomTree.BuildPostDominatorTree();
        }
    }
    MachineNaturalLoopForest LoopForest;
    void BuildLoopForest() {
        LoopForest.C = this;
        LoopForest.BuildLoopForest();
    }

    MachineDominatorTree DomTree, PostDomTree;
    void BuildDominatoorTree(bool buildPost = true) {
        DomTree.C = this;
        DomTree.BuildDominatorTree();

        PostDomTree.C = this;
        if (buildPost) {
            PostDomTree.BuildPostDominatorTree();
        }
    }
    MachineNaturalLoopForest LoopForest;
    void BuildLoopForest() {
        LoopForest.C = this;
        LoopForest.BuildLoopForest();
    }

private:
    class Iterator {
    protected:
        MachineCFG *mcfg;

    public:
        Iterator(MachineCFG *mcfg) : mcfg(mcfg) {}
        MachineCFG *getMachineCFG() { return mcfg; }
        virtual void open() = 0;
        virtual MachineCFGNode *next() = 0;
        virtual bool hasNext() = 0;
        virtual void rewind() = 0;
        virtual void close() = 0;
    };
    class SeqScanIterator;
    class ReverseIterator;
    class DFSIterator;
    class BFSIterator;

public:
    DFSIterator *getDFSIterator();
    BFSIterator *getBFSIterator();
    SeqScanIterator *getSeqScanIterator();
    ReverseIterator *getReverseIterator(Iterator *);
};
#include "cfg_iterators/cfg_iterators.h"
#endif