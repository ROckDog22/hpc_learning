%code requires {
  #include <memory>
  #include <string>
  #include <ast.h>
  #include <cstring>
}

%{

#include <iostream>
#include <memory>
#include <string>
#include <ast.h>
// 声明 lexer 函数和错误处理函数
int yylex();
void yyerror(std::unique_ptr<BaseAST> &ast, const char *s);

using namespace std;

%}


%parse-param { std::unique_ptr<BaseAST> &ast }

%union {
  std::string *str_val;
  int int_val;
  BaseAST *ast_val;
}

// lexer 返回的所有 token 种类的声明
// 注意 IDENT 和 INT_CONST 会返回 token 的值, 分别对应 str_val 和 int_val
%token INT RETURN
%token <str_val> IDENT OPERATOR
%token <int_val> INT_CONST

// 非终结符的类型定义
%type <ast_val> FuncDef FuncType Block Stmt Number Exp PrimaryExp UnaryExp UnaryOp

%%

CompUnit
  : FuncDef {
    auto comp_unit = make_unique<CompUnitAST>();
    comp_unit->func_def = unique_ptr<BaseAST>($1);
    ast = move(comp_unit);
  }
  ;

FuncDef
  : FuncType IDENT '(' ')' Block {
    auto ast = new FuncDefAST();
    ast->func_type = unique_ptr<BaseAST>($1);
    ast->ident = *unique_ptr<string>($2);
    ast->block = unique_ptr<BaseAST>($5);
    $$ = ast;
  }
  ;

// 同上, 不再解释
FuncType
  : INT {
    auto ast = new FuncTypeAST();
    $$ = ast;
  }
  ;

Block
  : '{' Stmt '}' {
    auto ast = new BlockAST();
    ast->stmt = unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  ;

Stmt
  : RETURN Exp ';' {
    auto ast = new StmtAST();
    ast->exp = unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  ;

Exp
  : UnaryExp {
    auto ast = new ExpAST();
    ast->unaryexp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  ;

PrimaryExp
  : "(" Exp ")" {
    auto ast = new PrimaryExpAST();
    ast->flag = 1;
    ast->exp = unique_ptr<BaseAST>($2);
    $$ = ast;
  } | Number{
    auto ast = new PrimaryExpAST();
    ast->flag = 2;
    ast->number = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  ;

UnaryExp
  : PrimaryExp {
    auto ast = new UnaryExpAST();
    ast->flag = 1;
    ast->primaryexp = unique_ptr<BaseAST>($1);
    $$ = ast;
  } | UnaryOp UnaryExp {
    auto ast = new UnaryExpAST();
    ast->flag = 2;
    ast->unaryop = unique_ptr<BaseAST>($1);
    ast->unaryexp = unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  ;

UnaryOp
  : OPERATOR {
    auto ast = new UnaryOpAst();
    ast->val = *unique_ptr<string>($1);
    $$ = ast;
  }
  ;

Number
  : INT_CONST {
    auto ast = new NumberAST();
    ast->val = $1;
    $$ = ast;
  }
  ;

%%

// 定义错误处理函数, 其中第二个参数是错误信息
// parser 如果发生错误 (例如输入的程序出现了语法错误), 就会调用这个函数
void yyerror(unique_ptr<BaseAST> &ast, const char *s) {
  
  extern int yylineno;
  extern char *yytext;
  int len = strlen(yytext);
  int i;
  char buf[512] = {0};
  for (i = 0; i < len; i++) {
    sprintf(buf,"%s%d ",buf,yytext[i]);
  }
    fprintf(stderr, "ERROR: %s at symbol '%s' on line %d\n", s, buf, yylineno);
}
