//==========================================================================
//  CSTAT.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cStatistic : base for statistics
//    cStdDev    : collects min, max, mean, standard deviation
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CSTAT_H
#define __CSTAT_H

#include "defs.h"

#include <stdio.h>
#include "cownedobject.h"

class cTransientDetection;
class cAccuracyDetection;

//==========================================================================

/**
 * Base class of different statistic collecting classes.
 * cStatistic is the base class for all statistical data
 * collection classes. cStatistic itself adds no data members
 * or algorithms to cOwnedObject, it only defines virtual functions
 * that will be redefined in descendants. No instance of cStatistic
 * can be created.
 *
 * @ingroup Statistics
 */
class SIM_API cStatistic : public cOwnedObject
{
  public:
    cTransientDetection *td;    // ptr to associated object
    cAccuracyDetection *ra;     // ptr to associated object
    int genk;                   // index of random number generator to use

  protected:

    // internal: utility function for implementing loadFromFile() functions
    void freadvarsf (FILE *f,  const char *fmt, ...);

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Copy constructor.
     */
    cStatistic(const cStatistic& r);

    /**
     * Constructor, creates an object with the given name
     */
    explicit cStatistic(const char *name=NULL);

    /**
     * Destructor.
     */
    virtual ~cStatistic();

    /**
     * Assignment operator. It is present since descendants may refer to it.
     * The name member doesn't get copied; see cNamedObject's operator=() for more details.
     */
    cStatistic& operator=(const cStatistic& res);
    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /* Note: No dup() because this is an abstract class! */

    /**
     * Serializes the object into an MPI send buffer.
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual void netPack(cCommBuffer *buffer);

    /**
     * Deserializes the object from an MPI receive buffer
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual void netUnpack(cCommBuffer *buffer);
    //@}

    /** @name Collecting values. */
    //@{

    /**
     * Collects one value.
     * This method is pure virtual, implementation is provided in subclasses.
     */
    virtual void collect(double val) = 0;

    /**
     * Collects one value with a given weight.
     */
    virtual void collect2(double val, double weight);

    /**
     * Same as the collect(double) method.
     */
    void operator+= (double val) {collect(val);}

    /**
     * This function should be redefined in derived classes to clear
     * the results collected so far.
     * This method is pure virtual, implementation is provided in subclasses.
     */
    virtual void clearResult() = 0;
    //@}

    /** @name Statistics of collected data. */
    //@{

    /**
     * Returns the number of samples collected.
     * This method is pure virtual, implementation is provided in subclasses.
     */
    virtual long samples() const = 0;

    /**
     * Returns the sum of weights of the samples collected.
     * This method is pure virtual, implementation is provided in subclasses.
     */
    virtual double weights() const = 0;

    /**
     * Returns the sum of samples collected.
     * This method is pure virtual, implementation is provided in subclasses.
     */
    virtual double sum() const = 0;

    /**
     * Returns the squared sum of the collected data.
     * This method is pure virtual, implementation is provided in subclasses.
     */
    virtual double sqrSum() const = 0;

    /**
     * Returns the minimum of the samples collected.
     * This method is pure virtual, implementation is provided in subclasses.
     */
    virtual double min() const = 0;

    /**
     * Returns the maximum of the samples collected.
     * This method is pure virtual, implementation is provided in subclasses.
     */
    virtual double max() const = 0;

    /**
     * Returns the mean of the samples collected.
     * This method is pure virtual, implementation is provided in subclasses.
     */
    virtual double mean() const = 0;

    /**
     * Returns the standard deviation of the samples collected.
     * This method is pure virtual, implementation is provided in subclasses.
     */
    virtual double stddev() const = 0;

    /**
     * Returns the variance of the samples collected.
     * This method is pure virtual, implementation is provided in subclasses.
     */
    virtual double variance() const = 0;
    //@}

    /** @name Transient and result accuracy detection. */
    //@{

    /**
     * Assigns transient and accuracy detection objects to the statistic
     * object.
     */
    void addTransientDetection(cTransientDetection *object);

    /**
     * Assigns transient and accuracy detection objects to the statistic
     * object.
     */
    void addAccuracyDetection(cAccuracyDetection *object);

    /**
     * Returns the assigned transient and accuracy detection objects.
     */
    cTransientDetection *transientDetectionObject() const  {return td;}

    /**
     * Returns the assigned transient and accuracy detection objects.
     */
    cAccuracyDetection  *accuracyDetectionObject() const   {return ra;}
    //@}

    /** @name Generating random numbers based on the collected data */
    //@{

    /**
     * Sets the index of the random number generator to use when the
     * object has to generate a random number based on the statistics
     * stored.
     */
    void setGenK(int gen_nr)   {genk=gen_nr;}

    /**
     * Generates a random number based on the collected data. Uses the random number generator set by setGenK().
     * This method is pure virtual, implementation is provided in subclasses.
     */
    virtual double random() const = 0;
    //@}

    /** @name Writing to text file, reading from text file, recording to scalar file. */
    //@{

    /**
     * Writes the contents of the object into a text file.
     * This method is pure virtual, implementation is provided in subclasses.
     */
    virtual void saveToFile(FILE *) const = 0;

    /**
     * Reads the object data from a file written out by saveToFile().
     * This method is pure virtual, implementation is provided in subclasses.
     */
    virtual void loadFromFile(FILE *) = 0;

