//==========================================================================
//  CSTLWATCH.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  WATCH_VECTOR, WATCH_MAP etc macros
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>
#include "omnetpp/cstlwatch.h"
#include "omnetpp/cclassdescriptor.h"
#include "omnetpp/globals.h"

NAMESPACE_BEGIN

//
// Internal
//
class SIM_API cStdVectorWatcherDescriptor : public cClassDescriptor //noncopyable
{
  private:
    std::string vectorTypeName;  // type name of the inspected type, e.g. "std::vector<foo::Bar>"
    std::string elementTypeName; // type name of vector elements, e.g. "foo::Bar"
  public:
    cStdVectorWatcherDescriptor(const char *vecTypeName, const char *elemTypeName);
    virtual ~cStdVectorWatcherDescriptor();

    virtual const char **getPropertyNames() const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount() const;
    virtual const char *getFieldName(int field) const;
    virtual unsigned int getFieldTypeFlags(int field) const;
    virtual const char *getFieldTypeString(int field) const;
    virtual const char **getFieldPropertyNames(int field) const;
    virtual const char *getFieldProperty(int field, const char *propertyname) const;
    virtual int getFieldArraySize(void *object, int field) const;

    virtual std::string getFieldValueAsString(void *object, int field, int i) const;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(int field) const;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const;
};

cStdVectorWatcherDescriptor::cStdVectorWatcherDescriptor(const char *vecType, const char *elemType) :
cClassDescriptor(vecType, NULL)
{
    vectorTypeName = vecType;
    elementTypeName = elemType;
}

cStdVectorWatcherDescriptor::~cStdVectorWatcherDescriptor()
{
}

const char **cStdVectorWatcherDescriptor::getPropertyNames() const
{
    static const char **names = { NULL };
    return names;
}

const char *cStdVectorWatcherDescriptor::getProperty(const char *propertyname) const
{
    return NULL;
}

int cStdVectorWatcherDescriptor::getFieldCount() const
{
    return 1;
}

unsigned int cStdVectorWatcherDescriptor::getFieldTypeFlags(int field) const
{
    return FD_ISARRAY; //TODO we could return FD_ISCOMPOUND, FD_ISPOINTER, FD_ISCOBJECT / FD_ISCOWNEDOBJECT, with a little help from cStdVectorWatcherBase, and then elements would become inspectable (currently they displayed as just strings)
}

const char *cStdVectorWatcherDescriptor::getFieldName(int field) const
{
    return "elements";
}

const char *cStdVectorWatcherDescriptor::getFieldTypeString(int field) const
{
    return elementTypeName.c_str();
}

const char **cStdVectorWatcherDescriptor::getFieldPropertyNames(int field) const
{
    static const char **names = { NULL };
    return names;
}

const char *cStdVectorWatcherDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    return NULL;
}

int cStdVectorWatcherDescriptor::getFieldArraySize(void *object, int field) const
{
    cStdVectorWatcherBase *pp = (cStdVectorWatcherBase *)object;
    return pp->size();
}

std::string cStdVectorWatcherDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    cStdVectorWatcherBase *pp = (cStdVectorWatcherBase *)object;
    return pp->at(i);
}

bool cStdVectorWatcherDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    return false; // not supported
}

const char *cStdVectorWatcherDescriptor::getFieldStructName(int field) const
{
    return NULL;  //TODO we could return elementTypeName (if it is a compound type; if it's a pointer, the '*' should be removed)
}

void *cStdVectorWatcherDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    return NULL;  //TODO we could return a pointer to the given array element (if element is a compound type)
}


//--------------------------------

std::string cStdVectorWatcherBase::info() const
{
    if (size()==0)
        return std::string("empty");
    std::stringstream out;
    out << "size=" << size();
    return out.str();
}

std::string cStdVectorWatcherBase::detailedInfo() const
{
    std::stringstream out;
    int n = size()<=3 ? size() : 3;
    for (int i=0; i<n; i++)
        out << getFullName() << "[" << i << "] = " << at(i) << "\n";
    if (size()>3)
        out << "...\n";
    return out.str();
}

cClassDescriptor *cStdVectorWatcherBase::getDescriptor()
{
    if (!desc) {
        // try to find existing descriptor for this particular type (e.g. "std::vector<double>");
        // if there isn't, create and register a new one
        desc = (cClassDescriptor *) classDescriptors.getInstance()->lookup(getClassName());
        if (!desc) {
            desc = new cStdVectorWatcherDescriptor(getClassName(), getElemTypeName());
            classDescriptors.getInstance()->add(desc);
        }
    }
    return desc;
}

NAMESPACE_END

