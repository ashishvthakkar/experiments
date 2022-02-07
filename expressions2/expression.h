#ifndef EXPERIMENTS_EXPRESSIONS2_EXPRESSION_H
#define EXPERIMENTS_EXPRESSIONS2_EXPRESSION_H

#include <iostream>
#include <memory>
#include <string>

#include "operator_node.h"

namespace code_experiments {

template <typename T>
class OperatorNode;

template <typename T>
class Expression {
public:
  explicit Expression(const std::string& expr);
  explicit Expression(std::istream& in);
  Expression<T>& operator=(Expression<T>&& from) noexcept;
  Expression(Expression<T>&& from) noexcept;
  Expression();

  ~Expression() = default;
  Expression<T>& operator=(Expression<T>& from) = delete;
  Expression(const Expression<T>& from) = delete;

  [[nodiscard]] T Eval();
  void PrintAsTree(int indent);
  std::string ToStringWithParen();
  void ToStream(std::ostream& out);

private:
  [[nodiscard]] int GetOperatorIndex(const std::string& expr) const;
  void Parse(const std::string& expr);
  [[nodiscard]] T GetSimplifiedVal(const std::string& expr) const;

  T simplified_value_;
  std::unique_ptr<OperatorNode<T>> operator_node_cache_;
};

}  // namespace code_experiments

#endif