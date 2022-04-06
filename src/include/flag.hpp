/**
 * @file flag.hpp
 * @author Orfeas Zografos (eurydice.ddns.net)
 * @brief Extensible flag utils and FlagSet interface.
 * @version 0.1
 * @date 2022-04-02
 * 
 */
#ifndef FLAG_HPP_
#define FLAG_HPP_

#include <string>
#include <map>
#include <memory>
#include <optional>
#include <concepts>

#include <log.hpp>
#include <utils.hpp>

namespace pnt_cli {
    //!Note: To extend, implement pnt_cli::fromString<T>, pnt_cli::toString<T> by explicitly specializing for your type T.
    
    template<typename T> T fromString(const std::string& str);
    template<typename T> std::string toString(const T& val);

    template<typename T>
    concept FlagType = requires (T t, const std::string& s) {
        { pnt_cli::fromString<T>(s) } -> std::same_as<T>;
    };
    
    template<std::integral T> T fromString(const std::string& str) { return std::stoi(str); }
    // template<std::integral T> std::string toString(const T& val) {return std::to_string(val); }

    template<std::floating_point T> T fromString(const std::string& str) { return std::stof(str); }
    // template<std::floating_point T> std::string toString(const T& val) {return std::to_string(val); }

    template<> std::string fromString<std::string>(const std::string& str) { return str; }
    // template<> std::string toString<std::string>(const std::string& val) { return val; }

    template<> bool fromString<bool>(const std::string& str) { error_m("Function not needed"); }
    // template<> std::string toString<bool>(const bool& val) { return val ? "true" : "false"; }

    class Flag {
        private:
            std::string name_;
            std::string shorthand_;
            std::string description_;
            utils::type_id_t type_id_;
        protected:
            Flag(std::string name, std::string shorthand, std::string description, utils::type_id_t type_id_val)
                : name_(name), shorthand_(shorthand), description_(description), type_id_(type_id_val) {}
        public:
            /**
             * @brief Check if the flag type matches the function template argument type.
             * 
             * @tparam T Type to check against
             * @return true if matches
             */
            template<FlagType T> bool typeMatches() { return utils::type_id<T>() == type_id_;}

            // overload stream operator for printing
            friend std::ostream& operator<<(std::ostream& os, const Flag& f) {
                os << "{" <<  f.name_ << " (" << f.shorthand_ << ") " << f.description_ << "}";
                return os;
            }

            Flag() = delete;
            virtual ~Flag() = default;

    };

    template<FlagType T>
    class FlagImpl : public Flag {
        private:
        public:
            T value;
            FlagImpl() = delete;
            FlagImpl(std::string name, std::string shorthand, std::string description, T defaultVal) 
                : Flag(name, shorthand, description, utils::type_id<T>()), value(defaultVal) {};
            ~FlagImpl() = default;    
    };

    class FlagSet {
        private:
            std::map<std::string, std::shared_ptr<Flag>> flags_;
            std::map<std::string, std::shared_ptr<Flag>> shorthands_;
            std::map<std::string, std::string> long_to_short_;

            /**
             * @brief Helper function checking Flag existence in flag map.
             * 
             * @param map the map to check in
             * @param name the name to check for
             * @return Flag* to the flag if found, nullptr otherwise
             */
            static Flag* find_in_map(const std::map<std::string, std::shared_ptr<Flag>>& map, const std::string& name) {
                auto it = map.find(name);
                if (it == map.end()) {
                    return nullptr;
                }
                return it->second.get();
            }
            /**
             * @brief Checks in either of the flagset maps depending on the size of the flag name.
             * 
             * @param name the name to check for
             * @return Flag* to the flag if found, nullptr otherwise
             */
            Flag* find_in_either_map(const std::string& name) {
                Flag* f;
                if (name.length() == 2) { f = find_in_map(shorthands_, name); } 
                else { f = find_in_map(flags_, name); }
                return f;
            }
        public:
            FlagSet() = default;            
            ~FlagSet() = default;

            bool empty() { return flags_.empty(); }
            size_t size() { return flags_.size(); }

            /**
             * @brief Adds a new flag of type `T` to the flagset
             * 
             * @tparam T 
             * @param name the name of the flag 
             * @param description the description of the flag
             * @param defaultVal the default value of the flag
             * @param shorthand (optionally) the shorthand of the flag
             */
            template<FlagType T>
            void addFlag(const std::string& name, const std::string& description, T defaultVal,  const std::string& shorthand = "") {
                auto flag = std::make_shared<FlagImpl<T>>(name, shorthand, description, defaultVal);
                flags_[name] = flag;
                if (shorthand != "") {
                    shorthands_[shorthand] = flag;
                    long_to_short_[name] = shorthand;
                }
            }
            
            /**
             * @brief Checks if a flag of type `T` exists
             * 
             * @tparam T the type of the flag to check for
             * @param name the name of the flag to check for
             * @return FlagImpl<T>* to the flag if found, nullptr if not or if the flag is not of type `T`
             */
            template<FlagType T>
            FlagImpl<T>* find(const std::string &name) {
                auto f = find_in_either_map(name);
                return f && f->typeMatches<T>() ? static_cast<FlagImpl<T>*>(f) : nullptr;
            }

            /**
             * @brief Gets the current value of a flag of type `T`
             * 
             * @tparam T the type of the flag to get
             * @param name the name of the flag to get
             * @return std::optional<T> the value of the flag if found, nullopt otherwise
             */
            template<FlagType T>
            std::optional<T> get(const std::string &name) {
                FlagImpl<T>* f = find<T>(name);
                return f ? std::make_optional(f->value) : std::nullopt;
            }


            /**
             * @brief Sets the value of a flag of type `T`
             * 
             * @tparam T 
             * @param name the name of the flag to set
             * @param val the value to set the flag to
             * @return true if successful, false otherwise
             */
            template<FlagType T>
            bool set(const std::string &name, const std::string& val) {
                FlagImpl<T>* f = find<T>(name);
                if (!f) {
                    debug_m("Tried to set non existent flag: " + name);
                    return false;
                };
                f->value = fromString<T>(val);
                return true;
            }

            // overload stream operator
            friend std::ostream& operator<<(std::ostream& os, const FlagSet& fs) {
                os << "FlagSet:" << '\n';
                os << "\tFlags:" << '\n';
                os << '\t' << utils::stringify_indirect_map(fs.flags_);
                os << "\tShorthands:" << '\n';
                os << '\t' << utils::stringify_indirect_map(fs.shorthands_);
                os << "\tLong to short:" << '\n';
                os << '\t' << utils::stringify_map(fs.long_to_short_);
                return os;
            }
    };
} // namespace pnt_cli



#endif // FLAG_HPP_