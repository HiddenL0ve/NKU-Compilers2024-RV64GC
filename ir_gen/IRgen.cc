#include "IRgen.h"
#include "../include/ir.h"
#include "semant.h"

extern SemantTable semant_table;    // 也许你会需要一些语义分析的信息

IRgenTable irgen_table;    // 中间代码生成的辅助变量
LLVMIR llvmIR;             // 我们需要在这个变量中生成中间代码
std::map<FuncDefInstruction, int> max_label_map{};
std::map<FuncDefInstruction, int> max_reg_map{};

void AddLibFunctionDeclare();
extern int regnumber;
int max_label = -1;
static FuncDefInstruction now_function;
static int now_label = 0;
static int loop_start_label = -1;
static int loop_end_label = -1;
static Type::ty function_returntype = Type::VOID;
// 在基本块B末尾生成一条新指令
void IRgenArithmeticI32(LLVMBlock B, BasicInstruction::LLVMIROpcode opcode, int reg1, int reg2, int result_reg);
void IRgenArithmeticF32(LLVMBlock B, BasicInstruction::LLVMIROpcode opcode, int reg1, int reg2, int result_reg);
void IRgenArithmeticI32ImmLeft(LLVMBlock B, BasicInstruction::LLVMIROpcode opcode, int val1, int reg2, int result_reg);
void IRgenArithmeticF32ImmLeft(LLVMBlock B, BasicInstruction::LLVMIROpcode opcode, float val1, int reg2,
                               int result_reg);
void IRgenArithmeticI32ImmAll(LLVMBlock B, BasicInstruction::LLVMIROpcode opcode, int val1, int val2, int result_reg);
void IRgenArithmeticF32ImmAll(LLVMBlock B, BasicInstruction::LLVMIROpcode opcode, float val1, float val2,
                              int result_reg);

void IRgenIcmp(LLVMBlock B, BasicInstruction::IcmpCond cmp_op, int reg1, int reg2, int result_reg);
void IRgenFcmp(LLVMBlock B, BasicInstruction::FcmpCond cmp_op, int reg1, int reg2, int result_reg);
void IRgenIcmpImmRight(LLVMBlock B, BasicInstruction::IcmpCond cmp_op, int reg1, int val2, int result_reg);
void IRgenFcmpImmRight(LLVMBlock B, BasicInstruction::FcmpCond cmp_op, int reg1, float val2, int result_reg);

void IRgenFptosi(LLVMBlock B, int src, int dst);
void IRgenSitofp(LLVMBlock B, int src, int dst);
void IRgenZextI1toI32(LLVMBlock B, int src, int dst);

void IRgenGetElementptrIndexI32(LLVMBlock B, BasicInstruction::LLVMType type, int result_reg, Operand ptr,
                                std::vector<int> dims, std::vector<Operand> indexs);

void IRgenGetElementptrIndexI64(LLVMBlock B, BasicInstruction::LLVMType type, int result_reg, Operand ptr,
                                std::vector<int> dims, std::vector<Operand> indexs);

void IRgenLoad(LLVMBlock B, BasicInstruction::LLVMType type, int result_reg, Operand ptr);
void IRgenStore(LLVMBlock B, BasicInstruction::LLVMType type, int value_reg, Operand ptr);
void IRgenStore(LLVMBlock B, BasicInstruction::LLVMType type, Operand value, Operand ptr);

void IRgenCall(LLVMBlock B, BasicInstruction::LLVMType type, int result_reg,
               std::vector<std::pair<enum BasicInstruction::LLVMType, Operand>> args, std::string name);
void IRgenCallVoid(LLVMBlock B, BasicInstruction::LLVMType type,
                   std::vector<std::pair<enum BasicInstruction::LLVMType, Operand>> args, std::string name);

void IRgenCallNoArgs(LLVMBlock B, BasicInstruction::LLVMType type, int result_reg, std::string name);
void IRgenCallVoidNoArgs(LLVMBlock B, BasicInstruction::LLVMType type, std::string name);

void IRgenRetReg(LLVMBlock B, BasicInstruction::LLVMType type, int reg);
void IRgenRetImmInt(LLVMBlock B, BasicInstruction::LLVMType type, int val);
void IRgenRetImmFloat(LLVMBlock B, BasicInstruction::LLVMType type, float val);
void IRgenRetVoid(LLVMBlock B);

void IRgenBRUnCond(LLVMBlock B, int dst_label);
void IRgenBrCond(LLVMBlock B, int cond_reg, int true_label, int false_label);

void IRgenAlloca(LLVMBlock B, BasicInstruction::LLVMType type, int reg);
void IRgenAllocaArray(LLVMBlock B, BasicInstruction::LLVMType type, int reg, std::vector<int> dims);

RegOperand *GetNewRegOperand(int RegNo);

// generate TypeConverse Instructions from type_src to type_dst
// eg. you can use fptosi instruction to converse float to int
// eg. you can use zext instruction to converse bool to int
void IRgenTypeConverse(LLVMBlock B, Type::ty type_src, Type::ty type_dst, int src) {    // 类型转换
    if (type_dst == type_src) {
        return;
    } else if (type_src == Type::INT && type_dst == Type::FLOAT) {
        ++regnumber;
        IRgenSitofp(B, src, regnumber);
    } else if (type_src == Type::FLOAT && type_dst == Type::INT) {
        ++regnumber;
        IRgenFptosi(B, src, regnumber);
    } else if (type_src == Type::BOOL && type_dst == Type::INT) {
        ++regnumber;
        IRgenZextI1toI32(B, src, regnumber);

    } else if (type_src == Type::INT && type_dst == Type::BOOL) {
        ++regnumber;
        IRgenIcmpImmRight(B, BasicInstruction::IcmpCond::ne, src, 0, regnumber);    // reg!=0;
    } else if (type_src == Type::FLOAT && type_dst == Type::BOOL) {
        ++regnumber;
        IRgenFcmpImmRight(B, BasicInstruction::FcmpCond::ONE, src, 0, regnumber);

    } else if (type_src == Type::BOOL && type_dst == Type::FLOAT) {
        ++regnumber;
        IRgenZextI1toI32(B, src, regnumber);
        src = regnumber;
        ++regnumber;
        IRgenSitofp(B, src, regnumber);
    }
    // TODO("IRgenTypeConverse. Implement it if you need it");
}

