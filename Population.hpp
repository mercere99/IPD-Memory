#pragma once

#include "../Empirical/include/emp/base/vector.hpp"
#include "../Empirical/include/emp/datastructs/UnorderedIndexMap.hpp"
#include "../Empirical/include/emp/math/Random.hpp"

#include "Competition.hpp"
#include "Strategy.hpp"

class Population {
private:
  emp::vector<size_t> org_counts;     // Map of strategy ID to how many are in population.
  emp::vector<SummaryStrategy> info;  // More details about strategies being used.
  size_t generation = 0;
  CompetitionManager manager;

public:
  size_t GetSize() const {
    size_t total = 0;
    for (size_t count : org_counts) total += count;
    return total;
  }

  size_t GetGeneration() const { return generation; }

  void AddOrg(SummaryStrategy org, size_t count=1) {
    size_t id = org.GetID();
    if (org_counts.size() <= id) {
      info.resize(id+1);
      org_counts.resize(id+1);
    }
    info[id] = org;
    org_counts[id] += count;
  }

  double CalcFitness(size_t strategy_id) const {
    double fitness = 0.0;
    for (size_t opponent_id = 0; opponent_id < org_counts.size(); ++opponent_id) {
      if (org_counts[opponent_id] == 0) continue; // Skip opponent strategies not in use.
      const size_t opponent_count = org_counts[opponent_id];
      fitness += manager.Compete(strategy_id, opponent_id, 64).GetScore1() * opponent_count;
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
    for (size_t i=0; i < pop_size; ++i) {  // New pop should be same size as old pop.
      size_t id = index_map.Index(random.GetDouble(index_map.GetWeight()));
      // MUTATE?????
      ++next_counts[id];
    }

    std::swap(org_counts, next_counts);
  }

  void Print() const {
    for (size_t strategy_id = 0; strategy_id < org_counts.size(); ++strategy_id) {
      if (org_counts[strategy_id] == 0) continue; // Skip strategies not in use.
      SummaryStrategy strategy = info[strategy_id];

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
