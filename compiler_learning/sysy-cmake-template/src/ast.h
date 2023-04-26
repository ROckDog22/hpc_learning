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
      std::cout << val;
    }
  };

class UnaryOpAst : public BaseAST {
 public:
  string val;

  void Dump() const override {
    std::cout << val;
  }
};

class ExpAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> unaryexp;

  void Dump() const override {
    std::cout << "ExpAST";
  }
};

class PrimaryExpAST : public BaseAST {
 public:
  int flag;
  std::unique_ptr<BaseAST> exp;
  std::unique_ptr<BaseAST> number;

  void Dump() const override {
    std::cout << "PrimaryExp";
  }
};

class UnaryExpAST : public BaseAST {
 public:

  string flag;
  std::unique_ptr<BaseAST> primaryexp;
  std::unique_ptr<BaseAST> unaryop;
  std::unique_ptr<BaseAST> unaryexp;

  void Dump() const override {
    std::cout << "UnaryExpAst";
  }
};

class FuncTypeAST : public BaseAST {
 public:

  void Dump() const override {
    std::cout<<"i32";
  }
};

class StmtAST : public BaseAST {
  public:

    std::unique_ptr<BaseAST> exp;

    void Dump() const override {
      std::cout<<"%entry:"<<endl;
      std::cout << "PrimaryExp";
      std::cout << "  ret "<<endl;;
  }
};

class BlockAST : public BaseAST {
 public:

  std::unique_ptr<BaseAST> stmt;

  void Dump() const override {
    std::cout << "{"<<endl;
    std::cout << "PrimaryExp";
    std::cout << "}"<<endl;
  }
};

class FuncDefAST : public BaseAST {
 public:

  std::unique_ptr<BaseAST> func_type;
  std::string ident;
  std::unique_ptr<BaseAST> block;

  void Dump() const override {
    std::cout << "fun @"<<ident<<"(): ";
    std::cout << "PrimaryExp";
    block->Dump();
  }
};

class CompUnitAST : public BaseAST {
 public:

  std::unique_ptr<BaseAST> func_def;

  void Dump() const override {
    std::cout << "PrimaryExp";
  }
};
