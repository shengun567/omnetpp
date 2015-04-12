//=========================================================================
//  ARRAYBUILDER.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_ARRAYBUILDER_H
#define __OMNETPP_ARRAYBUILDER_H

#include "common/commonutil.h"
#include "commonnodes.h"
#include "xyarray.h"

NAMESPACE_BEGIN

class ArrayBuilderNodeType;

/**
 * Stores all data in vector (two 'double' vectors in fact).
 */
class SCAVE_API ArrayBuilderNode : public SingleSinkNode
{
    friend class ArrayBuilderNodeType;
    private:
        double *xvec;
        double *yvec;
        size_t veccapacity;
        size_t veclen;
        BigDecimal *xpvec;
        bool collectEvec;
        eventnumber_t *evec; // event numbers
        void resize();
    public:
        ArrayBuilderNode();
        virtual ~ArrayBuilderNode();
        virtual bool isReady() const;
        virtual void process();
        virtual bool isFinished() const;

        void sort();
        size_t length() {return veclen;}
        XYArray *getArray();
};


class SCAVE_API ArrayBuilderNodeType : public SingleSinkNodeType
{
    friend class ArrayBuilderNode;
    public:
        virtual const char *getName() const {return "arraybuilder";}
        virtual const char *getDescription() const;
        virtual bool isHidden() const {return true;}
        virtual void getAttributes(StringMap& attrs) const;
        virtual void getAttrDefaults(StringMap& attrs) const;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const;
};

NAMESPACE_END


#endif


