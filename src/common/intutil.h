//==========================================================================
//   INTUTIL.H  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_COMMON_INTUTIL_H
#define __OMNETPP_COMMON_INTUTIL_H

#include <cinttypes>
#include <limits>
#include <type_traits>
#include <string>
#include "commondefs.h"
#include "exception.h"

namespace omnetpp {
namespace common {

typedef int64_t intval_t;

// helper for checked_int_cast
void intCastError(const std::string& num, const char *errmsg=nullptr);

/**
 * @brief Safe integer cast: it throws an exception if in case of an overflow,
 * i.e. when if the target type cannot represent the value in the source type.
 * The context argument will be used for the error message.
 */
template<typename ToInt, typename FromInt>
ToInt checked_int_cast(FromInt x, const char *errmsg=nullptr)
{
    static_assert(std::is_integral<ToInt>::value && std::is_integral<FromInt>::value, "checked_int_cast expects integers");
    ToInt res = x;
    if ((x<0) != (res<0) || x-res != 0)  // note: x!=res would result in warning: signed-unsigned comparison
        intCastError(std::to_string(x), errmsg);
    return res;
}

/**
 * @brief Safe integer cast: it throws an exception if in case of an overflow,
 * i.e. when if the target type cannot represent the value in the source type.
 * The context argument will be used for the error message.
 */
template<typename ToInt>
ToInt checked_int_cast(double d, const char *errmsg=nullptr)
{
    static_assert(std::is_integral<ToInt>::value, "checked_int_cast expects integer template argument");
    if (d < std::numeric_limits<ToInt>::min() || d > std::numeric_limits<ToInt>::max())
        intCastError(std::to_string(d), errmsg);
    return (ToInt)d;
}

inline double safeCastToDouble(intval_t x)
{
    double d = (double)x;
    intval_t x2 = (intval_t)d;
    if (x != x2)
        throw opp_runtime_error("Integer %" PRId64 " too large, conversion to double would incur precision loss (use explicit cast to double to suppress this error)", (int64_t)x);
    return d;
}

// safe integer operations (throw exception on overflow):

intval_t safeAdd(intval_t a, intval_t b);
intval_t safeSub(intval_t a, intval_t b);
intval_t safeMul(intval_t a, intval_t b);
intval_t intPow(intval_t base, intval_t exp);
intval_t shift(intval_t a, intval_t b);

}  // namespace common
}  // namespace omnetpp

#endif


