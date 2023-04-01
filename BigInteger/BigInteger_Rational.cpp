#include <iostream>
#include <vector>
#include <string>

class BigInteger {
 public:

  BigInteger() {
    sign_ = true;
    digits_ = std::vector<int64_t>();
  }

  BigInteger(const int64_t& a) {
    long long b = a;
    sign_ = (b >= 0);
    if (!sign_) { b *= -1; }
    if (b == 0) { digits_.push_back(b); }
    while (b != 0) {
      digits_.push_back(b % base);
      b /= base;
    }
  }

  BigInteger(const BigInteger& other) {
    sign_ = other.sign_;
    digits_ = other.digits_;
  }

  static int64_t pow_ten(const size_t& degree) {
    int64_t answer = 1;
    for (size_t j = 0; j < degree; ++j) {
      answer *= 10;
    }
    return answer;
  }

  BigInteger(const std::string& s) {
    bool with_sign = false;
    if (s[0] == '-' or s[0] == '+') {
      with_sign = true;
    }
    if (s[0] == '-') {
      sign_ = false;
    } else {
      sign_ = true;
    }
    int64_t tmp = 0;
    int counter = 0;
    for (int64_t j = static_cast<int64_t>(s.size()) - 1; j >= (0 + static_cast<int64_t>(with_sign)); --j) {
      if (counter == 9) {
        counter = 0;
        digits_.push_back(tmp);
        tmp = 0;
      }
      tmp += (static_cast<int64_t>(s[j] - '0') * pow_ten(counter));
      ++counter;

    }
    digits_.push_back(tmp);
    this->clear();
  }

  ~BigInteger() = default;

  explicit operator bool() const;

  BigInteger& operator+() {
    return *this;
  }

  BigInteger operator-() const {
    BigInteger copy = *this;
    if (this->digits_.size() == 1 and this->digits_[0] == 0) {
      copy.sign_ = true;
    } else {
      copy.sign_ = !copy.sign_;
    }
    return copy;
  }

  BigInteger& operator+=(const BigInteger& other);

  static uint64_t max(const uint64_t a, const uint64_t b) {
    if (a > b) {
      return a;
    }
    return b;
  }

  std::string toString() {
    std::string str_int;
    if (!sign_) {
      str_int += '-';
    }
    for (int64_t j = static_cast<int64_t>(digits_.size()) - 1; j >= 0; --j) {
      if (j != static_cast<int64_t>(digits_.size()) - 1) {
        str_int += std::to_string(digits_[j] + base).substr(1);
      } else {
        str_int += std::to_string(digits_[j]);
      }
    }
    return str_int;
  }
  BigInteger& operator-=(const BigInteger& other);

  BigInteger& operator*=(const BigInteger& other);

  BigInteger& operator/=(const BigInteger& other);

  BigInteger& operator%=(const BigInteger& other);

  BigInteger& operator++() {
    *this += 1;
    return *this;
  }

  BigInteger& shiftLeft(const int64_t& degree) {
    std::vector<int64_t> tmp = this->digits_;
    this->digits_.clear();
    for (int64_t i = 0; i < degree; ++i) {
      this->digits_.push_back(0);
    }
    for (int64_t j = 0; j < static_cast<int64_t>(tmp.size()); ++j) {
      this->digits_.push_back(tmp[j]);
    }
    this->clear();
    return *this;
  }

  BigInteger operator++(int) {
    BigInteger copy = *this;
    ++*this;
    return copy;
  }

  BigInteger& operator--() {
    *this -= 1;
    return *this;
  }

  BigInteger operator--(int) {
    BigInteger copy = *this;
    --*this;
    return copy;
  }

 private:
  bool sign_; // Знак числа, false = -; true = положительное
  std::vector<int64_t> digits_;
  int64_t base = 1000000000;

  friend bool operator<(const BigInteger& a, const BigInteger& b);
  friend std::istream& operator>>(std::istream& in, BigInteger& a);
  friend std::ostream& operator<<(std::ostream& out, BigInteger a);

  static bool modCompare(const BigInteger& a, const BigInteger& b) {
    if (a.sign_ and b.sign_) {
      return a < b;
    } else if (a.sign_ and !b.sign_) {
      return a < -b;
    } else if (!a.sign_ and b.sign_) {
      return -a < b;
    } else {
      return b < a;
    }
  }

  void clear() {
    for (int64_t j = static_cast<int64_t>(digits_.size()) - 1; j > 0; --j) {
      if (digits_[j] == 0) {
        digits_.pop_back();
      } else {
        return;
      }
    }
    if (digits_[0] == 0) {
      this->sign_ = true;
    }
  }

};

