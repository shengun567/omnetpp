package org.omnetpp.test.gui.access;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.Label;
import org.eclipse.gef.ui.palette.FlyoutPaletteComposite;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Canvas;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.common.util.IPredicate;
import org.omnetpp.common.util.ReflectionUtils;
import org.omnetpp.ned.editor.graph.edit.CompoundModuleEditPart;
import org.omnetpp.test.gui.core.InUIThread;
import org.omnetpp.test.gui.core.NotInUIThread;

public class CompoundModuleEditPartAccess extends EditPartAccess
{
	public CompoundModuleEditPartAccess(CompoundModuleEditPart compoundModuleeditPart) {
		super(compoundModuleeditPart);
	}
	
	public CompoundModuleEditPart getCompoundModuleEditPart() {
		return (CompoundModuleEditPart)editPart;
	}

	@InUIThread
	public FlyoutPaletteCompositeAccess getFlyoutPaletteComposite() {
		Composite composite = editPart.getRoot().getViewer().getControl().getParent().getParent();
		return new FlyoutPaletteCompositeAccess((FlyoutPaletteComposite)findDescendantControl(composite, FlyoutPaletteComposite.class));
	}

	@NotInUIThread
	public void createSubModuleWithPalette(String type, String name, int x, int y) {
		getFlyoutPaletteComposite().clickButtonWithLabel(type);
		clickBackground(x, y);
		String namePattern = Character.toLowerCase(type.charAt(0)) + type.substring(1) + ".*";
		renameSubmodule(namePattern, name);
	}

	@InUIThread
	public void clickBackground(int x, int y) {
		FigureAccess figureAccess = new FigureAccess(getFigure());
		figureAccess.click(LEFT_MOUSE_BUTTON, figureAccess.toDisplay(x, y));
	}

	@NotInUIThread
	public void renameSubmodule(String oldName, String newName) {
		clickSubmoduleFigureWithName(oldName);
		typeInNewName(newName);
	}

	@InUIThread
	public void typeInNewName(String name) {
		Canvas canvas = new FigureAccess(getFigure()).getCanvas();
		Text text = (Text)Access.findDescendantControl(canvas, Text.class);
		TextAccess textAccess = new TextAccess(text);
		textAccess.pressKey(SWT.F6);
		textAccess.typeIn(name);
		textAccess.pressEnter();
	}

	@InUIThread
	public void clickSubmoduleFigureWithName(final String name) {
		IFigure compoundModuleFigure = getFigure();
		IFigure labelFigure = findDescendantFigure(compoundModuleFigure, new IPredicate() {
			public boolean matches(Object object) {
				return object instanceof Label && ((Label)object).getText().matches(name);
			}
		});

		new FigureAccess(labelFigure).click(LEFT_MOUSE_BUTTON);
	}

	@NotInUIThread
	public void createConnectionWithPalette(String name1, String name2, String connectionOptionLabel) {
		getFlyoutPaletteComposite().clickButtonWithLabel("Connection");
		clickSubmoduleFigureWithName(name1);
		clickSubmoduleFigureWithName(name2);
		clickConnectionOption(connectionOptionLabel);
	}

	@InUIThread
	protected void clickConnectionOption(String label) {
		new MenuAccess(getDisplay().getActiveShell().getMenu()).activateMenuItemWithMouse(label);
	}

	protected IFigure getFigure() {
		IFigure compoundModuleFigure = (IFigure)ReflectionUtils.invokeMethod(editPart, "getCompoundModuleFigure");
		return compoundModuleFigure;
	}
}
