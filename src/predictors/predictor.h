#ifndef PREDICTIONPREDICTOR_H
#define PREDICTIONPREDICTOR_H

#include <predictors/preddeadlinemissmanager.h>
#include <scheduling/job.h>
#include <predictors/prediction.h>
#include <utils/log.h>
#include <assert.h>

enum Predictor_type_t
{
     USER_RUNTIME_ESTIMATE = 0,
     HISTORICAL_CLASSIFIER,
     OTHER_PREDICTOR,
};

enum DeadlineMissManager_t
{
  GRADUAL=0,
  FACTOR,
  OTHER_DM_MANAGER,
};

#include <map>

using namespace std;
using std::map;

using namespace Simulator;
using namespace Utils;

namespace PredictionModule {

/**
* @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>	
*/

typedef pair<Job*,int> pairJobInt;

/**
 * this is the base class for all the predictors, all the predictors will follow the same interface the interface for the predictors it's mainly based on the once proposed by Tsafrir in its sim2give simulator  reference his papers when using this module
 */
class Predictor{
public:
  Predictor(Log* log);
  virtual ~Predictor();
 
   /**
   * This function can be used for estimate the performance variable, however the job is not being queued necesarily.
   * @param job The job that has to predicted
   * @return The prediction for the job runtime 
   */
  virtual Prediction* estimateJobPerformance(Job* job) = 0;
 
  /**
   * Invoked immediately upon the job's arrival, when it is placed in the waitqueue. `pred_v' will contain at least one runtime prediction for the given job.
   * @param job The job that has arrived
   * @return The prediction for the job runtime 
   */ 
  virtual Prediction* jobArrived(Job* job) = 0;
  
  /**
   * Invoked when a job is scheduled to run, i.e. when it is moved from thewaitqueue to the runqueue.
   * @param job The job that has started
   * @param time The startime for the job 
   */
  virtual void jobStarted(Job* job, double time       // job's start-time ("now")                             
                           ) = 0;

  
  /**
   * The given RUNNING job has just exceeded its prediction (i.e. the previous
   * prediction was too short).
   * Produce a new prediction to this job (and possibly to other jobs which
   * are effected) using the appropriate missed-deadlines manager ("pre" if
   * the current missed prediction is smaller than the user estimate; "post"
   * otherwise).  
   * Need to generate a new (longer) estimate at least for this job, and
   * possibly to others. This means that after this function is finished,
   * pred_v will contain at least one Pred_t associated with job_id.
   * 
   * @param job The job that has reached the deadline 
   * @param time The time when the deadline has been reached 
   * @return 
   */
  virtual Prediction* jobDeadlineMissed(Job* job,
                                   double     time  // in ("now")                                     
                                   ) = 0;

  /**
   * 
   * The given RUNNING job has just terminated. This information may (or may
   * not) help in producing more accurate predictions for currently waiting or
   * running jobs. The returned prev_v may be empty (which means that this
   * Predictor_t has decided not to change any previous predictions).
   *   
   * @param job The job that has terminated 
   * @param time The time 
   * @param completed If it has completed or not 
   * @return if no nulll means that no prediction have been changed 
   */
  virtual Prediction* jobTerminated(Job* job,    // in
                                double     time,      // in ("now")
                                bool       completed // successful completion?
                                ) = 0;

  //Sets and gets methods
  void setMaxEstimate(double theValue);
  double getmaxEstimate() const;
  void setPostManager(PredDeadlineMissManager* theValue);
  PredDeadlineMissManager* getpostManager() const;
  void setPreManager(PredDeadlineMissManager* theValue);
  PredDeadlineMissManager* getpreManager() const;
  Predictor_type_t gettype() const;
  void setModelIntervalGeneration(const int& theValue);
  int getModelIntervalGeneration() const;
		
  
protected:  
  double maxEstimate; /**< maximal estimate allowed on machine (seconds; a system defined value)*/  
  PredDeadlineMissManager* preManager;/**<  manger for pre-estimate missed deadlines.*/  
  PredDeadlineMissManager* postManager;/**< manger for post-estimate missed deadlines.*/
  Predictor_type_t type;/**< the predictor type*/
  //management for the number of misses for a given job 
  map<Job*,int> missedDeadlineCount;/**< contains the counts that a given jobs has missed its deadline*/ 
  int updateMissedCountJob(Job* job);/**< update the number of misses for the current job*/ 
  
  Log* log; /**< The loggin engine */
  
  int ModelIntervalGeneration; /**< A double indicating the amount of job finished between two different model generations  */
  

};

}

#endif
