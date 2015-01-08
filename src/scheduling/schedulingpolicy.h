#ifndef SCHEDULINGPOLICY_H
#define SCHEDULINGPOLICY_H

#include <utils/architectureconfiguration.h>
#include <utils/configurationfile.h>

#include <scheduling/job.h>
#include <scheduling/reservationtable.h>
#include <utils/log.h>
#include <utils/paravertrace.h>
#include <scheduling/metric.h>

#include <utils/jobqueue.h>
#include <utils/starttimejobqueue.h>
#include <utils/estimatefinishjobqueue.h>
#include <utils/realfinishjobqueue.h>

#include <scheduling/resourceusage.h>

#include <scheduling/powerconsumption.h>

/**
* @author Francesc Guim,C6-E201,93 401 16 50,
*/

using namespace Utils;

namespace Simulator {


class Simulation;

/** 
* This class is an abstract class that defines all the functionalities that a scheduling policy has to provide.
* All the subclasses must implement the the pure virtual methods. 
*/
class SchedulingPolicy{
public:
  /* Constructor and destructors */
  SchedulingPolicy();
  virtual ~SchedulingPolicy();
    
  /*Auxiliar functions for scheduling */
  /**
   * Adds a given job to the wait queue, usually when it arrives to the system
   * @param job The job that has to be added to the wait queue
   */
  virtual void jobAdd2waitq(Job* job); 
  
  /**
   * Moves a job from the wait queue to the reservation table, using an specified set of resources
   * @param job The job that has to be movedto the run table 
   */
  virtual void jobWaitq2RT(Job* job) = 0;
  /**
   * Removes a job from the RT
   * @param job The job that has to be removed from the reservationtable
   */
  virtual void jobRemoveFromRT(Job* job);
  /**
   * Chooses the next job to move to the processors 
   * @return A reference to the more appropiate job 
   */
  virtual Job* jobChoose() =  0; //Chooses the more appropiate job to run 
  /**
   * Starts a given job 
   * @param job The job to start 
   */
  virtual void jobStart(Job* job) = 0; //Starts a job


  /** the following methods give information about the current scheduling stuff - note that only the last onces provides an implementation, this is caused due to the information that they provide should be provided by any of the local systems. The last method, EstimatePerformanceResponse, by default implements the common fields that scheduling systems should provide, however each subclass may extend its definition. */


  /**
   * Return the number of backfilled jobs in the current moment
   * @return An integer containing the backfilled jobs 
   */
  virtual double getBackfilledJobs();  
  /**
   * Return the the amount of pending work (area of requested time x requested processors for the queued jobs)
   * @return A integer containing the pending work
   */
  virtual double getLeftWork() = 0;

  /**
   * Return the number of jobs in the run queue 
   * @return A integer containing the number of running jobs 
   */
  virtual double getJobsIntheRQ() = 0;

  /** Virtual implemented methods */
  virtual double getJobsIntheWQ();  
  virtual Metric* EstimatePerformanceResponse(metric_t MetricTpye,Job* job);
  virtual Metric* getStaticSystemInformation(metric_t MetricTpye);
  virtual int getNumberCPUUsed();
   
  void setReservationTable(ReservationTable* theValue);
  ReservationTable* getreservationTable() const;
  void setPrvTrace(ParaverTrace* theValue);
  ParaverTrace* getprvTrace() const;	
  virtual void setJobList(map< int, Job * >* theValue);
  map< int, Job * >* getJobList() const;
  virtual void setSim(Simulation* theValue); //its declared as a virtual due to some policies may do special processing on this action (GridBackfilling) 
  virtual void setGlobalTime(double globaltime); //the same
  Simulation* getsim() const;
  void setArchitecture(ArchitectureConfiguration* theValue);
  ArchitectureConfiguration* getarchitecture() const;	
  void setLog(Log* theValue);
  Log* getlog() const;
  void setEmulateCPUFactor(const double& theValue);
  double getEmulateCPUFactor() const;

  virtual void PostProcessScheduling(); /**< tells to the policy that the job scheduling has finished, at this point the scheduling policy can do all the stuff of post processing  */
  virtual void PreProcessScheduling(); /**< tells to the policy that the job scheduling will start, at this point the policy can do all the necessary preporcessing activities.*/

  void setUseParaver(bool theValue); 
  bool getuseParaver() const;
  void setRS_policyUsed(const RS_policy_type_t& theValue);
  RS_policy_type_t getRS_policyUsed() const;
  void setSchedulingPolicySimulated(const policy_type_t& theValue);
  policy_type_t getschedulingPolicySimulated() const;
  void setMaxAllowedRuntime(double theValue);
  double getmaxAllowedRuntime() const;			
  void setCostAllocationUnit ( double theValue );
  double getCostAllocationUnit() const;
  void setAllocationUnitHour ( double theValue );
  double getAllocationUnitHour() const;
  void updateResourceUsage();

