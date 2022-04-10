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
#include <stdexcept>
#include <memory>
#include <optional>
#include <functional>
#include <concepts>
#include <ranges>


#include <flag.hpp>

namespace pnt_cli {
    class Command;
    using Action = std::function<int(Command const&, std::vector<std::string>)>;

    inline std::shared_ptr<Command> makeCommand(
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
            
            Command() = delete;
            Command(const std::string& name, const std::string& description, Action action) 
                : name_(name), description_(description), action_(action) {}
            int invoke(const std::vector<std::string>&);

            // Member functions for Flag searching

            template<FlagType T>
            void addFlagToSet(FlagSet&, const std::string&, const std::string&, T, const std::string&);

            Flag* find_persistent_flag_simple(std::string const&) const;
            Flag* find_flag_simple(std::string const&) const;
            template<FlagType T>
            FlagImpl<T>* find_persistent_flag(std::string const&) const;
            template<FlagType T> 
            FlagImpl<T>* find_flag(std::string const&) const;

            // Member functions for executing
            /**
             * @brief Searches for a subcommand name in the args provided
             * 
             * @return const iterator to the first arg in the vector that is a subcommand, or args.end()
             */
            std::vector<std::string>::const_iterator 
                find_first_subcommand(const std::vector<std::string>&) const;
            
            //!Note is meant to be called from root downwards, so only checks for flags in the current command
            //!Note due to above behaviour, local flags that appear in two commands will be consumed by the one higher in the tree
            // TODO: possibly would be better to prohibit subcommands from having local flags with the same name as commands higher in the tree
            /**
             * @brief parses the args to consume flags that are visible to this command
             */
            void consume_flags(std::vector<std::string>&);

        public:
            // This is the only way to create a Command.
            friend std::shared_ptr<Command> makeCommand(
                const std::string& name,
                const std::string& description,
                Action action
            ) {
                return std::shared_ptr<Command>(
                    new Command(name, description, action)
                );
            }            

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

            int execute(int, char**);
    };
    inline Flag* Command::find_persistent_flag_simple(std::string const& name) const {
        if (auto flag = persistent_flags_.find_simple(name))
            return flag;
        if (hasParent())
            return parent_->find_persistent_flag_simple(name);
        return nullptr;
    }
    inline Flag* Command::find_flag_simple(std::string const& name) const {
        if (auto flag = local_flags_.find_simple(name))
            return flag;
        return find_persistent_flag_simple(name);
    }
    template<FlagType T>
    inline FlagImpl<T>* Command::find_persistent_flag(const std::string& name) const {
        return find_persistent_flag_simple(name).as<T>();
    }
    template<FlagType T>
    inline FlagImpl<T>* Command::find_flag(const std::string& name) const {
        return find_flag_simple(name).as<T>();
    };
    inline bool Command::hasParent() const { return (bool)parent_; }
    inline bool Command::hasSubcommands() const { return !subcommands_.empty(); }
    inline bool Command::hasFlags() const {
        return !persistent_flags_.empty() ||
                !local_flags_.empty();
    }
    inline std::shared_ptr<Command> Command::addSubcommand(
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
    inline std::shared_ptr<Command> Command::addSubcommand(std::shared_ptr<Command> subCmd) {
        subcommands_[subCmd->name_] = subCmd;
        auto shared_this = shared_from_this();
        subCmd->parent_ = shared_this;
        return subCmd;
        
    }
    template<FlagType T>
    inline std::optional<T> Command::getFlag(const std::string& name) const {
        if (FlagImpl<T>* val = find_flag<T>(name))
            return val->get();
        return std::nullopt;
    }
    template<FlagType T>
    inline bool Command::setFlag(const std::string& name, const std::string& val) {
        if (FlagImpl<T>* f = find_flag<T>(name)) {
            f->set(val);
            return true;
        }
        return false;
    }

    //*DONE: Catch cases of flag conflicts
    //*DONE: Test cases of flag conflicts
    template<FlagType T>
    inline void Command::addFlagToSet(
        FlagSet& set,
        const std::string& name,
        const std::string& description,
        T default_value,
        const std::string& shorthand
    ) {
        if (find_flag<T>(name))
            throw std::runtime_error(std::string("Flag with name ") + name + " already exists");
        if (shorthand.length() && find_flag<T>(shorthand))
            throw std::runtime_error(std::string("Flag with shorthand ") + shorthand + " already exists");
        set.addFlag<T>(name, description, default_value, shorthand);
    }
    template<FlagType T>
    inline void Command::addPersistentFlag(
        const std::string& name,
        const std::string& description,
        T default_value,
        const std::string& shorthand
    ) {
        addFlagToSet<T>(persistent_flags_, name, description, default_value, shorthand);
    }
    template<FlagType T>
    inline void Command::addLocalFlag(
        const std::string& name,
        const std::string& description,
        T default_value,
        const std::string& shorthand
    ){
        addFlagToSet<T>(local_flags_, name, description, default_value, shorthand);
    }
    inline int Command::invoke(std::vector<std::string> const& args) {
        return action_(*this, args);
    }
    inline std::vector<std::string>::const_iterator
        Command::find_first_subcommand(std::vector<std::string> const& args) const {
        auto iterToFound = std::find_if(args.begin(), args.end(), [this](auto const& arg) {
            return this->subcommands_.find(arg) != this->subcommands_.end();
        });
        return iterToFound;
    }
    // TODO: capture indexes of flags, read them and remove them from args
    inline void Command::consume_flags(std::vector<std::string>& args) {
        // auto decider = [this](std::string const& arg) {
        //     std::string flag_name;
        //     if (arg[0] == '-' && arg[1] == '-') {
        //         flag_name = arg.substr(2);
        //         if (flag_name.length() <= 0)
        //             throw std::runtime_error("Invalid flag name: " + arg);
        //     } else if (arg[0] == '-') {
        //         flag_name = arg.substr(1);
        //         if (flag_name.length() != 1)
        //             throw std::runtime_error("Flag shorthand must be one character: " + arg);
        //     } else { return false; }
        //     if (Flag* flag = this->find_flag_simple(flag_name)) {
        //     }
        // };

        for (size_t i = 0; i < args.size(); i++) {
            std::string flag_name;
            std::string flag_value;
            if (args[i][0] == '-' && args[i][1] == '-') {
                flag_name = args[i].substr(2);
                if (flag_name.length() <= 0)
                    throw std::runtime_error("Invalid flag name: " + args[i]);
            } else if (args[i][0] == '-') {
                flag_name = args[i].substr(1);
                if (flag_name.length() <= 0) {
                    throw std::runtime_error("Flag shorthand must be at least one character");
                } else if (flag_name.length() > 1) {
                    flag_value = flag_name.substr(1);
                    flag_name = flag_name.substr(0, 1);
                    //!NOTE: this is were you stopped working on this.
                }
            } else { continue; }

        }
    }




    inline int Command::execute(int argc, char** argv) {
        if (!hasParent()) return parent_->execute(argc, argv);
        std::vector<std::string> args(argv + 1, argv + argc);
        error_m("Not implemented");
        return 0;
    }
} // namespace paint_cli

#endif // COMMAND_HPP_