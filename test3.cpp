#include <atomic>
#include <cstddef>
#include <stdexcept>
#include <type_traits>
#include <utility>

template <typename T>
class SharedPointer {
private:
    struct ControlBlock {
        std::atomic<std::size_t> ref_count{1};
    };

    T* ptr_ = nullptr;
    ControlBlock* cb_ = nullptr;

    void release() noexcept {
        if (!cb_) return;

        // fetch_sub returns the old value
        if (cb_->ref_count.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            // Ensure we see all writes to *ptr_ before deleting
            std::atomic_thread_fence(std::memory_order_acquire);
            delete ptr_;
            delete cb_;
        }
        ptr_ = nullptr;
        cb_ = nullptr;
    }

public:
    // default
    SharedPointer() noexcept = default;

    // nullptr
    SharedPointer(std::nullptr_t) noexcept {};

    // raw pointer
    explicit SharedPointer(T* raw) : ptr_(raw) {
        if (raw) cb_ = new ControlBlock();
    }

    // construct T in-place (perfect forwarding), but do NOT hijack copy/move ctor
    template <typename... Args>
    explicit SharedPointer(Args&&... args)
    : ptr_(new T(std::forward<Args>(args)...)), cb_(new ControlBlock()) {}

    // copy ctor
    SharedPointer(const SharedPointer& other) noexcept : ptr_(other.ptr_), cb_(other.cb_) {
        if (cb_) cb_->ref_count.fetch_add(1, std::memory_order_relaxed);
    }

    // move ctor
    SharedPointer(SharedPointer&& other) noexcept : ptr_(other.ptr_), cb_(other.cb_) {
        other.ptr_ = nullptr;
        other.cb_ = nullptr;
    }

    ~SharedPointer() { release(); }

    // copy assignment
    SharedPointer& operator=(const SharedPointer& other) noexcept {
        if (this == &other) return *this;

        // Increment first to be safe even if other == *this (we already handled) or in weird aliasing cases
        if (other.cb_) other.cb_->ref_count.fetch_add(1, std::memory_order_relaxed);

        release();
        ptr_ = other.ptr_;
        cb_  = other.cb_;
        return *this;
    }

    // move assignment
    SharedPointer& operator=(SharedPointer&& other) noexcept {
        if (this == &other) return *this;

        release();
        ptr_ = other.ptr_;
        cb_  = other.cb_;
        other.ptr_ = nullptr;
        other.cb_  = nullptr;
        return *this;
    }

    // operators
    T& operator*() const {
        if (!ptr_) throw std::runtime_error("dereference null SharedPointer");
        return *ptr_;
    }

    T* operator->() const noexcept { return ptr_; }

    // utilities
    T* get() const noexcept { return ptr_; }

    std::size_t use_count() const noexcept {
        return cb_ ? cb_->ref_count.load(std::memory_order_relaxed) : 0;
    }

    explicit operator bool() const noexcept { return ptr_ != nullptr; }

    void reset() noexcept { release(); }

    void reset(T* raw) {
        release();
        if (raw) {
            ptr_ = raw;
            cb_ = new ControlBlock();
        }
    }
};