enum op {
    ADD = 0,     // +
    SUB = 1,     // -
    MUL = 2,     // *
    DIV = 3,     // /
    MOD = 4,     // %
    GEQ = 5,     // >=
    GT = 6,      // >
    LEQ = 7,     // <=
    LT = 8,      // <
    EQ = 9,      // ==
    NE = 10,     // !=
    OR = 11,     // ||
    AND = 12,    // &&
    NOT = 13,    // !
};
void BinaryIrInt(LLVMBlock B, op opcode, int reg1, int reg2) {
    if (opcode == op::ADD) {
        IRgenArithmeticI32(B, BasicInstruction::LLVMIROpcode::ADD, reg1, reg2, ++regnumber);
    } else if (opcode == op::SUB) {
        IRgenArithmeticI32(B, BasicInstruction::LLVMIROpcode::SUB, reg1, reg2, ++regnumber);
    } else if (opcode == op::MUL) {
        IRgenArithmeticI32(B, BasicInstruction::LLVMIROpcode::MUL, reg1, reg2, ++regnumber);
    } else if (opcode == op::DIV) {
        IRgenArithmeticI32(B, BasicInstruction::LLVMIROpcode::DIV, reg1, reg2, ++regnumber);
    } else if (opcode == op::MOD) {
        IRgenArithmeticI32(B, BasicInstruction::LLVMIROpcode::MOD, reg1, reg2, ++regnumber);
    } else if (opcode == op::GEQ) {
        IRgenIcmp(B, BasicInstruction::IcmpCond::sge, reg1, reg2, ++regnumber);
    } else if (opcode == op::GT) {
        IRgenIcmp(B, BasicInstruction::IcmpCond::sgt, reg1, reg2, ++regnumber);
    } else if (opcode == op::LEQ) {
        IRgenIcmp(B, BasicInstruction::IcmpCond::sle, reg1, reg2, ++regnumber);
    } else if (opcode == op::LT) {
        IRgenIcmp(B, BasicInstruction::IcmpCond::slt, reg1, reg2, ++regnumber);
    } else if (opcode == op::EQ) {
        IRgenIcmp(B, BasicInstruction::IcmpCond::eq, reg1, reg2, ++regnumber);
    } else if (opcode == op::NE) {
        IRgenIcmp(B, BasicInstruction::IcmpCond::ne, reg1, reg2, ++regnumber);
    } else {
        assert(false);
    }
}

void BinaryIrFloat(LLVMBlock B, op opcode, int reg1, int reg2) {
    if (opcode == op::ADD) {
        IRgenArithmeticF32(B, BasicInstruction::LLVMIROpcode::FADD, reg1, reg2, ++regnumber);
    } else if (opcode == op::SUB) {
        IRgenArithmeticF32(B, BasicInstruction::LLVMIROpcode::FSUB, reg1, reg2, ++regnumber);
    } else if (opcode == op::MUL) {
        IRgenArithmeticF32(B, BasicInstruction::LLVMIROpcode::FMUL, reg1, reg2, ++regnumber);
    } else if (opcode == op::DIV) {
        IRgenArithmeticF32(B, BasicInstruction::LLVMIROpcode::FDIV, reg1, reg2, ++regnumber);
    } else if (opcode == op::GEQ) {
        IRgenFcmp(B, BasicInstruction::FcmpCond::OGE, reg1, reg2, ++regnumber);
    } else if (opcode == op::GT) {
        IRgenFcmp(B, BasicInstruction::FcmpCond::OGT, reg1, reg2, ++regnumber);
    } else if (opcode == op::LEQ) {
        IRgenFcmp(B, BasicInstruction::FcmpCond::OLE, reg1, reg2, ++regnumber);
    } else if (opcode == op::LT) {
        IRgenFcmp(B, BasicInstruction::FcmpCond::OLT, reg1, reg2, ++regnumber);
    } else if (opcode == op::EQ) {
        IRgenFcmp(B, BasicInstruction::FcmpCond::OEQ, reg1, reg2, ++regnumber);
    } else if (opcode == op::NE) {
        IRgenFcmp(B, BasicInstruction::FcmpCond::ONE, reg1, reg2, ++regnumber);
    } else {
        assert(false);
    }
}
void Binary(tree_node *a, tree_node *b, op opcode, LLVMBlock B) {
    int left = a->attribute.T.type;
    int right = b->attribute.T.type;
    if (left == Type::INT && right == Type::INT) {
        // printf("%d",regnumber);
        a->codeIR();
        int reg1 = regnumber;
        // printf("%d",reg1);
        b->codeIR();
        int reg2 = regnumber;
        // printf("%d",reg2);
        BinaryIrInt(B, opcode, reg1, reg2);
    } else if (left == Type::FLOAT && right == Type::INT) {
        a->codeIR();
        int reg1 = regnumber;
        b->codeIR();
        int reg2 = regnumber;
        IRgenSitofp(B, reg2, ++regnumber);    // b int->float
        reg2 = regnumber;
        BinaryIrFloat(B, opcode, reg1, reg2);
    } else if (left == Type::FLOAT && right == Type::FLOAT) {
        a->codeIR();
        int reg1 = regnumber;
        b->codeIR();
        int reg2 = regnumber;
        BinaryIrFloat(B, opcode, reg1, reg2);
    } else if (left == Type::INT && right == Type::FLOAT) {
        a->codeIR();
        int reg1 = regnumber;
        b->codeIR();
        int reg2 = regnumber;
        IRgenSitofp(B, reg1, ++regnumber);    // a int->float
        reg1 = regnumber;
        BinaryIrFloat(B, opcode, reg1, reg2);
    } else if (left == Type::BOOL && right == Type::BOOL) {
        a->codeIR();
        int reg1 = regnumber;
        b->codeIR();
        int reg2 = regnumber;
        IRgenZextI1toI32(B, reg1, ++regnumber);
        reg1 = regnumber;
        IRgenZextI1toI32(B, reg2, ++regnumber);
        reg2 = regnumber;
        BinaryIrInt(B, opcode, reg1, reg2);
    } else if (left == Type::BOOL && right == Type::INT) {
        a->codeIR();
        int reg1 = regnumber;
        b->codeIR();
        int reg2 = regnumber;
        IRgenZextI1toI32(B, reg1, ++regnumber);
        reg1 = regnumber;
        BinaryIrInt(B, opcode, reg1, reg2);
    } else if (left == Type::INT && right == Type::BOOL) {
        a->codeIR();
        int reg1 = regnumber;
        b->codeIR();
        int reg2 = regnumber;
        IRgenZextI1toI32(B, reg2, ++regnumber);
        reg2 = regnumber;
        BinaryIrInt(B, opcode, reg1, reg2);
    } else if (left == Type::BOOL && right == Type::FLOAT) {
        a->codeIR();
        int reg1 = regnumber;
        b->codeIR();
        int reg2 = regnumber;
        IRgenZextI1toI32(B, reg1, ++regnumber);
        reg1 = regnumber;
        IRgenSitofp(B, reg1, ++regnumber);
        reg1 = regnumber;
        BinaryIrFloat(B, opcode, reg1, reg2);
    } else if (left == Type::FLOAT && right == Type::BOOL) {
        a->codeIR();
        int reg1 = regnumber;
        b->codeIR();
        int reg2 = regnumber;
        IRgenZextI1toI32(B, reg2, ++regnumber);
        reg2 = regnumber;
        IRgenSitofp(B, reg2, ++regnumber);
        reg2 = regnumber;
        BinaryIrFloat(B, opcode, reg1, reg2);
    } else if (left == Type::PTR && right == Type::BOOL) {
        assert(false);
    } else if (left == Type::PTR && right == Type::PTR) {
        assert(false);
    } else if (left == Type::BOOL && right == Type::PTR) {
        assert(false);
    } else if (left == Type::INT && right == Type::PTR) {
        assert(false);
    } else if (left == Type::PTR && right == Type::INT) {
        assert(false);
    } else if (left == Type::FLOAT && right == Type::PTR) {
        assert(false);
    } else if (left == Type::PTR && right == Type::FLOAT) {
        assert(false);
    } else if (left == Type::VOID && right == Type::PTR) {
        assert(false);
    } else if (left == Type::VOID && right == Type::VOID) {
        assert(false);
    } else if (left == Type::PTR && right == Type::VOID) {
        assert(false);
    } else if (left == Type::INT && right == Type::VOID) {
        assert(false);
    } else if (left == Type::FLOAT && right == Type::VOID) {
        assert(false);
    } else if (left == Type::VOID && right == Type::FLOAT) {
        assert(false);
    } else if (left == Type::VOID && right == Type::INT) {
        assert(false);
    } else if (left == Type::VOID && right == Type::BOOL) {
        assert(false);
    } else if (left == Type::BOOL && right == Type::VOID) {
        assert(false);
    }
}
void SingleIrInt(LLVMBlock B, op opcode, int reg) {
    if (opcode == op::ADD) {

    } else if (opcode == op::SUB) {
        IRgenArithmeticI32ImmLeft(B, BasicInstruction::LLVMIROpcode::SUB, 0, reg, ++regnumber);    // 0-reg 立即数在左边
    } else if (opcode == op::NOT) {
        IRgenIcmpImmRight(B, BasicInstruction::IcmpCond::eq, reg, 0, ++regnumber);    // reg==0;  !1=0;!0=1;
    } else {
        assert(false);
    }
}
void SingleIrFloat(LLVMBlock B, op opcode, int reg) {
    if (opcode == op::ADD) {

    } else if (opcode == op::SUB) {
        IRgenArithmeticF32ImmLeft(B, BasicInstruction::LLVMIROpcode::SUB, 0, reg, ++regnumber);
    } else if (opcode == op::NOT) {
        IRgenFcmpImmRight(B, BasicInstruction::FcmpCond::OEQ, reg, 0, ++regnumber);
    } else {
        assert(false);
    }
}
void Single(tree_node *a, op opcode, LLVMBlock B) {
    int b = a->attribute.T.type;
    if (b == Type::FLOAT) {
        a->codeIR();
        int reg = regnumber;
        SingleIrFloat(B, opcode, reg);
    } else if (b == Type::INT) {
        a->codeIR();
        int reg = regnumber;
        SingleIrInt(B, opcode, reg);
    } else if (b == Type::BOOL) {
        a->codeIR();
        int reg = regnumber;
        IRgenZextI1toI32(B, reg, ++regnumber);
        reg = regnumber;
        SingleIrInt(B, opcode, reg);
    } else {
        assert(false);
    }
}

