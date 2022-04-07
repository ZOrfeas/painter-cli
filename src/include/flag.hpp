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
        { pnt_cli::toString<T>(t) } -> std::same_as<std::string>;
    };

    
    template<std::integral T> T fromString(const std::string& str) { return std::stoi(str); }
    template<std::integral T> std::string toString(const T& val) {return std::to_string(val); }

    template<std::floating_point T> T fromString(const std::string& str) { return std::stof(str); }
    template<std::floating_point T> std::string toString(const T& val) {return std::to_string(val); }

    template<> std::string fromString<std::string>(const std::string& str) { return str; }
    template<> std::string toString<std::string>(const std::string& val) { return val; }

    template<> bool fromString<bool>(const std::string& str) { return "true" == str ? true : false; }
    template<> std::string toString<bool>(const bool& val) { return val ? "true" : "false"; }

    class Flag {
        private:
            std::string name_;
            std::string shorthand_;
            std::string description_;
        protected:
            utils::type_id_t type_id_;
            Flag(std::string name, std::string shorthand, std::string description, utils::type_id_t type_id_val)
                : name_(name), shorthand_(shorthand), description_(description), type_id_(type_id_val) {}
        public:
            virtual void set(const std::string&) = 0;
            /**
             * @brief Check if the flag type matches the function template argument type.
             * 
             * @tparam T Type to check against
             * @return true if matches
             */
            template<FlagType T> bool typeMatches() const ;
            // overload stream operator for printing
            friend std::ostream& operator<<(std::ostream&, const Flag&); 
            Flag() = delete;
            virtual ~Flag() = default;
    };
    template<FlagType T>
    bool Flag::typeMatches() const { return utils::type_id<T>() == type_id_; }
    std::ostream& operator<<(std::ostream& os, const Flag& f) {
        os << "{" <<  f.name_ << " (" << f.shorthand_ << ") " << f.description_ << "}";
        return os;
    }

    template<FlagType T>
    class FlagImpl : public Flag {
        private:
            T value;
        public:
            FlagImpl() = delete;
            FlagImpl(std::string name, std::string shorthand, std::string description, T defaultVal) 
                : Flag(name, shorthand, description, utils::type_id<T>()), value(defaultVal) {};
            void set(const std::string&) override;
            T get() const;
            ~FlagImpl() = default;    
    };
    template<FlagType T>
    void FlagImpl<T>::set(const std::string& str)  {
        value = fromString<T>(str);
    }
    template<FlagType T> T FlagImpl<T>::get() const {
        return value;
    }


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
            static Flag* find_in_map(
                const std::map<std::string, std::shared_ptr<Flag>>&,
                const std::string&
            );
            /**
             * @brief Checks in either of the flagset maps depending on the size of the flag name.
             * 
             * @param name the name to check for
             * @return Flag* to the flag if found, nullptr otherwise
             */
            Flag* find_in_either_map(const std::string&) const;
        public:
            FlagSet() = default;            
            ~FlagSet() = default;

            bool empty() const;
            size_t size() const;

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
            void addFlag(
                const std::string&,
                const std::string&,
                T,
                const std::string& = ""
            );
            
            /**
             * @brief Checks if a flag of type `T` exists
             * 
             * @tparam T the type of the flag to check for
             * @param name the name of the flag to check for
             * @return FlagImpl<T>* to the flag if found, nullptr if not or if the flag is not of type `T`
             */
            template<FlagType T>
            FlagImpl<T>* find(const std::string&) const;

            /**
             * @brief Gets the current value of a flag of type `T`
             * 
             * @tparam T the type of the flag to get
             * @param name the name of the flag to get
             * @return std::optional<T> the value of the flag if found, nullopt otherwise
             */
            template<FlagType T>
            std::optional<T> get(const std::string&) const;


            /**
             * @brief Sets the value of a flag of type `T`
             * 
             * @tparam T 
             * @param name the name of the flag to set
             * @param val the value to set the flag to
             * @return true if successful, false otherwise
             */
            template<FlagType T>
            bool set(const std::string&, const std::string&);

            friend std::ostream& operator<<(std::ostream&, const FlagSet&);
    };
    Flag* FlagSet::find_in_map(const std::map<std::string, std::shared_ptr<Flag>>& map, const std::string& name) {
        auto it = map.find(name);
        if (it == map.end()) {
            return nullptr;
        }
        return it->second.get();
    }
    Flag* FlagSet::find_in_either_map(const std::string& name) const {
        Flag* f;
        if (name.length() == 2) { f = find_in_map(shorthands_, name); } 
        else { f = find_in_map(flags_, name); }
        return f;
    }
    bool FlagSet::empty() const { return flags_.empty(); }
    size_t FlagSet::size() const { return flags_.size(); }
    template<FlagType T> void FlagSet::addFlag(
        const std::string& name, 
        const std::string& description,
        T defaultVal,
        const std::string& shorthand
    ) {
        auto flag = std::make_shared<FlagImpl<T>>(name, shorthand, description, defaultVal);
        flags_[name] = flag;
        if (shorthand != "") {
            shorthands_[shorthand] = flag;
            long_to_short_[name] = shorthand;
        }
    }
    template<FlagType T> FlagImpl<T>* FlagSet::find(const std::string& name) const {
        auto f = find_in_either_map(name);
        return f && f->typeMatches<T>() ? static_cast<FlagImpl<T>*>(f) : nullptr;
    }
    template<FlagType T> std::optional<T> FlagSet::get(const std::string& name) const {
        FlagImpl<T>* f = find<T>(name);
        return f ? std::make_optional(f->get()) : std::nullopt;
    }
    template<FlagType T> bool FlagSet::set(const std::string& name, const std::string& val) {
        FlagImpl<T>* f = find<T>(name);
        if (!f) {
            debug_m("Tried to set non existent flag: " + name);
            return false;
        };
        f->set(val);
        return true;
    }
    std::ostream& operator<<(std::ostream& os, const FlagSet& fs) {
        os << "FlagSet:" << '\n';
        os << "\tFlags:" << '\n';
        os << '\t' << utils::stringify_indirect_map(fs.flags_);
        os << "\tShorthands:" << '\n';
        os << '\t' << utils::stringify_indirect_map(fs.shorthands_);
        os << "\tLong to short:" << '\n';
        os << '\t' << utils::stringify_map(fs.long_to_short_);
        return os;
    }
} // namespace pnt_cli



#endif // FLAG_HPP_