#ifndef SIMULATORANALOGICALSCHEDULINGPOLICY_H
#define SIMULATORANALOGICALSCHEDULINGPOLICY_H

#include <scheduling/schedulingpolicy.h>
#include <scheduling/analogicalresertaviontable.h>

namespace Simulator {

/**
 @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

/** This class is an intermediate class that agregates all the methods for those Scheduling policies that are using a analogical reservation table. For instance the findAllocation methods */ 
class AnalogicalSchedulingPolicy : public SchedulingPolicy{
public:
  AnalogicalSchedulingPolicy();
  ~AnalogicalSchedulingPolicy();
  
protected:
  /* protected methods */  
  /* the following three methods are virtual since other easy backfilling variants may need to tunne the behavriour of the allocation procedure */
  virtual AnalogicalJobAllocation* findAllocation(Job* job, double starttime,double runtime);
  virtual void releaseJobResources(Job* job); 
  
    /* AUXILIAR FUNCTIONS */
  AnalogicalBucket* findMinimumStartTimeBucket(BucketSetOrdered* set); 
  AnalogicalBucket* findNextGreaterStTimeBucket(BucketSetOrdered* set, AnalogicalBucket* referenceBucket);
  AnalogicalBucket* findMaximumStartTimeBucket(BucketSetOrdered* set);
  

  
  /**
  * Allocates the job to the provided allocation 
  * @param job The job to allocated 
  * @param allocation The allocation for the job 
  */
  virtual void allocateJob(Job* job, AnalogicalJobAllocation* allocation) = 0; 

};

}

#endif
