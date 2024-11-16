#include "my_ostream_combo.hpp"

cls_06::my_ostream_combo::my_ostream_combo(my_ostream *os1, my_ostream *os2) : os1_(os1), os2_(os2) {}

cls_06::my_ostream_combo::~my_ostream_combo() {
    delete os1_;
    delete os2_;
}

cls_06::my_ostream &cls_06::my_ostream_combo::operator<<(int i) {
    *os1_ << i;
    *os2_ << i;
    return *this;
}

cls_06::my_ostream &cls_06::my_ostream_combo::operator<<(double d) {
    *os1_ << d;
    *os2_ << d;
    return *this;
}

cls_06::my_ostream &cls_06::my_ostream_combo::operator<<(const std::string &s) {
    *os1_ << s;
    *os2_ << s;
    return *this;
}
