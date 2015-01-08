#include <utils/jobqueue.h>
namespace Utils {

/***************************
  Constructor and destructors
***************************/

/**
 * The default constructor for the class
 */
JobQueue::JobQueue()
{
  this->jobs = 0;
}

/**
 * The default destructor for the class 
 */
JobQueue::~JobQueue()
{
}


/**
 * Returns the number of jobs that are queued in the queue 
 * @return An integer containing the number of queued jobs 
 */
int JobQueue::getjobs() const
{
  return jobs;
}


/**
 * Sets the number of jobs queued in the queue 
 * @param theValue The number of queued jobs 
 */
void JobQueue::setJobs(const int& theValue)
{
  jobs = theValue;
}

}

