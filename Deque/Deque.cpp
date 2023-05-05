#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

template<typename T, bool IsConst = false>
class DequeIt {
 private:
  const int64_t bucket_size = 32;
  T** external_;
  int64_t array;
  int64_t position;
  using Type = typename std::conditional<IsConst, const T, T>::type;
  template<typename K, bool Const>
  friend bool operator<(const DequeIt<K, Const>& a, const DequeIt<K, Const>& b);
  template<typename K, bool Const>
  friend int64_t operator-(const DequeIt<K, Const>& a,
                           const DequeIt<K, Const>& b);
  template<typename K, bool Const>
  friend bool operator==(const DequeIt<K, Const>& a,
                         const DequeIt<K, Const>& b);

 public:
  using difference_type = int64_t;
  using value_type = Type;
  using pointer = Type*;
  using reference = Type&;
  using iterator_category = std::random_access_iterator_tag;

  DequeIt(T** external, int64_t array, int64_t position)
      : external_(external), array(array), position(position) {}

  DequeIt(const DequeIt& other) {
    external_ = other.external_;
    array = other.array;
    position = other.position;
  }

  DequeIt& operator=(DequeIt other) {
    std::swap(other.external_, external_);
    std::swap(other.array, array);
    std::swap(other.position, position);
    return *this;
  }

  Type& operator*() { return (external_[array][position]); }

  Type* operator->() { return (external_[array] + position); }

  operator DequeIt<T, true>() { return DequeIt<T, true>(array, position); }

  std::pair<int, int> GetDecomposition(const int& x) const {
    // x = a * bucket_size + r, где r всегда больше нуля, а - любое целое
    // возвращаем пару <a, r>
    int bucket_size_ = static_cast<int>(bucket_size);
    int a = (x / bucket_size_) - 1 +
        ((bucket_size_ + (x % bucket_size_)) / bucket_size_);
    int r = (bucket_size_ + (x % bucket_size_)) % bucket_size_;
    return {a, r};
  }

  DequeIt& operator+=(const int& value) {
    auto indices = GetDecomposition(value + position);
    array += indices.first;
    position = indices.second;
    return *this;
  }

  DequeIt& operator-=(const int& value) { return (*this += -value); }

  DequeIt& operator--() {
    *this -= 1;
    return *this;
  }

  DequeIt& operator++() {
    *this += 1;
    return *this;
  }

  DequeIt operator++(int) {
    DequeIt copy = *this;
    ++*this;
    return copy;
  }

  DequeIt operator--(int) {
    DequeIt copy = *this;
    --*this;
    return copy;
  }
};

template<typename T, bool IsConst>
int64_t operator-(const DequeIt<T, IsConst>& a, const DequeIt<T, IsConst>& b) {
  if (a.array == b.array) {
    return a.position - b.position;
  }
  int64_t delta = a.position - b.position;
  return delta + ((a.array - b.array) * (a.bucket_size));
}

template<typename T, bool IsConst>
bool operator<(const DequeIt<T, IsConst>& a, const DequeIt<T, IsConst>& b) {
  if (a.array < b.array) {
    return true;
  }
  if (a.array > b.array) {
    return false;
  }
  if (a.position < b.position) {
    return true;
  }
  return false;
}

template<typename T, bool IsConst>
bool operator>(const DequeIt<T, IsConst>& a, const DequeIt<T, IsConst>& b) {
  return b < a;
}

template<typename T, bool IsConst>
bool operator>=(const DequeIt<T, IsConst>& a, const DequeIt<T, IsConst>& b) {
  return !(a < b);
}

template<typename T, bool IsConst>
bool operator<=(const DequeIt<T, IsConst>& a, const DequeIt<T, IsConst>& b) {
  return !(b > a);
}

template<typename T, bool IsConst>
bool operator==(const DequeIt<T, IsConst>& a, const DequeIt<T, IsConst>& b) {
  if (a.position == b.position and a.array == b.array) {
    return true;
  }
  return false;
}

template<typename T, bool IsConst>
bool operator!=(const DequeIt<T, IsConst>& a, const DequeIt<T, IsConst>& b) {
  return !(b == a);
}

template<typename T, bool IsConst>
DequeIt<T, IsConst> operator+(const DequeIt<T, IsConst>& a, const int& b) {
  DequeIt result = a;
  result += b;
  return result;
}

template<typename T, bool IsConst>
DequeIt<T, IsConst> operator-(const DequeIt<T, IsConst>& a, const int& b) {
  DequeIt result = a;
  result -= b;
  return result;
}

