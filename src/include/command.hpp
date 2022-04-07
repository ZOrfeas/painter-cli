/**
 * @file command.hpp
 * @author Zografos Orfeas
 * @brief 
 * @version 0.1
 * @date 2022-04-01
 */

#ifndef COMMAND_HPP_
#define COMMAND_HPP_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <optional>
#include <functional>
#include <concepts>

#include <flag.hpp>

namespace pnt_cli {
    class Command;
    using Action = std::function<int(const Command*, std::vector<std::string>)>;

    std::shared_ptr<Command> makeCommand(
        const std::string& name,
        const std::string& description,
        Action action
    );

    class Command : public std::enable_shared_from_this<Command> {
        private:
            std::string name_;
            std::string description_;
            Action action_;
            FlagSet persistent_flags_;
            FlagSet local_flags_;
            
            std::map<std::string, std::shared_ptr<Command>> subcommands_;
            std::shared_ptr<Command> parent_;

            template<FlagType T>
            FlagImpl<T>* find_persistent_flag(const std::string&) const;

            template<FlagType T> 
            FlagImpl<T>* find_flag(const std::string&) const;

            Command() = delete;
            Command(const std::string& name, const std::string& description, Action action) 
                : name_(name), description_(description), action_(action) {}

            friend std::shared_ptr<Command> makeCommand(
                const std::string& name,
                const std::string& description,
                Action action
            ) {
                return std::shared_ptr<Command>(
                    new Command(name, description, action)
                );
            }
        public:
            ~Command() = default;
            
            bool hasParent() const;
            bool hasSubcommands() const;
            bool hasFlags() const;

            std::shared_ptr<Command> addSubcommand(const std::string&, const std::string&, Action);
            std::shared_ptr<Command> addSubcommand(std::shared_ptr<Command>);

            template<FlagType T>
            std::optional<T> getFlag(const std::string&) const;

            template<FlagType T>
            bool setFlag(const std::string&, const std::string&);

            template<FlagType T>
            void addPersistentFlag(const std::string&, const std::string&, T, const std::string& = "");

            template<FlagType T>
            void addLocalFlag(const std::string&, const std::string&, T, const std::string& = "");

            Command(Command const&) = delete;             // Copy construct
            Command(Command&&) = delete;                  // Move construct
            Command& operator=(Command const&) = delete;  // Copy assign
            Command& operator=(Command &&) = delete;      // Move assign

            int operator()(const std::vector<std::string>&);

            int execute(int, char**);
    };

    template<FlagType T>
    FlagImpl<T>* Command::find_persistent_flag(const std::string& name) const {
        if (auto val = persistent_flags_.find<T>(name))
            return val;
        if (parent_)
            return parent_->find_persistent_flag<T>(name);
        return nullptr;
    }
    template<FlagType T>
    FlagImpl<T>* Command::find_flag(const std::string& name) const {
        if (auto val = local_flags_.find<T>(name))
            return val;
        return find_persistent_flag<T>(name);             
    };
    bool Command::hasParent() const { return (bool)parent_; }
    bool Command::hasSubcommands() const { return !subcommands_.empty(); }
    bool Command::hasFlags() const {
        return !persistent_flags_.empty() ||
                !local_flags_.empty();
    }
    std::shared_ptr<Command> Command::addSubcommand(
        const std::string& name,
        const std::string& description,
        Action action
    ) {
        auto cmd = makeCommand(name, description, action);
        subcommands_[name] = cmd;
        auto shared_this = shared_from_this();
        cmd->parent_ = shared_this;
        return cmd;                
    }
    std::shared_ptr<Command> Command::addSubcommand(std::shared_ptr<Command> subCmd) {
        subcommands_[subCmd->name_] = subCmd;
        auto shared_this = shared_from_this();
        subCmd->parent_ = shared_this;
        return subCmd;
        
    }
    template<FlagType T>
    std::optional<T> Command::getFlag(const std::string& name) const {
        if (FlagImpl<T>* val = find_flag<T>(name))
            return val->get();
        return std::nullopt;
    }
    template<FlagType T>
    bool Command::setFlag(const std::string& name, const std::string& val) {
        if (FlagImpl<T>* f = find_flag<T>(name)) {
            f->set(val);
            return true;
        }
        return false;
    }

    // TODO: Catch cases of flag conflicts
    template<FlagType T>
    void Command::addPersistentFlag(
        const std::string& name,
        const std::string& description,
        T default_value,
        const std::string& shorthand
    ) {
        persistent_flags_.addFlag<T>(name, description, default_value, shorthand);
    }
    template<FlagType T>
    void Command::addLocalFlag(
        const std::string& name,
        const std::string& description,
        T default_value,
        const std::string& shorthand
    ){
        local_flags_.addFlag<T>(name, description, default_value, shorthand);
    }
    int Command::operator()(const std::vector<std::string>& args) {
        return action_(this, args);
    }
    int Command::execute(int argc, char** argv) {
        if (!hasParent()) return parent_->execute(argc, argv);
        std::vector<std::string> args(argv + 1, argv + argc);
        error_m("Not implemented");
        return 0;
    }

} // namespace paint_cli

#endif // COMMAND_HPP_