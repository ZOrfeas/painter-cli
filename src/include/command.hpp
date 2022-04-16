#ifndef COMMAND_HPP_
#define COMMAND_HPP_

#include <string>
#include <unordered_map>
#include <span>
#include <functional>

namespace painter::cli {
    class Command;
    using Action = std::function<int(Command const&, std::span<char*>)>;

    class Command {
        private:
            std::string name_;
            std::string description_;  

            std::shared_ptr<Command> parent_;
            std::unordered_map<std::string, std::shared_ptr<Command>> subcommands_;    
    };
}

#endif /* COMMAND_HPP_ */