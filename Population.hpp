#pragma once

#include "../Empirical/include/emp/base/vector.hpp"
#include "../Empirical/include/emp/datastructs/UnorderedIndexMap.hpp"
#include "../Empirical/include/emp/math/Random.hpp"

#include "Competition.hpp"
#include "Strategy.hpp"
#include "Logger.hpp"

class Population {
private:
  emp::vector<size_t> org_counts;              // Map of strategy ID to how many are in population.
  mutable emp::vector<SummaryStrategy> strategy_info;  // More details about strategies being used.
  size_t generation = 0;
  CompetitionManager manager;

  const size_t max_generations = 10000;
  const size_t num_rounds = 64;
  const double mut_prob = 0.01;
  const double memory_cost = 0.1;

  const bool hard_defect_toggle = true;
  const size_t hard_defect_round = 32;

public:
  size_t GetSize() const {
    size_t total = 0;
    for (size_t count : org_counts) total += count;
    return total;
  }

  // [[nodiscard]] auto & GetStrategy(this auto self, size_t strategy_id) {
  //   if (strategy_id >= self.strategy_info.size()) {
  //     // Otherwise we need to increase the number of strategies we have information about.
  //     const size_t old_size = self.strategy_info.size();
  //     self.strategy_info.resize(strategy_id+1);
  //     for (size_t id = old_size; id < self.strategy_info.size(); ++id) {
  //       self.strategy_info[id] = SummaryStrategy{id};
  //     }
  //   }
  //   return self.strategy_info[strategy_id];
  // }

  [[nodiscard]] SummaryStrategy & GetStrategy(size_t strategy_id) {
    if (strategy_id >= strategy_info.size()) {
      // Otherwise we need to increase the number of strategies we have information about.
      const size_t old_size = strategy_info.size();
      strategy_info.resize(strategy_id+1);
      for (size_t id = old_size; id < strategy_info.size(); ++id) {
        strategy_info[id] = SummaryStrategy{id};
      }
    }
    return strategy_info[strategy_id];
  }

  [[nodiscard]] const SummaryStrategy & GetStrategy(size_t strategy_id) const {
    if (strategy_id >= strategy_info.size()) {
      // Otherwise we need to increase the number of strategies we have information about.
      const size_t old_size = strategy_info.size();
      strategy_info.resize(strategy_id+1);
      for (size_t id = old_size; id < strategy_info.size(); ++id) {
        strategy_info[id] = SummaryStrategy{id};
      }
    }
    return strategy_info[strategy_id];
  }

  [[nodiscard]] size_t GetGeneration() const { return generation; }

  void AddOrg(const SummaryStrategy & org, size_t count=1) {
    // emp::PrintLn("Adding org '", org.GetName(), "'.");
    size_t id = org.GetID();
    // emp::PrintLn("...id: ", id);
    GetStrategy(id) = org;
    // emp::PrintLn("...after get: ", org.GetName());
    // emp::PrintLn("...internal:  ", GetStrategy(id).GetName());
    if (org_counts.size() <= id) org_counts.resize(id+1);
    org_counts[id] += count;
  }

  double CalcFitness(size_t strategy_id) const {
    double fitness = 0.0;
    for (size_t opponent_id = 0; opponent_id < org_counts.size(); ++opponent_id) {
      if (org_counts[opponent_id] == 0) continue; // Skip opponent strategies not in use.
      // Determine # of opponents; note that we should not compete with self.
      const size_t opponent_count = org_counts[opponent_id] - (strategy_id == opponent_id);
      const double base_fitness = manager
                                  .Compete(strategy_id, opponent_id, num_rounds, hard_defect_toggle, hard_defect_round)
                                  .GetScore1();
      const double penalty = GetStrategy(strategy_id).GetMemorySize() * memory_cost;
      fitness +=  (base_fitness - penalty) * opponent_count;
    }
    return fitness;
  }

  void Update(emp::Random & random) {
    ++generation;
    
    // Build an index map of weights proportional to the probability of each strategy reproducing.
    emp::UnorderedIndexMap index_map(org_counts.size());
    for (size_t strategy_id = 0; strategy_id < org_counts.size(); ++strategy_id) {
      if (org_counts[strategy_id] == 0) continue; // Skip strategies not in use.
      index_map[strategy_id] = org_counts[strategy_id] * CalcFitness(strategy_id);
    }

    // Choose who replicates and put them in a new population.
    const size_t pop_size = GetSize();
    emp::vector<size_t> next_counts(org_counts.size());
    for (size_t i = 0; i < pop_size; ++i) {  // New pop should be same size as old pop.
      // Select
      size_t id = index_map.Index(random.GetDouble(index_map.GetWeight()));

      // Mutate?
      if (random.P(mut_prob)) {
        const SummaryStrategy mut_strategy = GetStrategy(id).Mutate(random);
        id = mut_strategy.GetID();
        if (id >= next_counts.size()) {
          next_counts.resize(id+1);
        }
        if (GetStrategy(id).GetName() == "none") GetStrategy(id) = mut_strategy;
     }

      ++next_counts[id];
    }

    std::swap(org_counts, next_counts);
  }

  void Run(emp::Random & random) {
    for (size_t update = 0; update <= max_generations; ++update) {
      Update(random);
      if (update % 100 == 0) {
        emp::PrintLn("Update ", update, ":");
        Print();
      }
    }
  }

  void Print() const {
    for (size_t strategy_id = 0; strategy_id < org_counts.size(); ++strategy_id) {
      if (org_counts[strategy_id] == 0) continue; // Skip strategies not in use.
      const SummaryStrategy & strategy = GetStrategy(strategy_id);

      std::cout << "Strategy " << strategy_id << ":"
                << "  Count=" << org_counts[strategy_id]
                << "  Fitness=" << CalcFitness(strategy_id)
                << "  StartState=" << strategy.GetStartState()
                << "  DecisionList=" << strategy.GetDecisionList()
                << "  Name=" << strategy.GetName()
                << "\n";
    }
  }
};