void AddNoReturnBlock() {
    for (auto block : llvmIR.function_block_map[now_function]) {
        LLVMBlock B = block.second;
        auto opcode = B->Instruction_list.back()->GetOpcode();
        if (B->Instruction_list.empty() || (!(opcode == BasicInstruction::LLVMIROpcode::RET)) &&
                                           (!(opcode == BasicInstruction::LLVMIROpcode::BR_COND)) &&
                                           (!(opcode == BasicInstruction::LLVMIROpcode::BR_UNCOND))) {
            if (function_returntype == Type::VOID) {
                IRgenRetVoid(B);
            } else if (function_returntype == Type::INT) {
                IRgenRetImmInt(B, BasicInstruction::LLVMType::I32, 0);
            } else if (function_returntype == Type::FLOAT) {
                IRgenRetImmFloat(B, BasicInstruction::LLVMType::FLOAT32, 0);
            }
        }
    }
}

BasicInstruction::LLVMType TLLvm[7] = {BasicInstruction::LLVMType::VOID,    BasicInstruction::LLVMType::I32,
                                       BasicInstruction::LLVMType::FLOAT32, BasicInstruction::LLVMType::I1,
                                       BasicInstruction::LLVMType::PTR,     BasicInstruction::LLVMType::DOUBLE,
                                       BasicInstruction::LLVMType::I64};

void BasicBlock::InsertInstruction(int pos, Instruction Ins) {    // 将操作插在前面或者后面 0前面 1后面
    assert(pos == 0 || pos == 1);
    if (pos == 0) {
        Instruction_list.push_front(Ins);
    } else if (pos == 1) {
        Instruction_list.push_back(Ins);
    }
}

/*
二元运算指令生成的伪代码：
    假设现在的语法树节点是：AddExp_plus
    该语法树表示 addexp + mulexp

    addexp->codeIR()
    mulexp->codeIR()
    假设mulexp生成完后，我们应该在基本块B0继续插入指令。
    addexp的结果存储在r0寄存器中，mulexp的结果存储在r1寄存器中
    生成一条指令r2 = r0 + r1，并将该指令插入基本块B0末尾。
    标注后续应该在基0插本块B入指令，当前节点的结果寄存器为r2。
    (如果考虑支持浮点数，需要查看语法树节点的类型来判断此时是否需要隐式类型转换)
*/

/*
while语句指令生成的伪代码：
    while的语法树节点为while(cond)stmt

    假设当前我们应该在B0基本块开始插入指令
    新建三个基本块Bcond，Body，Bend
    在B0基本块末尾插入一条无条件跳转指令，跳转到Bcond

    设置当前我们应该在Bcond开始插入指令
    cond->codeIR()    //在调用该函数前你可能需要设置真假值出口
    假设cond生成完后，我们应该在B1基本块继续插入指令，Bcond的结果为r0
    如果r0的类型不为bool，在B1末尾生成一条比较语句，比较r0是否为真。
    在B1末尾生成一条条件跳转语句，如果为真，跳转到Body，如果为假，跳转到Bend

    设置当前我们应该在Body开始插入指令
    stmt->codeIR()
    假设当stmt生成完后，我们应该在B2基本块继续插入指令
    在B2末尾生成一条无条件跳转语句，跳转到Bcond

    设置当前我们应该在Bend开始插入指令
*/

void __Program::codeIR() {
    AddLibFunctionDeclare();
    auto comp_vector = *comp_list;
    for (auto comp : comp_vector) {
        comp->codeIR();
    }
}

