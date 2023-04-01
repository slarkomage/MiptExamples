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
  friend class Queue;
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
  }

  void Pop() {
    if ((elements_.top_ == nullptr) or (minimums_.top_ == nullptr)) {
      return;
    }
    if (elements_.top_->value == minimums_.top_->value) {
      elements_.Pop();
      minimums_.Pop();
    } else {
      elements_.Pop();
    }
  }

  void Back() {
    if (elements_.size_ == 0) {
      std::cout << "error" << '\n';
    } else {
      std::cout << elements_.top_->value << '\n';
    }
  }

  int Size() const { return elements_.size_; }

  int Top() { return elements_.top_->value; }

  void Min() {
    if (minimums_.top_ == nullptr) {
      std::cout << "error" << '\n';
    } else {
      std::cout << minimums_.top_->value << '\n';
    }
  }

  int MinInt() { return minimums_.top_->value; }

  void Clear() {
    elements_.Clear();
    minimums_.Clear();
  }

 private:
  Stack elements_;
  Stack minimums_;
  friend class Queue;
};

class Queue {
 public:
  void Enqueue(const int& n) {
    left_stack_.Push(n);
    std::cout << "ok" << '\n';
  }

  void Shift() {
    while (left_stack_.Size() != 0) {
      right_stack_.Push(left_stack_.Top());
      left_stack_.Pop();
    }
  }

  void Dequeue() {
    if (right_stack_.Size() == 0) {
      this->Shift();
    }
    if (right_stack_.Size() == 0) {
      std::cout << "error" << '\n';
    } else {
      std::cout << right_stack_.Top() << '\n';
      right_stack_.Pop();
    }
  }

  void Front() {
    if (right_stack_.Size() == 0) {
      this->Shift();
    }
    if (right_stack_.Size() == 0) {
      std::cout << "error" << '\n';
    } else {
      std::cout << right_stack_.Top() << '\n';
    }
  }

  void Size() {
    std::cout << (right_stack_.Size() + left_stack_.Size()) << '\n';
  }

  void Min() {
    if (left_stack_.Size() == 0 and right_stack_.Size() == 0) {
      std::cout << "error" << '\n';
    } else if (left_stack_.Size() == 0 and right_stack_.Size() > 0) {
      right_stack_.Min();
    } else if (left_stack_.Size() > 0 and right_stack_.Size() == 0) {
      left_stack_.Min();
    } else {
      std::cout << std::min(left_stack_.MinInt(), right_stack_.MinInt())
                << '\n';
    }
  }

  void Clear() {
    left_stack_.Clear();
    right_stack_.Clear();
    std::cout << "ok" << '\n';
  }

 private:
  Stack_with_min left_stack_;
  Stack_with_min right_stack_;
};

int main() {
  Queue queue;
  int amount_commands;
  std::cin >> amount_commands;
  for (int j = 0; j < amount_commands; ++j) {
    std::string command;
    std::cin >> command;
    if (command == "enqueue") {
      int number;
      std::cin >> number;
      queue.Enqueue(number);
    } else if (command == "dequeue") {
      queue.Dequeue();
    } else if (command == "size") {
      queue.Size();
    } else if (command == "front") {
      queue.Front();
    } else if (command == "min") {
      queue.Min();
    } else if (command == "clear") {
      queue.Clear();
    }
  }
}
