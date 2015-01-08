#ifndef SIMULATORREALFINISHJOBQUEUE_H
#define SIMULATORREALFINISHJOBQUEUE_H

#include <scheduling/job.h>
#include <utils/jobqueue.h>

#include <assert.h>
#include <set>

#define REAL_FINISH_TIME_JOB_Qt 4

using namespace std;
using std::set;

using namespace Simulator;

namespace Utils {

/**< Comparation operation of two jobs based on it's submit time*/

struct finishtime_lt_t {
    bool operator() (Job* job1, Job* job2) const {
	
	double x = job1->getJobSimFinishTime();
	double y = job2->getJobSimFinishTime();
	assert( x >= 0 && y >= 0 );

	if( x != y )
	    return x < y;
	else
	   return job1->getJobNumber() < job2->getJobNumber();
    }
};


/** Set based on the finish time for the job */
typedef set<Job*, finishtime_lt_t> RealFinishQueue;

/**
* @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

/** 
*  This class implements a job queue where the order of the queue is based on the real finish time for the job 
*/
class RealFinishJobQueue : public JobQueue{
public:
  RealFinishJobQueue();
  ~RealFinishJobQueue();
  
  /* Auxiliar functions and main functions */
  virtual void insert(Job* job);
  virtual void erase(Job* job);
  virtual Job* headJob();    
  virtual Job* next(); 
  virtual Job* begin();
  virtual void deleteCurrent();   
  virtual bool contains(Job* job);
  
private:
  RealFinishQueue queue; /**< Contains the queue of jobs where they are sorted in the order defined in the comparaison operator */
  RealFinishQueue::iterator currentIterator;/**< The iterator that points to the current position that is being queried to the queue */
};

}

#endif
