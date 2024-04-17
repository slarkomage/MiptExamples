#include <array>
#include <cmath>
#include <iostream>
#include <memory>
#include <type_traits>
#include <iterator>
#include <vector>

template <typename T>
struct BaseNode;

template <typename T>
struct Node;

template <typename T, typename Alloc>
class List;

template <typename Key, typename Value>
struct MapNode;

template <typename Key, typename Value, bool IsConst>
class UnorderedMapIterator;

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
class UnorderedMap;

template <typename T>
struct BaseNode {
  BaseNode* next{nullptr};
  BaseNode* prev{nullptr};

  BaseNode() : next(this), prev(this) {}

  BaseNode(BaseNode* next, BaseNode* prev) : next(next), prev(prev) {}

  BaseNode(BaseNode&& other) {
    prev = other.prev == &other ? this : other.prev;
    next = other.next == &other ? this : other.next;
    prev->next = this;
    next->prev = this;
    other.prev = &other;
    other.next = &other;
  }

  BaseNode(const BaseNode& other) : next(other.next),
  prev(other.prev) {}

  BaseNode& operator=(BaseNode&& other)  noexcept {
    prev = other.prev == &other ? this : other.prev;
    next = other.next == &other ? this : other.next;
    prev->next = this;
    next->prev = this;
    other.prev = &other;
    other.next = &other;
    return *this;
  }

  T& GetValue() {
    return (reinterpret_cast<Node<T>*>(this))->element;
  }

  T* GetValuePointer() {
    return &((reinterpret_cast<Node<T>*>(this))->element);
  }
};

template <typename T>
struct Node : BaseNode<T> {
  T element;
  Node() : BaseNode<T>() {}
  explicit Node(const T& element) : BaseNode<T>(), element(element) {}
  explicit Node(T&& element) : BaseNode<T>(), element(std::move(element)) {}
  explicit Node(Node&& other) : BaseNode<T>(std::move(other)),
                       element(std::move(element)) {}
  template <typename... Args>
  explicit Node(Args&&... args) : BaseNode<T>(), element(std::forward<Args>(args)...) {}
};

template <typename T, typename Alloc = std::allocator<T>>
class List {
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

    ListIterator(const BaseNode<T>* current) :
        current_(const_cast<BaseNode<T>*>(current)) {}

    ListIterator(const BaseNode<T>& current) :
        current_(const_cast<BaseNode<T>*>(&current)) {}

    Type& operator*() const { return current_->GetValue(); }

    Type* operator->() const { return current_->GetValuePointer(); }

    ListIterator(const ListIterator<false>& other) : current_(other.current_) {}

    ListIterator& operator=(ListIterator<false> other) {
      std::swap(current_, other.current_);
      return *this;
    }

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

    BaseNode<T>* GetPtr() { return current_; }

    const BaseNode<T>* GetPtr() const { return current_; }

