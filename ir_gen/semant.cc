#include "semant.h"
#include "../include/SysY_tree.h"
#include "../include/ir.h"
#include "../include/type.h"
/*
    语义分析阶段需要对语法树节点上的类型和常量等信息进行标注, 即NodeAttribute类
    同时还需要标注每个变量的作用域信息，即部分语法树节点中的scope变量
    你可以在utils/ast_out.cc的输出函数中找到你需要关注哪些语法树节点中的NodeAttribute类及其他变量
    以及对语义错误的代码输出报错信息
*/

/*
    错误检查的基本要求:
    • 检查 main 函数是否存在 (根据SysY定义，如果不存在main函数应当报错)；
    • 检查未声明变量，及在同一作用域下重复声明的变量；
    • 条件判断和运算表达式：int 和 bool 隐式类型转换（例如int a=5，return a+!a）；
    • 数值运算表达式：运算数类型是否正确 (如返回值为 void 的函数调用结果是否参与了其他表达式的计算)；
    • 检查未声明函数，及函数形参是否与实参类型及数目匹配；
    • 检查是否存在整型变量除以整型常量0的情况 (如对于表达式a/(5-4-1)，编译器应当给出警告或者直接报错)；

    错误检查的进阶要求:
    • 对数组维度进行相应的类型检查 (例如维度是否有浮点数，定义维度时是否为常量等)；
    • 对float进行隐式类型转换以及其他float相关的检查 (例如模运算中是否有浮点类型变量等)；
*/
extern LLVMIR llvmIR;

SemantTable semant_table;
std::vector<std::string> error_msgs{}; // 将语义错误信息保存到该变量中

bool ishasmain=false;
int hasloop=0;
bool isinfunc=false;
int returnnumi=1;
float returnnumf=1.0;
bool isinreturn=false;
bool iscond=false;
NodeAttribute TypeConvert(const NodeAttribute attr, Type::ty targetType) {
    NodeAttribute result = attr;
    result.T.type = targetType;

    if (targetType == Type::FLOAT && attr.T.type == Type::INT) {
        result.V.val.FloatVal = (float)(attr.V.val.IntVal);
    }
    else if (targetType == Type::INT && attr.T.type == Type::BOOL) {
        result.V.val.IntVal = attr.V.val.BoolVal ? 1 : 0;
    }
    else if(targetType == Type::BOOL && attr.T.type == Type::INT) {
        result.V.val.BoolVal = attr.V.val.IntVal!=0 ? true : false;
    }

    return result;
}

