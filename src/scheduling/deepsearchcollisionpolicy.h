#ifndef SIMULATORDEEPSEARCHCOLLISIONPOLICY_H
#define SIMULATORDEEPSEARCHCOLLISIONPOLICY_H

#include <scheduling/schedulingpolicy.h>
#include <scheduling/virtualanalogicalrtable.h>
#include <utils/fcfsjobqueue.h>
#include <utils/estimatefinishjobqueue.h>
#include <utils/realfinishjobqueue.h>
#include <statistics/average.h>
#include <utils/paravertrace.h>

#include <list>
#include <vector>


using namespace Statistics;
using namespace Utils;
using namespace std;
using std::list;
using std::vector;

namespace Simulator {

/**
* @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

/** An ordered list of jobs that indidcates the order in which the job assignedment should be treated */
typedef list<Job*> Outcome;

/** A vector of possible outcomes */
typedef vector<Outcome*> Outcomes;

/** This function implements the DeepSearchCollision policy. This policy is currently under research and its continously developed. So it may occurs that the behaviour of this policy is not as the user whould expected. */
class DeepSearchCollisionPolicy : public SchedulingPolicy{


public:
  DeepSearchCollisionPolicy(ArchitectureConfiguration* theValue,Log* log,double globaltime, metric_t metrictype, statistic_t statisticType);
  ~DeepSearchCollisionPolicy();
    
  /* iherited functions from the SchedulingPolicy class */  
  virtual void jobAdd2waitq(Job* job); 
  virtual void jobWaitq2RT(Job* job); 
  virtual void jobRemoveFromRT(Job* job); 
  virtual Job* jobChoose(); 
  virtual void jobStart(Job* job); 
  virtual  double getJobsIntheWQ();
  virtual double getLeftWork();
  virtual double getJobsIntheRQ();
  virtual Metric* EstimatePerformanceResponse(metric_t MetricTpye,Job* job);
  
  VirtualAnalogicalJobAllocation* MakeVirtualAllocation(AnalogicalResertavionTable* rtable,Outcome* toAllocate,double time);
  Outcomes* ComputeAllPossibleOutcomes(FCFSJobQueue* queue);  
  
  void setGenerateAllParaversRTables(bool theValue);
  bool getgenerateAllParaversRTables() const;
  void setParaverOutPutDir(const string& theValue);
  string getparaverOutPutDir() const;
  void setParaverTrace(const string& theValue);
  string getparaverTrace() const;
  void setArchitectureType(const architecture_type_t& theValue);
  architecture_type_t getarchitectureType() const;	
  
  virtual void setJobList(map< int, Job * >* theValue); //we have to forward the list to the virtual analogical reservation table	
  
private:  
  FCFSJobQueue waitQueue; /**< wait queue job list */  
  EstimateFinishJobQueue estimateFiniQueue; /**< runing queue job list ordered by estimated finish time*/  
  RealFinishJobQueue realFiniQueue; /**< runing queue job list ordered by real finish time*/  
  vector<AnalogicalBucket*> CPUMapping; /** current allocation to job cpu*/  
  double lastAllocationStart; /**< we force the FCFS using the last time used - the current job can not start before the previous allocated one*/  
  metric_t metricType; /**< the metric to optimize when computing the penalties*/
  statistic_t statisticType; /**< the metric and statistic to optimize when computing the penalties */ 
  
  /* PARAVER STUFF */
  bool generateAllParaversRTables; /**< indicates if all the intermediate paraver trace files must be generated .. this is basically for debug purposes .. be aware becasue for each job allocation a paraver trace file will be generated !*/
  string paraverOutPutDir; /**< indicates the dir where all the paravertraces have to be stored */         
  string paraverTrace; /**< indicates the prefix for all the paravertrace name */         
  architecture_type_t architectureType; /**< indicates the architecture type being simulated */         
  int NumberOfJobs; /**< indicates the number of jobs being simulated */         
  int paraverTraceNumber;  /**< indicates the last number of trace generated (it is incremented in each paravertrace dump)*/         


};

}

#endif
