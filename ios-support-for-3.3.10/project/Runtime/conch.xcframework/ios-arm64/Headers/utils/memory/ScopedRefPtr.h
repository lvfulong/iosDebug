#pragma once

#include <cassert>
#include <compare>
#include <concepts>
#include <cstddef>
#include <type_traits>
#include <utility>

namespace laya
{

// 引用计数类型概念: 要求具有 AddRef() 和 Release() 方法
template <typename T>
concept IsRefCountedType = requires(const T &x) {
    x.AddRef();
    x.Release();
};

// 引用计数智能指针，类似 std::shared_ptr 但配合 RefCounted/RefCountedThreadSafe 使用
// 避免手动调用 AddRef/Release 导致的内存泄漏
//
// 用法:
//   ScopedRefPtr<MyFoo> foo(new MyFoo());
//   foo->Method();
//   // foo 离开作用域时自动释放
//
//   ScopedRefPtr<MyFoo> foo = MakeRefCounted<MyFoo>();
//   ScopedRefPtr<MyFoo> bar = foo;  // 两者共享引用
//   foo.reset();                     // 显式释放 foo 的引用
template <class T>
class ScopedRefPtr
{
  public:
    using element_type = T;

    constexpr ScopedRefPtr() = default;
    constexpr ScopedRefPtr(std::nullptr_t) {}

    // 从裸指针构造，增加引用计数
    ScopedRefPtr(T *p) : ptr_(p)
    {
        if (ptr_)
        {
            ptr_->AddRef();
        }
    }

    // 拷贝构造
    ScopedRefPtr(const ScopedRefPtr &r) : ScopedRefPtr(r.ptr_) {}

    // 拷贝转换构造 (派生类 -> 基类)
    template <typename U>
        requires(std::convertible_to<U *, T *>)
    ScopedRefPtr(const ScopedRefPtr<U> &r) : ScopedRefPtr(r.get()) {}

    // 移动构造
    ScopedRefPtr(ScopedRefPtr &&r) noexcept : ptr_(r.ptr_) { r.ptr_ = nullptr; }

    // 移动转换构造
    template <typename U>
        requires(std::convertible_to<U *, T *>)
    ScopedRefPtr(ScopedRefPtr<U> &&r) noexcept : ptr_(r.ptr_)
    {
        r.ptr_ = nullptr;
    }

    ~ScopedRefPtr()
    {
        if (ptr_)
        {
            ptr_->Release();
        }
    }

    T *get() const { return ptr_; }

    T &operator*() const
    {
        assert(ptr_);
        return *ptr_;
    }

    T *operator->() const
    {
        assert(ptr_);
        return ptr_;
    }

    ScopedRefPtr &operator=(std::nullptr_t)
    {
        reset();
        return *this;
    }

    ScopedRefPtr &operator=(T *p) { return *this = ScopedRefPtr(p); }

    // 统一赋值运算符，通过值传递 + swap 处理自赋值
    ScopedRefPtr &operator=(ScopedRefPtr r) noexcept
    {
        swap(r);
        return *this;
    }

    // 释放当前引用
    void reset() { ScopedRefPtr().swap(*this); }

    // 释放所有权，调用者负责后续 Release()
    [[nodiscard]] T *release()
    {
        T *p = ptr_;
        ptr_ = nullptr;
        return p;
    }

    void swap(ScopedRefPtr &r) noexcept { std::swap(ptr_, r.ptr_); }

    explicit operator bool() const { return ptr_ != nullptr; }

    // 比较运算符
    template <typename U>
    friend bool operator==(const ScopedRefPtr &lhs, const ScopedRefPtr<U> &rhs)
    {
        return lhs.ptr_ == rhs.get();
    }

    template <typename U>
    friend bool operator==(const ScopedRefPtr &lhs, const U *rhs)
    {
        return lhs.ptr_ == rhs;
    }

    friend bool operator==(const ScopedRefPtr &lhs, std::nullptr_t)
    {
        return lhs.ptr_ == nullptr;
    }

    template <typename U>
    friend auto operator<=>(const ScopedRefPtr &lhs, const ScopedRefPtr<U> &rhs)
    {
        return lhs.ptr_ <=> rhs.get();
    }

    friend auto operator<=>(const ScopedRefPtr &lhs, std::nullptr_t)
    {
        return lhs.ptr_ <=> static_cast<T *>(nullptr);
    }

  private:
    T *ptr_ = nullptr;

    // 允许不同类型的 ScopedRefPtr 互相访问 ptr_ (用于移动转换构造)
    template <typename U>
    friend class ScopedRefPtr;
};

template <typename T>
void swap(ScopedRefPtr<T> &lhs, ScopedRefPtr<T> &rhs) noexcept
{
    lhs.swap(rhs);
}

// 工厂函数，构造 T 并包装为 ScopedRefPtr
template <typename T, typename... Args>
ScopedRefPtr<T> MakeRefCounted(Args &&...args)
{
    return ScopedRefPtr<T>(new T(std::forward<Args>(args)...));
}

} // namespace laya
