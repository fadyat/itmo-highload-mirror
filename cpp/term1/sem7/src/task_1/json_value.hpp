#pragma once

#include <cstddef>
#include <variant>
#include <vector>
#include <string>

namespace cls07 {
class JsonValue {
  public:
    using ListType = std::vector<JsonValue>;
    using StringType = std::string;

  private:
    std::variant<std::nullptr_t, bool, int, StringType, ListType> value_;

  public:
    JsonValue();
    JsonValue(bool value);
    JsonValue(int value);
    JsonValue(char value);
    JsonValue(const char *value);
    JsonValue(const StringType &value);

    JsonValue(const JsonValue &other) = default;
    JsonValue &operator=(const JsonValue &other) = default;

    bool IsNull() const;
    bool IsBool() const;
    bool IsInt() const;
    bool IsString() const;
    bool IsArray() const;

    bool &GetBool();
    bool GetBool() const;
    int &GetInt();
    int GetInt() const;
    StringType &GetString();
    StringType &GetString() const;
    ListType &GetArray();
    ListType &GetArray() const;

    void SetNull();
    void Append(const JsonValue &value);

    JsonValue &operator[](size_t index);
    const JsonValue &operator[](size_t index) const;
};

}
