#pragma once

#include <cstdlib>
#include <getopt.h>
#include <optional>
#include <string>
#include <unordered_map>

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

namespace argparser {
  static std::string getUsage() {
#define X(name) #name "<number>"
    return "[" SIMULATION_VARIABLES "]" "endTime <number>";
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
        {"endTimme", optional_argument, 0, 0},
        {0, 0, 0, 0}};

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

        double value = std::atof(optarg);

        if (currentArg == "endTime") {
          config.endTime = value;
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

    return config;
  }
};
