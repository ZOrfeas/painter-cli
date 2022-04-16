#ifndef OPTION_HPP_
#define OPTION_HPP_

#include <string>
#include <iostream>
#include <sstream>
#include <memory>
#include <concepts>

#include <flopt.hpp>
#include <utils.hpp>

namespace painter::cli {
    using namespace painter::utils;

    template<typename T>
    concept OptionType = 
        Deserializable<T> &&
        !std::same_as<T, bool>;

    template<OptionType T>
    class OptionImpl;

    class Option : public FlagOption {
        private:
            type_id_t type_tag_;
        protected:
            Option(
                std::string const&,
                std::string const&,
                std::string const&,
                type_id_t
            );
        public:
            template<OptionType T> bool type_tag_matches() const;
            template<OptionType T> OptionImpl<T>* as();
    };
    inline Option* as_option(FlagOption* flopt) {
        if (flopt->is_option()) {
            return static_cast<Option*>(flopt);
        }
        std::cerr << "Not an option" << std::endl;
        std::exit(1);
    }

    template<OptionType T>
    class OptionImpl : public Option {
        private:
            T value_;
        public:
            OptionImpl(
                std::string const&,
                std::string const&,
                T const&,
                std::string const& = ""
            );
            void set(std::string const&) override;
            T get() const;
    };

    //////////////////////////////////////////////////
    // Option class member function implementations //
    //////////////////////////////////////////////////

    inline Option::Option(
        std::string const& name,
        std::string const& description,
        std::string const& shorthand,
        type_id_t type_tag
    ): FlagOption(name, description, shorthand, FlOptType::Option),
    type_tag_(type_tag) {}

    template<OptionType T>
    inline bool Option::type_tag_matches() const {
        return type_tag_ == type_id<T>();
    }
    template<OptionType T>
    inline OptionImpl<T>* Option::as() {
        if (type_tag_matches<T>()) {
            return static_cast<OptionImpl<T>*>(this);
        }
        std::cerr << "Type tag mismatch" << std::endl;
        std::exit(1);
    }

    //////////////////////////////////////////////////////
    // OptionImpl class member function implementations //
    //////////////////////////////////////////////////////

    template<OptionType T>
    inline OptionImpl<T>::OptionImpl (
        std::string const& name,
        std::string const& description,
        T const& default_value,
        std::string const& shorthand
    ): Option(name, description, shorthand, type_id<T>()),
    value_(default_value) {}
    template<OptionType T>
    inline T OptionImpl<T>::get() const { return value_; }
    template<OptionType T>
    inline void OptionImpl<T>::set(std::string const& str) {
        std::stringstream(str) >> value_;
    }

}

#endif /* FLAG_HPP_ */