template<typename T>
class Deque {
 private:
  const int64_t bucket_size_ = 32;
  int64_t size_; // size of external array!
  T** external_;
  int64_t front_shift;
  int64_t back_shift;
  friend class DequeIt<T>;

  std::pair<int64_t, int64_t> GetDecomposition(const int64_t& x) const {
    // x = a * bucket_size + r, где r всегда больше нуля, а - любое целое
    // возвращаем пару <a, r>
    int64_t a = (x / bucket_size_) - 1 +
        ((bucket_size_ + (x % bucket_size_)) / bucket_size_);
    int64_t r = (bucket_size_ + (x % bucket_size_)) % bucket_size_;
    return {a, r};
  }

 public:
  using iterator = DequeIt<T, false>;
  using const_iterator = DequeIt<T, true>;
  // Конструктор по умолчанию:
  Deque() : size_(0), external_(new T* [2]), front_shift(0), back_shift(0) {}

  // Конструктор от int:
  explicit Deque(const int& sz) : Deque(sz, T()) {}

  // От двух аргументов:
  Deque(const int& sz, const T& obj)
      : size_(((static_cast<int64_t>(sz) / bucket_size_) / 2) * 2 + 2),
        external_(new T* [size_]), front_shift(0), back_shift(0) {
    for (int64_t j = 0; j < size_; ++j) {
      external_[j] = nullptr;
    }
    auto indices1 = GetDecomposition(sz / 2 + sz % 2 - 1);
    auto indices2 = GetDecomposition(-(sz / 2));
    int64_t j = 0;
    int64_t i = 0;
    int64_t k = 1;
    int64_t f = bucket_size_ - 1;
    try {
      for (; j < indices1.first; ++j) {
        external_[(size_ / 2) + j] =
            reinterpret_cast<T*>(new char[sizeof(T) * bucket_size_]);
        for (; i < bucket_size_; ++i) {
          new(external_[(size_ / 2) + j] + i) T(obj);
        }
        i = 0;
      }
      external_[(size_ / 2) + j] =
          reinterpret_cast<T*>(new char[sizeof(T) * bucket_size_]);
      for (; i <= indices1.second; ++i) {
        new(external_[(size_ / 2) + j] + i) T(obj);
      }

      for (; k < abs(indices2.first); ++k) {
        external_[(size_ / 2) - k] =
            reinterpret_cast<T*>(new char[sizeof(T) * bucket_size_]);
        for (; f >= 0; --f) {
          new(external_[(size_ / 2) - k] + f) T(obj);
        }
        f = bucket_size_ - 1;
      }

      f = bucket_size_ - 1;
      external_[(size_ / 2) - k] =
          reinterpret_cast<T*>(new char[sizeof(T) * bucket_size_]);
      for (; f >= indices2.second; --f) {
        new(external_[(size_ / 2) - k] + f) T(obj);
      }

      back_shift = sz / 2 + sz % 2;
      front_shift = sz / 2;
    } catch (...) {
      i -= 1;
      for (; j >= 0; --j) {
        for (; i >= 0; --i) {
          (external_[size_ / 2 + j] + i)->~T();
        }
        i = bucket_size_ - 1;
        delete[] reinterpret_cast<char*>(external_[size_ / 2 + j]);
        external_[size_ / 2 + j] = nullptr;
      }

      f += 1;
      for (; k > 0; --k) {
        for (; f < bucket_size_; ++f) {
          (external_[size_ / 2 - k] + f)->~T();
        }
        f = 0;
        delete[] reinterpret_cast<char*>(external_[size_ / 2 - k]);
        external_[size_ / 2 - k] = nullptr;
      }
      throw;
    }
  }

  // Copy-constructor:
  Deque(const Deque<T>& other)
      : size_(other.size_), external_(new T* [size_]),
        front_shift(other.front_shift), back_shift(other.back_shift) {
    try {
      for (int64_t j = 0; j < size_; ++j) {
        if (other.external_[j] == nullptr) {
          external_[j] = nullptr;
        } else {
          external_[j] =
              reinterpret_cast<T*>(new char[sizeof(T) * bucket_size_]);
        }
      }

      for (int64_t i = std::max(front_shift - front_shift, -front_shift);
           i < back_shift; ++i) {
        new(external_[size_ / 2 + i / bucket_size_] + (i % bucket_size_))
            T(other.external_[size_ / 2 + i / bucket_size_][i % bucket_size_]);
      }

      for (int64_t j = std::max(back_shift - back_shift, -back_shift);
           j < front_shift; ++j) {
        new(external_[size_ / 2 - 1 - j / bucket_size_] +
            (bucket_size_ - 1 - (j % bucket_size_)))
            T(other.external_[size_ / 2 - 1 - j / bucket_size_]
              [bucket_size_ - 1 - (j % bucket_size_)]);
      }
    } catch (...) {
      throw;
    }
  }

