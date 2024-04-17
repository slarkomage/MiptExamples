#include <array>
#include <iostream>
#include <memory>
#include <type_traits>

template <typename T>
class SharedPtr;

template <typename T>
class WeakPtr;

struct BaseControlBlock {
 protected:
  size_t shared_count;
  size_t weak_count;
 public:
  BaseControlBlock() : shared_count(1), weak_count(0) {};

  void CreateShared() {
    shared_count += 1;
  }
  void CreateWeak() {
    weak_count += 1;
  }

  size_t GetSharedCount() const {
    return shared_count;
  }

  size_t GetWeakCount() const {
    return weak_count;
  }

  virtual char* GetObject() = 0;

  virtual void DeleteObject() = 0;

  virtual void DeleteBlock() = 0;

  void DeleteShared() {
    shared_count -= 1;
    if (shared_count == 0) {
      DeleteObject();
      if (weak_count == 0) {
        DeleteBlock();
      }
    }
  }

  void DeleteWeak() {
    weak_count -= 1;
    if (shared_count == 0 and weak_count == 0) {
      DeleteBlock();
    }
  }

  virtual ~BaseControlBlock() = default;
};

template <typename T, typename Allocator = std::allocator<T>,
    typename Deleter = std::default_delete<T>>
struct ControlBlockRegular : public BaseControlBlock {
 public:
  explicit ControlBlockRegular(T* ptr) : object(ptr) {}

  ControlBlockRegular(T* ptr, Allocator alloc, Deleter deleter)
      : object(ptr),
        alloc(alloc),
        deleter(deleter) {}

  void DeleteObject() override {
    deleter(object);
  }

  void DeleteBlock() override {
    BlockAlloc block_alloc(alloc);
    BlockAllocTraits::deallocate(block_alloc, this, 1);
  }

  char* GetObject() override {
    return reinterpret_cast<char*>(object);
  }

 protected:
  using AllocTraits = std::allocator_traits<Allocator>;
  using BlockAlloc =
      typename AllocTraits::template rebind_alloc<
          ControlBlockRegular<T, Allocator, Deleter>>;
  using BlockAllocTraits = std::allocator_traits<BlockAlloc>;
  T* object;
  Allocator alloc;
  Deleter deleter;
};

template <typename T, typename Allocator = std::allocator<T>>
struct ControlBlockMakeShared : public BaseControlBlock {
 public:
  template <typename... Args>
  explicit ControlBlockMakeShared(Allocator alloc, Args&& ... args)
      : alloc(alloc) {
    AllocTraits::construct(alloc, reinterpret_cast<T*>(object),
                           std::forward<Args>(args)...);
  }

  void DeleteObject() override {
    AllocTraits::destroy(alloc, reinterpret_cast<T*>(object));
  }

  void DeleteBlock() override {
    BlockAlloc block_alloc(alloc);
    BlockAllocTraits::deallocate(block_alloc, this, 1);
  }

  char* GetObject() override {
    return reinterpret_cast<char*>(reinterpret_cast<T*>(object));
  }

 protected:
  using AllocTraits = std::allocator_traits<Allocator>;
  using BlockAlloc =
      typename AllocTraits::template rebind_alloc<
          ControlBlockMakeShared<T, Allocator>>;
  using BlockAllocTraits = std::allocator_traits<BlockAlloc>;
  char object[sizeof(T)];
  Allocator alloc;
};

template <typename T>
class SharedPtr {
 public:
  SharedPtr() : cb_(nullptr) {};

  SharedPtr(T* c_ptr) : cb_(new ControlBlockRegular<T>(c_ptr)) {}

  template <typename U, typename Allocator, typename Deleter>
  SharedPtr(U* ptr, Deleter deleter, Allocator alloc) {
    using AllocTraits = std::allocator_traits<Allocator>;
    using BlockAlloc =
        typename AllocTraits::template rebind_alloc<
            ControlBlockRegular<T, Allocator, Deleter>>;
    using BlockAllocTraits = std::allocator_traits<BlockAlloc>;
    BlockAlloc block_alloc(alloc);
    cb_ = BlockAllocTraits::allocate(block_alloc, 1);
    new(reinterpret_cast<ControlBlockRegular<T, Allocator, Deleter>*>(cb_))
        ControlBlockRegular(static_cast<T*>(ptr), alloc, deleter);
  }

  SharedPtr(const WeakPtr<T>& other) : cb_(other.cb_) {
    if (cb_) {
      cb_->CreateShared();
    }
  }

  template <typename U, typename Deleter>
  SharedPtr(U* ptr, Deleter deleter)
      : SharedPtr(ptr, deleter, std::allocator<T>()) {}

  template <typename U>
  SharedPtr(const SharedPtr<U>& other) : cb_(other.cb_) {
    if (cb_) {
      cb_->CreateShared();
    }
  }

  SharedPtr(const SharedPtr& other) : cb_(other.cb_) {
    if (cb_) {
      cb_->CreateShared();
    }
  }

  template <typename U>
  SharedPtr<T>& operator=(const SharedPtr<U>& other) {
    SharedPtr<T> copy(other);
    swap(copy);
    return *this;
  }

  SharedPtr<T>& operator=(const SharedPtr& other) {
    SharedPtr<T> copy(other);
    swap(copy);
    return *this;
  }

