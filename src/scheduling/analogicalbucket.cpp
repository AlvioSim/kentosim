#include <scheduling/analogicalbucket.h>
#include <assert.h>

namespace Simulator {

/**
 * The default constructor for the class
 */
AnalogicalBucket::AnalogicalBucket() : Bucket()
{  
  this->penalizedEndTime = 0;
  this->penalizedTime = 0;
  this->usePenalizedTime = false;
}

/**
 * The default destructor for the class 
 */
AnalogicalBucket::~AnalogicalBucket()
{
}

/**
 * The constructor more commontly used for the class
 * @param StartTime The start time for the bucket 
 * @param EndTime The end time for the bucket
 */
AnalogicalBucket::AnalogicalBucket(double StartTime, double EndTime) : Bucket()
{  
  //for sanity , if EndTime is -1 means that is the last bucket ..
  //be aware that a bucket can be from one second till inifity 
  assert(StartTime <= EndTime || EndTime == -1);
  
  this->startTime = StartTime;
  this->endTime = EndTime;
  this->penalizedEndTime = EndTime;
  this->penalizedTime = 0;
  this->usePenalizedTime = false;
}

/**
 * Deallocates the job from the bucket.
 */
void AnalogicalBucket::deallocateJob()
{
  assert(this->job != NULL);
  this->job = 0;
  this->used = false;
  this->usePenalizedTime = false;
}


/**
 * Returns teh penalized time of the bucket 
 * @return The penalized time 
 */
double AnalogicalBucket::getpenalizedTime() const
{
  return penalizedTime;
}


/**
 * Sets the penalized time 
 * @param theValue A double with the penalized time 
 */
void AnalogicalBucket::setPenalizedTime(double theValue)
{
  assert(theValue >= 0);
  penalizedTime = theValue;
  this->penalizedEndTime = this->endTime + theValue;
}


/**
 * Returns the penalized end time (currently deprecated, it should not be used, however is left just in case )
 * @return The penalized end time 
 */
double AnalogicalBucket::getpenalizedEndTime() const
{
  return penalizedEndTime;
}


/**
 * Sets the penalized end time (currently deprecated, it should not be used, however is left just in case )
 * @param theValue  A double with the penalized Time
 */
void AnalogicalBucket::setPenalizedEndTime(double theValue)
{
  assert(theValue >= 0);
  penalizedEndTime = theValue;
}


/**
 * Returns true if the value for the penalized Time has a correct value or not 
 * @return A bool if the penalized time is used 
 */
bool AnalogicalBucket::getusePenalizedTime() const
{
  return usePenalizedTime;
}


/**
 * Sets if the penalized time has a correct value 
 * @param theValue A bool indicating if the penalizedTime is used 
 */
void AnalogicalBucket::setUsePenalizedTime(bool theValue)
{
  usePenalizedTime = theValue;
}

/**
 * Returns the end time of the bucket 
 * @return A double with the end time 
 */
double AnalogicalBucket::getendTime() const
{
  if(this->usePenalizedTime)
  {
   return this->penalizedEndTime;
  }
  else
   return this->endTime;
}

/**
 * Sets the end time for the bucket 
 * @param theValue The end time 
 */
void AnalogicalBucket::setEndTime(double theValue)
{
  this->endTime = theValue;
  this->penalizedEndTime = this->penalizedTime + theValue;
}


}
