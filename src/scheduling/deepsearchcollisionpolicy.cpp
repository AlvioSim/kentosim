#include <scheduling/deepsearchcollisionpolicy.h>
#include <scheduling/virtualanalogicalrtable.h>
#include <scheduling/sharedshadow.h>
#include <archdatamodel/node.h>
#include <utils/mnconfiguration.h>
#include <archdatamodel/marenostrum.h>
#include <utils/utilities.h>
#include <scheduling/simulation.h>

#include <list>
#include <algorithm>
#include <math.h>

namespace Simulator{

/**
 * The constructor for the class.
 * @param theValue A reference to the architecture configuration file  
 * @param log A reference to the logging engine
 * @param globaltime The starttime of the simulation
 * @param metrictype The metric that the allocation policy wants to optimize (suitable to disapear)
 * @param statisticType The statistic type that the allocation policy will use for compute the metric to optimize
 */
DeepSearchCollisionPolicy::DeepSearchCollisionPolicy(ArchitectureConfiguration* theValue,Log* log,double globaltime,metric_t metrictype, statistic_t statisticType)
{
  this->architecture = theValue;
  this->log = log;
  this->globaltime = globaltime;
  this->lastAllocationStart = -1; 
  
  /*initializint the reservationTable*/
  this->reservationTable = new VirtualAnalogicalRTable(theValue,log,globaltime);
  ((VirtualAnalogicalRTable*)this->reservationTable)->setMetricType(metrictype);
  ((VirtualAnalogicalRTable*)this->reservationTable)->setStatisticType(statisticType);
  
  //Initialize the vector of cpu mapping job
  for(int i = 0; i < this->architecture->getNumberCpus();i++)
  {
    this->CPUMapping.push_back(0);
  }
  
  this->metricType = metrictype;
  this->statisticType = statisticType;

  /* paraverstuff*/
  this->NumberOfJobs = 0;
  this->paraverTraceNumber = 0;
  
  this->schedulingPolicySimulated = DEEP;

}

/**
 * The default destructor for the class 
 */
DeepSearchCollisionPolicy::~DeepSearchCollisionPolicy()
{
  /* free the reservationTable */
  delete this->reservationTable;
}


/**
 * Adds a given job to the wait queue, usually when it arrives to the system.
 * @see The class SchedulingPolicy 
 * @param job the job to be added to the wait queue
 */
void DeepSearchCollisionPolicy::jobAdd2waitq(Job* job)
{
   //first we call to the upper class that will make some arrangements in the jobs, like updated the job run time  according the jobruntime factor 
  SchedulingPolicy::jobAdd2waitq(job);

  
  //increment the number of jobs currentNumberOfJobs 
  this->NumberOfJobs++;
  this->waitQueue.insert(job);
  
  //information oriented to debug 
  string worstOutcome;  

  bool notAllocated = true;
  
  double starttime;
   
  starttime = globaltime; //we will try to allocate the job from now 
    
  double runtime ;
  if(job->getRequestedTime() == -1)
    runtime = job->getRunTime();
  else
    runtime = job->getRequestedTime();
      
   /* We have to do a gready search to find out the allocation that maximises the current 
      metric we will create all the possible allocations and analise it's impact 
      we will choose the allocation that have the lower overall penalty.
      
      Steeps to do:
      
     
     1- Create all the possible outcomes, taking into account all the jobs that are currently allocated in the 
        Reservation table but have not being allocated. 
   */
   
   Outcomes* outcomes = ComputeAllPossibleOutcomes(&this->waitQueue);
   
   log->debug("There are "+itos(outcomes->size())+" outcomes to be evaluated ..",6);

   VirtualAnalogicalJobAllocation* bestAllocation = NULL;   
   Outcome* bestOutcome = NULL;
   
   int lastEvaluated = 0;
   
   for(Outcomes::iterator it = outcomes->begin();it!= outcomes->end();++it)
   {     
    Outcome* outcome = *it;       
    
    int seqjob = 0;
    string seqjobs = "";
    
    if(log->getlevel() >= 1)
    {
      log->debug("Evaluating the outcome :",3);
                  
      for(Outcome::iterator itj = outcome->begin();itj != outcome->end();++itj)
      {
        Job* outJob = *itj;
      	log->debug("JOB "+itos(outJob->getJobNumber()),3);
      	seqjobs+=itos(outJob->getJobNumber());
      }
           
      seqjob = atoi(seqjobs.c_str());
      lastEvaluated = seqjob;
    }
    
    double worst = -1;    
    
    VirtualAnalogicalJobAllocation* virtualAlloc= this->MakeVirtualAllocation((AnalogicalResertavionTable*)this->reservationTable,outcome,starttime);     
     switch(this->metricType)
     {
       case WAITTIME:
       {
         double waittime =  *((double*)virtualAlloc->getperformance()->getvalue());
         
         if(worst > waittime || bestAllocation == NULL)
         {
            worst = waittime;
            worstOutcome = seqjobs;
         }
         
         if(bestAllocation == NULL || waittime < *((double*)bestAllocation->getperformance()->getvalue()))
         {
           bestOutcome = *it;
           bestAllocation = virtualAlloc;           
         }         
         
         break;
       }
     }
     
     if(bestAllocation != virtualAlloc)
     {
       //not relevant so we delete it ..
       virtualAlloc->freeReservationTable();
       delete virtualAlloc;
     }
   }
  
  string outBestOut = "";
  
  for(Outcome::iterator it = bestOutcome->begin();it != bestOutcome->end();++it)
  {
    Job* job = *it;
    
    outBestOut+= " "+itos(job->getJobNumber());
    
    //based on the result we allocate the jobs.        
    if(bestAllocation->getAllocType(job->getJobNumber()) == FIRST_BYTIME)
      log->debug("The job "+itos(job->getJobNumber())+"has been allocated using the FIRST_BYTIME approach",6);
      
    if(bestAllocation->getAllocType(job->getJobNumber()) == CPUS_CONTINOUS)
      log->debug("The job "+itos(job->getJobNumber())+"has been allocated using the CPUS_CONTINOUS approach",6);
     
    if(bestAllocation->getAllocType(job->getJobNumber()) == LESS_CONSUME)
      log->debug("The job "+itos(job->getJobNumber())+"has been allocated using the LESS_CONSUME approach",6);

      
    /* the allocatin for the job is over here */
    double startTime = bestAllocation->getStartTime(job->getJobNumber());
    double endTime = bestAllocation->getEndTime(job->getJobNumber());
    
    //we set the penallitzed runtime according the reservationTable penalty
    job->setPenalizedRunTime(bestAllocation->getjobPenalty(job->getJobNumber()));
    
    job->setJobSimStartTime(startTime);
    job->setJobSimFinishTime(endTime+job->getPenalizedRunTime());
    job->setJobSimEstimateFinishTime(startTime+job->getRequestedTime()+job->getPenalizedRunTime());
    
    //At this time we must in some way remove event from the simulator that are about this job 
    //and create a start event according to the new scheduling
    log->debug("Now the job with id "+itos(job->getJobNumber())+" will start at "+ftos(job->getJobSimStartTime())+" and will finish at  "+ftos(job->getJobSimStartTime()+job->getRequestedTime()+job->getPenalizedRunTime())+" with a penalty of "+ftos(job->getPenalizedRunTime()),6);
    
    this->sim->deleteJobEvents(job);
    this->sim->forceJobStart(job->getJobSimStartTime(),job->getJobSimStartTime()+job->getRequestedTime()+job->getPenalizedRunTime(),job);     
  }
  
  if(log->getlevel() <=1)
  {
    log->debug("The best current outcome is "+outBestOut,1);
    log->debug("The worst outcome is "+worstOutcome,1);
  }
  
  //we remove the current reservation table and modify it for the current one 
  delete this->reservationTable;
  delete bestAllocation;
  this->reservationTable = bestAllocation->getreservationTable();
    
}

/**
 * Moves a job from teh wait queue to the reservation table, using an specified set of resources
 * @see The class SchedulingPolicy  
 * @param job The job that has started
 */
void DeepSearchCollisionPolicy::jobWaitq2RT(Job* job)
{ 
  /* the job should start now !
     first we find the allocation that is associated to the job and move it to the queue
  */
  this->waitQueue.erase(job);
  this->realFiniQueue.insert(job);
  this->estimateFiniQueue.insert(job);     
}

/**
 * Removes a job from the RT. 
 * @see The class SchedulingPolicy  
 * @param job Job to be removed from the wait queue
 */
void DeepSearchCollisionPolicy::jobRemoveFromRT(Job* job)
{

  //We call to the base class - important it has to update some info   
  SchedulingPolicy::jobRemoveFromRT(job);

  //we decrement the number of jobs ..
  this->NumberOfJobs--;

  AnalogicalJobAllocation* allocation = (AnalogicalJobAllocation*) 
                     ((AnalogicalResertavionTable*)this->reservationTable)->getJobAllocation(job);
  
  /*carrying out the deallocation to the processors */
  for(deque< AnalogicalBucket * >::iterator iter = allocation->allocations.begin();
      iter != allocation->allocations.end(); ++iter)  
  {
    AnalogicalBucket* bucket = *iter;

    /* for sanity  we check that the cpu is not used by any job ..*/
    assert(this->CPUMapping[bucket->getcpu()] != 0);
    
    this->CPUMapping[bucket->getcpu()] = 0;
  }
  
  /* we free the corresponding buckets*/
  ((AnalogicalResertavionTable*)this->reservationTable)->deAllocateJob(job);        
  
  /* we check if we are killing the job or not */
  if(this->globaltime - job->getJobSimStartTime() < job->getRunTime() )
   job->setJobSimisKilled(true);
  
  
}

/**
 * Chooses the more appropiate job to run .
 * @see The class SchedulingPolicy  
 * @return The appropiate job to run 
 */
Job* DeepSearchCollisionPolicy::jobChoose() 
{
  /*As these policy is implementing a FCFS we just can try to choose the head of the wait queue 
    we just check if it's starttime is the current one
  */
  Job* head = this->waitQueue.headJob();
  
  if(this->globaltime >= head->getJobSimStartTime())
  {
    return head;
  }
  else
   return 0;  
}

/**
 * Function that is called once the job should start 
 * @see The class SchedulingPolicy  
 * @param job The job that to start 
 */
void DeepSearchCollisionPolicy::jobStart(Job* job)
{
  
  AnalogicalJobAllocation* allocation = (AnalogicalJobAllocation*) 
                 ((AnalogicalResertavionTable*)this->reservationTable)->getJobAllocation(job);
  
  deque<Bucket*> forParaver;
  
  /*carrying out the allocation to the processors */
  for(deque< AnalogicalBucket * >::iterator iter = allocation->allocations.begin();
      iter != allocation->allocations.end(); ++iter)  
  {
    AnalogicalBucket* bucket = *iter;
            
    /* for sanity  we check that the cpu is not used by any job ..*/
    assert(this->CPUMapping[bucket->getcpu()] == 0);
    
    this->CPUMapping[bucket->getcpu()] = bucket;
    
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
}

/**
 * Given an outcome the function evaluates how it would perfrom in a given virtual allocation. The objective is to check what would change in terms of performance of the global sistem , and how the reservation table would be once all the jobs in the outcomes would be allocated. 
 * @param rtable The virtual reservation Table where the outcome will be evaluated
 * @param toAllocate The outcome containing all the jobs that will allocated 
 * @param time The tim indicating the current time 
 * @return The reservation table orginal plus the allocation of all the jobs in the outcome 
 */
VirtualAnalogicalJobAllocation* DeepSearchCollisionPolicy::MakeVirtualAllocation(AnalogicalResertavionTable* rtable,Outcome* toAllocate,double time)
{
   
  if(toAllocate->begin() == toAllocate->end())
  {  
    //should'nt happend
    assert(false);
  }
  
  VirtualAnalogicalJobAllocation* result  = new VirtualAnalogicalJobAllocation();
  
  //we create a virtual reservation table that will be used for check the effect of the current outcome
  VirtualAnalogicalRTable* table = new VirtualAnalogicalRTable();
       
  //We use the overloaded VirtualAnalogicalRTable "=" operator to copy the table
  //we aware that only the current jobs are being allocated 
  *table =  *((AnalogicalResertavionTable*)rtable);
  
  int Allocatedjob = 0;
  
  log->debug("There are "+itos(toAllocate->size())+ " jobs in the outcome",6);
  
  string headerOutcomeDump ="The outcome evaluated ";
   
  for(Outcome::iterator it = toAllocate->begin();it != toAllocate->end();++it)
  {
       
    Job* job = *it;
   
    headerOutcomeDump+=" "+itos(job->getJobNumber());
   
    bool notAllocated = true;
    double currentTime = time;
      
    while(notAllocated)
    {   
      //we compute the shadows according the current allocation
      table->resetPenalties(NULL,NULL);        
      vector<ShadowSetOrdered*>  shadows = table->ComputeShadows(NULL,NULL,NULL);                
      //we compute the penalties that each shadows add to each job
      table->ComputeShadowPenalties(&shadows); 
      //Now that the penalties have been computed, we must update the free buckets  according the new penalties
      table->updateFreeBuckets();
   	
      /* first we find all the set of buckets that are elegible given the currentTime */
      BucketSetOrdered avaibleBuckets = table->findFirstBucketCpus(currentTime,job->getRequestedTime()); 
    
      /* second we find which of these buckets are suitable for our purposes .. */
      AnalogicalJobAllocation* firstAllocation = table->findFastMethodAllocation(&avaibleBuckets,job->getRequestedTime(),job->getNumberProcessors(),currentTime);
      AnalogicalJobAllocation* continousAllocation = table->findFastConsecutiveMethodAllocation(&avaibleBuckets,job->getRequestedTime(),job->getNumberProcessors(),currentTime);
      AnalogicalJobAllocation* resourceUsageConcient = table->findLessConsumeMethodAllocation(&avaibleBuckets,job->getRequestedTime(),job->getNumberProcessors(),currentTime,job);
      /* now for each of these allocations we cut the buckets for fit the real required time for the job 
        we will use them only for check the possible collisions that will would occur witht exactly this allocation ..
  
  
  
  void resetPenalties(AnalogicalJobAllocation* allocation,VirtualAnalogicalRTable* table); //this function can be used for clear the penalties associated to the buckets.. this is important when evaluating the different possible outcomes..

        remebre that the CutBuckets cut the buckets and create a new buckets ! so we will have 
      */
      AnalogicalJobAllocation* allocationByTime = table->CutBuckets(firstAllocation,job);
      AnalogicalJobAllocation* allocationByCPUS = table->CutBuckets(continousAllocation,job);
      AnalogicalJobAllocation* allocationByReso = table->CutBuckets(resourceUsageConcient,job);
      
      //Now we check which of them has a better performance 
      Metric * metricByTime = table->computeAllocationPenalties(allocationByTime);
      Metric * metricByCpus = table->computeAllocationPenalties(allocationByCPUS);
      Metric * metricByReso = table->computeAllocationPenalties(allocationByReso);
      
      /*we will try to allocate the job the earliest location .. and consecutive cpus */
      if(allocationByCPUS->getallocationProblem() && allocationByTime->getallocationProblem() && allocationByReso->getallocationProblem())
      {
        /* for some reason the job has not been allocated to the , we try to search in the following interval of time */
        BucketSetOrdered::iterator iter = avaibleBuckets.begin();
        AnalogicalBucket* bucket = *iter;
      
        currentTime = bucket->getendTime();
      
      }
      else
      {
        //the job can be allocated .. so we've to return the best metric of the current outcome        
        AnalogicalJobAllocation* choosenAllocation;
        
        if(!allocationByReso->getallocationProblem() && 
            ((allocationByTime->getallocationProblem()&& allocationByCPUS->getallocationProblem()) || metricByReso->betterOrEqualThan(metricByCpus)))
        {
          choosenAllocation = resourceUsageConcient;
          result->setAllocType(job->getJobNumber(),LESS_CONSUME);
        }        
        else if(!allocationByCPUS->getallocationProblem() && (allocationByTime->getallocationProblem() ||     metricByCpus->betterOrEqualThan(metricByTime)))
        {
         //we allocate by contigous allocation
         choosenAllocation = continousAllocation; 
         result->setAllocType(job->getJobNumber(),CPUS_CONTINOUS);         
        }
        else if(!allocationByTime->getallocationProblem() && allocationByCPUS->getallocationProblem())
        {
         //we allocate by time 
         choosenAllocation = firstAllocation;
         result->setAllocType(job->getJobNumber(),FIRST_BYTIME);
        }     
        
        result->setStartTime(job->getJobNumber(),choosenAllocation->getstartTime());
        result->setEndTime(job->getJobNumber(),choosenAllocation->getendTime());
        
        //Now we allocate the job, and copy the penalities from the virtual allocation to the current one ..
        ((AnalogicalResertavionTable*)table)->allocateJob(job,choosenAllocation);        
        
        //now the allocationByCPUs has the real allocated buckets so we must add them to the allBuckets of 
        //all buckets structure
        for(deque<AnalogicalBucket*>::iterator itbuck = choosenAllocation->allocations.begin();
            itbuck != choosenAllocation->allocations.end(); ++itbuck)
        {
           AnalogicalBucket* bucket = *itbuck;
           table->allBuckets->insert(bucket);
        }
        notAllocated = false;
      }
    }
      
    //let's iterate again and try to allocate the next job ..
  }
       
  for(Outcome::iterator it = toAllocate->begin();it != toAllocate->end();++it)
  {
    //Lets update the job penalties for the given allocation
    Job* updateJob = *it;
    result->setJobPenalty(updateJob->getJobNumber(),updateJob->getPenalizedRunTime());
  
  }
  
  table->resetPenalties(NULL,NULL); 
  vector<ShadowSetOrdered*>  shadows = table->ComputeShadows(NULL,NULL,NULL);  
  //we compute the penalties that each shadows add to each job, and update the RTable
  table->ComputeShadowPenalties(&shadows);
  //we compute the metrics associated to the current table 
  result->setPerformance(table->computePerformanceMetric(NULL));
        
  result->setReservationTable(table);

  //we dump the current reservation table in case that the debug is done
  if(this->getgenerateAllParaversRTables())
  { 
    
    string stringMetric = "";
    
    switch(this->metricType)
    {
       case WAITTIME:
       {
         double waittime =  *((double*)result->getperformance()->getvalue());
         stringMetric = ftos(waittime);
         break;
       }
     }
     
   
  
    table->setDumpHeader("The allocation of the for the outcome: "+headerOutcomeDump+". With metric value "+ stringMetric+" . See the following reservation dump:");
    table->setDumpDefaultName(this->paraverOutPutDir+"/"+headerOutcomeDump);
    table->DumpAllocatedBuckets();
  }

  return result;
}


/**
 * Given a set of jobs returns all the possible outcomes (that means all the permutations in the submission order of the jobs scheduled)
 * @param queue The queue containing in FCFS all the jobs to be permuted 
 * @return All the possible outcomes 
 */
Outcomes* DeepSearchCollisionPolicy::ComputeAllPossibleOutcomes(FCFSJobQueue* queue)
{
  Outcomes* allOutcomes = new Outcomes();
  
  FCFSQueue* jq = queue->getQueue();
  //firts we put all the jobs in a list container 
  Outcome* toPermute = new Outcome();
  for(FCFSQueue::iterator itera = jq->begin();itera != jq->end();++itera)
  {
    Job* job = *itera;
    toPermute->push_back(job);
  }
   
  allOutcomes->push_back(toPermute);
  
  Outcome::iterator first = toPermute->begin();
  Outcome::iterator last = toPermute->end();
  
  //now , all the jobs are inserted to the associative container.. so we permute all the elements
  //be AWARE the next_permutation returns all the permutations in lexical descending oreder .. 
  //given the current permutation .. so we must provide the vector ordered in ascendign order if 
  //we wanna get the whole possible permutations ..
  while (next_permutation(first, last,arrivaltime_lt_t()))
  {
    Outcome::iterator it = first;
    
    Outcome* newOutcome = new Outcome();
    
    while (it != last)
    {
      newOutcome->push_back(*it);
      ++it;
    }  

    allOutcomes->push_back(newOutcome);
  }
  
  return allOutcomes;
}





/**
 * If returns true, will mean that each time that a outcome is evaluated, the virtual analogical reservationTable will be dumped to a paraver trace file. 
 * @return A bool indicating if generate or not the paraver traces
 */
bool DeepSearchCollisionPolicy::getgenerateAllParaversRTables() const
{
  return generateAllParaversRTables;
}


/**
 * Sets if generate the paraver intermediate traces during the simulations (if true, be aware coz it may use a lot of disc space )
 * @see getgenerateAllParaversRTables function 
 * @param theValue A bool indicating if generate or not the traces
 */
void DeepSearchCollisionPolicy::setGenerateAllParaversRTables(bool theValue)
{
  generateAllParaversRTables = theValue;
}

/**
 * Function that returns the paraver output dir of the DeepSearchCollision Policy 
 * @return A string containing the path
 */
string DeepSearchCollisionPolicy::getparaverOutPutDir() const
{
  return paraverOutPutDir;
}


/**
 * Sets the dir where the paraver traces will be dump
 * @param theValue The path where to store the files 
 */
void DeepSearchCollisionPolicy::setParaverOutPutDir(const string& theValue)
{
  paraverOutPutDir = theValue;
}

/**
 * Function that returns the paraver trace name dir of the DeepSearchCollision Policy 
 * @return A string containing the path
 */
string DeepSearchCollisionPolicy::getparaverTrace() const
{
  return paraverTrace;
}

/**
 * Sets the dir where the paraver traces will be dump
 * @param theValue The file name prefix where to store the paraver traces file 
 */
void DeepSearchCollisionPolicy::setParaverTrace(const string& theValue)
{
  paraverTrace = theValue;
}

/**
 * Sets the architecture simulated in the scheduling instantiation
 * @param theValue The architecture type
 */
void DeepSearchCollisionPolicy::setArchitectureType(const architecture_type_t& theValue)
{
  architectureType = theValue;
}

/**
 * Returns the architecture type being simulted 
 * @return The architecture type
 */
architecture_type_t DeepSearchCollisionPolicy::getarchitectureType() const
{
  return this->architectureType;
}

/**
 * Returns the jobs that will be simulated 
 * @param theValue 
 */
void DeepSearchCollisionPolicy::setJobList(map< int, Job * >* theValue)
{
  SchedulingPolicy::setJobList(theValue);
  
  ((VirtualAnalogicalRTable*)this->reservationTable)->setJobList(theValue);
}

/**
 * Returns the number of jobs currently allocated in the waitQueue
 * @return 
 */
double DeepSearchCollisionPolicy::getJobsIntheWQ() 
{
  return this->waitQueue.getjobs();

}

/**
 * Return the the amount of pending work (area of requested time x requested processors for the queued jobs)
 * @return A integer containing the pending work
 */
double DeepSearchCollisionPolicy::getLeftWork()
{
  double pending_work = 0;
     
  for(Job* waitjob = this->waitQueue.begin();   
   waitjob != NULL;waitjob = this->waitQueue.next())     
  {
     pending_work+= waitjob->getRequestedTime()*waitjob->getNumberProcessors();
  }
  
  return pending_work;
}
  
/**
* Return the number of jobs in the run queue 
* @return A integer containing the number of running jobs 
*/
double DeepSearchCollisionPolicy::getJobsIntheRQ()
{
  return this->estimateFiniQueue.getjobs();
}

/**
 * Returns the metric that contains the estimation of which would be the performance of a given job if it would be allocated now
 * @param MetricTpye The metric that indicates which performance variable has to be computed 
 * @param job The job that will be checked
 * @return The metric containing the performance that the given job would experiment if it would be submitted right now
 */
Metric* DeepSearchCollisionPolicy::EstimatePerformanceResponse(metric_t MetricTpye,Job* job)
{

}

}

