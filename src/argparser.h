#pragma once

#include <string>
#include <cstdint>
#include <getopt.h>
#include <system_error>
#include <optional>

struct InitialSimulationConfiguration {
  double initialMet;
  double initialAdoMet;
  double initialAdoHcy;
  double initialHcy;
  double Metin;
};

struct SimulationConfigurtaion {
  // this block should be either fully initialized or uninitialized completely
  // create an optional initialSimulationConfiguration
  std::optional<InitialSimulationConfiguration> initialSimunlationConfiguration;
  std::optional<double> endTime;
};

namespace argparser {

    /* SimulationConfigurtaion parseArguments(int argc, const char **argv) { */
    /**/
    /*     SimulationConfigurtaion args{}; */
    /*     int option; */
    /*     int currentIdx = 0; */
    /*     while ((option = getopt(argc, (char *const *) (argv), "rx6s:p:")) != -1) { */
    /*         currentIdx += 1; */
    /*         switch (option) { */
    /*             case 'r': */
    /*                 if (args.recursionRequested) { */
    /*                     ThrowUsageMessage("Recursion Desired (-r) flag can be specified only once"); */
    /*                 } */
    /*                 args.recursionRequested = true; */
    /*                 break; */
    /*             case 'x': */
    /*                 if (args.reverseQuery) { */
    /*                     ThrowUsageMessage("Reversed query (-x) flag can be specified only once"); */
    /*                 } */
    /*                 args.reverseQuery = true; */
    /*                 break; */
    /*             case '6': */
    /*                 if (args.queryTypeAAAA) { */
    /*                     ThrowUsageMessage("AAAA query (-6) flag can be specified only once"); */
    /*                 } */
    /*                 args.queryTypeAAAA = true; */
    /*                 break; */
    /*             case 's': */
    /*                 if (!args.server.empty()) { */
    /*                     ThrowUsageMessage("Server (-s) parameter can be specified only once"); */
    /*                 } */
    /*                 args.server = optarg; */
    /*                 break; */
    /*             case 'p': */
    /*                 if (args.port) { */
    /*                     ThrowUsageMessage("Port (-p) parameter can be specified only once"); */
    /*                 } */
    /*                 args.port = static_cast<uint16_t>(std::stoi(optarg)); */
    /*                 break; */
    /*             case '?': */
    /*             default: */
    /*                 ThrowUsageMessage("unknown option \"" + std::string(argv[currentIdx]) + "\""); */
    /*                 break; */
    /*         } */
    /*     } */
    /**/
    /*     if (args.server.empty()) { */
    /*         ThrowUsageMessage("Server -s parameter must be specified"); */
    /*     } */
    /**/
    /*     if (optind == argc - 1) { */
    /*         args.address = argv[optind++]; */
    /*     } else { */
    /*         ThrowUsageMessage("Too many arguments"); */
    /*     } */
    /**/
    /*     return args; */
    /* } */
}

