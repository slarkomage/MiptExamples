#include <algorithm>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>

class List {
 public:
  struct Node {
    int value;
    Node* next{nullptr};
    Node* prev{nullptr};
  };

  List() : root_(nullptr), size_(0) {}

  List(const List& other) : root_(nullptr), size_(0) {
    if (other.size_ == 0) {
      return;
    }
    Node* curr = other.root_;
    Node* new_root = new Node;
    new_root->value = curr->value;
    root_ = new_root;
    Node* last = root_;
    size_ += 1;
    for (size_t i = 1; i < other.size_; ++i) {
      curr = curr->next;
      Node* new_node = new Node;
      new_node->value = curr->value;
      new_node->prev = last;
      last->next = new_node;
      last = new_node;
      size_ += 1;
    }
  }

  void Swap(List& other) {
    std::swap(root_, other.root_);
    std::swap(size_, other.size_);
  }

  List& operator=(List other) {
    Swap(other);
    return *this;
  }

  ~List() {
    while (root_ != nullptr) {
      Erase(root_->value);
    }
  }

  void Insert(const int& number) {
    Node* element = new Node;
    element->value = number;
    element->prev = nullptr;
    element->next = root_;
    if (root_ != nullptr) {
      root_->prev = element;
    }
    root_ = element;
    size_ += 1;
  }

  void Erase(const int& number) { Erase(root_, number); }

  bool Find(const int& number) { return Find(root_, number); }

  size_t Size() const { return size_; }

  int Back() { return root_->value; }

 private:
  Node* root_;
  size_t size_;

  void Erase(Node* current, const int& number) {
    if (current == nullptr) {
      return;
    }
    if (current->value == number) {
      if (current == root_) {
        root_ = current->next;
        if (root_ != nullptr) {
          root_->prev = nullptr;
        }
        delete current;
        size_ -= 1;
        return;
      }
      current->prev->next = current->next;
      if (current->next != nullptr) {
        current->next->prev = current->prev;
      }
      delete current;
      size_ -= 1;
      return;
    }
    Erase(current->next, number);
  }

  bool Find(Node* current, const int& number) {
    if (current == nullptr) {
      return false;
    }
    if (current->value == number) {
      return true;
    }
    return Find(current->next, number);
  }
};

class HashTable {
 public:
  HashTable() : size_(0), capacity_(kDefaultCapacity) {
    a_ = random_device() % kBound;
    b_ = random_device() % kBound;
    data_ = std::vector<List>(kDefaultCapacity, List());
  }

  void Insert(const int& number) {
    if (Find(number)) {
      return;
    }
    size_t index = Hash(number);
    data_[index].Insert(number);
    size_ += 1;
    double load_factor =
        static_cast<double>(size_) / static_cast<double>(capacity_);
    if (load_factor >= kMaxLoadFactor) {
      ReHash();
    }
  }

  bool Find(const int& number) {
    size_t index = Hash(number);
    return data_[index].Find(number);
  }

  void Erase(const int& number) {
    if (Find(number)) {
      size_t index = Hash(number);
      data_[index].Erase(number);
      size_ -= 1;
    } else {
      return;
    }
  }

 private:
  const size_t kBound = 2147483647;
  const size_t kDefaultCapacity = 4;
  const double kMaxLoadFactor = 0.95;
  std::vector<List> data_;
  size_t size_;
  size_t capacity_;
  size_t a_, b_;
  static std::random_device random_device;

  size_t Hash(const int& number) const {
    size_t tmp = (a_ * static_cast<size_t>(number) + b_) % kBound;
    return tmp % capacity_;
  }

  void ReHash() {
    a_ = random_device() % kBound;
    b_ = random_device() % kBound;
    std::vector<List> old_data = data_;
    capacity_ *= 2;
    data_ = std::vector<List>(capacity_, List());
    for (size_t i = 0; i < (capacity_ / 2); ++i) {
      if (old_data[i].Size() != 0) {
        while (old_data[i].Size() != 0) {
          int element = old_data[i].Back();
          old_data[i].Erase(element);
          size_t index = Hash(element);
          data_[index].Insert(element);
        }
      }
    }
  }
};

std::random_device HashTable::random_device;

void ProcessingRequests(HashTable& hash_table) {
  size_t amount_requests;
  std::cin >> amount_requests;
  for (size_t i = 0; i < amount_requests; ++i) {
    char command;
    std::cin >> command;
    int number;
    std::cin >> number;
    if (command == '+') {
      hash_table.Insert(number);
    } else if (command == '-') {
      hash_table.Erase(number);
    } else if (command == '?') {
      if (hash_table.Find(number)) {
        std::cout << "YES" << '\n';
      } else {
        std::cout << "NO" << '\n';
      }
    }
  }
}

int main() {
  HashTable hash_table;
  ProcessingRequests(hash_table);
}
