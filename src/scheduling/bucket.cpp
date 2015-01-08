#include <scheduling/bucket.h>

namespace Simulator {

/**
 * The default constructor for the class
 */
Bucket::Bucket()
{
  this->node = 0;
  this->cpu = 0;
  this->endTime = 0;
  this->startTime = 0; 
  this->id = 0;
  this->job = NULL;
  this->used = false;
}
 
/**
 * The default destructor for the class 
 */
Bucket::~Bucket()
{
}

/**
 * Function that returns id of the bucket 
 * @return An integer with the id 
 */
int Bucket::getId() const
{
  return id;
}


/**
 * Function that sets the id of the bucket 
 * @param theValue The id for the bucket
 */
void Bucket::setId(const int theValue)
{
  id = theValue;
}

/**
 * Function that returns job of the bucket 
 * @return A reference to the job (null if no job assigned)
 */
Job* Bucket::getjob() const
{
  return job;
}


void Bucket::setJob(Job* theValue)
{
  job = theValue;
}


/**
 * Function that returns true if the buckets is assigned to a job
 * @return A bool indicating if it is used by a job
 */
bool Bucket::getused() const
{
  return used;
}

	
/**
 * Function that sets if the bucket is used by a job or not
 * @param theValue A bool indicating in its used by a job
 */
void Bucket::setUsed(bool theValue)
{
  used = theValue;
}

/**
 * Function that returns startTime of the bucket 
 * @return A double with the startime for the bucket
 */
double Bucket::getstartTime() const
{
  return startTime;
}



/**
 * Sets the startTime for the bucket 
 * @param theValue A double indicating the start time for the bucket
 */
void Bucket::setStartTime(double theValue)
{
  startTime = theValue;
}


/**
 * Function that returns the endTime of the bucket 
 * @return A double with the end time 
 */
double Bucket::getendTime() const
{
  return endTime;
}


/**
 * Sets the endTime for the job 
 * @param theValue A double with the end time 
 */
void Bucket::setEndTime(double theValue)
{
  endTime = theValue;
}

/**
 * Function that returns the cpu of the bucket 
 * @return A integer with the cpu
 */
int Bucket::getcpu() const
{
  return cpu;
}


/**
 * Sets the cpu to whom the bucket is asigned 
 * @param theValue A integer indicating the cpu number
 */
void Bucket::setCpu(const int& theValue)
{
  cpu = theValue;
}


/**
 * Function that returns number of node of the bucket 
 * @return A integer indicating the node number
 */
int Bucket::getnode() const
{
  return node;
}


/**
 * Sets the number of node for the bucket 
 * @param theValue A integer indicating the number of node
 */
void Bucket::setNode(const int& theValue)
{
  node = theValue;
}

}
