#pragma once

#include <algorithm>
#include <bitset>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <istream>
#include <numeric>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

namespace mp {

class bignum {
  private:
    std::vector<uint32_t> numbers_;

  public:
    bignum() = default;
    bignum(uint32_t);
    explicit bignum(const std::string &);

    bignum(const bignum &) = default;
    bignum &operator=(const bignum &) = default;

    explicit operator uint32_t() const;
    explicit operator bool() const;
    std::string to_string() const;

    friend std::ostream &operator<<(std::ostream &, bignum &);
    friend std::istream &operator>>(std::istream &, bignum &);

    friend bignum operator+(const bignum &, const bignum &);
    bignum &operator+=(const bignum &);
    bignum operator-(const bignum &) = delete;

    friend bignum operator*(const bignum &, const bignum &);
    bignum &operator*=(const bignum &);

  private:
    void remove_leading_zeros();
    char extract_last_digit();
};

inline std::ostream &operator<<(std::ostream &os, const mp::bignum &n) {
    os << n.to_string();
    return os;
}

inline std::istream &operator>>(std::istream &is, mp::bignum &n) {
    std::string s;
    is >> s;
    n = mp::bignum(s);
    return is;
}

inline void bignum::remove_leading_zeros() {
    while (!numbers_.empty() && numbers_.back() == 0) {
        numbers_.pop_back();
    }
}

inline char bignum::extract_last_digit() {
    uint64_t carry = 0;
    uint64_t pow2_32 = static_cast<uint64_t>(1) << 32;
    for (size_t i = numbers_.size(); i > 0; --i) {
        uint64_t current = pow2_32 * carry + static_cast<uint64_t>(numbers_[i - 1]);
        numbers_[i - 1] = static_cast<uint32_t>(current / 10);
        carry = current % 10;
    }

    remove_leading_zeros();
    return static_cast<char>(carry + '0');
}

inline mp::bignum operator+(const mp::bignum &lhs, const mp::bignum &rhs) {
    std::size_t size = std::max(lhs.numbers_.size(), rhs.numbers_.size()) + 1;

    bignum result = bignum();
    result.numbers_.resize(size);

    uint64_t carry = 0;
    for (std::size_t i = 0; i < size; ++i) {
        uint32_t a = i < lhs.numbers_.size() ? lhs.numbers_[i] : 0;
        uint32_t b = i < rhs.numbers_.size() ? rhs.numbers_[i] : 0;
        uint64_t sum = carry + static_cast<uint64_t>(a) + static_cast<uint64_t>(b);

        result.numbers_[i] = static_cast<uint32_t>(sum);
        carry = sum >> 32;
    }

    result.remove_leading_zeros();
    return result;
}

inline mp::bignum operator*(const mp::bignum &lhs, const mp::bignum &rhs) {
    std::size_t size = lhs.numbers_.size() + rhs.numbers_.size();

    bignum result = bignum();
    result.numbers_.resize(size);

    for (std::size_t i = 0; i < lhs.numbers_.size(); ++i) {
        uint64_t carry = 0;
        for (std::size_t j = 0; j < rhs.numbers_.size(); ++j) {
            uint64_t product = carry + static_cast<uint64_t>(lhs.numbers_[i]) * static_cast<uint64_t>(rhs.numbers_[j]) +
                               static_cast<uint64_t>(result.numbers_[i + j]);
            result.numbers_[i + j] = static_cast<uint32_t>(product);
            carry = product >> 32;
        }

        result.numbers_[i + rhs.numbers_.size()] += static_cast<uint32_t>(carry);
    }

    result.remove_leading_zeros();
    return result;
}

} // namespace mp

inline mp::bignum::bignum(uint32_t n) {
    if (n != 0) {
        numbers_.push_back(n);
    }
}

inline mp::bignum::bignum(const std::string &s) {
    std::size_t size = s.size();
    bignum power = bignum(1);

    for (size_t i = 0; i < size; ++i) {
        uint32_t digit = static_cast<uint32_t>(s[size - i - 1] - '0');
        *this += power * digit;
        power *= 10;
    }
}

inline mp::bignum::operator uint32_t() const { return numbers_.empty() ? 0 : numbers_.front(); }

inline mp::bignum::operator bool() const { return !numbers_.empty(); }

inline std::string mp::bignum::to_string() const {
    if (!*this) {
        return "0";
    }

    mp::bignum copy = *this;
    std::string result;

    while (copy) {
        result.push_back(copy.extract_last_digit());
    }

    std::reverse(result.begin(), result.end());
    return result;
}

inline mp::bignum &mp::bignum::operator+=(const mp::bignum &other) {
    *this = *this + other;
    return *this;
}

inline mp::bignum &mp::bignum::operator*=(const mp::bignum &rhs) {
    *this = *this * rhs;
    return *this;
}

namespace mp {

class polynomial {
  private:
    std::vector<uint32_t> coef_;

  public:
    explicit polynomial(const std::string &);

    uint32_t at(std::size_t) const;
    uint32_t &at(std::size_t);

    template <typename T> T operator()(const T &x) const;

  private:
    std::pair<int, int> parse_part(const std::string &);
};

inline std::pair<int, int> polynomial::parse_part(const std::string &part) {
    size_t x_pos = part.find('*'), power_pos = part.find("^");

    int coef = x_pos != std::string::npos ? std::stoi(part.substr(0, x_pos)) : 1,
        power = power_pos != std::string::npos ? std::stoi(part.substr(power_pos + 1)) : 0;

    return {coef, power};
}

inline polynomial::polynomial(const std::string &s) {
    std::istringstream stream(s);
    std::string part;

    while (std::getline(stream, part, '+')) {
        auto [coef, power] = parse_part(part);
        if (power + 1 > static_cast<int>(coef_.size())) {
            coef_.resize(power + 2, 0);
        }

        coef_[power] += coef;
    }
}

inline uint32_t polynomial::at(std::size_t x) const { return coef_.at(x); }

inline uint32_t &polynomial::at(std::size_t x) { return coef_.at(x); }

template <typename T> inline T polynomial::operator()(const T &x) const {
    return std::accumulate(coef_.rbegin(), coef_.rend(), T(0), [&](T res, const T &coef) { return res * x + coef; });
}

} // namespace mp
