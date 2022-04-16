#ifndef PNTR_CLI_HPP_
#define PNTR_CLI_HPP_

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <span>

#include <command.hpp>

namespace painter::cli {

    class CommandLine {
        private:
            std::shared_ptr<Command> root_;
            
            CommandLine(const CommandLine&) = delete;
            CommandLine& operator=(const CommandLine&) = delete;
        public:
            CommandLine(std::string const&, Action const&);
            
            int run(int argc, char* argv[]);
            int run(std::span<char*>);

            CommandLine(CommandLine&&) = default;
            CommandLine& operator=(CommandLine&&) = default;
            ~CommandLine() = default;
    };

}

#endif /* PNTR_CLI_HPP_ */