  //Power related stuff
  void setPowerConsumption ( PowerConsumption* theValue );
  PowerConsumption* getpowerConsumption() const;
  int getCPUUsage();
  bool computePower(); 
  void initializationPower(PowerConsumption* powerinf);
  double getenergyConsumedFromPower();
  double computeSavingFromPower();
  virtual void updateEnergyConsumedInPower(double globaltime, Job* job);
  void setPowerAware(bool aware);
  bool getPowerAware();
  void increaseNumberOfReducedModeCPUS(Job * job);
  void decreaseNumberOfReducedModeCPUS(Job * job);
  double getRatio(int jobNumber, double freq);
  void setPreviousTimeToPower(double time);
  void setFirstStartTime(double time);
  void dumpGlobalPowerInformation(double end_time, int usefull);
  
	  
  void setCenterName(const string& theValue);
  string getCenterName() const;  

//protected:
   /*Fields that are used in the SchedulingPolicie entity */
   ArchitectureConfiguration* architecture; /**< Contains a link to the architecture being simulated  */   
   Log* log;/**< log information*/   
   double globaltime; /**< current global time */      
   ReservationTable* reservationTable; /**< the reservation table where the jobs are being scheduled*/   
   ParaverTrace* prvTrace;/**< optionally the scheduling can be stored to the trace*/
   bool useParaver; /**< Indicates if the job events have to be forwarded to the parever trace */
   map<int,Job*>* JobList; /**< The list of jobs that are being scheduled  */
   Simulation* sim; /**< A reference to the simulation */
   double EmulateCPUFactor;/**< indicates if the job runtime has to be multiplied by X , this is the simples way to study the efect of having systems with different performance. The emulate factor is something related to a given simulation. The user can study the impact of modifying the cpu capacity in the configuration architecture file, however, in this case the workload provided as an input for the simulation has to provide the original cpu where the job was submitted. */
   map<Job*,bool> JobInTheSystem; /**< this map contains all the jobs that are currently in the system, they can be either in the wait queue or in the run table */
   double submittedJobs; /**< number of jobs that have been submitted since the time 0 */
   RS_policy_type_t RS_policyUsed; /**< The resource selection policy used by the scheduler for decide where the jobs have to be allocated */
   
   map<Job*,bool> JobProcessed; /**< The jobs that have been already processed */
   
   policy_type_t schedulingPolicySimulated; /**< Indicates which scheduling is being simulated */   
   double maxAllowedRuntime; /**< Indicates the maximum amount of runtime allowed runtime for the submitted jobs, it mainly models the job queue of the local resource, -1 means no limit*/ 

   double CostAllocationUnit; /**< Indicates the amount of money that an allocation unit cost, this will allow the amount of money that the execution of the workload cost */
   double AllocationUnitHour; /**< Indicates the amount of allocations units per hour that center charges */
   
   ResourceUsage currentResourceUsage; /**< Indicates the current usage of the system ! Important .. For have the correct usage in a given time stamp the function - updateResourceUsage has to be called */
   double lastTimeResourceUsageUpdate; /**< Indicates the last time that the globlal resource was updated.. this is mainly to avoid update the currentresoureusage many times while the simulation clock has not changed.. sime the resource usage is the same */

   EstimateFinishJobQueue RunningQueue; /**< The queue that contains the jobs ordered by its estimated finish time */
   JobQueue* waitQueue; /**< The wait queue of the simulation */


    //Derived information about the jobs that have already finished. - this information is stored in order to provide some quality of service information to the brokers 
    double AccumulatedWT; /**<Contains the accumulted wait times for all the jobs */
    double AccumulatedSld; /**< Contains the accumulated slowdown for all the jobs */
    double AccumulatedRespT; /**< Contains the accumulated response time for all the jobs */
    double AccumulatedRT; /**< Contains the accumulated runtime for all the jobs */

    double FinishedJobs; /**< Contains the number of finished Jobs  */

    //Regarding the power consumption in the simulator   
    PowerConsumption* power;
    bool powerAware; /**<  It is true only if power awarness affects scheduling policy i.e. for utilization based saving it is not needed (i.e. when we use the medium modes)*/
    double starttime; /* start time of the first job we are interested in */
    int backfilledReduced;  /*  the number of jobs that are backfilled at medium frequency   */
    double backfilledReducedDemand;

   
    string CenterName; 
};

  

}
#endif
