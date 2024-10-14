#pragma once

#include <cstddef>

char const *read(const char *data, int &out);

char const *read(const char *data, size_t &out);

char const *read(const char *data, char *&out);

char *write(char *data, int in);

char *write(char *data, size_t in);

char *write(char *data, const char *in);


