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
#include <map>
#include <memory>
#include <optional>
#include <functional>

#include <flag.hpp>

namespace pnt_cli {
    class Command {
        private:
            using Action = std::function<int(const Command*, std::vector<std::string>)>;

            std::string name_;
            std::string description_;
            Action action_;
            FlagSet persistent_flags_;
            FlagSet local_flags_;
            
            std::map<std::string, std::shared_ptr<Command>> subcommands_;
            std::shared_ptr<Command> parent_;

            template<FlagType T>
            std::optional<T> find_persistent_flag(const std::string& name) const {
                if (auto val = persistent_flags_.getVal<T>(name))
                    return val;
                if (parent_)
                    return parent_->find_persistent_flag<T>(name);
                return std::nullopt;
            }
        public:
            Command() = delete;

            Command(std::string name, std::string description, Action action) 
                : name_(std::move(name)),
                  description_(std::move(description)),
                  action_(std::move(action)) {}

            template<FlagType T> 
            std::optional<T> getFlag(const std::string &name) const {
                if (auto val = local_flags_.getVal<T>(name))
                    return val;
                return find_persistent_flag<T>(name);             
            };
            template<FlagType T>
            void addPersistentFlag(std::string name, std::string shorthand, std::string description, T default_value) {
                persistent_flags_.addFlag<T>(name, shorthand, description, default_value);
            }
            template<FlagType T>
            void addLocalFlag(std::string name, std::string shorthand, std::string description, T default_value) {
                local_flags_.addFlag<T>(name, shorthand, description, default_value);
            }

            ~Command() = default;

            Command(Command const&) = delete;             // Copy construct
            Command(Command&&) = delete;                  // Move construct
            Command& operator=(Command const&) = delete;  // Copy assign
            Command& operator=(Command &&) = delete;      // Move assign
            int execute(int argc, char** argv) {
                error_m("Not implemented");
                return 0;
            }
    };
} // namespace paint_cli

#endif // COMMAND_HPP_