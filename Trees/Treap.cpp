#include <iostream>
#include <random>
#include <vector>

class Treap {
 public:
  struct Node {
    long long key;
    long long priority;
    long long size_subtree;
    Node* left_child{nullptr};
    Node* right_child{nullptr};
  };

  Treap() = default;

  ~Treap() { Clear(); }

  void Insert(long long value) { root_ = Insert(root_, value); }

  void Delete(long long value) { root_ = Delete(root_, value); }

  bool Find(long long value) { return Find(root_, value); }

  void NextX(long long value) { root_ = Next(root_, value); }

  void PrevX(long long value) { root_ = Prev(root_, value); }

  void Kth(long long number) { root_ = Kth(root_, number); }

  void Clear() {
    Clear(root_);
    root_ = nullptr;
  }

 private:
  Node* root_ = nullptr;
  static std::random_device random_device;

  static std::pair<Node*, Node*> Split(Node* tree_root, long long value) {
    if (tree_root == nullptr) {
      return {nullptr, nullptr};
    }
    if (tree_root->key > value) {
      if (tree_root->left_child != nullptr) {
        tree_root->size_subtree -= tree_root->left_child->size_subtree;
      }
      std::pair<Node*, Node*> left_split = Split(tree_root->left_child, value);
      tree_root->left_child = left_split.second;
      if (left_split.second != nullptr) {
        tree_root->size_subtree += left_split.second->size_subtree;
      }
      return {left_split.first, tree_root};
    }
    if (tree_root->right_child != nullptr) {
      tree_root->size_subtree -= tree_root->right_child->size_subtree;
    }
    std::pair<Node*, Node*> right_split = Split(tree_root->right_child, value);
    tree_root->right_child = right_split.first;
    if (right_split.first != nullptr) {
      tree_root->size_subtree += right_split.first->size_subtree;
    }
    return {tree_root, right_split.second};
  }

  static Node* Merge(Node* tree1, Node* tree2) {
    if (tree1 == nullptr) {
      return tree2;
    }
    if (tree2 == nullptr) {
      return tree1;
    }
    if (tree1->priority < tree2->priority) {
      tree1->size_subtree += tree2->size_subtree;
      tree1->right_child = Merge(tree1->right_child, tree2);
      return tree1;
    }
    tree2->size_subtree += tree1->size_subtree;
    tree2->left_child = Merge(tree1, tree2->left_child);
    return tree2;
  }

  static bool Find(Node* start, long long value) {
    if (start == nullptr) {
      return false;
    }
    if (start->key == value) {
      return true;
    }
    if (start->key < value) {
      return Find(start->right_child, value);
    }
    return Find(start->left_child, value);
  }

  static Node* Insert(Node* root, long long value) {
    if (Find(root, value)) {
      return root;
    }
    std::pair<Node*, Node*> splited = Split(root, value);
    Node* new_node = new Node;
    new_node->key = value;
    new_node->priority = random_device();
    new_node->size_subtree = 1;
    return Merge(Merge(splited.first, new_node), splited.second);
  }

  static Node* Delete(Node* root, long long value) {
    if (!Find(root, value)) {
      return root;
    }
    std::pair<Node*, Node*> splitted1 = Split(root, value);
    std::pair<Node*, Node*> splitted2 = Split(splitted1.first, value - 1);
    delete splitted2.second;
    return Merge(splitted2.first, splitted1.second);
  }

  static Node* Next(Node* root, long long value) {
    std::pair<Node*, Node*> splitted = Split(root, value);
    if (splitted.second == nullptr) {
      std::cout << "none" << '\n';
    } else {
      Node* curr = splitted.second;
      while (curr->left_child != nullptr) {
        curr = curr->left_child;
      }
      std::cout << curr->key << '\n';
    }
    return Merge(splitted.first, splitted.second);
  }

  static Node* Prev(Node* root, long long value) {
    std::pair<Node*, Node*> splitted = Split(root, value - 1);
    if (splitted.first == nullptr) {
      std::cout << "none" << '\n';
    } else {
      Node* curr = splitted.first;
      while (curr->right_child != nullptr) {
        curr = curr->right_child;
      }
      std::cout << curr->key << '\n';
    }
    return Merge(splitted.first, splitted.second);
  }

  static std::pair<Node*, Node*> SplitBySize(Node* root, long long number) {
    if (root == nullptr) {
      return {nullptr, nullptr};
    }

    if (root->size_subtree < number) {
      return {nullptr, root};
    }

    long long left_size;
    if (root->left_child == nullptr) {
      left_size = 0;
    } else {
      left_size = root->left_child->size_subtree;
    }

    if (left_size >= number) {
      root->size_subtree -= left_size;
      auto left_split = SplitBySize(root->left_child, number);
      root->left_child = left_split.second;
      if (left_split.second != nullptr) {
        root->size_subtree += left_split.second->size_subtree;
      }
      return {left_split.first, root};
    }
    if (root->right_child != nullptr) {
      root->size_subtree -= root->right_child->size_subtree;
    }
    auto right_split = SplitBySize(root->right_child, number - left_size - 1);
    root->right_child = right_split.first;
    if (right_split.first != nullptr) {
      root->size_subtree += right_split.first->size_subtree;
    }
    return {root, right_split.second};
  }

  static Node* Kth(Node* root, long long number) {
    auto splitted = SplitBySize(root, number);
    if (splitted.first == nullptr) {
      std::cout << "none" << '\n';
    } else {
      Node* curr = splitted.first;
      while (curr->right_child != nullptr) {
        curr = curr->right_child;
      }
      std::cout << curr->key << '\n';
    }
    return Merge(splitted.first, splitted.second);
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

void Proccessing(Treap& treap, const std::string& command,
                 const long long& number) {
  if (command == "insert") {
    treap.Insert(number);
  } else if (command == "delete") {
    treap.Delete(number);
  } else if (command == "exists") {
    if (treap.Find(number)) {
      std::cout << "true" << '\n';
    } else {
      std::cout << "false" << '\n';
    }
  } else if (command == "next") {
    treap.NextX(number);
  } else if (command == "prev") {
    treap.PrevX(number);
  } else if (command == "kth") {
    treap.Kth(number + 1);
  }
}

std::random_device Treap::random_device;

int main() {
  Treap treap;
  std::string command;
  long long number;
  while (std::cin >> command) {
    std::cin >> number;
    Proccessing(treap, command, number);
  }
}
