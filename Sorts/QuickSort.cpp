#include <iostream>
#include <vector>

std::pair<int, int> Partition(std::vector<int>* array, int pivot_index,
                              int left_border, int right_border) {
  int step = left_border;
  int based_element = (*array)[pivot_index];
  while (step <= right_border) {
    if ((*array)[step] > based_element) {
      std::swap((*array)[step], (*array)[right_border]);
      right_border--;
    } else if ((*array)[step] == based_element) {
      step++;
    } else {
      std::swap((*array)[step], (*array)[left_border]);
      left_border++;
      step++;
    }
  }
  std::pair<int, int> answer;
  answer = {right_border, left_border};
  return answer;
}

void Filling(std::vector<int>* numbers, int amount_numbers) {
  for (int i = 0; i < amount_numbers; i++) {
    int number;
    std::cin >> number;
    numbers->push_back(number);
  }
}

void QuickSort(std::vector<int>* array, int left_border, int right_border) {
  int pivot = (rand() % (right_border - left_border + 1)) + left_border;
  std::pair<int, int> based_index =
      Partition(array, pivot, left_border, right_border);
  int index_r = based_index.first;
  int index_l = based_index.second;
  if (left_border != index_l) {
    QuickSort(array, left_border, index_l - 1);
  }
  if (right_border != index_r) {
    QuickSort(array, index_r + 1, right_border);
  }
}

void OutputSolution(std::vector<int>& numbers) {
  for (int v : numbers) {
    std::cout << v << " ";
  }
}

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);
  std::vector<int> numbers;
  int amount_numbers;
  std::cin >> amount_numbers;
  Filling(&numbers, amount_numbers);
  QuickSort(&numbers, 0, amount_numbers - 1);
  OutputSolution(numbers);
}
