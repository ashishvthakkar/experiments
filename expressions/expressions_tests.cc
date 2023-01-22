#include <gtest/gtest.h>
#include <glog/logging.h>

#include <algorithm>
#include <map>

#include "expression.h"
#include "operator_node.h"
#include "utils.h"

namespace code_experiments {

class ExpressionsTests : public ::testing::Test {};

TEST_F(ExpressionsTests, BasicOperatorCheck) {  // NOLINT
  std::string left("1");
  std::string right("2");
  OperatorNode<int> node(left, '+', right);
  EXPECT_EQ(node.Eval(), 3);
}

TEST_F(ExpressionsTests, BasicExpressionsCheck) {  // NOLINT
  std::string expression("1+2");
  Expression<int> expr(expression);
  EXPECT_EQ(expr.Eval(), 3);
}

TEST_F(ExpressionsTests, BasicNoParensCheck) {  // NOLINT
  std::string expression("5+3*5");
  Expression<int> expr(expression);
  EXPECT_EQ(expr.Eval(), 20);
  expression = "5-3*5";
  Expression<int> expr2(expression);
  EXPECT_EQ(expr2.Eval(), -10);
}

TEST_F(ExpressionsTests, BasicBodmasCheck) {  // NOLINT
  std::map<std::string, int> expression_to_value = {
      {"5+3*5+1", 21},       // NOLINT(cppcoreguidelines-avoid-magic-numbers)
      {"5+3*5-1", 19},       // NOLINT(cppcoreguidelines-avoid-magic-numbers)
      {"1+(2+3)*4", 21},     // NOLINT(cppcoreguidelines-avoid-magic-numbers)
      {"2-3-1", -2},         // NOLINT(cppcoreguidelines-avoid-magic-numbers)
      {"-3-1+2", -2},        // NOLINT(cppcoreguidelines-avoid-magic-numbers)
      {"+3-1+2", 4},         // NOLINT(cppcoreguidelines-avoid-magic-numbers)
      {"(-5)", -5},          // NOLINT(cppcoreguidelines-avoid-magic-numbers)
      {"(5+3)*5+1", 41},     // NOLINT(cppcoreguidelines-avoid-magic-numbers)
      {"5+3*(5-1)", 17},     // NOLINT(cppcoreguidelines-avoid-magic-numbers)
      {"1+(2+3)*4+5", 26},   // NOLINT(cppcoreguidelines-avoid-magic-numbers)
      {"1 + 4/2 - 5/4", 2},  // NOLINT(cppcoreguidelines-avoid-magic-numbers)
      {"5^2+(3-1)^3-5+2+2*3",
       36}};  // NOLINT(cppcoreguidelines-avoid-magic-numbers)
  std::for_each(
      expression_to_value.begin(),
      expression_to_value.end(),
      [](const std::pair<std::string, int>& element) {
        int value = Compute(element.first);
        LOG(INFO) << element.first << " = " << value;
        EXPECT_EQ(value, element.second);
      });
}

TEST_F(ExpressionsTests, SimpleSimplifyCheck) {  // NOLINT
  EXPECT_EQ(Simplify("a"), "a");
  EXPECT_EQ(Simplify("a*(b+c)"), "a*b+a*c");
  EXPECT_EQ(Simplify("(a+b)*(a+b)"), "2a*b+a^2+b^2");
}

TEST_F(ExpressionsTests, SimplifyCheck) {  // NOLINT
  std::map<std::string, std::string> expression_to_value = {
      {"a+b", "a+b"},
      {"a-b", "a-b"},
      {"-a+b", "-a+b"},
      {"a*b", "a*b"},
      {"a/b", "a/b"},
      {"a*(b+c)", "a*b+a*c"},
      {"a/(b+c)", "a/(b+c)"},
      {"(a+b)/(c+d)", "a/(c+d)+b/(c+d)"},
      {"(a+b)*(c+d)", "a*c+a*d+b*c+b*d"},
      {"(a+b+c)*(d+e+f)", "a*d+a*e+a*f+b*d+b*e+b*f+c*d+c*e+c*f"},
      {"(a+b)*(c+d)*(e+f)", "a*c*e+a*c*f+a*d*e+a*d*f+b*c*e+b*c*f+b*d*e+b*d*f"},
      {"(a-b)*(c-d)", "a*c-a*d-b*c+b*d"},
      {"(a-b)*(a-b)", "-2a*b+a^2+b^2"},
      {"2a*(c+3d)", "2a*c+6a*d"},
      {"4a/2b", "2a/b"},
      {"(a+2b)*(c+2d)", "a*c+2a*d+2b*c+4b*d"},
      {"a+b-a", "b"},
      {"(a+b)*(a-b)", "a^2-b^2"}};
  std::for_each(
      expression_to_value.begin(),
      expression_to_value.end(),
      [](const std::pair<std::string, std::string>& element) {
        std::string value = Simplify(element.first);
        LOG(INFO) << element.first << " = " << value;
        EXPECT_EQ(value, element.second);
      });
}

TEST_F(ExpressionsTests, StoiCheck) {  // NOLINT
  std::string::size_type sz = 0;
  int coeff = std::stoi("2a", &sz);
  EXPECT_EQ(coeff, 2);
  EXPECT_EQ(sz, 1);
  std::string expr("16b*c");
  coeff = std::stoi(expr, &sz);
  EXPECT_EQ(coeff, 16);
  EXPECT_EQ(sz, 2);
  EXPECT_EQ(expr.substr(sz), "b*c");
}

TEST_F(ExpressionsTests, IndirectPowersCheck) {  // NOLINT
  std::string basic_expr("(a+b)");
  std::string expr(basic_expr);
  const int power = 4;
  for (int i = 1; i < power; i++) {
    expr += "*" + basic_expr;
  }
  const std::string expected = "4a*b^3+6a^2*b^2+4a^3*b+a^4+b^4";
  const std::string result(Simplify(expr));
  LOG(INFO) << basic_expr << " to the power " << power << ": " << result;
  EXPECT_EQ(result, expected);
}

TEST_F(ExpressionsTests, DirectPowersCheck) {  // NOLINT
  std::map<std::string, std::string> expression_to_value = {
      {"a^2", "a^2"},
      {"(a-b)^2", "-2a*b+a^2+b^2"},
      {"(a+b)^2+(c+d)", "2a*b+a^2+b^2+c+d"},
      {"(a*(b+c))^2", "2a^2*b*c+a^2*b^2+a^2*c^2"},
      {"(a*(b-c))^2", "-2a^2*b*c+a^2*b^2+a^2*c^2"},
      {"(a+b)^4", "4a*b^3+6a^2*b^2+4a^3*b+a^4+b^4"}};
  std::for_each(
      expression_to_value.begin(),
      expression_to_value.end(),
      [](const std::pair<std::string, std::string>& element) {
        std::string value = Simplify(element.first);
        LOG(INFO) << element.first << " = " << value;
        EXPECT_EQ(value, element.second);
      });
}

}  // namespace code_experiments