#include <array>
#include <iostream>
#include <memory>
#include <type_traits>
#include <iterator>
#include <vector>

template <size_t N>
class StackStorage {
 public:
  std::array<char, N> stack;
  size_t shift;
  StackStorage() : shift(0) {}
  StackStorage(const StackStorage& other) = delete;
  StackStorage& operator=(const StackStorage& other) = delete;
  char* GetFirstPtr() {
    return stack.data();
  }
  size_t GetShift() {
    return shift;
  }
  void ChangeShift(size_t delta) {
    shift += delta;
  }
};

template <typename T, size_t N>
class StackAllocator {
 public:
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using value_type = T;
  using type = T;

  explicit StackAllocator(StackStorage<N>& storage) : storage_(&storage) {}

  template <typename T2>
  StackAllocator(const StackAllocator<T2, N>& other)
      : storage_(other.storage_) {}

  template <typename T2>
  StackAllocator<T, N>& operator=(StackAllocator<T2, N> other) {
    std::swap(other.storage, storage_);
    return *this;
  }

  ~StackAllocator() = default;

  T* allocate(size_t sz) {
    size_t free_space = N - storage_->GetShift();
    void* curr_ptr =
        static_cast<void*>(storage_->GetFirstPtr() + storage_->GetShift());
    if (std::align(alignof(T), sizeof(T), curr_ptr, free_space)) {
      T* final_ptr = reinterpret_cast<T*>(curr_ptr);
      // учтем, что из-за выравнивания мог произойти сдвиг (изменился free_space)
      storage_->ChangeShift((N - storage_->GetShift()) - free_space);
      // учтем количество элементов аллоцированных
      storage_->ChangeShift(sizeof(T) * sz);
      return final_ptr;
    }
    return nullptr; // если не хватило места выделить память
  }

  template <typename U>
  struct rebind {
    using other = StackAllocator<U, N>;
  };

  void deallocate(T* position, size_t sz) {
    std::ignore = position;
    std::ignore = sz;
  }

  StackStorage<N>* storage_;
};

template <typename T, typename Alloc = std::allocator<T>>
class List {
 private:
  struct BaseNode {
    BaseNode* next{nullptr};
    BaseNode* prev{nullptr};
    BaseNode() : next(this), prev(this) {}
    BaseNode(BaseNode* next, BaseNode* prev) : next(next), prev(prev) {}
    T& GetValue() {
      return (reinterpret_cast<Node*>(this))->element;
    }
    T* GetValuePointer() {
      return &((reinterpret_cast<Node*>(this))->element);
    }
  };

  struct Node : BaseNode {
    T element;
    Node() : BaseNode() {}
    explicit Node(const T& element) : BaseNode(), element(element) {}
  };

 public:
  template <bool IsConst>
  class ListIterator {
   private:
    using Type = typename std::conditional<IsConst, const T, T>::type;

   public:
    using difference_type = int64_t;
    using value_type = Type;
    using pointer = Type*;
    using reference = Type&;
    using iterator_category = std::bidirectional_iterator_tag;
    friend class List;
    explicit ListIterator(const BaseNode* current) :
        current_(const_cast<BaseNode*>(current)) {}

//    ListIterator(const ListIterator& other) {
//      current_ = other.current_;
//    }

//    ListIterator& operator=(ListIterator other) {
//      std::swap(current_, other.current_);
//      return *this;
//    }

    Type& operator*() const { return current_->GetValue(); }

    Type* operator->() const { return current_->GetValuePointer(); }

    ListIterator(const ListIterator<false>& other) : current_(other.current_) {}

    ListIterator& operator=(ListIterator<false> other) {
      std::swap(current_, other.current_);
      return *this;
    }

    // почему то просто оператора приведения типа не достаточно, падает
    // на std::reverse итераторах (не кастуется const к non-const)

//    operator ListIterator<true>() {  // NOLINT
//      return ListIterator<true>(current_);
//    }

    ListIterator& operator++() {
      current_ = current_->next;
      return (*this);
    }

    ListIterator& operator--() {
      current_ = current_->prev;
      return (*this);
    }

    ListIterator operator++(int) {
      ListIterator copy = *this;
      ++(*this);
      return copy;
    }

    ListIterator operator--(int) {
      ListIterator copy = *this;
      --(*this);
      return copy;
    }

    bool operator==(const ListIterator& other) const {
      return (current_ == other.current_);
    }

    bool operator!=(const ListIterator& other) const {
      return (current_ != other.current_);
    }

   private:
    BaseNode* current_;
  };

  using iterator = ListIterator<false>;
  using const_iterator = ListIterator<true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  List() = default;

  List(size_t sz, const T& sample, Alloc alloc)
      : size_(0), node_alloc_(alloc) {
    size_t j = 0;
    while (j < sz) {
      try {
        push_back(sample);
        ++j;
      } catch (...) {
        this->~List();
        throw;
      }
    }
  }

  // К сожалению, не получилось использовать делиг конструктор,
  // т.к. появляется лишнее создание объекта (

  explicit List(size_t sz) : size_(0) {
    size_t j = 0;
    while (j < sz) {
      Node* new_node = AllocTraits::allocate(node_alloc_, 1);
      try {
        AllocTraits::construct(node_alloc_, new_node);
        new_node->next = root_.next;
        new_node->prev = &root_;
        root_.next->prev = static_cast<BaseNode*>(new_node);
        root_.next = static_cast<BaseNode*>(new_node);
        size_ += 1;
        ++j;
      } catch (...) {
        AllocTraits::deallocate(node_alloc_, new_node, 1);
        this->~List();
        throw;
      }
    }
  }

