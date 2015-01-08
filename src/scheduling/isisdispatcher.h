#ifndef SIMULATORISISDISPATCHER_H
#define SIMULATORISISDISPATCHER_H

#include <scheduling/schedulingpolicy.h>
#include <utils/fcfsjobqueue.h>
#include <predictors/predictor.h>
#include <statistics/processuniform.h>

#include <list>
#include <vector>
#include <map>

using namespace std;
using namespace PredictionModule;
using namespace Statistics;
using std::list;
using std::vector;
using std::map;

namespace Simulator {

/**
* @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

typedef pair<Job*,SchedulingPolicy*>PairSubmission;
typedef pair<SchedulingPolicy*,int> PairSubmitedJobs;

/** 
* Implementes the isis-dispatcher policy The main objective of the discussed policy is to provide an scheduling entity that will be used by the user or any other component (f.i: components that requires self-scheduling or self-orchestrat
ion) for decide where to submit their jobs. As we have already mentioned in the preceeding sections, the dispachers chose the center based on a set of metrics that each center should provide. In this paper we have evaluated two different se
t of metrics: run time metrics concerning the status of the center (like the number of queued jobs), and the wait time prediction metric. Providing such information should not be complicated for t
he centers since it could be an external component with only few type of interactions with the local resource managers and scheduling systems.

In the rest of the section we presented the submission algorithm that the local dispacher uses for select the most appropiate center for submit the job, the task asignament alogrithm policies that
 have been evaluated in this paper, and finally, the prediction model that has been used for evaluated the asignament policy proposed \textit{Less-WaitTime}.
*/
class ISISDispatcherPolicy : public SchedulingPolicy{
public:
  ISISDispatcherPolicy();
  ISISDispatcherPolicy(Log* log,bool UsePredictionService,Predictor* predictionService);
  ~ISISDispatcherPolicy();
  
  /* inherited methods from the scheduling abstract policy */ 
  virtual void jobAdd2waitq(Job* job); //Adds a given job to the wait queue, usually when it arrives to the system
  virtual void jobWaitq2RT(Job* job); //Moves a job from teh wait queue to the reservation table, using an specified set of resources
  virtual void jobRemoveFromRT(Job* job); //Removes a job from the RT
  virtual Job* jobChoose(); //Chooses the more appropiate job to run 
  virtual void jobStart(Job* job); //Starts a job    
  virtual  double getJobsIntheWQ();
  virtual  double getBackfilledJobs();
  virtual double getLeftWork();
  virtual double getJobsIntheRQ();
  virtual Metric* EstimatePerformanceResponse(metric_t MetricTpye,Job* job); 
  virtual int  getNumberCPUSUsed();
  
  virtual void setSim(Simulation* theValue); //the policy has to propagate the simulator link to all the instantiated policies
  virtual void setGlobalTime(double globaltime); //the same
  
  void setISISDispatcherMetric2Optimize(const metric_t& theValue);
  metric_t getISISDispatcherMetric2Optimize() const;
  
  void setISISDispatcherArchitectures(vector< ArchitectureConfiguration * >* theValue);
  vector< ArchitectureConfiguration * >* getISISDispatcherArchitectures() const;
  
  void setISISDispatcherParaverTracesName(vector< string >* theValue);
  vector< string >* getISISDispatcherParaverTracesName() const;
  
  void setISISDispatcherPolicies(vector< SchedulingPolicy * >* theValue);
  vector< SchedulingPolicy * >* getISISDispatcherPolicies() const;
  
  void setISISDispatcherStatistic2Optimize(const statistic_t& theValue);
  statistic_t getISISDispatcherStatistic2Optimize() const;	
  
  void setISISDispatcherCentersName(vector< string >* theValue);
  vector< string >* getISISDispatcherCentersName() const;
  
  void setISISDispatcherEmulateCPUFactor(vector< double >* theValue);
  vector< double >* getISISDispatcherEmulateCPUFactor() const;	

  virtual void PostProcessScheduling(); //the post processing function will generate all the paraver traces 
 
  void setAmountOfKnownCenters ( int theValue );
  int getamountOfKnownCenters();
	


private:
  vector<SchedulingPolicy*>* ISISDispatcherPolicies; /**<  Vector with all the policies that are simulated in the grid */
  vector<ArchitectureConfiguration*>* ISISDispatcherArchitectures;/**<  Vector with all the arechitectures used by each policy */
  vector<string>* ISISDispatcherParaverTracesName;/**<  The paraver trace file names of each center if required to be generated */
  vector<string>* ISISDispatcherCentersName;/**<  The name of each center being simulated (each policy and architecture belong to one center )*/
  vector<double>* ISISDispatcherEmulateCPUFactor;/**<  The cpu factora that has to be applied to each center in the simulation*/
  
  map<SchedulingPolicy*, int> SubmittedJobs;/**<  The number of submitted jobs to the given policy */
  
  metric_t ISISDispatcherMetric2Optimize;/**<  The metric that has to be optimized in the overall policy */
  statistic_t ISISDispatcherStatistic2Optimize;/**<  The statistic to use for compute the previous metric */
  
  
  JobQueue* waitQueue;/**< the wait queue */
  map<Job*,SchedulingPolicy*> submissions;/**<  Hasmap that indicates where the jobs have been submitted*/
  
  bool UsePredictionService; /**< Indicates if the prediction service has to be used for estimate the job runtime in a given center */
  Predictor* predictionService; /**< Contains the prediction service that will be used for estimate the job. */

  int amountOfKnownCenters; /**< Indicates the amount of centers that are known by the isis dispatcher, this is the first approach to model the amount of information that the prediction service can provide. */
  
  ProcessUniform* ADSRandom; /** This variable is used to uniformly choose the different centers that are choosen */
};

}

#endif
