#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <concepts>

#define CONSTEXPR_AND_EXPECT_TRUE(x) \
    static_assert((x)); EXPECT_TRUE((x))

namespace painter::utils {

    template<typename T>
    concept Deserializable =
        requires(std::istream& is, T& t) {
            { is >> t } -> std::same_as<std::istream&>;
        };

    template<typename T>
    concept Serializable =
        requires(std::ostream& os, T const& t) {
            { os << t } -> std::same_as<std::ostream&>;
        };

    template<Deserializable T>
    T deserialize(std::string const& str) {
        std::istringstream iss(str);
        T t;
        iss >> t;
        return t;
    }

    template<Serializable T>
    std::string serialize(T const& t) {
        std::ostringstream oss;
        oss << t;
        return oss.str();
    }

    /**
     * @brief Some shared_ptr functionality but constexpr
     * 
     * @tparam T the type of pointer stored
     */
    template<typename T>
    class cnst_shared_ptr {
    private:
        T* ptr_;
        size_t* ref_cnt_;

        constexpr
        void __cleanup_self__() {
            if (ref_cnt_ == nullptr) {
                return;
            }
            --(*ref_cnt_);
            if (*ref_cnt_ == 0) {
                if (ptr_ != nullptr) {
                    delete ptr_;
                }
                delete ref_cnt_;
            }
        }
        constexpr
        void init_from(cnst_shared_ptr<T> const& other) {
            this->ptr_ = other.ptr_;
            this->ref_cnt_ = other.ref_cnt_;
            if (this->ptr_ != nullptr) {
                ++(*this->ref_cnt_);
            }
        }
        constexpr
        void steal_from(cnst_shared_ptr<T>&& dying_other) {
            this->ptr_ = dying_other.ptr_;
            this->ref_cnt_ = dying_other.ref_cnt_;
            dying_other.ptr_ = nullptr;
            dying_other.ref_cnt_ = nullptr;
        }
    public:
        constexpr
        cnst_shared_ptr(): ptr_(nullptr), ref_cnt_(nullptr) {}
        constexpr
        cnst_shared_ptr(T *ptr): ptr_(ptr), ref_cnt_(new size_t(1)) {}

        // copy constructor
        constexpr
        cnst_shared_ptr(cnst_shared_ptr<T> const& other) {
            init_from(other);
        }
        // copy assignement
        constexpr
        cnst_shared_ptr& operator=(cnst_shared_ptr<T> const& other) {
            if (this == &other) { return *this; }
            __cleanup_self__();
            init_from(other);
            return *this;
        }
        // move constructor
        constexpr
        cnst_shared_ptr(cnst_shared_ptr<T>&& dying_other) {
            steal_from(std::move(dying_other));
        }
        constexpr 
        cnst_shared_ptr& operator=(cnst_shared_ptr<T>&& dying_other) {
            if (this == &dying_other) { return *this; }
            __cleanup_self__();
            steal_from(std::move(dying_other));
            return *this;
        }
        constexpr
        ~cnst_shared_ptr() {
            // std::cout << "Destructor:\n";
            __cleanup_self__();
        }
        constexpr
        size_t get_count() const {
            return *ref_cnt_;
        }
        constexpr
        T* get() const {
            return ptr_;
        }
        constexpr
        T& operator*() const {
            return *ptr_;
        }
        constexpr
        T* operator->() const {
            return ptr_;
        }
    };

    /**
     * @brief A vector allowed in constexpr context
     * 
     * @tparam T the type of elements stored
     */
    template<typename T>
    class cnst_vector {
    private:
        T* data_;
        size_t capacity_, size_;
        constexpr
        void __cleanup_self__() {
            if (data_ != nullptr) {
                delete[] data_;
            }
        }
        constexpr
        void copy_from(cnst_vector<T> const& other) {
            this->capacity_ = other.capacity_;
            this->size_ = other.size_;
            this->data_ = new T[this->capacity_];
            std::copy(other.data_, other.data_ + this->size_, this->data_);
        }
        constexpr
        void steal_from(cnst_vector<T>&& dying_other) {
            this->capacity_ = dying_other.capacity_;
            this->size_ = dying_other.size_;
            this->data_ = dying_other.data_;
            dying_other.data_ = nullptr;
        }
    public:
        constexpr cnst_vector()
            :   data_(new T[1]), capacity_(1), size_(0) {}
        constexpr cnst_vector(size_t capacity)
            :   data_(new T[capacity]), capacity_(capacity), size_(0) {}
        constexpr
        cnst_vector(std::initializer_list<T> init_list)
            :   data_(new T[init_list.size()]), capacity_(init_list.size()),
                size_(init_list.size())
        {
            std::copy(init_list.begin(), init_list.end(), this->data_);
        }
        // copy constructor
        constexpr cnst_vector(cnst_vector<T> const& other)
            :   data_(new T[other.capacity_]), capacity_(other.capacity_),
                size_(other.size_)
        {
            std::copy(other.data_, other.data_ + other.size_, data_);
        }
        // copy assignement
        constexpr
        cnst_vector& operator=(cnst_vector<T> const& other) {
            if (this == &other) { return *this; }
            __cleanup_self__();
            copy_from(other);
        }
        // move constructor
        constexpr cnst_vector(cnst_vector<T>&& dying_other) {
            steal_from(std::move(dying_other));
        }
        // move assignement
        constexpr
        cnst_vector& operator=(cnst_vector<T>&& dying_other) {
            if (this == &dying_other) { return *this; }
            __cleanup_self__();
            steal_from(std::move(dying_other));
        }
        constexpr ~cnst_vector() {
            __cleanup_self__();
        }
        constexpr size_t size() const { return size_; }
        constexpr size_t capacity() const { return capacity_; }
        constexpr T* data() const { return data_; }
        constexpr T& operator[](size_t idx) const {
            return data_[idx];
        }
        constexpr void resize(size_t new_capacity) {
            if (new_capacity == capacity_) { return; }
            if (new_capacity < size_) {
                size_ = new_capacity;
                return;
            }
            T* new_data = new T[new_capacity];
            std::copy(data_, data_ + size_, new_data);
            delete[] data_;
            data_ = new_data;
            capacity_ = new_capacity;
        }
        constexpr void push_back(T const& val) {
            if (size_ == capacity_) {
                resize(capacity_ * 2);
            } 
            data_[size_++] = val;
        }
        constexpr void push_back(T&& val) {
            if (size_ == capacity_) {
                resize(capacity_ * 2);
            }
            data_[size_++] = std::move(val);
        }
        constexpr void pop_back() { --size_; }
        constexpr void clear() { size_ = 0; }   
        constexpr auto begin() const { return data_; }
        constexpr auto end() const { return data_ + size_; }
    };

}


#endif // UTILS_HPP_