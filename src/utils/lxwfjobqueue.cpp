#include <utils/lxwfjobqueue.h>

namespace Utils {

/**
 * The default constructor for the class
 */
LXWFJobQueue::LXWFJobQueue()
{
 this->jobs = 0;
}

/**
 * The default destructor for the class 
 */
LXWFJobQueue::~LXWFJobQueue()
{
}

/**
 * Function used for insert a job to the queue
 * @param job The job to be inserted 
 */
void LXWFJobQueue::insert(Job* job)
{ 
  bool inserted = this->queue.insert(job).second;
  this->jobs++;
  int queue_s = this->queue.size();
  
  assert(inserted && queue_s == this->jobs); /*checking the sanity of the job queue*/
}

/**
 * Function used for delete a given job from the queue
 * @param job The job to be deleted 
 */
void LXWFJobQueue::erase(Job* job)
{
  bool deleted = this->queue.erase(job);
  
  this->jobs--;
  int queue_s = this->queue.size();
  
  assert(deleted && queue_s == this->jobs); /*checking the sanity of the job queue*/
}

/**
 * Function used for getting the next job in the current position, if null means that there are no more jobs the jobs will be returned following the given criteria followed by the queue
 * @return The next job in the iterator 
 */
Job* LXWFJobQueue::next()
{
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
Job* LXWFJobQueue::begin()
{
  this->currentIterator = this->queue.begin();
  
  if(this->currentIterator != this->queue.end())
   return (Job*) *this->currentIterator;
  else
   return NULL;
}

/**
 * Function used for get the headJob of the queue
 * @return The head job of the queue 
 */
Job* LXWFJobQueue::headJob()
{
  LXWFQueue::iterator it = this->queue.begin();
  
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
bool LXWFJobQueue::contains(Job* job)
{
  LXWFQueue::iterator it = this->queue.find(job);
  
  return it != this->queue.end();

}

/**
 * Function that deletes the current element pointed by the iterator
 */
void LXWFJobQueue::deleteCurrent()
{
  if(/*this->currentIterator == NULL ||*/ this->currentIterator == this->queue.end())
   return;
  
  this->queue.erase(this->currentIterator);   
  
  this->jobs--;
  int queue_s = this->queue.size();
  
  assert(queue_s == this->jobs); /*checking the sanity of the job queue*/
   
}

}
