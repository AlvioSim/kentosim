#ifndef UTILSRUAJOBQUEUE_H
#define UTILSRUAJOBQUEUE_H


#include <assert.h>
#include <set>

#include <utils/jobqueue.h>

using namespace std;
using std::set;

using namespace Simulator;

namespace Utils {

/**< Comparation operation of two jobs based on the old penalized runtime , note that this penalty should not be changed without the notification, the jobs are saved in two structures, the set and the vector , when the rua queue is called to uptadte the elements the jobs will be taken from the vector and reiserted to the queue */

struct RUA_lt_t {
    bool operator() (Job* job1, Job* job2) const {
	
	double x = job1->getJobNumber();
	double y = job2->getJobNumber();
	assert( x >= 0 && y >= 0 );

	if( x != y )
	    return x < y;
	else
	   return job1->getJobNumber() < job2->getJobNumber();
    }
};


typedef set<Job*, RUA_lt_t> RUAQueue;

/**
	@author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/
class RUAJobQueue : public JobQueue
{
public:
  RUAJobQueue();
  ~RUAJobQueue();

  /* Auxiliar functions and main functions */
  virtual void insert(Job* job);
  virtual void erase(Job* job);
  virtual Job* headJob();    
  virtual Job* next(); 
  virtual Job* begin();
  virtual void deleteCurrent();   
  virtual bool contains(Job* job);
  bool updateJobOrders();

  vector<Job*> vectJobs; /**< Contains the all the vectors of the queue , when resetting the vectors for the backfilling stuff we will reinsert the jobs based on this vector, for simplicity as it has to be used by the rua backfilling we keep it public  */

  
private:
  RUAQueue queue; /**< Contains the queue of jobs where they are sorted in the order defined in the comparaison operator */
  RUAQueue::iterator currentIterator;/**< The iterator that points to the current position that is being queried to the queue */


};

}

#endif
