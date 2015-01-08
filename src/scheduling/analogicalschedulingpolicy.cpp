#include <scheduling/analogicalschedulingpolicy.h>

namespace Simulator {

/**
 * The default constructor for the class
 */
AnalogicalSchedulingPolicy::AnalogicalSchedulingPolicy(): SchedulingPolicy()
{

}


/**
 * The default destructor for the class 
 * @return 
 */
AnalogicalSchedulingPolicy::~AnalogicalSchedulingPolicy()
{
}

/**
 * Release the resources used by the job. 
 * @param job The job that has to be realeased 
 */
void AnalogicalSchedulingPolicy::releaseJobResources(Job* job)
{
  AnalogicalJobAllocation* allocation = (AnalogicalJobAllocation*) 
                     ((AnalogicalResertavionTable*)this->reservationTable)->getJobAllocation(job);
  
  deque<Bucket*> forParaver;

  /*
    carrying out the deallocation to the processors 
    it must be done externally to the deAllocate policy due to this
    function may be called while the jobs is still not running !
  */
  
  for(deque< AnalogicalBucket * >::iterator iter = allocation->allocations.begin();
      iter != allocation->allocations.end(); ++iter)  
  {
    AnalogicalBucket* bucket = *iter;
            
    /* for sanity  we check that the cpu is not used by any job ..*/
    assert(((AnalogicalResertavionTable*)this->reservationTable)->CPUMapping[bucket->getcpu()] != 0);
    
    ((AnalogicalResertavionTable*)this->reservationTable)->CPUMapping[bucket->getcpu()] = 0;

    /*if the paraver trace is not null we notify for such deallocation */
    if(this->prvTrace != 0)
      forParaver.push_back((Bucket*) bucket);     
  }
  
  if(this->useParaver)
    this->prvTrace->ParaverJobExit(job->getJobNumber(),forParaver); 

  /* we free the corresponding buckets*/
  this->reservationTable->deAllocateJob(job);  

  /* the job won't be for anymore in the system */
  map<Job*,bool>::iterator jobSysIt = this->JobInTheSystem.find(job);
  assert(jobSysIt != this->JobInTheSystem.end());
  this->JobInTheSystem.erase(jobSysIt);

}

/**
 * Find an an allocation based on the resource sellection polcy set in the simulation and based on the job requirements 
 * ALSO USED IN THE FCFS (AND WITH EASY COLLISION POLICY) SCHEDULING .. IN CASE OF MODIFICATION PLEASE UPDTATE THE fcfs STUFF
 * @param job The job that would like to be allocated 
 * @param starttime The required start time for the allocation 
 * @param runtime The required time for the allocation 
 * @see The class SchedulingPolicy 
 * @return The allocation that mathces the requirements (if there is no allocation it will be detailed in the allocation problem filed )
 */
AnalogicalJobAllocation* AnalogicalSchedulingPolicy::findAllocation(Job* job, double starttime,double runtime)
{

  //ALSO USED IN THE FCFS (AND WITH EASY COLLISION POLICY) SCHEDULING .. IN CASE OF MODIFICATION PLEASE UPDTATE THE fcfs STUFF
  
  AnalogicalJobAllocation* allocationByTime = NULL;
  AnalogicalJobAllocation* allocationByCPUS  = NULL;
  AnalogicalJobAllocation* allocation  = NULL;
 

    /* first we have to allocate the job the reservation table
       we try to find the allocation by the fastest way ..       
       we try to find out the first bucket for each cpu that satisfies that its lenght 
       it's enough for the current job we try to allocate rigth now
       
    */

  //while there are problems of finding cpus in a given starttime, we will iterate and incremeneting the startime for the search.
  
  bool allocated = false;

  while(!allocated)
  {
    BucketSetOrdered suitablebuckets = 
                  ((AnalogicalResertavionTable*)this->reservationTable)->findFirstBucketCpus(starttime,runtime);
    
    
    
    assert(suitablebuckets.size() != 0); //should not occur 
    
    /* we try to find the earliest buckets that shares the required period of time    
    */  
    
    bool useFirstFit = this->getRS_policyUsed() == FIRST_FIT || 
                       this->getRS_policyUsed() == FF_AND_FCF ||
                       this->getRS_policyUsed() == FIND_LESS_CONSUME || 
                       this->getRS_policyUsed() ==  EQUI_DISTRIBUTE_CONSUME;
                       
     ;


    bool useContinousFit = this->getRS_policyUsed() == FIRST_CONTINOUS_FIT || this->getRS_policyUsed() == FF_AND_FCF;
    
    if(useFirstFit)  
      allocationByTime = ((AnalogicalResertavionTable*)this->reservationTable)->findFastMethodAllocation(&suitablebuckets,
                                                                  runtime,
                                                                  job->getNumberProcessors(),
                                                                  starttime
                                                                  );
   if(useContinousFit)
      allocationByCPUS = ((AnalogicalResertavionTable*)this->reservationTable)->findFastConsecutiveMethodAllocation(&suitablebuckets,
                                                                  runtime,
                                                                  job->getNumberProcessors(),
                                                                  starttime
                                                                  );
    /*we will try to allocate the job the earliest location .. and consecutive cpus */
    
    bool continousAllocation = useContinousFit /* the continous fit can be used */
    			       && !allocationByCPUS->getallocationProblem() /* there is no problem with the allocation */
                               && !(this->getRS_policyUsed() == FF_AND_FCF && allocationByTime->getstartTime() <= 
                                  allocationByCPUS->getstartTime()); /* in case of FF_AND_FCF it start time is earlier */
    
    bool firstFitAllocation =  useFirstFit /* the first fit can be used */
                               && !allocationByTime->getallocationProblem() /* there is no proble with the allocation */
                               && !(this->getRS_policyUsed() == FF_AND_FCF && allocationByTime->getstartTime() > 
                                  allocationByCPUS->getstartTime()); /* in case of FF_AND_FCF it start time is earlier */
 
//RUTGERS
    bool dcFirstVmAllocation = false;
    if(this->getRS_policyUsed() == DC_FIRST_VM){

/*
    BucketSetOrdered suitablebuckets = 
                  ((AnalogicalResertavionTable*)this->reservationTable)->findFirstBucketCpus(starttime,runtime);
*/

      int type = job->getQueueNumber();

      allocationByTime = ((AnalogicalResertavionTable*)this->reservationTable)->findDcFirstVmAllocation(&suitablebuckets,
                                                                  runtime,
                                                                  job->getNumberProcessors(),
                                                                  starttime,
								  type,
								  this
                                                                  );
      dcFirstVmAllocation = true;
    }

    if(continousAllocation)
    {
      delete allocationByTime;
      allocation = allocationByCPUS;
      allocated = true;
      job->setAllocatedWith(FIRST_CONTINOUS_FIT);
    }
    else if(firstFitAllocation)
    {
      delete allocationByCPUS;
      allocation = allocationByTime;
      allocated = true;
      job->setAllocatedWith(FIRST_FIT);
    }
    else if(dcFirstVmAllocation)
    {
      delete allocationByCPUS;
      allocation = allocationByTime;
      allocated = true;
      job->setAllocatedWith(DC_FIRST_VM);
    }
    else
    {           
       
       if(log->getlevel() >= 6)
         for(BucketSetOrdered::iterator erit = suitablebuckets.begin();erit != suitablebuckets.end();++erit)
         {
           AnalogicalBucket* erb = *erit;
           log->error("Bucket cpu "+itos(erb->getcpu())+" from "+ftos(erb->getstartTime())+" till "+ftos(erb->getendTime()));         
         }
    
       AnalogicalBucket* next = findMaximumStartTimeBucket(&suitablebuckets);
              
       if(next == NULL)
       {
         log->error("Something went wrong, the job "+itos(job->getJobNumber())+" requires "+itos(job->getNumberProcessors())+" processors. And the suitables are:"+itos(suitablebuckets.size())+ " the maximum it startime is "+ftos(next->getstartTime()));                    
           
         for(BucketSetOrdered::iterator erit = suitablebuckets.begin();erit != suitablebuckets.end();++erit)
         {
           AnalogicalBucket* erb = *erit;
           log->error("Bucket cpu "+itos(erb->getcpu())+" from "+ftos(erb->getstartTime())+" till "+ftos(erb->getendTime()));         
         }
         
         
        assert(false);
       }
      
       starttime = next->getstartTime();                   
      
    }
    
    if(allocated)
    {
      //we set the hint that indicates when the secheduler could continue looking for the suitable allocation 
      AnalogicalBucket* next = findMaximumStartTimeBucket(&suitablebuckets);
      allocation->setNextStartTimeToExplore(next->getstartTime());
    }
    

  }
  assert(allocation != NULL);
  
  return allocation;

}


/**
 * Returns the bucket with the maximum starttime in the provided set (it should be changed .. there is no need to look for all the buckets of the queue, just look the last )
 * @param set The set of buckets where to find 
 * @return The bucket with the maximum starttime 
 */
AnalogicalBucket* AnalogicalSchedulingPolicy::findMaximumStartTimeBucket(BucketSetOrdered* set)
{
  AnalogicalBucket* max = NULL;

  for(BucketSetOrdered::iterator it = set->begin();it != set->end();++it)
  {
    AnalogicalBucket* cur = *it;
           
    if(max == NULL)
      max = cur;
    else
    {
      if(max->getstartTime() < cur->getstartTime())
        max = cur;
    }                                  
  }
  
  return max;
}

/**
 * Function that, given a set of buckets and a reference bucket returns the next bucket that has the inmediate startime repsect the reference one
 * @param set The set of buckets where to explore
 * @param referenceBucket The reference bucket  
 * @return A reference to the bucket that satisfies the required propierty
 */
AnalogicalBucket* AnalogicalSchedulingPolicy::findNextGreaterStTimeBucket(BucketSetOrdered* set, AnalogicalBucket* referenceBucket)
{

  for(BucketSetOrdered::iterator it = set->begin();it != set->end();++it)
  {
    AnalogicalBucket* cur = *it;
  
    if(referenceBucket->getstartTime() < cur->getstartTime())
    {
      return  cur;
    }
  }

  return NULL;
}

/**
 * Returns the bucket with the minimum starttime in the provided set (it should be changed .. there is no need to look for all the buckets of the queue, just look the last )
 * @param set The set of buckets where to find 
 * @return The bucket with the minimum starttime 
 */
AnalogicalBucket* AnalogicalSchedulingPolicy::findMinimumStartTimeBucket(BucketSetOrdered* set)
{
  AnalogicalBucket* lower = NULL;

  for(BucketSetOrdered::iterator it = set->begin();it != set->end();++it)
  {
    AnalogicalBucket* cur = *it;
           
    if(lower == NULL)
      lower = cur;
    else
    {
      if(lower->getstartTime() > cur->getstartTime())
        lower = cur;
    }                                  
  }
  
  return lower;
}


}
