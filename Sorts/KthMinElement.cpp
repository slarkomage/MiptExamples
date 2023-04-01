#include <iostream>
#include <vector>

int Partition(std::vector<int>* array, int pivot_index, int left_border,
              int right_border) {
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
  return left_border;
}

int QuickSelect(std::vector<int>* array, int k_number, int left_border,
                int right_border) {
  if (right_border == left_border) {
    return (*array)[left_border];
  }
  int pivot = (rand() % (right_border - left_border)) + 1 + left_border;
  int pivot_position = Partition(array, pivot, left_border, right_border);
  if (pivot_position == k_number) {
    return (*array)[pivot_position];
  }
  if (k_number < pivot_position) {
    return (QuickSelect(array, k_number, left_border, pivot_position - 1));
  }
  return (QuickSelect(array, k_number, pivot_position + 1, right_border));
}

void Filling(std::vector<int>* numbers, int amount_numbers, int a0, int a1) {
  numbers->push_back(a0);
  numbers->push_back(a1);
  for (int i = 2; i < amount_numbers; i++) {
    int ai =
        ((*numbers)[i - 1] * 123 + (*numbers)[i - 2] * 45) % (10000000 + 4321);
    numbers->push_back(ai);
  }
}

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);
  std::vector<int> numbers;
  int amount_numbers;
  int position;
  int a00;
  int a01;
  std::cin >> amount_numbers;
  std::cin >> position;
  std::cin >> a00;
  std::cin >> a01;
  position--;
  Filling(&numbers, amount_numbers, a00, a01);
  std::cout << QuickSelect(&numbers, position, 0, amount_numbers - 1);
}