void Exp::codeIR() { addexp->codeIR(); }

void AddExp_plus::codeIR() {
    LLVMBlock B = llvmIR.GetBlock(now_function, now_label);
    Binary(addexp, mulexp, op::ADD, B);
}

void AddExp_sub::codeIR() {
    LLVMBlock B = llvmIR.GetBlock(now_function, now_label);
    Binary(addexp, mulexp, op::SUB, B);
}

void MulExp_mul::codeIR() {
    LLVMBlock B = llvmIR.GetBlock(now_function, now_label);
    Binary(mulexp, unary_exp, op::MUL, B);
}

void MulExp_div::codeIR() {
    LLVMBlock B = llvmIR.GetBlock(now_function, now_label);
    Binary(mulexp, unary_exp, op::DIV, B);
}

void MulExp_mod::codeIR() {
    LLVMBlock B = llvmIR.GetBlock(now_function, now_label);
    Binary(mulexp, unary_exp, op::MOD, B);
}

void RelExp_leq::codeIR() {
    LLVMBlock B = llvmIR.GetBlock(now_function, now_label);
    Binary(relexp, addexp, op::LEQ, B);
}

void RelExp_lt::codeIR() {
    LLVMBlock B = llvmIR.GetBlock(now_function, now_label);
    Binary(relexp, addexp, op::LT, B);
}

void RelExp_geq::codeIR() {
    LLVMBlock B = llvmIR.GetBlock(now_function, now_label);
    Binary(relexp, addexp, op::GEQ, B);
}

void RelExp_gt::codeIR() {
    LLVMBlock B = llvmIR.GetBlock(now_function, now_label);
    Binary(relexp, addexp, op::GT, B);
}

void EqExp_eq::codeIR() {
    LLVMBlock B = llvmIR.GetBlock(now_function, now_label);
    Binary(eqexp, relexp, op::EQ, B);
}

void EqExp_neq::codeIR() {
    LLVMBlock B = llvmIR.GetBlock(now_function, now_label);
    Binary(eqexp, relexp, op::NE, B);
}

/*
（a&&b）
define i1 @logical_and_with_branches(i1 %a, i1 %b) {
entry:
  br i1 %a, label %check_b, label %return_false

check_b:
  br i1 %b, label %return_true, label %return_false

return_true:
  ret i1 true

return_false:
  ret i1 false
}
*/

// short circuit &&
void LAndExp_and::codeIR() {
    assert((true_label != -1) && (false_label != -1));    // 确保已经在之前的处理中存在相应的label，即确保在条件语句中
    int lefttrue_label = llvmIR.NewBlock(now_function, ++max_label)->block_id;

    landexp->true_label = lefttrue_label;
    landexp->false_label = false_label;
    landexp->codeIR();

    LLVMBlock B = llvmIR.GetBlock(now_function, now_label);
    if (landexp->attribute.T.type != Type::BOOL) {
        IRgenTypeConverse(B, landexp->attribute.T.type, Type::BOOL, regnumber);
    }
    IRgenBrCond(B, regnumber, lefttrue_label, this->false_label);

    now_label = lefttrue_label;
    eqexp->true_label = this->true_label;
    eqexp->false_label = this->false_label;
    eqexp->codeIR();
    LLVMBlock B1 = llvmIR.GetBlock(now_function, now_label);
    if (eqexp->attribute.T.type != Type::BOOL) {
        IRgenTypeConverse(B1, eqexp->attribute.T.type, Type::BOOL, regnumber);
    }
}

// short circuit ||
void LOrExp_or::codeIR() {
    assert((true_label != -1) && (false_label != -1));
    int rfalse_lable = llvmIR.NewBlock(now_function, ++max_label)->block_id;

    lorexp->false_label = rfalse_lable;
    lorexp->true_label = this->true_label;
    lorexp->codeIR();
    LLVMBlock B = llvmIR.GetBlock(now_function, now_label);
    if (lorexp->attribute.T.type != Type::BOOL) {
        IRgenTypeConverse(B, lorexp->attribute.T.type, Type::BOOL, regnumber);
    }
    IRgenBrCond(B, regnumber, true_label, rfalse_lable);

    now_label = rfalse_lable;
    landexp->false_label = this->false_label;
    landexp->true_label = this->true_label;
    landexp->codeIR();
    LLVMBlock B1 = llvmIR.GetBlock(now_function, now_label);

    if (landexp->attribute.T.type != Type::BOOL) {
        IRgenTypeConverse(B, landexp->attribute.T.type, Type::BOOL, regnumber);
    }
}

void ConstExp::codeIR() { addexp->codeIR(); }

// stmbol_table中存储了变量名以及其对应的alloca结果指针寄存器
void Lval::codeIR() {
    LLVMBlock b = llvmIR.GetBlock(now_function, now_label);
    Operand ptr_operand;
    VarAttribute lval_attribute;
    bool formal_array_tag = false;
    int alloca_reg = irgen_table.symbol_table.lookup(name);
    if (alloca_reg != -1) {    // 局部变量
        ptr_operand = GetNewRegOperand( alloca_reg);    // 对于a=5这个例子，该条语句在构建%a这个操作数，或者存在的话直接返回;通过指针寄存器的值分配对应的操作数
        lval_attribute = irgen_table.RegTable[alloca_reg];
        formal_array_tag = irgen_table.FormalArrayTable[alloca_reg];    // 用于判断是否为函数参数
    } else {    // 返回-1证明不在symbol_table中，为全局变量
        lval_attribute = semant_table.GlobalTable[name];
        ptr_operand = GetNewGlobalOperand(name->get_string());
    }
    /*
          int a[3][3];
          int value = a[1][2];

          %a = alloca [3 x [3 x i32]]   ; 分配一个 3x3 的二维数组，类型为 [3 x [3 x i32]]
          %ptr = getelementptr  [3 x [3 x i32]], [3 x [3 x i32]]* %a, i32 1, i32 2  ; 获取 a[1][2] 的地址
          %val = load i32, i32* %ptr     ; 加载 a[1][2] 的值到 %val 中
    */

    // 下述代码相当于对a[1][2]的处理
    auto lltype = TLLvm[lval_attribute.type];    // 左值类型
    std::vector<Operand>indexs;    // 使用时用到的数组维度 例如：定义了 a[10][20]但此时使用为a[3][5]此时将会把3、5转换为对应的索引
    if (dims != nullptr) {
        for (auto d : *dims) {
            d->codeIR();
            if (d->attribute.T.type != Type::INT) {
                IRgenTypeConverse(b, d->attribute.T.type, Type::INT, regnumber);
            }
            indexs.push_back(GetNewRegOperand(regnumber));    // 真实使用的时候用到的数组的偏移
        }
    }
    if (indexs.empty() == false || attribute.T.type == Type::PTR) {    // 对于数组
        if (!formal_array_tag) {
            indexs.insert(indexs.begin(), new ImmI32Operand(0));
        }
        if (lltype == BasicInstruction::LLVMType::I32)
            IRgenGetElementptrIndexI32(b, lltype, ++regnumber, ptr_operand, lval_attribute.dims, indexs);
        else if (lltype == BasicInstruction::LLVMType::FLOAT32) {
            IRgenGetElementptrIndexI32(b, lltype, ++regnumber, ptr_operand, lval_attribute.dims, indexs);
        }
        ptr_operand = GetNewRegOperand(regnumber);
    }
    ptr = ptr_operand;
    if (is_left == false) {    // 右值
        if (attribute.T.type != Type::PTR) {
            IRgenLoad(b, lltype, ++regnumber, ptr_operand);
        }
    }
}

