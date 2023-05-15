// Copyright (c) 2023 PaddlePaddle Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <cassert> // 断言语句
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include "ast.h"
#include "koopa.h"
using namespace std;

// 声明 lexer 的输入, 以及 parser 函数
// 为什么不引用 sysy.tab.hpp 呢? 因为首先里面没有 yyin 的定义
// 其次, 因为这个文件不是我们自己写的, 而是被 Bison 生成出来的
// 你的代码编辑器/IDE 很可能找不到这个文件, 然后会给你报错 (虽然编译不会出错)
// 看起来会很烦人, 于是干脆采用这种看起来 dirty 但实际很有效的手段

// extern C的外部变量声明，它告诉编译器yyin是一个指向FILE类型的指针变量，但是yyin的定义不在当前编译单元中，而在其他编译单元中，因此需要用extern关键字来引用
// 在编写编译器或者解释器时，通常需要读取源代码文件进行分析和处理，yyin就是这样的一个指针变量，它指向了当前正在处理的源代码文件
// yyin通常与词法分析器lexer和语法分析器parser配合使用，词法分析器会从yyin指向的文件中读取字符并进行词法分析，将分析结果传递给语法分析器进行语法分析和代码生成。
extern FILE *yyin;
// 是c++函数声明，它指定了一个名为yyparse的函数，该函数接受一个unique_ptr<BaseAST>类型的引用作为参数，并返回一个整数值
// yyparse函数通常用于解析源代码生成抽象语法树AST，在编写编译器或解释器时，词法分析器和语法分析器通常是分开的模块，yyparse
// 函数通常用于解析源代码并生成抽象语法树AST，yyparse 是语法分析的入口点，它会从词法分析器获取标记，将它们组合成语法结构生成AST
// yyparse执行完完成后，解析器通常会将AST传递给代码生成器进行代码生成。
extern int yyparse(unique_ptr<BaseAST> &ast);

void parse_string(const char* str)
{
  // 解析字符串 str, 得到 Koopa IR 程序
  koopa_program_t program;
  koopa_error_code_t ret = koopa_parse_from_string(str, &program);
  assert(ret == KOOPA_EC_SUCCESS);  // 确保解析时没有出错
  // 创建一个 raw program builder, 用来构建 raw program
  koopa_raw_program_builder_t builder = koopa_new_raw_program_builder();
  // 将 Koopa IR 程序转换为 raw program
  koopa_raw_program_t raw = koopa_build_raw_program(builder, program);
  // 释放 Koopa IR 程序占用的内存
  koopa_delete_program(program);

  cout<<"   .text"<<endl;

  for (size_t i = 0; i < raw.funcs.len; ++i) {
  // 正常情况下, 列表中的元素就是函数, 我们只不过是在确认这个事实
  // 当然, 你也可以基于 raw slice 的 kind, 实现一个通用的处理函数
      assert(raw.funcs.kind == KOOPA_RSIK_FUNCTION);
      // 获取当前函数
      koopa_raw_function_t func = (koopa_raw_function_t) raw.funcs.buffer[i];

      cout<<"   .globl "<<func->name+1<<endl;
      cout<<func->name+1<<":"<<endl;

      for (size_t j = 0; j < func->bbs.len; ++j) {
        assert(func->bbs.kind == KOOPA_RSIK_BASIC_BLOCK);
        koopa_raw_basic_block_t bb = (koopa_raw_basic_block_t)func->bbs.buffer[j];
        for (size_t k = 0; k < bb->insts.len; ++k){
          koopa_raw_value_t value = (koopa_raw_value_t)bb->insts.buffer[k];
          // 示例程序中, 你得到的 value 一定是一条 return 指令
          assert(value->kind.tag == KOOPA_RVT_RETURN);
          // 于是我们可以按照处理 return 指令的方式处理这个 value
          // return 指令中, value 代表返回值
          koopa_raw_value_t ret_value = value->kind.data.ret.value;
          // 示例程序中, ret_value 一定是一个 integer
          assert(ret_value->kind.tag == KOOPA_RVT_INTEGER);
          // 于是我们可以按照处理 integer 的方式处理 ret_value
          // integer 中, value 代表整数的数值
          int32_t int_val = ret_value->kind.data.integer.value;
          // 示例程序中, 这个数值一定是 0
          //assert(int_val == 0);
          cout<<"   li "<<"a0 , "<<int_val<<endl;
          cout<<"   ret"<<endl;
        }
      // ...
      }
    // ...
  }

  // 处理完成, 释放 raw program builder 占用的内存
  // 注意, raw program 中所有的指针指向的内存均为 raw program builder 的内存
  // 所以不要在 raw program builder 处理完毕之前释放 builder
  koopa_delete_raw_program_builder(builder);
}

int main(int argc, const char *argv[]) {
  // 解析命令行参数. 测试脚本/评测平台要求你的编译器能接收如下参数:
  // compiler 模式 输入文件 -o 输出文件
  assert(argc == 5);
  auto mode = argv[1];
  auto input = argv[2];
  auto output = argv[4];

  // 打开输入文件, 并且指定 lexer 在解析的时候读取这个文件
  yyin = fopen(input, "r");
  assert(yyin);

  // 调用 parser 函数, parser 函数会进一步调用 lexer 解析输入文件的
  // parse input file
  unique_ptr<BaseAST> ast;
  auto parse_ret = yyparse(ast);
  assert(!parse_ret);
  freopen("IR.txt","w",stdout);
  ast->Dump();
  // FILE* ff=fopen("IR.txt","r");
  // char *buf=(char *)malloc(10000000);
  // fread(buf, 1,10000000, ff);
  // freopen(output,"w",stdout);
  // parse_string(buf);
  return 0;
}
