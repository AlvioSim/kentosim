#include <scheduling/easypredschedulingpolicy.h>
#include <scheduling/simulation.h>


namespace Simulator {

/**
 * The default constructor for the class
 */

EASYPredSchedulingPolicy::EASYPredSchedulingPolicy()
{
  this->predictor = NULL;
  this->predictorTypeUsed = OTHER_PREDICTOR;
}



/**
 * This is the constructor that should be used, invoques that constructor for the parent classes 
 * @param theValue A reference to the architecture configuration file  
 * @param log A reference to the logging engine
 * @param globaltime The starttime of the simulation
 * @param backfillingType The backfillingType that have to be used 
 */
EASYPredSchedulingPolicy::EASYPredSchedulingPolicy(ArchitectureConfiguration* theValue,Log* log,double globaltime,Backfilling_variant_t backfillingType) : EASYSchedulingPolicy(theValue,log,globaltime,backfillingType)
{
}

/**
 * The default destructor for the class 
 */
EASYPredSchedulingPolicy::~EASYPredSchedulingPolicy()
{
}


/**
 * Function that returns the runtime prediction for a given job 
 * @param job The job whom prediction is required 
 * @return A double indicating the runtime estimation 
 */
double EASYPredSchedulingPolicy::predictRequestedRuntime(Job* job)
{
   Prediction* pred = this->predictor->jobArrived(job);
   double ret = pred->getprediction();
   
   delete pred;
   
   return ret;
}

/**
 * Adds a given job to the wait queue, usually when it arrives to the system.
 * @see The class SchedulingPolicy 
 * @param job the job to be added to the wait queue
 */
void EASYPredSchedulingPolicy::jobAdd2waitq(Job* job) //Adds a given job to the wait queue, usually when it arrives to the system
{
  //first we call to the upper class that will make some arrangements in the jobs, like updated the job run time  according the jobruntime factor 
  SchedulingPolicy::jobAdd2waitq(job);


/*  virtual 
2- If not:
  2.1- If the number of reservations is less than the maximum allowed by the current policy:
      + We allocate the job to the current reservation table. */
      
  
  //first of all we predict the job runtime estimate , the  virtual  value will be stored in the requested runtime value for the job 
  double requestedRT = this->predictRequestedRuntime(job);
  
  log->debug("The predicted runtime for the job "+itos(job->getJobNumber())+" is "+ftos(requestedRT),4);
  job->setRequestedTime(requestedRT);
   
  EASYSchedulingPolicy::jobAdd2waitq(job);
  
}


/**
 * Function that is called once the job should start 
 * @see The class SchedulingPolicy  
 * @param job The job that to start 
 */
void EASYPredSchedulingPolicy::jobStart(Job* job)
{
  predictor->jobStarted(job,globaltime);
  
  EASYSchedulingPolicy::jobStart(job);

}

/**
 * Removes a job from the RT. 
 * @see The class SchedulingPolicy  
 * @param job Job to be removed from the wait queue
 */
void EASYPredSchedulingPolicy::jobRemoveFromRT(Job* job) 
{
  //We call to the base class - important it has to update some info   
  SchedulingPolicy::jobRemoveFromRT(job);

  /* we get the information regarding the allocation for the job */
  AnalogicalJobAllocation* allocation = (AnalogicalJobAllocation*) 
                     ((AnalogicalResertavionTable*)this->reservationTable)->getJobAllocation(job);
  
  bool finishedEarlier = allocation->getendTime()-allocation->getstartTime() > job->getRunTime();
  bool after = allocation->getendTime()-allocation->getstartTime() < job->getRunTime();
  
   log->debug("The job "+itos(job->getJobNumber())+" has finished. It started at "+ftos(job->getJobSimStartTime())+" and finished at "+ftos(job->getJobSimFinishTime())+" its rt is "+ftos(job->getRunTime())+" it requested was "+ ftos(job->getRequestedTime())+" allocation size "+ftos(allocation->getendTime()-allocation->getstartTime()),2);
  
  
  
  /*If the job has finished earlier or later than expected we should*/
  if(after)
  {    
    //We must call the function that reschedules called deadLineMissed that:
    //Jobs must be rescheduled .. and the buckets for the job must be extended 
    this->deadlineMissed(job);
    //we update the ExtraForBackfill and ShadowForBackfill 
    this->UpdateShadowForBackfill();
    return;
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
    log->debug("The job "+itos(job->getJobNumber())+" has finished earlier than expected. Rescheduling all the jobs",2);
    log->debug(ftos(job->getRunTime())+"<"+ftos(allocationLength),2);
    //We must call the function that reschedules called deadLineMissed that:
    this->JobOverstimated(job);
  }
  
  job->setJobSimStatus(COMPLETED);
  
  //feedback the predictor 
  this->predictor->jobTerminated(job,job->getJobSimEstimateFinishTime(),job->getJobSimStatus() == COMPLETED);
    
}

/**
 * This function is triggered when the job has been understimated. In this case the reservation table has to be recomputed. We should reschedule all the jobs due to may be the current schedule is not as good as expected. Job prediction prediction has to be recomputed and the predictor has to be advised. 
 * @param job The job that has reached the deadline 
 */
void EASYPredSchedulingPolicy::deadlineMissed(Job* job)
{ 
 //this means that the job should be killed in traditional backfilling however, as we r using prediction 
 //we have to extend the deadline for the job 
 //we deallocate all the queued jobs
 this->resetSchedulingAllQueuedJobs();
    
 //we extend the buckets for the current scheduled job
 double alpha = computeRuntimeExtension(job);
 ((AnalogicalResertavionTable*)this->reservationTable)->extendRuntime(job,alpha);
 
 
 //before update the finish time we remove it from the running queue and reinsert it later. This have to be done due to the set container for the running queue is based on the finish time, and we are changeing it
 this->RunningQueue.erase(job);
 
 //updating variable information for the job 
 job->setRequestedTime(job->getRequestedTime()+alpha);
 job->setJobSimFinishTime(job->getRequestedTime()+job->getJobSimStartTime());
 job->setJobSimEstimateFinishTime(job->getRequestedTime()+job->getJobSimStartTime());
 
 //we reinsert the job 
 this->RunningQueue.insert(job);
 
 //we must create again the finish event for the job and update its run time 
 this->sim->forceJobFinish(job->getJobSimFinishTime(),job);
    
 //we reeschedule all the queued jobs
 this->rescheduleAllQueuedJobs();  
}

/**
 * This function is called when a deadlineMissed is triggered due to understimation of a job. The function awares the predictor about the mistake and requires a new prediction.
 * @param job 
 * @return 
 */
double EASYPredSchedulingPolicy::computeRuntimeExtension(Job* job)
{
  double originalEstimation = job->getRequestedTime();

  //now its simple but we can implement more sophisticate stuff
  Prediction* newPred = this->predictor->jobDeadlineMissed(job,globaltime);
  
  double ret = newPred->getprediction()-originalEstimation;
  
  delete newPred;
  
  return ret;
}


/**
 * Function that returns the reference to the predictor being used in  the EASY Policy 
 * @return A reference to the used predictor
 */
 
Predictor* EASYPredSchedulingPolicy::getpredictor() const
{
  return predictor;
}


/**
 * Sets the predictor being used in by the policy
 * @param theValue A reference to the predictor
 */
void EASYPredSchedulingPolicy::setPredictor(Predictor* theValue)
{
  predictor = theValue;
}


/**
 * Returns the predictor type used by the policy
 * @return A Predictor_type_t with the predictor type
 */
Predictor_type_t EASYPredSchedulingPolicy::getpredictorTypeUsed() const
{
  return predictorTypeUsed;
}


/**
 * Sets the type of predictor used in the simulation.
 * @param theValue The type of predictor used in the simulation.
 */
void EASYPredSchedulingPolicy::setPredictorTypeUsed(const Predictor_type_t& theValue)
{
  predictorTypeUsed = theValue;
}

}