void ImplicitConvert(NodeAttribute &left, NodeAttribute &right, Type::ty targetType, int line_number) {
    if (left.T.type != targetType) {
        left = TypeConvert(left, targetType);
    }
    if (right.T.type != targetType) {
        right = TypeConvert(right, targetType);
    }
    if (left.T.type != right.T.type || left.T.type != targetType) {
        error_msgs.push_back("Wrong type in operands at line " + std::to_string(line_number) + "\n");
        return;
    }
}
NodeAttribute SingleOperation(NodeAttribute attr, std::string op, int line_number) {
    NodeAttribute result;
    Type::ty commonType;
    result.V.ConstTag = attr.V.ConstTag;
    if (attr.T.type == Type::BOOL) {
        attr = TypeConvert(attr, Type::INT);
    }
    // if(result.V.ConstTag){
    //     error_msgs.push_back("Not a const " + std::to_string(line_number) + "\n");
    // }
    if (attr.T.type == Type::INT){
        result.T.type=Type::INT;
        if (op == "+") {
            result.V.val.IntVal = attr.V.val.IntVal;
        } else if (op == "-") {
            result.V.val.IntVal = attr.V.val.IntVal * -1;
        } else if (op == "!") {
            attr = TypeConvert(attr, Type::BOOL);
            result.T.type=Type::BOOL;
            result.V.val.BoolVal = !attr.V.val.BoolVal;
        }
        else{
            error_msgs.push_back("Invalid operator '" + op + "' at line " + std::to_string(line_number) + "\n");
        }
    }
    else if (attr.T.type == Type::FLOAT){
        result.T.type=Type::FLOAT;
        if (op == "+") {
            result.V.val.FloatVal = attr.V.val.FloatVal;
        } else if (op == "-") {
            result.V.val.FloatVal = attr.V.val.FloatVal * -1.0f;
        } else if (op == "!") {
            attr = TypeConvert(attr, Type::BOOL);
            result.T.type=Type::BOOL;
            result.V.val.BoolVal = !attr.V.val.BoolVal;
        }
        else{
            error_msgs.push_back("Invalid operator '" + op + "' at line " + std::to_string(line_number) + "\n");
        }
    }else {
         error_msgs.push_back("Invalid operator '" + op + "' at line " + std::to_string(line_number) + "\n");
    }
    return result;
}
NodeAttribute BinaryOperation(NodeAttribute left, NodeAttribute right, std::string op, int line_number) {
    NodeAttribute result;
    Type::ty commonType;

    if (left.T.type == Type::FLOAT || right.T.type == Type::FLOAT) {
        commonType = Type::FLOAT;
    }
    else if (left.T.type == Type::BOOL || right.T.type == Type::BOOL) {
        commonType = Type::INT; 
    }else if(left.T.type == Type::VOID || right.T.type == Type::VOID){
        error_msgs.push_back("invalid operators in line " + std::to_string(line_number) + "\n");
    }

    ImplicitConvert(left, right, commonType, line_number);

    result.T.type = commonType;
    result.V.ConstTag = left.V.ConstTag & right.V.ConstTag;
    // if(result.V.ConstTag){
    //     error_msgs.push_back("Not a const " + std::to_string(line_number) + "\n");
    // }
    if (commonType == Type::INT) {
        if (op == "+") {
            result.V.val.IntVal = left.V.val.IntVal + right.V.val.IntVal;
        } else if (op == "-") {
            result.V.val.IntVal = left.V.val.IntVal - right.V.val.IntVal;
        } else if (op == "*") {
            result.V.val.IntVal = left.V.val.IntVal * right.V.val.IntVal;
        } else if (op == "/") {
            if(right.V.val.IntVal == 0){
                error_msgs.push_back("Cannot be zero at line " + std::to_string(line_number) + "\n");
            }
            else result.V.val.IntVal = left.V.val.IntVal / right.V.val.IntVal;
        } else if(op == "%") {
            result.V.val.IntVal = left.V.val.IntVal % right.V.val.IntVal;
        } else if(op == "<=") {
            result.T.type = Type::BOOL;
            result.V.val.BoolVal = (left.V.val.IntVal <= right.V.val.IntVal);
        } else if(op == "<") {
            result.T.type = Type::BOOL;
            result.V.val.BoolVal = (left.V.val.IntVal < right.V.val.IntVal);
        } else if(op == ">=") {
            result.T.type = Type::BOOL;
            result.V.val.BoolVal = (left.V.val.IntVal >= right.V.val.IntVal);
        } else if(op == ">") {
            result.T.type = Type::BOOL;
            result.V.val.BoolVal = (left.V.val.IntVal > right.V.val.IntVal);
        } else if(op == "==") {
            result.T.type = Type::BOOL;
            result.V.val.BoolVal = (left.V.val.IntVal == right.V.val.IntVal);
        } else if(op == "!=") {
            result.T.type = Type::BOOL;
            result.V.val.BoolVal = (left.V.val.IntVal != right.V.val.IntVal);
        }else if(op == "&&") {
            result.T.type = Type::BOOL;
            left = TypeConvert(left, Type::BOOL);
            right = TypeConvert(left, Type::BOOL);
            result.V.val.BoolVal = (left.V.val.BoolVal && right.V.val.BoolVal);
        } else if(op == "||") {
            result.T.type = Type::BOOL;
            left = TypeConvert(left, Type::BOOL);
            right = TypeConvert(left, Type::BOOL);
            result.V.val.BoolVal = (left.V.val.BoolVal || right.V.val.BoolVal);
        }
        else{
            error_msgs.push_back("Unsupported operator '" + op + "' at line " + std::to_string(line_number) + "\n");
        }
    }
    else if (commonType == Type::FLOAT) {
        if (op == "+") {
            result.V.val.FloatVal = left.V.val.FloatVal + right.V.val.FloatVal;
        } else if (op == "-") {
            result.V.val.FloatVal = left.V.val.FloatVal - right.V.val.FloatVal;
        } else if (op == "*") {
            result.V.val.FloatVal = left.V.val.FloatVal * right.V.val.FloatVal;
        } else if (op == "/") {
            if(right.V.val.FloatVal == 0.0f){
                error_msgs.push_back("Cannot be zero at line " + std::to_string(line_number) + "\n");
            }
            else result.V.val.FloatVal = left.V.val.FloatVal / right.V.val.FloatVal;
        } else if(op == "<=") {
            result.T.type = Type::BOOL;
            result.V.val.BoolVal = (left.V.val.FloatVal <= right.V.val.FloatVal);
        } else if(op == "<") {
            result.T.type = Type::BOOL;
            result.V.val.BoolVal = (left.V.val.FloatVal < right.V.val.FloatVal);
        } else if(op == ">=") {
            result.T.type = Type::BOOL;
            result.V.val.BoolVal = (left.V.val.FloatVal >= right.V.val.FloatVal);
        } else if(op == ">") {
            result.T.type = Type::BOOL;
            result.V.val.BoolVal = (left.V.val.FloatVal > right.V.val.FloatVal);
        } else if(op == "==") {
            result.T.type = Type::BOOL;
            result.V.val.BoolVal = (left.V.val.FloatVal == right.V.val.FloatVal);
        } else if(op == "!=") {
            result.T.type = Type::BOOL;
            result.V.val.BoolVal = (left.V.val.FloatVal != right.V.val.FloatVal);
        } else if(op == "&&") {
            result.T.type = Type::BOOL;
            left = TypeConvert(left, Type::BOOL);
            right = TypeConvert(left, Type::BOOL);
            result.V.val.BoolVal = (left.V.val.BoolVal && right.V.val.BoolVal);
        } else if(op == "||") {
            result.T.type = Type::BOOL;
            left = TypeConvert(left, Type::BOOL);
            right = TypeConvert(left, Type::BOOL);
            result.V.val.BoolVal = (left.V.val.BoolVal || right.V.val.BoolVal);
        }
        else{
            error_msgs.push_back("Unsupported operator '" + op + "' at line " + std::to_string(line_number) + "\n");
        }
    }
    return result;
}

