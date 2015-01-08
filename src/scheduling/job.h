#ifndef SIMULATORJOB_H
#define SIMULATORJOB_H

#define BSLD_THRESHOLD 60

#include <vector>
#include <string>
#include <map>

using namespace std;
using std::vector;

namespace Simulator {


// key is start time and value is a penalized time 
typedef map<double, double> jobListBucketPenalizedTime_t;
typedef map<int, jobListBucketPenalizedTime_t *> jobMapBucketPenalizedTimes_t;


/** The enumaration with all the different status that a given job may go through */
enum job_status_t 
{
   COMPLETED = 0,
   KILLED_BACKFILLING,
   KILLED_QUEUE_LIMIT,
   KILLED_NOT_ENOUGH_RESOURCES,
   RUNNING,
   LOCAL_QUEUED,
   GRID_QUEUED,
   FAILED,
   CANCELLED,
   NO_STATUS,
};

/** This enumeration shows all the resource selection policies avaiable in the simulator. The scheduling policy descides which jobs have to starts, and the RS policy decides in which resources the jobs are allocated */
enum RS_policy_type_t{
    FIRST_FIT=0,
    FIRST_CONTINOUS_FIT,
    FF_AND_FCF,
    FIND_LESS_CONSUME,
    FIND_LESS_CONSUME_THRESSHOLD,
    EQUI_DISTRIBUTE_CONSUME,
    DEFAULT_RS_P,
    DC_FIRST_VM,
    OTHER_RS,
};


/**
* @author Francesc Guim,C6-E201,93 401 16 50,
*
*/
class Job{
public:
  //Constructors and destructors
  Job();
  ~Job();

  //Set and get methods

  void resetBucketPenalizedTime(int bucketId = -1);
  void addBucketPenalizedTime(int bucketId, double startTime, double endTime, double penalizedTime);
  const jobListBucketPenalizedTime_t * getBucketPenalizedTimes(int bucketId);

  void setAvgCPUTimeUsed(double theValue);
  double getAvgCPUTimeUsed();
  void setBWEthernedUsed(double theValue);
  double getBWEthernedUsed() ;
  void setBWMemoryUsed(double theValue);  
  double getBWMemoryUsed() ;
  void setBWNetworkUsed(double theValue);  
  double getBWNetworkUsed() ;
  void setExecutable( int theValue);  
  int getExecutable() ;
  void setGroupID( int theValue);  
  int getGroupID() ;
  void setJobNumber( int theValue);  
  int getJobNumber() ;
  void setMemoryUsed(double theValue);  
  double getMemoryUsed() ;
  void setNumberProcessors( int theValue);  
  int getNumberProcessors() ;    
  void setPartitionNumber( int theValue);
  int getPartitionNumber() ;
  void setPrecedingJobNumber( int theValue);  
  int getPrecedingJobNumber() ;
  void setQueueNumber( int theValue);  
  int getQueueNumber() ;
  void setRequestedMemory(double theValue);  
  double getRequestedMemory() ;
  void setRequestedProcessors( int theValue);  
  int getRequestedProcessors() ;
  void setRequestedTime(double theValue);  
  double getRequestedTime() ;
  void setRunTime(double theValue);  
  double getRunTime() ;
  void setSubmitTime(double theValue);  
  double getSubmitTime() ;
  void setThinkTimePrecedingJob( int theValue);  
  int getThinkTimePrecedingJob() ;
  void setUsedMemory(double theValue);  
  double getUsedMemory() ;
  void setUserID( int theValue);  
  int getUserID() ;
  void setWaitTime(double theValue);  
  double getWaitTime() ;
  void setStatus( job_status_t theValue);  
  void setStatus( int theValue);  
  job_status_t getStatus() ;
  void setJobSimFinishTime(double theValue);
  double getJobSimFinishTime()  ;
  void setJobSimStartTime(double theValue);
  double getJobSimStartTime()  ;
  void setUsedCpus(const vector< int >& theValue);
  vector< int > getUsedCpus() const;
  void setPenalizedRunTime(double theValue);
  double getPenalizedRunTime()  ;
  void setLastBucket(  int  theValue);
  int getLastBucket()  ;
  void setStartBucket(  int  theValue);
  int getStartBucket()  ;
  void setJobSimEstimateFinishTime(double theValue);
  double getJobSimEstimateFinishTime() const;
  bool getJobSimisKilled();
  void setJobSimisKilled(bool isKilled);
  void setJobSimSubmitTime(double theValue);
  double getJobSimSubmitTime() const;
  void setJobSimisBackfilled(bool theValue);
  bool getJobSimisBackfilled() const;
  void setJobSimBackfillingTime(double theValue);
  double getJobSimBackfillingTime() const;
  void setJobSimWaitTime(double theValue);
  double getJobSimWaitTime() const;
  void setJobSimSLD(double theValue);
  double getJobSimSLD() const;
  void setJobSimBSLD(double theValue);
  double getJobSimBSLD() const;
  void setJobSimDeadlineMissed(bool theValue);
  bool getjobSimDeadlineMissed() const;
  void setUserRuntimeEstimate(double theValue);
  double getUserRuntimeEstimate() const;
  void setJobSimCompleted(bool theValue);
  bool getJobSimCompleted() const;
  void setOldPenalizedRunTime(const double& theValue);
  double getOldPenalizedRunTime() const;
  void setJobSimCenter(const string& theValue);
  string getJobSimCenter() const;	
  void setPenalizedRunTimeReal(double theValue);
  double getPenalizedRunTimeReal() const;
  void setOldPenalizedRunTimeReal(double theValue);
  double getOldPenalizedRunTimeReal() const;
  void setJobSimStatus(const job_status_t& theValue);
  job_status_t getJobSimStatus() const;
  void setJobSimLastSimStartTime(double theValue);
  double getJobSimLastSimStartTime() const;
  void setOriginalRequestedTime(double theValue);
  double getOriginalRequestedTime() const;	
  string getAttributeValue(string attribute);
  void setJobSimEstimatedShadow(double theValue);
  double getJobSimEstimatedShadow() const;
  void setAllocatedWith(const RS_policy_type_t& theValue);
  RS_policy_type_t getallocatedWith() const;	
  void setRuntimePrediction(double theValue);
  double getRuntimePrediction() const;	
  void setWaitTimePrediction ( double theValue );
  double getWaitTimePrediction() const;
  void setMonetaryCost ( double theValue );
  double getMonetaryCost() const;
  void setPredictedMonetaryCost ( double theValue );
  double getPredictedMonetaryCost() const;
  void setMaximumAllowedCost ( double theValue );
  double getMaximumAllowedCost() const;
  void setJobSimEstimatedSTWithFF ( double theValue );
  double getJobSimEstimatedSTWithFF() const;
  void setJobSimBSLDPRT ( double theValue );
  double getJobSimBSLDPRT() const;
  void setDisckUsed ( double theValue );
  double getDisckUsed() const;
  bool getRunsAtReducedFrequency();
  void setRunsAtReducedFrequency(bool ind);
  double getRunsAtFrequency();
  void setRunsAtFrequency(double freq);
  
	

private:

