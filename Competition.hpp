#pragma once

#include <compare>

#include "../Empirical/include/emp/io/io_utils.hpp"

#include "Strategy.hpp"

class CompetitionResult {
private:
  int score1 = 0;
  int score2 = 0;

  int total_rounds = 0;

  int coop_coop_count = 0;
  int coop_defect_count = 0;
  int defect_coop_count = 0;
  int defect_defect_count = 0;

  int retaliation_count1 = 0; // How many times defect AFTER opponent defected
  int aggression_count1 = 0;  // How many times defect AFTER opponent cooperated
  int forgiveness_count1 = 0; // How many times cooperate AFTER opponent defected
  int reciprocity_count1 = 0; // How many times cooperate AFTER opponent cooperated

  int retaliation_count2 = 0; // How many times defect AFTER opponent defected
  int aggression_count2 = 0;  // How many times defect AFTER opponent cooperated
  int forgiveness_count2 = 0; // How many times cooperate AFTER opponent defected
  int reciprocity_count2 = 0; // How many times cooperate AFTER opponent cooperated

public:
  CompetitionResult() = default;
  CompetitionResult(const CompetitionResult &) = default;
  CompetitionResult(const bool play1, const bool play2, const bool last1, const bool last2)
    : total_rounds(1)
  {
    if (play1 == DEFECT && play2 == DEFECT) {
      score1 = 1;
      score2 = 1;
      defect_defect_count = 1;
    }
    else if (play1 == COOPERATE && play2 == COOPERATE) {
      score1 = 3;
      score2 = 3;
      coop_coop_count = 1;
    }
    else if (play1 == COOPERATE && play2 == DEFECT) {
      score1 = 0;
      score2 = 5;
      coop_defect_count = 1;
    }
    else if (play1 == DEFECT && play2 == COOPERATE) {
      score1 = 5;
      score2 = 0;
      defect_coop_count = 1;
    }

    if (play1 == DEFECT && last1 == DEFECT) { retaliation_count1 = 1; }
    if (play1 == DEFECT && last1 == COOPERATE) { aggression_count1 = 1; }
    if (play1 == COOPERATE && last1 == DEFECT) { forgiveness_count1 = 1; }
    if (play1 == COOPERATE && last1 == COOPERATE) { reciprocity_count1 = 1; }

    if (play2 == DEFECT && last2 == DEFECT) { retaliation_count2 = 1; }
    if (play2 == DEFECT && last2 == COOPERATE) { aggression_count2 = 1; }
    if (play2 == COOPERATE && last2 == DEFECT) { forgiveness_count2 = 1; }
    if (play2 == COOPERATE && last2 == COOPERATE) { reciprocity_count2 = 1; }
  }

  CompetitionResult & operator=(const CompetitionResult &) = default;

  CompetitionResult & operator+=(CompetitionResult in) {
    score1 += in.score1;
    score2 += in.score2;
    total_rounds += in.total_rounds;
    coop_coop_count += in.coop_coop_count;
    coop_defect_count += in.coop_defect_count;
    defect_coop_count += in.defect_coop_count;
    defect_defect_count += in.defect_defect_count;
    retaliation_count1 += in.retaliation_count1;
    aggression_count1 += in.aggression_count1; 
    forgiveness_count1 += in.forgiveness_count1;
    reciprocity_count1 += in.reciprocity_count1;
    retaliation_count2 += in.retaliation_count2;
    aggression_count2 += in.aggression_count2; 
    forgiveness_count2 += in.forgiveness_count2;
    reciprocity_count2 += in.reciprocity_count2;
    return *this;
  }

  [[nodiscard]] CompetitionResult operator+(CompetitionResult in) const {
    in += *this;
    return in;
  }

  [[nodiscard]] int GetScore1() const { return score1; }
  [[nodiscard]] int GetScore2() const { return score2; }
  [[nodiscard]] int GetCooperate1() const { return forgiveness_count1 + reciprocity_count1; }
  [[nodiscard]] int GetCooperate2() const { return forgiveness_count2 + reciprocity_count2; }
  [[nodiscard]] int GetDefect1() const { return retaliation_count1 + aggression_count1; }
  [[nodiscard]] int GetDefect2() const { return retaliation_count2 + aggression_count2; }

};

class Competition {
private:
  const SummaryStrategy strategy1;
  const SummaryStrategy strategy2;
  const size_t num_rounds;

  // Indicates whether a hard defect will occur in the competition
  const bool hard_defect_toggle; 
  const size_t hard_defect_round;

  emp::vector<CompetitionResult> results;

public:
  Competition(SummaryStrategy strategy1, 
              SummaryStrategy strategy2, 
              size_t num_rounds,
              bool hard_defect_toggle,
              size_t hard_defect_round)
    : strategy1(strategy1), 
      strategy2(strategy2), 
      num_rounds(num_rounds), 
      hard_defect_toggle(hard_defect_toggle),
      hard_defect_round(hard_defect_round) {}

  [[nodiscard]] CompetitionResult Compete(
    emp::BitVector & mem1,
    emp::BitVector & mem2,
    // 'hd_toggle' (NOT 'hard_defect_toggle') indicates whether a hard defect will occur this round
    const bool hd_toggle) const 
  { 
    const bool action1 = hd_toggle ? DEFECT : strategy1.GetAction(mem1);
    const bool action2 = hd_toggle ? DEFECT : strategy2.GetAction(mem2);
    CompetitionResult result(action1, action2, mem1[0], mem2[0]);
    // emp::PrintLn("Action1:", action1, "  Action2:", action2);

    // Update Memory
    mem1.PushFront(action2);
    mem1.PopBack();
    mem2.PushFront(action1);
    mem2.PopBack();

    return result;
  }

  [[nodiscard]] auto operator==(const Competition & in) const {
    return strategy1 == in.strategy1 && strategy2 == in.strategy2 && num_rounds == in.num_rounds;
  }

  [[nodiscard]] auto operator<(const Competition & in) const {
    if (strategy1 < in.strategy1) return true;
    if (strategy1 > in.strategy1) return false;
    if (strategy2 < in.strategy2) return true;
    if (strategy2 > in.strategy2) return false;
    return num_rounds < in.num_rounds;
  }

  [[nodiscard]] CompetitionResult Run() const {
    CompetitionResult result;
    emp::BitVector mem1 = strategy1.GetStartState();
    emp::BitVector mem2 = strategy2.GetStartState();

    for (size_t i = 0; i < num_rounds; ++i) {
      if (hard_defect_toggle && i == hard_defect_round) {
        CompetitionResult r = Compete(mem1, mem2, true);
        results.emplace_back(r);
        result += r;
      }
      else { 
        CompetitionResult r = Compete(mem1, mem2, false); 
        results.emplace_back(r);
        result += r;
      } 
    }
    return result;
  }

  [[nodiscard]] emp::vector<CompetitionResult> GetResults() const { return results; }
};

class CompetitionManager {
private:
  mutable std::map<Competition, CompetitionResult> result_cache;

public:
  const CompetitionResult & Compete(
    SummaryStrategy strategy1,
    SummaryStrategy strategy2,
    size_t num_rounds,
    bool hard_defect_toggle,
    size_t hard_defect_round) const
  {
    Competition competition{strategy1, strategy2, num_rounds, hard_defect_toggle, hard_defect_round};
    if (!result_cache.contains(competition)) {
      result_cache[competition] = competition.Run();
    }
    return result_cache[competition];
  }
};
