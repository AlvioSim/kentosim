#ifndef PREDICTIONPREDDEADLINEMISSMANAGER_H
#define PREDICTIONPREDDEADLINEMISSMANAGER_H

#include <scheduling/job.h>

using namespace Simulator;

namespace PredictionModule {


/** 
@author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

/**	
*--------------------------------------------------------------------------
* This function is invoked when a prediction was missed and returns a
* new (bigger) prediction.
*
* POSTCONDITION:
* that should be imposed by a derived class is that
*   If  : elapsed        <  estimate  &&  ! is_cheater
*   Then: returned_value <= estimate
* Because there is _never_ a reason to believe a scheduler will violate
* the backfilling-rule which specifies that a job is killed once its
* estimate is reached. A cheater manager may know the actual runtime
* so there's no need to impose on it this postcondition.
*
* POSTCONDITION:
*   returned_value > elapsed
*
* PRECONDITION:
* This is implied from the first postcondition and from the constraint
* of 'miss_count' below (which says that once the elapsed time exceeds
* the original user estimate, a new count should be started).
*   If  : elapsed    == estimate
*   Then: miss_count == 0
*
* PRECONDITION:
*   If  : elapsed < estimate
*   Then: is_pre_manager
*   Else: is_post_manager
*
* job:
*   The id of the job which missed its deadline.
* elapsed:
*   The time a job is currently running.
* estimate:
*   The original user estimate of the job.
* miss_count:
*   How many times was the job's prediction deadline missed in the past.
 *   NOTE that this is a new count for pre/post managers i.e. the count
*   is reinitialized to zero when a predictor begins to use a post
*   managers.
*------------------------------------------------------------------------- 	
*/
class PredDeadlineMissManager{
public:
  PredDeadlineMissManager();
  virtual ~PredDeadlineMissManager();

  

  PredDeadlineMissManager(bool is_cheater, bool is_pre_manager, bool is_post_manager);

  /* gets and sets methods*/
  void setIsCheater(bool theValue);
  bool getisCheater() const;
  void setIsPostManager(bool theValue);
  bool getisPostManager() const;
  void setIsPreManager(bool theValue);
  bool getisPreManager() const;

  
  /**
   * Function that is called when a job prediction was wrong and a deadline has reach 
   * @param job the job thath has reached 
   * @param elapsed the elapsed runtime of the job
   * @param estimate the estimated time 
   * @param miss_count the number of times that the estimation has been wrong 
   * @return The new estimation
   */
  virtual double deadlineMiss(Job* job, double elapsed, double estimate, int    miss_count) = 0;    

protected:

  /**< A subclass may cheat when giving a new prediction (namely use the actual
  * runtime of the job which is known in advance to the simulator).
  * if this is the case, then the post conditions should be refined as
  * shortly specified.*/
  bool isCheater;
  
  /**< May this class be used to generate new predictions when the previous
  *  prediction is SMALLER than the user estimate? */
  bool isPreManager;

  /**< May this class be used to generate new predictions when the previous
    prediction is BIGGER than or EQUAL the user estimate? */
  bool isPostManager;

    
  void preCond(Job* job, double elapsed, double estimate, int miss_count);
  void postCond(Job* job, double elapsed, double estimate, int miss_count, double ret_val);

};

}

#endif