    /**
     * Records basic statistics (number of observations, mean, standard
     * deviation, min, max) into the scalar output file by performing
     * several calls to the current module's recordScalar() function.
     * The values will be written under the name "name.samples",
     * "name.mean", "name.stddev", "name.min", "name.max". If name is NULL
     * or missing, the object name (name()) is used.
     * This method may be overridden in subclasses.
     */
    virtual void recordScalar(const char *name=NULL);
    //@}
};

//==========================================================================

/**
 * Statistics class to collect min, max, mean, standard deviation.
 *
 * @ingroup Statistics
 */
class SIM_API cStdDev : public cStatistic
{
  protected:
    long num_samples;
    double min_samples,max_samples;
    double sum_samples,sqrsum_samples;

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Copy constructor.
     */
    cStdDev(const cStdDev& r) : cStatistic() {setName(r.name());operator=(r);}

    /**
     * Constructor.
     */
    explicit cStdDev(const char *name=NULL);

    /**
     * Destructor.
     */
    virtual ~cStdDev() {}

    /**
     * Assignment operator. The name member doesn't get copied; see cNamedObject's operator=() for more details.
     */
    cStdDev& operator=(const cStdDev& res);
    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cStdDev *dup() const  {return new cStdDev(*this);}

    /**
     * Produces a one-line description of object contents.
     * See cObject for more details.
     */
    virtual std::string info() const;

    /**
     * Produces a multi-line description of the object.
     * See cObject for more details.
     */
    virtual std::string detailedInfo() const;

    /**
     * Serializes the object into an MPI send buffer.
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual void netPack(cCommBuffer *buffer);

    /**
     * Deserializes the object from an MPI receive buffer
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual void netUnpack(cCommBuffer *buffer);
    //@}

    /** @name Redefined cStatistic functions. */
    //@{

    /**
     * Collects one value.
     */
    virtual void collect(double val);

    /**
     * Returns the number of samples collected.
     */
    virtual long samples() const   {return num_samples;}

    /**
     * Returns the sum of weights of the samples collected.
     */
    virtual double weights() const {return num_samples;}

    /**
     * Returns the sum of samples collected.
     */
    virtual double sum() const     {return sum_samples;}

    /**
     * Returns the squared sum of the collected data.
     */
    virtual double sqrSum() const  {return sqrsum_samples;}

    /**
     * Returns the minimum of the samples collected.
     */
    virtual double min() const     {return min_samples;}

    /**
     * Returns the maximum of the samples collected.
     */
    virtual double max() const     {return max_samples;}

    /**
     * Returns the mean of the samples collected.
     */
    virtual double mean() const    {return num_samples ? sum_samples/num_samples : 0.0;}

    /**
     * Returns the standard deviation of the samples collected.
     */
    virtual double stddev() const;

    /**
     * Returns the variance of the samples collected.
     */
    virtual double variance() const;

    /**
     * Returns numbers from a normal distribution with the current mean and
     * standard deviation.
     */
    virtual double random() const;

    /**
     * Clears the results collected so far.
     */
    virtual void clearResult();

    /**
     * Writes the contents of the object into a text file.
     */
    virtual void saveToFile(FILE *) const;

    /**
     * Reads the object data from a file written out by saveToFile()
     * (or written by hand)
     */
    virtual void loadFromFile(FILE *);
    //@}
};

//==========================================================================

/**
 * Statistics class to collect doubles and calculate weighted statistics
 * of them. It can be used for example to calculate time average.
 *
 * @ingroup Statistics
 */
class SIM_API cWeightedStdDev : public cStdDev
{
  protected:
    double sum_weights;

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Constructors, destructor, duplication and assignment.
     */
    cWeightedStdDev(const cWeightedStdDev& r) : cStdDev() {setName(r.name());operator=(r);}

    /**
     * Constructors, destructor, duplication and assignment.
     */
    explicit cWeightedStdDev(const char *name=NULL) : cStdDev(name)  {sum_weights=0;}

    /**
     * Constructors, destructor, duplication and assignment.
     */
    virtual ~cWeightedStdDev() {}

    /**
     * Assignment operator. The name member doesn't get copied; see cNamedObject's operator=() for more details.
     */
    cWeightedStdDev& operator=(const cWeightedStdDev& res);
    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cWeightedStdDev *dup() const  {return new cWeightedStdDev(*this);}

    /**
     * Produces a one-line description of object contents.
     * See cObject for more details.
     */
    virtual std::string info() const;

    /**
     * Serializes the object into an MPI send buffer.
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual void netPack(cCommBuffer *buffer);

    /**
     * Deserializes the object from an MPI receive buffer
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual void netUnpack(cCommBuffer *buffer);
    //@}

    /** @name Redefined cStatistic functions. */
    //@{

    /**
     * Collects one value.
     */
    virtual void collect(double val)  {collect2(val,1.0);}

    /**
     * Collects one value with a given weight.
     */
    virtual void collect2(double val, double weight);

    /**
     * Clears the results collected so far.
     */
    virtual void clearResult();

    /**
     * Returns the sum of weights of the samples collected.
     */
    virtual double weights() const {return sum_weights;}

    /**
     * Returns the mean of the samples collected.
     */
    virtual double mean() const    {return sum_weights!=0 ? sum_samples/sum_weights : 0.0;}

    /**
     * Returns the variance of the samples collected.
     */
    virtual double variance() const;

    /**
     * Writes the contents of the object into a text file.
     */
    virtual void saveToFile(FILE *) const;

    /**
     * Reads the object data from a file, in the format written out by saveToFile().
     */
    virtual void loadFromFile(FILE *);
    //@}
};

#endif