void Arrayinit(InitVal init, VarAttribute &v,int begPos, int dimsIdx){
    int pos=begPos;
    for(InitVal i:*(init->GetList())){
        if(i->IsExp()){//初始化中为{3，4}这种情况
            int p=i->attribute.T.type;//初始化列表的类型
            if(p==Type::VOID){
                error_msgs.push_back("exp can not be void in initval in line " + std::to_string(init->GetLineNumber()) +
                                     "\n");
            }
            if(v.type==Type::INT){//数组类型为整型
                if(p==Type::INT){
                    v.IntInitVals[pos]=i->attribute.V.val.IntVal;
                }else if(p==Type::FLOAT){//不允许浮点型初始化整型数组
                    error_msgs.push_back("Floating-point numbers cannot be used to initialize integer arrays. in line " + std::to_string(init->GetLineNumber()) +
                                     "\n");
                }
            }if(v.type==Type::FLOAT){
                if(p==Type::INT){
                    v.FloatInitVals[pos]=i->attribute.V.val.IntVal;
                }else if(p==Type::FLOAT){
                    v.FloatInitVals[pos]=i->attribute.V.val.FloatVal;
                }
            }
            pos++;
        }else{//初始化中有{3,4，{4,5}}嵌套的情况
            int Block_size=0;
            int min_dim=1;
            for(int j=dimsIdx+1;j<v.dims.size();j++){
                Block_size*=v.dims[j];
            }
            int position=pos-begPos;
            while(position%Block_size!=0){
                Block_size/=v.dims[dimsIdx+min_dim];
                min_dim++;
            }
            Arrayinit(i,v,pos,dimsIdx+min_dim);
            pos+=Block_size;
        }
    }

}


void initintconst(InitVal init,VarAttribute &v){
     int size=1;
       for(auto dim:v.dims){
         size*=dim;
       }
       v.IntInitVals.resize(size, 0);
    if(v.dims.empty()){//非数组常量的初始化处理
        if(init->GetExp()==nullptr){ 
            error_msgs.push_back("The initval is invalid in line " +
                                     std::to_string(init->GetLineNumber()) + "\n");
            return;}//非数组初始化只能使用表达式

        auto exp=init->GetExp();
        if(exp->attribute.T.type==Type::VOID){
             error_msgs.push_back("Expression can't be void in initval in line " +
                                     std::to_string(init->GetLineNumber()) + "\n");
        }
        else if(exp->attribute.T.type==Type::INT){
           v.IntInitVals[0]=exp->attribute.V.val.IntVal;
        }
        else if(exp->attribute.T.type==Type::FLOAT){
           v.IntInitVals[0]=exp->attribute.V.val.FloatVal;
        }
     }
     else{//常量数组初始化的处理
       if(init->IsExp()==1){
        if(init->GetExp()!=nullptr){
            error_msgs.push_back("The initVal of array can't only be a number in line " + std::to_string(init->GetLineNumber()) + "\n");

        }
        return;
       }else{
           Arrayinit(init,v,0,0);
       }
    }
}
void initfloatconst(InitVal init,VarAttribute &v){
       int size=1;
       for(auto dim:v.dims){
         size*=dim;
       }
        v.FloatInitVals.resize(size, 0);
    if(v.dims.empty()){//非数组常量的初始化处理
        if(init->GetExp()==nullptr) {
             error_msgs.push_back("The initval is invalid in line " +
                                     std::to_string(init->GetLineNumber()) + "\n");
            return;}//非数组初始化只能使用表达式

        auto exp=init->GetExp();
        if(exp->attribute.T.type==Type::VOID){
             error_msgs.push_back("Expression can't be void in initval in line " +
                                     std::to_string(init->GetLineNumber()) + "\n");
        }
        else if(exp->attribute.T.type==Type::INT){
           v.FloatInitVals[0]=exp->attribute.V.val.IntVal;
        }else if(exp->attribute.T.type==Type::FLOAT){
           v.FloatInitVals[0]=exp->attribute.V.val.FloatVal;
        }
    }else{//常量数组初始化的处理
       if(init->IsExp()==1){
        if(init->GetExp()!=nullptr){
            error_msgs.push_back("The initVal of array can't only be a number in line " + std::to_string(init->GetLineNumber()) + "\n");

        }
        return;
       }else{
           Arrayinit(init,v,0,0);
       }
    }
}
void __Program::TypeCheck() {
    semant_table.symbol_table.enter_scope();
    auto comp_vector = *comp_list;
    for (auto comp : comp_vector) {
        comp->TypeCheck();
    }
    if(!ishasmain){
        error_msgs.push_back("the main function does not exist \n");
    }
}

void Exp::TypeCheck() {
    addexp->TypeCheck();

    attribute = addexp->attribute;
}

void AddExp_plus::TypeCheck() {
    addexp->TypeCheck();
    mulexp->TypeCheck();

    attribute = BinaryOperation(addexp->attribute, mulexp->attribute, "+", line_number);
}

void AddExp_sub::TypeCheck() {
    addexp->TypeCheck();
    mulexp->TypeCheck();

    attribute = BinaryOperation(addexp->attribute, mulexp->attribute, "-", line_number);
}

