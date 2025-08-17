// DEVELOPER NOTES:
// - Strategies are assumed to have SOME memory; index 0 is used without checking.

#include <iostream>
#include <string>
#include <fstream>

#include "emp/config/SettingsManager.hpp"
#include "emp/io/io_utils.hpp"
#include "emp/math/Random.hpp"

#include "Competition.hpp"
#include "Population.hpp"
#include "Strategy.hpp"

int main(int argc, char * argv[])
{
  // For now, if there is an argument, it is the name of the config file to use.
  emp::String config_name = "IPD.cfg";
  if (argc > 1) config_name = argv[1];

  emp::SettingsManager settings;
  Population pop;
  pop.SetupConfig(settings);

  std::map<emp::String, SummaryStrategy> strategy_map;

  // Add a "Strategy" keyword to specify new strategies right from the config file.
  settings.AddKeyword("Strategy",
    [&strategy_map](emp::vector<emp::String> args){
      if (args.size() < 1) { emp::notify::Error("Must specify NAME when defining a strategy."); abort(); }
      emp::String name = args[0];
      if (args.size() < 2) { emp::notify::Error("Must specify DECISION_LIST when defining strategy '", name, "'."); abort(); }
      if (!args[1].IsComposedOf("01")) { emp::notify::Error("DECISION_LIST for strategy '", name, "' must be a binary sequence."); abort(); }
      emp::String decision_list = args[1];
      size_t mem_size = decision_list.size() - 1;
      emp::String start_memory = "";
      if (mem_size > 0) {
        if (args.size() < 3) { emp::notify::Error("Must specify STARTING_MEMORY when defining strategy '", name, "'."); abort(); }
        if (!args[2].IsComposedOf("01")) { emp::notify::Error("STARTING_MEMORY for strategy '", name, "' must be a binary sequence."); abort(); }
        if (args[2].size() != mem_size) { emp::notify::Error("STARTING_MEMORY size for '", name, "' must be ", mem_size, "."); abort(); }
        start_memory = args[2];
      }
      strategy_map.emplace(name, SummaryStrategy{start_memory, decision_list, name});
      emp::PrintLn("Defined strategy '", name, "'.");
    },
    "Add strategy with NAME DECISION_LIST STARTING_MEMORY\nSkip STARTING_MEMORY if empty.");

  // Add a "Inject" keyword to add specific strategies into the population.
  settings.AddKeyword("Inject",
    [&strategy_map, &pop](emp::vector<emp::String> args){
      if (args.size() < 1) { emp::notify::Error("Must specify NAME of strategy to Inject into population."); abort(); }
      emp::String name = args[0];
      if (!strategy_map.contains(name)) { emp::notify::Error("Strategy '", name, "' Unknown!  Cannot Inject."); abort(); }
      if (args.size() < 2) { emp::notify::Error("Must specify HOW MANY of strategy '", name, "' to Inject."); abort(); }
      if (!args[1].OnlyDigits()) { emp::notify::Error("Number of strategy '", name, "' to inject must be a whole number."); abort(); }
      size_t inject_count = args[1].AsULL();
      pop.AddOrg(strategy_map[name], inject_count);
      emp::PrintLn("Injected ", inject_count, " of strategy '", name, "'.");
    },
    "Add strategy with NAME DECISION_LIST STARTING_MEMORY\nSkip STARTING_MEMORY if empty.");

  settings.AddKeyword("Run",
    [&pop](emp::vector<emp::String> args){
      if (args.size() < 1) { emp::notify::Error("Must specify random seed to use."); abort(); }
      if (!args[0].OnlyDigits()) { emp::notify::Error("Seed for a Run must be numerical."); abort(); }
      size_t start_seed = args[0].AsULL();
      size_t end_seed = start_seed + 1;
      if (args.size() > 1) {
        if (!args[1].OnlyDigits()) { emp::notify::Error("End seed for a Run must be numerical."); abort(); }
        end_seed = args[1].AsULL();
        if (end_seed <= start_seed) {
          emp::notify::Error("End seed for a run (", end_seed,") must be greater than start seed (", start_seed, ").");
          abort();
        }
      }
      for (size_t cur_seed = start_seed; cur_seed < end_seed; ++cur_seed) {
        emp::PrintLn("=== Starting Run with seed ", cur_seed, " ===");
        emp::Random random(cur_seed);
        Population test_pop = pop; // Keep the original population with base stats.
        test_pop.Run(random);
        test_pop.ExportHistory("history" + std::to_string(cur_seed));
      }
    },
    "Add strategy with NAME DECISION_LIST STARTING_MEMORY\nSkip STARTING_MEMORY if empty.");

  // settings.SetVerbose();
  bool success = settings.Load(config_name);
  if (!success) {
    emp::PrintLn(settings.GetError());
    exit(1);
  }

  // pop.MultiRun();

}
