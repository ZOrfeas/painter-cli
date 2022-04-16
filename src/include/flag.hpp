#ifndef FLAG_HPP_
#define FLAG_HPP_

#include <iostream>

#include <flopt.hpp>
#include <utils.hpp>

namespace painter::cli {
    using namespace painter::utils;
    
    class Flag : public FlagOption {
        private:
            bool value_;
        public:
            Flag(
                std::string const&,
                std::string const&,
                bool,
                std::string const& = ""
            );
            void set(std::string const& = "") override;
            bool get() const;
    };
    inline Flag* as_flag(FlagOption* flopt) {
        if (flopt->is_flag()) {
            return static_cast<Flag*>(flopt);
        }
        std::cerr << "Not a flag" << std::endl;
        std::exit(1);
    }

    ////////////////////////////////////////////////
    // Flag class member function implementations //
    ////////////////////////////////////////////////

    inline Flag::Flag(
        std::string const& name,
        std::string const& description,
        bool default_value,
        std::string const& shorthand
    ): FlagOption(name, description, shorthand, FlOptType::Flag),
        value_(default_value) {}
    inline void Flag::set(std::string const&) { value_ = !value_; }
    inline bool Flag::get() const { return value_; }

}


#endif /* FLAG_HPP_ */