void MulExp_mul::TypeCheck() {
    mulexp->TypeCheck();
    unary_exp->TypeCheck();

    attribute = BinaryOperation(mulexp->attribute, unary_exp->attribute, "*", line_number);
}

void MulExp_div::TypeCheck() {
    mulexp->TypeCheck();
    unary_exp->TypeCheck();

    attribute = BinaryOperation(mulexp->attribute, unary_exp->attribute, "/", line_number);
}

void MulExp_mod::TypeCheck() {
    mulexp->TypeCheck();
    unary_exp->TypeCheck();

    attribute = BinaryOperation(mulexp->attribute, unary_exp->attribute, "%", line_number);
}

void RelExp_leq::TypeCheck() {
    relexp->TypeCheck();
    addexp->TypeCheck();

    attribute = BinaryOperation(relexp->attribute, addexp->attribute, "<=", line_number);
}

void RelExp_lt::TypeCheck() {
    relexp->TypeCheck();
    addexp->TypeCheck();

    attribute = BinaryOperation(relexp->attribute, addexp->attribute, "<", line_number);
}

void RelExp_geq::TypeCheck() {
    relexp->TypeCheck();
    addexp->TypeCheck();

    attribute = BinaryOperation(relexp->attribute, addexp->attribute, ">=", line_number);
}

void RelExp_gt::TypeCheck() {
    relexp->TypeCheck();
    addexp->TypeCheck();

    attribute = BinaryOperation(relexp->attribute, addexp->attribute, ">", line_number);
}

void EqExp_eq::TypeCheck() {
    eqexp->TypeCheck();
    relexp->TypeCheck();

    attribute = BinaryOperation(eqexp->attribute, relexp->attribute, "==", line_number);
}

void EqExp_neq::TypeCheck() {
    eqexp->TypeCheck();
    relexp->TypeCheck();

    attribute = BinaryOperation(eqexp->attribute, relexp->attribute, "!=", line_number);
}

void LAndExp_and::TypeCheck() {
    landexp->TypeCheck();
    eqexp->TypeCheck();

    attribute = BinaryOperation(landexp->attribute, eqexp->attribute, "&&", line_number);
}
void LOrExp_or::TypeCheck() {
    lorexp->TypeCheck();
    landexp->TypeCheck();

    attribute = BinaryOperation(lorexp->attribute, landexp->attribute, "||", line_number);
}

void ConstExp::TypeCheck() {
    addexp->TypeCheck();
    attribute = addexp->attribute;
    if (!attribute.V.ConstTag) {    // addexp is not const
        error_msgs.push_back("Expression is not const " + std::to_string(line_number) + "\n");
    }
}

void Lval::TypeCheck() { //变量作为左值使用时的检查
    is_left=false;
    VarAttribute val=semant_table.symbol_table.lookup_val(name);
    VarAttribute v1=semant_table.symbol_table.lookup_val(name);
    if (val.type == Type::VOID) {
        if (semant_table.GlobalTable.find(name) != semant_table.GlobalTable.end()) {
            val = semant_table.GlobalTable[name];
            if(val.type==Type::INT){
            attribute.V.val.IntVal=val.IntInitVals[0];
            }else if(val.type==Type::FLOAT){
            attribute.V.val.FloatVal=val.FloatInitVals[0];
            }
            scope = 0;
        } else {
            if(iscond){
                error_msgs.push_back("the cond type is invalid in line " + std::to_string(line_number) + "\n");
            }
                error_msgs.push_back("Undefined var in line " + std::to_string(line_number) + "\n");
            
            return;
        }
    } else {
        scope = semant_table.symbol_table.lookup_scope(name);
    }

    std::vector<int> arrayindexs; 
    bool arrayindexConstTag = true;  
    if (dims != nullptr) {  
        for (auto d : *dims) {  
            d->TypeCheck();  
            if (d->attribute.T.type == Type::VOID) {  
                error_msgs.push_back("Array Dim can not be void in line " + std::to_string(line_number) + "\n");
            } else if (d->attribute.T.type == Type::FLOAT) {  
                error_msgs.push_back("Array Dim can not be float in line " + std::to_string(line_number) + "\n");     
            }
            arrayindexs.push_back(d->attribute.V.val.IntVal);  
            arrayindexConstTag &= d->attribute.V.ConstTag;  
        }
    }
     if (arrayindexs.size() == val.dims.size()) {  
        attribute.V.ConstTag = val.ConstTag & arrayindexConstTag;  
        attribute.T.type = val.type;  
         if (attribute.V.ConstTag) {  
            if (attribute.T.type == Type::INT) {  
                int idx=0;
                for (int curIndex = 0; curIndex < arrayindexs.size(); curIndex++) {
                idx *= val.dims[curIndex];
                idx += arrayindexs[curIndex];
              }
                attribute.V.val.IntVal = val.IntInitVals[idx];  
            } else if (attribute.T.type == Type::FLOAT) {  
                int idx=0;
                for (int curIndex = 0; curIndex < arrayindexs.size(); curIndex++) {
                idx *= val.dims[curIndex];
                idx += arrayindexs[curIndex];
              }
                attribute.V.val.IntVal = val.FloatInitVals[idx];    
            }
        }else if(v1.type != Type::VOID){
             //printf("%d",10);
             //return;
            if (attribute.T.type == Type::INT) {  
                VarAttribute v=semant_table.symbol_table.lookup_val(name);
                attribute.V.val.IntVal=v.IntInitVals[0];
            } else if (attribute.T.type == Type::FLOAT) {  
                VarAttribute v=semant_table.symbol_table.lookup_val(name);
                attribute.V.val.FloatVal=v.FloatInitVals[0];
            }
        }
    } else if (arrayindexs.size() < val.dims.size()) {  
        attribute.V.ConstTag = false; 
        attribute.T.type = Type::PTR;  
    } else {
        error_msgs.push_back("Array is unmatched in line " + std::to_string(line_number) + "\n");  // 数组不匹配，报错
    }
    }

