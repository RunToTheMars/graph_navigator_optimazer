#ifndef UTILS_H
#define UTILS_H

#include "math.h"

namespace cmp_constants
{
  constexpr double min_compare = 1.e-12;
}

inline bool fuzzy_eq (double a, double b, double eps = cmp_constants::min_compare)
{
    return 3 * fabs (a - b) <= eps * (fabs (a) + fabs (b) + 1);
}

template <typename T>
int isize (const T &vec)
{
  return static_cast<int> (vec.size ());
}

#endif // UTILS_H
