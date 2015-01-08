#include <utils/estimatedruntimejobqueue.h>
#include <utils/jobqueue.h>

namespace Utils {

/**
 * The default constructor for the class
 */
EstimatedRunTimeJobQueue::EstimatedRunTimeJobQueue() : JobQueue()
{
}

/**
 * The default destructor for the class 
 */
EstimatedRunTimeJobQueue::~EstimatedRunTimeJobQueue()
{
}


/***************************
  Main methods
***************************/

/**
 * Function used for insert a job to the queue
 * @param job The job to be inserted 
 */
void EstimatedRunTimeJobQueue::insert(Job* job)
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
Job* EstimatedRunTimeJobQueue::next()
{
  /* in case we are current to the end */
  if(this->currentIterator == this->queue.end())
    return NULL;

  this->currentIterator++;
  
  if(this->currentIterator != this->queue.end())
   return (Job*) *this->currentIterator;
  else
   return NULL;
  
}

/**
 * Initialize the iterator of the queue and returns the first job of the queue
 * @return The first job of the queue 
 */
Job* EstimatedRunTimeJobQueue::begin()
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
void EstimatedRunTimeJobQueue::erase(Job* job)
{
  bool deleted = this->queue.erase(job);
  
  this->jobs--;
  int queue_s = this->queue.size();

  assert(deleted && queue_s == this->jobs); /*checking the sanity of the job queue*/
}

/**
 * Function used for get the headJob of the queue
 * @return The head job of the queue 
 */
Job* EstimatedRunTimeJobQueue::headJob()
{
  EstRTQueue::iterator it = this->queue.begin();
  
  if(it == this->queue.end())
    return NULL;    
  else
    return (Job*) *it;
}

/**
 * Returns the reference to the queue implemented using the STL set class 
 * @return A reference to the set class 
 */
EstRTQueue* EstimatedRunTimeJobQueue::getQueue()
{
return &this->queue;
}

/**
 * Function that deletes the current element pointed by the iterator
 */
void EstimatedRunTimeJobQueue::deleteCurrent()
{
  if(/*this->currentIterator == NULL ||*/ this->currentIterator == this->queue.end())
   return;
  
  this->queue.erase(this->currentIterator);
  
  this->jobs--;
  int queue_s = this->queue.size();
  
  assert(queue_s == this->jobs); /*checking the sanity of the job queue*/ 
}

/**
 * Function that retruns true if the job is inserted in the queue false otherwise
 * @param job The job that has to be checked 
 * @return A bool containing if the job is inserted 
 */
bool EstimatedRunTimeJobQueue::contains(Job* job)
{
  EstRTQueue::iterator it = this->queue.find(job);
  
  return it != this->queue.end();

}

}
