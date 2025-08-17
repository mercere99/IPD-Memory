#pragma once

#include <compare>

#include "emp/io/io_utils.hpp"
#include "emp/bits/Bits.hpp"

#include "Strategy.hpp"

class CompetitionResult {
private:
  emp::BitVector player1_moves;
  emp::BitVector player2_moves;

public:
  CompetitionResult() = default;
  CompetitionResult(const CompetitionResult &) = default;
  CompetitionResult(const bool play1, const bool play2) {
    player1_moves.push_back(play1);
    player2_moves.push_back(play2);
  }

  CompetitionResult & operator=(const CompetitionResult &) = default;

  CompetitionResult & operator+=(CompetitionResult in) {
    player1_moves.Append(in.player1_moves);
    player2_moves.Append(in.player2_moves);
    return *this;
  }

  [[nodiscard]] CompetitionResult operator+(CompetitionResult in) const {
    in += *this;
    return in;
  }

  [[nodiscard]] size_t GetCooperate1() const { return player1_moves.CountOnes(); }
  [[nodiscard]] size_t GetCooperate2() const { return player2_moves.CountOnes(); }
  [[nodiscard]] size_t GetDefect1() const { return player1_moves.CountZeros(); }
  [[nodiscard]] size_t GetDefect2() const { return player1_moves.CountZeros(); }
  [[nodiscard]] emp::BitVector GetPlayer1Moves() const { return player1_moves; }
  [[nodiscard]] emp::BitVector GetPlayer2Moves() const { return player2_moves; }

  [[nodiscard]] size_t CountCoopCoop() const { return (player1_moves & player2_moves).CountOnes(); }
  [[nodiscard]] size_t CountCoopDefect() const { return (player1_moves & ~player2_moves).CountOnes(); }
  [[nodiscard]] size_t CountDefectCoop() const { return (~player1_moves & player2_moves).CountOnes(); }
  [[nodiscard]] size_t CountDefectDefect() const { return (~player1_moves & ~player2_moves).CountOnes(); }

  [[nodiscard]] int CalcScore1() const { return CountDefectDefect() + CountCoopCoop() * 3 + CountDefectCoop() * 5; }
  [[nodiscard]] int CalcScore2() const { return CountDefectDefect() + CountCoopCoop() * 3 + CountCoopDefect() * 5; }

  [[nodiscard]] size_t CountRetaliation1() { return ((~player1_moves << 1) & ~player2_moves).CountOnes(); }
  [[nodiscard]] size_t CountAggression1() { return ((~player1_moves << 1) & player2_moves).CountOnes(); }
  [[nodiscard]] size_t CountForgiveness1() { return ((player1_moves << 1) & ~player2_moves).CountOnes(); }
  [[nodiscard]] size_t CountReciprocity1() { return ((player1_moves << 1) & player2_moves).CountOnes(); }

  [[nodiscard]] size_t CountRetaliation2() { return ((~player2_moves << 1) & ~player1_moves).CountOnes(); }
  [[nodiscard]] size_t CountAggression2() { return ((~player2_moves << 1) & player1_moves).CountOnes(); }
  [[nodiscard]] size_t CountForgiveness2() { return ((player2_moves << 1) & ~player1_moves).CountOnes(); }
  [[nodiscard]] size_t CountReciprocity2() { return ((player2_moves << 1) & player1_moves).CountOnes(); }
};

class Competition {
private:
  const SummaryStrategy strategy1;
  const SummaryStrategy strategy2;
  const size_t num_rounds;

  // Indicates whether a hard defect will occur in the competition
  const bool hard_defect_toggle; 
  const size_t hard_defect_round;

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
    // TODO: Bug in GetAction() when memory is 0
    const bool action1 = hd_toggle ? DEFECT : strategy1.GetAction(mem1);
    const bool action2 = hd_toggle ? DEFECT : strategy2.GetAction(mem2);
    CompetitionResult result(action1, action2);
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
    // TODO: Bug at Compete() when strategies are memory 0
    CompetitionResult result;
    emp::BitVector mem1 = strategy1.GetStartState();
    emp::BitVector mem2 = strategy2.GetStartState();

    for (size_t i = 0; i < num_rounds; ++i) {
      if (hard_defect_toggle && i == hard_defect_round) {
        CompetitionResult r = Compete(mem1, mem2, true);
        result += r;
      }
      else { 
        CompetitionResult r = Compete(mem1, mem2, false); 
        result += r;
      } 
    }
    return result;
  }

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
