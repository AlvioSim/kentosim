#include <scheduling/easywithcollisionschedulingpolicy.h>
#include <scheduling/powerawarebackfilling.h>

#include <scheduling/simulation.h>

namespace Simulator {

/**
 * The default constructor for the class
 */
PowerAwareBackfilling::PowerAwareBackfilling()
{  
}


/**
 * The default destructor for the class 
 */

PowerAwareBackfilling::~PowerAwareBackfilling()
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
PowerAwareBackfilling::PowerAwareBackfilling(ArchitectureConfiguration* architecturedef,Log* log,double globaltime,Backfilling_variant_t backfillingType) : EASYSchedulingPolicy(architecturedef,log,globaltime,backfillingType)
{
  //we will free the reservation table instantiated by the EASY backfilling scheduling policy used at the upper level 
  //and set up the reservation table that takes into account the collisions between the jobs 
  this->reservationTable = new VirtualAnalogicalRTable(architecturedef,log,globaltime);
  ((VirtualAnalogicalRTable*)this->reservationTable)->setFreeBucketsOnJobCompletion(false);
  
  this->tryToBackfillAllways = false;
  this->thresshold = -1;
}

/**
 * Allocates the job to the provided allocation. The starttime and endtime reals are done according the collions and penalities of the jobs.
 * @param job The job to allocated 
 * @param allocation The allocation for the job 
 */
void PowerAwareBackfilling::allocateJob(Job* job, AnalogicalJobAllocation* allocation)
{
  assert(allocation->getendTime()-allocation->getstartTime() > 0);
  /*We create an start time and end time according the allocation.*/    
  ((AnalogicalResertavionTable*)this->reservationTable)->allocateJob(job,allocation);


  job->setJobSimStartTime(allocation->getstartTime());  
  job->setJobSimEstimateFinishTime(allocation->getstartTime()+job->getRequestedTime());
  
  
  /* we will base our decision about kill or not the job depending on the original penality computed at given moment
     and we will only update such value in case that the penalty is higer than the original 
   */   
    
  if(job->getRunTime()+job->getPenalizedRunTimeReal()>this->maxAllowedRuntime && this->maxAllowedRuntime != -1)
      //killed for the queue limit 
      job->setJobSimFinishTime(allocation->getstartTime()+this->maxAllowedRuntime);
  else if(allocation->getstartTime()+job->getRunTime()+job->getPenalizedRunTimeReal() < 
     allocation->getendTime())
     //here no problem ! we only have to set the finish time for the job - its realtime plus the penalty 
     job->setJobSimFinishTime(allocation->getstartTime()+job->getRunTime()+job->getPenalizedRunTimeReal());
  else
     job->setJobSimFinishTime(allocation->getendTime());

 
  // PF we can update all penalized times of all jobs regarded new job allocated in reservation tablA
  ((VirtualAnalogicalRTable*)this->reservationTable)->resetPenalties(NULL, NULL); 
  vector<ShadowSetOrdered*>  shadows = ((VirtualAnalogicalRTable*)this->reservationTable)->ComputeShadows(NULL,NULL,NULL);  
  ((VirtualAnalogicalRTable*)this->reservationTable)->ComputeShadowPenalties(&shadows);
  ((VirtualAnalogicalRTable*)this->reservationTable)->deleteShadows(&shadows);
    
  //now that the job has been allocated with the selected allocation we must have to be sure that all the 
  //jobs in the system really finish acording their penalized time. 
  updateTerminationEvents(job);
}


/**
 * Function that updates, if needed, the finish time for all the jobs according their job penalies 
 * @param scheduledJob The job that currently is being allocated (events have not be generated twice)
 */
void PowerAwareBackfilling::updateTerminationEvents(Job* scheduledJob)
{
  //return;

  for(map<Job*,bool>::iterator jobit = this->JobInTheSystem.begin(); jobit != this->JobInTheSystem.end();++jobit)
  {
     /* for each job we must check that the event of job termination is */
     Job* job = jobit->first;

     
     /* we get the information regarding the allocation for the job  - and check that the real time for the allocation has not changed*/
     AnalogicalJobAllocation* allocation = (AnalogicalJobAllocation*) 
                     ((AnalogicalResertavionTable*)this->reservationTable)->getJobAllocation(job);
                     
     if(job->getJobSimStartTime() == -1)
       //the job has still not set the start time, may be in the WQ but not Reserved ? , anyway we do not update anything 
       continue;
     
     //here we have to check if due to new collisions the real runtime for the job has been reduced 
     //coz in this case we have to force the finish time earlier, 
     double oldPenalizedRealRuntime = job->getPenalizedRunTimeReal();
     
     if(job->getRunTime() < job->getRequestedTime())
     {
       //we will use the same allocation returned , but modifying its end time, due to this we will store it 
       double ReqRTFinishTime = allocation->getendTime();
   
       allocation->setEndTime(allocation->getstartTime()+job->getRunTime());
       AnalogicalJobAllocation* allocationForCheckingRT = ((VirtualAnalogicalRTable*)this->reservationTable)->CutBuckets(allocation,job);
   
       //we restore the old runtime 
       allocation->setEndTime(ReqRTFinishTime);
   
       //we compute the penality for this jobs according the realtime allocation 
       ((VirtualAnalogicalRTable*)this->reservationTable)->resetPenalties(NULL,NULL); 
       vector<ShadowSetOrdered*>  shadows = ((VirtualAnalogicalRTable*)this->reservationTable)->ComputeShadows(allocationForCheckingRT,job,NULL);  
       //we compute the penalties that each shadows add to each job, and update the RTable
       ((VirtualAnalogicalRTable*)this->reservationTable)->ComputeShadowPenalties(&shadows);   
       ((VirtualAnalogicalRTable*)this->reservationTable)->deleteShadows(&shadows);
   
       freeAllocation(allocationForCheckingRT);
       //and set this value to the job 
       job->setPenalizedRunTimeReal(job->getPenalizedRunTime());
   
     }
     
     //in case that the penalized runtime 
     if(oldPenalizedRealRuntime != job->getPenalizedRunTimeReal())
     {
        log->debug("Rescheduling the finish time for the job "+itos(job->getJobNumber()));
        //probably the current allocation has modified the penalities of some jobs, so we must have to update their finish time 
        job->setJobSimStartTime(job->getJobSimStartTime());
        job->setJobSimFinishTime(job->getJobSimStartTime()+job->getPenalizedRunTimeReal()+job->getRunTime());
        
        this->sim->deleteJobFinishEvent(job);
        
        //in case that the job is the once that is being scheduled their events will be generated later. 
        if(scheduledJob != job)
          this->sim->forceJobFinish(job->getJobSimStartTime()+job->getPenalizedRunTimeReal()+job->getRunTime(),job);
     }
  }
}


/**
 * Release the resources used by the job. 
 * @param job The job that has to be realeased 
 */
void PowerAwareBackfilling::releaseJobResources(Job* job)
{
 
  ((VirtualAnalogicalRTable*)this->reservationTable)->resetPenalties(NULL, NULL); 
  vector<ShadowSetOrdered*>  shadows = ((VirtualAnalogicalRTable*)this->reservationTable)->ComputeShadows(NULL,NULL,NULL);  
  //we compute the penalties that each shadows add to each job, and update the RTable
  ((VirtualAnalogicalRTable*)this->reservationTable)->ComputeShadowPenalties(&shadows);
  ((VirtualAnalogicalRTable*)this->reservationTable)->deleteShadows(&shadows);

  EASYSchedulingPolicy::releaseJobResources(job);
}

/**
 * This function checks wether the condiction of the threashold for the given job can be satisfied or not ..
 * @param job  The job to be checked 
 * @return True if the threshold could be satisfied
 */
bool PowerAwareBackfilling::canSatisfyThresholdRestriction(Job* job,double thresshold)
{
  //this formula has been desinged for the memory BW model, please re-consider it when taking into account other more complex models, such as memory bandwithd etc. On the other hand this function is considered by a homogeneous architecture, note that for a heterogenous systems this formula is not valid !


  MareNostrum* mn = ((MNConfiguration*) this->architecture)->getmarenostrum();

  Node* node = mn->getNodeWithGlobalId(0);
  double BWPerNode = node->getMemoryBW();
  int cpusNode = node->getNCPUS();

  double memreq = job->getBWMemoryUsed();

  //now we check, if at least one processor per node could be allocated (this is the worst case..)
  //but at least the threshold can be respected
  int numberProcessorsPerNode = (int)(job->getNumberProcessors() / this->architecture->getNumberNodes());
  
  //its not possible to allocate half process to one node and the other to the other :-)
  if(job->getNumberProcessors() % this->architecture->getNumberNodes() > 0)
    numberProcessorsPerNode++;

  double maxJobDemandPerNode = numberProcessorsPerNode*memreq;

  if(maxJobDemandPerNode / BWPerNode >= thresshold)
    return false;
  else 
    return true;
}

/**
 * Find an an allocation based on the resource sellection polcy set in the simulation and based on the job requirements. And takes into account the penalties for the jobs (when doing the real allocation and event generation)  
 * @param job The job that would like to be allocated 
 * @param starttime The required start time for the allocation 
 * @param runtime The required time for the allocation 
 * @see The class SchedulingPolicy 
 * @return The allocation that mathces the requirements (if there is no allocation it will be detailed in the allocation problem filed )
 */

AnalogicalJobAllocation* PowerAwareBackfilling::findAllocation(Job* job, double starttime,double runtime)
{
 double nextStarttime = starttime;
 double maxSldPenaltyThreshold = 1.5;   
  
 
 AnalogicalJobAllocation* allocation = NULL;
 bool allocated = false;
 bool firstLook = true;

 //in some situations, can occur that all the buckets are representing until the interval -1.. what means that no one is allocated after this bucket, in this situations, following the previous findMaximumStarttimeBucket we always will reach the same next starttime. In this situations we use and index, that goes from the bucket with less sarttime until the last once, incremeneting the nextstarttime to this 
 int indexAllInfinite = 0;
 bool rightLimitFound = false;
 deque<AnalogicalBucket*> suitableStartTimes;
 int delays = 0; //this value indicates how many times we had to look ahead for a further allocation
  
 while(!allocated) 
 {

   //if the righ limit situation is not found we can go ahead
   BucketSetOrdered suitablebuckets;

   if(!rightLimitFound)
     suitablebuckets = ((AnalogicalResertavionTable*)this->reservationTable)->findFirstBucketCpus(starttime,runtime);
    

   AnalogicalBucket* nextToExplore;
   //before continue going ahead .. we must keep a copy of the next startime where to start, i do this mainly due to the suitable buckets may be destroyed or change due to the resource selection policy, basically the less consume stuff. That adds and delete processes in the rtable for evaluate the penalty.
   if(!rightLimitFound)
   {
     nextToExplore = findMaximumStartTimeBucket(&suitablebuckets);
     nextStarttime = nextToExplore->getstartTime();

     //we reach the left limit situation.     
     if(nextStarttime == starttime)
     {
       rightLimitFound = true;
       //we initialize the vector of start times                
       for(BucketSetOrdered::iterator erit = suitablebuckets.begin();erit != suitablebuckets.end();++erit)
       {
         AnalogicalBucket* possibleST = *erit;
         log->debug("Startime for the next possible bucket is: "+ftos(possibleST->getstartTime()),6);
         if(possibleST->getstartTime() > starttime)
         {           
           suitableStartTimes.push_back(possibleST);
           indexAllInfinite++;
         }
       }
     }
   }

   //we are in the left limit situation
   if(rightLimitFound)
   {     
     indexAllInfinite++;
     nextToExplore = suitableStartTimes.front();
     suitableStartTimes.pop_front();
   }


   if(nextToExplore == NULL)
   {
     log->error("Something went wrong, the job "+itos(job->getJobNumber())+" requires "+itos(job->getNumberProcessors())+" processors. And the suitables are:"+itos(suitablebuckets.size())+ " the maximum it startime is "+ftos(nextToExplore->getstartTime()));                    
           
     for(BucketSetOrdered::iterator erit = suitablebuckets.begin();erit != suitablebuckets.end();++erit)
     {
       AnalogicalBucket* erb = *erit;
       log->error("Bucket cpu "+itos(erb->getcpu())+" from "+ftos(erb->getstartTime())+" till "+ftos(erb->getendTime()));         
     }
     
     assert(false);
   }
      


   //if the first loop we try to find out what's up with the first fit allocation, this stuff is only for compare
   //the difference of a FF outcome and using the other resource selection policy specified 
   //if its not the first time mean that it has some problems ... the previous time
   if(firstLook)
   {
     AnalogicalJobAllocation* allocationByTime = ((AnalogicalResertavionTable*)this->reservationTable)->findFastMethodAllocation(&suitablebuckets,
                                                                  runtime,
                                                                  job->getNumberProcessors(),
                                                                  starttime
                                                                 );
     job->setJobSimEstimatedSTWithFF(allocationByTime->getstartTime());

     
     firstLook = false || allocationByTime->getallocationProblem();
   }
    
   assert(suitablebuckets.size() != 0); //should not occur 
   
   job->setAllocatedWith(this->getRS_policyUsed());
   
   if(this->getRS_policyUsed() == FIND_LESS_CONSUME)
     allocation = ((VirtualAnalogicalRTable*)this->reservationTable)->findLessConsumeMethodAllocation(&suitablebuckets,
                                                                  runtime,
                                                                  job->getNumberProcessors(),
                                                                  starttime,
                                                                  job
                                                                  );
   
   else if(this->getRS_policyUsed() == EQUI_DISTRIBUTE_CONSUME)
     allocation = ((VirtualAnalogicalRTable*)this->reservationTable)->findDistributeConsumeMethodAllocation(&suitablebuckets,
                                                                  runtime,
                                                                  job->getNumberProcessors(),
                                                                  starttime,job);
   
   else if(this->getRS_policyUsed() == FIND_LESS_CONSUME_THRESSHOLD)
   {
     //first we check wether the condition of the thresshold can be satisfied, 
     if(canSatisfyThresholdRestriction(job,this->thresshold))
       allocation = ((VirtualAnalogicalRTable*)this->reservationTable)->findLessThresholdConsumeMethodAllocation(&suitablebuckets,
                                                                  runtime,
                                                                  job->getNumberProcessors(),
                                                                  starttime,
                                                                  job,
                                                                  this->thresshold
                                                                  );
     else
     //in the contrary case the less consume is called
     {
        //for expirence we have stated that sometimes the less consume in this cituations where the threshold can not be satisifed is not usefull. We have empirically determinined that given thresholds behaves even better, normally 1.25 it's a good limit.
        if(canSatisfyThresholdRestriction(job,this->alternativeThreshold))
          allocation = ((VirtualAnalogicalRTable*)this->reservationTable)->findLessThresholdConsumeMethodAllocation(&suitablebuckets,
                                                                  runtime,
                                                                  job->getNumberProcessors(),
                                                                  starttime,
                                                                  job,
                                                                  this->alternativeThreshold
                                                                  );
        else
        
         allocation = ((VirtualAnalogicalRTable*)this->reservationTable)->findLessConsumeMethodAllocation(&suitablebuckets,
                                                                  runtime,
                                                                  job->getNumberProcessors(),
                                                                    starttime,job);
     }
   }
   else
     allocation = EASYSchedulingPolicy::findAllocation(job,starttime,job->getRequestedTime());

   /* if an allocation problem has occurred .. we remove the allocation */ 
   if(allocation->getallocationProblem())
   {
      log->debug("The allocation has a problem .. may be not enough processors.",6);
      delete allocation;
      allocation = NULL; 
      starttime = nextStarttime;  
   }
   else
     allocated = true;

   delays++;

 }
 
 
 //here we have to estimate the penalized runtime for the job in case that the real rt is lower than the requested, 
 //coz for the finish time for the job we must use the Runtime and its penalty. In the other case, when the ReqRT is >  than the RT we do not do anything due to the job will be killed. 

 AnalogicalJobAllocation* allocationForCheckingReq = ((VirtualAnalogicalRTable*)this->reservationTable)->CutBuckets(allocation,job);

 if(job->getRunTime() < job->getRequestedTime())
 {   

   //we will use the same allocation returned , but modifying its end time, due to this we will store it 
   double ReqRTFinishTime = allocation->getendTime();
   
   allocation->setEndTime(allocation->getstartTime()+job->getRunTime());
   AnalogicalJobAllocation* allocationForCheckingRT = ((VirtualAnalogicalRTable*)this->reservationTable)->CutBuckets(allocation,job);
   
   //we restore the old runtime 
   allocation->setEndTime(ReqRTFinishTime);
   
   //we compute the penality for this jobs according the realtime allocation 
   Metric * metricByTime = ((VirtualAnalogicalRTable*)this->reservationTable)->computeAllocationPenalties(allocationForCheckingRT); 
   
   delete metricByTime;
   freeAllocation(allocationForCheckingRT);

   //and set this value to the job 
   job->setPenalizedRunTimeReal(job->getPenalizedRunTime());
   
 }
 else
 { 
   Metric * metricByTime = ((VirtualAnalogicalRTable*)this->reservationTable)->computeAllocationPenalties(allocationForCheckingReq);
 
   //in case job has been overstimated or perfectly estimated we set real penalized time according the Requested time
   //it will be killed. //although the job will be killed, in case that the requested rt > real rt we set the RT penalty with the Req RT requested 
   job->setPenalizedRunTimeReal(job->getPenalizedRunTime());
    
   /* we must free all the used memory that won't be used later */
   delete metricByTime;
   freeAllocation(allocationForCheckingReq);
 }  

 assert(allocation->getendTime()-allocation->getstartTime() > 0);  

 return allocation;

}

/**
 * Deletes all the buckets used by a given allocation, it is an auxiliar function.
 * @param allocationForCheckingReq  The allocation to free.
 */
void PowerAwareBackfilling::freeAllocation(AnalogicalJobAllocation* allocationForCheckingReq)
{
 ((AnalogicalResertavionTable*)this->reservationTable)->freeAllocation(allocationForCheckingReq);
}

/**
 * Sets the list of jobs that will scheduled 
 * @param theValue The list of jobs 
 */
void PowerAwareBackfilling::setJobList(map< int, Job * >* theValue)
{
  SchedulingPolicy::setJobList(theValue);
  ((VirtualAnalogicalRTable*)this->reservationTable)->setJobList(theValue);
}

/**
 * Moves a job from teh wait queue to the reservation table, using an specified set of resources
 * IF ANY CHANGE - PLEASE CHECK THE EASY WITH COLLISIONS SCHEDULING POLICY 
 * @see The class SchedulingPolicy  
 * @param job The job that has started
 */
void PowerAwareBackfilling::jobRemoveFromRT(Job* job) //Removes a job from the RT
{

  //We call to the base class - important it has to update some info   
  SchedulingPolicy::jobRemoveFromRT(job);

  /* IF ANY CHANGE - PLEASE CHECK THE EASY SCHEDULING POLICY */

  /* we get the information regarding the allocation for the job */
  AnalogicalJobAllocation* allocation = (AnalogicalJobAllocation*) 
                     ((AnalogicalResertavionTable*)this->reservationTable)->getJobAllocation(job);
  
  bool finishedEarlier = allocation->getendTime()-allocation->getstartTime() > job->getRunTime()+job->getPenalizedRunTimeReal();
  
  bool killedBackfilling = allocation->getendTime()-allocation->getstartTime() < job->getRunTime()+job->getPenalizedRunTimeReal();
  bool killedQLimit = job->getRunTime()>this->maxAllowedRuntime+job->getPenalizedRunTimeReal()  && this->maxAllowedRuntime != -1;;
  
  
   log->debug("The job "+itos(job->getJobNumber())+" has finished. It started at "+ftos(job->getJobSimStartTime())+" and finished at "+ftos(job->getJobSimFinishTime())+" its rt is "+ftos(job->getRunTime())+" it requested was "+ ftos(job->getRequestedTime())+" allocation size "+ftos(allocation->getendTime()-allocation->getstartTime()),2);
  
  //First we will mark the job as killed or completed, this is done before release the resources 
  //due to some other policies (like EASY with collisions ) that are using the Reservation Virtual Table
  //may decide to do not free the buckets for the finished jobs due to this buckets may 
  //afect the computation for the penalty for other still running jobs 
  //If the job has reached its deadlinet but has not finished we mark it as killed.
  if(killedBackfilling)
  {            
    assert(job->getRequestedTime() < job->getRunTime()+job->getRunTime()+job->getPenalizedRunTimeReal());
    log->debug("The job "+itos(job->getJobNumber())+" has been killed for the backfilling algo .",2); 
    job->setJobSimisKilled(true);
    job->setJobSimStatus(KILLED_BACKFILLING);
  }
  else if(killedQLimit)
  {    
    log->debug("The job "+itos(job->getJobNumber())+" has been killed for the queue limit.",2); 
    job->setJobSimisKilled(true);
    job->setJobSimStatus(KILLED_QUEUE_LIMIT);  
  }
  else 
  {
    //we set the status that the job had in the original system 
    job->setJobSimStatus(job->getStatus());
  }
  
  
  //We compute it here since the allocation will be released in later 
  double allocSize = allocation->getendTime()-allocation->getstartTime();
  if(finishedEarlier)
  {
    /*
    Before any thing due to the nature of the reservation table
    as we know that the job has finished earlier, we must update the reservation table !
    remember that the reservation table collision is consious about all the jobs collsions and  
    that it stores all the jobs (either the finished jobs) till any of the non completed allocations is 
    not afected by them.
    
    */
    
    double time2Reduce = job->getRequestedTime()-(job->getRunTime()+job->getPenalizedRunTimeReal());
    
    ((AnalogicalResertavionTable*)this->reservationTable)->reduceRuntime(job,time2Reduce);
  }
  /*First we relaese the resources used by the job */ 
  releaseJobResources(job);
  //remove it from the running queue 
  this->RunningQueue.erase(job);
  //we update the ExtraForBackfill and ShadowForBackfill 
  this->UpdateShadowForBackfill();
    
  /*If the job has finished earlier than expected we should*/
  if(finishedEarlier)
  {
    log->debug("The job "+itos(job->getJobNumber())+" has finished earlier than expected. Rescheduling all the jobs",2);
    log->debug(ftos(job->getRunTime())+"<"+ftos(allocSize),2);
    //We must call the function that reschedules called deadLineMissed that:
    if(this->numberOfJobsInTheRT != 0)
      //some scheduling arrangements may be can be carried out 
      this->JobOverstimated(job);
    else
      //we have to check that no running jobs are being afected by this JobOverstimated
      this->updateTerminationEvents(NULL);
  }
  
  
}


/**
 * the threshold to be used in case of lessconsuemthreshold
 * @return  A double containing the threshols
 */
double PowerAwareBackfilling::getthresshold() const
{
	return thresshold;
}



/**
 * Sets the threshold to be used in case of lessconsuemthreshold
 * @param theValue The threshold
 */
void PowerAwareBackfilling::setThresshold ( double theValue )
{

	thresshold = theValue;
}

/**
 * the alternative threshold to be used in case of lessconsuemthreshold and in those cases that the base threshold can not be satisfied
 * @return  A double containing the alternative threshold
 */
double PowerAwareBackfilling::getalternativeThreshold() const
{
	return alternativeThreshold;
}


/**
 * Sets the alternative threshold to be used in case of lessconsuemthreshold and in those cases that the base threshold can not be satisfied
 * @param theValue The threshold
 */
void PowerAwareBackfilling::setAlternativeThreshold ( double theValue )
{
	alternativeThreshold = theValue;
}


}
