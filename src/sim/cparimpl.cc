//=========================================================================
//  CPARIMPL.CC - part of
//
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

#include <sstream>
#include "omnetpp/cparimpl.h"
#include "omnetpp/cproperties.h"
#include "omnetpp/ccomponent.h"
#include "omnetpp/csimulation.h"

#include "omnetpp/cboolparimpl.h"
#include "omnetpp/clongparimpl.h"
#include "omnetpp/cdoubleparimpl.h"
#include "omnetpp/cstringparimpl.h"
#include "omnetpp/cxmlparimpl.h"

#ifdef WITH_PARSIM
#include "omnetpp/ccommbuffer.h"
#endif

NAMESPACE_BEGIN

long cParImpl::total_parimpl_objs;
long cParImpl::live_parimpl_objs;
cStringPool cParImpl::unitStringPool("cParImpl::unitStringPool");


cParImpl::cParImpl()
{
    unitp = NULL;
    total_parimpl_objs++;
    live_parimpl_objs++;
}

cParImpl::~cParImpl()
{
    unitStringPool.release(unitp);
    live_parimpl_objs--;
}

void cParImpl::copy(const cParImpl& other)
{
    setUnit(other.getUnit());
}

cParImpl& cParImpl::operator=(const cParImpl& other)
{
    bool shared = isShared();
    cNamedObject::operator=(other);
    copy(other);
    setIsShared(shared); // preserve FL_ISSHARED flag
    return *this;
}

void cParImpl::parsimPack(cCommBuffer *buffer) const
{
    //TBD
}

void cParImpl::parsimUnpack(cCommBuffer *buffer)
{
    //TBD
}

std::string cParImpl::info() const
{
    return str();
}

std::string cParImpl::detailedInfo() const
{
    std::stringstream out;
    out << cPar::getTypeName(getType()) << " " << getName();
    if (containsValue())
    {
        if (isSet())
            out << " = " << str();
        else
            out << " = default(" << str() << ")";
        out << " isExpression=" << (isExpression()?"true":"false");
    }
    else
    {
        out << " (unassigned)";
    }
    return out.str();
}

cParImpl *cParImpl::dup() const
{
    throw cRuntimeError(this, E_CANTDUP);  // cannot instantiate an abstract class
}

const char *cParImpl::getUnit() const
{
    return unitp;
}

void cParImpl::setUnit(const char *s)
{
    unitStringPool.release(unitp);
    unitp = unitStringPool.get(s);
}

cNEDValue cParImpl::evaluate(cExpression *expr, cComponent *context) const
{
    static int depth;
    try
    {
        depth++;
        if (depth >= 5)
            throw cRuntimeError("Evaluation nesting too deep (circular parameter references?)");
        cNEDValue ret = expr->evaluate(context);
        depth--;
        return ret;
    }
    catch (std::exception& e)
    {
        depth = 0;
        throw;
    }
}

bool cParImpl::containsConstSubexpressions() const
{
    cExpression *expr = getExpression();
    return expr==NULL ? false : expr->containsConstSubexpressions();
}

void cParImpl::evaluateConstSubexpressions(cComponent *context)
{
    cExpression *expr = getExpression();
    if (expr)
        expr->evaluateConstSubexpressions(context);
}

int cParImpl::compare(const cParImpl *other) const
{
    int res = strcmp(getName(), other->getName());
    if (res!=0)
        return res;

    unsigned short flags2 = flags & ~FL_ISSHARED; // ignore "isShared" flag
    unsigned short otherflags2 = other->flags & ~FL_ISSHARED;
    if (flags2!=otherflags2)
        return flags2 < otherflags2 ? -1 : 1;

    if (getType()!=other->getType())
        return getType() < other->getType() ? -1 : 1;

    return opp_strcmp(unitp, other->unitp);
}

cParImpl *cParImpl::createWithType(Type type)
{
    switch (type)
    {
        case cPar::BOOL:    return new cBoolParImpl();
        case cPar::DOUBLE:  return new cDoubleParImpl();
        case cPar::LONG:    return new cLongParImpl();
        case cPar::STRING:  return new cStringParImpl();
        case cPar::XML:     return new cXMLParImpl();
        default: throw cRuntimeError("cParImpl::createWithType(): no such type: %d", type);
    }
}

NAMESPACE_END

