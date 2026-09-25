#include <iostream>

#include "wayshadow/app.hpp"
#include "wayshadow/config.hpp"

int main(int argc, char* argv[]) {
  const auto cli = wayshadow::CliOptions::Parse(argc, argv);

  switch (cli.action) {
    case wayshadow::CliOptions::Action::kShowHelp:
      wayshadow::CliOptions::PrintUsage(argv[0]);
      return 0;
    case wayshadow::CliOptions::Action::kShowVersion:
      wayshadow::CliOptions::PrintVersion();
      return 0;
    case wayshadow::CliOptions::Action::kError:
      std::cerr << "Error: " << cli.error_message << "\n\n";
      wayshadow::CliOptions::PrintUsage(argv[0]);
      return 1;
    case wayshadow::CliOptions::Action::kRun:
      break;
  }

  wayshadow::Application app(cli.config);

  return app.Run();
}
