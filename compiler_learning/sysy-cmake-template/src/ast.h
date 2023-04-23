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
  std::unique_ptr<std::string> value;

  void Dump() const override {
    std::cout << "FuncTypeAST { ";
    std::cout<< *value;
    std::cout << " }";
  }
};

// class NumberAST : public BaseAST {
//  public:
//   int value;

//   void Dump() const override {
//     std::cout << value;
//   }
// };

class StmtAST : public BaseAST {
  public:
    int number;

    void Dump() const override {
      std::cout << "StmtAST { return ";
      std::cout << std::to_string(number);
      std::cout << " };";
  }
};

class BlockAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> stmt;
  void Dump() const override {
    std::cout << "BlockAST { ";
    stmt->Dump();
    std::cout << " }";
  }
};

class FuncDefAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> func_type;
  std::string ident;
  std::unique_ptr<BaseAST> block;

  void Dump() const override {
    std::cout << "FuncDefAST { ";
    func_type->Dump();
    std::cout << ", " << ident << ", ";
    block->Dump();
    std::cout << " }";
  }
};

class CompUnitAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> func_def;

  void Dump() const override {
    std::cout << "CompUnitAST { ";
    func_def->Dump();
    std::cout << " }";
  }
};
