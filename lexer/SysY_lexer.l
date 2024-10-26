%option noyywrap
%{
#include <cmath>
#include <vector>
#include <cstring>
#include<fstream>
#include "../include/symtab.h"
#include "../parser/SysY_parser.tab.h"

extern int line_number;
extern int col_number;
extern int cur_col_number;
extern IdTable id_table;

std::vector<char> string_content;
void yyerror(char* s, ...);
%}

commentbegin1 "/*"
comment "//".*
commentelement1 .
commentelement2 \n
commentend "*/"
%x COMMENT


%%
    /* TODO():增加处理注释的代码*/
{comment} {
       cur_col_number=col_number;
       col_number+=strlen(yytext); 
 }
{commentbegin1} {
       cur_col_number=col_number;
       col_number+=strlen(yytext);
       BEGIN COMMENT;}

"*/" {
    cur_col_number=col_number;
    col_number+=strlen(yytext);
    char a[100]="Compilation Error: Unmatched end of unmatched */";
    yylval.error_msg = a;
    //printf("%s\n", yylval.error_msg);
    return ERROR;
}
<COMMENT>{commentelement2} {
        line_number+=1;
        col_number=0;
  }
<COMMENT>{commentend} {BEGIN INITIAL;}
<COMMENT>{commentelement1} {
        cur_col_number=col_number;
        col_number+=strlen(yytext);
        }


    /* TODO():增加处理列号的代码(cur_col_number表示当前token开始位置, col_number表示当前token结束位置) */                    
"<=" {cur_col_number = col_number;col_number += strlen(yytext);return LEQ;}         
">=" {cur_col_number = col_number;col_number += strlen(yytext);return GEQ;}         
"==" {cur_col_number = col_number;col_number += strlen(yytext);return EQ;}        
"!=" {cur_col_number = col_number;col_number += strlen(yytext);return NE;}
"&&" {cur_col_number = col_number;col_number += strlen(yytext);return AND;}         
"||" {cur_col_number = col_number;col_number += strlen(yytext);return OR;}               
"const" {cur_col_number = col_number;col_number += strlen(yytext);return CONST;}     
"if" {cur_col_number = col_number;col_number += strlen(yytext);return IF;}       
"else" {cur_col_number = col_number;col_number += strlen(yytext);return ELSE;}  
"while" {cur_col_number = col_number;col_number += strlen(yytext);return WHILE;}
"void" {cur_col_number = col_number;col_number += strlen(yytext);return NONE_TYPE;}    
"int" {cur_col_number = col_number;col_number += strlen(yytext);return INT;}    
"float" {cur_col_number = col_number;col_number += strlen(yytext);return FLOAT;}      
"return" {cur_col_number = col_number;col_number += strlen(yytext);return RETURN;}    
"break" {cur_col_number = col_number;col_number += strlen(yytext);return BREAK;}     
"continue" {cur_col_number = col_number;col_number += strlen(yytext);return CONTINUE;}

\n {++line_number;col_number = 0;}

[ \t\f\r\v] {col_number += strlen(yytext);}

[\.\+\-\*\/\=\<\!\%\>] {cur_col_number = col_number;col_number += strlen(yytext);return yytext[0];}

[\{\}\;\(\)\,\[\]] {cur_col_number = col_number;col_number += strlen(yytext);return yytext[0];}

[_a-zA-Z][_a-zA-Z0-9]* {  //标识符
    cur_col_number = col_number;
    col_number += strlen(yytext);
    yylval.symbol_token = id_table.add_id(std::string(yytext));
    return IDENT;
}

(([1-9][0-9]*)|0)[eE][\+\-]?[0-9]+ {   //十进制整数科学计数法
     cur_col_number = col_number;
    col_number += strlen(yytext);
    // yylval.float_token = stoi(std::string(yytext));
    yylval.float_token = 0;
    int cur=0;

    int zhengfu = 1;
    if(yytext[cur]=='+' || yytext[cur]=='-'){
        if(yytext[cur]=='-'){
            zhengfu = -1;
        }
        cur++;
    }

    for(int i = cur;yytext[i] != 'e' && yytext[i] != 'E' && yytext[i];i++){
        yylval.float_token *= 10;
        yylval.float_token += yytext[i] - '0';
        cur=i;
    }
     int signal = 1;
     cur++;
    if(yytext[cur]){
        cur++;
        if(yytext[cur] == '-'){
            signal= -1;
            cur++;
        }
        else if(yytext[cur] == '+'){
            signal= 1;
            cur++;
        }

        double exp = 0;
        for(int i=cur; yytext[i]; i++){
            exp *= 10;
            exp += yytext[i] - '0';
        }
        exp *= signal;
        yylval.float_token *= pow(10.0,exp);
    }
    yylval.float_token*=zhengfu;
    return FLOAT_CONST;
}



