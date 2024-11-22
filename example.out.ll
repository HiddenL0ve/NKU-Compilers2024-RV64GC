Program
  FuncDef   Name:gcd   ReturnType: Int
    FuncFParam   name:m   Type:Int   scope:1

    FuncFParam   name:n   Type:Int   scope:1

    Block   Size:2
      VarDecls   Type: Int
        VarDef   name:r   scope:1
          init:
            VarInitVal_exp
              AddExp_plus: (+)   Type: Int   
                Lval   Type: Int      name:m   scope:1
                Lval   Type: Int      name:n   scope:1
      ReturnStmt:
        Lval   Type: Int      name:r   scope:1
  FuncDef   Name:main   ReturnType: Int
    Block   Size:1
      ReturnStmt:
        Intconst   val:0   Type: Int   ConstValue: 0
