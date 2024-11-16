
#include "my_ostream_file.hpp"

cls_06::my_ostream_file::my_ostream_file(const std::string &filename) { file_.open(filename); }

cls_06::my_ostream_file::~my_ostream_file() { file_.close(); }

cls_06::my_ostream &cls_06::my_ostream_file::operator<<(int value) {
    file_ << value;
    return *this;
}

cls_06::my_ostream &cls_06::my_ostream_file::operator<<(double value) {
    file_ << value;
    return *this;
}

cls_06::my_ostream &cls_06::my_ostream_file::operator<<(const std::string &value) {
    file_ << value;
    return *this;
}
