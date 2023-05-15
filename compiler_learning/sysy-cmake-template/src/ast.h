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

#pragma once

#include "iostream"
using namespace std;

// int now 定义为强符号，指的是全局变量或者函数，并且该变量或函数被多个文件引用，那么在链接时会出现符号重复定义
// 的问题 解决办法是将强符号改为弱符号，可以通过在定义时使用关键字extern 来实现，int x = 1 改为弱符号 extern int x =1;
// 这样在其他文件中使用 extern int x 就不会出现符号重复定义的错误， 如果定义是不指定处置 默认为弱符号
// 使用命名空间，可以将定义强符号的变量或函数放在一个命名空间中，以避免与其他文件中的符号冲突，使用时前面加上命名空间的名称
// 将定义放在头文件中，如果定义强符号的变量或函数需要在多个文件中使用，引入头文件，在包含头文件时需要使用预处理指令#ifndef #define
// 避免头文件重复包含
// 如果只需要在当前文件中使用，可以将其定义为静态变量或函数，加上static 就只会在当前文件中使用避免冲突

static int now = 0;

class BaseAST {
 public:

  virtual ~BaseAST() = default;

  virtual void Dump() const = 0;

};

class NumberAST : public BaseAST  {
  public:
    int val;

    void Dump() const {
      // std::cout << val;
    }
  };

class UnaryOpAst : public BaseAST {
 public:
  char val;

  void Dump() const override {
    std::cout << val;
  }
};

class ExpAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> unaryexp;

  void Dump() const override {
    unaryexp->Dump();
  }
};

class PrimaryExpAST : public BaseAST {
 public:
  int flag;
  std::unique_ptr<BaseAST> exp;
  std::unique_ptr<BaseAST> number;
  
  void Dump() const override {
    if(flag==1){
      exp->Dump();
    }else if(flag==2){
      number->Dump();
    }
  }
};

class UnaryExpAST : public BaseAST {
 public:

  int flag;
  char unaryop;
  std::unique_ptr<BaseAST> primaryexp;
  std::unique_ptr<BaseAST> unaryexp;

  void Dump() const override {
    if(flag == 1){
      primaryexp->Dump();
    }else if(flag == 2){
      unaryexp->Dump();
      if(unaryop == '-'){
        cout<<"  %"<<now <<" = sub 0, %"<<now-1<<endl;
      }
      if(unaryop=='!'){
        cout<<"  %"<<now <<" = eq "<< 6 <<", 0"<<endl;
      }
      now++;
    }
  }
};

class FuncTypeAST : public BaseAST {
 public:

  void Dump() const override {
    std::cout<<" i32 ";
  }
};

class StmtAST : public BaseAST {
  public:

    std::unique_ptr<BaseAST> exp;

    void Dump() const override {
      std::cout<<"%entry:"<<endl;
      exp->Dump();
      std::cout << "  ret %"<<now;
  }
};

class BlockAST : public BaseAST {
 public:

  std::unique_ptr<BaseAST> stmt;

  void Dump() const override {
    std::cout << "{"<<endl;
    stmt->Dump();
    std::cout << "}"<<endl;
  }
};

class FuncDefAST : public BaseAST {
 public:

  std::unique_ptr<BaseAST> func_type;
  std::string ident;
  std::unique_ptr<BaseAST> block;

  void Dump() const override {
    std::cout << "fun @"<<ident<<"():";
    func_type->Dump();
    block->Dump();
  }
};

class CompUnitAST : public BaseAST {
 public:

  std::unique_ptr<BaseAST> func_def;

  void Dump() const override {
    func_def->Dump();
  }
};
