#ifndef _TYPES_H_
#define _TYPES_H_

#ifdef USE_REAL_STD_VECTOR
#include <vector>
template <class T> using Vector = std::vector<T>;
#else
template <class T> using Vector = seq<T>;
#endif

typedef float sample_t;
//typedef double sample_t;

#endif // _TYPES_H_
