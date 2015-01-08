#include <utils/ruajobqueue.h>
#include <algorithm>

namespace Utils {

/**
 * The default constructor for the class
 */
RUAJobQueue::RUAJobQueue()
{
 this->jobs = 0;
}

/**
 * The default destructor for the class 
 */
RUAJobQueue::~RUAJobQueue()
{
}

/**
 * This function reorders all the jobs that are in the queue, this function has been mainly designed for when to elements for the jobs 
 * @return True if the reorder has been done correctly 
 */
bool RUAJobQueue::updateJobOrders()
{
  //first we reset the queue , not sure what's up if the old penalty has been modified ...
  this->queue.clear();

  for(vector<Job*>::iterator it = this->vectJobs.begin();it != this->vectJobs.end();++it)
  {
    Job* job = *it;
    this->queue.insert(job);
  }
}

/**
 * Function used for insert a job to the queue
 * @param job The job to be inserted 
 */
void RUAJobQueue::insert(Job* job)
{ 
  //we must do it. The main problem is that depending on the previous simulation steps the OldPenalizedRuntime could be changed..thus violating the weak strick ordering function of the STL..
  updateJobOrders();


  bool inserted = this->queue.insert(job).second;
  this->jobs++;
  int queue_s = this->queue.size();
  
  assert(inserted && queue_s == this->jobs); /*checking the sanity of the job queue*/

  //we add the job to the backend 
  this->vectJobs.push_back(job);
}

/**
 * Function used for delete a given job from the queue
 * @param job The job to be deleted 
 */
void RUAJobQueue::erase(Job* job)
{
  //we must do it. The main problem is that depending on the previous simulation steps the OldPenalizedRuntime could be changed..thus violating the weak strick ordering function of the STL..
  updateJobOrders();

  //first we safely remove the job from the vector
  vector<Job*>::iterator it = find(this->vectJobs.begin(), this->vectJobs.end(),job);
  this->vectJobs.erase(it);

  bool deleted = this->queue.erase(job);
  
  this->jobs--;
  int queue_s = this->queue.size();
  
  assert(deleted && queue_s == this->jobs); /*checking the sanity of the job queue*/
}

/**
 * Function used for getting the next job in the current position, if null means that there are no more jobs the jobs will be returned following the given criteria followed by the queue
 * @return The next job in the iterator 
 */
Job* RUAJobQueue::next()
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
Job* RUAJobQueue::begin()
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
Job* RUAJobQueue::headJob()
{
  RUAQueue::iterator it = this->queue.begin();
  
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
bool RUAJobQueue::contains(Job* job)
{
  RUAQueue::iterator it = this->queue.find(job);
  
  return it != this->queue.end();

}

/**
 * Function that deletes the current element pointed by the iterator
 */
void RUAJobQueue::deleteCurrent()
{
  //first we safely remove the job from the vector
  Job* job = *this->currentIterator;
  vector<Job*>::iterator it = find(this->vectJobs.begin(), this->vectJobs.end(),job);
  this->vectJobs.erase(it);


  if(/*this->currentIterator == NULL ||*/ this->currentIterator == this->queue.end())
   return;
  
  this->queue.erase(this->currentIterator);   
  
  this->jobs--;
  int queue_s = this->queue.size();
  
  assert(queue_s == this->jobs); /*checking the sanity of the job queue*/

}


}
