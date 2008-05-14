//=========================================================================
//  NODETYPEREGISTRY.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _NODETYPEREGISTRY_H_
#define _NODETYPEREGISTRY_H_

#include <map>
#include <vector>
#include <string>
#include "node.h"
#include "nodetype.h"

NAMESPACE_BEGIN

class DataflowManager;

typedef std::vector<NodeType*> NodeTypeVector;


/**
 * Factory for all available node types.
 *
 * @see NodeType, Node
 */
class SCAVE_API NodeTypeRegistry
{
    protected:
        static NodeTypeRegistry *inst;
        typedef std::map<std::string,NodeType*> NodeTypeMap;
        NodeTypeMap nodeTypeMap;

        NodeTypeRegistry();
        ~NodeTypeRegistry();

    public:
        /** Access to singleton instance */
        static NodeTypeRegistry *instance();

        /** Returns list of node types */
        NodeTypeVector getNodeTypes();

        /** True if a node type with the given name exists */
        bool exists(const char *name);

        /** Returns a node type by name */
        NodeType *getNodeType(const char *name);

        /** Parse filter spec, and create the corresponding node */
        Node *createNode(const char *filterSpec, DataflowManager *mgr);

        /**
         * Parse the given string in form "name(arg1,arg2,...)" into a
         * filter name and filter args list.
         */
        static void parseFilterSpec(const char *filterSpec, std::string& name, std::vector<std::string>& args);

        /** Add a node type to the factory */
        void add(NodeType *nodetype);

        /** Remove a node type from the factory */
        void remove(NodeType *nodetype);

};

NAMESPACE_END


#endif
