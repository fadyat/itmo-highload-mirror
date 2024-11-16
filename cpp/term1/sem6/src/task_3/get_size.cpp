#include "get_size.hpp"

#include <algorithm>
#include <regex>
#include <string>
#include <vector>

namespace {

std::vector<Field> parse_fields(const std::string &sdef) {
    std::vector<Field> fields;
    std::regex field_rgx(R"((?:alignas\((\d+)\)\s*)?(\w+)\s+(\w+)\s*;)");
    std::smatch match;

    auto begin = std::sregex_iterator(sdef.begin(), sdef.end(), field_rgx);
    auto end = std::sregex_iterator();

    for (std::sregex_iterator i = begin; i != end; ++i) {
        std::smatch match = *i;
        Field field = {.type = match[2].str(),
                       .name = match[3].str(),
                       .alignment = match[1].matched ? static_cast<size_t>(std::stoi(match[1].str())) : 1};

        fields.push_back(field);
    }

    return fields;
}

int get_type_size(const std::string &type) {
    if (type == "int") {
        return 4;
    } else if (type == "char") {
        return 1;
    } else if (type == "short") {
        return 2;
    } else if (type == "long") {
        return 8;
    } else if (type == "float") {
        return 4;
    } else if (type == "double") {
        return 8;
    } else {
        return 0;
    };
}

}

size_t cls_06::GetSize(const std::string &sdef) {
    size_t position = 0;
    size_t max_type_size = 1;

    for (const auto &field : parse_fields(sdef)) {
        size_t type_size = get_type_size(field.type);
        size_t alignment = std::max(type_size, field.alignment);

        max_type_size = std::max(max_type_size, alignment);

        position = (position + alignment - 1) / alignment * alignment;
        position += type_size;
    }

    if (position % max_type_size != 0) {
        position = (position + max_type_size - 1) / max_type_size * max_type_size;
    }

    return std::max(position, size_t(1));
}
