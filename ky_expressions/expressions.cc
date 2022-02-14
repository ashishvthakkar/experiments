#include "expressions.h"

#include <iostream>
#include <map>
#include <sstream>

namespace ky_expr {

//--- Constant ---

ConstantExpression::ConstantExpression(int value) : value_(value) {}

int ConstantExpression::Compute() { return value_; }

std::string ConstantExpression::AsString() {
  // std::stringstream stream;
  // stream << "(" << value_ << ")";
  // return stream.str();
  return "(" + std::to_string(value_) + ")";
}

std::unique_ptr<Expression> ConstantExpression::Load(std::istream &s) {
  int value;
  s >> value;
  return std::make_unique<ConstantExpression>(value);
}

void ConstantExpression::Save(std::ostream &s) { s << "C " << value_ << " "; }

//--- BinaryOperatorExpression ---

BinaryOperatorExpression::BinaryOperatorExpression(
    char op,
    std::unique_ptr<Expression> l,
    std::unique_ptr<Expression> r)
    : op_(op),
      l_(std::move(l)),
      r_(std::move(r)) {}

int BinaryOperatorExpression::Compute() {
  switch (op_) {
    case '+':
      return l_->Compute() + r_->Compute();
    case '-':
      return l_->Compute() - r_->Compute();
    case '*':
      return l_->Compute() * r_->Compute();
    case '/':
      return l_->Compute() / r_->Compute();
    case '^':
      return l_->Compute() ^ r_->Compute();
    default:
      std::cerr << "bad operator" << std::endl;
      abort();
  }
}

std::string BinaryOperatorExpression::AsString() {
  // std::stringstream stream;
  // stream << "(" << l_->AsString() << op_ << r_->AsString() << ")";
  // return stream.str();
  return "(" + l_->AsString() + op_ + r_->AsString() + ")";
}

std::unique_ptr<Expression> BinaryOperatorExpression::Load(std::istream &s) {
  char o;
  s >> o;
  return std::make_unique<BinaryOperatorExpression>(
      o,
      std::move(Expression::Load(s)),
      std::move(Expression::Load(s)));
}

void BinaryOperatorExpression::Save(std::ostream &s) {
  s << "BOp " << op_ << " ";
  l_->Save(s);
  r_->Save(s);
}

//--- TernaryOperatorExpression ---

TernaryOperatorExpression::TernaryOperatorExpression(
    std::unique_ptr<Expression> condition,
    std::unique_ptr<Expression> onTrue,
    std::unique_ptr<Expression> onFalse)
    : condition_(std::move(condition)),
      onTrue_(std::move(onTrue)),
      onFalse_(std::move(onFalse)) {}

int TernaryOperatorExpression::Compute() {
  return condition_->Compute() ? onTrue_->Compute() : onFalse_->Compute();
}

std::string TernaryOperatorExpression::AsString() {
  std::stringstream stream;
  stream << "(" << condition_->AsString() << "?" << onTrue_->AsString() << ":"
         << onFalse_->AsString() << ")";
  return stream.str();
}

std::unique_ptr<Expression> TernaryOperatorExpression::Load(std::istream &s) {
  auto c = Expression::Load(s);
  auto t = Expression::Load(s);
  auto f = Expression::Load(s);
  return std::make_unique<TernaryOperatorExpression>(
      std::move(c),
      std::move(t),
      std::move(f));
}

void TernaryOperatorExpression::Save(std::ostream &s) {
  s << "TOp ";
  condition_->Save(s);
  onTrue_->Save(s);
  onFalse_->Save(s);
}

//--- FunctionCallExpression ---

FunctionCallExpression::FunctionCallExpression(
    std::string name,
    std::vector<std::unique_ptr<Expression>> parameters)
    : name_(std::move(name)),
      parameters_(std::move(parameters)) {}

int FunctionCallExpression::Compute() {
  auto fold = name_ == "min" ? [](int x, int y) { return x < y ? x : y; }
              : name_ == "max"
                  ? [](int x, int y) { return x < y ? x : y; }
                  : [](int x, int y) {
                      std::cerr << "unknown function " << std::endl;
                      abort();
                      return 0;
                    };

  bool first = true;
  int result;
  for (const auto &parameter : parameters_) {
    result = first ? (first = false, parameter->Compute())
                   : fold(result, parameter->Compute());
  }

  return result;
}

std::string FunctionCallExpression::AsString() {
  std::stringstream stream;
  stream << name_ << "(";
  bool first = true;
  for (const auto &parameter : parameters_) {
    if (first) {
      first = false;
    } else {
      stream << ",";
    }
    stream << parameter->AsString();
  }
  stream << ")";
  return stream.str();
}

std::unique_ptr<Expression> FunctionCallExpression::Load(std::istream &s) {
  std::string name;
  s >> name;

  int count;
  s >> count;

  std::vector<std::unique_ptr<Expression>> parameters;
  while (count--) {
    parameters.push_back(Expression::Load(s));
  }

  return std::make_unique<FunctionCallExpression>(
      std::move(name),
      std::move(parameters));
}

void FunctionCallExpression::Save(std::ostream &s) {
  s << "Fx " << name_ << " " << parameters_.size() << " ";
  for (const auto &parameter : parameters_) {
    parameter->Save(s);
  }
}

//--- Expression ---

std::unique_ptr<Expression> Expression::Load(std::istream &s) {
  using Loader = std::unique_ptr<Expression> (*)(std::istream &);
  static std::map<std::string, Loader> loaders;

  static bool initialized = false;
  if (!initialized) {
    loaders["C"] = ConstantExpression::Load;
    loaders["BOp"] = BinaryOperatorExpression::Load;
    loaders["TOp"] = TernaryOperatorExpression::Load;
    loaders["Fx"] = FunctionCallExpression::Load;
  }

  std::string key;
  s >> key;
  auto i = loaders.find(key);
  if (i == loaders.end()) {
    std::cerr << "bad token: " << key << std::endl;
    abort();
  }
  return i->second(s);
}

}  // namespace ky_expr