void FuncRParams::codeIR() {}

void Func_call::codeIR() {
    LLVMBlock B = llvmIR.GetBlock(now_function, now_label);

    Type::ty funcTypeRet = semant_table.FunctionTable[name]->return_type;
    BasicInstruction::LLVMType llvm_type = TLLvm[funcTypeRet];
    std::vector<std::pair<BasicInstruction::LLVMType, Operand>> args_vec;

    // 无参数的函数调用
    if (funcr_params == nullptr) {
        if (funcTypeRet == Type::VOID) {
            IRgenCallVoidNoArgs(B, llvm_type, name->get_string());
        } else {
            ++regnumber;
            IRgenCallNoArgs(B, llvm_type, regnumber, name->get_string());
        }
    } else {                                                         // 处理有函数参数的调用
        auto params = ((FuncRParams *)funcr_params)->params;         // 该函数实参
        auto fparams = semant_table.FunctionTable[name]->formals;    // 形参
        for (int i = 0; i < (*params).size(); i++) {
            auto cur_param = (*params)[i];
            auto cur_fparam = (*fparams)[i];
            cur_param->codeIR();
            if (cur_param->attribute.T.type != cur_fparam->attribute.T.type) {
                IRgenTypeConverse(B, cur_param->attribute.T.type, cur_fparam->attribute.T.type,
                                  regnumber);    // 形参类型不匹配时做类型转换
            }
            args_vec.push_back({TLLvm[cur_fparam->attribute.T.type], GetNewRegOperand(regnumber)});
        }
        if (funcTypeRet == Type::VOID) {
            IRgenCallVoid(B, llvm_type, args_vec, name->get_string());
        } else {
            ++regnumber;
            IRgenCall(B, llvm_type, regnumber, args_vec, name->get_string());
        }
    }
}

void UnaryExp_plus::codeIR() {
    LLVMBlock B = llvmIR.GetBlock(now_function, now_label);
    Single(unary_exp, op::ADD, B);
}

void UnaryExp_neg::codeIR() {
    LLVMBlock B = llvmIR.GetBlock(now_function, now_label);
    Single(unary_exp, op::SUB, B);
}

void UnaryExp_not::codeIR() {
    LLVMBlock B = llvmIR.GetBlock(now_function, now_label);
    Single(unary_exp, op::NOT, B);
}

void IntConst::codeIR() {
    LLVMBlock B = llvmIR.GetBlock(now_function, now_label);
    ++regnumber;
    IRgenArithmeticI32ImmAll(B, BasicInstruction::LLVMIROpcode::ADD, val, 0, regnumber);
}

void FloatConst::codeIR() {
    LLVMBlock B = llvmIR.GetBlock(now_function, now_label);
    ++regnumber;
    IRgenArithmeticF32ImmAll(B, BasicInstruction::LLVMIROpcode::FADD, val, 0, regnumber);
}

void StringConst::codeIR() {}

void PrimaryExp_branch::codeIR() { exp->codeIR(); }

void assign_stmt::codeIR() {
    LLVMBlock B = llvmIR.GetBlock(now_function, now_label);
    lval->codeIR();
    exp->codeIR();
    IRgenTypeConverse(B, exp->attribute.T.type, lval->attribute.T.type, regnumber);
    IRgenStore(B, TLLvm[lval->attribute.T.type], GetNewRegOperand(regnumber), ((Lval *)lval)->ptr);
}

void expr_stmt::codeIR() { exp->codeIR(); }

void block_stmt::codeIR() {
    irgen_table.symbol_table.enter_scope();
    b->codeIR();
    irgen_table.symbol_table.exit_scope();
}