void FuncRParams::TypeCheck() {

 }

void Func_call::TypeCheck() { 
    //检查是否为puf函数，没有定义该函数
     if(name->get_string()=="putf"){
        return;
     }
     //检查该函数是否已经有声明
     auto fun=semant_table.FunctionTable.find(name);
     if(fun==semant_table.FunctionTable.end()){
        error_msgs.push_back("The function is undefined in line " + std::to_string(line_number) + "\n");
        return;
     }
     //检查函数的形参匹配问题
     int funcparams_len=0;//调用时的参数
     if(funcr_params!=nullptr){
        auto params=((FuncRParams *)funcr_params)->params;
        for(auto param:*params){
            param->TypeCheck();
            if(param->attribute.T.type==Type::VOID){
                error_msgs.push_back("The funcRParam is void in line " + std::to_string(line_number) + "\n");
            }
        }
        funcparams_len=params->size();
     }
     FuncDef f=fun->second;
     int f_len=f->formals->size();//实际定义时的参数
     
     if(f_len!=funcparams_len){
        error_msgs.push_back("Function FuncFParams and FuncRParams are not matched in line " + std::to_string(line_number) + "\n");
     }
    attribute.T.type = semant_table.FunctionTable[name]->return_type;
    if(f->returniszero==true&&attribute.T.type==Type::INT){
        attribute.V.val.IntVal=0;
    }else if(f->returniszero==true&&attribute.T.type==Type::FLOAT){
        attribute.V.val.FloatVal=0.0;
    }else if(f->returniszero==false&&attribute.T.type==Type::INT){
         attribute.V.val.IntVal=f->num.returnmuni;
         if(f->name->get_string()=="getint"||f->name->get_string()=="getarray"){
            attribute.V.val.IntVal=2147483647;
         }
    }else if(f->returniszero==false&&attribute.T.type==Type::FLOAT){
        attribute.V.val.FloatVal=f->num.returnmunf;
        if(f->name->get_string()=="getfloat"||f->name->get_string()=="getfarray"){
            attribute.V.val.FloatVal=2147483647.0;
         }
    }else{
        attribute.V.val.IntVal=1;
    }
    if(iscond==true&&attribute.T.type==Type::VOID){
        error_msgs.push_back("the cond type is invalid in line " + std::to_string(line_number) + "\n");   
    }
    attribute.V.ConstTag = false;
 }

void UnaryExp_plus::TypeCheck() {
    unary_exp->TypeCheck();
    attribute = SingleOperation(unary_exp->attribute, "+", line_number);
}

void UnaryExp_neg::TypeCheck() {
    unary_exp->TypeCheck();
    attribute = SingleOperation(unary_exp->attribute, "-", line_number);
}

void UnaryExp_not::TypeCheck() {
    unary_exp->TypeCheck();
    attribute = SingleOperation(unary_exp->attribute, "!", line_number);
}

void IntConst::TypeCheck() {
    attribute.T.type = Type::INT;
    attribute.V.ConstTag = true;
    attribute.V.val.IntVal = val;
}

void FloatConst::TypeCheck() {
    attribute.T.type = Type::FLOAT;
    attribute.V.ConstTag = true;
    attribute.V.val.FloatVal = val;
}

void StringConst::TypeCheck() { TODO("StringConst Semant"); }

void PrimaryExp_branch::TypeCheck() {
    exp->TypeCheck();
    attribute = exp->attribute;
}

void assign_stmt::TypeCheck() { 
    //  printf("%d",10);
    //  return;
    lval->TypeCheck();
    exp->TypeCheck();
    VarAttribute v=semant_table.symbol_table.lookup_val(((Lval *)lval)->get_name());
    if(v.type!=Type::VOID){
    if(lval->attribute.T.type==Type::INT){
        int i=exp->attribute.V.val.IntVal;
        v.IntInitVals[0]=i;
        v.type=Type::INT;
    }else if(lval->attribute.T.type==Type::FLOAT){
        float f=exp->attribute.V.val.FloatVal;
        v.FloatInitVals[0]=f;
        v.type=Type::FLOAT;
    }
    semant_table.symbol_table.add_Symbol(((Lval *)lval)->get_name(),v);
   
    }else{
        VarAttribute val=semant_table.GlobalTable[((Lval *)lval)->get_name()];
        if(lval->attribute.T.type==Type::INT){
        int i=exp->attribute.V.val.IntVal;
        val.IntInitVals[0]=i;
        val.type=Type::INT;
        }else if(lval->attribute.T.type==Type::FLOAT){
        float f=exp->attribute.V.val.FloatVal;
        val.FloatInitVals[0]=f;
        val.type=Type::FLOAT;
       }
       semant_table.GlobalTable[((Lval *)lval)->get_name()]=val;
    }
     ((Lval *)lval)->is_left = true;
    if (exp->attribute.T.type == Type::VOID) {
        error_msgs.push_back("void type can not be assign_stmt's expression " + std::to_string(line_number) + "\n");
    } 
}

