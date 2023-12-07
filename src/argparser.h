#pragma once

#include <cstdlib>
#include <getopt.h>
#include <optional>
#include <unordered_map>
#include <string>


#define SIMULATION_VARIABLES                                                   \
  X(initialMet)                                                                \
  X(initialAdoMet)                                                             \
  X(initialAdoHcy)                                                             \
  X(initialHcy)                                                                \
  X(metin)

struct InitialSimulationConfiguration {
#define X(name) double name;
  SIMULATION_VARIABLES
#undef X
};

struct SimulationConfiguration {
  std::optional<InitialSimulationConfiguration> initialSimulationConfiguration;
  std::optional<double> endTime;
};

#define X(name) .name = 0.5,
const InitialSimulationConfiguration DEFAULT_SIMULATION_CONFIGURATION = {
  SIMULATION_VARIABLES
};
#undef X

#define END_TIME_STR "endTime"
const double MIN_END_TIME = 1;

namespace argparser {
  static std::string getUsage() {
#define X(name) "--"#name " number "
    return (
      "[ " SIMULATION_VARIABLES "]" " [--" END_TIME_STR " number]\n"
      "  simulation parameters: µMol\n"
      "  endTime: (min " + std::to_string(MIN_END_TIME) + ")\n"
    );
#undef X
  }

  static SimulationConfiguration parseArguments(int argc, char **argv) {
    SimulationConfiguration config;
    std::unordered_map<std::string, bool> setFlags;
    std::string currentArg;

    static struct option longOptions[] = {
#define X(name) {#name, required_argument, 0, 0},
      SIMULATION_VARIABLES
#undef X
      {END_TIME_STR, required_argument, 0, 0},
      {0, 0, 0, 0}
    };

    int optionIndex = 0;
    while (true) {
      int c = getopt_long(argc, argv, "", longOptions, &optionIndex);

      if (c == -1) {
        break;
      }

      if (c == 0) {
        currentArg = longOptions[optionIndex].name;
        if (setFlags[currentArg]) {
          throw std::runtime_error("Error: Argument '" + currentArg + "' initialized more than once.");
        }
        setFlags[currentArg] = true;

        char *anychars = NULL;
        const double value = std::strtod(optarg, &anychars);

        if (*anychars != '\0') {
          throw std::runtime_error("Error: Argument '" + currentArg + "' is not a number. Found \"" + anychars + "\"");
        }

        if (currentArg == END_TIME_STR) {
          config.endTime = value;
          if (config.endTime < MIN_END_TIME) {
            throw std::runtime_error("Error: " END_TIME_STR " must be greater than " + std::to_string(MIN_END_TIME));
          }
          continue;
        }

        if (!config.initialSimulationConfiguration) {
          config.initialSimulationConfiguration = InitialSimulationConfiguration{};
        }
        
        // setting initial simulations configuration arguments if found
#define X(name)                                                                \
  if (currentArg == #name) {                                                   \
    config.initialSimulationConfiguration->name = value;                       \
  } else
        SIMULATION_VARIABLES { /*intentionally empty to close the chain */ }
#undef X


      } else {
        throw std::runtime_error(
            "Error: Unknown argument or argument with no value.");
      }
    }

#define X(name) if (!setFlags[#name])
    // Checking if all required initialSimulationConfiguration fields are set
    if (config.initialSimulationConfiguration) {
      SIMULATION_VARIABLES {
        throw std::runtime_error(
          "Error: Some fields of InitialSimulationConfiguration are not initialized.");
      }
    }
#undef X
    // if the length of inintialSimulationConfiguration + optional (endTime) !== argc - 1 then throw error

#define X(name) #name,
    const char *(iscStrings[]) = { SIMULATION_VARIABLES };
#undef X
    const auto iscLength = config.initialSimulationConfiguration ? sizeof(iscStrings) / sizeof(char*) : 0;

    const auto numProcessedArgs = (iscLength + !!config.endTime.has_value()) * 2;
    const bool areAnyExtraArgs = numProcessedArgs != argc - 1;
    if (areAnyExtraArgs) {
      throw std::runtime_error(
        "Error: Wrong number of arguments."
      );
    }

    return config;
  }
};
