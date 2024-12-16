#include "simplify_cfg.h"

void SimplifyCFGPass::Execute() {
    for (auto [defI, cfg] : llvmIR->llvm_cfg) {
        EliminateUnreachedBlocksInsts(cfg);
    }
}

// 删除从函数入口开始到达不了的基本块和指令
// 不需要考虑控制流为常量的情况，你只需要从函数入口基本块(0号基本块)开始dfs，将没有被访问到的基本块和指令删去即可
void SimplifyCFGPass::EliminateUnreachedBlocksInsts(CFG *C) {
    // TODO("EliminateUnreachedBlocksInsts");
    std::vector<bool> visited(C->block_map->size(), false);
    // 使用栈进行DFS遍历
    std::stack<LLVMBlock> stack;
    stack.push(C->block_map->at(0));    // 从入口基本块开始
    visited[0] = true;

    while (!stack.empty()) {
        LLVMBlock current_block = stack.top();
        stack.pop();
        // 获取当前基本块的后继基本块
        std::vector<LLVMBlock> successors = C->GetSuccessor(current_block);
        // 遍历后继基本块
        for (LLVMBlock succ : successors) {
            int succ_id = succ->block_id;
            if (!visited[succ_id]) {
                visited[succ_id] = true;
                stack.push(succ);
            }
        }
    }

    // 遍历所有基本块，删除不可达的基本块和指令
    for (int i = 0; i < C->block_map->size(); ++i) {
        if (!visited[i]) {
            // 删除不可达基本块的指令
            LLVMBlock block_to_remove = C->block_map->at(i);

            // 清空该基本块的指令列表
            block_to_remove->Instruction_list.clear();

            // 删除该基本块
            C->block_map->erase(i);
            // 从控制流图中移除该基本块
            C->G[i].clear();
            for (auto &succ_list : C->G) {
                succ_list.erase(std::remove(succ_list.begin(), succ_list.end(), block_to_remove), succ_list.end());
            }
        }
    }
}