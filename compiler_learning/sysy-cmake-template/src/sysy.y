// 设置一些必要的选项 塞到bison生成的文件当中去
%code requires {
  #include <memory>
  #include <string>
  #include <ast.h>
  #include <cstring>
}

// 引用头文件
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

// 定义parser函数的参数，Bison生成的parser函数返回类型一定是int，所以我们呢没办法通过返回值返回AST，只能通过参数来返回AST
// 手动修改
%parse-param { std::unique_ptr<BaseAST> &ast }

// 定义yylval， 定义为一个联合体
// 因为token的值有的是字符串指针，有的是整数
// lexer用到的str_val,int_val就是在这里被定义的
// 为什么用字符串指针而不是string 或者 unique_ptr<string>
// 在union里面写一个带析构函数的类时，如果使用该类创建对象并在该对象上调用析构函数，
// 调用析构函数可能会影响其他成员，union是一种特殊类型的数据类型，它的内存布局与其中最大的成员相关，其大小等于最大成员的代销
// 当调用union中一个成员的析构函数时，由于所有成员共用一块内存，可能会影响其他成员的值，
// 而且由于共享义块内存，析构函数无法正确释放资源，可能会导致内存泄漏或者文件未能正确关闭
%union {
  std::string *str_val;
  char char_val;
  int int_val;
  BaseAST *ast_val;
}

// lexer 返回的所有 token 种类的声明
// 注意 IDENT 和 INT_CONST 会返回 token 的值, 分别对应 str_val 和 int_val
%token INT RETURN
%token <str_val> IDENT
%token <int_val> INT_CONST
%token <char_val> UnaryOp

// 非终结符的类型定义
%type <ast_val> FuncDef FuncType Block Stmt Number Exp PrimaryExp UnaryExp

%%
// 定义语法规则，同时定义了parser解析完语法规则之后的操作
// $$表示非终结符的语法规则，符号赋值返回结果
// 为什么用unique_ptr<string>  先解引用，
// string分配了一个动态分配的字符数组，，可以方便地进行字符串操作，例如拼接查找替换，当生命周期结束时，会自动调用析构
// 函数释放内存，unique_ptr是一个智能指针类型，可以自动管理动态分配的内存，当unique对象生命周期结束时，会自动调用
// 析构函数释放内存，unique_ptr使用RAII资源获取即初始化的技术，有效避免内存泄漏和多次释放同一块内存的问题， 是指针
// 通过解引用来访问其所指向的字符串， 析构师只想控对象会导致程序崩溃
// union 里面的成员变量共享同一段内存空间，当修改其中一个值时，需要注意其他成员变量的值可能会发生变化
CompUnit
  : FuncDef {
    // 返回unique_ptr<T>
    auto comp_unit = make_unique<CompUnitAST>();
    comp_unit->func_def = unique_ptr<BaseAST>($1);
    // 因为每个unique_ptr都是一种智能指针，用于管理动态内存的分配, 当需要转移一个对象用move
    ast = move(comp_unit);
  }
  ;

FuncDef
  : FuncType IDENT '(' ')' Block {
    auto ast = new FuncDefAST();
    ast->func_type = unique_ptr<BaseAST>($1);
    ast->ident = *unique_ptr<string>($2); // 就是访问对象 string &
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
  : '(' Exp ')' {
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
    ast->unaryop = $1;
    ast->unaryexp = unique_ptr<BaseAST>($2);
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
  cerr << "error: " << s << endl;
}
