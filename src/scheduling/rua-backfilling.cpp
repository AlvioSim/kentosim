#include <scheduling/rua-backfilling.h>
#include <scheduling/virtualanalogicalrtable.h>

#include <scheduling/simulation.h>

namespace Simulator {

/**
 * The default constructor for the class
 */
RUABackfilling::RUABackfilling()
{
}

/**
 * The default destructor for the class 
 */
RUABackfilling::~RUABackfilling()
{
}

/**
 * 
 * This is the constructor that should be used, invoques that constructor for the parent classes 
 * @param architecturedef A reference to the architecture configuration file  
 * @param log A reference to the logging engine
 * @param globaltime The starttime of the simulation
 * @param backfillingType The backfillingType that have to be used 
 */
RUABackfilling::RUABackfilling(ArchitectureConfiguration* architecturedef,Log* log,double globaltime,Backfilling_variant_t backfillingType) : EASYWithCollisionSchedulingPolicy(architecturedef,log,globaltime,backfillingType)
{
  //it can be set by the sets and gets .. but by default the 1 1.15 1.25 1.5 
  this->factors = new vector<float>; 
  this->factors->push_back(1);
  this->factors->push_back(1.05);
  this->factors->push_back(1.10);
  this->factors->push_back(1.15);
  this->factors->push_back(1.25);
  this->factors->push_back(1.5);

  Optimize = RESPONSE_TIME;

}



/**
 * Function that makes a reservation for the job
 * @param job The job that have to added to the reservation queue and have to be allocated. The future allocations must not delate the allocation asigned to the given job.
 */
void RUABackfilling::makeJobReservation(Job* job)
{
  // we increment in 1 the current used reservations.
  this->numberOfJobsInTheRT++;


  AnalogicalJobAllocation* allocation = evaluateBetterOutCome(job);

  this->allocateJob(job,allocation);
  // we push the job to the ReservationTableQueue Queue - must be done in this order, the sim start time is assigned in the allocation
  
  //it can occur that we are rescheduling all the jobs in the waitqueu so in this case 
  //we have to check that the job is not already inserted in the WQ 
  if(!this->reservationTableQueue.contains(job))  
    this->reservationTableQueue.insert(job);
    
  // we update the lastStartTimeInRT time in the reserervation table
  this->updateLastStartTimeRT();
  //and finally we create the jobs evenets acording the new allocation 
  this->sim->forceJobStart(job->getJobSimStartTime(),job->getJobSimFinishTime(),job);
}

/**
 * Find an an allocation based on the resource sellection polcy set in the simulation and based on the job requirements. And takes into account the penalties for the jobs (when doing the real allocation and event generation)  
 * @param job The job that would like to be allocated 
 * @param starttime The required start time for the allocation 
 * @param runtime The required time for the allocation 
 * @see The class SchedulingPolicy 
 * @return The allocation that mathces the requirements (if there is no allocation it will be detailed in the allocation problem filed )
 */

AnalogicalJobAllocation* RUABackfilling::findAllocation(Job* job, double starttime,double runtime)
{
 AnalogicalJobAllocation* allocation = evaluateBetterOutCome(job);
 return allocation;

}

/**
 * Function that moves one job from the noReservedJobs to the Reservation table, the number of allowed reservations is decremented   
 * @return The job that has been moved to the reservation queue
 */
Job* RUABackfilling::pushNextJobReservation()
{ 
  Job* first = NULL;
  
  if(this->noReservedJobs->getjobs() > 0)
  {
    //at least there is one job that can be moved on the RTable
    //so we move the head (we follow a FCFS criteria), so we just remove form the noReservedJobs queue and make the reservation
    first = this->noReservedJobs->headJob();
    this->noReservedJobs->erase(first);
    
    //if the job has been backfilled we have to remove from the backfilled jobs structure 
    //and reset it schedule. Probably the new schedule will be similar or even the same that 
    //the current, however , for be fair we reschedule giving the prority to it 
    map<Job*,bool>::iterator backfilled = this->backfilledJobs.find(first);
    if(backfilled != this->backfilledJobs.end())
    {
      this->backfilledJobs.erase(backfilled);
      this->resetJobSchedule(first); 
    }
    else  
      //otherwise from the backfilling queue 
      this->backfillingQueue->erase(first);
    
    //we have to earse the jobs from the map later for avoid containers iterator corruption 
    vector<Job*> toDelete;
    
    //for be fair for the job we must realase all the backfilled jobs and empty the allocations done to them
    for(map<Job*,bool>::iterator it = this->backfilledJobs.begin();it != this->backfilledJobs.end();++it)    
    {
      Job* backfilled = (Job*) it->first;
      toDelete.push_back(backfilled);
      this->backfillingQueue->insert(backfilled);
           
      //we reset all the scheduling stuff 
      this->resetJobSchedule(backfilled);
      backfilled->setJobSimisBackfilled(false);      
    }
    
    //now we safetly free the jobs form the map 
    deleteJobsMap(&toDelete,&this->backfilledJobs);
    
    //we make the reservation for the first job 
    this->makeJobReservation(first);
    
    //now we have to recompute the shadow stuff due to the reservation has changed 
    this->UpdateShadowForBackfill();
    
    //now the ShadowForBackfill indicates a maximum start time for the reserved job 
    first->setJobSimEstimatedShadow(ShadowForBackfill);

    //first we update the backfilling queue according the hints that the different jobs have according the backfilling priority queue of the jobs. Note that this function will evaluate which are the hints of the job to have less penalty
    evaluateBackfillingHints();
    
    //once done the reservation we try to backfill the jobs again (may be de allocation can change)
    for(Job* noReserved = this->backfillingQueue->begin();   
      noReserved != NULL;noReserved = this->backfillingQueue->next())
    {
      if(backfillJob(noReserved))
      {
        log->debug("The job "+itos(noReserved->getJobNumber())+" has been backfilled",6);
        this->sim->forceJobStart(noReserved->getJobSimStartTime(),noReserved->getJobSimFinishTime(),noReserved);
        this->backfillingQueue->deleteCurrent(); /* we earase the job in this way due to we want to avoid modify the container outside the loop, cos then the STL can crash*/
      }
      else
        log->debug("The job "+itos(noReserved->getJobNumber())+" with req rt "+ftos(noReserved->getRequestedTime())+" can not be backfilled",6);
    }
    
  }
  
  return first;
}

/**
 * This function updates the order of the backfilling queue according the current penalties of the jobs, orderd by potential better outcomes for each of the jobs 
 */
void RUABackfilling::evaluateBackfillingHints()
{
    //once done the reservation we try to backfill the jobs again (may be de allocation can change)
    //we have to use the vector of the rua queue due to the old penalized runtime may have changed and this has effects in the 
    //order of the set and may imply a crash of the STL .. be carfull with the strict ordering function !
    for(vector<Job*>::iterator noReservedIt = ((RUAJobQueue*) this->backfillingQueue)->vectJobs.begin();   
        noReservedIt != ((RUAJobQueue*) this->backfillingQueue)->vectJobs.end();
        ++noReservedIt)
    {
      Job* noReserved = *noReservedIt;
      evaluateBetterOutCome(noReserved);     
    }
    //now it's time too update the queue order of the backfilling
    ((RUAJobQueue*) this->backfillingQueue)->updateJobOrders();
}

/**
 * This function will try to find out the best outcome, in terms of allocation, when the job can start. 
 * @param job The job to be evaluated
 * @return The better allocation to the job..
 */
AnalogicalJobAllocation* RUABackfilling::evaluateBetterOutCome(Job* job)
{
  // we set the start time and end time according the allocation algorithm  described in the paper
  //we evaluate all the factors that are set in the configuration 
  float choosenFactor = -1;
  double choosenSLD = -1;
  double choosenResponseTime = -1;
  bool atLeastOne = false;
  
  //we save the current configuration thresshold, as you can see below it will be modified during the evaluation process
  double configuredThreshold = this->thresshold;
 
  for(vector<float>::iterator it = this->factors->begin();it != this->factors->end();++it)
  {
    AnalogicalJobAllocation* allocation = NULL;
 
    this->thresshold = *it;

    //we must do it each time coz we know that perhaps inside the findLessThreshodMethodeAllocation the suitable buckets may be destroied or modified. So we, have to take care about this ..
    BucketSetOrdered suitablebuckets = 
                  ((AnalogicalResertavionTable*)this->reservationTable)->findFirstBucketCpus(this->globaltime,job->getRequestedTime());
         

    if(canSatisfyThresholdRestriction(job,this->thresshold))
    {
       allocation = ((VirtualAnalogicalRTable*)this->reservationTable)->findLessThresholdConsumeMethodAllocation(&suitablebuckets,
                                                                  job->getRequestedTime(),
                                                                  job->getNumberProcessors(),
                                                                  this->globaltime,
                                                                  job,
                                                                  this->thresshold
                                                                  );

       if(allocation->getallocationProblem())
       {
         delete allocation;
         continue;
       }
       

       //we compute the slowdown according the submission time of the job , the penalized runtime for the job according the allocation, the start time of the allocation 
       double estWaittime = this->globaltime-job->getJobSimSubmitTime(); //the current waittime for the job
       assert(estWaittime >= 0);
       estWaittime+= allocation->getstartTime()-this->globaltime; //estimated 
       assert(estWaittime >= 0);
       double estRuntime = job->getRequestedTime()+job->getOldPenalizedRunTime();
 
      
      if(this->Optimize == SLD)
      {
         double jobSld = (estRuntime+estWaittime)/estRuntime;
      

         if(choosenSLD == -1 || choosenSLD > jobSld)
         {
           choosenFactor = this->thresshold;
           choosenSLD = jobSld;
           atLeastOne = true;
       
         }
         
         delete allocation;
      }
      
      if(this->Optimize == RESPONSE_TIME)
      {
         double responseTime = (estRuntime+estWaittime);
      

         if(choosenResponseTime == -1 || choosenResponseTime > responseTime)
         {
           choosenFactor = this->thresshold;
           choosenResponseTime = responseTime;
           atLeastOne = true;
         }
      
         delete allocation;
      }
    }
    
  }


  AnalogicalJobAllocation* choosenAllocation = NULL;

  //if no allocation has been found that satisfies any of the thresholds specified in the RUA algorithm we try to find out the allocation followig the default resource selection policy
  if(!atLeastOne)
  {    
    choosenAllocation = EASYWithCollisionSchedulingPolicy::findAllocation(job,globaltime,job->getRequestedTime());
  } 
  else
  {
    //otherwise , we know that at least one factor can produce the "more appropiate" job allocation for the current job, so, we follow again the procedure of generating the allocation. Note that we must find again de suitable buckets since the references  to other Analogical bucekts that we could store in an old allocatiom may be destroied when evaluating other possible allocations
    BucketSetOrdered suitablebuckets = 
                  ((AnalogicalResertavionTable*)this->reservationTable)->findFirstBucketCpus(this->globaltime,job->getRequestedTime());

    this->thresshold = choosenFactor;

    choosenAllocation = ((VirtualAnalogicalRTable*)this->reservationTable)->findLessThresholdConsumeMethodAllocation(&suitablebuckets,
                                                                  job->getRequestedTime(),
                                                                  job->getNumberProcessors(),
                                                                  this->globaltime,
                                                                  job,
                                                                  this->thresshold
                                                                  );

   
  }


  //can not be null.. any of the two preceeding methods must return a allocation..
  assert(choosenAllocation != NULL); 

  //we restore the threshold set in the RSP configuration 
  this->thresshold = configuredThreshold;

  return choosenAllocation;
}

/**
 * Reevaluates the current planned scheduling, this is mainly done when a deadline missed is triggered
 */
void RUABackfilling::rescheduleAllQueuedJobs()
{

  // Will allocate all the jobs in the ReservationTableQueue, Find the new allocation, create the new start and finishd events.
  // will try to backfill all the rest of the jobs as done in 1. (suggestion create a backfillFunction that will iterate over the 1 for all the jobs in the  noReservedJobs). For do this stuff we will invoque the job arrival like if the jobs would arrive 
  for(Job* reserved = this->reservationTableQueue.begin();   
      reserved != NULL;reserved = this->reservationTableQueue.next())
  {
    //first we mark as we do not really had 
    numberOfJobsInTheRT--;
    this->makeJobReservation(reserved);
    
    //the current start time must be, at minimum least than the previous one  ! 
    /*
            IMPORTANT ! DUE TO THE FIND SUITABLE BUCKETS FUNCTION FOR SOME JOBS WITH VERY LARGE NUMBER OF CPUS 
            CAN OCCUR THAT THIS CAN NOT BE SATISFIED - IN THE CASE THAT FOR THE CPU i THERE ARE MORE THAN 1 BUCKETS
            CAN OCCUR THAT THE FIND SUITABLE CPUS CHOSES THE BUCKET 1 WHILE IT DONES NOT SHARE THE REQUIRED AMOUNT OF
            TIME WITH THE BUCKETS OF THE OTHER CPUS BUCKETS - NEXT VERSIONS OF THIS FUNCIONS MUST RETURN 
            ALL THE BUCKETS THAT SATISFIES THAT SHARES ALL THE REQUIRED 
    */
    assert(reserved->getJobSimStartTime() <= reserved->getJobSimLastSimStartTime() || true);
  }  
  
  
  //first we update the backfilling queue according the hints that the different jobs have according the backfilling priority queue of the jobs. Note that this function will evaluate which are the hints of the job to have less penalty
  evaluateBackfillingHints();
    
  //once done the reservation we try to backfill the jobs again (may be de allocation can change)
  for(Job* noReserved = this->backfillingQueue->begin();   
    noReserved != NULL;noReserved = this->backfillingQueue->next())
  {
    if(backfillJob(noReserved))
    {
      log->debug("The job "+itos(noReserved->getJobNumber())+" has been backfilled",6);
      this->sim->forceJobStart(noReserved->getJobSimStartTime(),noReserved->getJobSimFinishTime(),noReserved);
      this->backfillingQueue->deleteCurrent(); /* we earase the job in this way due to we want to avoid modify the container outside the loop, cos then the STL can crash*/
    }
    else
      log->debug("The job "+itos(noReserved->getJobNumber())+" with req rt "+ftos(noReserved->getRequestedTime())+" can not be backfilled",6);
  }
  
}

/**
 * Returns the current metric that has to be optimized by the rua policy .. actually only the sld and response time are considered
 * @return An metric_t containing the metric that is currently optimized.
 */
metric_t RUABackfilling::getOptimize() const
{
	return Optimize;
}



/**
 * Sets the current metric that has to be optimized by the rua policy
 * @param theValue The metric (currently the sld and response time)
 */
void RUABackfilling::setOptimize ( const metric_t& theValue )
{
	Optimize = theValue;
}

}