BigInteger operator+(const BigInteger& a, const BigInteger& b) {
  BigInteger result = a;
  result += b;
  return result;
}

BigInteger operator-(const BigInteger& a, const BigInteger& b) {
  BigInteger result = a;
  result -= b;
  return result;
}

BigInteger operator*(const BigInteger& a, const BigInteger& b) {
  BigInteger result = a;
  result *= b;
  return result;
}

BigInteger operator/(const BigInteger& a, const BigInteger& b) {
  BigInteger result = a;
  result /= b;
  return result;
}
BigInteger operator%(const BigInteger& a, const BigInteger& b) {
  BigInteger result = a;
  result %= b;
  return result;
}
bool operator>=(const BigInteger& a, const BigInteger& b) {
  return !(a < b);
}

bool operator>(const BigInteger& a, const BigInteger& b) {
  return b < a;
}

bool operator<=(const BigInteger& a, const BigInteger& b) {
  return !(a > b);
}

bool operator==(const BigInteger& a, const BigInteger& b) {
  return !(a < b || a > b);
}

bool operator!=(const BigInteger& a, const BigInteger& b) {
  return !(a == b);
}

BigInteger& BigInteger::operator+=(const BigInteger& other) {
  if ((other.sign_ && this->sign_) or (!other.sign_ && !this->sign_)) {
    uint64_t right_degree = other.digits_.size();
    uint64_t left_degree = this->digits_.size();
    uint64_t max_degree = right_degree > left_degree ? right_degree : left_degree;
    for (uint64_t i = 0; i < max_degree - left_degree; ++i) {
      this->digits_.push_back(0);
    }
    int64_t tmp = 0;
    for (uint64_t i = 0; i < right_degree; ++i) {
      tmp += other.digits_[i];
      tmp += this->digits_[i];
      this->digits_[i] = (tmp % base);
      tmp -= (tmp % base);
      tmp /= base;
    }
    for (uint64_t i = right_degree; i < left_degree; ++i) {
      if (tmp == 0) { break; }
      tmp += this->digits_[i];
      this->digits_[i] = (tmp % base);
      tmp /= base;
    }
    if (tmp != 0) { this->digits_.push_back(tmp); }
    return *this;
  } else if (this->sign_ and !other.sign_) {
    return (*this -= (-other));
  } else {
    this->sign_ = true;
    *this -= other;
    *this = -(*this);
    return *this;
  }
}

BigInteger& BigInteger::operator-=(const BigInteger& other) {
  if (this->sign_ && other.sign_) {
    if (*this >= other) {
      bool below_zero = false;
      int64_t delta = 0;
      uint64_t max_degree = max(other.digits_.size(), this->digits_.size());
      if (this->digits_.size() < max_degree) {
        for (uint64_t i = 0; i < max_degree - this->digits_.size(); ++i) {
          this->digits_.push_back(0);
        }
      }
      for (uint64_t i = 0; i < other.digits_.size(); ++i) {
        delta = this->digits_[i] - other.digits_[i];
        if (delta < 0) {
          uint64_t degree_debt = i;
          if (!below_zero) {
            for (uint64_t j = i + 1; j < max_degree; ++j) {
              if (this->digits_[j] > 0) {
                this->digits_[j] = this->digits_[j] - 1;
                degree_debt = j;
                break;
              }
            }
          }
          if (degree_debt == i) { below_zero = true; }
          if (!below_zero) {
            for (int64_t j = static_cast<int64_t>(degree_debt) - 1; j > static_cast<int64_t>(i); --j) {
              this->digits_[j] = (base - 1);
            }
            delta += base;
          }
          if (below_zero) {
            delta *= -1;
            this->sign_ = false;
          }
        }
        digits_[i] = delta;
      }
      this->clear();
      return *this;
    } else {
      BigInteger tmp = other;
      tmp -= *this;
      *this = tmp;
      this->clear();
      this->sign_ = false;
      return *this;
    }
  } else if (!this->sign_ && !other.sign_) {
    return (*this += (-other));
  } else if (!this->sign_ && other.sign_) {
    this->sign_ = true;
    *this += other;
    this->sign_ = false;
    return *this;
  } else {
    return (*this += (-other));
  }
}