void expr_stmt::TypeCheck() {
    exp->TypeCheck();
    attribute = exp->attribute;
}

void block_stmt::TypeCheck() { b->TypeCheck(); }

void ifelse_stmt::TypeCheck() {
    iscond=true;
    Cond->TypeCheck();
    iscond=false;
    // if (Cond->attribute.T.type == Type::VOID) {
    //     error_msgs.push_back("if cond type is invalid in line " + std::to_string(line_number) + "\n");
    // }
    ifstmt->TypeCheck();
    elsestmt->TypeCheck();
}

void if_stmt::TypeCheck() {
    iscond=true;
    Cond->TypeCheck();
    iscond=false;
    // if (Cond->attribute.T.type == Type::VOID) {
    //     error_msgs.push_back("if cond type is invalid in line " + std::to_string(line_number) + "\n");
    // }
    ifstmt->TypeCheck();
}

void while_stmt::TypeCheck() { 
    iscond=true;
    Cond->TypeCheck();
    iscond=false;
    // if(Cond->attribute.T.type==Type::VOID){
    //      error_msgs.push_back("while cond type is invalid in line " + std::to_string(line_number) + "\n");
    // }
    hasloop++;
    body->TypeCheck();
    hasloop--;
 }

void continue_stmt::TypeCheck() {  
    if (hasloop==0) {
        error_msgs.push_back("continue is not in while stmt in line " + std::to_string(line_number) + "\n");
    }
 }

void break_stmt::TypeCheck() { 
    if (hasloop==0) {
        error_msgs.push_back("break is not in while stmt in line " + std::to_string(line_number) + "\n");
    } }

void return_stmt::TypeCheck() { 
    isinreturn=true;
    return_exp->TypeCheck(); 
    if(return_exp->attribute.T.type==Type::INT)
    { 
        returnnumi=return_exp->attribute.V.val.IntVal;
       // printf("%d",returnnumi);
        }
    else if(return_exp->attribute.T.type==Type::FLOAT)
    {returnnumf=return_exp->attribute.V.val.FloatVal;}
    isinreturn=false;
    if (return_exp->attribute.T.type == Type::VOID) {
        error_msgs.push_back("return type is invalid in line " + std::to_string(line_number) + "\n");
    }
    
    }

void return_stmt_void::TypeCheck() {}

void ConstInitVal::TypeCheck() { 
    for (auto init : *initval) {
        init->TypeCheck();
    } }

void ConstInitVal_exp::TypeCheck() { 
    if(exp==nullptr){
        return;
    }
    exp->TypeCheck();
    attribute=exp->attribute;
    if (attribute.V.ConstTag==0) {    
        error_msgs.push_back("Expression is not const in line" + std::to_string(line_number) + "\n");
    }
    // if (attribute.T.type == Type::VOID) {
    //     error_msgs.push_back("Initval expression can't be void in line " + std::to_string(line_number) + "\n");
    // }
 }

void VarInitVal::TypeCheck() { 
    for (auto init : *initval) {
        init->TypeCheck();
    } }

void VarInitVal_exp::TypeCheck() { 
     if (exp == nullptr) {
        return;
    }

    exp->TypeCheck();
    attribute = exp->attribute;

    // if (attribute.T.type == Type::VOID) {
    //     error_msgs.push_back("Initval expression can't be void in line " + std::to_string(line_number) + "\n");
    // }
 }

void VarDef_no_init::TypeCheck() { 
    // printf("%d",10);
    // return ;
    //  return;
    if(dims!=nullptr){
     auto dim=*dims;
     for(auto d :dim){
         d->TypeCheck();
         if (d->attribute.T.type == Type::FLOAT) {
                    error_msgs.push_back("Array Dim can not be float in line " + std::to_string(line_number) + "\n");
                }
         if(d->attribute.V.ConstTag==false){
             error_msgs.push_back("Array Dim must be const expression in line " + std::to_string(line_number) +
                                         "\n");
         }     
     }
   } }

void VarDef::TypeCheck() { 
   // printf("%d",10);
    //  return;
    if(dims!=nullptr){
     auto dim=*dims;
     for(auto d :dim){
         d->TypeCheck();
         if (d->attribute.T.type == Type::FLOAT) {
                    error_msgs.push_back("Array Dim can not be float in line " + std::to_string(line_number) + "\n");
                }
         if(d->attribute.V.ConstTag==false){
             error_msgs.push_back("Array Dim must be const expression in line " + std::to_string(line_number) +
                                         "\n");
         }     
     }
   } }

void ConstDef::TypeCheck() { 
    if(dims!=nullptr){
     auto dim=*dims;
     for(auto d :dim){
         d->TypeCheck();
         if (d->attribute.T.type == Type::FLOAT) {
                    error_msgs.push_back("Array Dim can not be float in line " + std::to_string(line_number) + "\n");
                }
         if(d->attribute.V.ConstTag==false){
             error_msgs.push_back("Array Dim must be const expression in line " + std::to_string(line_number) +
                                         "\n");
         }     
     }
     } 
      if(init!=nullptr){
        init->TypeCheck();
      }
   }

