#include "my_ostream_con.hpp"
#include <iostream>

cls_06::my_ostream_con &cls_06::my_ostream_con::operator<<(int a) {
    std::cout << a;
    return *this;
}

cls_06::my_ostream_con &cls_06::my_ostream_con::operator<<(double a) {
    std::cout << a;
    return *this;
}

cls_06::my_ostream_con &cls_06::my_ostream_con::operator<<(const std::string &a) {
    std::cout << a;
    return *this;
}