  //  Деструктор:
  ~Deque() {
    if (external_ != nullptr) {
      for (int64_t j = 0; j < size_; ++j) {
        if (external_[j] != nullptr) {
          for (int64_t i = 0; i < bucket_size_; ++i) {
            (external_[j] + i)->~T();
          }
          delete[] reinterpret_cast<char*>(external_[j]);
        }
      }
      delete[] external_;
    }
  }

  void swap(Deque<T>& other) { // Copy and swap idiom!
    std::swap(other.size_, size_);
    std::swap(other.front_shift, front_shift);
    std::swap(other.back_shift, back_shift);
    std::swap(other.external_, external_);
  }

  Deque& operator=(Deque<T> other) {
    swap(other);
    return *this;
  }

  size_t size() const { return static_cast<size_t>(front_shift + back_shift); }

  T& operator[](const size_t& index) {
    int64_t delta = static_cast<int64_t>(index) - front_shift;
    auto indices = GetDecomposition(delta);
    return external_[size_ / 2 + indices.first][indices.second];
  }

  const T& operator[](const size_t& index) const {
    int64_t delta = static_cast<int64_t>(index) - front_shift;
    auto indices = GetDecomposition(delta);
    return external_[size_ / 2 + indices.first][indices.second];
  }

  T& at(const size_t& index) {

    if (static_cast<int64_t>(index) > (front_shift + back_shift - 1) or
        static_cast<int64_t>(index) < 0) {
      throw std::out_of_range(std::string("Bruh!"));
    }
    return (*this)[index];
  };

  const T& at(const size_t& index) const {
    if (static_cast<int64_t>(index) > (front_shift + back_shift - 1) or
        static_cast<int64_t>(index) < 0) {
      throw std::out_of_range(std::string("Bruh!"));
    }
    return (*this)[index];
  };

  void push_back(const T& arg) {
    auto indices = GetDecomposition(back_shift);

    if (size_ == 0) {
      T** new_ext = new T* [2];
      new_ext[1] = reinterpret_cast<T*>(new char[sizeof(T) * bucket_size_]);
      new_ext[0] = nullptr;
      try {
        new(new_ext[1]) T(arg);
      } catch (...) {
        delete[] reinterpret_cast<char*>(new_ext[1]);
        delete[] new_ext;
        throw;
      }
      std::swap(external_, new_ext);
      delete[] new_ext;
      back_shift += 1;
      size_ = 2;
      return;
    }

    if (indices.first + size_ / 2 >= size_) {
      T** new_ext = new T* [size_ * 2];

      for (int64_t j = 0; j < size_ / 2; ++j) {
        new_ext[j] = nullptr;
        new_ext[size_ * 2 - 1 - j] = nullptr;
      }
      new_ext[size_ + size_ / 2] =
          reinterpret_cast<T*>(new char[sizeof(T) * bucket_size_]);
      try {
        new(new_ext[size_ + size_ / 2]) T(arg);
      } catch (...) {
        delete[] reinterpret_cast<char*>(new_ext[size_ + size_ / 2]);
        delete[] new_ext;
        throw;
      }
      for (int64_t j = 0; j < size_; ++j) {
        new_ext[size_ / 2 + j] = external_[j];
        external_[j] = nullptr;
      }
      size_ *= 2;
      back_shift += 1;
      std::swap(new_ext, external_);
      delete[] new_ext;
      return;
    }

    if (external_[indices.first + size_ / 2] ==
        nullptr) { // нужно создать новый бакет
      external_[indices.first + size_ / 2] =
          reinterpret_cast<T*>(new char[sizeof(T) * bucket_size_]);
    }
    try {
      new(external_[indices.first + size_ / 2] + indices.second) T(arg);
    } catch (...) {
      throw;
    }
    back_shift += 1;
  };

