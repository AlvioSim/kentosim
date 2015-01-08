#ifndef SIMULATOREASYSCHEDULINGPOLICY_H
#define SIMULATOREASYSCHEDULINGPOLICY_H

#include <scheduling/analogicalschedulingpolicy.h>
#include <scheduling/analogicalresertaviontable.h>
#include <utils/ruajobqueue.h>
#include <statistics/statistics.h>

enum Backfilling_variant_t
{
     EASY_BACKFILLING = 0,
     SJF_BACKFILLING,
     LXWF_BACKFILLING,
     RUA_BACKFILLING,
     POWER_AWARE_BACKFILLING,
     OTHER_BACKFILLING
     
};

namespace Simulator {

/**
* @author Francesc Guim,C6-E201,93 401 16 50,
*/

typedef pair<Job*,bool> pairJobBool;

/** * This class implements the commonlty kown backfilling scheduling policies. Almost all the proposed Backfilling policies can be used, different properties for the wait queue and backfilling queue are modeled (SJF, LXWF and FCFS) and different number of reservations can be also specified
*/
class EASYSchedulingPolicy : public AnalogicalSchedulingPolicy{
public:
  EASYSchedulingPolicy();
  virtual ~EASYSchedulingPolicy();
  EASYSchedulingPolicy(ArchitectureConfiguration* theValue,Log* log,double globaltime,Backfilling_variant_t backfillingType);

  /* iherited functions from the SchedulingPolicy class */  
  virtual void jobAdd2waitq(Job* job); //Adds a given job to the wait queue, usually when it arrives to the system
  virtual void jobWaitq2RT(Job* job); //Moves a job from teh wait queue to the reservation table, using an specified set of resources
  virtual void jobRemoveFromRT(Job* job); //Removes a job from the RT
  virtual Job* jobChoose(); //Chooses the more appropiate job to run 
  virtual void jobStart(Job* job); //Starts a job    
  virtual  double getBackfilledJobs();
  virtual double getLeftWork();
  virtual double getJobsIntheRQ();
  virtual Metric* EstimatePerformanceResponse(metric_t MetricTpye,Job* job);
  virtual void allocateJob(Job* job, AnalogicalJobAllocation* allocation); 
  
  virtual void JobOverstimated(Job* job); //this function triggers when the job has been overstimated  the reservation table has to be recomputed. We should reschedule all the jobs due to may be the current schedule is not as good as expected. We have defined it as a virtual 
  
  /* sets and gets */
  void setNumberOfReservations(const int& theValue);
  int getnumberOfReservations() const;
  void setAllocateAllJobs(bool theValue);
  bool getallocateAllJobs() const;
  void setTryToBackfillAllways(bool theValue);
  bool gettryToBackfillAllways() const;
	
	  

protected:
  virtual  bool backfillJob(Job* job); 
  virtual void updateLastStartTimeRT();  
  virtual void resetJobSchedule(Job* job); 
  virtual void makeJobReservation(Job* job); 
  virtual Job* pushNextJobReservation(); 
  virtual void reevaluateScheduling(); 
  virtual void resetSchedulingAllQueuedJobs();
  virtual void rescheduleAllQueuedJobs();
  
  void UpdateShadowForBackfill(); 
  bool HaveHintToBeBackfilled(Job* job); 
  
  void deleteJobsMap(vector<Job*>* toDelete,map<Job*,bool>* MapContainer); 

  
  int numberOfReservations; /**< This fixes the number of jobs that will be allocated in the reservation table */    
  int numberOfJobsInTheRT; /**< The number of jobs that are currently in the reservation table */
  
  StartTimeJobQueue reservationTableQueue; /**< is a queue ordered by the starttime of the job. This queue will contain those jobs that have been queued but that have not been allocated to the rtable once a job is removed from the RT one from this queue should be moved in the RT*/
  JobQueue* noReservedJobs; /**< The queue ordered by the submission time of the job.*/
  JobQueue* backfillingQueue; /**< The queue ordered by the submission time of the job. */
  
  map<Job*,bool> backfilledJobs;  /**<  It is a hashmap of jobid boolean and indicates when a job has been backfilled or not */

  double lastStartTimeInRT; /**< contains the last start time for the jobs that have been allocated to the reservation table */
 
  
  double realBackfilledJobs; /**< Double that indicates the number of jobs that have started and have been backfilled */
  
  bool allocateAllJobs; /**< if true, all the jobs, included those that can not be backfilled will be allocated in the reservation table (be aware that all those jobs that are not included in the reservation, their allocation may change during the time) */
  bool tryToBackfillAllways; /**< if true all the jobs will be try to be backfilled, otherwise we only try to backfill the job in the case that in the current time, there are enough resources for start it. If its true, the job may be is backfilled although it does not start now, but will be started earlier than the reservation and won't interfer with it. This is usefull when estimatin the waittime for the job. */
  
  
  double ExtraForBackfill; /**<  Number of jobs that can be used for backfilling jobs in the current time */
  double ShadowForBackfill; /**< The maximum duration for the jobs that can be backfilled in the current available free cpus but that exceeds the ExtraForBackfill variable */

  bool Agressive; /**  if it is true we try to backfill at top frequency if it is not possible to backfill at lower one  */
   
};

}

#endif
