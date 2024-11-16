#pragma once

#include "my_ostream.hpp"

namespace cls_06 {
class my_ostream_combo : public my_ostream {
  private:
    my_ostream *os1_, *os2_;

  public:
    explicit my_ostream_combo(my_ostream *os1, my_ostream *os2);
    ~my_ostream_combo() override;

    my_ostream_combo(const my_ostream_combo &) = delete;
    my_ostream_combo &operator=(const my_ostream_combo &) = delete;

    my_ostream &operator<<(int) override;
    my_ostream &operator<<(double) override;
    my_ostream &operator<<(const std::string &) override;
};
}
