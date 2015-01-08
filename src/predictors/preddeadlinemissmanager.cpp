#include <predictors/preddeadlinemissmanager.h>
#include <assert.h>

namespace PredictionModule {

/**
 * The default constructor for the manager  
 */
PredDeadlineMissManager::PredDeadlineMissManager()
{
  this->isCheater = false;
  this->isPreManager = true;
  this->isPostManager = true;
}


/**
 * The default destructor for the manager 
 */
PredDeadlineMissManager::~PredDeadlineMissManager()
{
}

/**
 * The more common used constructor
 * @param is_cheater If the deadlineMissManager is cheater 
 * @param is_pre_manager If its a premanager 
 * @param is_post_manager If its a postmanager 
 */
PredDeadlineMissManager::PredDeadlineMissManager(bool is_cheater, bool is_pre_manager, bool is_post_manager)
{
  this->isCheater = is_cheater;
  this->isPreManager = is_pre_manager;
  this->isPostManager = is_post_manager;
}

/**
 * Checks that the precondition for the estimation is correct. Must be invoked at the beginning of deadline_miss().
 * @param job The job about who concerns the estimation
 * @param elapsed The elapsed time for the job
 * @param estimate The estimation for the job 
 * @param miss_count The times that the estimation for the job has been wrong
 */
void PredDeadlineMissManager::preCond(Job* job, double elapsed, double estimate, int miss_count) {
//   if( elapsed == estimate )
//     assert( miss_count==0 );
//   if( elapsed < estimate  )
//     assert( this->isPreManager);
//   else
//     assert( this->isPostManager );
}

/**
 * Checks that postconditions for the prediction and the manager are correct. Must be invoked at the end of deadline_miss().
 * @param job The job about who concerns the estimation
 * @param elapsed The elapsed time for the job
 * @param estimate The estimation for the job 
 * @param miss_count The times that the estimation for the job has been wrong
 * @param ret_val The new estimation 
 */
void PredDeadlineMissManager::postCond(Job* job, double elapsed, double estimate, int miss_count, 
   double ret_val)  {
  
  if( elapsed < estimate )
    assert( ret_val <=estimate || this->isCheater);
   
  assert( ret_val > elapsed );
}


/**
 * Returns true if the job is cheater, false otherwise 
 * @return A bool of if it is cheater
 */
bool PredDeadlineMissManager::getisCheater() const
{
  return isCheater;
}


/**
 * Sets if the manager is cheater or not 
 * @param theValue True if it is cheater
 */
void PredDeadlineMissManager::setIsCheater(bool theValue)
{
  isCheater = theValue;
}


/**
 * Returns true if the manager is a post manager 
 * @return A bool of if it is post manager
 */
bool PredDeadlineMissManager::getisPostManager() const
{
  return isPostManager;
}


/**
 * Sets if the manager is a post manager
 * @param theValue A bool of it is a  post manager 
 */
void PredDeadlineMissManager::setIsPostManager(bool theValue)
{
  isPostManager = theValue;
}


/**
 * Returns if the manager is a pre manager 
 * @return A bool if it is a pre manager 
 */
bool PredDeadlineMissManager::getisPreManager() const
{
  return isPreManager;
}


/**
 * Sets it it is a pre manager 
 * @param theValue A bool if it is a pre manager
 */
void PredDeadlineMissManager::setIsPreManager(bool theValue)
{
  isPreManager = theValue;
}

}
