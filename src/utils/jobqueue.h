#ifndef JOBQUEUE_H
#define JOBQUEUE_H

#include <scheduling/job.h>

/**
* @author Francesc Guim,C6-E201,93 401 16 50,
*/

/** This function defines the methods that the job queues have to provide. This is a pure abstract class.*/
using namespace Simulator;

namespace  Utils{
class JobQueue{
public:
  /*Constructors and destructors*/
  JobQueue();
  ~JobQueue();
  
  /*Auxiliar and main methods*/
  virtual void insert(Job* job) = 0; 
  virtual void erase(Job* job) = 0; 
  virtual Job* headJob() = 0; 
  virtual Job* next() = 0; 
  virtual void deleteCurrent() = 0; 
  virtual Job* begin() = 0;
  
  virtual bool contains(Job* job) = 0; 
    
  void setJobs(const int& theValue);
  int getjobs() const;
	

protected:
  int jobs; /**<Contains the number of jobs stored in the queue, is only for checking the sanity of the queue */

};
}
#endif