void VarDecl::TypeCheck() { //对变量声明时的检查；
    //  printf("%d",10);
    //  return;
   std::vector<Def> defs=*var_def_list;
   for(Def def:defs){
   if(semant_table.symbol_table.lookup_scope(def->get_name())==semant_table.symbol_table.get_current_scope()){
     error_msgs.push_back("previous declaration of '"+def->get_name()->get_string()+ "' was in line"+ std::to_string(line_number) + "\n");
   }
   def->scope=semant_table.symbol_table.get_current_scope();
   
   VarAttribute v;
   
   //对数组进行检查
   if(def->get_dims()!=nullptr){
     auto dims=*def->get_dims();
     for(auto d :dims){
         d->TypeCheck();
         if (d->attribute.T.type == Type::FLOAT) {
                    error_msgs.push_back("Array Dim can not be float in line " + std::to_string(line_number) + "\n");
                }
         if(d->attribute.V.ConstTag==false){
             error_msgs.push_back("Array Dim must be const expression in line " + std::to_string(line_number) +
                                         "\n");
         }
         v.dims.push_back(d->attribute.V.val.IntVal);     
     }
   }
    v.type=type_decl;
    v.ConstTag=false;
    InitVal init = def->get_init();
        if (init != nullptr) {
            init->TypeCheck();
            if(type_decl==Type::INT){
                
               initintconst(init,v);
               
            }else if(type_decl==Type::FLOAT){
              initfloatconst(init,v);
            }
        }else{
            if(type_decl==Type::INT){
                 v.IntInitVals.resize(1, 0);
                 v.IntInitVals[0]=0;
            }else if(type_decl==Type::FLOAT){
                 v.IntInitVals.resize(1, 0);
                v.FloatInitVals[0]=0;
            }
        }
    semant_table.symbol_table.add_Symbol(def->get_name(), v);
   }
   
     }

void ConstDecl::TypeCheck() { 
    auto defs=*var_def_list;
   for(auto def:defs){
   if(semant_table.symbol_table.lookup_scope(def->get_name())==semant_table.symbol_table.get_current_scope()){
     error_msgs.push_back("previous declaration of '"+def->get_name()->get_string()+ "' was in line"+ std::to_string(line_number) + "\n");
   }
   def->scope=semant_table.symbol_table.get_current_scope();
   
   VarAttribute v;
   
   //对数组进行检查
   if(def->get_dims()!=nullptr){
     auto dims=*def->get_dims();
     for(auto d :dims){
         d->TypeCheck();
         if (d->attribute.T.type == Type::FLOAT) {
                    error_msgs.push_back("Array Dim can not be float in line " + std::to_string(line_number) + "\n");
                }
         if(d->attribute.V.ConstTag==false){
             error_msgs.push_back("Array Dim must be const expression in line " + std::to_string(line_number) +
                                         "\n");
         }
         v.dims.push_back(d->attribute.V.val.IntVal);     
     }
   }
    v.type=type_decl;
    v.ConstTag=true;
    InitVal init = def->get_init();
        if (init != nullptr) {
            init->TypeCheck();
            if(type_decl==Type::INT){
               initintconst(init,v);
            }else if(type_decl==Type::FLOAT){
              initfloatconst(init,v);
            }
        }
    semant_table.symbol_table.add_Symbol(def->get_name(), v);
   }
    }

void BlockItem_Decl::TypeCheck() { decl->TypeCheck(); }

void BlockItem_Stmt::TypeCheck() { stmt->TypeCheck(); }

void __Block::TypeCheck() {
    semant_table.symbol_table.enter_scope();
    auto item_vector = *item_list;
    for (auto item : item_vector) {
        item->TypeCheck();
    }
    semant_table.symbol_table.exit_scope();
}

void __FuncFParam::TypeCheck() {
    VarAttribute val;
    val.ConstTag = false;
    val.type = type_decl;
    scope = 1;

    // 如果dims为nullptr, 表示该变量不含数组下标, 如果你在语法分析中采用了其他方式处理，这里也需要更改
    if (dims != nullptr) {    
        auto dim_vector = *dims;

        // the fisrt dim of FuncFParam is empty
        // eg. int f(int A[][30][40])
        val.dims.push_back(-1);    // 这里用-1表示empty，你也可以使用其他方式
        for (int i = 1; i < dim_vector.size(); ++i) {
            auto d = dim_vector[i];
            d->TypeCheck();
            if (d->attribute.V.ConstTag == false) {
                error_msgs.push_back("Array Dim must be const expression in line " + std::to_string(line_number) +
                                     "\n");
            }
            if (d->attribute.T.type == Type::FLOAT) {
                error_msgs.push_back("Array Dim can not be float in line " + std::to_string(line_number) + "\n");
            }
            val.dims.push_back(d->attribute.V.val.IntVal);
        }
        attribute.T.type = Type::PTR;
    } else {
        attribute.T.type = type_decl;
    }

    if (name != nullptr) {
        if (semant_table.symbol_table.lookup_scope(name) != -1) {
            error_msgs.push_back("multiple difinitions of formals in function " + name->get_string() + " in line " +
                                 std::to_string(line_number) + "\n");
        }
        if(val.type == Type::INT){
            val.IntInitVals.resize(1, 1);
        } else if(val.type == Type::FLOAT){
            val.FloatInitVals.resize(1,1.0f);
        }
        semant_table.symbol_table.add_Symbol(name, val);
    }
}

