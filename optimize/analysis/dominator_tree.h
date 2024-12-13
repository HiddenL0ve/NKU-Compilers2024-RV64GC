#ifndef DOMINATOR_TREE_H
#define DOMINATOR_TREE_H
#include "../../include/ir.h"
#include "../pass.h"
#include <set>
#include <vector>

class DominatorTree {
public:
    CFG *C;
    std::vector<std::vector<LLVMBlock>> dom_tree{};
    std::vector<LLVMBlock> idom{};

    void BuildDominatorTree(bool reverse = false);    // build the dominator tree of CFG* C
    std::set<int> GetDF(std::set<int> S);             // return DF(S)  S = {id1,id2,id3,...}
    std::set<int> GetDF(int id);                      // return DF(id)
    bool IsDominate(int id1, int id2);                // if blockid1 dominate blockid2, return true, else return false
    
    using BitsetType = std::bitset<65536>; // 假设一个足够大的固定大小，或使用动态大小替代方案
    std::vector<BitsetType> atdom{};       // 后支配前沿包含了所有那些在某些路径上不被A后支配的基本块
    std::vector<BitsetType> df{};
    // TODO(): add or modify functions and members if you need
};

class DomAnalysis : public IRPass {
private:
    std::map<CFG *, DominatorTree> DomInfo;

public:
    DomAnalysis(LLVMIR *IR) : IRPass(IR) {}
    void Execute();
    DominatorTree *GetDomTree(CFG *C) { return &DomInfo[C]; }
    // TODO(): add more functions and members if you need
};
#endif