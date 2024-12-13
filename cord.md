
## VarDecl
### int a=2;
 - 先为a分配一个寄存器，放入符号表以及regtable中，通过oprand获得a的操作数
 - 有初始化：
    - 调用2->codeIR();数据类型转换；
    - 其返回的结果存在相应regnumber中，获取操作数
    - 进行store操作，将右边的初始化操作数store左边的值即a
-  无初始化：
    - 分为int和float型，得到一个初始化为0或者0f的操作数
    - 进行store操作
### int a[2][3]={1，2....}
- 首先是将a加入到符号表中
- 遍历a的每一个dims，将其push进入val中
- IRgenAllocaArray 生成 %r0 = alloca [4 x [5 x i32]]
- 将val加入regtable中
- 遍历init表，对位进行数组初始化同时调用CallInstruction进行内存分配
- 对于数组初始化：
     - 对{1，2，....}遍历调用1->codeir();类型不匹配则进行类型转换
     - 获取初始化的数组对应元素的位置 GetElementptrInstruction 插入对应的索引值
   - 获得位置的操作数，以及初始化数值的操作数，进行store操作


## ConstDecl
  - 与Vardecl相同，只是其一定会有初始化列表

## FunDef
### int f (int a[])
- 进入新的作用域
- new FunctionDefineInstruction 获取一个对于函数定义最开始的指令
- 放入 llvmIR.NewFunction
- 初始化寄存器和符号表：label从0开始，irgen_table.RegTable.clear();irgen_table.FormalArrayTable.clear();
- 得到第一个block块（0号块）
- 进行函数形参的处理：
     - 遍历函数形参列表
       - 正常形参
            - 类型插入newFuncDef->InsertFormal(lltype);
            - alloca分配寄存器，获取操作数
            - store进行形参的传入  
            - define i32 @f(i32 %r0)//其中的i32 %r0是在FunctionDefineInstruction中的printir()通过遍历插入的参数列表一起输出出来的
              {
                 L0:  ;
                      %r1 = alloca i32
                      store i32 %r0, ptr %r1
                       br label %L1
               }
            - 加入符号表以及regtable表
       - 数组  
            - 类型插入为指针类型 newFuncDef->InsertFormal(BasicInstruction::LLVMType::PTR);
            - 变量每一维度，将每一维dims加入相应的val.dims中
            - 加入符号表，regtable表，FormalArrayTable表
- IRgenBRUnCond(B, 1)无条件跳转到1号lable
- 获得下一块，更新lable值
- 调用block->codeIR();
- 退出新的作用域

## Lval
- 分配基本块
- 初始化相关变量，操作数，变量属性
- 从符号表symbol_table查找变量名name返回寄存器编号，区分全局（-1）和局部变量
    - 局部
        - 获取局部变量的寄存器操作数
        - 保存寄存器编号对应的变量属性
        - 检查是否为函数参数
    - 全局
        - 从全局表中获取变量属性
        - 生成全局变量操作数
- 处理数组元素
    - index记录数组下标，同时类型转换？
    - 对于局部数组，添加基地址偏移
    - IRgenGetElementptrIndexI32：生成 getelementptr 指令，计算数组元素的地址
    - 更新 ptr_operand 为新计算的地址
- 处理右值
    - 生成 load 指令，将值加载到寄存器
## func_call:
- FunctionTable表中获得函数返回类型
- 有函数参数的调用：
   - 在FunctionTable中获得获取形参，在该节点中获取实参
   - 遍历实参：
           - 调用cur_param->codeIR()进行实参代码输出; 
           - 与形参类型不匹配则进行类型转换
           -  args_vec.push_back({TLLvm[cur_fparam->attribute.T.type], GetNewRegOperand(regnumber)});将对用的实参类型及操作数放入args_vec中
   - 如果返回类型为void 则 IRgenCallVoid(B, llvm_type, args_vec, name->get_string());
   - 返回类型不是void  IRgenCall(B, llvm_type, regnumber, args_vec, name->get_string()); regnumber中记录返回值
- 无函数参数的调用：
    - 返回值为void:IRgenCallVoidNoArgs 
    - 有回值：IRgenCallNoArgs(B, llvm_type, regnumber, name->get_string());其中regnumber中存储返回的值
  