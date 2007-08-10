package org.omnetpp.ned.editor.graph.properties;

import org.eclipse.ui.views.properties.IPropertySource;

/**
 * Objects that support property sheets should implement this interface
 *
 * @author rhornig
 */
public interface IPropertySourceSupport {

    /**
     * Returns the cached property source (if any)
     */
    public IPropertySource getPropertySource();

    /**
     * @param propertySource the property source to be cached for later use
     */
    public void setPropertySource(IPropertySource propertySource);
}
