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
            FlagImpl<T>* find_persistent_flag(const std::string& name) const {
                if (auto val = persistent_flags_.find<T>(name))
                    return val;
                if (parent_)
                    return parent_->find_persistent_flag<T>(name);
                return nullptr;
            }
            template<FlagType T> 
            FlagImpl<T>* findFlag(const std::string& name) const {
                if (auto val = local_flags_.find<T>(name))
                    return val;
                return find_persistent_flag<T>(name);             
            };

            Command() = delete;
            Command(const std::string& name, const std::string& description, Action action) 
                : name_(name), description_(description), action_(action) {}

            friend 
            std::shared_ptr<Command> makeCommand(
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
            
            bool isRoot() const { return !parent_; }
            bool hasFlags() const {
                return !persistent_flags_.empty() ||
                       !local_flags_.empty();
            }

            void addSubcommand(const std::string& name, const std::string& description, Action action) {
                subcommands_[name] = makeCommand(name, description, action);
                auto shared_this = shared_from_this();
                subcommands_[name]->parent_ = shared_this;
            }
            void addSubcommand(std::shared_ptr<Command> subCmd) {
                subcommands_[subCmd->name_] = subCmd;
                auto shared_this = shared_from_this();
                subCmd->parent_ = shared_this;
            }

            template<FlagType T>
            std::optional<T> getFlag(const std::string& name) const {
                if (FlagImpl<T>* val = findFlag<T>(name))
                    return val->get();
                return std::nullopt;
            }

            template<FlagType T>
            bool setFlag(const std::string& name, const std::string& val) {
                if (FlagImpl<T>* f = findFlag<T>(name)) {
                    f->set(val);
                    return true;
                }
                return false;
            }

            template<FlagType T>
            void addPersistentFlag(std::string name, std::string description, T default_value, std::string shorthand = "") {
                persistent_flags_.addFlag<T>(name, description, default_value, shorthand);
            }
            template<FlagType T>
            void addLocalFlag(std::string name, std::string description, T default_value, std::string shorthand = "") {
                local_flags_.addFlag<T>(name, description, default_value, shorthand);
            }

            Command(Command const&) = delete;             // Copy construct
            Command(Command&&) = delete;                  // Move construct
            Command& operator=(Command const&) = delete;  // Copy assign
            Command& operator=(Command &&) = delete;      // Move assign

            int operator()(const std::vector<std::string>& args) {
                return action_(this, args);
            }

            int execute(int argc, char** argv) {
                if (parent_) return parent_->execute(argc, argv);
                std::vector<std::string> args(argv + 1, argv + argc);
                error_m("Not implemented");
            }
    };

} // namespace paint_cli

#endif // COMMAND_HPP_