  jobMapBucketPenalizedTimes_t jobPenalties;

  //Variables related to the job and the workload 
  //Be aware that for instance the WaitTime may differ in the simulation
  int JobNumber; /**< a counter field, starting from 1.*/
  double SubmitTime; /**< in seconds. The earliest time the log refers to is zero, and is the submittal time the of the first job. The lines in the log are sorted by ascending submittal times. In makes sense for jobs to also be numbered in this order.*/
  double WaitTime; /**< in seconds. The difference between the job's submit time and the time at which it actually began to run. Naturally, this is only relevant to real logs, not to models.*/
  double RunTime; /**< in seconds. The wall clock time the job was running (end time minus start time).
We decided to use ``wait time'' and ``run time'' instead of the equivalent ``start time'' and ``end time'' because they are directly attributable to the scheduler and application, and are more suitable for models where only the run time is relevant.
Note that when values are rounded to an integral number of seconds (as often happens in logs) a run time of 0 is possible and means the job ran for less than 0.5 seconds. On the other hand it is permissable to use floating point values for time fields.*/
  int NumberProcessors; /**< an integer. In most cases this is also the number of processors the job uses; if the job does not use all of them, we typically don't know about it.*/
  double AvgCPUTimeUsed; /**< both user and system, in seconds. This is the average over all processors of the CPU time used, and may therefore be smaller than the wall clock runtime. If a log contains the total CPU time used by all the processors, it is divided by the number of allocated processors to derive the average. */
  double UsedMemory; /**< in kilobytes. This is again the average per processor.*/
  int RequestedProcessors; /**< */
  double RequestedTime; /**<  This can be either runtime (measured in wallclock seconds), or average CPU time per processor (also in seconds) -- the exact meaning is determined by a header comment. In many logs this field is used for the user runtime estimate (or upper bound) used in backfilling. If a log contains a request for total CPU time, it is divided by the number of requested processors.*/
  double OriginalRequestedTime; /**< This contains the original requested time that was specified in the original worklog, in the case that the scheduler is using prediction, the RequestedTime may have a prediction in this case, this variable will hold the orginal stimation. If -1 means that the orginal estimation is set in the RequestedTime variable */
  double RequestedMemory; /**< again kilobytes per processor.*/
  job_status_t status ; /**< Status 1 if the job was completed, 0 if it failed, and 5 if cancelled. If information about chekcpointing or swapping is included, other values are also possible. See usage note below. This field is meaningless for models, so would be -1.*/
  int UserID; /**< a natural number, between one and the number of different users.*/
  int GroupID; /**< a natural number, between one and the number of different groups. Some systems control resource usage by groups rather than by individual users*/
  int Executable ; /**< a natural number, between one and the number of different applications appearing in the workload. in some logs, this might represent a script file used to run jobs rather than the executable directly; this should be noted in a header comment.*/
  int QueueNumber; /**< a natural number, between one and the number of different queues in the system. The nature of the system's queues should be explained in a header comment. This field is where batch and interactive jobs should be differentiated: we suggest the convention of denoting interactive jobs by 0.*/
  int PartitionNumber; /**< a natural number, between one and the number of different partitions in the systems. The nature of the system's partitions should be explained in a header comment. For example, it is possible to use partition numbers to identify which machine in a cluster was used.*/
  int PrecedingJobNumber; /**< this is the number of a previous job in the workload, such that the current job can only start after the termination of this preceding job. Together with the next field, this allows the workload to include feedback as described below. */
  int ThinkTimePrecedingJob; /**< this is the number of seconds that should elapse between the termination of the preceding job and the submittal of this one.*/
 