  explicit SharedPtr(BaseControlBlock* cb) : cb_(cb) {}

  ~SharedPtr() {
    if (cb_) {
      cb_->DeleteShared();
    }
  }

  template <typename U>
  SharedPtr(SharedPtr<U>&& other) noexcept {
    cb_ = other.cb_;
    other.cb_ = nullptr;
  }

  SharedPtr(SharedPtr&& other) noexcept {
    cb_ = other.cb_;
    other.cb_ = nullptr;
  }

  template <typename U>
  SharedPtr& operator=(SharedPtr<U>&& other) noexcept {
    SharedPtr copy(std::move(other));
    swap(copy);
    return *this;
  }

  SharedPtr& operator=(SharedPtr&& other) noexcept {
    SharedPtr copy(std::move(other));
    swap(copy);
    return *this;
  }

  size_t use_count() const {
    return cb_->GetSharedCount();
  }

  void reset() {
    if (cb_) {
      cb_->DeleteShared();
    }
    cb_ = nullptr;
  }

  template <typename U>
  void reset(U* ptr) {
    if (cb_) {
      cb_->DeleteShared();
    }
    cb_ = new ControlBlockRegular<U>(ptr);
  }

  T* get() const {
    if (cb_) {
      return reinterpret_cast<T*>(cb_->GetObject());
    }
    return nullptr;
  }

  T* operator->() const {
    return get();
  }

  T& operator*() const {
    return *(get());
  }

  void swap(SharedPtr& other) {
    std::swap(cb_, other.cb_);
  } // for copy and swap idiom

 private:
  BaseControlBlock* cb_{nullptr}; // cb = control block

  template <typename>
  friend
  class SharedPtr;
  template <typename>
  friend
  class WeakPtr;
};

template <typename T>
class WeakPtr {
 public:
  WeakPtr() : cb_(nullptr) {};

  template <typename U>
  WeakPtr(const SharedPtr<U>& other) : cb_(other.cb_) {
    if (cb_) {
      cb_->CreateWeak();
    }
  }

  template <typename U>
  WeakPtr(const WeakPtr<U>& other) : cb_(other.cb_) {
    if (cb_) {
      cb_->CreateWeak();
    }
  }

  WeakPtr(const WeakPtr& other) : cb_(other.cb_) {
    if (cb_) {
      cb_->CreateWeak();
    }
  }

  template <typename U>
  WeakPtr<T>& operator=(const WeakPtr<U>& other) {
    WeakPtr<T> copy(other);
    swap(copy);
    return *this;
  }

  WeakPtr<T>& operator=(const WeakPtr& other) {
    WeakPtr<T> copy(other);
    swap(copy);
    return *this;
  }

  template <typename U>
  WeakPtr(WeakPtr<U>&& other) {
    cb_ = other.cb_;
    other.cb_ = nullptr;
  }

  WeakPtr(WeakPtr&& other) noexcept {
    cb_ = other.cb_;
    other.cb_ = nullptr;
  }

  template <typename U>
  WeakPtr<T>& operator=(WeakPtr<U>&& other) {
    WeakPtr<T> copy(std::move(other));
    swap(copy);
    return *this;
  }

  WeakPtr<T>& operator=(WeakPtr&& other) noexcept {
    WeakPtr<T> copy(std::move(other));
    swap(copy);
    return *this;
  }

  ~WeakPtr() {
    if (cb_) {
      cb_->DeleteWeak();
    }
  }

  T* get() const {
    if (cb_) {
      return reinterpret_cast<T*>(cb_->GetObject());
    }
    return nullptr;
  }

  T* operator->() const {
    return get();
  }

  T& operator*() const {
    return *(get());
  }

  size_t use_count() const {
    return cb_->GetSharedCount();
  }

  bool expired() const {
    if (cb_) {
      return (cb_->GetSharedCount() == 0);
    }
    return true;
  }

  SharedPtr<T> lock() const {
    if (expired()) {
      return SharedPtr<T>{};
    }
    cb_->CreateShared();
    return SharedPtr<T>(this->cb_);
  }

  void swap(WeakPtr& other) {
    std::swap(cb_, other.cb_);
  } // again copy and swap

 private:
  BaseControlBlock* cb_;
  template <typename>
  friend
  class SharedPtr;
  template <typename>
  friend
  class WeakPtr;

};

template <typename T, typename Allocator, typename... Args>
SharedPtr<T> allocateShared(const Allocator& alloc, Args&& ... args) {
  using AllocTraits = std::allocator_traits<Allocator>;
  using BlockAlloc =
      typename AllocTraits::template rebind_alloc<
          ControlBlockMakeShared<T, Allocator>>;
  using BlockAllocTraits = std::allocator_traits<BlockAlloc>;
  BlockAlloc block_alloc(alloc);
  ControlBlockMakeShared<T, Allocator>* cb_ =
      BlockAllocTraits::allocate(block_alloc, 1);
  new(cb_) ControlBlockMakeShared<T, Allocator>(alloc, std::forward<Args>(args)...);
  return SharedPtr<T>(static_cast<BaseControlBlock*>(cb_));
}

template <typename T, typename... Args>
SharedPtr<T> makeShared(Args&& ... args) {
  return allocateShared<T>(std::allocator<T>{}, std::forward<Args>(args)...);
}
