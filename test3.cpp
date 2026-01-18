/*
实现一种shared_ptr，要求如下：
1. 构造函数的入参可以是数据类型对应的构造函数参数、左值、右值和裸指针，功能包括复制构造和移动构造。
2. 实现引用计数，并初始化为1。增加引用则计数+1，减少引用计数-1，当计数归0时自动析构对象。
3. 实现操作符->、*和=。
*/

template<typename T>
class SharedPointer {
    private:
        struct Controlblock {
            std::atomic<std::size_t> ref_count{1};
        };
        T* ptr_ = nullptr;
        Controlblock* cb = nullptr;

        void release() noexcept {
            if (!cb_) return;
            if (cb->ref_count.fetch_sub(1, std::memory_order_acq_rel) == 1) {
                delete ptr_;
                delete cb_;
            }
            ptr_ = nullptr;
            cb_ = nullptr;
        }

    public:
        // default contruction
        SharedPointer() noexcept = default;

        // nullptr contruction
        SharedPointer(std::nullptr_t) noexcept {}

        // raw pointer
        explicit SharedPointer(T* raw) : ptr_(raw) {
            if (raw) cb_ = new Controlblock();
        }

        // forward reference, can solve both rvalue & lvalue
        template <typename... Args>
        explicit SharedPointer(Args&&... args) {
            ptr_ = new T(std::forward<Args>(args)...);
            cb_ = new Controlblock();
        }

        SharedPointer(const SharedPointer& other) noexcept : ptr_(other.ptr_), cb_(other.cb_) {
            if (cb_) cb_->ref_count.fetch_add(1, std::memory_order_relaxed);
        }

        SharedPointer(SharedPointer&& other) noexcept : ptr_(other.ptr_), cb_(other.cb_) {
            other.ptr_ = nullptr;
            other.cb_ = nullptr;
        }

        ~SharedPointer() {
            release();
        }


        // copy assignment
        SharedPointer& operator=(const SharedPointer& other) noexcept {
            if (this == &other) return *this;
            if (other.cb_) other.cb_->ref_count.fetch_add(1, std::memory_order_relaxed);
            release();
            ptr_ = other.ptr_;
            cb_ = other.cb_;
            return *this;
        }

        // move assignment
        SharedPointer& operator=() noexcept {
            if (this == &other) return * this;
            release();
            ptr_ = other.ptr_;
            cb_ = other.cb_;
            other.ptr_ = nullptr;
            other.cb_ = nullptr;
            return *this;
        }

        T& operator*() const {
            if (!ptr_) throw std::runtime_error("");
            return *ptr_;
        }

        T* operator->() const noexcept {
            return ptr_;
        }

        T* get() const noexcept {return ptr_;}

        std::size_t use_count() const noexcept {
            return cb_ ? cb_->ref_count.load(std::memory_order_relaxed) : 0;
        }

        explicit operator bool() const noexcept {return ptr_ != nullptr;}

        void reset() noexcept {release();}

        void reset(T* raw) {
            release();
            if (raw) {
                ptr_ = raw;
                cb_ = new Controlblock();
            }
        }

};