   private:
    BaseNode<T>* current_;
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
      Node<T>* new_node = AllocTraits::allocate(node_alloc_, 1);
      try {
        AllocTraits::construct(node_alloc_, new_node);
        new_node->next = root_.next;
        new_node->prev = &root_;
        root_.next->prev = static_cast<BaseNode<T>*>(new_node);
        root_.next = static_cast<BaseNode<T>*>(new_node);
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
      Node<T>* new_node = AllocTraits::allocate(node_alloc_, 1);
      try {
        AllocTraits::construct(node_alloc_, new_node);
        new_node->next = root_.next;
        new_node->prev = &root_;
        root_.next->prev = static_cast<BaseNode<T>*>(new_node);
        root_.next = static_cast<BaseNode<T>*>(new_node);
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

  List(List&& other) noexcept {
    BaseNode<T>* current_ptr = root_.next;
    for (size_t j = 0; j < size_; ++j) {
      BaseNode<T>* next_ptr = current_ptr->next;
      AllocTraits::destroy(node_alloc_,
                           reinterpret_cast<Node<T>*>(current_ptr));
      AllocTraits::deallocate(node_alloc_,
                              reinterpret_cast<Node<T>*>(current_ptr), 1);
      current_ptr = next_ptr;
    }
    root_ = std::move(other.root_);
    size_ = other.size_;
    node_alloc_ = other.node_alloc_;
    other.size_ = 0;
  }

  List(const List& other, Alloc other_alloc) : node_alloc_(other_alloc) {
    try {
      for (const auto& node : other) {
        push_back(node);
      }
    } catch (...) {
      this->~List();
      throw;
    }
  }

  List& operator=(List&& other) noexcept {
    List copy(std::move(other));
    std::swap(size_, copy.size_);
    std::swap(node_alloc_, copy.node_alloc_);
    std::swap(root_, copy.root_);
    return (*this);
  }

  void list_swap(List& other) {
    std::swap(other.size_, size_);
    std::swap(other.root_, root_);
    std::swap(other.node_alloc_, node_alloc_);
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
    BaseNode<T>* current_ptr = root_.next;
    for (size_t j = 0; j < size_; ++j) {
      BaseNode<T>* next_ptr = current_ptr->next;
      AllocTraits::destroy(node_alloc_,
                           reinterpret_cast<Node<T>*>(current_ptr));
      AllocTraits::deallocate(node_alloc_,
                              reinterpret_cast<Node<T>*>(current_ptr), 1);
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

  Alloc get_allocator() const { return node_alloc_; }

  void insert(const_iterator pos, const T& element) {
    Node<T>* new_node = AllocTraits::allocate(node_alloc_, 1);
    try {
      AllocTraits::construct(node_alloc_, new_node, element);
    } catch (...) {
      AllocTraits::deallocate(node_alloc_, new_node, 1);
      throw;
    }
    new_node->next = pos.current_;
    new_node->prev = pos.current_->prev;
    pos.current_->prev->next = static_cast<BaseNode<T>*>(new_node);
    pos.current_->prev = static_cast<BaseNode<T>*>(new_node);
    size_ += 1;
  }

  template <typename... Args>
  void emplace(const_iterator pos, Args&& ... args) {
    Node<T>* new_node = AllocTraits::allocate(node_alloc_, 1);
    try {
      AllocTraits::construct(node_alloc_, new_node,
                             std::forward<Args>(args)...);
    } catch (...) {
      AllocTraits::deallocate(node_alloc_, new_node, 1);
      throw;
    }
    BaseNode<T>* base_new_node = reinterpret_cast<BaseNode<T>*>(new_node);
    base_new_node->next = pos.current_;
    base_new_node->prev = pos.current_->prev;
    pos.current_->prev->next = base_new_node;
    pos.current_->prev = base_new_node;
    size_ += 1;
  }

  void erase(const_iterator iter) {
    iter.current_->prev->next = iter.current_->next;
    iter.current_->next->prev = iter.current_->prev;
    AllocTraits::destroy(node_alloc_,
                         reinterpret_cast<Node<T>*>(iter.current_));
    AllocTraits::deallocate(node_alloc_,
                            reinterpret_cast<Node<T>*>(iter.current_), 1);
    size_ -= 1;
  }

  void cut_node(BaseNode<T>* ptr) {
    ptr->next->prev = ptr->prev;
    ptr->prev->next = ptr->next;
    ptr->next = ptr->prev = nullptr;
    size_ -= 1;
  }

  void insert_after(BaseNode<T>* pos, BaseNode<T>* ptr) {
    ptr->prev = pos;
    ptr->next = pos->next;
    if (pos->next != nullptr) {
      pos->next->prev = ptr;
    }
    pos->next = ptr;
    size_ += 1;
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

  template <typename... Args>
  void emplace_back(Args&& ... args) {
    emplace(cend(), std::forward<Args>(args)...);
  }

  template <typename... Args>
  void emplace_front(Args&& ... args) {
    emplace(cbegin(), std::forward<Args>(args)...);
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
  BaseNode<T> root_;
  size_t size_{0};
  using NodeAlloc =
      typename std::allocator_traits<Alloc>::template rebind_alloc<Node<T>>;
  using AllocTraits = std::allocator_traits<NodeAlloc>;
  [[no_unique_address]] NodeAlloc node_alloc_;
};

template <typename Key, typename Value>
struct MapNode {
  using NodeType = std::pair<const Key, Value>;
  MapNode() = default;
  MapNode(const MapNode& other) = default;
  MapNode(MapNode&& other) = default;
  MapNode(size_t hash, NodeType* ptr) : hash_(hash), pair_ptr(ptr) {}

  size_t GetHash() const {
    return hash_;
  }

  NodeType* GetPointer() const {
    return pair_ptr;
  }

  NodeType& GetReference() const {
    return (*pair_ptr);
  }

 private:
  size_t hash_;
  NodeType* pair_ptr;
};

template <typename Key, typename Value, bool IsConst>
class UnorderedMapIterator {
 private:
  using PairType = std::pair<const Key, Value>;
  using MapNodeType = MapNode<Key, Value>;
  using ListBaseNodeType = BaseNode<MapNodeType>;
  using ListNodeType = Node<MapNodeType>;
  template <typename>
  friend bool operator!=(const UnorderedMapIterator& it1,
                         const UnorderedMapIterator& it2);
 public:
  ListBaseNodeType* pos_{nullptr};
  using difference_type = int64_t;
  using value_type = typename std::conditional_t<IsConst, const PairType, PairType>;
  using pointer = typename std::conditional_t<IsConst, const PairType*, PairType*>;
  using reference = typename std::conditional_t<IsConst, const PairType&, PairType&>;
  using iterator_categoty = std::bidirectional_iterator_tag;

  UnorderedMapIterator(const ListBaseNodeType* ptr) :
      pos_(const_cast<ListBaseNodeType*>(ptr)) {}

  UnorderedMapIterator(const UnorderedMapIterator<Key, Value, false>& other)
      : pos_(other.pos_) {}

  UnorderedMapIterator& operator=(UnorderedMapIterator<Key, Value, false>
                                  other) {
    std::swap(pos_, other.pos_);
    return *this;
  }

  UnorderedMapIterator() = default;
  ~UnorderedMapIterator() = default;

  UnorderedMapIterator& operator++() {
    pos_ = pos_->next;
    return *this;
  }

  UnorderedMapIterator operator++(int) {
    UnorderedMapIterator copy(*this);
    ++(*this);
    return copy;
  }

  UnorderedMapIterator& operator--() {
    pos_ = pos_->prev;
    return *this;
  }

  UnorderedMapIterator operator--(int) {
    UnorderedMapIterator copy(*this);
    --(*this);
    return copy;
  }

  ListBaseNodeType* GetListPtr() const {
    return pos_;
  }

  pointer operator->() const {
    return pos_->GetValuePointer()->GetPointer();
  }

  reference operator*() const {
    return pos_->GetValuePointer()->GetReference();
  }

  size_t GetHash() const {
    return pos_->GetValuePointer()->GetHash();
  }
};

template <typename Key, typename Value, bool IsConst>
bool operator!=(const UnorderedMapIterator<Key, Value, IsConst>& it1,
                const UnorderedMapIterator<Key, Value, IsConst>& it2) {
  return (it1.pos_ != it2.pos_);
}

template <typename Key, typename Value, bool IsConst>
bool operator==(const UnorderedMapIterator<Key, Value, IsConst>& it1,
                const UnorderedMapIterator<Key, Value, IsConst>& it2) {
  return !(it1 != it2);
}

template <typename Key, typename Value, typename Hash = std::hash<Key>,
    typename Equal = std::equal_to<Key>,
    typename Alloc = std::allocator<std::pair<const Key, Value>>>
class UnorderedMap {
 public:
  using NodeType = std::pair<const Key, Value>;
  using iterator = UnorderedMapIterator<Key, Value, false>;
  using const_iterator = UnorderedMapIterator<Key, Value, true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

 private:
  using NodeTypeAlloc = Alloc;
  using NodeTypeAllocTraits = typename std::allocator_traits<NodeTypeAlloc>;

  using MapNodeType = MapNode<Key, Value>;
  using MapNodeAlloc =
      typename NodeTypeAllocTraits::template rebind_alloc<MapNodeType>;
  using MapNodeAllocTraits = typename std::allocator_traits<MapNodeAlloc>;

  using ListBaseNodeType = BaseNode<MapNodeType>;
  using ListNodeType = Node<MapNodeType>;
  using ListNodeAlloc =
      typename NodeTypeAllocTraits::template rebind_alloc<ListNodeType>;
  using ListNodeAllocTraits = typename std::allocator_traits<ListNodeAlloc>;

  using ArrayAlloc =
      typename NodeTypeAllocTraits::template rebind_alloc<ListBaseNodeType*>;
  using ListType = List<MapNodeType, MapNodeAlloc>;
  using ArrayType = std::vector<ListBaseNodeType*, ArrayAlloc>;

  static constexpr size_t default_bucket_amount = 16;
  static constexpr float default_max_load_factor = 0.95;

  ArrayType buckets_;
  ListType list_;
  [[no_unique_address]] Alloc allocator_;
  float max_load_factor_ = default_max_load_factor;

 public:
  UnorderedMap() : buckets_(default_bucket_amount, nullptr) {}

  UnorderedMap(const UnorderedMap& other)
      : buckets_(other.buckets_.size(), nullptr),
        allocator_(NodeTypeAllocTraits::select_on_container_copy_construction(
            other.allocator_)),
        max_load_factor_(other.max_load_factor_) {
    try {
      for (auto it = other.begin(); it != other.end(); ++it) {
        emplace(*it);
      }
    } catch (...) {
      this->~UnorderedMap();
      throw;
    }
  }

  UnorderedMap(UnorderedMap&& other)
  noexcept: buckets_(std::move(other.buckets_)), list_(std::move(other.list_)),
            allocator_(other.allocator_),
            max_load_factor_(other.max_load_factor_) {
    other.buckets_ = ArrayType(default_bucket_amount, nullptr);
  }

  ~UnorderedMap() {
    for (iterator it = begin(); it != end(); ++it) {
      NodeTypeAllocTraits::destroy(allocator_, &(*it));
      NodeTypeAllocTraits::deallocate(allocator_, &(*it), 1);
    }
  }

  void swap(UnorderedMap& other) {
    if constexpr (NodeTypeAllocTraits::propagate_on_container_swap::value) {
      allocator_.swap(other.allocator_);
    } else {
      std::swap(allocator_, other.allocator_);
    }
    list_.list_swap(other.list_);
    std::swap(buckets_, other.buckets_);
    std::swap(max_load_factor_, other.max_load_factor_);
  }

  UnorderedMap& operator=(const UnorderedMap& other) {
    UnorderedMap copy(other);
    swap(copy);
    return *this;
  }

  UnorderedMap& operator=(UnorderedMap&& other) noexcept {
    list_ = std::move(other.list_);
    buckets_ = std::move(other.buckets_);
    max_load_factor_ = other.max_load_factor_;

    other.buckets_ = ArrayType (default_bucket_amount, nullptr);
    return *this;
  }

  iterator find(const Key& key) {
    size_t hash = Hash()(key);
    auto pos = buckets_[hash % buckets_.size()];
    if (pos == nullptr) {
      return end();
    }
    iterator it(pos);
    while (it != end() and (it.GetHash() % buckets_.size() ==
        hash % buckets_.size()) and !Equal()(key, (*it).first)) {
      ++it;
    }
    if (it == end() or (it.GetHash() % buckets_.size() !=
        hash % buckets_.size())) {
      return end();
    }
    return it;
  }

  const_iterator find(const Key& key) const {
    size_t hash = Hash()(key);
    auto pos = buckets_[hash % buckets_.size()];
    if (pos == nullptr) {
      return cend();
    }
    const_iterator it(pos);
    while (it != cend() and (it.GetHash() % buckets_.size() ==
        hash % buckets_.size()) and !Equal()(key, (*it).first)) {
      ++it;
    }
    if (it == cend() or (it.GetHash() % buckets_.size() !=
        hash % buckets_.size())) {
      return cend();
    }
    return it;
  }

  Value& at(const Key& key) {
    auto pos = find(key);
    if (pos == end()) {
      throw std::out_of_range("Bruh!");
    }
    return pos->second;
  }

  const Value& at(const Key& key) const {
    auto pos = find(key);
    if (pos == end()) {
      throw std::out_of_range("Bruh!");
    }
    return pos->second;
  }

  Value& operator[](const Key& key) {
    auto pos = find(key);
    if (pos == end()) {
      pos = (emplace(key, std::move(Value()))).first;
      return (*pos).second;
    }
    return (*pos).second;
  }

  size_t size() const { return list_.size(); }

  float max_load_factor() const {
    return max_load_factor_;
  }

  float load_factor() const {
    return size() / buckets_.size();
  }

  void max_load_factor(float max_factor) {
    max_load_factor_ = max_factor;
  }

  void erase(const_iterator it) {
    size_t hash = it.GetHash();
    auto node_ptr = const_cast<NodeType*>(it.operator->());
    NodeTypeAllocTraits::destroy(allocator_, node_ptr);
    NodeTypeAllocTraits::deallocate(allocator_, node_ptr, 1);
    size_t pos = hash % buckets_.size();
    if (it.GetListPtr() == buckets_[pos]) {
      auto next = it;
      ++next;
      if ((next == cend()) or (next.GetHash() % buckets_.size() != pos)) {
        buckets_[pos] = nullptr;
      } else {
        buckets_[pos] = next.GetListPtr();
      }
    }
    list_.cut_node(it.GetListPtr());
  }

  template <typename... Args>
  std::pair<iterator, bool> emplace(Args&&... args) {
    NodeType* new_node = NodeTypeAllocTraits::allocate(allocator_, 1);
    try {
      NodeTypeAllocTraits::construct(allocator_, new_node,
                                     std::forward<Args>(args)...);
    } catch (...) {
      NodeTypeAllocTraits::deallocate(allocator_, new_node, 1);
      throw;
    }

    size_t hash = Hash()(new_node->first);
    if (buckets_[hash % buckets_.size()] == nullptr) {
      list_.emplace_front(hash, new_node);
      buckets_[hash % buckets_.size()] = (list_.begin()).GetPtr();

      if (load_factor() > max_load_factor_) {
        rehash(2 * buckets_.size());
      }
      return {begin(), true};
    }

    iterator it(buckets_[hash % buckets_.size()]);
    while (it != end() and (it.GetHash() % buckets_.size()) ==
    (hash % buckets_.size()) and !Equal()(new_node->first, (*it).first)) {
      ++it;
    }

    if (it == end() or it.GetHash() != hash) {
      list_.emplace(typename ListType::const_iterator(it.GetListPtr()),
                    hash, new_node);

      if (load_factor() > max_load_factor_) {
        rehash(2 * buckets_.size());
      }
      return {--it, true};
    }

    NodeTypeAllocTraits::destroy(allocator_, new_node);
    NodeTypeAllocTraits::deallocate(allocator_, new_node, 1);
    return {end(), false};
  }

  void rehash(size_t buckets_amount) {
    buckets_ = ArrayType(buckets_amount, nullptr);
    for (auto it = begin(); it != end(); ++it) {
      size_t hash = it.GetHash();
      size_t pos = hash % buckets_amount;
      if (buckets_[pos] == nullptr) {
        buckets_[pos] = it.GetListPtr();
      } else {
        list_.insert_after(buckets_[pos], it.GetListPtr());
      }
    }
  }

  void reserve(size_t size) {
    if (size > std::ceil(max_load_factor_ * buckets_.size())) {
      rehash(std::ceil(size / max_load_factor_));
    }
  }

  template <typename InputIterator>
  void insert(InputIterator begin, InputIterator end) {
    for (; begin != end; ++begin) {
      emplace(std::forward<decltype(*begin)>(*begin));
    }
  }

  std::pair<iterator, bool> insert(const NodeType& pair) {
    return emplace(pair);
  }

  std::pair<iterator, bool> insert(NodeType&& pair) {
    return emplace(std::move(pair));
  }

  template <typename U>
  std::pair<iterator, bool> insert(U&& node) {
    return emplace(std::move(node));
  }

  template <typename InputIterator>
  void erase(InputIterator begin, InputIterator end) {
    for (; begin != end;) {
      erase(begin++);
    }
  }

  iterator begin() {
    return iterator(list_.begin().GetPtr());
  }

  iterator end() {
    return iterator(list_.end().GetPtr());
  }

  const_iterator cend() const {
    return const_iterator(list_.end().GetPtr());
  }

  const_iterator cbegin() const {
    return const_iterator(list_.begin().GetPtr());
  }

  const_iterator begin() const {
    return cbegin();
  }

  const_iterator end() const {
    return cend();
  }

  reverse_iterator rbegin() {
    return reverse_iterator(end());
  }

  const_reverse_iterator crbegin() const {
    return const_reverse_iterator(cend());
  }

  const_reverse_iterator rbegin() const {
    return crbegin();
  }

  reverse_iterator rend() {
    return reverse_iterator(begin());
  }

  const_reverse_iterator crend() const {
    return const_reverse_iterator(cbegin());
  }

  const_reverse_iterator rend() const {
    return crend();
  }
};
