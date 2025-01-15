#ifndef IRGEN_H
#define IRGEN_H

#include "../include/SysY_tree.h"
#include "../include/cfg.h"
#include "../include/symtab.h"
#include "../include/type.h"
#include <assert.h>
#include <map>
#include <vector>

int regnumber=0;
class IRgenTable {
public:
    // 如果你无从下手,推荐先阅读LLVMIR类的printIR函数,了解我们是如何输出中间代码的
    // 然后你可以尝试往和输出相关联的变量中随便添加一些函数定义指令, 新建一些基本块或添加几条指令看看输出是怎么变化的
    // 弄懂LLVMIR类是如何存储中间代码后，剩余的就是理解中间代码生成算法了

    // TODO():添加更多你需要的成员变量和成员函数
    SymbolRegTable symbol_table;
    std::map<int, int> FormalArrayTable;
    std::map<int,VarAttribute> RegTable;
    std::map<int, int> FormalArrayTable;
    std::map<int,VarAttribute> RegTable;
    IRgenTable() {}
};
enum op{
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
void Binary(tree_node *a, tree_node *b, op opcode, LLVMBlock B){
    int left=a->attribute.T.type;
    int right=b->attribute.T.type;
    if(left==Type::INT&&right==Type::INT){
        a->codeIR();
        int reg1=regnumber;
        b->codeIR();
        int reg2=regnumber;
        BinaryIrInt(B,opcode,reg1,reg2);
    }else if(left=Type::FLOAT&&right==Type::INT){
         a->codeIR();
        int reg1 = regnumber;
         b->codeIR();
         int reg2 = regnumber;
       IRgenSitofp(B, reg2, ++regnumber);    // b int->float
        reg2 = regnumber;
        BinaryIrFloat(B,opcode,reg1,reg2);
    }else if(left=Type::FLOAT&&right==Type::FLOAT){
         a->codeIR();
        int reg1=regnumber;
        b->codeIR();
        int reg2=regnumber;
        BinaryIrFloat(B,opcode,reg1,reg2);
    }else if(left=Type::INT&&right==Type::FLOAT){
         a->codeIR();
        int reg1 = regnumber;
         b->codeIR();
         int reg2 = regnumber;
        IRgenSitofp(B, reg1, ++regnumber);    // a int->float
        reg1 = regnumber;
        BinaryIrFloat(B,opcode,reg1,reg2);
    }else if(left=Type::BOOL&&right==Type::BOOL){
        a->codeIR();
        int reg1=regnumber;
        b->codeIR();
        int reg2=regnumber;
        IRgenZextI1toI32(B,reg1,++regnumber);
        reg1=regnumber;
        IRgenZextI1toI32(B,reg2,++regnumber);
        reg2=regnumber;
        BinaryIrInt(B,opcode,reg1,reg2);
    }else if(left=Type::BOOL&&right==Type::INT){
        a->codeIR();
        int reg1=regnumber;
        b->codeIR();
        int reg2=regnumber;
        IRgenZextI1toI32(B,reg1,++regnumber);
        reg1=regnumber;
        BinaryIrInt(B,opcode,reg1,reg2);
    }else if(left=Type::INT&&right==Type::BOOL){
        a->codeIR();
        int reg1=regnumber;
        b->codeIR();
        int reg2=regnumber;
        IRgenZextI1toI32(B,reg2,++regnumber);
        reg2=regnumber;
        BinaryIrInt(B,opcode,reg1,reg2);
    }else if(left=Type::BOOL&&right==Type::FLOAT){
         a->codeIR();
        int reg1=regnumber;
        b->codeIR();
        int reg2=regnumber;
        IRgenZextI1toI32(B,reg1,++regnumber);
        reg1=regnumber;
        IRgenSitofp(B,reg1,++regnumber);
        reg1=regnumber;
        BinaryIrFloat(B,opcode,reg1,reg2);
    }else if(left=Type::FLOAT&&right==Type::BOOL){
         a->codeIR();
        int reg1=regnumber;
        b->codeIR();
        int reg2=regnumber;
        IRgenZextI1toI32(B,reg2,++regnumber);
        reg2=regnumber;
        IRgenSitofp(B,reg2,++regnumber);
        reg2=regnumber;
        BinaryIrFloat(B,opcode,reg1,reg2);
    }else if(left=Type::PTR&&right==Type::BOOL){
        assert(false);
    }else if(left=Type::PTR&&right==Type::PTR){
        assert(false);
    }else if(left=Type::BOOL&&right==Type::PTR){
        assert(false);
    }else if(left=Type::INT&&right==Type::PTR){
        assert(false);
    }else if(left=Type::PTR&&right==Type::INT){
        assert(false);
    }else if(left=Type::FLOAT&&right==Type::PTR){
        assert(false);
    }else if(left=Type::PTR&&right==Type::FLOAT){
        assert(false);
    }else if(left=Type::VOID&&right==Type::PTR){
        assert(false);
    }else if(left=Type::VOID&&right==Type::VOID){
        assert(false);
    }else if(left=Type::PTR&&right==Type::VOID){
        assert(false);
    }else if(left=Type::INT&&right==Type::VOID){
        assert(false);
    }else if(left=Type::FLOAT&&right==Type::VOID){
        assert(false);
    }else if(left=Type::VOID&&right==Type::FLOAT){
        assert(false);
    }else if(left=Type::VOID&&right==Type::INT){
        assert(false);
    }else if(left=Type::VOID&&right==Type::BOOL){
        assert(false);
    }else if(left=Type::BOOL&&right==Type::VOID){
        assert(false);
    }
}
void BinaryIrInt(LLVMBlock B, op opcode, int reg1, int reg2){
    if(opcode==op::ADD){
        IRgenArithmeticI32(B, BasicInstruction::LLVMIROpcode::ADD, reg1, reg2, regnumber++);
    } else if(opcode==op::SUB){
        IRgenArithmeticI32(B, BasicInstruction::LLVMIROpcode::SUB, reg1, reg2, regnumber++);
    } else if(opcode==op::MUL){
        IRgenArithmeticI32(B, BasicInstruction::LLVMIROpcode::MUL, reg1, reg2, regnumber++);
    } else if(opcode==op::DIV){
        IRgenArithmeticI32(B, BasicInstruction::LLVMIROpcode::DIV, reg1, reg2, regnumber++);
    } else if(opcode==op::MOD){
        IRgenArithmeticI32(B, BasicInstruction::LLVMIROpcode::MOD, reg1, reg2, regnumber++);
    } else if(opcode==op::GEQ){
        IRgenIcmp(B, BasicInstruction::IcmpCond::sge, reg1, reg2, regnumber++);
    } else if(opcode==op::GT){
        IRgenIcmp(B, BasicInstruction::IcmpCond::sgt, reg1, reg2, regnumber++);
    } else if(opcode==op::LEQ){
        IRgenIcmp(B, BasicInstruction::IcmpCond::sle, reg1, reg2, regnumber++);
    } else if(opcode==op::LT){
        IRgenIcmp(B, BasicInstruction::IcmpCond::slt, reg1, reg2, regnumber++);
    } else if(opcode==op::EQ){
        IRgenIcmp(B, BasicInstruction::IcmpCond::eq, reg1, reg2, regnumber++);
    } else if(opcode==op::NE){
        IRgenIcmp(B, BasicInstruction::IcmpCond::ne, reg1, reg2, regnumber++);
    } 
    else{
        assert(false); 
    }
}

void BinaryIrFloat(LLVMBlock B, op opcode, int reg1, int reg2){
    if(opcode==op::ADD){
        IRgenArithmeticF32(B, BasicInstruction::LLVMIROpcode::FADD, reg1, reg2, regnumber++);
    } else if(opcode==op::SUB){
        IRgenArithmeticF32(B, BasicInstruction::LLVMIROpcode::FSUB, reg1, reg2, regnumber++);
    } else if(opcode==op::MUL){
        IRgenArithmeticF32(B, BasicInstruction::LLVMIROpcode::FMUL, reg1, reg2, regnumber++);
    } else if(opcode==op::DIV){
        IRgenArithmeticF32(B, BasicInstruction::LLVMIROpcode::FDIV, reg1, reg2, regnumber++);
    } else if(opcode==op::GEQ){
        IRgenFcmp(B, BasicInstruction::FcmpCond::OGE, reg1, reg2, regnumber++);
    } else if(opcode==op::GT){
        IRgenFcmp(B, BasicInstruction::FcmpCond::OGT, reg1, reg2, regnumber++);
    } else if(opcode==op::LEQ){
        IRgenFcmp(B, BasicInstruction::FcmpCond::OLE, reg1, reg2, regnumber++);
    } else if(opcode==op::LT){
        IRgenFcmp(B, BasicInstruction::FcmpCond::OLT, reg1, reg2, regnumber++);
    } else if(opcode==op::EQ){
        IRgenFcmp(B, BasicInstruction::FcmpCond::OEQ, reg1, reg2, regnumber++);
    } else if(opcode==op::NE){
        IRgenFcmp(B, BasicInstruction::FcmpCond::ONE, reg1, reg2, regnumber++);
    } 
    else{
        assert(false); 
    }
}
void Single(tree_node *a, op opcode, LLVMBlock B){
    int b=a->attribute.T.type;
    if(b==Type::FLOAT){
        a->codeIR();
        int reg=regnumber;
        SingleIrFloat(B,opcode,reg);
    }else if(b==Type::INT){
        a->codeIR();
        int reg=regnumber;
        SingleIrInt(B,opcode,reg);
    }else if(b==Type::BOOL){
        a->codeIR();
        int reg = regnumber;
        IRgenZextI1toI32(B, reg, ++regnumber);
        reg = regnumber;
        SingleIrInt(B,opcode,reg);
    } else{
        assert(false);
    }
}

void SingleIrInt(LLVMBlock B, op opcode, int reg){
    if(opcode==op::ADD){

    } else if(opcode==op::SUB){
        IRgenArithmeticI32ImmLeft(B, BasicInstruction::LLVMIROpcode::SUB, 0, reg, regnumber++);
    } else if(opcode==op::NOT){
        IRgenIcmpImmRight(B, BasicInstruction::IcmpCond::eq, reg, 0, regnumber++);
    } 
    else{
        assert(false);
    }
}
void SingleIrFloat(LLVMBlock B, op opcode, int reg){
    if(opcode==op::ADD){

    } else if(opcode==op::SUB){
        IRgenArithmeticF32ImmLeft(B, BasicInstruction::LLVMIROpcode::SUB, 0, reg, regnumber++);
    } else if(opcode==op::NOT){
        IRgenFcmpImmRight(B, BasicInstruction::FcmpCond::OEQ, reg, 0, regnumber++);
    }
    else{
        assert(false);
    }
}
#endif

