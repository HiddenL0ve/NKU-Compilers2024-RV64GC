#include "dominator_tree.h"
#include "../../include/ir.h"

void DomAnalysis::Execute() {
    for (auto [defI, cfg] : llvmIR->llvm_cfg) {
        DomInfo[cfg].C = cfg;
        DomInfo[cfg].BuildDominatorTree();
    }
}
void dfs(int cur, const std::vector<std::vector<LLVMBlock>> &G, std::vector<int> &result, std::vector<int> &vsd) {
    vsd[cur] = 1;
    for (auto next_block : G[cur]) {
        int i = next_block->block_id;
        if (vsd[i] == 0) {
            dfs(i, G, result, vsd);
        }
    }
    result.push_back(cur);
}
void DominatorTree::BuildDominatorTree(bool reverse) {
    int begin_id = 0;
    auto const *G = &(C->G);
    auto const *invG = &(C->invG);
    // 后支配树
    if (reverse) {
        Assert(C->ret_block != nullptr);
        begin_id = C->ret_block->block_id;
        std::swap(G, invG);    // 直接交换
    }

    int num = C->max_label + 1;
    printf("%d", C->max_label + 1);
    dom_tree.clear();
    dom_tree.resize(num);//一个存储每个节点的支配树的容器，dom_tree[i] 存储直接支配节点 i 。
    idom.clear();
    atdom.clear();
    atdom.resize(num);
    std::vector<int> PostOrder_id;//节点的后续遍历的结果存储
    std::vector<int> vsd;
    for (int i = 0; i <= C->max_label; i++) {
        vsd.push_back(0);
    }
    dfs(begin_id, (*G), PostOrder_id, vsd);
    atdom[begin_id].set(begin_id);    // 相当于 setbit(begin_id, 1)
    for (int i = 0; i <= C->max_label; i++) {
        for (int j = 0; j <= C->max_label; j++) {
            if (i != begin_id) {
                atdom[i].set(j);    // 初始化全部为1
            }
        }
    }
    bool changed = 1;
    while (changed) {//节点 u 的支配集合 = 其前驱节点支配集合的交集，再加上节点自身。
        changed = false;
        for (std::vector<int>::reverse_iterator it = PostOrder_id.rbegin(); it != PostOrder_id.rend(); ++it) {
            auto u = *it;
            BitsetType new_dom_u;

            if (!(*invG)[u].empty()) {
                new_dom_u = atdom[(*((*invG)[u].begin()))->block_id];//首先让其输出化为前驱第一个节点的支配集合
                for (auto v : (*invG)[u]) {
                    new_dom_u &= atdom[v->block_id];//与上所有的前驱节点的支配集合
                }
            }
            new_dom_u.set(u);    // 设置当前节点；再加上节点自身
            if (new_dom_u != atdom[u]) {
                atdom[u] = new_dom_u;
                changed = true;
            }
        }
    }
    idom.resize(num);
    for (auto [u, bb] : *C->block_map) {
        if (u == begin_id) {
            continue;
        }
        for (int v = 0; v <= C->max_label; v++) {//直接支配是一个特殊的概念，指的是在支配集合中，除了当前节点 u 之外，没有其他的节点能够支配v
            if (atdom[u].test(v)) {    // 使用 test() 方法检查位是否被设置
                auto tmp = (atdom[u] & atdom[v]) ^ atdom[u];
                if (tmp.count() == 1 && tmp.test(u)) {
                    idom[u] = (*C->block_map)[v];
                    dom_tree[v].push_back((*C->block_map)[u]);
                }
            }
        }
    }
        df.clear();
        df.resize(num);
        for (int i = 0; i < (*G).size(); i++) {
            if(C->invG[i].size()<2){
                continue;
            }
            for(auto edg_end : (*invG)[i]){
                  int runner=edg_end->block_id;//第一个循环：一个块自己支配自己，所以也满足支配其前驱但是不直接支配他
                  while(idom[i]->block_id!=runner&&runner!=begin_id){
                    df[runner].set(i);
                    runner=idom[runner]->block_id;//支配其前驱但是又不直接支配他
                  }
            }
        }
    
}

std::set<int> DominatorTree::GetDF(std::set<int> S) {
    int num = C->max_label + 1;
    BitsetType result(num);
    for (auto node : S) {
        if (node >= 0 && node <= C->max_label) {
            result |= df[node];
        }
    }

    // 将结果位集转换为std::set<int>
    std::set<int> ret;
    for (int i = 0; i <= C->max_label; ++i) {
        if (result.test(i)) {
            ret.insert(i);
        }
    }

    return ret;
}

std::set<int> DominatorTree::GetDF(int id) {
    std::set<int> ret;
    for (int i = 0; i <= C->max_label; ++i) {
        
        if (df[id].test(i)) {
            ret.insert(i);
        }
    }
    return ret;
}

bool DominatorTree::IsDominate(int id1, int id2) { return atdom[id2].test(id1); }