([1-9][0-9]*)|0 {    //带正负号的十进制整数
    cur_col_number = col_number;
    col_number += strlen(yytext);
    // yylval.int_token = stoi(std::string(yytext));
    int signal=1;
    long num=0;
    if(yytext[0]=='-'||yytext[0]=='+'){
        if(yytext[0]=='-')
          signal=-1;
    for(int i = 1;yytext[i];i++){
        num *= 10;
       num += yytext[i] - '0';
    }   
      num*=signal;
    }else{
    num = 0;
    for(int i = 0;yytext[i];i++){
        num *= 10;
        num += yytext[i] - '0';
    }
    }
        yylval.int_token=num;
        return INT_CONST;
   
}

    /*
      在词法分析中，你只需要考虑yylval的三种类型，分别为int_token, float_token, symbol_token
      之所以要定义这三种类型，是因为我们只返回这个token的类型是不够的，我们还需要知道这个token的属性
      例如我们需要知道INT_CONST具体的值是多少
    */
    /*TODO():参考SysY2022定义, 处理更多的词法, 如浮点数，十六进制数等*/

(([0-9]+\.[0-9]*)|(\.[0-9]+))([eE][\+\-]?[0-9]+)? {  //十进制浮点数+科学计数法
    cur_col_number = col_number;
    col_number += strlen(yytext);
    yylval.float_token = 0;
    int cur=0;

    int zhengfu = 1;
    if(yytext[cur]=='+' || yytext[cur]=='-'){
        if(yytext[cur]=='-'){
            zhengfu = -1;
        }
        cur++;
    }
    for(int i=cur; yytext[i] != '.' && yytext[i]; i++){
        yylval.float_token *= 10;
        yylval.float_token += yytext[i] - '0';
        cur = i;
    }
    if(yytext[cur]=='.'){
        cur+=1;
    }else{
    cur += 2;}
    double t = 0.1;
    for(int i=cur; yytext[i] != 'e' && yytext[i] != 'E' && yytext[i]; i++){
        yylval.float_token += (yytext[i] - '0') * t;
        t /= 10;
        cur++;
    }
    int signal = 1;
    if(yytext[cur]){
        cur++;
        if(yytext[cur] == '-'){
            signal= -1;
            cur++;
        }
        else if(yytext[cur] == '+'){
            signal= 1;
            cur++;
        }

        double exp = 0;
        for(int i=cur; yytext[i]; i++){
            exp *= 10;
            exp += yytext[i] - '0';
        }
        exp *= signal;
        yylval.float_token *= pow(10.0,exp);
    }
    yylval.float_token*=zhengfu;
    return FLOAT_CONST;
}




"0"[xX](([0-9a-fA-F]+\.[0-9a-fA-F]*)|(\.[0-9a-fA-F]+))([pP][\+\-]?[0-9]+)? {  //十六进制浮点数(+科学计数法)
    cur_col_number = col_number;
    col_number += strlen(yytext);
    yylval.float_token = 0;
    int cur=0;

    int zhengfu = 1;
    if(yytext[cur]=='+' || yytext[cur]=='-'){
        if(yytext[cur]=='-'){
            zhengfu = -1;
        }
        cur++;
    }

    for(int i=cur; yytext[i] != '.' && yytext[i]; i++){
        yylval.float_token *= 16;
        if(yytext[i]>='0' && yytext[i]<='9'){
            yylval.float_token += yytext[i] - '0';
        }
        else if(yytext[i]>='a' && yytext[i]<='f'){
            yylval.float_token += yytext[i] - 'a' + 10;
        }
        else if(yytext[i]>='A' && yytext[i]<='F'){
            yylval.float_token += yytext[i] - 'A' + 10;
        } 
        cur = i;
    }

    if(yytext[cur]=='.'){
        cur+=1;
    }else{
    cur += 2;}
    double t = pow(16.0,-1);

    for(int i=cur; yytext[i] != 'p' && yytext[i] != 'P' && yytext[i]; i++){
        int tem;
        if(yytext[i]>='0' && yytext[i]<='9'){
            tem = yytext[i] - '0';
        }
        else if(yytext[i]>='a' && yytext[i]<='f'){
            tem = yytext[i] - 'a' + 10;
        }
        else if(yytext[i]>='A' && yytext[i]<='F'){
           tem = yytext[i] - 'A' + 10;
        } 
        yylval.float_token += tem * t;
        t /= 16;
        cur++;
    }

    int signal = 1;
    if(yytext[cur]){
        cur++;
        if(yytext[cur] == '-'){
            signal= -1;
            cur++;
        }
        else if(yytext[cur] == '+'){
            signal= 1;
            cur++;
        }

        double exp = 0;
        for(int i=cur; yytext[i]; i++){
            exp *= 2;
            exp += yytext[i] - '0';
        }
        exp *= signal;
        yylval.float_token *= pow(2.0,exp);
    }
    yylval.float_token*=zhengfu;
    return FLOAT_CONST;
}


