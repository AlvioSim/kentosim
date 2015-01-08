#ifndef SIMULATORBUCKET_H
#define SIMULATORBUCKET_H

#include <scheduling/job.h>


namespace Simulator {

/**
* @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

/**
* This class implements a bucket. A bucket is an interval of time that represents a free slot for computational time, or a slot of computational time where a given job is allocated
* @see analogical bucket   
*/
class Bucket{
public:
  Bucket(); 
  ~Bucket();

  void setId(const int theValue);
  int getId() const;
  Job* getjob() const;  
  void setUsed(bool theValue);
  bool getused() const;
  void setJob(Job* theValue);
  void setCpu(const int& theValue);
  int getcpu() const;
  void setNode(const int& theValue);
  int getnode() const;
	
  /* this functions are suitable to be overloaded by its subclasses, mainly for implement some wrapping or specific stuff 
     in the specific policies, for example in the deepsearch policy, the getEndTime will return the penalized runtime if 
     if its required on the current analogical bucket (see it's definition in case of doub)
  */
  virtual void setStartTime(double theValue);
  virtual double getstartTime() const;
  virtual void setEndTime(double theValue);
  virtual double getendTime() const;

  


protected:
  bool used; /**< Indicates if some one is assigned to the bucket ?*/
  Job* job; /**< In case yes, which job is allocated to the slot  */    
  int node; /**< To which node the bucket belongs */
  int id;/**< The id of the bucket is global */
  int cpu; /**< To which processors the bucket belongs */
  double startTime;/**< The startime for the bucket */
  double endTime; /**< The endtime for the bucket */
  
};

}

#endif