/*
entry:if(a>0)
  %a = alloca i32
  store i32 10, i32* %a
  %0 = load i32, i32* %a
  %cmp = icmp sgt i32 %0, 0

  br i1 %cmp, label %if.then, label %if.else

if.then:
  call void @doSomething()
  br label %if.end

if.else:
  call void @doSomethingElse()
  br label %if.end

if.end:
  ret i32 0
*/
void ifelse_stmt::codeIR() {
    int if_label = llvmIR.NewBlock(now_function, ++max_label)->block_id;
    int else_label = llvmIR.NewBlock(now_function, ++max_label)->block_id;
    int end_label = llvmIR.NewBlock(now_function, ++max_label)->block_id;

    Cond->true_label = if_label;
    Cond->false_label = else_label;
    Cond->codeIR();
    LLVMBlock B1 = llvmIR.GetBlock(now_function, now_label);
    IRgenTypeConverse(B1, Cond->attribute.T.type, Type::BOOL, regnumber);
    IRgenBrCond(B1, regnumber, if_label, else_label);

    now_label = if_label;
    ifstmt->codeIR();
    LLVMBlock B2 = llvmIR.GetBlock(now_function, now_label);
    IRgenBRUnCond(B2, end_label);

    now_label = else_label;
    elsestmt->codeIR();
    LLVMBlock B3 = llvmIR.GetBlock(now_function, now_label);
    IRgenBRUnCond(B3, end_label);

    now_label = end_label;
}
/*
对于 if（a>0）:
entry:
  %a = alloca i32
  store i32 10, i32* %a
  %0 = load i32, i32* %a
  %cmp = icmp sgt i32 %0, 0  //cond->codeIR()
  br i1 %cmp, label %if.then, label %if.end ---  IRgenBrCond(trueB,regnumber,if_label,end_label);
if.then:
  call void @doSomething() // ifstmt->codeIR();
  br label %if.end   //  IRgenBRUnCond(B1,end_label);

if.end:
  ret i32 0

*/
void if_stmt::codeIR() {
    max_label++;
    int if_label = llvmIR.NewBlock(now_function, max_label)->block_id;
    max_label++;
    int end_label = llvmIR.NewBlock(now_function, max_label)->block_id;

    Cond->true_label = if_label;      // 对应上述代码的if.then
    Cond->false_label = end_label;    // 对应上述代码的 if.end
    Cond->codeIR();

    LLVMBlock B = llvmIR.GetBlock(now_function, now_label);
    IRgenTypeConverse(B, Cond->attribute.T.type, Type::BOOL, regnumber);
    IRgenBrCond(B, regnumber, if_label, end_label);

    now_label = if_label;
    ifstmt->codeIR();
    LLVMBlock B1 = llvmIR.GetBlock(now_function, now_label);
    IRgenBRUnCond(B1, end_label);

    now_label = end_label;
}
/*
#include <stdio.h>

int main()
{
   int a = 0, b = 1;
   while(a < 5)
   {
       a++;
       b *= a;
   }
   return b;
}

define i32 @main() #0 {
entry:
  %retval = alloca i32, align 4
  %a = alloca i32, align 4
  %b = alloca i32, align 4
  store i32 0, i32* %retval, align 4
  store i32 0, i32* %a, align 4
  store i32 1, i32* %b, align 4
  br label %while.cond

while.cond:                                       ; preds = %while.body, %entry
  %0 = load i32, i32* %a, align 4
  %cmp = icmp slt i32 %0, 5
  br i1 %cmp, label %while.body, label %while.end

while.body:                                       ; preds = %while.cond
  %1 = load i32, i32* %a, align 4
  %inc = add nsw i32 %1, 1
  store i32 %inc, i32* %a, align 4
  %2 = load i32, i32* %a, align 4
  %3 = load i32, i32* %b, align 4
  %mul = mul nsw i32 %3, %2
  store i32 %mul, i32* %b, align 4
  br label %while.cond

while.end:                                        ; preds = %while.cond
  %4 = load i32, i32* %b, align 4
  ret i32 %4
}

*/
void while_stmt::codeIR() {
    int while_cond = llvmIR.NewBlock(now_function, ++max_label)->block_id;
    int while_body = llvmIR.NewBlock(now_function, ++max_label)->block_id;
    int while_end = llvmIR.NewBlock(now_function, ++max_label)->block_id;

    int t1 = loop_start_label;
    int t2 = loop_end_label;
    loop_start_label = while_cond;
    loop_end_label = while_end;

    LLVMBlock b = llvmIR.GetBlock(now_function, now_label);
    IRgenBRUnCond(b, while_cond);

    now_label = while_cond;
    Cond->true_label = while_body;
    Cond->false_label = while_end;
    Cond->codeIR();
    LLVMBlock B1 = llvmIR.GetBlock(now_function, now_label);
    IRgenTypeConverse(B1, Cond->attribute.T.type, Type::BOOL, regnumber);
    IRgenBrCond(B1, regnumber, while_body, while_end);

    now_label = while_body;
    body->codeIR();
    LLVMBlock B2 = llvmIR.GetBlock(now_function, now_label);
    IRgenBRUnCond(B2, while_cond);

    now_label = while_end;

    loop_start_label = t1;    // 存在循环嵌套的情况
    loop_end_label = t2;
}

void continue_stmt::codeIR() {
    LLVMBlock B = llvmIR.function_block_map[now_function][now_label];
    IRgenBRUnCond(B, loop_start_label);
}

void break_stmt::codeIR() {
    LLVMBlock B = llvmIR.function_block_map[now_function][now_label];
    IRgenBRUnCond(B, loop_end_label);
}

void return_stmt::codeIR() {
    return_exp->codeIR();
    LLVMBlock B = llvmIR.GetBlock(now_function, now_label);
    if (return_exp->attribute.T.type != function_returntype) {
        IRgenTypeConverse(B, return_exp->attribute.T.type, function_returntype, regnumber);
    }
    IRgenRetReg(B, TLLvm[function_returntype], regnumber);
}

void return_stmt_void::codeIR() {
    LLVMBlock B = llvmIR.GetBlock(now_function, now_label);
    IRgenRetVoid(B);
}

void ConstInitVal::codeIR() {}

void ConstInitVal_exp::codeIR() { exp->codeIR(); }

void VarInitVal::codeIR() {}

void VarInitVal_exp::codeIR() { exp->codeIR(); }

void VarDef_no_init::codeIR() {}

void VarDef::codeIR() {}

void ConstDef::codeIR() {}

std::vector<int> GetIndexes(std::vector<int> dims, int Index) {//通过行主次序的排列计算应该在那个索引位置
    std::vector<int> ret;
    ret.reserve(dims.size()); 
    for (auto it = dims.rbegin(); it != dims.rend(); ++it) { 
        int dimSize = *it;
        ret.push_back(Index % dimSize); 
        Index /= dimSize; 
    }
    std::reverse(ret.begin(), ret.end());

    return ret;
}

// Reference: https://github.com/yuhuifishash/NKU-Compilers2024-RV64GC.git/ir_gen/IRgen.cc line 561-583
void RecursiveArrayInitIR(LLVMBlock block, const std::vector<int> dims, int arrayaddr_reg_no, InitVal init,
                          int beginPos, int endPos, int dimsIdx, Type::ty ArrayType) {
    int pos = beginPos;
    for (InitVal iv : *(init->GetList())) {
        if (iv->IsExp()) {
            iv->codeIR();
            int init_val_reg = regnumber;
            if (iv->attribute.T.type != ArrayType) {
                IRgenTypeConverse(block, iv->attribute.T.type, ArrayType, init_val_reg);
            }
            init_val_reg = regnumber;
            int addr_reg = ++regnumber;
            auto gep =new GetElementptrInstruction(TLLvm[ArrayType], GetNewRegOperand(addr_reg),
                                         GetNewRegOperand(arrayaddr_reg_no), dims, BasicInstruction::LLVMType::I32);
            gep->push_idx_imm32(0);
            std::vector<int> indexes = GetIndexes(dims, pos);
            for (int idx : indexes) {
                gep->push_idx_imm32(idx);
            }
            block->InsertInstruction(1, gep);
            Operand initreg=GetNewRegOperand(init_val_reg);
            Operand addreg=GetNewRegOperand(addr_reg);
            IRgenStore(block, TLLvm[ArrayType], initreg, addreg);
            pos++;
        }
    }
}

