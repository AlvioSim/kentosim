#include <scheduling/easyschedulingpolicy.h>
#include <scheduling/simulation.h>

#include <algorithm>
#include <math.h>

#include <utils/fcfsjobqueue.h>
#include <utils/estimatedruntimejobqueue.h>
#include <utils/lxwfjobqueue.h>
#include <utils/ruajobqueue.h>


namespace Simulator {

/**
 * The default constructor for the class
 */

EASYSchedulingPolicy::EASYSchedulingPolicy() : AnalogicalSchedulingPolicy()
{
  this->numberOfReservations = -1;
  this->numberOfJobsInTheRT = 0;
  
  this->architecture = 0;
  this->log = 0;
  this->globaltime = 0;
  this->lastStartTimeInRT = 0;
  this->ExtraForBackfill = 0;
  this->ShadowForBackfill = 0;
  this->schedulingPolicySimulated = EASY;
  this->Agressive = true;
}


/**
 * 
 * This is the constructor that should be used, invoques that constructor for the parent classes 
 * @param theValue A reference to the architecture configuration file  
 * @param log A reference to the logging engine
 * @param globaltime The starttime of the simulation
 * @param backfillingType The backfillingType that have to be used 
 */
EASYSchedulingPolicy::EASYSchedulingPolicy(ArchitectureConfiguration* theValue,Log* log,double globaltime,Backfilling_variant_t backfillingType) : AnalogicalSchedulingPolicy()
{
  this->numberOfReservations = 1;
  this->numberOfJobsInTheRT = 0;
  
  this->architecture = theValue;
  this->log = log;
  this->globaltime = globaltime;
  this->lastStartTimeInRT = globaltime;
  this->tryToBackfillAllways = true;
  this->ExtraForBackfill = this->architecture->getNumberCpus();
  this->ShadowForBackfill = globaltime;
   
  /*initializint the reservationTable*/
  this->reservationTable = new AnalogicalResertavionTable(theValue,log,globaltime);
  
  /*we create the queues according the backfilling variant*/
  switch(backfillingType)
  {
    case EASY_BACKFILLING:
       this->backfillingQueue = new FCFSJobQueue();
       this->noReservedJobs = new FCFSJobQueue();
       this->waitQueue = new FCFSJobQueue();
       break;
    
    case SJF_BACKFILLING:
       this->backfillingQueue = new EstimatedRunTimeJobQueue();
       this->noReservedJobs = new FCFSJobQueue();
       this->waitQueue = new FCFSJobQueue();
       break;
    
    case LXWF_BACKFILLING:
       this->backfillingQueue = new LXWFJobQueue();
       this->noReservedJobs = new FCFSJobQueue();
       this->waitQueue = new FCFSJobQueue();
       break;
    case RUA_BACKFILLING:
       this->backfillingQueue = new RUAJobQueue();
       this->noReservedJobs = new FCFSJobQueue();
       this->waitQueue = new FCFSJobQueue();
       break;
  }
  
  this->realBackfilledJobs = 0;
  this->allocateAllJobs = false; //by default all the jobs that do not belong to the reservation and that can not be backfilled are not allocated to the reservation table. 
  
  this->Agressive = true;
  
}


/**
 * The default destructor for the class 
 */
EASYSchedulingPolicy::~EASYSchedulingPolicy()
{
  delete this->reservationTable;
}

/**
 * This is a void function that updates the ExtraForBackfill and ShadowForBackfill for backfill variables, see their definition. Usign the alogirithm used in the sim2give simulator this  function is used for compute the interval of time that the jobs can be backfilled or not. The shadow, althoug may be the job won't be able to be backfilled (depends on the Resource Selection Policy used), provides some hint about we can try or not. This make sense when the tryToBackfillAllways is true  
 */
void EASYSchedulingPolicy::UpdateShadowForBackfill()
{
  int availableCPUS =  this->architecture->getNumberCpus() - this->reservationTable->getNumberCPUSUsed() ;
  
  // 2- update shadow & extra
  this->ShadowForBackfill = this->globaltime;
  this->ExtraForBackfill = availableCPUS;    
  
  double RequiredCPUSForReservations = 0;

  for(Job* reserved = this->reservationTableQueue.begin();reserved != NULL; reserved = this->reservationTableQueue.next())
  {
    RequiredCPUSForReservations+= reserved->getNumberProcessors();  
  }

  assert(RequiredCPUSForReservations >= 0);

  for(Job* runningJob = this->RunningQueue.begin(); runningJob != NULL;
      runningJob = this->RunningQueue.next())
  {
      int as = 3;
  }
  
  for(Job* runningJob = this->RunningQueue.begin(); runningJob != NULL && this->ExtraForBackfill < RequiredCPUSForReservations;
      runningJob = this->RunningQueue.next())
  {
    assert(this->ShadowForBackfill <= runningJob->getJobSimEstimateFinishTime());
    this->ShadowForBackfill = runningJob->getJobSimEstimateFinishTime(); 
    this->ExtraForBackfill+= runningJob->getNumberProcessors();
  
  }
 
  this->ExtraForBackfill-= RequiredCPUSForReservations;

  assert(this->ExtraForBackfill >= 0);
  //never can occurs that the number of free cpus is higer than the available cpus (this assert by construction never will occur, however we keep it here for future remains :-)

}



 
 /**
 * This function is triggered when the job has been over . In this case the reservation table has to be recomputed. We should reschedule all the jobs due to may be the current schedule is not as good as expected. 
 * @param job The job that has reached the deadline 
 */
void EASYSchedulingPolicy::JobOverstimated(Job* job)
{
    //this function triggers when the job has been overstimated  or understimated
    //in the EASY variant we only consider that the job has been overstimated, otherwise
    //it would be killed, so we just call the reevaluateScheduling function
    this->resetSchedulingAllQueuedJobs();
    this->rescheduleAllQueuedJobs();

}



/**
 * Function that reset all the scheduling. It deallocates all the jobs that are allocated in the reservation table (but are not runnig)
 */
void EASYSchedulingPolicy::resetSchedulingAllQueuedJobs()
{
  //the reservation table has to be recomputed.
  //We should reschedule all the jobs due to may be the current schedule is not as good as expected    
  //For all the jobs in the backfilledJobs:
  vector<Job*> toDelete;
  
  for(map<Job*,bool>::iterator it = this->backfilledJobs.begin();it != this->backfilledJobs.end();++it)    
  {
    Job* backfilled = (Job*) it->first;
    toDelete.push_back(backfilled);
    //we reset all the scheduling stuff 
    this->resetJobSchedule(backfilled);
    this->backfillingQueue->insert(backfilled);
  }
  
  deleteJobsMap(&toDelete,&this->backfilledJobs);
  
  //For all the jobs in the ReservationTableQueue:
  for(Job* reserved = this->reservationTableQueue.begin();   
      reserved != NULL;reserved = this->reservationTableQueue.next())
  {
    //   We must remove it from the ReservationTable.      
    //   We must delete all the events according to this job.
    this->resetJobSchedule(reserved);
  }
}

/**
 * Auxiliar function that safetly deletes a set jobs from the MapContainer. It allows deleting jobs safely from a hashmap without corrupting the container iterator
 * @param toDelete Jobs to delete 
 * @param MapContainer A reference of the hashmap where to delete.
 */
void EASYSchedulingPolicy::deleteJobsMap(vector<Job*>* toDelete,map<Job*,bool>* MapContainer)
{
  //we must do in this way due to deleteing the job from the container in the interation haas 
  //a wrong behaviour due to the stl does no allow to do it 
  for(vector<Job*>::iterator itDel = toDelete->begin();itDel != toDelete->end();++itDel)
  {
    Job* toDel = *itDel;
    
    map<Job*,bool>::iterator it = MapContainer->find(toDel);
    assert(it != MapContainer->end());
    MapContainer->erase(it);
  }

}

/**
 * Reevaluates the current planned scheduling, this is mainly done when a deadline missed is triggered
 */
void EASYSchedulingPolicy::rescheduleAllQueuedJobs()
{

  // Will allocate all the jobs in the ReservationTableQueue, Find the new allocation, create the new start and finishd events.
  // will try to backfill all the rest of the jobs as done in 1. (suggestion create a backfillFunction that will iterate over the 1 for all the jobs in the  noReservedJobs). For do this stuff we will invoque the job arrival like if the jobs would arrive 
  for(Job* reserved = this->reservationTableQueue.begin();   
      reserved != NULL;reserved = this->reservationTableQueue.next())
  {
    //first we mark as we do not really had 
    numberOfJobsInTheRT--;
    this->makeJobReservation(reserved);
    
    //the current start time must be, at minimum lest than the previous one  ! 
    /*
            IMPORTANT ! DUE TO THE FIND SUITABLE BUCKETS FUNCTION FOR SOME JOBS WITH VERY LARGE NUMBER OF CPUS 
            CAN OCCUR THAT THIS CAN NOT BE SATISFIED - IN THE CASE THAT FOR THE CPU i THERE ARE MORE THAN 1 BUCKETS
            CAN OCCUR THAT THE FIND SUITABLE CPUS CHOSES THE BUCKET 1 WHILE IT DONES NOT SHARE THE REQUIRED AMOUNT OF
            TIME WITH THE BUCKETS OF THE OTHER CPUS BUCKETS - NEXT VERSIONS OF THIS FUNCIONS MUST RETURN 
            ALL THE BUCKETS THAT SATISFIES THAT SHARES ALL THE REQUIRED 
    */
    assert(reserved->getJobSimStartTime() <= reserved->getJobSimLastSimStartTime() || true);
  }  
  
  //we must make a copy due to the backfillingQueue will be modified inside the jobAdd2waitq and the container of 
  //the backfillingQueue remains unestable     
  vector<Job*> toReschedule;
  for(Job* noReserved = this->backfillingQueue->begin();   
      noReserved != NULL;noReserved = this->backfillingQueue->next())
  {
    toReschedule.push_back(noReserved);
  }
  
  //now we can safetly iterater over the vector 
  for(vector<Job*>::iterator it = toReschedule.begin(); it != toReschedule.end();++it)
  {
    Job* noReserved = *it;
  
    
    
    this->jobAdd2waitq(noReserved);
    
    if(noReserved->getJobSimStartTime() != -1)
      this->sim->forceJobStart(noReserved->getJobSimStartTime(),noReserved->getJobSimFinishTime(),noReserved);
  }
  
}

/**
 * Function that reevaluates all the scheduling of the jobs, that means deallocating all the jobs of the reservation table and backfilled jobs and allocate them again. This function is called mainly on deadline missed.
 */
void EASYSchedulingPolicy::reevaluateScheduling()
{
  this->resetSchedulingAllQueuedJobs();
  this->rescheduleAllQueuedJobs();
}


/**
 * Function that checks is the job has some hint to be backfilled according the current 
 * @param job The jobs that has to be checked if can be backfilled or not.
 * @return 
 */
bool EASYSchedulingPolicy::HaveHintToBeBackfilled(Job* job)
{
 int availableCPUS =  this->architecture->getNumberCpus() - this->reservationTable->getNumberCPUSUsed();    
 double minEstimateTerm = this->globaltime+job->getRequestedTime();
 
 bool result = job->getNumberProcessors() <= availableCPUS &&  /* enough cpus */
               (minEstimateTerm < this->ShadowForBackfill || /* will finish earlier than the reservation jobs */
                job->getNumberProcessors() < this->ExtraForBackfill /* can use the free processors that won't be used by the jobs till the end */
               );
               
 log->debug("Job ->"+itos(job->getJobNumber())+"; Job minEstimateTerm-> "+ftos(minEstimateTerm)+"; Job required processors-> "+itos(job->getNumberProcessors())+"; availableCPUS-->"+itos(availableCPUS)+"; ExtraForBackfill-> "+ftos(ExtraForBackfill)+"; ShadowForBackfill-> "+ftos(ShadowForBackfill),6);
 
             
 return result;
}

/**
 * Function that tries to backfill the given job 
 * @param job The job to be backfilled 
 * @return 
 */
bool EASYSchedulingPolicy::backfillJob(Job* job)
{
  //if the job is already backfilled we simply return
  map<Job*,bool>::iterator backfilled = this->backfilledJobs.find(job);
  if(backfilled != this->backfilledJobs.end())
    return false;
     
  bool HaveBackfillingHint = HaveHintToBeBackfilled(job);
 
  /* we use the Shadow and extra for have some hint while the job will be able to be allocated or not */
  if(!tryToBackfillAllways && !HaveBackfillingHint)
  {            
    return false;
  }
 
  /* we backfill the job if the start time for the allocation is less than the last start time (lastStartTimeInRT) of all the jobs in the reservation table.*/
 
  double starttime = globaltime; //we will try to allocate the job from now  

  if ( this->powerAware )
  {
    // try to allocate job with powerAware
    vector<double>::iterator it= architecture->Frequencies.begin();
    while (it!=architecture->Frequencies.end())
    {
      AnalogicalJobAllocation* allocation = findAllocation(job,starttime,ceil(job->getRequestedTime()*power->getScalingRatio(job->getJobNumber(),*it)));
  
      /* in case the job can be allocated we do it*/
      if(!allocation->getallocationProblem() && (allocation->getstartTime() <= this->lastStartTimeInRT))
      {    
        job->setRunsAtFrequency(*it);  /* it is important to set it here before allocateJob  */
     
        if (*it < power->gettopFrequency()) 
          job->setRunsAtReducedFrequency(true);  

        this->allocateJob(job,allocation);        
    
        this->backfilledJobs.insert(pairJobBool(job,true));       
        this->log->debug("The job "+itos(job->getJobNumber())+" is backfilled",5); 
        job->setJobSimisBackfilled(true);
        job->setJobSimBackfillingTime(job->getJobSimStartTime());
    
        assert(HaveBackfillingHint);
    
        return true;
      }

      delete allocation;
      it++;
    }

  }
  else {
    // try to backfill job without powerAware
    AnalogicalJobAllocation* allocation = findAllocation(job,starttime,job->getRequestedTime());

    if(!allocation->getallocationProblem() && (allocation->getstartTime() <= this->lastStartTimeInRT))
    {    
      this->allocateJob(job,allocation);        
    
      this->backfilledJobs.insert(pairJobBool(job,true));       
      this->log->debug("The job "+itos(job->getJobNumber())+" is backfilled",5); 
      job->setJobSimisBackfilled(true);
      job->setJobSimBackfillingTime(job->getJobSimStartTime());
    
      assert(HaveBackfillingHint);
    
      return true;
    }
    else
    {
      //since the allocation won't be used any more we delete it 
      log->debug("Job can not be backfilled.the allocation was at "+ftos(allocation->getstartTime())+" and the lastStartTimeInRT at "+ftos(this->lastStartTimeInRT),6);
      delete allocation;
    }

  }
 
  log->debug("Job can not be backfilled "+ftos(job->getJobNumber()),6);
  return false;
  
}

/**
 * Allocates the job to the provided allocation 
 * @param job The job to allocated 
 * @param allocation The allocation for the job 
 */
void EASYSchedulingPolicy::allocateJob(Job* job, AnalogicalJobAllocation* allocation)
{      
  /*We create an start time and end time according the allocation.*/    
  ((AnalogicalResertavionTable*)this->reservationTable)->allocateJob(job,allocation);
    
  job->setJobSimStartTime(allocation->getstartTime());
    
    
    /* in case that the job runtime is less than the maxAllowedRuntime by the current architecture the job will be killed , but not for backfilling alogirithm, it will be killed due to the overload of the queue/architecture limit */
        
  if ( (this->powerAware) && (job->getRunsAtReducedFrequency()) ) 
  {
    if(ceil(job->getRunTime()*power->getScalingRatio(job->getJobNumber(),job->getRunsAtFrequency()))>this->maxAllowedRuntime && this->maxAllowedRuntime != -1)
      job->setJobSimFinishTime(allocation->getstartTime()+this->maxAllowedRuntime);
    else if(job->getRunTime() < job->getRequestedTime())
      /*in the case that the job has been overstimated we have to set the finish time to its real time */        
      job->setJobSimFinishTime(allocation->getstartTime()+ceil(job->getRunTime()*power->getScalingRatio(job->getJobNumber(),job->getRunsAtFrequency())));
    else
      job->setJobSimFinishTime(allocation->getendTime());
      
    job->setJobSimEstimateFinishTime(allocation->getstartTime()+ceil(job->getRequestedTime()*power->getScalingRatio(job->getJobNumber(),job->getRunsAtFrequency())));
  }
  else
  {
   if(job->getRunTime()>this->maxAllowedRuntime && this->maxAllowedRuntime != -1)
     job->setJobSimFinishTime(allocation->getstartTime()+this->maxAllowedRuntime);
   else if(job->getRunTime() < job->getRequestedTime())
     /* in the case that the job has been overstimated we have to set the finish time to its real time */        
     job->setJobSimFinishTime(allocation->getstartTime()+job->getRunTime());
   else
     job->setJobSimFinishTime(allocation->getendTime());
      
   job->setJobSimEstimateFinishTime(allocation->getstartTime()+job->getRequestedTime());
  }
}


/**
 * Function that pdates the last start time in the reservation table
 */
void EASYSchedulingPolicy::updateLastStartTimeRT()
{
  Job* lastStart = this->reservationTableQueue.backJob();
  if(lastStart != NULL)
    this->lastStartTimeInRT = lastStart->getJobSimStartTime();
   
}

/**
 * Adds a given job to the wait queue, usually when it arrives to the system.
 * @see The class SchedulingPolicy 
 * @param job the job to be added to the wait queue
 */
void EASYSchedulingPolicy::jobAdd2waitq(Job* job) //Adds a given job to the wait queue, usually when it arrives to the system
{
  log->debug("There are "+itos(this->waitQueue->getjobs())+" in th WQ - "+itos(backfillingQueue->getjobs())+" in the BingQ - "+itos(backfilledJobs.size())+" in the BedQ - "+itos(this->reservationTable->getNumberCPUSUsed())+" used processors",6);

  //first we call to the upper class that will make some arrangements in the jobs, like updated the job run time  according the jobruntime factor 
  SchedulingPolicy::jobAdd2waitq(job);   

  job->setRunsAtReducedFrequency(false);
  /*
2- If not:
  2.1- If the number of reservations is less than the maximum allowed by the current policy:
      + We allocate the job to the current reservation table. */
  if(numberOfJobsInTheRT < numberOfReservations)      
  {
    makeJobReservation(job);    
  }
  else
  {
    // Otherwise, right now we don't care when this job will be able to run. So:
    // We push the job to the noReservedJobs Queue
    
    //it can occur that we are rescheduling all the jobs in the waitqueu so in this case 
    //we have to check that the job is not already inserted in the WQ  (see JobOverstimated)
    if(!this->noReservedJobs->contains(job))
    {          
      this->noReservedJobs->insert(job); 
      this->backfillingQueue->insert(job);
    }
    else
      assert(this->backfillingQueue->contains(job)); //should not be already backfilled
    
    //we only will try to backfill the job if there are enough free cpus for running it ..
    //otherwise, we know for sure that won't be able to start earlier than the reserved job 
    //due to there are no enough resources free 
    int availableCPUS =  this->architecture->getNumberCpus() - this->reservationTable->getNumberCPUSUsed() ;
    bool enoughResources = availableCPUS >= job->getNumberProcessors();    
     
     
    if(backfillJob(job))
    {
      log->debug("The job "+itos(job->getJobNumber())+" has been backfilled",6);   
      this->backfillingQueue->erase(job);
    }
    else
    {              
      job->setJobSimStartTime(-1); //we do not know when it will start 
      job->setJobSimFinishTime(-1);
     
    }
    
  }
  
  //it can occur that we are rescheduling all the jobs in the waitqueu so in this case 
  //we have to check that the job is not already inserted in the WQ 
  if(!this->waitQueue->contains(job))
    this->waitQueue->insert(job);
    
  this->JobInTheSystem.insert(pairJobBool(job,true));
}


/**
 * Function that makes a reservation for the job
 * @param job The job that have to added to the reservation queue and have to be allocated. The future allocations must not delate the allocation asigned to the given job.
 */
void EASYSchedulingPolicy::makeJobReservation(Job* job)
{
  // we increment in 1 the current used reservations.
  this->numberOfJobsInTheRT++;

  job->setRunsAtReducedFrequency(false);

  // we set the start time and end time according the current allocation.
  AnalogicalJobAllocation* allocation = this->findAllocation(job,globaltime,job->getRequestedTime());
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
 * Moves a job from teh wait queue to the reservation table, using an specified set of resources
 * @see The class SchedulingPolicy  
 * @param job The job that has started
 */
 
 void EASYSchedulingPolicy::jobWaitq2RT(Job* job) //Moves a job from teh wait queue to the reservation table, using an specified set of resources
{    
  //before move promote the jobs that are currently waitting for the resources we must move the job to the processors 
  AnalogicalJobAllocation* allocation = (AnalogicalJobAllocation*) 
                 ((AnalogicalResertavionTable*)this->reservationTable)->getJobAllocation(job);
  
  deque<Bucket*> forParaver;
    
  /*carrying out the allocation to the processors */
  for(deque< AnalogicalBucket * >::iterator iter = allocation->allocations.begin();
        iter != allocation->allocations.end(); ++iter)  
  {
    AnalogicalBucket* bucket = *iter;
           
    /* for sanity  we check that the cpu is not used by any job ..*/
    AnalogicalBucket* slot = ((AnalogicalResertavionTable*)this->reservationTable)->CPUMapping[bucket->getcpu()]; 
    assert(slot == 0);
    
    ((AnalogicalResertavionTable*)this->reservationTable)->CPUMapping[bucket->getcpu()] = bucket;
    
      /*if the paraver trace is not null we notify for such allocation */
    if(this->prvTrace != 0)
    {
        forParaver.push_back((Bucket*) bucket);     
    }
   }  
    
  if(this->useParaver)
  {

    this->prvTrace->ParaverJobRuns(job->getJobNumber(),forParaver); 
  }
  
  if(job->getJobSimisBackfilled())
   this->realBackfilledJobs++;
   
  job->setJobSimStatus(RUNNING);

  //finally we insert it to the runque 
  this->RunningQueue.insert(job);
  
  map<Job*,bool>::iterator it = this->backfilledJobs.find(job);
  
  if(it != this->backfilledJobs.end())
  {
    this->backfilledJobs.erase(it);
    this->noReservedJobs->erase(job);
  }
  else
  {  
    this->reservationTableQueue.erase(job);    
    this->numberOfJobsInTheRT--;
          
    Job* reserved = NULL;
  
    if((reserved = pushNextJobReservation()) != NULL)
      log->debug("The job "+itos(reserved->getJobNumber())+" has been moved to a reservation",6); 
  }
  this->waitQueue->erase(job);
}



/**
 * Function that reset the scheduling information for the job.
 * @param job The job to be reset.
 */
void EASYSchedulingPolicy::resetJobSchedule(Job* job)
{
  //we remove it from the RTable
  this->reservationTable->deAllocateJob(job);        
  //We must delete all the events according to this job.
  this->sim->deleteJobEvents(job);
  //we reset its start times and ends times
  job->setJobSimStartTime(-1);
  job->setJobSimFinishTime(-1);
  job->setJobSimEstimateFinishTime(-1);

}


/**
 * Moves a job from teh wait queue to the reservation table, using an specified set of resources
 * IF ANY CHANGE - PLEASE CHECK THE EASY WITH COLLISIONS SCHEDULING POLICY 
 * @see The class SchedulingPolicy  
 * @param job The job that has started
 */
void EASYSchedulingPolicy::jobRemoveFromRT(Job* job) //Removes a job from the RT
{
  bool finishedEarlier, killedBackfilling, killedQLimit;

  //We call to the base class - important it has to update some info   
  SchedulingPolicy::jobRemoveFromRT(job);

  /* we get the information regarding the allocation for the job */
  AnalogicalJobAllocation* allocation = (AnalogicalJobAllocation*) 
                     ((AnalogicalResertavionTable*)this->reservationTable)->getJobAllocation(job);
  
  if ( (this->powerAware) && (job->getRunsAtReducedFrequency()) ) 
  {
    finishedEarlier = allocation->getendTime()-allocation->getstartTime() > ceil(job->getRunTime()*power->getScalingRatio(job->getJobNumber(),job->getRunsAtFrequency()));
    killedBackfilling = allocation->getendTime()-allocation->getstartTime() < ceil(job->getRunTime()*power->getScalingRatio(job->getJobNumber(),job->getRunsAtFrequency()));
    killedQLimit =  ceil(job->getRunTime()*power->getScalingRatio(job->getJobNumber(),job->getRunsAtFrequency()))>this->maxAllowedRuntime && this->maxAllowedRuntime != -1;
  }
  else
  {
    finishedEarlier = allocation->getendTime()-allocation->getstartTime() > job->getRunTime();
    killedBackfilling = allocation->getendTime()-allocation->getstartTime() < job->getRunTime();
    killedQLimit =  job->getRunTime()>this->maxAllowedRuntime && this->maxAllowedRuntime != -1;
  }
  
   log->debug("The job "+itos(job->getJobNumber())+" has finished. It started at "+ftos(job->getJobSimStartTime())+" and finished at "+ftos(job->getJobSimFinishTime())+" its rt is "+ftos(job->getRunTime())+" it requested was "+ ftos(job->getRequestedTime())+" allocation size "+ftos(allocation->getendTime()-allocation->getstartTime()),6);
  
  if(killedBackfilling)
  { 
    assert(job->getRequestedTime() < job->getRunTime());
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
  
  double allocationLength = allocation->getendTime()-allocation->getstartTime();
  
  /*First we relaese the resources used by the job */
  releaseJobResources(job);
  //remove it from the running queue 
  this->RunningQueue.erase(job);
  //we update the ExtraForBackfill and ShadowForBackfill 
  this->UpdateShadowForBackfill();
  
  
  /*If the job has finished earlier than expected we should*/
  if(finishedEarlier)
  {
    log->debug("The job "+itos(job->getJobNumber())+" has finished earlier than expected. Rescheduling all the jobs",6);
    log->debug(ftos(job->getRunTime())+"<"+ftos(allocationLength),6);
    //We must call the function that reschedules called deadLineMissed that:
    this->JobOverstimated(job);
  } 
    
}

/**
 * Function that moves one job from the noReservedJobs to the Reservation table, the number of allowed reservations is decremented   
 * @return The job that has been moved to the reservation queue
 */
Job* EASYSchedulingPolicy::pushNextJobReservation()
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
 * Chooses the more appropiate job to run .
 * @see The class SchedulingPolicy  
 * @return The appropiate job to run 
 */
Job* EASYSchedulingPolicy::jobChoose()
{
 
} //Chooses the more appropiate job to run 

/**
 * Function that is called once the job should start 
 * @see The class SchedulingPolicy  
 * @param job The job that to start 
 */
void EASYSchedulingPolicy::jobStart(Job* job) //Starts a job    
{
  //we update the ShadowForBackfill and ExtraForBackfill 
  this->UpdateShadowForBackfill();
}


/**
 * Return the number of reservations used 
 * @return A integer with the number of reservations 
 */
int EASYSchedulingPolicy::getnumberOfReservations() const
{
  return numberOfReservations;
}


/**
 * Sets the number of reservations used in the algorithm
 * @param theValue Number of reservation 
 */
void EASYSchedulingPolicy::setNumberOfReservations(const int& theValue)
{
  numberOfReservations = theValue;
}


/**
 * Returns the number of jobs that are currently backfilled 
 * @return A integer with the number of backfilled jobs 
 */
double  EASYSchedulingPolicy::getBackfilledJobs()
{
  return this->realBackfilledJobs;
}

/**
 * Return the the amount of pending work (area of requested time x requested processors for the queued jobs)
 * @return A integer containing the pending work
 */
double EASYSchedulingPolicy::getLeftWork()
{
  double pending_work = 0;
     
  for(Job* waitjob = this->waitQueue->begin();   
   waitjob != NULL;waitjob = this->waitQueue->next())     
  {
     pending_work+= waitjob->getRequestedTime()*waitjob->getNumberProcessors();
  }
  
  return pending_work;
}
  
/**
* Return the number of jobs in the run queue 
* @return A integer containing the number of running jobs 
*/
double EASYSchedulingPolicy::getJobsIntheRQ()
{
  return this->RunningQueue.getjobs();
}

/**
 * Returns the metric that contains the estimation of which would be the performance of a given job if it would be allocated now
 * @param MetricTpye The metric that indicates which performance variable has to be computed 
 * @param job The job that will be checked
 * @return The metric containing the performance that the given job would experiment if it would be submitted right now
 */
Metric* EASYSchedulingPolicy::EstimatePerformanceResponse(metric_t MetricType,Job* job)
{
  //first we check if it's not a common metric, if true we return this value, otherwise we go ahead with the estimation 
  Metric* metric = SchedulingPolicy::EstimatePerformanceResponse(MetricType,job);

  if(metric != NULL)
    return metric;

  //we estimate when the job would start   
  AnalogicalJobAllocation* allocation = findAllocation(job,globaltime,ceil(job->getRequestedTime()*this->EmulateCPUFactor));
  double wouldStart = allocation->getstartTime();
  double waitTime = wouldStart-globaltime;
  double RT = job->getRequestedTime();
  metric = new Metric();
  
  //since the allocation won't be used any more we delete it 
  delete allocation;
  
  log->debug("The job "+itos(job->getJobNumber())+" would start at "+ftos(wouldStart)+" and arrives at "+ftos(globaltime)+ " in the EASY",4);
  
  switch(MetricType)
  {
   case SLD:
   {
     metric->setNativeDouble(RT/(RT+waitTime));
     break;
   }
   case BSLD:
   {     
     metric->setNativeDouble(RT/(max(RT,(double)BSLD_THRESHOLD)+waitTime)); 
   }   
   case WAITTIME:
   {     
     metric->setNativeDouble(waitTime);
     break;
   }  
   case BACKFILLED_JOB:
   {     
     metric->setNativeBool(wouldStart < this->lastStartTimeInRT);
     break;
   }     
   case BACKFILLED_JOBS:
   {     
     metric->setNativeDouble(this->realBackfilledJobs);
     break;
   }        
   case JOBS_IN_WQUEUE:
   {        
     metric->setNativeDouble(this->getJobsIntheWQ());
     break;
   }      
   case SUBMITED_JOBS:
   {     
     metric->setNativeDouble(this->submittedJobs);
     break;
   }             
   case LEAST_WORK_LEFT:
   {  double pending_work = 0;
     
     for(Job* waitjob = this->waitQueue->begin();   
      waitjob != NULL;waitjob = this->waitQueue->next())     
     {
       pending_work+= waitjob->getRequestedTime()*waitjob->getNumberProcessors();
     }
     
     metric->setNativeDouble(pending_work);
     
     break;
   }
   default:
     assert(false); 
  }
  
  metric->setType(MetricType);
  
  return metric;
}


/**
 * Return true if all the jobs have to be allocated in the RT (it is mainly used in situations where a clear picture of how is all scheduled is needed, for instance for predictions, but it has an extra cost)
 * @return A bool indicating if the all the jobs have to be allocated 
 */
bool EASYSchedulingPolicy::getallocateAllJobs() const
{
  return allocateAllJobs;
}


/**
 * Sets if all the jobs have to be allocated. 
 * @param theValue True if all the jobs have to be allocated in the RT.
 */
void EASYSchedulingPolicy::setAllocateAllJobs(bool theValue)
{
  allocateAllJobs = theValue;
}


/**
 * Returns if the jobs will be backfilled despite they won't start now
 * @return A bool indicating if jobs will be backfilled or not 
 */
bool EASYSchedulingPolicy::gettryToBackfillAllways() const
{
  return tryToBackfillAllways;
}


/**
 Sets if all the jobs have to be allocated if they can be backfilled although they won't start run now. It can occurs that a job would start earlier than the once that are in the reservation table, but it will not start inmediately. In some policies is needed to know the approximate backfilling time .. However it is exprensive ! 
 * @param theValue True if all the jobs have to be backfilled 
 */
void EASYSchedulingPolicy::setTryToBackfillAllways(bool theValue)
{
  tryToBackfillAllways = theValue;
}


}
