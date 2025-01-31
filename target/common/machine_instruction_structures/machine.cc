#include "machine.h"
#include "assert.h"

MachineDataType INT32(MachineDataType::INT, MachineDataType::B32);
MachineDataType INT64(MachineDataType::INT, MachineDataType::B64);
MachineDataType INT128(MachineDataType::INT, MachineDataType::B128);

MachineDataType FLOAT_32(MachineDataType::FLOAT, MachineDataType::B32);
MachineDataType FLOAT64(MachineDataType::FLOAT, MachineDataType::B64);
MachineDataType FLOAT128(MachineDataType::FLOAT, MachineDataType::B128);

void MachineCFG::AssignEmptyNode(int id, MachineBlock *Mblk) {
    if (id > this->max_label) {
        this->max_label = id;
    }
    MachineCFGNode *node = new MachineCFGNode;
    node->Mblock = Mblk;
    block_map[id] = node;
    while (G.size() < id + 1) {
        G.push_back({});
        // G.resize(id + 1);
    }
    while (invG.size() < id + 1) {
        invG.push_back({});
        // invG.resize(id + 1);
    }
}

// Just modify CFG edge, no change on branch instructions
void MachineCFG::MakeEdge(int edg_begin, int edg_end) {
    Assert(block_map.find(edg_begin) != block_map.end());
    Assert(block_map.find(edg_end) != block_map.end());
    Assert(block_map[edg_begin] != nullptr);
    Assert(block_map[edg_end] != nullptr);
    G[edg_begin].push_back(block_map[edg_end]);
    invG[edg_end].push_back(block_map[edg_begin]);
}

// Just modify CFG edge, no change on branch instructions
void MachineCFG::RemoveEdge(int edg_begin, int edg_end) {
    assert(block_map.find(edg_begin) != block_map.end());
    assert(block_map.find(edg_end) != block_map.end());
    auto it = G[edg_begin].begin();
    for (; it != G[edg_begin].end(); ++it) {
        if ((*it)->Mblock->getLabelId() == edg_end) {
            break;
        }
    }
    G[edg_begin].erase(it);

    auto jt = invG[edg_end].begin();
    for (; jt != invG[edg_end].end(); ++jt) {
        if ((*jt)->Mblock->getLabelId() == edg_begin) {
            break;
        }
    }
    invG[edg_end].erase(jt);
}

