#include <utils/estimatefinishjobqueue.h>

namespace Utils {

/**
 * The default constructor for the class
 */
EstimateFinishJobQueue::EstimateFinishJobQueue()
{
 this->jobs = 0;
}

/**
 * The default destructor for the class 
 */
EstimateFinishJobQueue::~EstimateFinishJobQueue()
{
}

/***************************
  Main methods
***************************/

/**
 * Function used for insert a job to the queue
 * @param job The job to be inserted 
 */
void EstimateFinishJobQueue::insert(Job* job)
{ 
  bool inserted = this->queue.insert(job).second;
  this->jobs++;
  int queue_s = this->queue.size();
  
  assert(inserted && queue_s == this->jobs); /*checking the sanity of the job queue*/
}

/**
 * Function used for getting the next job in the current position, if null means that there are no more jobs the jobs will be returned following the given criteria followed by the queue
 * @return The next job in the iterator 
 */
void EstimateFinishJobQueue::erase(Job* job)
{
  bool deleted = this->queue.erase(job);
  
  this->jobs--;
  int queue_s = this->queue.size();
  
  assert(deleted && queue_s == this->jobs); /*checking the sanity of the job queue*/
}

/**
 * Initialize the iterator of the queue and returns the first job of the queue
 * @return The first job of the queue 
 */
Job* EstimateFinishJobQueue::begin()
{
  this->currentIterator = this->queue.begin();
  
  if(this->currentIterator != this->queue.end())
   return (Job*) *this->currentIterator;
  else
   return NULL;
}

/**
 * Function used for delete a given job from the queue
 * @param job The job to be deleted 
 */
Job* EstimateFinishJobQueue::next()
{
  this->currentIterator++;
  
  if(this->currentIterator != this->queue.end())
   return (Job*) *this->currentIterator;
  else
   return NULL;
  
}


/**
 * Function used for get the headJob of the queue
 * @return The head job of the queue 
 */
Job* EstimateFinishJobQueue::headJob()
{
  EstimatedFinishQueue::iterator it = this->queue.begin();
  
  if(it == this->queue.end())
    return NULL;    
  else
    return (Job*) *it;
}

/**
 * Function that retruns true if the job is inserted in the queue false otherwise
 * @param job The job that has to be checked 
 * @return A bool containing if the job is inserted 
 */
bool EstimateFinishJobQueue::contains(Job* job)
{
  EstimatedFinishQueue::iterator it = this->queue.find(job);
  
  return it != this->queue.end();

}

/**
 * Function that deletes the current element pointed by the iterator
 */
void EstimateFinishJobQueue::deleteCurrent()
{
  if(/*this->currentIterator == NULL || */this->currentIterator == this->queue.end())
   return;
  
  this->queue.erase(this->currentIterator);   
   
  this->jobs--;
  int queue_s = this->queue.size();
  
  assert(queue_s == this->jobs); /*checking the sanity of the job queue*/ 

   
}

}
