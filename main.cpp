// If 
//
// DEVELOPER NOTES:
// - Strategies are assumed to have SOME memory; index 0 is used without checking.

#include <iostream>
#include <string>

#include "../Empirical/include/emp/io/io_utils.hpp"
#include "../Empirical/include/emp/math/Random.hpp"

#include "Competition.hpp"
#include "Population.hpp"
#include "Strategy.hpp"

constexpr size_t NUM_ROUNDS = 64;

int main()
{
  // Only tit-for-tat with grudge is possible! (No way to know if defect was recent)
  emp::BitVector all_cooperate_memory{COOPERATE, COOPERATE, COOPERATE};

  SummaryStrategy tit_for_tat(all_cooperate_memory, "1000", "tit-for-tat");
  SummaryStrategy majority_response{"110", "1100", "majority"};
  SummaryStrategy always_coop{all_cooperate_memory, "1111", "AC"};
  SummaryStrategy always_defect{all_cooperate_memory, "0000", "AD"};

  emp::Random random;

  Population pop;
  pop.AddOrg(tit_for_tat, 500);
  pop.AddOrg(majority_response, 500);
  pop.AddOrg(always_coop, 500);
  pop.AddOrg(always_defect, 500);

  std::cout << "Start state:\n";
  pop.Print();

  for (size_t update = 0; update < 10; ++update) {
    pop.Update(random);
    emp::PrintLn("Update ", update, ":");
    std::cout << "End state:\n";
    pop.Print();
  }


  // Competition comp{tit_for_tat, always_defect};
  // Competition comp{tit_for_tat, tit_for_tat};
  // Competition comp{tit_for_tat, always_coop};
  // Competition comp{always_coop, always_defect, NUM_ROUNDS;
  // auto result = comp.Run();
  // emp::PrintLn("Score1 = ", result.GetScore1(), "; Score2 = ", result.GetScore2());


  // SummaryStrategy test_strat(3, 77);
  // emp::PrintLn("STRAT_ID = ", test_strat.GetID());
}
