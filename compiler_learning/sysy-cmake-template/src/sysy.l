// 这三行都是Flex工具的选项，它们用于指定FLex工具在生成词法分析器的一些设置，具体含义
// 告诉flex不要使用默认的yywrp参数，yywrap用于在处理完当前输入流后继续处理下一个输入流，一下方式不需要处理多个输入流
%option noyywrap
// 不使用默认的yyunput函数，yyunput函数用于将一个字符插入到输入流中，如果不需要在词法分析器进行回溯，可以禁掉yyinput函数
%option nounput
// 告诉Flex不要包含默认的yyinput函数，yyinput函数用于从输入流中读取字符，不需要自定义输入方式，
%option noinput


%{
// 这里是全局的代码，因为最后要生成C/C++文件，实现主要逻辑的部分是用C/C++写的
// 难免会用到头文件，所以通常头文件和一些全局声明/定义写在这里 生成的文件包含的头文件 塞到FLex里面
#include <cstdlib>
#include <string>

// 因为 Flex 会用到 Bison 中关于 token 的定义
// 所以需要 include Bison 生成的头文件
// 语法生成器的头文件 因为二者内容互相依赖，需要相互调用
#include "sysy.tab.hpp"

using namespace std;

%}

// 这里
/* 编写词法分析器，使用正则表达式定义词法规则，易识别不同的词法单元 */
/* 空白符和注释 匹配0个或多个空格，制表符，换行符或回车符号*/
/* 识别单行注释 “//” 匹配双斜杠， .* 匹配任意字符任意次数，知道遇到行尾符号 */
/* 可以写在第四部分，但是那样太乱了，还是起个名字比较好理解一些
WhiteSpace    [ \t\n\r]*
LineComment   "//".*

/* 标识符为 匹配任何一个字母或下划线， 匹配任何个数的字母、数字或下划线*/
Identifier    [a-zA-Z_][a-zA-Z0-9_]*

/* 运算符 */
Operator      [+\-*/=<>!&|^%]

/* 整数字面量 */
Decimal       [1-9][0-9]*
Octal         0[0-7]*
Hexadecimal   0[xX][0-9a-fA-F]+
%%

/* 扫描到某个token后做的操作 */
{WhiteSpace}    { /* 忽略, 不做任何操作 */ }
{LineComment}   { /* 忽略, 不做任何操作 */ }

/* 遇到关键字返回token */
"int"           { return INT; }
"return"        { return RETURN; }

/* yytext 代表词法分析器匹配到的字符串内容，类型是char* yylval用来向parser传递lexer读取到的内容 */
/* str_val 和 int_val 是我们在Bison文件中定义的字段 */
{Identifier}    { yylval.str_val = new string(yytext); return IDENT; }
{Operator}      { yylval.str_val = new string(yytext); return OPERATOR; }
/* strtol c库函数，用来将字符串转化为长整数型数， nullptr用于存储转换结束的位置，不需要传入NULL，base 进制值，0根据字符串前缀自动判断进制“ */
// 这些INT RETURN 是稷山个就是BISON 更具我们的定义生成的枚举 就是TOKEN，
{Decimal}       { yylval.int_val = strtol(yytext, nullptr, 0); return INT_CONST; }
{Octal}         { yylval.int_val = strtol(yytext, nullptr, 0); return INT_CONST; }
{Hexadecimal}   { yylval.int_val = strtol(yytext, nullptr, 0); return INT_CONST; }

.               { return yytext[0]; }

%%


// 这里写一些用户自定义的代码 比如你希望在生成的c/c++文件里定义一个函数，做一些辅助工作，
// 你希望在之前的规则描述里调用你定义的函数，那么你可以把c/c++的函数定义在这里，声明写在文件开头
