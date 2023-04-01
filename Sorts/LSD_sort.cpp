#include <iostream>
#include <vector>

void LSDSort(std::vector<uint64_t>& array) {
  std::vector<uint64_t> answer(array.size());
  const int kBytes = 8;
  const int kRangeNumbersInByte = 256;
  for (int i = 0; i < kBytes; ++i) {
    std::vector<int> cnt(kRangeNumbersInByte, 0);
    for (int j = 0; j < static_cast<int>(array.size()); ++j) {
      uint64_t element = (array[j] >> (kBytes * i));
      element = (element & (kRangeNumbersInByte - 1));
      cnt[element] += 1;
    }
    for (int j = 1; j < kRangeNumbersInByte; ++j) {
      cnt[j] += cnt[j - 1];
    }
    for (int j = static_cast<int>(array.size()) - 1; j >= 0; --j) {
      uint64_t element = (array[j] >> (kBytes * i));
      element = (element & (kRangeNumbersInByte - 1));
      answer[cnt[element] - 1] = array[j];
      cnt[element] -= 1;
    }
    array = answer;
  }
}

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);
  int numbers;
  std::cin >> numbers;
  std::vector<uint64_t> array(numbers);
  for (int i = 0; i < numbers; ++i) {
    std::cin >> array[i];
  }
  LSDSort(array);
  for (int i = 0; i < numbers; ++i) {
    std::cout << array[i] << '\n';
  }
}
