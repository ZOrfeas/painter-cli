#ifndef FLOPT_SET_HPP_
#define FLOPT_SET_HPP_

#include <iostream>
#include <string>
#include <unordered_map>
#include <memory>
#include <utility>

#include <utils.hpp>
#include <flag.hpp>
#include <option.hpp>

namespace painter::cli {

    template<Deserializable T, typename... U>
    inline std::shared_ptr<FlagOption> make_flopt(U&&... u) {
        if constexpr (std::is_same_v<T, bool>) {
            return std::make_shared<Flag>(std::forward<U>(u)...);
        } else {
            return std::make_shared<OptionImpl<T>>(std::forward<U>(u)...);
        }
    }
    template<Deserializable T>
    inline T get_flopt_value(FlagOption *flopt) {
        if constexpr (std::is_same_v<T, bool>) {
            if (!flopt->is_flag()) {
                std::cerr << "Tried to get boolean from something other than a flag" << std::endl;
                std::exit(1);
            }
            return as_flag(flopt)->get();
        } else {
            if (!flopt->is_option()) {
                std::cerr << "Tried to get non-boolean from something other than an option" << std::endl;
                std::exit(1);
            }
            return as_option(flopt)->template as<T>()->get();
        }
    }

    class FloptSet {
        using str_flopt_map_t = 
            std::unordered_map<std::string, std::shared_ptr<FlagOption>>;
        using str_str_map_t = 
            std::unordered_map<std::string, std::string>;
        private:
            str_flopt_map_t flopts_;
            str_flopt_map_t shorthand_flopts_;

            static FlagOption* find_in_map(
                str_flopt_map_t const&,
                std::string const&
            );

            Flag* find_flag(std::string const&) const;
            Option* find_option(std::string const&) const;
        public:
            FloptSet() = default;
            ~FloptSet() = default;

            bool is_empty() const;
            size_t size() const;
            
            template<Deserializable T> 
            void add(
                std::string const&,
                std::string const&,
                T const&,
                std::string const& = ""
            );

            FlagOption* find_name(std::string const&) const;
            template<Deserializable T> T get_value(std::string const&) const;
            void set_value(std::string const&, std::string const&);
    };

    ////////////////////////////////////////////////////
    // FloptSet class member function implementations //
    ////////////////////////////////////////////////////

    inline FlagOption* FloptSet::find_in_map(
        str_flopt_map_t const& map,
        std::string const& name
    ) {
        auto it = map.find(name);
        if (it == map.end()) { return nullptr; }
        return it->second.get();
    }
    inline FlagOption* FloptSet::find_name(std::string const& name) const {
        if (name.length() == 1) {
            return find_in_map(shorthand_flopts_, name);
        } else if (name.length() > 1) {
            return find_in_map(flopts_, name);
        } else {
            return nullptr;
        }
    }
    inline Flag* FloptSet::find_flag(std::string const& name) const {
        auto flopt = find_name(name);
        if (flopt != nullptr && flopt->is_flag()) {
            return as_flag(flopt);
        }
        return nullptr;
    }
    inline Option* FloptSet::find_option(std::string const& name) const {
        auto flopt = find_name(name);
        if (flopt != nullptr && flopt->is_option()) {
            return as_option(flopt);
        }
        return nullptr;
    }
    inline bool FloptSet::is_empty() const {
        return flopts_.empty();
    }
    inline size_t FloptSet::size() const {
        return flopts_.size();
    }
    template<Deserializable T>
    inline void FloptSet::add(
        std::string const& name,
        std::string const& description,
        T const& default_value,
        std::string const& shorthand
    ) {
        if (find_name(name) != nullptr) {
            std::cerr << "Flopt " << name << " already exists" << std::endl;
            std::exit(1);
        }
        auto flopt = make_flopt<T>(name, description, default_value, shorthand);
        flopts_[name] = flopt;
        if (shorthand.length() != 0) {
            shorthand_flopts_[shorthand] = flopt;
        }
    }

    template<Deserializable T>
    inline T FloptSet::get_value(std::string const& name) const {
        if (auto flopt = find_name(name)) {
            return get_flopt_value<T>(flopt);
        }
        //!Note: could straight up exit here, but may want to allow handling
        throw std::runtime_error("Flopt " + name + " does not exist (while getting)");
    }
    void FloptSet::set_value(std::string const& name, std::string const& value) {
        if (auto flopt = find_name(name)) {
            flopt->set(value);
            return;
        }
        //!Note: could straight up exit here, but may want to allow handling
        throw std::runtime_error("Flopt " + name + " does not exist (while setting)");
    }
}

#endif /* FLOPT_SET_HPP_ */