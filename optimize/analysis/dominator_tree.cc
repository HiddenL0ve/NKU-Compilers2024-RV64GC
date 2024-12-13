#include "dominator_tree.h"
#include "../../include/ir.h"

void DomAnalysis::Execute() {
    for (auto [defI, cfg] : llvmIR->llvm_cfg) {
        DomInfo[cfg].C=cfg;
        DomInfo[cfg].BuildDominatorTree();
    }
}
void dfs(int cur,const std::vector<std::vector<LLVMBlock>> &G, std::vector<int> &result,std::vector<int> &vsd)
{
      vsd[cur]=1;
      for(auto next_block:G[cur]){
        int i=next_block->block_id;
           if(vsd[i]==0){
            dfs(i,G,result,vsd);
           }
      }
      result.push_back(cur);
}
void DominatorTree::BuildDominatorTree(bool reverse) { 
    int begin_id=0;
    auto const*G=&(C->G);
    auto const *invG=&(C->invG);
    //后支配树
    if(reverse){
        Assert(C->ret_block!=nullptr);
        begin_id=C->ret_block->block_id;
        std::swap(G,invG); //直接交换
    }

    int num=C->max_label+1;
    printf("%d",C->max_label+1);
    dom_tree.clear();
    dom_tree.resize(num);
    idom.clear();
    atdom.clear();
    atdom.resize(num);
    std::vector<int> PostOrder_id;
    std::vector<int> vsd;
     for (int i = 0; i <= C->max_label; i++) {
        vsd.push_back(0);
    }
    dfs(begin_id, (*G), PostOrder_id, vsd);
    atdom[begin_id].set(begin_id); // 相当于 setbit(begin_id, 1)
    for (int i = 0; i <= C->max_label; i++) {
        for (int j = 0; j <= C->max_label; j++) {
            if (i != begin_id) {
                atdom[i].set(j); // 相当于 setbit(j, 1)
            }
        }
    }
    bool changed=1;
    while (changed) {
    changed = false;
    for (std::vector<int>::reverse_iterator it = PostOrder_id.rbegin(); it != PostOrder_id.rend(); ++it) {
        auto u = *it;
        BitsetType new_dom_u;

        if (!(*invG)[u].empty()) {
            new_dom_u = atdom[(*((*invG)[u].begin()))->block_id];
            for (auto v : (*invG)[u]) {
                new_dom_u &= atdom[v->block_id];
            }
        }
        new_dom_u.set(u); // 设置当前节点
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
    for (int v = 0; v <= C->max_label; v++) {
        if (atdom[u].test(v)) { // 使用 test() 方法检查位是否被设置
            BitsetType tmp = (atdom[u] & atdom[v]) ^ atdom[u];
            if (tmp.count() == 1 && tmp.test(u)) {
                idom[u] = (*C->block_map)[v];
                dom_tree[v].push_back((*C->block_map)[u]);
            }
        }
    }
    df.clear();
    df.resize(num);
    for (int i = 0; i < (*G).size(); i++) {
        for (auto edg_end : (*G)[i]) {
            int a = i;
            int b = edg_end->block_id;
            int x = a;
            while (x == b || IsDominate(x, b) == 0) {
                df[x].set(b); // 设置支配前沿关系
                if (idom[x] != NULL) {
                    x = idom[x]->block_id;
                } else {
                    break;
                }
            }
        }
    }
  }
 }

std::set<int> DominatorTree::GetDF(std::set<int> S) { 
    int num=C->max_label+1;
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
        // if (df[id][i]) {
        if (df[id].test(i)) {
            ret.insert(i);
        }
    }
    return ret;
}

bool DominatorTree::IsDominate(int id1, int id2) { 
     return atdom[id2].test(id1);
 }
