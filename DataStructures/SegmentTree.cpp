#include <algorithm>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <set>


class SegmentTree {
 public:
  ~SegmentTree() = default;

  SegmentTree() = default;

  SegmentTree(int sz) {
    while ((sz & (sz - 1)) != 0) {
      sz += 1;
    }
    size_ = sz;
    tree_ = std::vector<int>(2 * sz - 1, 0);
  }

  int Max(int l, int r) { return Max(0, l, r, 0, size_ - 1); }

  void Update(int pos, int new_value) {
    Update(0, pos, new_value, 0, size_ - 1);
  }

 private:
  std::vector<int> tree_;
  int size_;
  const int kMaxNeutral = -100000000;

  int Max(int v, int l, int r, int tl, int tr) {
    if (tl == l and tr == r) {
      return tree_[v];
    }
    int tm = (tl + tr) >> 1;
    int ans = kMaxNeutral;
    if (l <= tm) {
      ans = std::max(Max(2 * v + 1, l, std::min(r, tm), tl, tm), kMaxNeutral);
    }
    if (r >= tm + 1) {
      ans = std::max(Max(2 * v + 2, std::max(l, tm + 1), r, tm + 1, tr), ans);
    }
    return ans;
  }

  void Update(int v, int pos, int new_element, int tl, int tr) {
    if (tl == tr) {
      tree_[v] = new_element;
      return;
    }
    int tm = (tl + tr) >> 1;
    if (pos <= tm) {
      Update(2 * v + 1, pos, new_element, tl, tm);
    } else {
      Update(2 * v + 2, pos, new_element, tm + 1, tr);
    }
    tree_[v] = std::max(new_element, tree_[v]);
  }
};

class SegmentTreeMin {
 public:
  ~SegmentTreeMin() = default;

  SegmentTreeMin() = default;

  SegmentTreeMin(int sz) {
    while ((sz & (sz - 1)) != 0) {
      sz += 1;
    }
    size_ = sz;
    tree_ = std::vector<std::pair<int, int>>(2 * sz - 1, {kMinNeutral, 1});
  }

  int Min(int l, int r) { return Min(0, l, r, 0, size_ - 1); }

  int AmountMin(int l, int r) { return AmountMin(0, l, r, 0, size_ - 1); }

  void Update(int pos, int new_value) {
    Update(0, pos, new_value, 0, size_ - 1);
  }

 private:
  std::vector<std::pair<int, int>> tree_;
  int size_;
  const int kMinNeutral = 1000000000;

  int Min(int v, int l, int r, int tl, int tr) {
    if (tl == l and tr == r) {
      return tree_[v].first;
    }
    int tm = (tl + tr) >> 1;
    int ans = kMinNeutral;
    if (l <= tm) {
      ans = std::min(Min(2 * v + 1, l, std::min(r, tm), tl, tm), kMinNeutral);
    }
    if (r >= tm + 1) {
      ans = std::min(Min(2 * v + 2, std::max(l, tm + 1), r, tm + 1, tr), ans);
    }
    return ans;
  }

  int AmountMin(int v, int l, int r, int tl, int tr) {
    return tree_[0].second;
  }

  std::pair<int, int> Combine (std::pair<int, int> pr1, std::pair<int, int> pr2) {
    if (pr1.first < pr2.first) {
      return pr1;
    }
    if (pr1.first > pr2.first) {
      return pr2;
    }
    return {pr1.first, pr1.second + pr2.second};
  }

  void Update(int v, int pos, int new_element, int tl, int tr) {
    if (tl == tr) {
      tree_[v] = {new_element, 1};
      return;
    }
    int tm = (tl + tr) >> 1;
    if (pos <= tm) {
      Update(2 * v + 1, pos, new_element, tl, tm);
    } else {
      Update(2 * v + 2, pos, new_element, tm + 1, tr);
    }
    tree_[v] = Combine(tree_[2 * v + 1], tree_[2 * v + 2]);
  }
};
