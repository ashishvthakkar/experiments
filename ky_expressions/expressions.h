#ifndef KY_EXPRESSIONS_H
#define KY_EXPRESSIONS_H

#include <memory>
#include <string>
#include <vector>

namespace ky_expr {

class ConstantExpression;
class BinaryOperatorExpression;
class TernaryOperatorExpression;
class FunctionCallExpression;

class ExpressionVisitor {
public:
  virtual void Visit(ConstantExpression &) = 0;
  virtual void Visit(BinaryOperatorExpression &) = 0;
  virtual void Visit(TernaryOperatorExpression &) = 0;
  virtual void Visit(FunctionCallExpression &) = 0;
};

class Expression {
public:
  virtual ~Expression() = default;

  virtual int Compute() = 0;
  virtual std::string AsString() = 0;

  static std::unique_ptr<Expression> Load(std::istream &s);
  virtual void Save(std::ostream &s) = 0;

  virtual void Accept(ExpressionVisitor &visitor) = 0;
};

class ConstantExpression final : public Expression {
public:
  ConstantExpression(int value);

  int Compute() override;
  std::string AsString() override;

  static std::unique_ptr<Expression> Load(std::istream &s);
  void Save(std::ostream &s) override;

  void Accept(ExpressionVisitor &visitor) override { visitor.Visit(*this); }

  int GetValue() { return value_; }

private:
  int value_;
};

class BinaryOperatorExpression final : public Expression {
public:
  BinaryOperatorExpression(
      char op,
      std::unique_ptr<Expression> l,
      std::unique_ptr<Expression> r);

  int Compute() override;
  std::string AsString() override;

  static std::unique_ptr<Expression> Load(std::istream &s);
  void Save(std::ostream &s) override;

  void Accept(ExpressionVisitor &visitor) override { visitor.Visit(*this); }

  char GetOp() { return op_; }
  Expression &GetLHS() { return *l_; }
  Expression &GetRHS() { return *r_; }

private:
  char op_;
  std::unique_ptr<Expression> l_;
  std::unique_ptr<Expression> r_;
};

class TernaryOperatorExpression final : public Expression {
public:
  TernaryOperatorExpression(
      std::unique_ptr<Expression> condition,
      std::unique_ptr<Expression> onTrue,
      std::unique_ptr<Expression> onFalse);

  int Compute() override;
  std::string AsString() override;

  void Accept(ExpressionVisitor &visitor) override { visitor.Visit(*this); }

  static std::unique_ptr<Expression> Load(std::istream &s);
  void Save(std::ostream &s) override;

  Expression &GetCondition() { return *condition_; }
  Expression &GetOnTrue() { return *onTrue_; }
  Expression &GetOnFalse() { return *onFalse_; }

private:
  std::unique_ptr<Expression> condition_;
  std::unique_ptr<Expression> onTrue_;
  std::unique_ptr<Expression> onFalse_;
};

class FunctionCallExpression final : public Expression {
public:
  FunctionCallExpression(
      std::string name,
      std::vector<std::unique_ptr<Expression>> parameters);

  int Compute() override;
  std::string AsString() override;

  void Accept(ExpressionVisitor &visitor) override { visitor.Visit(*this); }

  static std::unique_ptr<Expression> Load(std::istream &s);
  void Save(std::ostream &s) override;

  const std::string &GetName() { return name_; }
  const std::vector<std::unique_ptr<Expression>> &GetParameters() {
    return parameters_;
  };

private:
  std::string name_;
  std::vector<std::unique_ptr<Expression>> parameters_;
};

}  // namespace ky_expr

#include "expressions.cc"

#endif  // KY_EXPRESSIONS_H
