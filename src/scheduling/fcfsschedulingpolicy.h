#ifndef SIMULATORFCFSSCHEDULINGPOLICY_H
#define SIMULATORFCFSSCHEDULINGPOLICY_H

#include <scheduling/analogicalschedulingpolicy.h>
#include <scheduling/analogicalresertaviontable.h>
#include <utils/fcfsjobqueue.h>
#include <utils/estimatefinishjobqueue.h>
#include <utils/realfinishjobqueue.h>

namespace Simulator {

typedef pair <int, Job*> pairJob;

/**
* @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>		
*/

/**
* This class implements the more simplest schedulingpolicy the first-come-first-serve
* @see the class schedulingpolicy
*/
class FCFSSchedulingPolicy : public AnalogicalSchedulingPolicy{
public:
  FCFSSchedulingPolicy();;
  FCFSSchedulingPolicy(ArchitectureConfiguration* theValue,Log* log,double globaltime);
  ~FCFSSchedulingPolicy();

  /* iherited functions from the SchedulingPolicy class */  
  virtual void jobAdd2waitq(Job* job); //Adds a given job to the wait queue, usually when it arrives to the system
  virtual void jobWaitq2RT(Job* job); //Moves a job from teh wait queue to the reservation table, using an specified set of resources
  virtual void jobRemoveFromRT(Job* job); //Removes a job from the RT
  virtual Job* jobChoose(); //Chooses the more appropiate job to run 
  virtual void jobStart(Job* job); //Starts a job    
  virtual  double getJobsIntheWQ();
  virtual double getLeftWork();
  virtual double getJobsIntheRQ();
  virtual Metric* EstimatePerformanceResponse(metric_t MetricTpye,Job* job);
  virtual void setGlobalTime(double globaltime);
  
  
  virtual void allocateJob(Job* job, AnalogicalJobAllocation* allocation);
  
  double lastAllowedStartTime; /**< Indicates the last allowed start time for the job  for do not violate the FCFS restriction */
  
private:
  
  FCFSJobQueue waitQueue;/**< The wait queue job list*/  
  RealFinishJobQueue realFiniQueue; /**< The runing queue job list ordered by real finish time*/
 
};

}

#endif
