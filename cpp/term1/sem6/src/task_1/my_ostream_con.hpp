#pragma once

#include "my_ostream.hpp"

namespace cls_06 {
class my_ostream_con : public my_ostream {
  public:
    my_ostream_con() = default;
    ~my_ostream_con() override = default;

    my_ostream_con &operator<<(int a) override;
    my_ostream_con &operator<<(double a) override;
    my_ostream_con &operator<<(const std::string &a) override;
};
}
