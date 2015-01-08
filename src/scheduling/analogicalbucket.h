#ifndef SIMULATORANALOGICALBUCKET_H
#define SIMULATORANALOGICALBUCKET_H

#include <scheduling/bucket.h>

namespace Simulator {

/**
* @author Francesc Guim,C6-E201,93 401 16 50,
*/

/**
* This class implements an analogical bucket. This kind of bucket is a bucket but represents also an interval of time.
* @see The class bucket
*/
class AnalogicalBucket : public Bucket{
public:
  AnalogicalBucket();
  AnalogicalBucket(double StartTime, double EndTime);
  ~AnalogicalBucket();

  void deallocateJob();
  void setPenalizedTime(double theValue);
  double getpenalizedTime() const;
  void setPenalizedEndTime(double theValue);
  double getpenalizedEndTime() const;
  void setUsePenalizedTime(bool theValue);
  bool getusePenalizedTime() const;
	
  virtual double getendTime() const;
  virtual void setEndTime(double theValue);
	



private:
  double penalizedTime; /**< Indicates the amount of time of the bucket that is a penalization to the job that is asigned to the bucket */
  double penalizedEndTime; /**< Indicates the end of the penalizedTime of the time - currently this is deprecated. But left just in case. */
  bool usePenalizedTime; /**< Indicates if the preceeding two variables have a good value */

};

}

#endif
