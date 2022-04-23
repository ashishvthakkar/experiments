#include <glog/logging.h>

#include <cmath>
#include <functional>
#include <vector>

class Payoff {
public:
  static double Simple(int num_turns) {
    double ev = 0;
    for (auto i = 0; i < num_turns - 1; i++) {
      ev += std::pow(1 - kValueProb, i) * kValueProb * kMaxTurnPayoff;
    }
    ev += std::pow(1 - kValueProb, num_turns - 1) * kTurnEV;
    return ev;
  }

  static double Better(int num_turns) {
    double next_ev = kTurnEV;
    double curr_ev = 0;
    for (auto i = num_turns - 1; i >= 1; i--) {
      curr_ev = 0;
      double remaining_prob = 1;
      for (int value = std::ceil(next_ev); value <= kMaxTurnPayoff; value++) {
        curr_ev += kValueProb * value;
        remaining_prob -= kValueProb;
      }
      curr_ev += remaining_prob * next_ev;
      next_ev = curr_ev;
    }
    return curr_ev;
  }

  // NOTE: This is a simple version and can be made a lot more efficient.
  static double NumTurnsFor(
      double payoff,
      const std::function<double(int)> &compute) {
    auto num_turns = 0;
    while (compute(num_turns) < payoff) num_turns++;
    return num_turns;
  }

private:
  static constexpr auto kValueProb = 1.0 / 6;
  static constexpr auto kTurnEV = 3.5;
  static constexpr auto kMaxTurnPayoff = 6;
};

int main() {
  const int max_turns = 50;
  for (auto num_turns = 1; num_turns <= max_turns; num_turns++) {
    LOG(INFO) << "Payoff for " << num_turns << " turns with "
              << "simple play = " << Payoff::Simple(num_turns)
              << " and with better play = " << Payoff::Better(num_turns);
  }
  const double desired_payoff = 5.99;
  LOG(INFO) << "Num turns required for a payoff of at least " << desired_payoff
            << " with simple play = "
            << Payoff::NumTurnsFor(desired_payoff, Payoff::Simple)
            << " and with better play = "
            << Payoff::NumTurnsFor(desired_payoff, Payoff::Better);
}