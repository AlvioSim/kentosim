#include <scheduling/policyentitycollector.h>
#include <scheduling/gridbackfilling.h>
#include <scheduling/isisdispatcher.h>
#include <scheduling/brankpolicy.h>

namespace Simulator {

/**
 * The default constructor for the class
 */
PolicyEntityCollector::PolicyEntityCollector(SchedulingPolicy* policy, Log* log)
{
  this->lastNumberOfBackfilledJobs = 0;
  this->policy = policy;
  this->log = log;
}

/**
 * The default destructor for the class 
 */
PolicyEntityCollector::~PolicyEntityCollector()
{
}

/**
 * Function that collects all the values form each sampling vector, currently several type of values are being collected, among others: nmberJobsInTheQueue, backfilledJobs and numberCPUSUsed
 * @param timeStamp Indicates the time where the values are being collected
 */
void PolicyEntityCollector::pullValues(double timeStamp)
{

  
  assert(numberJobsInTheQueue.size() == backfilledJobs.size());
  assert(numberCPUSUsed.size() == backfilledJobs.size());
  assert(numberCPUSUsed.size() == leftWork.size());
  assert(leftWork.size() == runningJobs.size());
  assert(runningJobs.size() == centerName.size());
  
  int numEntries = numberJobsInTheQueue.size();
  
  for(int i = 0; i < numEntries; i++)
  {                 
     assert(!(numberJobsInTheQueue.operator[](i) == 0 
              && leftWork.operator[](i) != 0));       
  }
  
  

  if(policy->getschedulingPolicySimulated() == ISIS_DISPATCHER)  
  {
    /*we will add one sampling per scheduling policy */
    ISISDispatcherPolicy* schedG = (ISISDispatcherPolicy*) this->policy;
    int pos = 0;
    
    for(vector<SchedulingPolicy*>::iterator it = schedG->getISISDispatcherPolicies()->begin();
        it != schedG->getISISDispatcherPolicies()->end();
        ++it
    )
    {
      string centerName = schedG->getISISDispatcherCentersName()->operator[](pos);    
      
      SchedulingPolicy* sched = *it;
      
      this->timeStamp.push_back(timeStamp);
      this->numberJobsInTheQueue.push_back(sched->getJobsIntheWQ());
      this->backfilledJobs.push_back(sched->getBackfilledJobs()-lastNumberOfBackfilledJobs);
      this->numberCPUSUsed.push_back(sched->getreservationTable()->getNumberCPUSUsed());
      this->leftWork.push_back(sched->getLeftWork());
      this->runningJobs.push_back(sched->getJobsIntheRQ());
      this->centerName.push_back(centerName);      
       
      pos++;
    }
  }
  else if(policy->getschedulingPolicySimulated() == BRANK_POLICY)  
  {
    /*we will add one sampling per scheduling policy */
    pullBrankInfo(policy,timeStamp);
  }
  else if(policy->getschedulingPolicySimulated() == META_BROKERING_SYSTEM)
  {
     MetaBrokeringSystem* metaSG = (MetaBrokeringSystem*) this->policy;
  
     for(vector<SchedulingPolicy*>::iterator archit = metaSG->getVirtualOrganitzations()->begin();
     archit != metaSG->getVirtualOrganitzations()->end();
     ++archit)
     {
       SchedulingPolicy* brpol = *archit;
          
       pullBrankInfo(brpol,timeStamp); 
     }
  }
  else
  {
    assert(!(policy->getJobsIntheWQ() == 0 && policy->getLeftWork() != 0));
    
    double jwq = policy->getJobsIntheWQ();
    double jbck = policy->getBackfilledJobs()-lastNumberOfBackfilledJobs;
    double cpusd = policy->getreservationTable()->getNumberCPUSUsed();
    double wlf = policy->getLeftWork();
    double rjobs = policy->getJobsIntheRQ();
    
    this->log->debug("Jobs in queue "+ftos(jwq),2);
    this->log->debug("Backfilled jobs "+ftos(cpusd),2);
    this->log->debug("Cpus used  "+ftos(cpusd),2);
    this->log->debug("Less workleft "+ftos(wlf),2);
    this->log->debug("Jobs in the run queue "+ftos(rjobs),2);
    
    
    this->timeStamp.push_back(timeStamp);
    this->numberJobsInTheQueue.push_back(jwq);
    this->backfilledJobs.push_back(jbck);
    this->numberCPUSUsed.push_back(cpusd);
    this->leftWork.push_back(wlf);
    this->runningJobs.push_back(rjobs);
    this->centerName.push_back("Unique");
  
    this->lastNumberOfBackfilledJobs  = policy->getBackfilledJobs(); 
  }
}

/**
 * Extract the infromation for all the centers that are modeled in the BrankArchitecture
 * @param policy The BRank System
 * @param timeStamp The timeStamp
 */
void PolicyEntityCollector::pullBrankInfo(SchedulingPolicy* brpol,double timeStamp)
{
    BRANKPolicy* schedG = (BRANKPolicy*) brpol;

    int pos = 0;
    
    for(vector<SchedulingPolicy*>::iterator it = schedG->getBRANKPolicyPolicies()->begin();
        it != schedG->getBRANKPolicyPolicies()->end();
        ++it
    )
    {
      string centerName = schedG->getBRANKPolicyCentersName()->operator[](pos);    
      
      SchedulingPolicy* sched = *it;
      
      this->timeStamp.push_back(timeStamp);
      this->numberJobsInTheQueue.push_back(sched->getJobsIntheWQ());
      this->backfilledJobs.push_back(sched->getBackfilledJobs()-lastNumberOfBackfilledJobs);
      this->numberCPUSUsed.push_back(sched->getreservationTable()->getNumberCPUSUsed());
      this->leftWork.push_back(sched->getLeftWork());
      this->runningJobs.push_back(sched->getJobsIntheRQ());
      this->centerName.push_back(centerName);      

      pos++;
    }
}

/**
 * Returns the sampling vector of number of backfilled jobs 
 * @return The sampling vector 
 */
vector<double>* PolicyEntityCollector::getbackfilledJobs()
{
  return &this->backfilledJobs;
}

/**
 * Returns the sampling vector of number of jobs in the queue
 * @return The sampling vector 
 */
vector<double>* PolicyEntityCollector::getnumberJobsInTheQueue()
{
  return &this->numberJobsInTheQueue;
}


/**
 * Returns the sampling vector of number of cpus used 
 * @return The sampling vector 
 */
vector<double>* PolicyEntityCollector::getnumberCPUSUsed()
{
  return &this->numberCPUSUsed;
}

/**
 * Returns the sampling vector of left work 
 * @return The sampling vector 
 */
vector<double>* PolicyEntityCollector::getleftWork()
{
  return &this->leftWork;
}

/**
 * Returns the sampling vector of number of running jobs
 * @return The sampling vector 
 */
vector<double>* PolicyEntityCollector::getrunningJobs()
{
  return &this->runningJobs;
}

/**
 * Returns the time stamp for each of the collections 
 * @return The time stamp vector 
 */
vector<double>* PolicyEntityCollector::gettimeStamp()
{
  return &this->timeStamp;
}


/**
 * Returns the center for each of the collections 
 * @return The center vector 
 */
vector<string>* PolicyEntityCollector::getcenterName()
{
  return &this->centerName;
}


}
