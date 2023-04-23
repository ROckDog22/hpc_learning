#pragma once
#include "iostream"
using namespace std;

class BaseAST {
 public:
  virtual ~BaseAST() = default;

  virtual void Dump() const = 0;
};

class FuncTypeAST : public BaseAST {
 public:

  void Dump() const override {
    std::cout<<"i32";
  }
};

class StmtAST : public BaseAST {
  public:
    int number;

    void Dump() const override {
      std::cout<<"%entry:"<<endl;
      std::cout << "  ret "<<std::to_string(number)<<endl;;
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
    std::cout << "fun @"<<ident<<"(): ";
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
