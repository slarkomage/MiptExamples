#include <algorithm>
#include <iostream>
#include <vector>
#include <set>
#include <unordered_map>

class SparseTable {
 public:
  SparseTable() = default;

  SparseTable(const std::vector<size_t>& data) {
    size_ = data.size();
    deg_ = std::vector<size_t>(size_ + 1, 0);
    for (size_t j = 2; j <= size_; ++j) {
      deg_[j] = deg_[j - 1];
      if ((j & (j - 1)) == 0) {
        deg_[j] += 1;
      }
    }

    table_ =
        std::vector<std::vector<std::pair<size_t, size_t>>>(deg_[size_] + 1,
                                                            std::vector<std::pair<
                                                                size_t,
                                                                size_t>>(size_));

    for (size_t i = 0; i < size_; ++i) {
      table_[0][i] = {data[i], i};
    }

    for (size_t k = 1; k <= deg_[size_]; ++k) {
      for (size_t i = 0; i < size_; ++i) {
        size_t last_index = i + (1 << (k - 1));
        if (last_index < size_) {
          table_[k][i] = std::min(table_[k - 1][i], table_[k - 1][last_index]);
        } else {
          table_[k][i] = table_[k - 1][i];
        }
      }
    }
  }

  ~SparseTable() = default;

  size_t GetMin(const size_t& left, const size_t& right) const {
    size_t degree = deg_[right - left + 1];
    return std::min(table_[degree][left],
                    table_[degree][right + 1 - (1 << degree)]).second;
  }

 private:
  std::vector<size_t> deg_;
  std::vector<std::vector<std::pair<size_t, size_t>>> table_{};
  size_t size_ = 0;
};
