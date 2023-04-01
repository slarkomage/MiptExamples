#include <iostream>
#include <random>
#include <vector>

class SplayTree {
 public:
  struct Node {
    std::string key;
    std::string value;
    Node* left_child{nullptr};
    Node* right_child{nullptr};
    Node* parent{nullptr};
  };

  SplayTree() = default;

  ~SplayTree() { Clear(); }

  void Insert(const std::string& key, const std::string& value) {
    Node* father = Find(root_, key);
    Node* element = new Node;
    element->key = key;
    element->value = value;
    if (father == nullptr) {
      root_ = element;
      return;
    }
    element->parent = father;
    if (father->key > key) {
      father->left_child = element;
    } else {
      father->right_child = element;
    }
    Splay(element);
  }

  std::string Find(const std::string& key) {
    Node* element = Find(root_, key);
    if (element == nullptr) {
      return "";
    }
    Splay(element);
    if (element->key == key) {
      return element->value;
    }
    return "";
  }

  void Clear() {
    Clear(root_);
    root_ = nullptr;
  }

 private:
  Node* root_ = nullptr;

  static void RightRotate(Node* vertex) {  // то же самое, что и zig
    Node* father = vertex->parent;
    father->left_child = vertex->right_child;
    if (vertex->right_child != nullptr) {
      vertex->right_child->parent = father;
    }
    if (father->parent != nullptr) {
      if (father->parent->left_child == father) {
        father->parent->left_child = vertex;
      } else {
        father->parent->right_child = vertex;
      }
    }
    vertex->parent = father->parent;
    father->parent = vertex;
    vertex->right_child = father;
  }

  static void LeftRotate(Node* vertex) {  // zig, но зеркально
    Node* father = vertex->parent;
    father->right_child = vertex->left_child;
    if (vertex->left_child != nullptr) {
      vertex->left_child->parent = father;
    }
    if (father->parent != nullptr) {
      if (father->parent->left_child == father) {
        father->parent->left_child = vertex;
      } else {
        father->parent->right_child = vertex;
      }
    }
    vertex->parent = father->parent;
    father->parent = vertex;
    vertex->left_child = father;
  }

  void Splay(Node* vertex) {
    Node* father = vertex->parent;
    if (father != nullptr) {
      Node* grand_father = father->parent;
      if (grand_father != nullptr) {
        if ((father == grand_father->left_child) and
            (vertex == father->left_child)) {  // zig-zig
          RightRotate(father);
          RightRotate(vertex);
          Splay(vertex);
        } else if ((father == grand_father->right_child) and
                   (vertex == father->right_child)) {  // Тоже zig-zig
          LeftRotate(father);
          LeftRotate(vertex);
          Splay(vertex);
        } else if ((father == grand_father->left_child) and
                   (vertex == father->right_child)) {  // Tut zig-zag
          LeftRotate(vertex);
          RightRotate(vertex);
          Splay(vertex);
        } else {  // zig-zag
          RightRotate(vertex);
          LeftRotate(vertex);
          Splay(vertex);
        }
      } else {
        if (vertex == father->left_child) {
          RightRotate(vertex);
          Splay(vertex);
        } else {
          LeftRotate(vertex);
          Splay(vertex);
        }
      }
    } else {
      root_ = vertex;
    }
  }

  static Node* Find(Node* start, const std::string& name) {
    if (start == nullptr) {
      return nullptr;
    }
    if (start->key == name) {
      return start;
    }
    if (start->key < name) {
      if (start->right_child != nullptr) {
        return Find(start->right_child, name);
      }
      return start;
    }
    if (start->left_child != nullptr) {
      return Find(start->left_child, name);
    }
    return start;
  }

  static void Clear(Node* start) {
    if (start == nullptr) {
      return;
    }
    if (start->left_child != nullptr) {
      Clear(start->left_child);
    }
    if (start->right_child != nullptr) {
      Clear(start->right_child);
    }
    delete start;
  }
};

void FillingTheData(SplayTree& tree1, SplayTree& tree2) {
  size_t numbers;
  std::cin >> numbers;
  for (size_t i = 0; i < numbers; ++i) {
    std::string racer;
    std::string car;
    std::cin >> racer >> car;
    tree1.Insert(racer, car);
    tree2.Insert(car, racer);
  }
}

void ProcessingTheData(SplayTree& tree1, SplayTree& tree2) {
  size_t amount_requests;
  std::cin >> amount_requests;
  for (size_t i = 0; i < amount_requests; ++i) {
    std::string request;
    std::cin >> request;
    std::cout << (tree1.Find(request) + tree2.Find(request)) << '\n';
  }
}

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);
  SplayTree racers_cars;
  SplayTree cars_racers;
  FillingTheData(racers_cars, cars_racers);
  ProcessingTheData(racers_cars, cars_racers);
}