bool operator<(const BigInteger& a, const BigInteger& b) {
  if (!a.sign_ && b.sign_) {
    return true;
  } else if (a.sign_ && !b.sign_) {
    return false;
  } else if (a.sign_ && b.sign_) {
    if (a.digits_.size() < b.digits_.size()) {
      return true;
    } else if (a.digits_.size() > b.digits_.size()) {
      return false;
    } else {
      for (int64_t j = static_cast<int64_t>(a.digits_.size()) - 1; j >= 0; --j) {
        if (a.digits_[j] < b.digits_[j]) {
          return true;
        } else if (a.digits_[j] > b.digits_[j]) {
          return false;
        }
      }
      return false;
    }
  } else {
    if (a.digits_.size() < b.digits_.size()) {
      return false;
    } else if (a.digits_.size() > b.digits_.size()) {
      return true;
    } else {
      for (int64_t j = static_cast<int64_t>(a.digits_.size()) - 1; j >= 0; --j) {
        if (a.digits_[j] < b.digits_[j]) {
          return false;
        } else if (a.digits_[j] > b.digits_[j]) {
          return true;
        }
      }
      return false;
    }
  }
}

BigInteger::operator bool() const {
  return (*this != BigInteger(0));
}

BigInteger& BigInteger::operator*=(const BigInteger& other) {
  this->sign_ = !(this->sign_ xor other.sign_);
  if (other == 0) {
    *this = BigInteger(0);
    return *this;
  } else if (modCompare(other, BigInteger(base))) {
    int64_t from_previous = 0;
    for (int64_t j = 0; j < static_cast<int64_t>(this->digits_.size()); ++j) {
      int64_t tmp = this->digits_[j] * other.digits_[0];
      tmp += from_previous;
      this->digits_[j] = (tmp % base);
      from_previous = (tmp / base);
    }
    if (from_previous != 0) { this->digits_.push_back(from_previous); }
    this->clear();
    return *this;
  } else {
    BigInteger tmp = *this;
    *this *= other.digits_[0];

    for (int64_t j = 1; j < static_cast<int64_t>(other.digits_.size()); ++j) {
      BigInteger tmp1 = tmp;
      *this += (tmp1 *= other.digits_[j]).shiftLeft(j);
    }
    this->clear();
    return *this;
  }
}

BigInteger& BigInteger::operator/=(const BigInteger& other) {
  if (modCompare(*this, other)) {
    *this = BigInteger(0);
    return *this;
  } else if (other == *this) {
    *this = BigInteger(1);
    return *this;
  } else {
    bool sign = !(this->sign_ xor other.sign_);
    this->sign_ = true;
    int64_t left_degree = static_cast<int64_t>(this->digits_.size());
    int64_t right_degree = static_cast<int64_t>(other.digits_.size());
    int64_t delta_degree = left_degree - right_degree;
    std::vector<int64_t> result(delta_degree + 1, 0);
    for (int64_t i = delta_degree; i >= 0; --i) {
      int64_t left_border = 0;
      int64_t right_border = base;
      while ((right_border - left_border) > 1) {
        int64_t middle = (left_border + right_border) / 2;
        BigInteger tmp = other;
        tmp.sign_ = true;
        tmp *= middle;
        tmp.shiftLeft(i);
        if (tmp > *this) {
          right_border = middle;
        } else {
          left_border = middle;
        }
      }
      BigInteger delta = other;
      delta.sign_ = true;
      *this -= (delta *= left_border).shiftLeft(i);
      result[i] = left_border;
    }
    this->digits_ = result;
    this->sign_ = sign;
    this->clear();
    return *this;
  }
}

BigInteger& BigInteger::operator%=(const BigInteger& other) {
  *this = *this - (other * (*this / other));
  return *this;
}

std::istream& operator>>(std::istream& in, BigInteger& a) {
  std::string str_in;
  in >> str_in;
  a = BigInteger(str_in);
  return in;
}

std::ostream& operator<<(std::ostream& out, BigInteger a) {
  out << a.toString();
  return out;
}

BigInteger operator ""_bi(unsigned long long x) {
  BigInteger tmp = BigInteger(x);
  return tmp;
}
BigInteger operator ""_bi(const char* s) {
  BigInteger tmp = BigInteger(s);
  return tmp;
}

BigInteger gcd(const BigInteger& a, const BigInteger& b) {
  BigInteger tmp_a = a >= 0 ? a : -a;
  BigInteger tmp_b = b >= 0 ? b : -b;
  while (tmp_a != 0 and tmp_b != 0) {
    if (tmp_a > tmp_b) {
      tmp_a %= tmp_b;
    } else {
      tmp_b %= tmp_a;
    }
  }
  return (tmp_a + tmp_b);
}

class Rational {
 public:

  Rational(const int64_t& a) {
    numerator_ = BigInteger(a);
    denominator_ = BigInteger(1);
  }

  Rational() : numerator_(BigInteger(0)), denominator_(BigInteger(1)) {}

