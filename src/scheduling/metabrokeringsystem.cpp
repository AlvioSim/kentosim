#include <scheduling/metabrokeringsystem.h>
#include <scheduling/gridjob.h>
#include <scheduling/gridanalogicalreservationtable.h>
#include <scheduling/brankpolicy.h>


namespace Simulator {
/**
 * The default constructor for the class
 */
MetaBrokeringSystem::MetaBrokeringSystem()
{
  UsePredictionService = false;
  predictionService = NULL;
  this->schedulingPolicySimulated = META_BROKERING_SYSTEM;
}

/**
 * This is the constructor that should be used, invoques that constructor for the parent classes  
 * @param log A reference to the logging engine
 * @param UsePredictionService A bool indicating if the prediction service has to be used or not 
 * @param predictionService A reference to the prediction service that will be used by the policy for estimate the job runtime 
 */
MetaBrokeringSystem::MetaBrokeringSystem(Log* log,bool UsePredictionService,Predictor* predictionService)
{
  //by default we will create a FCFSC 
  this->waitQueue = new FCFSJobQueue();
  
  //as all the scheduling policies must have associated a Reservation table by definition 
  this->log = log;

  this->reservationTable = new GridAnalogicalReservationTable();
  
  this->UsePredictionService = UsePredictionService;
  this->predictionService = predictionService;
  this->schedulingPolicySimulated = META_BROKERING_SYSTEM;
}


/**
 * The default destructor for the class 
 */
MetaBrokeringSystem::~MetaBrokeringSystem()
{
  //before destroy all the stuff, we will iterate over all the policies, and we will generate the traces 
  for(vector<SchedulingPolicy*>::iterator polit = this->VirtualOrganitzations->begin();
      polit != this->VirtualOrganitzations->end();
      ++polit)
  {
    SchedulingPolicy* pol = *polit;    
    delete pol;
  }
  
  delete this->VirtualOrganitzations;
  
  for(vector<SchedulingPolicy*>::iterator archit = this->VirtualOrganitzations->begin();
     archit != this->VirtualOrganitzations->end();
     ++archit)
  {
    SchedulingPolicy* arch = *archit;
    delete arch;
  }
   
  delete this->reservationTable;;  

  
}


/**
 * Invoqued when the global scheduling has finished it mainly creates the traces for each center 
 */
void MetaBrokeringSystem::PostProcessScheduling()
{
  //before destroy all the stuff, we will iterate over all the policies, and we will generate the traces 
  for(vector<SchedulingPolicy*>::iterator polit = this->VirtualOrganitzations->begin();
      polit != this->VirtualOrganitzations->end();
      ++polit)
  {
    SchedulingPolicy* pol = *polit;  
    pol->PostProcessScheduling(); 
    
  }

}


/**
 * Adds a given job to the wait queue, usually when it arrives to the system.
 * this function does not call to the parent class scheduling policy due to it will be called in each policy instantiated    
 * The current approach will iterater over each center and will ask for a performance estimation 
 * that the given job would achieve in the given center with the current load. 
 * @see The class SchedulingPolicy 
 * @param job the job to be added to the wait queue
 */
void MetaBrokeringSystem::jobAdd2waitq(Job* job)
{   
  //we simple get the VO to which the job was submitted and we submit to it
  GridJob* gjob = (GridJob*) job;

  //we find the VO where it was initially submitted
  string SubmittedVO = gjob->getVOSubmitted();  
  
  map<string,SchedulingPolicy*>::iterator itVO = this->VONamesMapping.find(SubmittedVO);
  assert(itVO != this->VONamesMapping.end()); 

  SchedulingPolicy* VO = itVO->second;

  assert(SubmittedVO == VO->getCenterName());
  
  this->submissions.insert(PairSubmission(job,VO));
  VO->jobAdd2waitq(job);  

}



/**
 * Moves a job from the wait queue to the reservation table, using an specified set of resources
 * @see The class SchedulingPolicy  
 * @param job The job that has started
 */
void MetaBrokeringSystem::jobWaitq2RT(Job* job)
{
  //we simply proxy the query of removing the job from the wait queue and add it to the Run table 
  map<Job*,SchedulingPolicy*>::iterator subit = this->submissions.find(job);
  
  assert(subit != this->submissions.end()); //the job should be already submitted !
  
  SchedulingPolicy* policy = subit->second;
  
  log->debug( " The job "+itos(job->getJobNumber())+" has to be moved to the run table.",2);
  
  policy->jobWaitq2RT(job); 
  //this->waitQueue->erase(job);
  this->RunningQueue.insert(job);

  // When a job finished we have to try to schedule the waiting jobs (that it was not possible to schedule them)
  if(this->waitQueue->getjobs() >0){
    Job *myjob = this->waitQueue->headJob();
    this->jobAdd2waitq(myjob);
  }
}

/**
 * This function must be called by the brokers when a job is forwarded from one VO to anotherone, this is specially important due to the flow of event notifications. This is not a matter of the model, is a matter of implementation of the simulator.
 * @param job 
 * @param VO 
 */
void MetaBrokeringSystem::jobForwarded(Job* job,SchedulingPolicy* newVO)
{
   GridJob* gjob = (GridJob*) job;

   //first we increment the number of forwarded times .
   gjob->setForwardedTimes(gjob->getforwardedTimes()+1);

  //we simply remove the link
  map<Job*,SchedulingPolicy*>::iterator subit = this->submissions.find(job);
  assert(subit != this->submissions.end()); //the job should be already submitted !
  this->submissions.erase(subit);

  //we update the link where the job has been forwarded 
  this->submissions.insert(PairSubmission(job,newVO));


}

// When a Job finishes this method is invoked

/**
 * Moves a job from teh wait queue to the reservation table, using an specified set of resources
 * @see The class SchedulingPolicy  
 * @param job The job that has started
 */
void MetaBrokeringSystem::jobRemoveFromRT(Job* job)
{
  //We call to the base class - important it has to update some info   
  SchedulingPolicy::jobRemoveFromRT(job);

  //we simply proxy the query of removing the job from the wait queue and add it to the Run table 
  map<Job*,SchedulingPolicy*>::iterator subit = this->submissions.find(job);
  
  assert(subit != this->submissions.end()); //the job should be already submitted !
  
  SchedulingPolicy* policy = subit->second;

  
  log->debug( " The job "+itos(job->getJobNumber())+" has to be moved to the run table.",2);
  
  policy->jobRemoveFromRT(job); 
  
  //time to update the predictor service in case if it is being used in the simulation model
  //We will estimate the job runtime if it was set or not 
  if(this->UsePredictionService)
  {      
   
    this->predictionService->jobTerminated(job, this->globaltime,job->getJobSimStatus() == COMPLETED);
    this->predictionService->estimateJobPerformance(job);
  }

  this->RunningQueue.erase(job);


  // When a job finished we have to try to schedule the waiting jobs (that it was not possible to schedule them)
  if(this->waitQueue->getjobs() >0){
    Job *myjob = this->waitQueue->headJob();
    this->jobAdd2waitq(myjob);
  }

}


/**
 * Chooses the more appropiate job to run .
 * @see The class SchedulingPolicy  
 * @return The appropiate job to run 
 */
Job* MetaBrokeringSystem::jobChoose()
{
  //for this version this remains empty, however it will be used for implemeent more sophisticated scheduling decisions 
}

/**
 * Function that is called once the job should start 
 * @see The class SchedulingPolicy  
 * @param job The job that to start 
 */
void MetaBrokeringSystem::jobStart(Job* job)
{
   //we simply proxy the query of removing the job from the wait queue and add it to the Run table 
  map<Job*,SchedulingPolicy*>::iterator subit = this->submissions.find(job);
  
  assert(subit != this->submissions.end()); //the job should be already submitted !
  
  SchedulingPolicy* policy = subit->second;
  
  log->debug( " The job "+itos(job->getJobNumber())+" has to start.",2);
  
  policy->jobStart(job); 
  
  //we update the hashmap of the submitted jobs 
  map<SchedulingPolicy*,int>::iterator subjobsit = this->SubmittedJobs.find(policy);
  
  assert(subjobsit != this->SubmittedJobs.end());
  
  this->SubmittedJobs[policy] = this->SubmittedJobs[policy]+1;

}

/**
 * Returns the number of jobs currently allocated in the waitQueue
 * @return An integer containing the number of jobs in the WQ
 */
double MetaBrokeringSystem::getJobsIntheWQ()
{
  return this->waitQueue->getjobs();
}

/**
 * Returns the number of jobs that are currently backfilled 
 * @return An integer containing the number of backfilled jobs 
 */
double MetaBrokeringSystem::getBackfilledJobs()
{
  double backfilledJobs = 0;

  for(vector<SchedulingPolicy*>::iterator polit = this->VirtualOrganitzations->begin();
      polit != this->VirtualOrganitzations->begin();
      ++polit)
  {
    SchedulingPolicy* pol = *polit;    
    backfilledJobs+= pol->getBackfilledJobs();
  }
  
  return backfilledJobs;
}

/**
 * Returns the number of processors used 
 * @return An integer containing the number of processors used 
 */
int  MetaBrokeringSystem::getNumberCPUSUsed()
{
   int used = 0;
   
   for(vector<SchedulingPolicy*>::iterator polit = this->VirtualOrganitzations->begin();
      polit != this->VirtualOrganitzations->begin();
      ++polit)
  {
    SchedulingPolicy* pol = *polit;    
    used+= pol->getreservationTable()->getNumberCPUSUsed();
  }  
  
  return used;
}
  

/**
 * Sets a reference to the simulation class, needed by some policies 
 * @param theValue A reference to the simulation class 
 */
void MetaBrokeringSystem::setSim(Simulation* theValue)
{  
  //we have to iterate over all the policies and set the simulation reference
  for(vector<SchedulingPolicy*>::iterator polit = this->VirtualOrganitzations->begin();
      polit != this->VirtualOrganitzations->end();
      ++polit)
  {
    SchedulingPolicy* pol = *polit;    
    pol->setSim(theValue);
  }
} 

/**
 * Sets the global time of the simulation 
 * @param globaltime The double with the global time 
 */
void MetaBrokeringSystem::setGlobalTime(double globaltime)
{
  this->log->debug("Setting the global time of the schdeuling policy and reservationTable to "+ftos(globaltime),2);
  this->globaltime = globaltime;
  
  /* we set also the global time for the reservationTable */
  this->reservationTable->setGlobaltime(globaltime);
  
  //we have to iterate over all the policies and set the simulation reference
  for(vector<SchedulingPolicy*>::iterator polit = this->VirtualOrganitzations->begin();
      polit != this->VirtualOrganitzations->end();
      ++polit)
  {
    SchedulingPolicy* pol = *polit;    
    pol->setGlobalTime(globaltime);
  }  
}

/**
 * Return the the amount of pending work (area of requested time x requested processors for the queued jobs)
 * @return A integer containing the pending work
 */
double MetaBrokeringSystem::getLeftWork()
{
  return 0;
}
  
/**
* Return the number of jobs in the run queue 
* @return A integer containing the number of running jobs 
*/
double MetaBrokeringSystem::getJobsIntheRQ()
{
  return 0;
}


/**
 * Returns the metric that contains the estimation of which would be the performance of a given job if it would be allocated now. Now this is not implemented , but would be in the future
 * @param MetricTpye The metric that indicates which performance variable has to be computed 
 * @param job The job that will be checked
 * @return The metric containing the performance that the given job would experiment if it would be submitted right now
 */
 
Metric* MetaBrokeringSystem::EstimatePerformanceResponse(metric_t MetricTpye,Job* job)
{
  return NULL;
}

/***************************************************************************************************************************
******************************gets and sets ********************************************************************************
****************************************************************************************************************************/


/**
 * Returns the vector that contains the different virtual organitzations available in the system
 * @return A vector containing the different VirtualOrganitzations
 */
vector< SchedulingPolicy * >* MetaBrokeringSystem::getVirtualOrganitzations() const
{
	return VirtualOrganitzations;
}

/**
 * Function that sets the vector that contains the different virtual organitzations available in the system
 * @param theValue The VirtualOrganitzations
 */
void MetaBrokeringSystem::setVirtualOrganitzations ( vector< SchedulingPolicy * >* theValue )
{
  VirtualOrganitzations = theValue;
  
 //we itereate over the policies and inform about the corresponding meta system.
 for(vector< SchedulingPolicy * >::iterator it = VirtualOrganitzations->begin(); it != VirtualOrganitzations->end();++it)
 {
   SchedulingPolicy* VO = *it;
   
   //in case it is a BRANK_POLICY 
   if(VO->getschedulingPolicySimulated() == BRANK_POLICY)
   {
     ((BRANKPolicy*) VO)->setMetaSystem(this);
   }

   this->SubmittedJobs.insert(PairSubmitedJobs(VO,0));    
 }
}



/**
 * Returns the vector that contains the different names virtual organitzations available in the system
 * @return A vector containing the different names for the VirtualOrganitzations
 */

vector< string > MetaBrokeringSystem::getVONames() const
{
	return VONames;
}


/**
 * Function that sets the vector that contains the names for the different virtual organitzations available in the system
 * @param theValue The VirtualOrganitzations
 */
void MetaBrokeringSystem::setVONames ( const vector< string >& theValue )
{
  VONames = theValue;
  int index = 0;
  
  assert(theValue.size() == this->VirtualOrganitzations->size());

  for(vector<string>::iterator it = VONames.begin(); it != VONames.end(); ++it)
  {
    string VOName = *it;
    SchedulingPolicy* pol = this->VirtualOrganitzations->operator[](index);
    this->VONamesMapping.insert(pairName(VOName,pol));
    index++;
  }
}

}