/*
     %a = alloca i32
    store i32 1, i32* %a
*/
void VarDecl::codeIR() {
    LLVMBlock B = llvmIR.GetBlock(now_function, 0);
    LLVMBlock initB = llvmIR.GetBlock(now_function, now_label);
    auto def_vector = *var_def_list;
    // 逐个处理变量定义
    for (auto def : def_vector) {
        VarAttribute val;
        val.type = type_decl;
        irgen_table.symbol_table.add_Symbol(def->get_name(), ++regnumber);
        int alloca_reg = regnumber;

        // 处理普通变量
        if (def->get_dims() == nullptr) {
            IRgenAlloca(B, TLLvm[type_decl], alloca_reg);
            irgen_table.RegTable[alloca_reg] = val;
            // 初始化
            Operand val_operand;
            InitVal init = def->get_init();
            if (init == nullptr) {
                if (type_decl == Type::INT) {
                    IRgenArithmeticI32ImmAll(initB, BasicInstruction::LLVMIROpcode::ADD, 0, 0, ++regnumber);
                    val_operand = GetNewRegOperand(regnumber);//得到一个全0的操作数，方便后续的初始化
                } else if (type_decl == Type::FLOAT) {
                    IRgenArithmeticF32ImmAll(initB, BasicInstruction::LLVMIROpcode::FADD, 0, 0, ++regnumber);
                    val_operand = GetNewRegOperand(regnumber);
                }    // 对于未初始化的变量将其初始化为0
            } else {
                Expression initExp = init->GetExp();
                initExp->codeIR();
                if(initExp->attribute.T.type!=type_decl)
                {
                IRgenTypeConverse(initB, initExp->attribute.T.type, type_decl, regnumber);
                }
                val_operand = GetNewRegOperand(regnumber);
            }
            Operand alloca= GetNewRegOperand(alloca_reg);
            IRgenStore(initB, TLLvm[type_decl], val_operand, alloca);
        } 
        // 处理数组变量
        // Reference: https://github.com/yuhuifishash/NKU-Compilers2024-RV64GC.git/ir_gen/IRgen.cc line 611-626
        else {
            auto dim_vector = *def->get_dims();
            for (auto d : dim_vector) {
                val.dims.push_back(d->attribute.V.val.IntVal);
            }
            IRgenAllocaArray(B, TLLvm[type_decl], alloca_reg, val.dims);
            irgen_table.RegTable[alloca_reg] = val;
            InitVal init = def->get_init();
            if (init != nullptr) {
                int size = 1;
                for (auto d : val.dims) {
                    size *= d;
                }
                CallInstruction *memsetCall =
                new CallInstruction(BasicInstruction::LLVMType::VOID, nullptr, std::string("llvm.memset.p0.i32"));
                memsetCall->push_back_Parameter(BasicInstruction::LLVMType::PTR,
                                                GetNewRegOperand(alloca_reg));    // array address
                memsetCall->push_back_Parameter(BasicInstruction::LLVMType::I8, new ImmI32Operand(0));
                memsetCall->push_back_Parameter(BasicInstruction::LLVMType::I32, new ImmI32Operand(size * sizeof(int)));
                memsetCall->push_back_Parameter(BasicInstruction::LLVMType::I1, new ImmI32Operand(0));
                llvmIR.function_block_map[now_function][now_label]->InsertInstruction(1, memsetCall);
                // 递归初始化数组元素
                RecursiveArrayInitIR(initB, val.dims, alloca_reg, init, 0, size - 1, 0, type_decl);
            }
        }
    }
}

void ConstDecl::codeIR() {

    LLVMBlock B = llvmIR.GetBlock(now_function, 0);
    LLVMBlock initB = llvmIR.GetBlock(now_function, now_label);
    auto def_vector = *var_def_list;
    for (auto def : def_vector) {
        VarAttribute val;
        val.type = type_decl;
        irgen_table.symbol_table.add_Symbol(def->get_name(), ++regnumber);
        int alloca_reg = regnumber;
        if (def->get_dims() == nullptr) {
            IRgenAlloca(B, TLLvm[type_decl], alloca_reg);
            irgen_table.RegTable[alloca_reg] = val;
            Operand val_operand;
            InitVal init = def->get_init();
            assert(init != nullptr);
            Expression initExp = init->GetExp();
            initExp->codeIR();
            IRgenTypeConverse(initB, initExp->attribute.T.type, type_decl, regnumber);
            val_operand = GetNewRegOperand(regnumber);
            IRgenStore(initB, TLLvm[type_decl], val_operand, GetNewRegOperand(alloca_reg));
        } else {
            auto dim_vector = *def->get_dims();
            for (auto d : dim_vector) {
                val.dims.push_back(d->attribute.V.val.IntVal);
            }
            IRgenAllocaArray(B, TLLvm[type_decl], alloca_reg, val.dims);
            irgen_table.RegTable[alloca_reg] = val;

            // Reference: https://github.com/yuhuifishash/NKU-Compilers2024-RV64GC.git/ir_gen/IRgen.cc line 674-687
            InitVal init = def->get_init();
            if (init != nullptr) {
                int array_sz = 1;
                for (auto d : val.dims) {
                    array_sz *= d;
                }

        // 创建一个调用llvm.memset.p0.i32内建函数的指令，用于初始化数组
        CallInstruction *memsetCall =
        new CallInstruction(BasicInstruction::LLVMType::VOID, nullptr, "llvm.memset.p0.i32");
        // 添加参数：数组地址
        memsetCall->push_back_Parameter(BasicInstruction::LLVMType::PTR, GetNewRegOperand(alloca_reg));
        // 添加参数：要设置的值（0）
        memsetCall->push_back_Parameter(BasicInstruction::LLVMType::I8, new ImmI32Operand(0)); 
        // 添加参数：要设置内存区域的大小（数组大小乘以元素大小）
        memsetCall->push_back_Parameter(BasicInstruction::LLVMType::I32, new ImmI32Operand(array_sz * sizeof(int)));
        // 添加参数：内存对齐（0表示没有特定对齐要求）
        memsetCall->push_back_Parameter(BasicInstruction::LLVMType::I1, new ImmI32Operand(0));
        // 将memset调用指令插入到当前函数的当前基本块中
        llvmIR.function_block_map[now_function][now_label]->InsertInstruction(1, memsetCall);
                RecursiveArrayInitIR(initB, val.dims, alloca_reg, init, 0, array_sz - 1, 0, type_decl);
            }
        }
    }
}

void BlockItem_Decl::codeIR() { decl->codeIR(); }

void BlockItem_Stmt::codeIR() { stmt->codeIR(); }

void __Block::codeIR() {
    irgen_table.symbol_table.enter_scope();

    auto item_vector = *item_list;
    for (auto item : item_vector) {
        item->codeIR();
    }

    irgen_table.symbol_table.exit_scope();
}

void __FuncFParam::codeIR() {  }

