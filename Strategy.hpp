// If memory size = MEM_SIZE
// We need to store MEM_SIZE+1 bits for decisions + MEM_SIZE bits for starting memory.
// That means there are 2^(2*MEM_SIZE+1) possible genomes.
// 0 memory bits -> 1 bit total   -> 2 genomes     CUMULATIVE: 2
// 1 memory bit  -> 3 bits total  -> 8 genomes     CUMULATIVE: 10
// 2 memory bits -> 5 bits total  -> 32 genomes    CUMULATIVE: 42
// 3 memory bits -> 7 bits total  -> 128 genomes   CUMULATIVE: 150
// 4 memory bits -> 9 bits total  -> 512 genomes   CUMULATIVE: 662
// 5 memory bits -> 11 bits total -> 2048 genomes  CUMULATIVE: 2710

#pragma once

#include <string>

#include "../Empirical/include/emp/bits/Bits.hpp"
#include "../Empirical/include/emp/math/math.hpp"

static constexpr bool COOPERATE = true;
static constexpr bool DEFECT = false;

constexpr size_t CountStrategies(size_t mem_bits) {
  emp_assert(mem_bits < 10, mem_bits);
  size_t result = 2;
  while (mem_bits > 0) {
    result *= 4;
    --mem_bits;
  }
  return result;
}

constexpr size_t CalcFirstStrategyID(size_t mem_bits) {
  size_t total = 0;
  while (mem_bits > 0) {
    --mem_bits;
    total += CountStrategies(mem_bits);
  }
  return total;
}


constexpr size_t IDToMemoryBits(size_t strategy_id) {
  size_t mem_bits = 0;
  size_t num_strats = CountStrategies(0);
  while (strategy_id >= num_strats) {
    strategy_id -= num_strats;
    num_strats = CountStrategies(++mem_bits);
  }
  return mem_bits;
}

constexpr size_t IDToMemoryDecisionList(size_t strategy_id) {
  size_t mem_bits = 0;
  size_t num_strats = CountStrategies(0);
  while (strategy_id >= num_strats) {
    strategy_id -= num_strats;
    num_strats = CountStrategies(++mem_bits);
  }
  return strategy_id;
}

class SummaryStrategy {
private:
  emp::BitVector start_state{};   // Initial memory
  emp::BitVector decision_list{}; // Choices based on num defects by opponent.
  std::string name;

public:
  SummaryStrategy(emp::BitVector start_state,
                  emp::BitVector decision_list,
                  std::string name="none")
    : start_state(start_state), decision_list(decision_list), name(name) {}

  SummaryStrategy(size_t strategy_id=0, std::string name="none") : name(name) {
    size_t mem_bits = 0;
    size_t num_strats = CountStrategies(0);
    while (strategy_id >= num_strats) {
      strategy_id -= num_strats;
      num_strats = CountStrategies(++mem_bits);
    }

    emp_assert(strategy_id < CountStrategies(mem_bits));
    // first mem_bits is starting memory
    if (mem_bits > 0) start_state.Import(strategy_id & emp::MaskLow(mem_bits), mem_bits);

    // next mem_bits+1 is decision_list
    decision_list.Import((strategy_id >> mem_bits) & emp::MaskLow(mem_bits+1), mem_bits+1);
  }
  SummaryStrategy(const SummaryStrategy &) = default;

  SummaryStrategy & operator=(const SummaryStrategy &) = default;

  [[nodiscard]] auto operator==(const SummaryStrategy & in) const {
    return start_state == in.start_state && decision_list == in.decision_list;
  }

  [[nodiscard]] auto operator<(const SummaryStrategy & in) const {
    if (start_state < in.start_state) return true;
    if (start_state > in.start_state) return false;
    return decision_list < in.decision_list;
  }
  [[nodiscard]] auto operator>(const SummaryStrategy & in) const { return in < *this; }
  [[nodiscard]] auto operator>=(const SummaryStrategy & in) const { return !(*this < in); }
  [[nodiscard]] auto operator<=(const SummaryStrategy & in) const { return !(in < *this); }


  [[nodiscard]] const std::string & GetName() const { return name; }
  [[nodiscard]] const emp::BitVector & GetStartState() const { return start_state; }
  [[nodiscard]] const emp::BitVector & GetDecisionList() const { return decision_list; }

  [[nodiscard]] size_t GetMemorySize() const { return start_state.size(); }
  [[nodiscard]] size_t GetID() const {
    size_t offset = CalcFirstStrategyID(GetMemorySize());
    size_t start_state_id = start_state.GetUInt32(0);
    size_t decision_id = decision_list.GetUInt32(0);
    return offset + start_state_id + (decision_id << start_state.size());
  }

  [[nodiscard]] bool GetAction(const emp::BitVector & mem) const {
    emp_assert(mem.size() == start_state.size());
    size_t num_opponent_defects = mem.CountZeros();
    return decision_list[num_opponent_defects];
  }
};
