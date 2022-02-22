#ifndef KY_EXPRESSIONS_DD_H
#define KY_EXPRESSIONS_DD_H

#include "expressions.h"

namespace ky_expr {

class Expressions final {
public:
  using Expr = std::unique_ptr<Expression>;

  static int Compute(Expr &e);
  static std::string AsString(Expr &e);
  static void Save(Expr &e, std::ostream &s);
};

}  // namespace ky_expr

#include "expressions_dd.cc"

#endif  // KY_EXPRESSIONS_DD_H
