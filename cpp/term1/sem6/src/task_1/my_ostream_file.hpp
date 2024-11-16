#pragma once

#include "my_ostream.hpp"
#include <fstream>

namespace cls_06 {
class my_ostream_file : public my_ostream {
  private:
    std::ofstream file_;

  public:
    explicit my_ostream_file(const std::string &filename);
    ~my_ostream_file() override;

    my_ostream &operator<<(int) override;
    my_ostream &operator<<(double) override;
    my_ostream &operator<<(const std::string &) override;
};
}