void __FuncDef::codeIR() {
    irgen_table.symbol_table.enter_scope();
    BasicInstruction::LLVMType FuncType = TLLvm[return_type];
    FuncDefInstruction newFuncDef = new FunctionDefineInstruction(FuncType, name->get_string());
    now_function = newFuncDef;
    function_returntype = return_type;
    llvmIR.NewFunction(now_function);
    // 初始化寄存器和符号表
    regnumber = 0;

    irgen_table.RegTable.clear();
    irgen_table.FormalArrayTable.clear();
    max_label = 0;
    LLVMBlock B = llvmIR.NewBlock(now_function, max_label);
    now_label = 0;
    // 处理形参
    auto formal_vector = *formals;
    regnumber = formal_vector.size() - 1;
    for (int i = 0; i < formal_vector.size(); i++) {
        auto formal = formal_vector[i];
        VarAttribute val;
        val.type = formal->type_decl;
        BasicInstruction::LLVMType lltype = TLLvm[formal->type_decl];
        // 处理形参
        if(formal->dims==nullptr){
            newFuncDef->InsertFormal(lltype);
            IRgenAlloca(B, lltype, ++regnumber);
            Operand regi=GetNewRegOperand(i);
            Operand regn=GetNewRegOperand(regnumber);
            IRgenStore(B, lltype, regi, regn);//将形参的值传入
            irgen_table.symbol_table.add_Symbol(formal->name, regnumber);
            irgen_table.RegTable[regnumber] = val;
        }
        else {//处理数组
            newFuncDef->InsertFormal(BasicInstruction::LLVMType::PTR);
            for (int d = 1; d < formal->dims->size(); d++) {//由于对于数组参数来说第一个高维度都为0，所以从下一维开始
                auto formal_dim = formal->dims->at(d);//从 dims 容器中获取索引为 d 的元素
                val.dims.push_back(formal_dim->attribute.V.val.IntVal);
            }
            irgen_table.FormalArrayTable[i] = 1;
            irgen_table.symbol_table.add_Symbol(formal->name, i);
            irgen_table.RegTable[i] = val;
        }
    }

    IRgenBRUnCond(B, 1);
    B = llvmIR.NewBlock(now_function, ++max_label);
    now_label = max_label;
    block->codeIR();
    AddNoReturnBlock();
    max_reg_map[newFuncDef] = regnumber;
    max_label_map[newFuncDef] = max_label;
    irgen_table.symbol_table.exit_scope();
}

void CompUnit_Decl::codeIR() {    // decl->codeIR();
}

void CompUnit_FuncDef::codeIR() { func_def->codeIR(); }

void AddLibFunctionDeclare() {
    FunctionDeclareInstruction *getint = new FunctionDeclareInstruction(BasicInstruction::I32, "getint");
    llvmIR.function_declare.push_back(getint);

    FunctionDeclareInstruction *getchar = new FunctionDeclareInstruction(BasicInstruction::I32, "getch");
    llvmIR.function_declare.push_back(getchar);

    FunctionDeclareInstruction *getfloat = new FunctionDeclareInstruction(BasicInstruction::FLOAT32, "getfloat");
    llvmIR.function_declare.push_back(getfloat);

    FunctionDeclareInstruction *getarray = new FunctionDeclareInstruction(BasicInstruction::I32, "getarray");
    getarray->InsertFormal(BasicInstruction::PTR);
    llvmIR.function_declare.push_back(getarray);

    FunctionDeclareInstruction *getfloatarray = new FunctionDeclareInstruction(BasicInstruction::I32, "getfarray");
    getfloatarray->InsertFormal(BasicInstruction::PTR);
    llvmIR.function_declare.push_back(getfloatarray);

    FunctionDeclareInstruction *putint = new FunctionDeclareInstruction(BasicInstruction::VOID, "putint");
    putint->InsertFormal(BasicInstruction::I32);
    llvmIR.function_declare.push_back(putint);

    FunctionDeclareInstruction *putch = new FunctionDeclareInstruction(BasicInstruction::VOID, "putch");
    putch->InsertFormal(BasicInstruction::I32);
    llvmIR.function_declare.push_back(putch);

    FunctionDeclareInstruction *putfloat = new FunctionDeclareInstruction(BasicInstruction::VOID, "putfloat");
    putfloat->InsertFormal(BasicInstruction::FLOAT32);
    llvmIR.function_declare.push_back(putfloat);

    FunctionDeclareInstruction *putarray = new FunctionDeclareInstruction(BasicInstruction::VOID, "putarray");
    putarray->InsertFormal(BasicInstruction::I32);
    putarray->InsertFormal(BasicInstruction::PTR);
    llvmIR.function_declare.push_back(putarray);

    FunctionDeclareInstruction *putfarray = new FunctionDeclareInstruction(BasicInstruction::VOID, "putfarray");
    putfarray->InsertFormal(BasicInstruction::I32);
    putfarray->InsertFormal(BasicInstruction::PTR);
    llvmIR.function_declare.push_back(putfarray);

    FunctionDeclareInstruction *starttime = new FunctionDeclareInstruction(BasicInstruction::VOID, "_sysy_starttime");
    starttime->InsertFormal(BasicInstruction::I32);
    llvmIR.function_declare.push_back(starttime);

    FunctionDeclareInstruction *stoptime = new FunctionDeclareInstruction(BasicInstruction::VOID, "_sysy_stoptime");
    stoptime->InsertFormal(BasicInstruction::I32);
    llvmIR.function_declare.push_back(stoptime);

    // 一些llvm自带的函数，也许会为你的优化提供帮助
    FunctionDeclareInstruction *llvm_memset =
    new FunctionDeclareInstruction(BasicInstruction::VOID, "llvm.memset.p0.i32");
    llvm_memset->InsertFormal(BasicInstruction::PTR);
    llvm_memset->InsertFormal(BasicInstruction::I8);
    llvm_memset->InsertFormal(BasicInstruction::I32);
    llvm_memset->InsertFormal(BasicInstruction::I1);
    llvmIR.function_declare.push_back(llvm_memset);

    FunctionDeclareInstruction *llvm_umax = new FunctionDeclareInstruction(BasicInstruction::I32, "llvm.umax.i32");
    llvm_umax->InsertFormal(BasicInstruction::I32);
    llvm_umax->InsertFormal(BasicInstruction::I32);
    llvmIR.function_declare.push_back(llvm_umax);

    FunctionDeclareInstruction *llvm_umin = new FunctionDeclareInstruction(BasicInstruction::I32, "llvm.umin.i32");
    llvm_umin->InsertFormal(BasicInstruction::I32);
    llvm_umin->InsertFormal(BasicInstruction::I32);
    llvmIR.function_declare.push_back(llvm_umin);

    FunctionDeclareInstruction *llvm_smax = new FunctionDeclareInstruction(BasicInstruction::I32, "llvm.smax.i32");
    llvm_smax->InsertFormal(BasicInstruction::I32);
    llvm_smax->InsertFormal(BasicInstruction::I32);
    llvmIR.function_declare.push_back(llvm_smax);

    FunctionDeclareInstruction *llvm_smin = new FunctionDeclareInstruction(BasicInstruction::I32, "llvm.smin.i32");
    llvm_smin->InsertFormal(BasicInstruction::I32);
    llvm_smin->InsertFormal(BasicInstruction::I32);
    llvmIR.function_declare.push_back(llvm_smin);
}
