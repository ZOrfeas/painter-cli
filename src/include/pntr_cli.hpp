#ifndef PNTR_CLI_HPP_
#define PNTR_CLI_HPP_

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <span>

namespace painter::cli {
class Command;
class CommandLine;

using Action = std::function<void(Command const&, std::vector<std::string> const&)>;
template<typename T>std::string to_string(T value);

template<typename T>
concept HasToString = requires(T t) {
    { painter::cli::to_string<T>(t) } -> std::same_as<T>;
};


class CommandLine {
    private:
        std::shared_ptr<Command> root_;
        CommandLine(const CommandLine&) = delete;
        CommandLine& operator=(const CommandLine&) = delete;
    public:
        CommandLine(std::string const&, Action const&);

        int run(std::span<char*>);

        CommandLine(CommandLine&&) = default;
        CommandLine& operator=(CommandLine&&) = default;
        ~CommandLine() = default;
};

class Command {
    private:
        std::string name_;
        std::string description_;
        Action action_;
        
        

        std::unordered_map<std::string, std::shared_ptr<Command>> subcommands_;
        std::shared_ptr<Command> parent_;
    public:

};


}

#endif /* PNTR_CLI_HPP_ */