Register MachineFunction::GetNewRegister(int regtype, int reglength, bool save_across_call) {
Register MachineFunction::GetNewRegister(int regtype, int reglength, bool save_across_call) {
    static int new_regno = 0;
    printf("%d",new_regno);
    Register new_reg;
    new_reg.is_virtual = true;
    new_reg.reg_no = new_regno++;
    new_reg.type.data_type = regtype;
    new_reg.type.data_length = reglength;
    // InitializeNewVirtualRegister(new_regno);
    return new_reg;
}

Register MachineFunction::GetNewReg(MachineDataType type) { return GetNewRegister(type.data_type, type.data_length); }

MachineBlock *MachineFunction::InitNewBlock() {
    int new_id = ++max_exist_label;
    MachineBlock *new_block = block_factory->CreateBlock(new_id);
    new_block->setParent(this);
    blocks.push_back(new_block);
    mcfg->AssignEmptyNode(new_id, new_block);
    return new_block;
}

MachineBlock *MachineFunction::InsertNewBranchOnlyBlockBetweenEdge(int begin, int end) {
    // TODO("Implement InsertNewBranchOnlyBlockBetweenEdge");
    // New Block
    auto new_block = InitNewBlock();
    auto mid = new_block->getLabelId();
    // Change Edge
    mcfg->RemoveEdge(begin, end);
    mcfg->MakeEdge(begin, mid);
    mcfg->MakeEdge(mid, end);
    // Redirect Branch
    MoveOnePredecessorBranchTargetToNewBlock(begin, end, mid);
    // Insert Branch in new block
    AppendUncondBranchInstructionToNewBlock(mid, end);
    // Redirect Phi
    RedirectPhiNodePredecessor(end, begin, mid);
    return new_block;
}
void MachineFunction::RedirectPhiNodePredecessor(int phi_block, int old_predecessor, int new_predecessor) {
    // in phi_block, find all phi_instructions containing <label old_predecessor> in phi instruction
    // then replace <label old_predecessor> to <label new_predecessor>
    // TODO("Implement RedirectPhiNodePredecessor");
    auto block = mcfg->GetNodeByBlockId(phi_block)->Mblock;
    for (auto ins : *block) {
        if (ins->arch == MachineBaseInstruction::COMMENT)
            continue;
        if (ins->arch != MachineBaseInstruction::PHI)
            break;
        auto mphi = (MachinePhiInstruction *)ins;
        for (auto &phi_pair : mphi->GetPhiList()) {
            if (phi_pair.first == old_predecessor) {
                phi_pair.first = new_predecessor;
            }
        }
    }
}

void dfs_post(int cur, const std::vector<std::vector<MachineCFG::MachineCFGNode *>> &G, std::vector<int> &result,
              std::vector<int> &vsd) {
    vsd[cur] = 1;
    for (auto next_block : G[cur]) {
        if (vsd[next_block->Mblock->getLabelId()] == 0) {
            dfs_post(next_block->Mblock->getLabelId(), G, result, vsd);
        }
    }
    result.push_back(cur);
}

void MachineDominatorTree::BuildDominatorTree(bool reverse) {
    auto const *G = &(C->G);
    auto const *invG = &(C->invG);
    auto begin_id = 0;
    if (reverse) {
        auto temp = G;
        G = invG;
        invG = temp;
        Assert(C->ret_block != nullptr);
        begin_id = C->ret_block->Mblock->getLabelId();
    }

    int block_num = C->max_label + 1;

    std::vector<int> PostOrder_id;
    std::vector<int> vsd;

    dom_tree.clear();
    dom_tree.resize(block_num);
    idom.clear();
    atdom.clear();

    for (int i = 0; i <= C->max_label; i++) {
        vsd.push_back(0);
    }

    dfs_post(begin_id, (*G), PostOrder_id, vsd);

    atdom.resize(block_num);
    for (int i = 0; i < block_num; i++) {
        atdom[i].remake(block_num);
    }
    // dom[u][v] = 1 <==> v dom u <==> v is in set dom(u)
    // atdom[0][0] = 1;
    atdom[begin_id].setbit(begin_id, 1);
    for (int i = 0; i <= C->max_label; i++) {
        for (int j = 0; j <= C->max_label; j++) {
            if (i != begin_id) {
                atdom[i].setbit(j, 1);
                // atdom[i][j] = 1;
            }
        }
    }
    bool changed = 1;
    while (changed) {
        changed = 0;
        for (std::vector<int>::reverse_iterator it = PostOrder_id.rbegin(); it != PostOrder_id.rend(); ++it) {
            auto u = *it;
            DynamicBitset new_dom_u(block_num);

            // Goal: calculate
            // dom(u) |= {u} | {& dom(v)}
            // First:
            // dom(u) = {& dom(v)}, v is qianqu
            if (!(*invG)[u].empty()) {
                new_dom_u = atdom[(*((*invG)[u].begin()))->Mblock->getLabelId()];
                for (auto v : (*invG)[u]) {
                    // new_dom_u &= atdom[v->block_id];
                    new_dom_u = new_dom_u & atdom[v->Mblock->getLabelId()];
                }
            }
            // Second:
            // dom(u) |= {u}
            new_dom_u.setbit(u, 1);
            if (new_dom_u != atdom[u]) {
                atdom[u] = new_dom_u;
                changed = 1;
            }
        }
    }
    idom.clear();
    idom.resize(block_num);
    // Goal calculate all immediate dom(idom)
    for (auto [u, bb] : C->block_map) {
        if (u == begin_id) {
            continue;
        }
        for (int v = 0; v <= C->max_label; v++) {
            // if v idom u, v must dom u
            // if (atdom[u][v]) {
            if (atdom[u].getbit(v)) {
                // dom(u) = {u,???,v,{domv path}}
                // dom(v) = {v,{domv path}}
                // ??? = NULL set if v idom u

                // equals dom(u)-dom(v)
                auto tmp = (atdom[u] & atdom[v]) ^ atdom[u];
                if (tmp.count() == 1 && tmp.getbit(u)) {
                    idom[u] = (C->block_map)[v]->Mblock;
                    dom_tree[v].push_back((C->block_map)[u]->Mblock);
                }
            }
        }
    }
#ifdef CHECK_DOMTREE
    std::cerr << "DOM TREE\n";
    for (int i = 0; i < dom_tree.size(); i++) {
        for (auto jb : dom_tree[i]) {
            std::cerr << i << "->" << jb->getLabelId() << std::endl;
        }
    }
    std::cerr << "\n";
#endif
}

void MachineDominatorTree::BuildPostDominatorTree() { BuildDominatorTree(true); }

static std::set<MachineBlock *> FindNodesInLoop(MachineCFG *C, MachineBlock *n, MachineBlock *d)    // backedge n->d
{
    std::set<MachineBlock *> loop_nodes;

    std::stack<MachineBlock *> S;

    loop_nodes.insert(n);
    loop_nodes.insert(d);

    if (n == d) {
        return loop_nodes;
    }

    S.push(n);
    while (!S.empty()) {
        MachineBlock *x = S.top();
        S.pop();
        for (auto preBB : C->GetPredecessorsByBlockId(x->getLabelId())) {
            if (loop_nodes.find(preBB->Mblock) == loop_nodes.end()) {
                loop_nodes.insert(preBB->Mblock);
                S.push(preBB->Mblock);
            }
        }
    }
    return loop_nodes;
}

static bool JudgeLoopContain(MachineNaturalLoop *l1, MachineNaturalLoop *l2) {
    for (auto l2_n : l2->loop_nodes) {
        if (l1->loop_nodes.find(l2_n) == l1->loop_nodes.end()) {
            return false;
        }
    }
    return true;
}

void MachineNaturalLoopForest::CombineSameHeadLoop() {
    std::set<MachineBlock *> header_set;
    std::set<MachineNaturalLoop *> erase_loop_set;
    for (auto l : loop_set) {
        if (header_set.find(l->header) != header_set.end()) {
            erase_loop_set.insert(l);
            MachineNaturalLoop *oldl = (header_loop_map.find(l->header))->second;
            for (auto l_nodes : l->loop_nodes) {
                oldl->loop_nodes.insert(l_nodes);
            }
            for (auto latch_nodes : l->latches) {
                oldl->latches.insert(latch_nodes);
            }
        } else {
            header_set.insert(l->header);
            header_loop_map.insert({l->header, l});
        }
    }

    for (auto l : erase_loop_set) {
        loop_set.erase(l);
    }
}

void MachineNaturalLoop::FindExitNodes(MachineCFG *C) {
    for (auto node : loop_nodes) {
        for (auto succ_node : C->GetSuccessorsByBlockId(node->getLabelId())) {
            auto succ_bb = succ_node->Mblock;
            if (loop_nodes.find(succ_bb) == loop_nodes.end()) {
                exits.insert(succ_bb);
                exitings.insert(node);
            }
        }
    }
    // for(auto nodes:exiting_nodes){
    //     nodes->comment = nodes->comment + "  exiting" + std::to_string(loop_id);;
    // }
}

void MachineNaturalLoopForest::BuildLoopForest() {
    loop_set.clear();
    loopG.clear();
    header_loop_map.clear();

    loop_cnt = 0;

    auto block_it = C->getSeqScanIterator();
    block_it->open();
    while (block_it->hasNext()) {
        auto block = block_it->next()->Mblock;
        block->loop_depth = 0;
        auto block_id = block->getLabelId();
        for (auto head_bb : C->GetSuccessorsByBlockId(block_id)) {    // bb->head_bb   backedge
            if (C->DomTree.IsDominate(head_bb->Mblock->getLabelId(), block_id)) {
                MachineNaturalLoop *l = new MachineNaturalLoop();
                l->header = head_bb->Mblock;
                l->latches.insert(block);
                l->loop_id = loop_cnt++;
                l->loop_nodes = FindNodesInLoop(this->C, block, head_bb->Mblock);
                loop_set.insert(l);
            }
        }
    }
    loop_cnt = loop_cnt - 1;
    CombineSameHeadLoop();

    for (auto l : loop_set) {
        l->FindExitNodes(this->C);
        // l->header->comment = l->header->comment + "  header" + std::to_string(l->loop_id);
    }

    // build forest
    loopG.resize(loop_cnt + 1);

    std::vector<std::vector<MachineNaturalLoop *>> tmploopG;
    std::vector<std::pair<int, MachineNaturalLoop *>> Indegree;
    tmploopG.resize(loop_cnt + 1);
    Indegree.resize(loop_cnt + 1);
    for (auto l1 : loop_set) {
        Indegree[l1->loop_id].second = l1;
        for (auto l2 : loop_set) {
            if (l1 == l2) {
                continue;
            }
            if (JudgeLoopContain(l1, l2)) {
                tmploopG[l1->loop_id].push_back(l2);
                Indegree[l2->loop_id].first++;
            }
        }
    }

    std::queue<MachineNaturalLoop *> q;

    for (auto L : Indegree) {
        if (L.first == 0 && L.second) {
            q.push(L.second);
        }
    }
    while (!q.empty()) {
        MachineNaturalLoop *x = q.front();
        q.pop();
        for (auto v : tmploopG[x->loop_id]) {
            --Indegree[v->loop_id].first;
            if (Indegree[v->loop_id].first == 0) {
                loopG[x->loop_id].push_back(v);
                v->fa_loop = x;
                q.push(v);
            }
        }
    }

    for (auto l : loop_set) {
        for (auto bb : l->loop_nodes) {
            bb->loop_depth += 1;
        }
    }

#ifdef CHECK_LOOPFOREST
    for (auto l : loop_set) {
        std::cerr << "\n";
        std::cerr << "loop:" << l->loop_id << "------------------------------------\n";
        std::cerr << "loop nodes: ";
        for (auto nodes : l->loop_nodes) {
            std::cerr << nodes->getLabelId() << " ";
        }
        std::cerr << "\n";
        if (l->preheader) {
            std::cerr << "preheader: " << l->preheader->getLabelId() << "\n";
        }
        std::cerr << "header: " << l->header->getLabelId() << "\n";
        std::cerr << "latch: ";
        for (auto nodes : l->latches) {
            std::cerr << nodes->getLabelId() << " ";
        }
        std::cerr << "\n";
        std::cerr << "exitings: ";
        for (auto nodes : l->exitings) {
            std::cerr << nodes->getLabelId() << " ";
        }
        std::cerr << "\n";
        std::cerr << "exits: ";
        for (auto nodes : l->exits) {
            std::cerr << nodes->getLabelId() << " ";
        }
        std::cerr << "\n";
        if (l->fa_loop) {
            std::cerr << "father loop " << l->fa_loop->loop_id << "\n";
        }
    }
#endif
}
