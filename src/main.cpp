#include "keypop/app.hpp"
#include "keypop/config.hpp"

#include <iostream>

int main(int argc, char* argv[]) {
    const auto cli = keypop::CliOptions::parse(argc, argv);

    switch (cli.action) {
    case keypop::CliOptions::Action::ShowHelp:
        keypop::CliOptions::print_usage(argv[0]);
        return 0;
    case keypop::CliOptions::Action::ShowVersion:
        keypop::CliOptions::print_version();
        return 0;
    case keypop::CliOptions::Action::Error:
        std::cerr << "Error: " << cli.error_message << "\n\n";
        keypop::CliOptions::print_usage(argv[0]);
        return 1;
    case keypop::CliOptions::Action::Run:
        break;
    }

    keypop::Application app(cli.config);
    return app.run();
}
