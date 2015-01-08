#include <scheduling/fcfsschedulingpolicy.h>
#include <scheduling/analogicaljoballocation.h>
#include <scheduling/simulation.h>

#include <math.h>

namespace Simulator {
/**
 * The default constructor for the class
 */

FCFSSchedulingPolicy::FCFSSchedulingPolicy()
{
}

/**  
 * This is the constructor that should be used, invoques that constructor for the parent classes 
 * @param theValue A reference to the architecture configuration file  
 * @param log A reference to the logging engine
 * @param globaltime The starttime of the simulation 
 */
FCFSSchedulingPolicy::FCFSSchedulingPolicy(ArchitectureConfiguration* theValue,Log* log,double globaltime) : AnalogicalSchedulingPolicy()
{
  this->architecture = theValue;
  this->log = log;
  this->globaltime = globaltime;
   
  /*initializint the reservationTable*/
  this->reservationTable = new AnalogicalResertavionTable(theValue,log,globaltime);
  this->lastAllowedStartTime = 0;
  this->schedulingPolicySimulated = FCFS;
     
}

/**
 * The default destructor for the class 
 */

FCFSSchedulingPolicy::~FCFSSchedulingPolicy()
{
  /* free the reservationTable */
  delete this->reservationTable;
}

/**
 * Adds a given job to the wait queue, usually when it arrives to the system.
 * @see The class SchedulingPolicy 
 * @param job the job to be added to the wait queue
 */

void FCFSSchedulingPolicy::jobAdd2waitq(Job* job)
{      
  //first we call to the upper class that will make some arrangements in the jobs, like updated the job run time  according the jobruntime factor 
  SchedulingPolicy::jobAdd2waitq(job);
  
  
  //otherwise to the queue
  this->waitQueue.insert(job);   

  double starttime = globaltime; //we will try to allocate the job from now         
  AnalogicalJobAllocation* allocation = findAllocation(job,this->lastAllowedStartTime,job->getRunTime());
  
  
  /* should not be any problem */
  if(!allocation->getallocationProblem())
  {
    this->allocateJob(job,allocation);
    this->log->debug("The job "+itos(job->getJobNumber())+" can start at "+ftos(allocation->getstartTime()),2); 
    
    //we update the last allowed start time to the start time of this job 
    this->lastAllowedStartTime = allocation->getstartTime();
  } 
  else
    assert(false);
    
}

/**
 * Moves a job from the wait queue to the reservation table, using an specified set of resources
 * @see The class SchedulingPolicy  
 * @param job The job that has started
 */
 
void FCFSSchedulingPolicy::jobWaitq2RT(Job* job)
{ 
  /* the job should start now !
     first we find the allocation that is associated to the job and move it to the queue
  */  
  this->realFiniQueue.insert(job);
  /* as this is a FCFS policy the job is allocated definitively in the queue, so we can delete it from the wait queue */
  this->waitQueue.erase(job);
 
}

/**
 * Moves a job from teh wait queue to the reservation table, using an specified set of resources
 * @see The class SchedulingPolicy  
 * @param job The job that has started
 */

void FCFSSchedulingPolicy::jobRemoveFromRT(Job* job)
{
  //We call to the base class - important it has to update some info   
  SchedulingPolicy::jobRemoveFromRT(job);

  AnalogicalJobAllocation* allocation = (AnalogicalJobAllocation*) 
                     ((AnalogicalResertavionTable*)this->reservationTable)->getJobAllocation(job);
  
  deque<Bucket*> forParaver;

  /*carrying out the deallocation to the processors 
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
  
  /* the job has finished so we just set the status */
  job->setJobSimStatus(COMPLETED);
  
}


/**
 * Allocates the job to the provided allocation 
 * @param job The job to allocated 
 * @param allocation The allocation for the job 
 */
void  FCFSSchedulingPolicy::allocateJob(Job* job, AnalogicalJobAllocation* allocation)
{    
    double runtime = job->getRunTime();
        
    /*We create an start time and end time according the allocation.*/    
    ((AnalogicalResertavionTable*)this->reservationTable)->allocateJob(job,allocation);
    job->setJobSimStartTime(allocation->getstartTime());
    job->setJobSimFinishTime(allocation->getendTime());
    job->setJobSimEstimateFinishTime(allocation->getstartTime()+runtime);

}

/**
 * Chooses the more appropiate job to run .
 * @see The class SchedulingPolicy  
 * @return The appropiate job to run 
 */
Job* FCFSSchedulingPolicy::jobChoose() 
{
  //in this policy should'nt be called
  assert(false);
}

/**
 * Function that is called once the job should start 
 * @see The class SchedulingPolicy  
 * @param job The job that to start 
 */
void FCFSSchedulingPolicy::jobStart(Job* job)
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
    assert(((AnalogicalResertavionTable*)this->reservationTable)->CPUMapping[bucket->getcpu()] == 0);
    
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
  
}

/**
 * Returns the number of jobs currently allocated in the waitQueue
 * @return 
 */

double FCFSSchedulingPolicy::getJobsIntheWQ() 
{
  return this->waitQueue.getjobs();

}

/**
 * Return the the amount of pending work (area of requested time x requested processors for the queued jobs)
 * @return A integer containing the pending work
 */
double FCFSSchedulingPolicy::getLeftWork()
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
double FCFSSchedulingPolicy::getJobsIntheRQ()
{
  return this->realFiniQueue.getjobs();
}

/**
 * Returns the metric that contains the estimation of which would be the performance of a given job if it would be allocated now
 * @param MetricTpye The metric that indicates which performance variable has to be computed 
 * @param job The job that will be checked
 * @return The metric containing the performance that the given job would experiment if it would be submitted right now
 */

Metric* FCFSSchedulingPolicy::EstimatePerformanceResponse(metric_t MetricType,Job* job)
{
  //we estimate when the job would start 
  //ALSO USED IN THE EASY SCHEDULING .. IN CACSE OF MODIFICATION PLEASE UPDTATE THE easy STUFF

  AnalogicalJobAllocation* allocation = findAllocation(job,this->lastAllowedStartTime,ceil(job->getRunTime()*this->EmulateCPUFactor));
  double wouldStart = allocation->getstartTime();
  double waitTime = wouldStart-globaltime;
  double RT = job->getRequestedTime();
  Metric* metric = new Metric();
  
  //since the allocation won't be used any more we delete it 
  delete allocation;
  
  log->debug("The job "+itos(job->getJobNumber())+" would start at "+ftos(wouldStart)+" and arrives at "+ftos(globaltime)+ " in the FCFS",2);
  
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
     
     for(Job* waitjob = this->waitQueue.begin();   
      waitjob != NULL;waitjob = this->waitQueue.next())     
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

void FCFSSchedulingPolicy::setGlobalTime(double globaltime)
{
  SchedulingPolicy::setGlobalTime(globaltime);
  
  if(this->lastAllowedStartTime < globaltime)
    this->lastAllowedStartTime = globaltime;
  
}

}