  void push_front(const T& arg) {
    auto indices = GetDecomposition(-front_shift - 1);
    if (size_ == 0) {
      T** new_ext = new T* [2];
      new_ext[0] = reinterpret_cast<T*>(new char[sizeof(T) * bucket_size_]);
      new_ext[1] = nullptr;
      try {
        new(new_ext[0] + (bucket_size_ - 1)) T(arg);
      } catch (...) {
        delete[] reinterpret_cast<char*>(new_ext[0]);
        delete[] new_ext;
        throw;
      }
      std::swap(external_, new_ext);
      delete[] new_ext;
      front_shift += 1;
      size_ = 2;
      return;
    }

    if ((indices.first + (size_ / 2ll)) < 0l) {
      T** new_ext = new T* [size_ * 2];

      for (int64_t j = 0; j < size_ / 2; ++j) {
        new_ext[j] = nullptr;
        new_ext[size_ * 2 - 1 - j] = nullptr;
      }
      new_ext[size_ / 2 - 1] =
          reinterpret_cast<T*>(new char[sizeof(T) * bucket_size_]);
      try {
        new(new_ext[size_ / 2 - 1] + indices.second) T(arg);
      } catch (...) {
        delete[] reinterpret_cast<char*>(new_ext[size_ / 2 - 1]);
        delete[] new_ext;
        throw;
      }
      for (int64_t j = 0; j < size_; ++j) {
        new_ext[size_ / 2 + j] = external_[j];
        external_[j] = nullptr;
      }
      size_ *= 2;
      front_shift += 1;
      std::swap(new_ext, external_);
      delete[] new_ext;
      return;
    }
    // ты тут
    if (external_[size_ / 2 + indices.first] ==
        nullptr) { // нужно создать новый бакет
      external_[size_ / 2 + indices.first] =
          reinterpret_cast<T*>(new char[sizeof(T) * bucket_size_]);
    }
    try {
      new(external_[size_ / 2 + indices.first] + (indices.second)) T(arg);
    } catch (...) {
      throw;
    }
    front_shift += 1;
  }

  void pop_back() {
    auto indices = GetDecomposition(back_shift - 1);
    (external_[indices.first + size_ / 2] + indices.second)->~T();
    --back_shift;
  }

  T front() {
    auto indices = GetDecomposition(-front_shift);
    return (external_[size_ / 2 + indices.first][indices.second]);
  }

  T back() {
    auto indices = GetDecomposition(back_shift - 1);
    return (external_[size_ / 2 + indices.first][indices.second]);
  }

  void pop_front() {
    auto indices = GetDecomposition(-front_shift);
    (external_[indices.first + size_ / 2] + indices.second)->~T();
    --front_shift;
  }

  iterator begin() {
    auto indices = GetDecomposition(-front_shift);
    return iterator(external_, (size_ / 2 + indices.first), indices.second);
  }

  iterator end() {
    auto indices = GetDecomposition(back_shift);
    return iterator(external_, size_ / 2 + indices.first, indices.second);
  }

  const_iterator begin() const {
    auto indices = GetDecomposition(-front_shift);
    return const_iterator(external_, size_ / 2 + indices.first, indices.second);
  }

  const_iterator end() const {
    auto indices = GetDecomposition(back_shift);
    return const_iterator(external_, size_ / 2 + indices.first, indices.second);
  }

  const_iterator cbegin() const {
    auto indices = GetDecomposition(-front_shift);
    return const_iterator(external_, size_ / 2 + indices.first, indices.second);
  }

  const_iterator cend() const {
    auto indices = GetDecomposition(back_shift);
    return const_iterator(external_, size_ / 2 + indices.first, indices.second);
  }

  std::reverse_iterator<iterator> rbegin() {
    return std::reverse_iterator<iterator>(end());
  }

  std::reverse_iterator<iterator> rend() {
    return std::reverse_iterator<iterator>(begin());
  }

  std::reverse_iterator<const_iterator> rbegin() const {
    return std::reverse_iterator<const_iterator>(end());
  }

  std::reverse_iterator<const_iterator> rend() const {
    return std::reverse_iterator<const_iterator>(begin());
  }

  std::reverse_iterator<const_iterator> crbegin() const {
    return std::reverse_iterator<const_iterator>(end());
  }

  std::reverse_iterator<const_iterator> crend() const {
    return std::reverse_iterator<const_iterator>(begin());
  }

  void insert(iterator it, T obj) {
    if (it == end()) {
      this->push_back(obj);
      return;
    }
    T tmp1 = T(*it);
    *it = obj;
    ++it;
    while (it != end()) {
      T tmp2 = T(*it);
      *it = tmp1;
      tmp1 = T(tmp2);
      ++it;
    }
    this->push_back(tmp1);
  }

  void erase(iterator it) {
    if (it == end()) {
      return;
    }
    iterator it2 = (it + 1);
    while (it2 != end()) {
      *it = T(*it2);
      ++it;
      ++it2;
    }
    this->pop_back();
  }
};
