#pragma once

#include <atomic>
#include <cassert>
#include <cstdint>
#include <type_traits>

namespace laya
{

// 非线程安全的引用计数基类
// 用于单线程场景，开销最小
class RefCountedBase
{
  public:
    RefCountedBase(const RefCountedBase &) = delete;
    RefCountedBase &operator=(const RefCountedBase &) = delete;

    bool HasOneRef() const { return ref_count_ == 1; }
    bool HasAtLeastOneRef() const { return ref_count_ >= 1; }

  protected:
    RefCountedBase() = default;
    ~RefCountedBase()
    {
        assert(ref_count_ == 0 && "RefCounted object deleted with non-zero ref count");
    }

    void AddRef() const { ++ref_count_; }

    // 返回 true 表示引用计数归零，对象应被销毁
    bool Release() const
    {
        assert(ref_count_ > 0);
        --ref_count_;
        return ref_count_ == 0;
    }

  private:
    mutable uint32_t ref_count_ = 0;
};

// 线程安全的引用计数基类
// 使用原子操作，可跨线程安全使用
class RefCountedThreadSafeBase
{
  public:
    RefCountedThreadSafeBase(const RefCountedThreadSafeBase &) = delete;
    RefCountedThreadSafeBase &operator=(const RefCountedThreadSafeBase &) = delete;

    bool HasOneRef() const { return ref_count_.load(std::memory_order_acquire) == 1; }
    bool HasAtLeastOneRef() const { return ref_count_.load(std::memory_order_acquire) >= 1; }

  protected:
    RefCountedThreadSafeBase() = default;
    ~RefCountedThreadSafeBase()
    {
        assert(ref_count_.load(std::memory_order_relaxed) == 0 &&
               "RefCountedThreadSafe object deleted with non-zero ref count");
    }

    void AddRef() const { ref_count_.fetch_add(1, std::memory_order_relaxed); }

    // 返回 true 表示引用计数归零，对象应被销毁
    bool Release() const
    {
        // Release 语义确保之前对对象的修改对执行 delete 的线程可见
        if (ref_count_.fetch_sub(1, std::memory_order_acq_rel) == 1)
        {
            return true;
        }
        return false;
    }

  private:
    mutable std::atomic<int32_t> ref_count_{0};
};

// 非线程安全的引用计数类
// 用法:
//   class MyClass : public laya::RefCounted<MyClass> {
//     private:
//       friend class laya::RefCounted<MyClass>;
//       ~MyClass();
//   };
template <class T>
class RefCounted : public RefCountedBase
{
  public:
    RefCounted() = default;

    void AddRef() const { RefCountedBase::AddRef(); }

    void Release() const
    {
        if (RefCountedBase::Release())
        {
            delete static_cast<const T *>(this);
        }
    }

  protected:
    ~RefCounted() = default;
};

// 线程安全的引用计数类
// 用法:
//   class MyClass : public laya::RefCountedThreadSafe<MyClass> {
//     private:
//       friend class laya::RefCountedThreadSafe<MyClass>;
//       ~MyClass();
//   };
template <class T>
class RefCountedThreadSafe : public RefCountedThreadSafeBase
{
  public:
    RefCountedThreadSafe() = default;

    void AddRef() const { RefCountedThreadSafeBase::AddRef(); }

    void Release() const
    {
        if (RefCountedThreadSafeBase::Release())
        {
            delete static_cast<const T *>(this);
        }
    }

  protected:
    ~RefCountedThreadSafe() = default;
};

} // namespace laya
