package org.omnetpp.cdt.makefile;

import java.util.HashMap;
import java.util.Map;

import org.eclipse.core.resources.IContainer;


/**
 * Represents contents of the OMNeT++ build specification file
 * @author Andras
 */
public class BuildSpecification {
    private Map<IContainer,MakemakeOptions> folderMakemakeOptions = new HashMap<IContainer,MakemakeOptions>();

    public BuildSpecification() {
    }
    
    /** 
     * Returns the set of folders for which there's some explicitly set
     * folder type or option.
     */
    public IContainer[] getMakemakeFolders() {
        return folderMakemakeOptions.keySet().toArray(new IContainer[]{});
    }

    public boolean isMakemakeFolder(IContainer folder) {
        // FIXME should return true ONLY for folders WITHIN a deep makefiles source tree
//        return folderMakemakeOptions.containsKey(folder);
        return true;
    }

    /**
     * Returns null if there's no makefile to be generated in that folder.
     */
    public MakemakeOptions getMakemakeOptions(IContainer folder) {
        return folderMakemakeOptions.get(folder);
    }

    public void setMakemakeOptions(IContainer folder, MakemakeOptions options) {
        if (options == null)
            folderMakemakeOptions.remove(folder);
        else 
            folderMakemakeOptions.put(folder, options);
    }

}
