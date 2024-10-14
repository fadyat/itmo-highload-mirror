#pragma once

#include <cstddef>

using FuncT = double (*)(double);

double Compose(size_t n, ...);
