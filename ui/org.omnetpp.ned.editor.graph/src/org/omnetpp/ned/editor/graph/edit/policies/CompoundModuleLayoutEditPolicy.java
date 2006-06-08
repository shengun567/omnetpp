package org.omnetpp.ned.editor.graph.edit.policies;

import java.util.List;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.RectangleFigure;
import org.eclipse.draw2d.XYLayout;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.EditPart;
import org.eclipse.gef.EditPolicy;
import org.eclipse.gef.GraphicalEditPart;
import org.eclipse.gef.LayerConstants;
import org.eclipse.gef.RequestConstants;
import org.eclipse.gef.commands.Command;
import org.eclipse.gef.editpolicies.ResizableEditPolicy;
import org.eclipse.gef.requests.ChangeBoundsRequest;
import org.eclipse.gef.requests.CreateRequest;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.ned.editor.graph.model.commands.CloneSubmoduleCommand;
import org.omnetpp.ned.editor.graph.model.commands.CreateSubmoduleCommand;
import org.omnetpp.ned.editor.graph.model.commands.SetConstraintCommand;
import org.omnetpp.ned2.model.INamedGraphNode;
import org.omnetpp.ned2.model.ISubmoduleContainer;


public class CompoundModuleLayoutEditPolicy extends DesktopLayoutEditPolicy {

    public CompoundModuleLayoutEditPolicy(XYLayout layout) {
        super();
        setXyLayout(layout);
    }

    /**
     * Override to return the <code>Command</code> to perform an {@link
     * RequestConstants#REQ_CLONE CLONE}. By default, <code>null</code> is
     * returned.
     * 
     * @param request
     *            the Clone Request
     * @return A command to perform the Clone.
     */
    @SuppressWarnings("unchecked")
	@Override
    protected Command getCloneCommand(ChangeBoundsRequest request) {
        CloneSubmoduleCommand cloneCmd = new CloneSubmoduleCommand((ISubmoduleContainer) getHost().getModel());

        for (GraphicalEditPart currPart : (List<GraphicalEditPart>)request.getEditParts()) {
            cloneCmd.addModule((INamedGraphNode)currPart.getModel(), 
            					(Rectangle) getConstraintForClone(currPart, request));
        }
        return cloneCmd;
    }

    @Override
    protected Command getCreateCommand(CreateRequest request) {
        CreateSubmoduleCommand create 
        		= new CreateSubmoduleCommand((ISubmoduleContainer) getHost().getModel(),
        									 (INamedGraphNode) request.getNewObject());
        create.setLocation((Rectangle)getConstraintFor(request));
        create.setLabel("Add");

        return create;
    }

    @Override
    protected Command createChangeConstraintCommand(EditPart child, Object constraint) {
        // HACK for fixing issue when the model returns unspecified size (-1,-1)
        // we have to calculate the center point in that direction manually using the size info
        // from the figure directly (which knows it's size) This is the inverse transformation of
        // CenteredXYLayout's traf.
        Rectangle figureBounds = ((GraphicalEditPart)child).getFigure().getBounds();
        Rectangle modelConstraint = (Rectangle)constraint;
        if (modelConstraint.width < 0) modelConstraint.x += figureBounds.width / 2;
        if (modelConstraint.height < 0) modelConstraint.y += figureBounds.height / 2;

        // create the constraint change command 
        INamedGraphNode module = (INamedGraphNode) child.getModel();
        SetConstraintCommand cmd = new SetConstraintCommand(module);
        cmd.setConstraint(modelConstraint);

        // if size constrant is not specified, then remove it from the model too
        // TODO is this needed?
        if ((modelConstraint.width < 0 || modelConstraint.height < 0) && module.getDisplayString().getSize() == null)
            cmd.setSize(null);
        
        return cmd;
    }

    /**
     * We create a generic resize policy that allows resizing in any direction. 
     * @see org.eclipse.gef.editpolicies.ConstrainedLayoutEditPolicy#createChildEditPolicy(org.eclipse.gef.EditPart)
     * 
     */
    @Override
    protected EditPolicy createChildEditPolicy(EditPart child) {
        ResizableEditPolicy policy = new ResizeFeedbackEditPolicy();
        return policy;
    }

    /*
     * (non-Javadoc)
     * 
     * @see org.eclipse.gef.editpolicies.LayoutEditPolicy#createSizeOnDropFeedback(org.eclipse.gef.requests.CreateRequest)
     */
    // this command is created when the user adds a new child by drag/resizing with a creation
    // tool selected. ie. specifying the width/length for the new child along with the location
    @Override
    protected IFigure createSizeOnDropFeedback(CreateRequest createRequest) {
        IFigure figure;

         figure = new RectangleFigure();
        ((RectangleFigure) figure).setFill(false);
        figure.setForegroundColor(ColorFactory.highlight);

        addFeedback(figure);
        return figure;
    }

    /**
     * Returns the layer used for displaying feedback. We must return the scaled feedback layer
     * 
     * @return the feedback layer
     */
    @Override
    protected IFigure getFeedbackLayer() {
        return getLayer(LayerConstants.SCALED_FEEDBACK_LAYER);
    }
}