  double BWMemoryUsed; /**< contains the average of MB/Sec of the Memory BW used by the job */
  double MemoryUsed; /**< contains the average of MB of memory used by the job*/
  double BWEthernedUsed; /**< contains the average of MB/Sec of the Etherned BW used by the job (GPFS)*/
  double BWNetworkUsed; /**<  contains the average of MB/Sec of the Networkd BW used by the job  */
  double DisckUsed; /** contains the average amount of disk that the application requires per unit time */
    
  //Variables realted simulation
  double JobSimSubmitTime; /**< The submit time job in the simulation  */
  double JobSimStartTime; /**< The job startime in the simulation*/
  double JobSimFinishTime; /**< The job finish time for the simulation*/
  double JobSimWaitTime; /**< The job wait time for the simulation*/
  double JobSimEstimateFinishTime; /**< The job estimate finish time in the simulation */
  bool JobSimisKilled;/**< Bool that indicates if the job has been killed in the simulation*/
  bool JobSimisBackfilled; /**<  Bool that indicates if the job has been backfilled in the simulation*/
  double JobSimBackfillingTime; /**< If has been backfilled, when*/
  double JobSimBSLD;/**< The bounded slowdown for the job in the simulation*/
  double JobSimSLD;/**< The slowdown for the job in the simulation*/
  bool JobSimDeadlineMissed; /**< If the job has reached the deadline in the simulation*/
  bool JobSimCompleted;/**< If the job has completed or not */
  string JobSimCenter;/**< The center where the job has been backfilled */
  job_status_t JobSimStatus;/**< The status for the job in the simulation*/
  
  double JobSimLastSimStartTime ; /**<  contains the last start time assigned to the job */
  
  vector<int> UsedCpus; /**< Contains the list of all the cpus used by the processor */
  double PenalizedRunTime;  /**< Contains the penalized runtime for the job due to the resource consumption collisions with other jobs - but related to the requested runtime */
  double OldPenalizedRunTime; /**< Contains the penalized runtime previous to the current PenalizedRunTime;*/
  double OldPenalizedRunTimeReal;
  double PenalizedRunTimeReal;  /**< Contains the penalized runtime for the real runtime */
  int startBucket;
  int lastBucket;
  
  double JobSimEstimatedShadow; /**< This variable is set when the job has been reserved, and indicates the amount of time that the job will be in the reservation position before start to run */
  
  double UserRuntimeEstimate; /**< initially will be the requested time */
  
  RS_policy_type_t allocatedWith; /**< This variable indicates with which allocation policy the job has been allocated to the processors */
  
  double RuntimePrediction; /**< This variable indicates the runtime that has been predicted by the prediction system in case it is used */
  double WaitTimePrediction; /**< This vairable indicates the wattime that has been predicted for the job either for the prediction system or by the local scheduler */


  double MonetaryCost; /**< This variable indicates the amount of money that its execution has caused */
  double PredictedMonetaryCost; /**< This variable indicates the amount of money that was estimated for the job */
  double MaximumAllowedCost; /**< indicates the maximum amount of money that users wants to spent in the job execution*/

  double JobSimEstimatedSTWithFF; /**< indicates the estimated time that the job would have in case it would be allocated with the FF RSP - this field will be fullfilled only in that the policy used would be the LessConsume or LessConsumeTh */

  double JobSimBSLDPRT; /**< This is the Bounded slowdown of the job considering the penalized runtime */
  
  bool RunsAtReducedFrequency; /**< Indicates wether the job is slowdowned   */

  bool RunEqualsEstimate;  /**<  Indicates to use real runtimes as estimates i.e. requested time  */

  double frequency; /**<  Frequency at which job runs (set only if it is backfilled )  */
};

}

#endif