void __FuncDef::TypeCheck() {
    returnnumf=1.0f;
    returnnumi=1;
    semant_table.symbol_table.enter_scope();

    semant_table.FunctionTable[name] = this;
     //printf("%d",10);
     //return ;
    if(name->get_string()=="main"){
        ishasmain=true;
    }
    auto formal_vector = *formals;
    for (auto formal : formal_vector) {
        formal->TypeCheck();
    }

    // block TypeCheck
    isinfunc=true;
    if (block != nullptr) {
        auto item_vector = *(block->item_list);
        for (auto item : item_vector) {
            // printf("%d",10);
            // return;
            item->TypeCheck();
        }
    }
    if(returnnumi==0&&return_type==Type::INT){
        this->returniszero=true;
        this->num.returnmuni=returnnumi;
        semant_table.FunctionTable[name] = this;
    }else if(returnnumf==0.0&&return_type==Type::FLOAT){
         this->returniszero=true;
         this->num.returnmunf=returnnumf;
        semant_table.FunctionTable[name] = this;
    }else if(return_type==Type::INT){
         this->num.returnmuni=returnnumi;
        semant_table.FunctionTable[name] = this;
    }else if(return_type==Type::FLOAT){
        this->num.returnmunf=returnnumf;
        semant_table.FunctionTable[name] = this;
    }

    isinfunc=false;
    semant_table.symbol_table.exit_scope();
}

std::map<std::string, VarAttribute> ConstGlobalMap;
std::map<std::string, VarAttribute> StaticGlobalMap;    
BasicInstruction::LLVMType Type2LLvm[6] = {
    BasicInstruction::LLVMType::VOID, 
    BasicInstruction::LLVMType::I32, 
    BasicInstruction::LLVMType::FLOAT32,
    BasicInstruction::LLVMType::I1,
    BasicInstruction::LLVMType::PTR,
    BasicInstruction::LLVMType::DOUBLE
};

void CompUnit_Decl::TypeCheck() {     
    Type::ty type_decl = decl->GetTypedecl();
    auto def_list = *decl->GetDefs();
    for (auto def : def_list) {
        if (semant_table.GlobalTable.find(def->get_name()) != semant_table.GlobalTable.end()) {
            error_msgs.push_back("Multiple definitions of variable in line " + std::to_string(line_number) + "\n");
        }

        VarAttribute val;
        val.ConstTag = def->IsConst();
        val.type = (Type::ty)type_decl;
        def->scope = 0;

        if (def->get_dims() != nullptr) {
            auto def_list = *def->get_dims();
            for (auto d : def_list) {
                d->TypeCheck();
                if (!d->attribute.V.ConstTag) {
                    error_msgs.push_back("Array Dim must be const expression " + std::to_string(line_number) + "\n");
                }
                if (d->attribute.T.type == Type::FLOAT) {
                    error_msgs.push_back("Array Dim can not be float in line " + std::to_string(line_number) + "\n");
                }
            }
            for (auto d : def_list) {
                val.dims.push_back(d->attribute.V.val.IntVal);
            }
        }

         InitVal init = def->get_init();
        if (init != nullptr) {
             init->TypeCheck();
             if (type_decl == Type::INT) {
                 initintconst(init, val);
            } else if (type_decl == Type::FLOAT) {
                initfloatconst(init, val);
             }
        }else {
             if(type_decl==Type::INT){
                 val.IntInitVals.resize(1, 0);
                 val.IntInitVals[0]=0;
            }else if(type_decl==Type::FLOAT){
                 val.IntInitVals.resize(1, 0);
                 val.FloatInitVals[0]=0.0;
            }
        }

        if (def->IsConst()) {
            ConstGlobalMap[def->get_name()->get_string()] = val;
        }

        StaticGlobalMap[def->get_name()->get_string()] = val;
        semant_table.GlobalTable[def->get_name()] = val;

        BasicInstruction::LLVMType lltype = Type2LLvm[type_decl];

        Instruction globalDecl;
        if (def->get_dims() != nullptr) {
            globalDecl = new GlobalVarDefineInstruction(def->get_name()->get_string(), lltype, val);
        } else if (init == nullptr) {
            globalDecl = new GlobalVarDefineInstruction(def->get_name()->get_string(), lltype, nullptr);
        } else if (lltype == BasicInstruction::LLVMType::I32) {
            globalDecl =
            new GlobalVarDefineInstruction(def->get_name()->get_string(), lltype, new ImmI32Operand(val.IntInitVals[0]));
        } else if (lltype == BasicInstruction::LLVMType::FLOAT32) {
            globalDecl = new GlobalVarDefineInstruction(def->get_name()->get_string(), lltype,
                                                        new ImmF32Operand(val.FloatInitVals[0]));
        }
        llvmIR.global_def.push_back(globalDecl);
    }
}

void CompUnit_FuncDef::TypeCheck() { func_def->TypeCheck(); }