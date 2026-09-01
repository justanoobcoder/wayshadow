#include "wayshadow/app.hpp"
#include "wayshadow/config.hpp"

#include <iostream>

int main(int argc, char* argv[]) {
    const auto cli = wayshadow::CliOptions::parse(argc, argv);

    switch (cli.action) {
    case wayshadow::CliOptions::Action::ShowHelp:
        wayshadow::CliOptions::print_usage(argv[0]);
        return 0;
    case wayshadow::CliOptions::Action::ShowVersion:
        wayshadow::CliOptions::print_version();
        return 0;
    case wayshadow::CliOptions::Action::Error:
        std::cerr << "Error: " << cli.error_message << "\n\n";
        wayshadow::CliOptions::print_usage(argv[0]);
        return 1;
    case wayshadow::CliOptions::Action::Run:
        break;
    }

    wayshadow::Application app(cli.config);
    return app.run();
}
