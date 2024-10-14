#include <iostream>
#include <vector>
#include <stdexcept>

#include "matrices.hpp"


class arg {
public:
    std::string operation;
    std::string path;

    arg(const std::string &operation, const std::string &path) :
            operation(operation),
            path(path) {}
};

class args {
public:
    std::string initial_matrix_path;
    std::vector<arg> v;

    args(int argc, char **argv);

    ~args();
};

args::args(int argc, char **argv) {
    if (argc < 2) {
        throw std::runtime_error("not enough args");
    }

    this->initial_matrix_path = argv[1];
    for (int i = 2; i < argc; i += 2) {
        if (i + 1 >= argc) {
            throw std::runtime_error("not enough args");
        }

        arg a(argv[i], argv[i + 1]);
        if (a.operation != "--add" && a.operation != "--mult") {
            throw std::runtime_error("invalid operation: " + a.operation);
        }

        this->v.push_back(a);
    }
}

args::~args() {
    this->v.clear();
}

int main(int argc, char **argv) {
    try {
        auto parsed_args = args(argc, argv);
        auto a = matrix(parsed_args.initial_matrix_path);

        for (const arg &arg: parsed_args.v) {
            auto b = matrix(arg.path);

            if (arg.operation == "--add") {
                a = add(a, b);
            } else if (arg.operation == "--mult") {
                a = mult(a, b);
            } else {
                throw std::runtime_error("invalid operation: " + std::string(arg.operation));
            }
        }

        std::cout << a;
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
