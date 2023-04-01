#include <algorithm>
#include <iostream>
#include <vector>
class DST {
 public:
  DST(size_t n): parent_(n), rank_(n, 0) {
    for (size_t j = 0; j < n; ++j) {
      parent_[j] = j;
    }
  }

  bool IsSame(size_t u, size_t v) {
    return Find(u) == Find(v);
  }

  size_t Find(size_t v) { // ищем корень
    if (parent_[v] == v) return v;
    parent_[v] = Find(parent_[v]);
    return parent_[v];
  }

  void Union(size_t u, size_t v) {
    size_t root_u = Find(u);
    size_t root_v = Find(v);
    if (root_u != root_v) {
      if (rank_[root_u] < rank_[root_v]) {
        std::swap(root_u, root_v);
      }
      parent_[root_v] = root_u;
      if (rank_[root_u] == rank_[root_v]) {
        rank_[root_u] += 1;
      }
    }
  }


 private:
  std::vector<size_t> parent_;
  std::vector<size_t> rank_;
};
