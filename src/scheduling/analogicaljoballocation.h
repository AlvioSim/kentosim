#ifndef SIMULATORANALOGICALJOBALLOCATION_H
#define SIMULATORANALOGICALJOBALLOCATION_H

#include <scheduling/joballocation.h>
#include <scheduling/analogicalbucket.h>

#include <deque>

using namespace std; 
using std::deque;

namespace Simulator {

/**
* @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

/**
* This class represents an analogical allocation of a job. 
* @see The joballocation class
*/
class AnalogicalJobAllocation : public JobAllocation{

public:
  AnalogicalJobAllocation(const deque< AnalogicalBucket * >& theValue,double starttime, double endtime);
  AnalogicalJobAllocation();
  virtual ~AnalogicalJobAllocation();
  void setAllocations(const deque< AnalogicalBucket * >& theValue);
  deque< AnalogicalBucket * > getallocations() const;
  virtual void setRealAllocation(bool theValue);
  void setNextStartTimeToExplore(double theValue);
  double getNextStartTimeToExplore() const;
	

  
  deque<AnalogicalBucket*> allocations; /**< Buckets associated to the current allocation, its keep public for performance stuff */
   
  private:
  
  double NextStartTimeToExplore; /**< this variable inidicates where to explore if this allocation is not satisfactory for the once who asked for this */

};

}

#endif
