#ifndef PREDICTIONMODULEUSERRUNTIMEESTIMATEPRED_H
#define PREDICTIONMODULEUSERRUNTIMEESTIMATEPRED_H

#include <predictors/predictor.h>


namespace PredictionModule {

/**
*  @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

/**
* This class implements the simplest predictor. It returns the user runtime estimation.
*/
class UserRuntimeEstimatePred  : public Predictor {
public:
  UserRuntimeEstimatePred(Log* log);
  ~UserRuntimeEstimatePred();

  virtual Prediction* jobArrived(Job* job) ;
  virtual void jobStarted(Job* job, double time);
  virtual Prediction* jobDeadlineMissed(Job* job, double     time);
  virtual Prediction* jobTerminated(Job* job, double     time, bool       completed);
  virtual Prediction* estimateJobPerformance(Job* job);
 
  
};

}

#endif
