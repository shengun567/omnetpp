//==========================================================================
//  LAYOUTDEFS.H - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//  Author: Levente Meszaros
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_LAYOUTDEFS_H
#define __OMNETPP_LAYOUTDEFS_H

#include "omnetpp/platdep/platdefs.h"

NAMESPACE_BEGIN

#if defined(LAYOUT_EXPORT)
#  define LAYOUT_API OPP_DLLEXPORT
#elif defined(LAYOUT_IMPORT) || defined(OMNETPPLIBS_IMPORT)
#  define LAYOUT_API OPP_DLLIMPORT
#else
#  define LAYOUT_API
#endif

NAMESPACE_END


#endif


