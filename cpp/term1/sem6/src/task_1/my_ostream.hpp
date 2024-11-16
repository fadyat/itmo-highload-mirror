#pragma once

#include <string>

namespace cls_06 {

class my_ostream {
  public:
    virtual my_ostream &operator<<(int) = 0;
    virtual my_ostream &operator<<(double) = 0;
    virtual my_ostream &operator<<(const std::string &) = 0;

    virtual ~my_ostream() = default;
};

}
