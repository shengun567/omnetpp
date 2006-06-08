package org.omnetpp.ned.editor.graph.model.commands;

import java.util.LinkedList;
import java.util.List;

import org.eclipse.gef.commands.Command;
import org.omnetpp.ned2.model.NEDElement;

/**
 * Clones a set of {@link NEDElement} (copy operation)
 * @author rhornig
 */
public class ClonePartsCommand extends Command {

    private List<NEDElement> newNodes;
    // nodes that should be cloned by the command
    private List<NEDElement> nodes;
    // parent node where cloned nodes should go
    private NEDElement parent;
    // sibling node before the cloned nodes should be inserted (null means insertion at the end)
    private NEDElement insertBefore;

    /**
     * Creates a generic cloning command that is able to clona any number of nodes and add them
     * to the provided parent node.
     * @param parent The parent node where the newly cloned nodes should go
     * @param insertBefore A sibling in the parent before we should insert the cloned nodes
     *        <code>null</code> should be used to insert at the end of the childlist
     */
    public ClonePartsCommand(NEDElement parent, NEDElement insertBefore) {
        super("Clone");
        this.parent = parent;
        this.insertBefore = insertBefore;
        nodes = new LinkedList<NEDElement>();
    }
    
    /**
     * Add a new {@link NEDElement} to the cloning list.
     * @param nodeToBeCloned
     */
    public void addPart(NEDElement nodeToBeCloned) {
        nodes.add(nodeToBeCloned);
    }
    
    @Override
    public boolean canExecute() {
    	return parent != null && nodes.size() > 0;
    }

    // clone just a single node and insert it into the correct posistion
    protected NEDElement clonePart(NEDElement oldNode ) {
        // duplicate the subtree but do not add to the new parent yet
    	NEDElement newNode = oldNode.deepDup(null);
        // TODO we should set a unique name for the cloned nodes here

    	// insert into the parent at the correct position
        parent.insertChildBefore(insertBefore, newNode);

        // keep track of the new modules so we can delete them in undo
        newNodes.add(newNode);
        
        return newNode;
    }

    @Override
    public void execute() {
        redo();
    }

    @Override
    public void redo() {
        newNodes = new LinkedList<NEDElement>();
        for (NEDElement toBeCloned : nodes)
        	clonePart(toBeCloned);
    }

    @Override
    public void undo() {
        for (NEDElement mod : newNodes)
            mod.removeFromParent();
    }

}