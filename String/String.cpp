#include <iostream>
#include <cstring>

class String {
 private:
  size_t sz = 0;
  char* arr = nullptr;
  size_t cap = 0;

 public:
  //С-style конструктор
  String(const char* cstr) : sz(strlen(cstr)), arr(new char[sz + 1]), cap(sz) {
    memcpy(arr, cstr, sz + 1);
  }

  //Конструктор для n элементов 'c':
  String(size_t n, char c) : sz(n), arr(new char[n + 1]), cap(n) {
    memset(arr, c, n);
    arr[n] = '\0';
  }

  //Конструктор по умолчанию:
  String() : sz(0), arr(new char[1]), cap(0) {
    arr[0] = '\0';
  }

  //Конструктор копирования:
  String(const String& str) : sz(str.sz), arr(new char[str.cap + 1]), cap(str.cap) {
    memcpy(arr, str.arr, sz + 1);
  }
  //Конструктор из чара (Можно и через конструктор для n элементов сделать):
  String(const char& c) : sz(1), arr(new char[2]), cap(1) {
    arr[0] = c;
    arr[1] = '\0';
  }
  //Присваивание
  void swap(String& str) {
    std::swap(arr, str.arr);
    std::swap(sz, str.sz);
    std::swap(cap, str.cap);
  }

  String& operator=(String str) {
    swap(str);
    return *this;
  }

  //Деструктор:
  ~String() {
    delete[] arr;
  }

  bool empty() const {
    return (sz == 0);
  }

  void clear() {
    sz = 0;
    arr[0] = '\0';
  }

  size_t size() const {
    return sz;
  }

  size_t capacity() const {
    return cap;
  }

  size_t length() const {
    return sz;
  }

  char& operator[](size_t index) {
    return arr[index];
  }

  const char& operator[](size_t index) const {
    return arr[index];
  }

  void shrink_to_fit() {
    char* new_arr = new char[sz + 1];
    memcpy(new_arr, arr, sz + 1);
    delete[] arr;
    cap = sz;
    arr = new_arr;
  }

  void pop_back() {
    sz -= 1;
    arr[sz] = '\0';
  }

  void push_back(char c) {
    if (cap == 0) {
      cap += 1;
      sz += 1;
      delete[] arr;
      arr = new char[2];
      arr[0] = c;
      arr[1] = '\0';
    } else if (cap == sz) {
      cap *= 2;
      char* temp_arr = new char[cap + 1];
      memcpy(temp_arr, arr, sz + 1);
      delete[] arr;
      arr = temp_arr;
      arr[sz] = c;
      arr[sz + 1] = '\0';
      ++sz;
    } else {
      arr[sz] = c;
      arr[sz + 1] = '\0';
      ++sz;
    }
  }

  char& front() {
    return arr[0];
  }

  const char& front() const {
    return arr[0];
  }

  char& back() {
    return arr[sz - 1];
  }

  const char& back() const {
    return arr[sz - 1];
  }

  String& operator+=(char c) {
    this->push_back(c);
    return *this;
  }

  static size_t lower_bound(const size_t& x, const size_t& y) {
    size_t border = y;
    if (border == 0) { border += 1; }
    while (true) {
      if (border >= x) {
        return border;
      }
      border <<= 1;
    }
  }

  String& operator+=(const String& str) {
    size_t temp_cap = lower_bound(sz + str.sz, cap);
    char* temp_arr = new char[temp_cap + 1];
    memcpy(temp_arr, arr, sz);
    memcpy(temp_arr + sz, str.arr, str.sz + 1);
    sz += str.sz;
    cap = temp_cap;
    delete[] arr;
    arr = temp_arr;
    return *this;
  }

  size_t find(const String& sub) const {
    if (sub.sz > sz) {
      return sz;
    }
    for (size_t i = 0; i < (sz - sub.sz + 1); ++i) {
      bool correct = true;
      for (size_t j = 0; j < sub.sz; ++j) {
        if (arr[i + j] != sub.arr[j]) {
          correct = false;
          break;
        }
      }
      if (correct) {
        return i;
      }
    }
    return sz;
  }

  size_t rfind(const String& sub) const {
    if (sub.sz > sz) {
      return sz;
    }
    size_t index = sz;
    for (size_t i = 0; i < (sz - sub.sz + 1); ++i) {
      bool correct = true;
      for (size_t j = 0; j < sub.sz; ++j) {
        if (arr[i + j] != sub.arr[j]) {
          correct = false;
          break;
        }
      }
      if (correct) {
        index = i;
      }
    }
    return index;
  }
  String substr(size_t start, size_t count) const {
    String tmp(count, '0');
    memcpy(tmp.arr, arr + start, count + 1);
    return tmp;
  }

  char* data() {
    return arr;
  }

  const char* data() const {
    return arr;
  }

};

bool operator==(const String& str1, const String& str2) {
  if (str1.size() != str2.size()) {
    return false;
  } else {
    for (size_t i = 0; i < str1.size(); ++i) {
      if (str1[i] != str2[i]) {
        return false;
      }
    }
    return true;
  }
}

bool operator<(const String& str1, const String& str2) {
  for (size_t i = 0; i < std::min(str1.size(), str2.size()); ++i) {
    if (str1[i] < str2[i]) {
      return true;
    } else if (str1[i] > str2[i]) {
      return false;
    }
  }
  return str1.size() < str2.size();
}

bool operator>(const String& str1, const String& str2) {
  return str2 < str1;
}

bool operator>=(const String& str1, const String& str2) {
  return !(str1 < str2);
}

bool operator<=(const String& str1, const String& str2) {
  return !(str1 > str2);
}

bool operator!=(const String& str1, const String& str2) {
  return !(str1 == str2);
}

String operator+(const String& str1, const String& str2) {
  String result = str1;
  result += str2;
  return result;
}

String operator+(const String& str1, char c) {
  String result = str1;
  result += c;
  return result;
}

String operator+(const char c, const String& str1) {
  String result(c);
  result += str1;
  return result;
}

std::ostream& operator<<(std::ostream& out, const String& str) {
  for (size_t i = 0; i < str.size(); ++i) {
    out << str[i];
  }
  return out;
}

std::istream& operator>>(std::istream& in, String& str) {
  while (true) {
    if (in.peek() == EOF) {
      return in;
    }
    if (isspace(in.peek())) {
      in.ignore();
      while (isspace(in.peek())) {
        in.ignore();
      }
      return in;
    }
    char c;
    in >> c;
    str.push_back(c);
  }
}