  Rational(const BigInteger& a) {
    numerator_ = a;
    denominator_ = BigInteger(1);
  }

  Rational(const BigInteger& a, const BigInteger& b) {
    if (b < 0) {
      numerator_ = -a;
      denominator_ = -b;
    } else {
      numerator_ = a;
      denominator_ = b;
    }
    this->decrease();
  }

  std::string toString() {
    this->decrease();
    if (denominator_ == 1) {
      return numerator_.toString();
    } else {
      std::string str;
      str += numerator_.toString();
      str += "/";
      str += denominator_.toString();
      return str;
    }
  }

  std::string asDecimal(size_t precision = 0) {
    bool sign;
    BigInteger tmp_numerator = numerator_;
    if (tmp_numerator >= 0) {
      sign = true;
    } else {
      tmp_numerator = -tmp_numerator;
      sign = false;
    }
    BigInteger whole_part = tmp_numerator / denominator_;
    BigInteger fractional_part = tmp_numerator % denominator_;
    std::string answer;
    if (!sign) {
      answer += '-';
    }
    BigInteger tmp(10);
    for (uint64_t i = 0; i < precision + 1; ++i) {
      fractional_part *= 10;
      tmp += fractional_part / denominator_;
      if (i != precision) { tmp *= 10; }
      fractional_part %= denominator_;
    }
    if ((tmp % 10) >= 5 and (tmp % 10) != 9) {
      tmp /= 10;
      tmp += 1;
    } else {
      tmp /= 10;
    }
    std::string second_part = tmp.toString();

    answer += whole_part.toString();
    if (precision > 0) {
      answer += '.';
      answer += second_part.substr(1);
    }
    return answer;
  }

  explicit operator double() {
    return (std::stod(asDecimal(10)));
  }

  Rational operator+() const {
    return *this;
  }

  Rational operator-() const;

  Rational& operator+=(const Rational& other) {
    BigInteger new_numerator = (this->numerator_ * other.denominator_) + (this->denominator_ * other.numerator_);
    BigInteger new_denominator = (this->denominator_ * other.denominator_);
    this->denominator_ = new_denominator;
    this->numerator_ = new_numerator;
    return *this;
  }

  Rational& operator-=(const Rational& other) {
    BigInteger new_numerator = (this->numerator_ * other.denominator_) - (this->denominator_ * other.numerator_);
    BigInteger new_denominator = (this->denominator_ * other.denominator_);
    this->denominator_ = new_denominator;
    this->numerator_ = new_numerator;
    return *this;
  }

  Rational& operator*=(const Rational& other) {
    this->numerator_ *= (other.numerator_);
    this->denominator_ *= (other.denominator_);
    return *this;
  }

  Rational& operator/=(const Rational& other) {
    if (other.numerator_ < 0) {
      this->numerator_ *= -(other.denominator_);
      this->denominator_ *= -(other.numerator_);
    } else {
      this->numerator_ *= (other.denominator_);
      this->denominator_ *= (other.numerator_);
    }
    return *this;
  }

  friend bool operator<(const Rational& a, const Rational& b);

 private:
  BigInteger numerator_;
  BigInteger denominator_;

  void decrease() {
    if (numerator_ == 0) {
      denominator_ = 1;
      return;
    } else if (denominator_ == 1) {
      return;
    }
    BigInteger tmp_numerator = (numerator_ >= 0 ? numerator_ : -numerator_);
    BigInteger decr = gcd(numerator_, denominator_);
    numerator_ /= decr;
    denominator_ /= decr;
  }
};

Rational operator+(const Rational& a, const Rational& b) {
  Rational answer = a;
  answer += b;
  return answer;
}

Rational operator-(const Rational& a, const Rational& b) {
  Rational answer = a;
  answer -= b;
  return answer;
}

Rational operator*(const Rational& a, const Rational& b) {
  Rational answer = a;
  answer *= b;
  return answer;
}

Rational operator/(const Rational& a, const Rational& b) {
  Rational answer = a;
  answer /= b;
  return answer;
}

Rational Rational::operator-() const {
  return (-1 * *this);
}

bool operator<(const Rational& a, const Rational& b) {
  return ((a.numerator_ * b.denominator_) < (b.numerator_ * a.denominator_));
}

bool operator>(const Rational& a, const Rational& b) {
  return b < a;
}

bool operator>=(const Rational& a, const Rational& b) {
  return !(a < b);
}

bool operator<=(const Rational& a, const Rational& b) {
  return !(a > b);
}

bool operator==(const Rational& a, const Rational& b) {
  return (!(a < b) and !(a > b));
}

bool operator!=(const Rational& a, const Rational& b) {
  return !(a == b);
}
