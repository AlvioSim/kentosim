#ifndef SIMULATORESTIMATEFINISHJOBQUEUE_H
#define SIMULATORESTIMATEFINISHJOBQUEUE_H


#include <scheduling/job.h>
#include <utils/jobqueue.h>

#include <assert.h>
#include <set>

#define ESTIMATE_FINISH_TIME_JOB_Qt 2

using namespace std;
using std::set;
using namespace Simulator;


namespace Utils {

/**< Comparation operation of two jobs based on it's submit time */

struct estimatefinishtime_lt_t {
    bool operator() (Job* job1, Job* job2) const {
	
	double x = job1->getJobSimEstimateFinishTime();
	double y = job2->getJobSimEstimateFinishTime();
	assert( x >= 0 && y >= 0 );

	if( x != y )
	    return x < y;
	else
	   return job1->getJobNumber() < job2->getJobNumber();
    }
};


/** Set based on the estimated finish time  */
typedef set<Job*, estimatefinishtime_lt_t> EstimatedFinishQueue;
/**
*@author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

/** Implements a job queue where the order is the job finish real time  */
class EstimateFinishJobQueue : public JobQueue{
public:
  EstimateFinishJobQueue();
  ~EstimateFinishJobQueue();
  /* Auxiliar functions and main functions */
  
  virtual void insert(Job* job);
  virtual void erase(Job* job);
  virtual Job* headJob();    
  virtual Job* next(); 
  virtual Job* begin();
  virtual void deleteCurrent();
  
  virtual bool contains(Job* job);


private:
  EstimatedFinishQueue queue; /**< Contains the queue of jobs.*/
  EstimatedFinishQueue::iterator currentIterator;/**< The iterator that points to the current position that is being queried to the queue */
};

}

#endif
