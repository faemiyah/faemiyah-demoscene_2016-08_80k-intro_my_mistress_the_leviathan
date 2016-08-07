#ifndef _UTILS_H_
#define _UTILS_H_

#include "types.hpp"
#include <algorithm>

template <typename T> void unused(T &&) {}

int clamp_i(const int &v, const int &lo, const int &hi) {
    return std::max(std::min(v, hi), lo);
}

float clamp_f(const float &value,const float &lower, const float &higher)
{
  return std::max(std::min(value, higher), lower);
}

#endif // _UTILS_H_
