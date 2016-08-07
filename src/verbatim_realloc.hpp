#ifndef VERBATIM_REALLOC_HPP
#define VERBATIM_REALLOC_HPP

// Must be included prior to redefining memory management.
#include <algorithm>
#include <cfloat>
#include <cstdio>
#if defined(USE_LD)
#include "glsl_shader_source.hpp"
#include <iostream>
#include <boost/throw_exception.hpp>
#endif

#if __cplusplus >= 201103L
/// Use C++11 exception specification if possible.
#define NOEXCEPT noexcept
#else
/// Otherwise use old style.
#define NOEXCEPT throw()
#endif

/// A global new operator using realloc.
///
/// \param sz Size to allocate.
/// \return Allocated pointer.
void* operator new(size_t sz)
{
  return dnload_realloc(NULL, sz);
}

/// A global delete operator using realloc.
///
/// \param ptr Pointer to free.
void operator delete(void *ptr) NOEXCEPT
{
  void* ret = dnload_realloc(ptr, 0);
  (void)ret;
}

/// Array new.
///
/// Replacement for new[].
///
/// \param ptr Existing ptr (may be NULL).
/// \param count Number of elements to allocate.
/// \return Reallocated ptr.
template <typename T> inline T* array_new(T* ptr, size_t count)
{
  return static_cast<T*>(dnload_realloc(ptr, sizeof(T) * count));
}

/// Array delete.
///
/// Replacement for delete[].
///
/// \param ptr Pointer to free.
inline void array_delete(void *ptr)
{
  void* ret = dnload_realloc(ptr, 0);
  (void)ret;
}

#endif
