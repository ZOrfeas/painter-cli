#ifndef FLOPT_HPP_
#define FLOPT_HPP_

#include <string>

#include <utils.hpp>

namespace painter::cli {
    enum class FlOptType {
        Flag, Option
    };

    class FlagOption {
        protected:
            std::string name_;
            std::string description_;
            std::string shorthand_;
            FlOptType flopt_type_;

            FlagOption(
                std::string const&,
                std::string const&,
                std::string const&,
                FlOptType
            );
        public:
            bool is_flag() const;
            bool is_option() const;
            virtual void set(std::string const& = "") = 0;
            virtual ~FlagOption() = default;
    };

    //////////////////////////////////////////////////////
    // FlagOption class member function implementations //
    //////////////////////////////////////////////////////

    inline FlagOption::FlagOption(
        std::string const& name,
        std::string const& description,
        std::string const& shorthand,
        FlOptType flopt_type
    ):  name_(name), 
        description_(description), 
        shorthand_(shorthand),
        flopt_type_(flopt_type) {}
    inline bool FlagOption::is_flag() const {
        return flopt_type_ == FlOptType::Flag;
    }
    inline bool FlagOption::is_option() const {
        return flopt_type_ == FlOptType::Option;
    }

}

#endif /* FLOPT_HPP_ */