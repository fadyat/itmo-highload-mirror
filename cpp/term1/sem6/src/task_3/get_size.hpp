#pragma once

#include <string>

struct Field {
    std::string type;
    std::string name;

    size_t alignment = 0;
};

namespace cls_06 {
size_t GetSize(const std::string &sdef);
}
