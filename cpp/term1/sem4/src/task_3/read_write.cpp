#include <cstring>
#include "read_write.hpp"

const char *read(const char *data, int &out) {
    memcpy(&out, data, sizeof(int));
    return data + sizeof(int);
}

const char *read(const char *data, size_t &out) {
    memcpy(&out, data, sizeof(size_t));
    return data + sizeof(size_t);
}

const char *read(const char *data, char *&out) {
    out = new char[strlen(data) + 1];
    memcpy(out, data, strlen(data) + 1);
    return data + strlen(data) + 1;
}

char *write(char *data, int in) {
    memcpy(data, &in, sizeof(int));
    return data + sizeof(int);
}

char *write(char *data, size_t in) {
    memcpy(data, &in, sizeof(size_t));
    return data + sizeof(size_t);
}

char *write(char *data, const char *in) {
    memcpy(data, in, strlen(in) + 1);
    return data + strlen(in) + 1;
}