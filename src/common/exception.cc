//=========================================================================
//  EXCEPTION.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>
#include <stdarg.h>
#include "exception.h"
#include "commonutil.h"

NAMESPACE_BEGIN


opp_runtime_error::opp_runtime_error(const char *messagefmt, ...) : std::runtime_error("")
{
    char buf[1024];
    VSNPRINTF(buf, 1024, messagefmt);
    errormsg = buf;
}

NAMESPACE_END

