#include "expressions_dd.h"

#include <iostream>
#include <sstream>
#include <iostream>

namespace ky_expr {

int Expressions::Compute(Expressions::Expr &e) {
  class V : public ExpressionVisitor {
  public:
    void Visit(ConstantExpression &e) { result_ = e.GetValue(); }

    void Visit(BinaryOperatorExpression &e) {
      e.GetLHS().Accept(*this);
      int l = result_;

      e.GetRHS().Accept(*this);
      int r = result_;

      switch (e.GetOp()) {
        case '+':
          result_ = l + r;
          break;
        case '-':
          result_ = l - r;
          break;
        case '*':
          result_ = l * r;
          break;
        case '/':
          result_ = l / r;
          break;
        case '^':
          result_ = l ^ r;
          break;
        default:
          std::cerr << "unknown operator " << e.GetOp() << std::endl;
          abort();
      }
    }

    void Visit(TernaryOperatorExpression &e) {
      e.GetCondition().Accept(*this);
      int c = result_;
      e.GetOnTrue().Accept(*this);
      int t = result_;
      e.GetOnFalse().Accept(*this);
      int f = result_;
      result_ = c ? t : f;
    }

    void Visit(FunctionCallExpression &e) {
      auto fold = e.GetName() == "min"
                      ? [](int x, int y) { return x < y ? x : y; }
                  : e.GetName() == "max"
                      ? [](int x, int y) { return x < y ? x : y; }
                      : [](int x, int y) {
                          std::cerr << "unknown function " << std::endl;
                          abort();
                          return 0;
                        };

      bool first = true;
      int result;
      for (const auto &parameter : e.GetParameters()) {
        parameter->Accept(*this);
        result = first ? first = false, result_ : fold(result, result_);
      }
      result_ = result;
    }

    int result_;
  } v;

  e->Accept(v);
  return v.result_;
}

std::string Expressions::AsString(Expressions::Expr &e) {
  class V : public ExpressionVisitor {
  public:
    void Visit(ConstantExpression &e) {
      result_ = "(" + std::to_string(e.GetValue()) + ")";
      // std::stringstream stream;
      // stream << "(" << e.GetValue() << ")";
      // result_ = stream.str();
    }

    void Visit(BinaryOperatorExpression &e) {
      e.GetLHS().Accept(*this);
      auto l = std::move(result_);
      e.GetRHS().Accept(*this);
      auto r = std::move(result_);
      result_ = "(" + l + e.GetOp() + r + ")";
      // std::stringstream stream;
      // stream << "(" << l << e.GetOp() << r << ")";
      // result_ = stream.str();
    }

    void Visit(TernaryOperatorExpression &e) {
      e.GetCondition().Accept(*this);
      auto c = std::move(result_);
      e.GetOnTrue().Accept(*this);
      auto t = std::move(result_);
      e.GetOnFalse().Accept(*this);
      auto f = std::move(result_);
      result_ = "(" + c + "?" + t + ":" + f + ")";
      // std::stringstream stream;
      // stream << "(" << c << "?" << t << ":" << f << ")";
      // result_ = stream.str();
    }

    void Visit(FunctionCallExpression &e) {
      std::stringstream stream;
      stream << e.GetName() << "(";
      bool first = true;
      for (const auto &parameter : e.GetParameters()) {
        if (first) {
          first = false;
        } else {
          stream << ",";
        }
        parameter->Accept(*this);
        stream << result_;
      }
      stream << ")";
      result_ = stream.str();
    }

    std::string result_;
  } v;

  e->Accept(v);
  return v.result_;
}

void Expressions::Save(Expressions::Expr &e, std::ostream &s) {
  class V : public ExpressionVisitor {
  public:
    V(std::ostream &s) : s_(s) {}

    // NOLINTNEXTLINE()
    void Visit(ConstantExpression &e) { s_ << "C " << e.GetValue() << " "; }

    void Visit(BinaryOperatorExpression &e) {
      // NOLINTNEXTLINE()
      s_ << "BOp " << e.GetOp() << " ";
      e.GetLHS().Accept(*this);
      e.GetRHS().Accept(*this);
    }

    void Visit(TernaryOperatorExpression &e) {
      s_ << "TOp ";
      e.GetCondition().Accept(*this);
      e.GetOnTrue().Accept(*this);
      e.GetOnFalse().Accept(*this);
    }

    void Visit(FunctionCallExpression &e) {
      s_ << "Fx " << e.GetName() << " " << e.GetParameters().size() << " ";
      for (const auto &parameter : e.GetParameters()) {
        parameter->Save(s_);
      }
    }

    std::ostream &s_;
  } v(s);

  e->Accept(v);
}

}  // namespace ky_expr
