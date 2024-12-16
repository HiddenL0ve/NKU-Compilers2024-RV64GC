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
        std::vector<LLVMBlock> successors = C->GetSuccessor(current_block);
        for (LLVMBlock succ : successors) {
            int succ_id = succ->block_id;
            if (!visited[succ_id]) {
                visited[succ_id] = true;
                //printf("%d ",succ_id);
                stack.push(succ);
            }
        }
        //printf("\n");
    }

    // 遍历所有基本块，删除不可达的基本块和指令
   for (auto it = C->block_map->begin(); it != C->block_map->end(); ) {
        int block_id = it->first;
        LLVMBlock block_to_remove = it->second;

        if (!visited[block_id]) {
            // 清空指令并删除块
            block_to_remove->Instruction_list.clear();
            it = C->block_map->erase(it);

            // 更新邻接表G和invG
            C->G[block_id].clear();
            for (auto& succ_list : C->G) {
                succ_list.erase(std::remove(succ_list.begin(), succ_list.end(), block_to_remove), succ_list.end());
            }
            for (auto& pred_list : C->invG) {
                pred_list.erase(std::remove(pred_list.begin(), pred_list.end(), block_to_remove), pred_list.end());
            }
        } else {
            ++it;
        }
    }
}