package org.omnetpp.figures;

import org.eclipse.draw2d.Label;
import org.eclipse.gef.tools.CellEditorLocator;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.figures.misc.IDirectEditSupport;
import org.omnetpp.figures.misc.LabelCellEditorLocator;

/**
 * Figure to represent top level components like SImpleModule, CHannel,CHannel IF and Module IF
 * @author rhornig
 */
public class TopLevelFigure extends Label implements IDirectEditSupport {

    private String tmpName;
    private TooltipFigure tooltipFigure;

    protected void setShape(Image img,
            String shape, int shapeWidth, int shapeHeight,
            Color shapeFillColor, Color shapeBorderColor, int shapeBorderWidth) {
        // handle defaults. if no size is given and no icon is present use a default icon
        if(shapeWidth <= 0 && shapeHeight <= 0 && img == null)
            img = ImageFactory.getImage(ImageFactory.DEFAULT_KEY);

        setIcon(img);
    }

    /**
	 * Adjusts the image properties using a displayString object (except the location and size)
	 * @param dps
	 */
	public void setDisplayString(IDisplayString dps) {

        // shape support
        String imgSize = dps.getAsStringDef(IDisplayString.Prop.IMAGESIZE);
        Image img = ImageFactory.getImage(
        		dps.getAsStringDef(IDisplayString.Prop.IMAGE),
        		imgSize,
        		ColorFactory.asRGB(dps.getAsStringDef(IDisplayString.Prop.IMAGECOLOR)),
        		dps.getAsIntDef(IDisplayString.Prop.IMAGECOLORPCT,0));

        // set the figure properties
        setShape(img,
        		dps.getAsStringDef(IDisplayString.Prop.SHAPE),
        		dps.getSize(null).width,
        		dps.getSize(null).height,
        		ColorFactory.asColor(dps.getAsStringDef(IDisplayString.Prop.FILLCOL)),
        		ColorFactory.asColor(dps.getAsStringDef(IDisplayString.Prop.BORDERCOL)),
        		dps.getAsIntDef(IDisplayString.Prop.BORDERWIDTH, -1));

        invalidate();
	}

    public void setTooltipText(String tttext) {
        if(tttext == null || "".equals(tttext)) {
            setToolTip(null);
            tooltipFigure = null;
        } else {
            tooltipFigure = new TooltipFigure();
            setToolTip(tooltipFigure);
            tooltipFigure.setText(tttext);
            invalidate();
        }
    }

    public CellEditorLocator getDirectEditCellEditorLocator() {
        return new LabelCellEditorLocator(this);
    }

    public String getDirectEditText() {
        return getText();
    }

    public void setDirectEditTextVisible(boolean visible) {
        // HACK to hide the text part only of the label
        if (!visible) {
            tmpName = getText();
            setText("");
        } else {
            if ("".equals(getText()))
                setText(tmpName);
        }
    }

    @Override
	public String toString() {
	    return getClass().getSimpleName()+" "+getText();
	}

    // TODO implement error decoration
    public void setErrorDecoration(boolean markError) {
        // setForegroundColor(markError ? ColorFactory.RED : null);
    }

}

