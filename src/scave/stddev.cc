//=========================================================================
//  STDDEV.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Tamas Borbely
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <math.h>
#include "channel.h"
#include "stddev.h"

USING_NAMESPACE


StddevNode::StddevNode()
{
}

StddevNode::~StddevNode()
{
}

bool StddevNode::isReady() const
{
    return in()->length()>0;
}

void StddevNode::process()
{
    int n = in()->length();
    for (int i=0; i<n; i++)
    {
        Datum a;
        in()->read(&a,1);
        collect(a.y);
    }
}

bool StddevNode::finished() const
{
    return in()->eof();
}

void StddevNode::collect(double val)
{
    if (++num_vals <= 0)
        throw opp_runtime_error("StddevNode: observation count overflow");

    sum_vals += val;
    sqrsum_vals += val*val;

    if (num_vals>1)
    {
        if (val<min_vals)
            min_vals = val;
        else if (val>max_vals)
            max_vals = val;
    }
    else
    {
        min_vals = max_vals = val;
    }
}

double StddevNode::variance() const
{
    if (num_vals<=1)
        return 0.0;
    else
    {
        double devsqr = (sqrsum_vals - sum_vals*sum_vals/num_vals)/(num_vals-1);
        if (devsqr<=0)
            return 0.0;
        else
            return devsqr;
    }
}

double StddevNode::stddev() const
{
    return sqrt( variance() );
}


//------

const char *StddevNodeType::description() const
{
    return "Collects basic statistics like min, max, mean, stddev.";
}

void StddevNodeType::getAttributes(StringMap& attrs) const
{
}

Node *StddevNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    Node *node = new StddevNode();
    node->setNodeType(this);
    mgr->addNode(node);
    return node;
}


