#include <scheduling/job.h>
#include <utils/utilities.h>

#include <math.h>

namespace Simulator {

/***************************
  Constructor and destructors
***************************/
/**
 * The default constructor for the class
 */
Job::Job()
{
  JobNumber= -1; 
  SubmitTime= -1;
  WaitTime= -1;
  RunTime= -1;
  NumberProcessors= -1;
  AvgCPUTimeUsed= -1;
  UsedMemory= -1;
  RequestedProcessors= -1;
  RequestedTime= -1;
  OriginalRequestedTime = -1;
  RequestedMemory= -1;
  JobSimStatus = LOCAL_QUEUED;
  status = NO_STATUS;
  UserID= -1;
  GroupID= -1;
  Executable = -1;
  QueueNumber= -1;
  PartitionNumber= -1;
  PrecedingJobNumber= -1;
  ThinkTimePrecedingJob= -1;
 
  BWMemoryUsed= -1; 
  MemoryUsed= -1; 
  BWEthernedUsed= -1;
  BWNetworkUsed= -1; 
  
  JobSimStartTime= -1;
  JobSimFinishTime= -1;
  JobSimWaitTime= -1;
  JobSimSubmitTime = -1;
  JobSimisKilled = false;
  JobSimisBackfilled= false;
  JobSimBackfillingTime = -1;
  JobSimBSLD = -1;
  JobSimEstimateFinishTime = -1;
  JobSimSLD = -1;
  JobSimCenter = "";
  
  PenalizedRunTime = 0;
  OldPenalizedRunTime = 0;
  PenalizedRunTimeReal = 0;
  
  UserRuntimeEstimate = -1;
  JobSimEstimatedShadow  = -1;
  RuntimePrediction = -1;
  WaitTimePrediction = -1;

  PredictedMonetaryCost = -1;
  MonetaryCost = -1;
  MaximumAllowedCost = -1;
  JobSimEstimatedSTWithFF = -1;

  RunsAtReducedFrequency = false;
  RunEqualsEstimate = false;
}

/**
 * The default destructor for the class 
 */
Job::~Job()
{

  // delete all associated penaltie times for all buckets
  for ( jobMapBucketPenalizedTimes_t::iterator it = jobPenalties.begin(); it != jobPenalties.end(); it++)
    delete it->second;

  jobPenalties.clear();
}

/**
 * Reset all penalized times associated to all buckets or one bucket
 */
void Job::resetBucketPenalizedTime(int bucketId)
{
  jobListBucketPenalizedTime_t * currentBucket;

  if ( bucketId == -1 )
  {
    // delete all associated penaltie times for all buckets
    for ( jobMapBucketPenalizedTimes_t::iterator it = jobPenalties.begin(); it != jobPenalties.end(); it++)
      delete it->second;

    jobPenalties.clear();
  }
  else if ( jobPenalties.count(bucketId) != 0)
  {
    currentBucket = jobPenalties[bucketId];
    jobPenalties.erase(bucketId);
    delete currentBucket;
  }

}


/**
 * Add new penalized time related to bucket one bucket
 */
void Job::addBucketPenalizedTime(int bucketId, double startTime, double endTime, double penalizedTime)
{
  jobListBucketPenalizedTime_t * currentBucket;


  if ( jobPenalties.count(bucketId) == 0 )
  {
    currentBucket = new jobListBucketPenalizedTime_t();
    jobPenalties[bucketId] = currentBucket;
  }
  else
    currentBucket = jobPenalties[bucketId];

  // add new penaltie time for this bucket
  typedef pair<double, double> pairDouble;
  if (currentBucket->find(startTime) != currentBucket->end())
    currentBucket->erase(currentBucket->find(startTime));

  currentBucket->insert(pairDouble(startTime, penalizedTime));


}

/**
 * Get penalized times related to one bucket
 */
const jobListBucketPenalizedTime_t * Job::getBucketPenalizedTimes(int bucketId)
{
  return ( jobPenalties.count(bucketId) != 0 ? jobPenalties[bucketId] : NULL);
}



/**
 * This function returns the content of an attribute in string format 
 * @param attribute The string of the attribute to be returned 
 * @return A string contaning the value of the attribute 
 */
string Job::getAttributeValue(string attribute)
{
  string result  = "";
  
  if(attribute == "JobNumber") result = itos(JobNumber); 
  else if(attribute == "SubmitTime") result =  ftos(SubmitTime); 
  else if(attribute == "WaitTime") result = ftos(WaitTime); 
  else if(attribute == "RunTime") result = ftos(RunTime); 
  else if(attribute == "NumberProcessors") result = itos(NumberProcessors); 
  else if(attribute == "AvgCPUTimeUsed") result = ftos(AvgCPUTimeUsed); 
  else if(attribute == "UsedMemory") result = ftos(UsedMemory); 
  else if(attribute == "RequestedProcessors") result = itos(RequestedProcessors); 
  else if(attribute == "RequestedTime") result = ftos(RequestedTime); 
  else if(attribute == "OriginalRequestedTime") result = ftos(OriginalRequestedTime); 
  else if(attribute == "RequestedMemory") result = ftos(RequestedMemory); 
  else if(attribute == "JobSimStatus") result = itos(JobSimStatus); 
  else if(attribute == "status") result = itos(status); 
  else if(attribute == "UserID") result = itos(UserID); 
  else if(attribute == "GroupID") result = itos(GroupID); 
  else if(attribute == "Executable") result = itos(Executable); 
  else if(attribute == "QueueNumber") result = itos(QueueNumber); 
  else if(attribute == "PartitionNumber") result = itos(PartitionNumber); 
  else if(attribute == "PrecedingJobNumber") result = itos(PrecedingJobNumber); 
  else if(attribute == "ThinkTimePrecedingJob") result = itos(ThinkTimePrecedingJob); 
  else if(attribute == "BWMemoryUsed") result =   ftos(BWMemoryUsed); 
  else if(attribute == "MemoryUsed") result = ftos(MemoryUsed); 
  else if(attribute == "BWEthernedUsed") result =  ftos(BWEthernedUsed); 
  else if(attribute == "BWNetworkUsed") result = ftos(BWNetworkUsed); 
  else if(attribute == "JobSimStartTime") result = ftos(JobSimStartTime); 
  else if(attribute == "JobSimFinishTime") result = ftos(JobSimFinishTime); 
  else if(attribute == "JobSimWaitTime") result = ftos(JobSimWaitTime); 
  else if(attribute == "JobSimSubmitTime") result = ftos(JobSimSubmitTime); 
  else if(attribute == "JobSimisKilled") result = itos(JobSimisKilled); 
  else if(attribute == "JobSimisBackfilled") result = itos(JobSimisBackfilled); 
  else if(attribute == "JobSimBackfillingTime") result = ftos(JobSimBackfillingTime); 
  else if(attribute == "JobSimBSLD") result = ftos(JobSimBSLD); 
  else if(attribute == "JobSimEstimateFinishTime") result = ftos(JobSimEstimateFinishTime); 
  else if(attribute == "JobSimSLD") result = ftos(JobSimSLD); 
  else if(attribute == "JobSimCenter") result = JobSimCenter;
  else if(attribute == "PenalizedRunTime") result = ftos(PenalizedRunTime); 
  else if(attribute == "OldPenalizedRunTime") result = ftos(OldPenalizedRunTime); 
  else if(attribute == "PenalizedRunTimeReal") result = ftos(PenalizedRunTimeReal); 
  else if(attribute == "UserRuntimeEstimate") result = ftos(UserRuntimeEstimate); 
  else
    assert(false);
    
  return result;
}


/***************************
   Sets and gets functions  
***************************/


/**
 * Returns the AvgCPUTimeUsed
 * @return A double containing the AvgCPUTimeUsed
 */
double Job::getAvgCPUTimeUsed() 
{
  return AvgCPUTimeUsed;
}


/**
 * Sets the AvgCPUTimeUsed to the job 
 * @param theValue The AvgCPUTimeUsed
 */
void Job::setAvgCPUTimeUsed(double theValue)
{
  AvgCPUTimeUsed = theValue;
}


/**
 * Returns the BWEthernedUsed
 * @return A double containing the BWEthernedUsed
 */
double Job::getBWEthernedUsed() 
{
  return BWEthernedUsed;
}

/**
 * Sets the BWEthernedUsed to the job
 * @param theValue The BWEthernedUsed
 */
void Job::setBWEthernedUsed(double theValue)
{
  BWEthernedUsed = theValue;
}

/**
 * Returns the BWMemoryUsed
 * @return A double containing the BWMemoryUsed
 */
double Job::getBWMemoryUsed() 
{
  return BWMemoryUsed;
}


/**
 * Sets the BWMemoryUsed to the job
 * @param theValue The BWMemoryUsed
 */
void Job::setBWMemoryUsed(double theValue)
{
  BWMemoryUsed = theValue;
}


/**
 * Returns the BWNetworkUsed
 * @return A double containing the BWNetworkUsed
 */
double Job::getBWNetworkUsed() 
{
  return BWNetworkUsed;
}


/**
 * Sets the BWNetworkUsed to the job
 * @param theValue The BWNetworkUsed
 */
void Job::setBWNetworkUsed(double theValue)
{
  BWNetworkUsed = theValue;
}


/**
 * Returns the Executable
 * @return A integer containing the Executable
 */
int Job::getExecutable() 
{
  return Executable;
}


/**
 * Sets the Executable to the job
 * @param theValue The Executable
 */
void Job::setExecutable( int theValue)
{
  Executable = theValue;
}


/**
 * Returns the GroupID
 * @return A integer containing the GroupID
 */
int Job::getGroupID() 
{
  return GroupID;
}


/**
 * Sets the GroupID to the job
 * @param theValue The GroupID
 */
void Job::setGroupID( int theValue)
{
  GroupID = theValue;
}


/**
 * Returns the JobNumber
 * @return A integer containing the JobNumber
 */
int Job::getJobNumber() 
{
  
  return JobNumber;
}


/**
 * Sets the JobNumber to the job
 * @param theValue The JobNumber
 */
void Job::setJobNumber( int theValue)
{
  JobNumber = theValue;
}


/**
 * Returns the MemoryUsed
 * @return A double containing the MemoryUsed
 */
double Job::getMemoryUsed() 
{
  return MemoryUsed;
}


/**
 * Sets the MemoryUsed to the job
 * @param theValue The MemoryUsed 
 */
void Job::setMemoryUsed(double theValue)
{
  MemoryUsed = theValue;
}


/**
 * Returns the NumberProcessors
 * @return A integer containing the NumberProcessors
 */
int Job::getNumberProcessors() 
{
  return NumberProcessors;
}


/**
 * Sets the NumberProcessors to the job
 * @param theValue The NumberProcessors
 */
void Job::setNumberProcessors( int theValue)
{
  NumberProcessors = theValue;
}

/**
 * Sets the PartitionNumber to the job
 * @param theValue The PartitionNumber
 */
void Job::setPartitionNumber( int theValue)
{
  PartitionNumber = theValue;
}


/**
 * Returns the PartitionNumber
 * @return A integer containing the PartitionNumber
 */
int Job::getPartitionNumber() 
{
  return PartitionNumber;
}


/**
 * Returns the PrecedingJobNumber
 * @return A integer containing the PrecedingJobNumber
 */
int Job::getPrecedingJobNumber() 
{
  return PrecedingJobNumber;
}


/**
 * Sets the PrecedingJobNumber to the job
 * @param theValue The PrecedingJobNumber
 */
void Job::setPrecedingJobNumber( int theValue)
{
  PrecedingJobNumber = theValue;
}


/**
 * Returns the QueueNumber
 * @return A integer containing the QueueNumber
 */
int Job::getQueueNumber() 
{
  return QueueNumber;
}


/**
 * Sets the QueueNumber to the job
 * @param theValue The QueueNumber
 */
void Job::setQueueNumber( int theValue)
{
  QueueNumber = theValue;
}


/**
 * Returns the  RequestedMemory
 * @return A double containing the  RequestedMemory
 */
double Job::getRequestedMemory() 
{
  return RequestedMemory;
}


/**
 * Sets the RequestedMemory to the job
 * @param theValue The RequestedMemory
 */
void Job::setRequestedMemory(double theValue)
{
  RequestedMemory = theValue;
}


/**
 * Returns the RequestedProcessors
 * @return A integer containing the RequestedProcessors
 */
int Job::getRequestedProcessors() 
{
  return RequestedProcessors;
}


/**
 * Sets the  RequestedProcessors to the job
 * @param theValue The RequestedProcessors
 */
void Job::setRequestedProcessors( int theValue)
{
  RequestedProcessors = theValue;
}


/**
 * Returns the RequestedTime
 * @return A double containing the RequestedTime
 */
double Job::getRequestedTime() 
{
  if (RunEqualsEstimate) return RunTime;
  else return RequestedTime;
}


/**
 * Sets the RequestedTime to the job
 * @param theValue The RequestedTime
 */
void Job::setRequestedTime(double theValue)
{
   /* If firs time we sabe the original*/
  if(this->OriginalRequestedTime == -1)
    this->OriginalRequestedTime = theValue; 

  RequestedTime = theValue;
  UserRuntimeEstimate = theValue;  
}


/**
 * Returns the RunTime
 * @return A double containing the RunTime
 */
double Job::getRunTime() 
{
  if(RunTime == -1)
    return this->RequestedTime;
  else
    return RunTime;
}


/**
 * Sets the RunTime to the job
 * @param theValue The RunTime
 */
void Job::setRunTime(double theValue)
{
  //if no requestime is provided by the workload we estimate it as the same runtime ..
  if(RequestedTime == -1)
    this->setRequestedTime(theValue);
  RunTime = theValue;
  
}


/**
 * Returns the SubmitTime
 * @return A double containing the 
 */
double Job::getSubmitTime() 
{
  return SubmitTime;
}


/**
 * Sets the JobSimSubmitTime to the job
 * @param theValue The JobSimSubmitTime
 */
void Job::setSubmitTime(double theValue)
{
  SubmitTime = theValue;
  //by default the sim submit time is exactly the same as this one .. but maybe for the load simulation this may change ..
  this->JobSimSubmitTime = SubmitTime;
}


/**
 * Returns the ThinkTimePrecedingJob
 * @return A integer containing the ThinkTimePrecedingJob
 */
int Job::getThinkTimePrecedingJob() 
{
  return ThinkTimePrecedingJob;
}


/**
 * Sets the  ThinkTimePrecedingJob to the job
 * @param theValue The ThinkTimePrecedingJob
 */
void Job::setThinkTimePrecedingJob( int theValue)
{
  ThinkTimePrecedingJob = theValue;
}


/**
 * Returns the UsedMemory
 * @return A double containing the UsedMemory
 */
double Job::getUsedMemory() 
{
  return UsedMemory;
}


/**
 * Sets the UsedMemory to the job
 * @param theValue The UsedMemory
 */
void Job::setUsedMemory(double theValue)
{
  UsedMemory = theValue;
}


/**
 * Returns the UserID
 * @return A integer containing the UserID
 */
int Job::getUserID() 
{
  return UserID;
}


/**
 * Sets the UserID to the job
 * @param theValue The UserID
 */
void Job::setUserID( int theValue)
{
  UserID = theValue;
}


/**
 * Returns the WaitTime
 * @return A double containing the WaitTime
 */
double Job::getWaitTime() 
{
  return WaitTime;
}


job_status_t Job::getStatus() 
{
  return status;
}


/**
 * Sets the to the job
 * @param theValue The 
 */
void Job::setStatus( job_status_t theValue)
{
  status = theValue;
}

/**
 * Sets the status to the job
 * @param theValue The status
 */
void Job::setStatus( int theValue)
{
  switch(theValue)
  {
    case 0:
      status = FAILED;
      break;
    case 1: 
      status = COMPLETED;
      break;
    case 5:
      status = CANCELLED;
      break;
    default:
      status = COMPLETED;      
  }
}

/**
 * Sets the WaitTime to the job
 * @param theValue The WaitTime 
 */
void Job::setWaitTime(double theValue)
{
  WaitTime = theValue;
}

/**
 * Returns the JobSimFinishTime
 * @return A double containing the JobSimFinishTime
 */
double Job::getJobSimFinishTime() 
{
  return JobSimFinishTime;
}


/**
 * Sets the JobSimFinishTime to the job
 * @param theValue The JobSimFinishTime
 */
void Job::setJobSimFinishTime(double theValue)
{  
  JobSimFinishTime = theValue;
}


/**
 * Returns the JobSimStartTime
 * @return A double containing the JobSimStartTime
 */
double Job::getJobSimStartTime() 
{
  return JobSimStartTime;
}


/**
 * Sets the  JobSimStartTime to the job
 * @param theValue The JobSimStartTime
 */
void Job::setJobSimStartTime(double theValue)
{
  //we save the previous start time only if the startime is different than -1
  if(JobSimStartTime != -1)
    this->JobSimLastSimStartTime = JobSimStartTime;
  JobSimStartTime = theValue;
  this->JobSimWaitTime = theValue-this->JobSimStartTime;
}


vector< int > Job::getUsedCpus() const
{
  return UsedCpus;
}


/**
 * Sets the UsedCpus to the job
 * @param theValue The UsedCpus
 */
void Job::setUsedCpus(const vector< int >& theValue)
{
  UsedCpus = theValue;
}

/**
 * Returns the PenalizedRunTime
 * @return A double containing the PenalizedRunTime
 */
double Job::getPenalizedRunTime() 
{
  return PenalizedRunTime;
}


/**
 * Sets the PenalizedRunTime to the job
 * @param theValue The PenalizedRunTime 
 */
void Job::setPenalizedRunTime(double theValue)
{
  this->OldPenalizedRunTime = PenalizedRunTime;
  PenalizedRunTime = theValue;
}


/**
 * Returns the lastBucket
 * @return A integer containing the lastBucket
 */
int Job::getLastBucket() 
{
  return lastBucket;
}


/**
 * Sets the lastBucket to the job
 * @param theValue The lastBucket 
 */
void Job::setLastBucket( int theValue)
{
  lastBucket = theValue;
}


/**
 * Returns the startBucket
 * @return A integer containing the startBucket
 */
int Job::getStartBucket() 
{
  return startBucket;
}


/**
 * Sets the startBucket to the job
 * @param theValue The startBucket
 */
void Job::setStartBucket( int theValue)
{
  startBucket = theValue;
}


/**
 * Returns the JobSimEstimateFinishTime
 * @return A double containing the JobSimEstimateFinishTime
 */
double Job::getJobSimEstimateFinishTime() const
{
  return JobSimEstimateFinishTime;
}


/**
 * Sets the JobSimEstimateFinishTime to the job
 * @param theValue The JobSimEstimateFinishTime
 */
void Job::setJobSimEstimateFinishTime(double theValue)
{
  JobSimEstimateFinishTime = theValue;
}

/**
 * Returns the JobSimisKilled
 * @return A bool containing the JobSimisKilled
 */
bool Job::getJobSimisKilled()
{
  return this->JobSimisKilled;
}

/**
 * Sets the JobSimisKilled to the job
 * @param theValue The JobSimisKilled
 */
void Job::setJobSimisKilled(bool isKilled)
{
  this->JobSimisKilled = isKilled;
}

/**
 * Returns the JobSimSubmitTime
 * @return A double containing the JobSimSubmitTime
 */
double Job::getJobSimSubmitTime() const
{
  return JobSimSubmitTime;
}

/**
 * Sets the JobSimSubmitTime to the job
 * @param theValue The JobSimSubmitTime
 */
void Job::setJobSimSubmitTime(double theValue)
{
  JobSimSubmitTime = theValue;  
}


/**
 * Returns the JobSimisBackfilled
 * @return A bool containing the JobSimisBackfilled
 */
bool Job::getJobSimisBackfilled() const
{
  return JobSimisBackfilled;
}


/**
 * Sets the JobSimisBackfilled to the job
 * @param theValue The JobSimisBackfilled
 */
void Job::setJobSimisBackfilled(bool theValue)
{
  JobSimisBackfilled = theValue;
  
  if(!JobSimisBackfilled)
   JobSimBackfillingTime = -1;
}

/**
 * Returns the JobSimBackfillingTime
 * @return A double containing the JobSimBackfillingTime
 */
double Job::getJobSimBackfillingTime() const
{
  return JobSimBackfillingTime;
}


/**
 * Sets the  JobSimBackfillingTime to the job
 * @param theValue The JobSimBackfillingTime
 */
void Job::setJobSimBackfillingTime(double theValue)
{
  JobSimBackfillingTime = theValue;
  
  if(theValue != -1)
    JobSimisBackfilled = true;
}


/**
 * Returns the JobSimWaitTime
 * @return A double containing the JobSimWaitTime
 */
double Job::getJobSimWaitTime() const
{
  return JobSimWaitTime;
}


/**
 * Sets the JobSimWaitTime to the job
 * @param theValue The JobSimWaitTime
 */
void Job::setJobSimWaitTime(double theValue)
{  
  assert(theValue >= 0);
  
  JobSimWaitTime = theValue;
  
  double runtime;
   
  if(this->getRequestedTime() != -1)
    runtime = min(this->getRunTime(),this->getRequestedTime());
  else
    runtime = this->getRunTime();
  
  
  this->setJobSimSLD((this->getJobSimWaitTime()+runtime)/runtime);
  this->setJobSimBSLD((this->getJobSimWaitTime()+runtime)/max(runtime,(double)BSLD_THRESHOLD));

  this->setJobSimBSLDPRT((this->getJobSimWaitTime()+runtime+this->getPenalizedRunTime())/max(runtime+this->getPenalizedRunTime(),(double)BSLD_THRESHOLD));


  
}


/**
 * Returns the JobSimSLD
 * @return A double containing the JobSimSLD
 */
double Job::getJobSimSLD() const
{
  return JobSimSLD;
}


/**
 * Sets the JobSimSLD to the job
 * @param theValue The JobSimSLD
 */
void Job::setJobSimSLD(double theValue)
{
  assert(theValue >= 1);
  JobSimSLD = theValue;
}


/**
 * Returns the JobSimBSLD
 * @return A double containing the JobSimBSLD
 */
double Job::getJobSimBSLD() const
{
  return JobSimBSLD;
}


/**
 * Sets the JobSimBSLD to the job
 * @param theValue The JobSimBSLD
 */
void Job::setJobSimBSLD(double theValue)
{
  if(theValue > 1)
   JobSimBSLD = theValue;
  else 
   JobSimBSLD = 1;
}


/**
 * Returns the JobSimDeadlineMissed
 * @return A bool containing the JobSimDeadlineMissed
 */
bool Job::getjobSimDeadlineMissed() const
{
  return JobSimDeadlineMissed;
}


/**
 * Sets the JobSimDeadlineMissed to the job
 * @param theValue The JobSimDeadlineMissed
 */
void Job::setJobSimDeadlineMissed(bool theValue)
{
  JobSimDeadlineMissed = theValue;
}


/**
 * Returns the UserRuntimeEstimate
 * @return A double containing the UserRuntimeEstimate
 */
double Job::getUserRuntimeEstimate() const
{
  return UserRuntimeEstimate;
}


/**
 * Sets the UserRuntimeEstimate to the job
 * @param theValue The UserRuntimeEstimate
 */
void Job::setUserRuntimeEstimate(double theValue)
{
  UserRuntimeEstimate = theValue;
}

/**
 * Returns the JobSimCompleted 
 * @return A bool containing the JobSimCompleted
 */
bool Job::getJobSimCompleted() const
{
  return JobSimCompleted;
}


/**
 * Sets the JobSimCompleted to the job
 * @param theValue The JobSimCompleted
 */
void Job::setJobSimCompleted(bool theValue)
{
  JobSimCompleted = theValue;
}


/**
 * Returns the OldPenalizedRunTime
 * @return A double containing the OldPenalizedRunTime
 */
double Job::getOldPenalizedRunTime() const
{
  return OldPenalizedRunTime;
}


/**
 * Sets the OldPenalizedRunTime to the job
 * @param theValue The OldPenalizedRunTime
 */
void Job::setOldPenalizedRunTime(const double& theValue)
{
  OldPenalizedRunTime = theValue;
}

/**
 * Returns the center where the job was submitted
 * @return A string containing the center 
 */
string Job::getJobSimCenter() const
{
  return JobSimCenter;
}


/**
 * Sets the JobSimCenter to the job
 * @param theValue The JobSimCenter
 */
void Job::setJobSimCenter(const string& theValue)
{
  JobSimCenter = theValue;
}

/**
 * Returns the PenalizedRunTimeReal
 * @return A double containing the PenalizedRunTimeReal
 */
double Job::getPenalizedRunTimeReal() const
{
  return PenalizedRunTimeReal;
}


/**
 * Sets the PenalizedRunTimeReal to the job
 * @param theValue The PenalizedRunTimeReal
 */
void Job::setPenalizedRunTimeReal(double theValue)
{
  this->OldPenalizedRunTimeReal = this->PenalizedRunTimeReal;
  PenalizedRunTimeReal = theValue;  
}


/**
 * Returns the OldPenalizedRunTimeReal
 * @return A double containing the OldPenalizedRunTimeReal
 */
double Job::getOldPenalizedRunTimeReal() const
{
  return OldPenalizedRunTimeReal;
}


/**
 * Sets the OldPenalizedRunTimeReal to the job
 * @param theValue The OldPenalizedRunTimeReal
 */
void Job::setOldPenalizedRunTimeReal(double theValue)
{  
  OldPenalizedRunTimeReal = theValue;
}


/**
 * Returns the Status for the job 
 * @return A job_status_t containing the status for the job 
 */
job_status_t Job::getJobSimStatus() const
{
  return JobSimStatus;
}


/**
 * Sets the  JobSimStatus to the job
 * @param theValue The JobSimStatus
 */
void Job::setJobSimStatus(const job_status_t& theValue)
{
  JobSimStatus = theValue;
}


/**
 * Returns the JobSimLastSimStartTime
 * @return A double containing the JobSimLastSimStartTime
 */
double Job::getJobSimLastSimStartTime() const
{
  return JobSimLastSimStartTime;
}


/**
 * Sets the JobSimLastSimStartTime to the job
 * @param theValue The JobSimLastSimStartTime
 */
void Job::setJobSimLastSimStartTime(double theValue)
{
  JobSimLastSimStartTime = theValue;
}


/**
 * Returns the OriginalRequestedTime
 * @return A double containing the OriginalRequestedTime
 */
double Job::getOriginalRequestedTime() const
{
  return OriginalRequestedTime;
}


/**
 * Sets the OriginalRequestedTime to the job
 * @param theValue The OriginalRequestedTime
 */
 void Job::setOriginalRequestedTime(double theValue)
{
  OriginalRequestedTime = theValue;
}

/**
 * Returns the JobSimEstimatedShadow
 * @return A double containing the JobSimEstimatedShadow
 */
double Job::getJobSimEstimatedShadow() const
{
  return JobSimEstimatedShadow;
}

/**
 * Sets the JobSimEstimatedShadow to the job
 * @param theValue The JobSimEstimatedShadow
 */
void Job::setJobSimEstimatedShadow(double theValue)
{
  JobSimEstimatedShadow = theValue;
}




/**
 * Returns the resource management method that has been used for allocate the job
 * @return A RS_policy_type_t containing the resource selection policy used for alocate the job
 */
RS_policy_type_t Job::getallocatedWith() const
{
  return allocatedWith;
}



/**
 * Sets the resource management method that has been used for allocate the job
 * @param theValue The resource management policy used
 */
void Job::setAllocatedWith(const RS_policy_type_t& theValue)
{
  allocatedWith = theValue;
}

/**
 * Returns the predicted runtime for the job estimated by the prediction system (in case it is used, -1 otherwise)
 * @return An integer containing the prediction time
 */
double Job::getRuntimePrediction() const
{
  return RuntimePrediction;
}


/**
 * Sets the prediction time for the job, in case it is set by the prediction system 
 * @param theValue The runtime prediction 
 */
void Job::setRuntimePrediction(double theValue)
{
  RuntimePrediction = theValue;
}

/**
 * Returns the predicted runtime for the job estimated by the prediction system or by the scheduling system (in case it is used, -1 otherwise)
 * @return An integer containing the wt time
 */
double Job::getWaitTimePrediction() const
{
  return WaitTimePrediction;
}

/**
 * Sets the wait prediction time for the job, in case it is set by the prediction system or by the scheduling system
 * @param theValue The wt prediction 
 */
void Job::setWaitTimePrediction ( double theValue )
{
  WaitTimePrediction = theValue;
}


/**
 * Returns the predicted runtime for the job estimated by the prediction system or by the scheduling system (in case it is used, -1 otherwise)
 * @return An integer containing the wt time
 */
double Job::getPredictedMonetaryCost() const
{
  return PredictedMonetaryCost;
}

/**
 * Sets the cost prediction time for the job, in case it is set by the prediction system or by the scheduling system
 * @param theValue The cost prediction 
 */
void Job::setPredictedMonetaryCost ( double theValue )
{
  PredictedMonetaryCost = theValue;
}

/**
 * Returns the cost for the job returned by the scheduling system (in case it is used, -1 otherwise)
 * @return An integer containing the wt time
 */
double Job::getMonetaryCost() const
{
  return MonetaryCost;
}

/**
 * Sets the cost for the job, in case it is set by the prediction system or by the scheduling system
 * @param theValue The cost prediction 
 */
void Job::setMonetaryCost ( double theValue )
{
  MonetaryCost = theValue;
}


/**
 * Returns the maximum required cost that the user provided as an input of the scheduling 
 * @return An integer containing the maximum allowd cost 
 */
double Job::getMaximumAllowedCost() const
{
  return MaximumAllowedCost;
}

/**
 * Sets the maxiumum allowed cost for the job
 * @param theValue The cost in units allocations that the job wants to spend
 */
void Job::setMaximumAllowedCost ( double theValue )
{
  MaximumAllowedCost = theValue;
}

/**
 * Returns the EstimatedSTWithFF 
 * @return An integer containing the estimated start time with the FirstFit policy
 */
double Simulator::Job::getJobSimEstimatedSTWithFF() const
{
	return JobSimEstimatedSTWithFF;
}

/**
 * Sets the EstimatedSTWithFF for the job
 * @param theValue The estimated start time 
 */
void Simulator::Job::setJobSimEstimatedSTWithFF ( double theValue )
{
	JobSimEstimatedSTWithFF = theValue;
}

/**
 * Returns the BSLDRT 
 * @return An integer containing the BSLD for the job considering the penalized runtime
 */
double Simulator::Job::getJobSimBSLDPRT() const
{
	return JobSimBSLDPRT;
}

/**
 * Sets the BSLD for the job considering the penalized runtime
 * @param theValue The BSLD considering the PRT
 */
void Simulator::Job::setJobSimBSLDPRT ( double theValue )
{
	JobSimBSLDPRT = theValue;
}


/**
 * Returns the DisckUsed 
 * @return An integer containing DisckUsed BSLD 
 */
double Job::getDisckUsed() const
{
	return DisckUsed;
}

/**
 * Sets the average size of DisckUsed for the job
 * @param theValue The DisckUsed used
 */
void Job::setDisckUsed ( double theValue )
{
	DisckUsed = theValue;
}

bool Job::getRunsAtReducedFrequency()
{
   return RunsAtReducedFrequency;
}
void Job::setRunsAtReducedFrequency(bool ind)
{
   RunsAtReducedFrequency = ind;
}


double Job::getRunsAtFrequency()
{
   return frequency;
}
void Job::setRunsAtFrequency(double freq)
{
   frequency = freq;
}


}