"0"[xX][0-9a-fA-F]+[pP][\+\-]?[0-9]+ {//十六进制整数+科学计数法
    cur_col_number = col_number;
    col_number += strlen(yytext);
    yylval.float_token = 0;

    int cur=0;

    int zhengfu = 1;
    if(yytext[cur]=='+' || yytext[cur]=='-'){
        if(yytext[cur]=='-'){
            zhengfu = -1;
        }
        cur++;
    }

    for(int i = 2; yytext[i] != 'p' && yytext[i] != 'P' && yytext[i]; i++){
        yylval.float_token *= 16;
        if(yytext[i]>='0' && yytext[i]<='9'){
            yylval.float_token += yytext[i] - '0';
        }
        else if(yytext[i]>='a' && yytext[i]<='f'){
            yylval.float_token += yytext[i] - 'a' + 10;
        }
        else if(yytext[i]>='A' && yytext[i]<='F'){
            yylval.float_token += yytext[i] - 'A' + 10;
        } 
        cur = i;
    }

    cur++;
    int signal = 1;
    if(yytext[cur]){
        cur++;
        if(yytext[cur] == '-'){
            signal= -1;
            cur++;
        }
        else if(yytext[cur] == '+'){
            signal= 1;
            cur++;
        }

        double exp = 0;
        for(int i=cur; yytext[i]; i++){
            exp *= 2;
            exp += yytext[i] - '0';
        }
        exp *= signal;
        yylval.float_token *= pow(2.0,exp);
    }
    yylval.float_token*=zhengfu;
    return FLOAT_CONST;
}

"0"[xX][0-9a-fA-F]+ {   //十六进制整数
    cur_col_number = col_number;
    col_number += strlen(yytext);
    long num=0;
    int cur=2;

    int zhengfu = 1;
    if(yytext[0]=='+' || yytext[0]=='-'){
        if(yytext[0]=='-'){
            zhengfu = -1;
        }
        cur++;
    }
    for(int i = cur; yytext[i]; i++){
        num *= 16;
        if(yytext[i]>='0' && yytext[i]<='9'){
            num += yytext[i] - '0';
        }
        else if(yytext[i]>='a' && yytext[i]<='f'){
            num += yytext[i] - 'a' + 10;
        }
        else if(yytext[i]>='A' && yytext[i]<='F'){
            num += yytext[i] - 'A' + 10;
        } 
    }
    num*=zhengfu;
    yylval.int_token=num;
    return INT_CONST;

}

"0"[0-7]+ {   //八进制整数
    cur_col_number = col_number;
    col_number += strlen(yytext);
    long num=0;
    int cur=1;

    int zhengfu = 1;
    if(yytext[cur-1]=='+' || yytext[cur-1]=='-'){
        if(yytext[cur-1]=='-'){
            zhengfu = -1;
        }
        cur++;
    }
    for (int i = cur; yytext[i]; i++) {
        num *= 8;
        num += (yytext[i] - '0');
    }
    num*=zhengfu;
     yylval.int_token=num;
     return INT_CONST;

}

    /*unknown tokens, return ERROR*/
. {
    cur_col_number = col_number;
    col_number += strlen(yytext);
    yylval.error_msg = yytext;
    return ERROR;
}
%%