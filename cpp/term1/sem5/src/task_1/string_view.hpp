#pragma once

#include <cstring>
#include <string>

class StringView {
  private:
    const char *data_;
    size_t size_;

  public:
    static constexpr size_t npos = -1;

    StringView(const char *str);
    StringView(const std::string &str);
    StringView(const char *str, size_t size);

    StringView &operator=(const char *str);
    StringView &operator=(const std::string &str);
    StringView &operator=(const StringView &str);

    const char *data() const;
    size_t size() const;

    bool operator==(const StringView &str) const;
    StringView substr(size_t start, size_t end = npos) const;

    const char &operator[](size_t idx) const;

    size_t find(const StringView &str) const;
    size_t find(const char str) const;

    bool startsWith(const StringView &str) const;
    bool startsWith(const char str) const;
    bool endsWith(const StringView &str) const;
    bool endsWith(const char str) const;

    size_t findFirstOf(const StringView &str, size_t start = 0) const;
    size_t findFirstOf(const char str, size_t start = 0) const;
    size_t findFirstNotOf(const StringView &str, size_t start = 0) const;
    size_t findFirstNotOf(const char str, size_t start = 0) const;

    void removePrefix(size_t count);
};
