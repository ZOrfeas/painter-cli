#ifndef COMMAND_HPP_
#define COMMAND_HPP_

#include <string>
#include <unordered_map>
#include <functional>
#include <utility>
#include <memory>
#include <span>

#include <flopt_set.hpp>

namespace painter::cli {
    class Command;
    using Action = std::function<int(Command const&, std::span<char*>)>;

    std::shared_ptr<Command> make_command(
        std::string const&, std::string const&, Action const&
    );
    class Command : public std::enable_shared_from_this<Command> {
        using flopt_lookup_t = std::tuple<
            Command*,
            FloptSet*,
            FlagOption*
        >;
        private:
            std::string name_;
            std::string description_;
            Action action_;
            FloptSet local_flopts_;
            FloptSet persistent_flopts_;

            std::shared_ptr<Command> parent_;
            std::unordered_map<std::string, std::shared_ptr<Command>> subcommands_;

            flopt_lookup_t lookup_persistent_flopt(std::string const&);     
            flopt_lookup_t lookup_flopt(std::string const&);
            Command(
                std::string const&, std::string const&, Action const&
            );
        public:
            friend std::shared_ptr<Command> make_command(
                std::string const& name,
                std::string const& description,
                Action const& action
            ) {
                return std::make_shared<Command>(name, description, action);
            }
            std::shared_ptr<Command> add_subcommand(
                std::string const&, std::string const&, Action const&
            );
            std::shared_ptr<Command> add_subcommand(
                std::shared_ptr<Command>
            );
            bool is_root() const;
            bool has_subcommands() const;
            bool has_flopts() const;

            template<Deserializable T>
            void add_local_flopt(
                std::string const&,
                std::string const&,
                T const&,
                std::string const&
            );
            template<Deserializable T>
            void add_persistent_flopt(
                std::string const&,
                std::string const&,
                T const&,
                std::string const&
            );
            template<Deserializable T>
            T get_flopt(std::string const&) const;
    };

    ///////////////////////////////////////////////////
    // Command class member function implementations //
    ///////////////////////////////////////////////////

    inline Command::Command(
        std::string const& name,
        std::string const& description,
        Action const& action
    ): name_(name), description_(description), action_(action) {}
    inline std::shared_ptr<Command> Command::add_subcommand(
        std::shared_ptr<Command> sub_cmd
    ) {
        if (subcommands_.find(sub_cmd->name_) != subcommands_.end()) {
            std::cerr << "Error: subcommand " << sub_cmd->name_ << " already exists" << std::endl;
            std::exit(1);
        }
        auto shared_self = shared_from_this();
        sub_cmd->parent_ = shared_self;
        subcommands_[sub_cmd->name_] = sub_cmd;
        return sub_cmd;
    }
    inline std::shared_ptr<Command> Command::add_subcommand(
        std::string const& name,
        std::string const& description,
        Action const& action
    ) {
        return add_subcommand(make_command(name, description, action));
    }
    inline auto Command::lookup_persistent_flopt(
        std::string const& name
    ) -> flopt_lookup_t {
        if (auto flopt = persistent_flopts_.find_name(name)) {
            return {this, &persistent_flopts_, flopt};
        } else if (!is_root()) {
            return parent_->lookup_persistent_flopt(name);
        } else {
            return {nullptr, nullptr, nullptr};
        }
    }
    inline auto Command::lookup_flopt(
        std::string const& name
    ) -> flopt_lookup_t {
        if (auto flopt = local_flopts_.find_name(name)) {
            return {this, &local_flopts_, flopt};
        }
        return lookup_persistent_flopt(name);
    }
    inline bool Command::is_root() const {
        return parent_ == nullptr;
    }
    inline bool Command::has_subcommands() const {
        return !subcommands_.empty();
    }
    inline bool Command::has_flopts() const {
        return !local_flopts_.is_empty() || 
            !persistent_flopts_.is_empty();
    }
    template<Deserializable T>
    void Command::add_local_flopt(
        std::string const& name,
        std::string const& description,
        T const& default_value,
        std::string const& shorthand
    ) {
        local_flopts_.add<T>(name, description, default_value, shorthand);
    }
    template<Deserializable T>
    void Command::add_persistent_flopt(
        std::string const& name,
        std::string const& description,
        T const& default_value,
        std::string const& shorthand
    ) {
        persistent_flopts_.add<T>(name, description, default_value, shorthand);
    }
    template<Deserializable T>
    inline T Command::get_flopt(std::string const& name) const {
        auto &[_, flopt_set, _] = lookup_flopt(name);
        if (flopt_set != nullptr) {
            //!Note: I think this pays a cost twice
            return flopt_set->get_value<T>(name);
        }
        throw std::runtime_error("No flopt named " + name);
    }
}

#endif /* COMMAND_HPP_ */