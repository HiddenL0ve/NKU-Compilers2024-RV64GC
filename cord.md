## 1 
```c
void Lval::codeIR() { 
    LLVMBlock b=llvmIR.GetBlock(now_function,now_label);
    std::vector<Operand> indexs;//使用时用到的数组维度 例如：定义了 a[10][20]但此时使用为a[3][5]此时将会把3、5转换为对应的索引
    if(dims!=nullptr){
        for(auto d:*dims){
            d->codeIR();
            IRgenTypeConverse(b,d->attribute.T.type,Type::INT,regnumber);
            indexs.push_back(GetNewRegOperand(regnumber));//真实使用的时候用到的数组的偏移
        }
    }
    
    Operand ptr_operand;
    VarAttribute lval_attribute;
    bool formal_array_tag=false;
    int alloca_reg=irgen_table.symbol_table.lookup(name);
    if(alloca_reg==-1){// 返回-1证明不在symbol_table中，为全局变量
        lval_attribute=semant_table.GlobalTable[name];
        ptr_operand=GetNewGlobalOperand(name->get_string());
    }else{//局部变量
       ptr_operand=GetNewRegOperand(alloca_reg);//对于a=5这个例子，该条语句在构建%a这个操作数，或者存在的话直接返回;通过指针寄存器的值分配对应的操作数
       lval_attribute=irgen_table.RegTable[alloca_reg];
       formal_array_tag=irgen_table.FormalArrayTable[alloca_reg];//用于判断是否为函数参数
    }
/*   
      int a[3][3];
      int value = a[1][2]; 

     %a = alloca [3 x [3 x i32]]   ; 分配一个 3x3 的二维数组，类型为 [3 x [3 x i32]]
      %ptr = getelementptr inbounds [3 x [3 x i32]], [3 x [3 x i32]]* %a, i32 1, i32 2  ; 获取 a[1][2] 的地址
      %val = load i32, i32* %ptr     ; 加载 a[1][2] 的值到 %val 中
*/

//下述代码相当于对a[1][2]的处理
    auto lltype=TLLvm[lval_attribute.type];
    if(indexs.empty()==false||attribute.T.type==Type::PTR){//对于数组
        if(formal_array_tag){//非函数参数
             indexs.insert(indexs.begin(),new ImmI32Operand(0));
        }
        if(lltype==BasicInstruction::LLVMType::I32)
            IRgenGetElementptrIndexI32(b,lltype,++regnumber,ptr_operand,lval_attribute.dims,indexs);
        else if(lltype==BasicInstruction::LLVMType::I64){
            IRgenGetElementptrIndexI64(b,lltype,++regnumber,ptr_operand,lval_attribute.dims,indexs);
        }
        ptr_operand=GetNewRegOperand(regnumber);
    }
    ptr=ptr_operand;
    if(is_left==false){//右值
        if(attribute.T.type!=Type::PTR){
            IRgenLoad(b,lltype,regnumber,ptr_operand);
        }
    }

 }
```
- 其中 ```if(formal_array_tag){//非函数参数
             indexs.insert(indexs.begin(),new ImmI32Operand(0));
        }``` 做出修改


## 对于fun_call的处理：

- 在func_def的树中增加bool retruniszero=false；
- 在 func_def中增加returnnumi以及retrunnumf分别来存储返回值为int或者float的值，通过判断其是否为0，来设置returniszero变量；
- 对于returnnumi以及returnnumf的获取，在return_stmt函数中通过return 后面表达式的intval或者floatval值来设置上述两个变量，这样就可以覆盖直接为一个intconst或floatconst或者lval的情况；

## 目前类型检查存在问题：
- 对于if以及while这种语句其中的cond进行检查，当检查不规范时，对于行号的输出存在一定问题，原因：在.y文件进行while以及if语句规约，记录行号时，记录的是整个语句的行号，导致最终的行号大小为最后的“}”的行号
- 双目运算符缺少对于void类型的检查判断