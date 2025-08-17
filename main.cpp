// DEVELOPER NOTES:
// - Strategies are assumed to have SOME memory; index 0 is used without checking.

#include <iostream>
#include <string>
#include <fstream>

#include "emp/io/io_utils.hpp"
#include "emp/math/Random.hpp"

#include "Competition.hpp"
#include "Population.hpp"
#include "Strategy.hpp"

int main()
{
  // Only tit-for-tat with grudge is possible! (No way to know if defect was recent)
  emp::BitVector all_cooperate_memory{COOPERATE, COOPERATE, COOPERATE};

  SummaryStrategy always_coop{"", "1", "AC"};
  SummaryStrategy always_defect{"", "0", "AD"};
  SummaryStrategy tit_for_tat("1", "10", "tit-for-tat");
  SummaryStrategy majority_response{"110", "1100", "majority"};

  SummaryStrategy mem_1_tft("1", "10", "tit-for-tat");
  SummaryStrategy mem_1_ad("1", "00", "AD");

  emp::Random random;

  Population pop;
  pop.AddOrg(tit_for_tat, 500); // Strategy 57
  pop.AddOrg(majority_response, 500); // Strategy 69
  pop.AddOrg(always_coop, 500); // Strategy 169
  pop.AddOrg(always_defect, 500); // Strategy 49

  // pop.AddOrg(mem_1_tft, 4); // Strategy 5
  // pop.AddOrg(mem_1_ad, 496); // Strategy 49

  // std::cout << "Start state:\n";
  // pop.Print();

  pop.Run(random);
  // pop.MultiRun();


  // // --- TODO: DEBUGGING MEMORY 0 ---
  // SummaryStrategy mem_0_ad(0, "AD");
  // SummaryStrategy mem_0_ac(1, "AC");

  // Competition comp{mem_0_ac, mem_0_ad, 64, false, 31};
  // auto result = comp.Run(); // Here's where it's stuck
  // auto p1_moves = result.GetPlayer1Moves();
  // auto p2_moves = result.GetPlayer2Moves();
  // emp::PrintLn("Player 1 Moves: ", p1_moves);
  // emp::PrintLn("Player 2 Moves: ", p2_moves);




  // Majority Response - Hard Defect
  // Competition comp{majority_response, majority_response, 64, true, 31};
  // auto result = comp.Run();
  // auto p1_moves = result.GetPlayer1Moves();
  // auto p2_moves = result.GetPlayer2Moves();
  // emp::PrintLn("Player 1 Moves: ", p1_moves);
  // emp::PrintLn("Player 2 Moves: ", p2_moves);

  // Majority Response - Ideal
  // Competition comp{majority_response, majority_response, 64, false, 31};
  // auto result = comp.Run();
  // auto p1_moves = result.GetPlayer1Moves();
  // auto p2_moves = result.GetPlayer2Moves();
  // emp::PrintLn("Player 1 Moves: ", p1_moves);
  // emp::PrintLn("Player 2 Moves: ", p2_moves);

  // Tit For Tat - Hard Defect
  // Competition comp{tit_for_tat, tit_for_tat, 64, true, 31};
  // auto result = comp.Run();
  // auto p1_moves = result.GetPlayer1Moves();
  // auto p2_moves = result.GetPlayer2Moves();
  // emp::PrintLn("Player 1 Moves: ", p1_moves);
  // emp::PrintLn("Player 2 Moves: ", p2_moves);

  // Tit For Tat - Ideal
  // Competition comp{tit_for_tat, tit_for_tat, 64, false, 31};
  // auto result = comp.Run();
  // auto p1_moves = result.GetPlayer1Moves();
  // auto p2_moves = result.GetPlayer2Moves();
  // emp::PrintLn("Player 1 Moves: ", p1_moves);
  // emp::PrintLn("Player 2 Moves: ", p2_moves);

  
  // // Save bit strings as long format table
  // emp::String p1_moves_str = p1_moves.ToArrayString();
  // emp::String p2_moves_str = p2_moves.ToArrayString();
  // emp::PrintLn(p1_moves_str);
  // emp::PrintLn(p2_moves_str);

  // // std::ofstream ofs("MR_HD.csv");
  // // std::ofstream ofs("MR_Ideal.csv");
  // // std::ofstream ofs("TFT_HD.csv");
  // std::ofstream ofs("TFT_Ideal.csv");
  // if (ofs.is_open()) {
  //   ofs << "Round,Player,Move\n";
  //   for (size_t round = 0; round < 64; ++round) {
  //     char move1 = p1_moves_str[round] == '1' ? 'C' : 'D';
  //     char move2 = p2_moves_str[round] == '1' ? 'C' : 'D';
  //     ofs << round + 1 << "," << "P1" << "," << move1 << "\n";
  //     ofs << round + 1 << "," << "P2" << "," << move2 << "\n";
  //   }
  // }

  // auto results = comp.GetResults();
  // for (auto & result : results) {
  //   emp::PrintLn("Cooperate1 = ", result.GetCooperate1(),"; Cooperate2 = ", result.GetCooperate2());
  //   emp::PrintLn("\nDefect1 = ", result.GetDefect1(),"; Defect2 = ", result.GetDefect2());
  // }
  

  // Competition comp{tit_for_tat, always_defect};
  // Competition comp{tit_for_tat, tit_for_tat};
  // Competition comp{tit_for_tat, always_coop};
  // Competition comp{always_coop, always_defect, NUM_ROUNDS;
  // auto result = comp.Run();
  // emp::PrintLn("Score1 = ", result.GetScore1(), "; Score2 = ", result.GetScore2());

  // SummaryStrategy test_strat(3, 77);
  // emp::PrintLn("STRAT_ID = ", test_strat.GetID());
}