  List(size_t sz, const T& sample) : size_(0) {
    size_t j = 0;
    while (j < sz) {
      try {
        push_back(sample);
        ++j;
      } catch (...) {
        this->~List();
        throw;
      }
    }
  }

  explicit List(Alloc alloc) : size_(0), node_alloc_(alloc) {}

  List(size_t sz, Alloc alloc) : size_(0), node_alloc_(alloc) {
    size_t j = 0;
    while (j < sz) {
      Node* new_node = AllocTraits::allocate(node_alloc_, 1);
      try {
        AllocTraits::construct(node_alloc_, new_node);
        new_node->next = root_.next;
        new_node->prev = &root_;
        root_.next->prev = static_cast<BaseNode*>(new_node);
        root_.next = static_cast<BaseNode*>(new_node);
        size_ += 1;
        ++j;
      } catch (...) {
        AllocTraits::deallocate(node_alloc_, new_node, 1);
        this->~List();
        throw;
      }
    }
  }

  List(const List& other) : node_alloc_(
      AllocTraits::select_on_container_copy_construction(other.node_alloc_)) {
    try {
      for (const auto& node : other) {
        push_back(node);
      }
    } catch (...) {
      this->~List();
      throw;
    }
  }

  List(const List& other, Alloc other_alloc): node_alloc_(other_alloc) {
    try {
      for (const auto& node : other) {
        push_back(node);
      }
    } catch (...) {
      this->~List();
      throw;
    }
  }

  List& operator=(const List& other) {  // copy and swap (strange)
    if constexpr (AllocTraits::propagate_on_container_copy_assignment::value) {
      List other_copy(other, other.node_alloc_);
      std::swap(other_copy.size_, size_);
      std::swap(other_copy.root_, root_);
      std::swap(other_copy.node_alloc_, node_alloc_);
    } else {
      List other_copy(other);
      std::swap(other_copy.size_, size_);
      std::swap(other_copy.root_, root_);
      std::swap(other_copy.node_alloc_, node_alloc_);
    }
    return (*this);
  }

  ~List() {
    BaseNode* current_ptr = root_.next;
    for (size_t j = 0; j < size_; ++j) {
      BaseNode* next_ptr = current_ptr->next;
      AllocTraits::destroy(node_alloc_, reinterpret_cast<Node*>(current_ptr));
      AllocTraits::deallocate(node_alloc_,
                              reinterpret_cast<Node*>(current_ptr), 1);
      current_ptr = next_ptr;
    }
    size_ = 0;
  }

  size_t size() const {
    return size_;
  }

  bool empty() const {
    return (size_ == 0);
  }

  Alloc get_allocator() const { return node_alloc_; } // сам должен скастоваться

  void insert(const_iterator iter, const T& element) {
    Node* new_node = AllocTraits::allocate(node_alloc_, 1);
    try {
      AllocTraits::construct(node_alloc_, new_node, element);
    } catch (...) {
      AllocTraits::deallocate(node_alloc_, new_node, 1);
      throw;
    }
    new_node->next = iter.current_;
    new_node->prev = iter.current_->prev;
    iter.current_->prev->next = static_cast<BaseNode*>(new_node);
    iter.current_->prev = static_cast<BaseNode*>(new_node);
    size_ += 1;
  }

  void erase(const_iterator iter) {
    iter.current_->prev->next = iter.current_->next;
    iter.current_->next->prev = iter.current_->prev;
    AllocTraits::destroy(node_alloc_, reinterpret_cast<Node*>(iter.current_));
    AllocTraits::deallocate(node_alloc_,
                            reinterpret_cast<Node*>(iter.current_), 1);
    size_ -= 1;
  }

  iterator begin() {
    return iterator(root_.next);
  }
  const_iterator begin() const {
    return const_iterator(root_.next);
  }
  iterator end() {
    return iterator(&root_);
  }
  const_iterator end() const {
    return const_iterator(&root_);
  }
  const_iterator cbegin() const {
    return const_iterator(root_.next);
  }
  const_iterator cend() const {
    return const_iterator(&root_);
  }
  reverse_iterator rbegin() {
    return reverse_iterator(end());
  }
  const_reverse_iterator rbegin() const {
    return const_reverse_iterator(cend());
  }
  reverse_iterator rend() {
    return reverse_iterator(begin());
  }
  const_reverse_iterator rend() const {
    return const_reverse_iterator(cbegin());
  }
  const_reverse_iterator crbegin() const {
    return const_reverse_iterator(cend());
  }
  const_reverse_iterator crend() const {
    return const_reverse_iterator(cbegin());
  }

  void push_back(const T& element) {
    insert(end(), element);
  }
  void pop_back() {
    erase(--end());
  }
  void push_front(const T& element) {
    insert(begin(), element);
  }
  void pop_front() {
    erase(begin());
  }

 private:
  BaseNode root_;
  size_t size_{0};
  using NodeAlloc =
      typename std::allocator_traits<Alloc>::template rebind_alloc<Node>;
  using AllocTraits = std::allocator_traits<NodeAlloc>;
  [[no_unique_address]] NodeAlloc node_alloc_;
};
