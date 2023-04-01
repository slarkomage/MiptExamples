#include <iostream>

class Stack {
 public:
  struct Node {
    int value;
    Node* prev;
  };

  Stack() : size_(0), top_(nullptr) {}

  void Push(const int& n) {
    Node* element = new Node;
    element->value = n;
    element->prev = this->top_;
    this->top_ = element;
    this->size_ += 1;
  }

  void Pop() {
    if (this->size_ == 0) {
      return;
    }
    Node* old_top = this->top_;
    this->top_ = this->top_->prev;
    this->size_ -= 1;
    delete old_top;
  }

  void Back() {
    if (this->size_ == 0) {
      std::cout << "error" << '\n';
    } else {
      std::cout << this->top_->value << '\n';
    }
  }

  void Size() const { std::cout << this->size_ << '\n'; }

  void Clear() {
    Clear(this->top_);
    this->top_ = nullptr;
    this->size_ = 0;
  }

 private:
  int size_;
  Node* top_;

  void Clear(Node* vertex) {
    if (vertex == nullptr) {
      return;
    }
    if (vertex->prev != nullptr) {
      Clear(vertex->prev);
      delete vertex;
    } else {
      delete vertex;
    }
  }

  friend class Stack_with_min;
};

class Stack_with_min {
 public:
  ~Stack_with_min() {
    elements_.Clear();
    minimums_.Clear();
  }

  void Push(const int& n) {
    elements_.Push(n);
    if ((minimums_.top_ == nullptr) or (n <= minimums_.top_->value)) {
      minimums_.Push(n);
    }
    std::cout << "ok" << '\n';
  }

  void Pop() {
    if ((elements_.top_ == nullptr) or (minimums_.top_ == nullptr)) {
      std::cout << "error" << '\n';
    } else {
      std::cout << elements_.top_->value << '\n';
      if (elements_.top_->value == minimums_.top_->value) {
        elements_.Pop();
        minimums_.Pop();
      } else {
        elements_.Pop();
      }
    }
  }

  void Back() {
    if (elements_.size_ == 0) {
      std::cout << "error" << '\n';
    } else {
      std::cout << elements_.top_->value << '\n';
    }
  }

  void Size() const { std::cout << elements_.size_ << '\n'; }

  void Min() {
    if (minimums_.top_ == nullptr) {
      std::cout << "error" << '\n';
    } else {
      std::cout << minimums_.top_->value << '\n';
    }
  }

  void Clear() {
    elements_.Clear();
    minimums_.Clear();
    std::cout << "ok" << '\n';
  }

 private:
  Stack elements_;
  Stack minimums_;
};

int main() {
  Stack_with_min stack_with_min;
  int amount_commands;
  std::cin >> amount_commands;
  for (int j = 0; j < amount_commands; ++j) {
    std::string command;
    std::cin >> command;
    if (command == "push") {
      int n;
      std::cin >> n;
      stack_with_min.Push(n);
    } else if (command == "pop") {
      stack_with_min.Pop();
    } else if (command == "size") {
      stack_with_min.Size();
    } else if (command == "back") {
      stack_with_min.Back();
    } else if (command == "min") {
      stack_with_min.Min();
    } else if (command == "clear") {
      stack_with_min.Clear();
    }
  }
}
