#ifndef SIMULATORSTARTTIMEJOBQUEUE_H
#define SIMULATORSTARTTIMEJOBQUEUE_H

#include <scheduling/job.h>
#include <utils/jobqueue.h>

#include <assert.h>
#include <set>

#define START_TIME_JOB_Qt 5

using namespace std;
using std::set;

using namespace Simulator;



namespace Utils {

/**< Comparation operation of two jobs based on it's start time */
struct starttime_lt_t {
    bool operator() (Job* job1, Job* job2) const {
	
	double x = job1->getJobSimStartTime();
	double y = job2->getJobSimStartTime();
	assert( x >= 0 && y >= 0 );

	if( x != y )
	    return x < y;
	else
	   return job1->getJobNumber() < job2->getJobNumber();
    }
};

/**
* @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

/** Set based on the start time  */
typedef set<Job*, starttime_lt_t> StartTimeQueue;

/** * This class implements a job queue ordered by its start time in the SIMULATION. */
class StartTimeJobQueue : public JobQueue{
public:
  
  //Constructors and destructors
  StartTimeJobQueue();
  ~StartTimeJobQueue();
  
  /* Auxiliar functions and main functions */
  virtual void insert(Job* job);
  virtual void erase(Job* job);
  virtual Job* headJob();
  virtual Job* next(); 
  virtual Job* begin();
  virtual void deleteCurrent();
  virtual bool contains(Job* job);
    
  Job* backJob();
  
  StartTimeQueue* getQueue();  
  

private:
  StartTimeQueue queue;  /**< Contains the queue of jobs where they are sorted in the order defined in the comparaison operator */
  StartTimeQueue::iterator currentIterator; /**< The iterator that points to the current position that is being queried to the queue */

};

}

#endif
