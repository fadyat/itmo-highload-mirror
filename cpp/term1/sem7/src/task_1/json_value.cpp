#include "json_value.hpp"
#include <cstddef>
#include <stdexcept>
#include <variant>

namespace cls07 {

JsonValue::JsonValue() : value_(nullptr) {}
JsonValue::JsonValue(bool value) : value_(value) {}
JsonValue::JsonValue(int value) : value_(value) {}
JsonValue::JsonValue(char value) : value_(StringType(1, value)) {}
JsonValue::JsonValue(const char *value) : value_(StringType(value)) {}
JsonValue::JsonValue(const JsonValue::StringType &value) : value_(value) {}

bool JsonValue::IsNull() const { return std::holds_alternative<std::nullptr_t>(value_); }
bool JsonValue::IsBool() const { return std::holds_alternative<bool>(value_); }
bool JsonValue::IsInt() const { return std::holds_alternative<int>(value_); }
bool JsonValue::IsString() const { return std::holds_alternative<StringType>(value_); }
bool JsonValue::IsArray() const { return std::holds_alternative<ListType>(value_); }

bool &JsonValue::GetBool() {
    if (!IsBool()) {
        value_ = false;
    }

    return std::get<bool>(value_);
}

bool JsonValue::GetBool() const {
    if (!IsBool()) {
        throw std::bad_variant_access();
    }

    return std::get<bool>(value_);
}

int &JsonValue::GetInt() {
    if (!IsInt()) {
        value_ = 0;
    }

    return std::get<int>(value_);
}

int JsonValue::GetInt() const {
    if (!IsInt()) {
        throw std::bad_variant_access();
    }

    return std::get<int>(value_);
}

JsonValue::StringType &JsonValue::GetString() {
    if (!IsString()) {
        value_ = StringType();
    }

    return std::get<JsonValue::StringType>(value_);
}

JsonValue::StringType &JsonValue::GetString() const {
    if (!IsString()) {
        throw std::bad_variant_access();
    }

    return const_cast<JsonValue *>(this)->GetString();
}

JsonValue::ListType &JsonValue::GetArray() {
    if (!IsArray()) {
        value_ = ListType();
    }

    return std::get<JsonValue::ListType>(value_);
}

JsonValue::ListType &JsonValue::GetArray() const {
    if (!IsArray()) {
        throw std::bad_variant_access();
    }

    return const_cast<JsonValue *>(this)->GetArray();
}

void JsonValue::SetNull() { value_ = nullptr; }

void JsonValue::Append(const JsonValue &value) {
    if (!IsArray()) {
        value_ = ListType();
    }

    GetArray().push_back(value);
}

JsonValue &JsonValue::operator[](size_t index) {
    if (!IsArray()) {
        value_ = ListType();
    }

    return GetArray().at(index);
}

const JsonValue &JsonValue::operator[](size_t index) const {
    if (!IsArray()) {
        throw std::out_of_range("Index out of range");
    }

    return GetArray().at(index);
}

}
