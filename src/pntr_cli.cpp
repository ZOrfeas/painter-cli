#include <pntr_cli.hpp>

namespace painter::cli {

CommandLine::CommandLine(
    std::string const& description,
    Action const& action
): root_(make_shared<Command>(description, action)) {}

int CommandLine::run(std::span<char*> args) {}

}