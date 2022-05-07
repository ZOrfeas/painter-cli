#ifndef COMMAND_HPP_
#define COMMAND_HPP_

#include <algorithm>
#include <string_view>

#include <flopt.hpp>
#include <utils.hpp>

namespace painter::cli {
    using namespace painter::utils;
    
    // TODO: decide what the ActionFn takes as arguments
    using ActionFn = int(*)();
    
    class Command {
    private:
        static constexpr auto is_flag_fn =
            [](cnst_shared_ptr<FloptBase> const& f) { return f->is_flag; };
        static constexpr auto is_option_fn =
            [](cnst_shared_ptr<FloptBase> const& f) { return f->is_option; };
        constexpr size_t owned_local_cnt(
            bool(*)(cnst_shared_ptr<FloptBase> const&)
        ) const;
        constexpr size_t owned_persistent_cnt(
            bool(*)(cnst_shared_ptr<FloptBase> const&)
        ) const;
        constexpr size_t persistent_flag_cnt() const;
        constexpr size_t persistent_option_cnt() const;
        constexpr bool flag_has_conflict(
            std::string_view,
            std::string_view
        ) const;
        constexpr
        bool cmd_name_has_conflict(std::string_view) const;

    public:
        std::string_view name;
        std::string_view description;
        ActionFn action;

        cnst_vector<cnst_shared_ptr<FloptBase>> local_flopts;
        cnst_vector<cnst_shared_ptr<FloptBase>> persistent_flopts;
        
        Command* parent;
        cnst_vector<Command*> subcommands;
        constexpr Command(
            std::string_view,
            std::string_view,
            ActionFn action,
            Command* parent = nullptr
        );
        // TODO: add constructors to simplify the definition of a 
        // TODO:    cli in a single step.
        //!NOTE: through the constructor/add_flopt functions, we can keep
        //!NOTE:    count of how many flopts of each type exist me thinks.
        //!NOTE: maybe we can allow the user to allocate space for these and
        //!NOTE:    provide it.
        constexpr ~Command();
        constexpr size_t flag_cnt() const;
        constexpr size_t option_cnt() const;

        template<Deserializable T>
        constexpr
        bool add_local_flopt(
            std::string_view, std::string_view,
            T const&, std::string_view = ""
        );
        template<Deserializable T>
        constexpr
        bool add_persistent_flopt(
            std::string_view, std::string_view,
            T const&, std::string_view = ""
        );
        constexpr
        bool add_subcommand(Command*);
        constexpr
        bool add_subcommand(
            std::string_view,
            std::string_view,
            ActionFn action
        );
    };

    ///////////////////////////////////////////////////
    // Command class member function implementations //
    ///////////////////////////////////////////////////

    constexpr inline Command::Command(
        std::string_view name,
        std::string_view description,
        ActionFn action,
        Command* parent
    ):  name(name), description(description),
        action(action), parent(parent) {}
    constexpr inline Command::~Command() {
        for (auto& subcommand : subcommands) {
            delete subcommand;
        }
    }
    constexpr inline bool Command::flag_has_conflict(
        std::string_view other_name,
        std::string_view other_shorthand
    ) const {
        return 
        std::any_of(
            local_flopts.begin(), local_flopts.end(),
            [&](cnst_shared_ptr<FloptBase> const& f) {
                return f->name == other_name ||
                (f->shorthand != "" && f->shorthand == other_shorthand);
            }
        ) ||
        std::any_of(
            persistent_flopts.begin(), persistent_flopts.end(),
            [&](cnst_shared_ptr<FloptBase> const& f) {
                return f->name == other_name ||
                (f->shorthand != "" && f->shorthand == other_shorthand);
            }
        );
    }
    template<Deserializable T>
    constexpr inline bool Command::add_local_flopt(
        std::string_view name, std::string_view description,
        T const& default_value, std::string_view shorthand
    ) {
        if (flag_conflicts(name, shorthand)) {
            return false;
        }
        local_flopts.push_back(
            cnst_shared_ptr<FloptBase>(
                new Flopt<T>(name, description, default_value, shorthand)
            )
        );
        return true;
    }
    template<Deserializable T>
    constexpr inline bool Command::add_persistent_flopt(
        std::string_view name, std::string_view description,
        T const& default_value, std::string_view shorthand
    ) {
        if (flag_conflicts(name, shorthand)) {
            return false;
        }
        persistent_flopts.push_back(
            cnst_shared_ptr<FloptBase>(
                new Flopt<T>(name, description, default_value, shorthand)
            )
        );
        return true;
    }

    constexpr inline size_t Command::owned_local_cnt(
        bool(*pred)(cnst_shared_ptr<FloptBase> const&)
    ) const {
        return std::count_if(
            local_flopts.begin(),
            local_flopts.end(),
            pred
        );
    }
    constexpr inline size_t Command::owned_persistent_cnt(
        bool(*pred)(cnst_shared_ptr<FloptBase> const&)
    ) const {
        return std::count_if(
            persistent_flopts.begin(),
            persistent_flopts.end(),
            pred
        );
    }
    constexpr inline size_t Command::persistent_flag_cnt() const {
        return owned_persistent_cnt(is_flag_fn) +
            (parent ? parent->persistent_flag_cnt() : 0);
    }
    constexpr inline size_t Command::persistent_option_cnt() const {
        return owned_persistent_cnt(is_option_fn) +
            (parent ? parent->persistent_option_cnt() : 0);
    }
    constexpr inline size_t Command::flag_cnt() const {
        return  owned_local_cnt(is_flag_fn) +
                persistent_flag_cnt();
    }
    constexpr inline size_t Command::option_cnt() const {
        return  owned_local_cnt(is_option_fn) +
                persistent_option_cnt();
    }

    constexpr inline bool Command::cmd_name_has_conflict(
        std::string_view other_name
    ) const {
        return other_name == name ||
            std::any_of(
                subcommands.begin(), subcommands.end(),
                [&other_name](Command* const& c) {
                    return c->name == other_name;
                }
            );
    }
    constexpr inline bool Command::add_subcommand(
        Command* subcommand
    ) {
        if (cmd_name_has_conflict(subcommand->name)) {
            return false;
        }
        subcommand->parent = this;
        subcommands.push_back(subcommand);
        return true;
    }
    constexpr inline bool Command::add_subcommand(
        std::string_view name,
        std::string_view description,
        ActionFn action
    ) {
        auto subcmd =  new Command(name, description, action, this);
        auto retval = add_subcommand(subcmd);
        if (!retval) { delete subcmd; }
        return retval;
    }

}

#endif // COMMAND_HPP_