#ifndef SIMULATORMETABROKERINGSYSTEM_H
#define SIMULATORMETABROKERINGSYSTEM_H

#include <scheduling/schedulingpolicy.h>
#include <utils/fcfsjobqueue.h>
#include <predictors/predictor.h>

#include <string.h>

#include <scheduling/jobrequirement.h>

#include <list>
#include <vector>
#include <map>

using namespace std;
using namespace PredictionModule;
using std::list;
using std::vector;
using std::map;

namespace Simulator {

/**
	@author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/
/**
* This class models a meta-brokering environament it is composed by several virtual organitzations.
*/

typedef pair<string,SchedulingPolicy*>pairName;
typedef pair<Job*,SchedulingPolicy*>PairSubmission;

class MetaBrokeringSystem :  public SchedulingPolicy{
public:
  MetaBrokeringSystem();
  MetaBrokeringSystem(Log* log,bool UsePredictionService,Predictor* predictionService);
  ~MetaBrokeringSystem();

  void setVirtualOrganitzations ( vector< SchedulingPolicy * >* theValue );
  vector< SchedulingPolicy * >* getVirtualOrganitzations() const;

  virtual void jobAdd2waitq(Job* job); //Adds a given job to the wait queue, usually when it arrives to the system
  virtual void jobWaitq2RT(Job* job); //Moves a job from teh wait queue to the reservation table, using an specified set of resources
  virtual void jobRemoveFromRT(Job* job); //Removes a job from the RT
  virtual Job* jobChoose(); //Chooses the more appropiate job to run 
  virtual void jobStart(Job* job); //Starts a job    
  virtual double getJobsIntheWQ();
  virtual double getBackfilledJobs();
  virtual double getLeftWork();
  virtual double getJobsIntheRQ();
  virtual Metric* EstimatePerformanceResponse(metric_t MetricTpye,Job* job); 
  virtual int  getNumberCPUSUsed();
  
  virtual void setSim(Simulation* theValue); //the policy has to propagate the simulator link to all the instantiated policies
  virtual void setGlobalTime(double globaltime); //the same

  virtual void PostProcessScheduling(); //the post processing function will generate all the paraver traces 

  void jobForwarded(Job* job,SchedulingPolicy* VO);
  void setVONames ( const vector< string >& theValue );
  vector< string > getVONames() const;
	

private:
  vector<SchedulingPolicy*>* VirtualOrganitzations; /**<  Vector with all the virtual organitzations  */
  map<string,SchedulingPolicy*> VONamesMapping; /**<  A mapping for the different VirtualOrganitzation names and its implementation */
 

  JobQueue* waitQueue;/**< the wait queue */
  map<Job*,SchedulingPolicy*> submissions;/**<  Hasmap that indicates where the jobs have been submitted*/
  
  bool UsePredictionService; /** Indicates if the prediction service has to be used for estimate the job runtime in a given center */
  Predictor* predictionService; /** Contains the prediction service that will be used for estimate the job. */

  map<SchedulingPolicy*, int> SubmittedJobs;/**<  The number of submitted jobs to the given policy */

  vector<string> VONames; /**< The list of the different virtual organitzations  */
};

}

#endif
