#include <scheduling/analogicaljoballocation.h>
#include <utils/utilities.h>

#include<iostream>
#include<string>
#include<sstream>
#include <vector>
#include <deque>

#include <stdarg.h>


using namespace std;
using std::vector;
using std::deque;


namespace Simulator {

/**
 * The constructor for the class. 
 * @param theValue A reference to the set of buckets that model the allocation for the job
 * @param starttime The starttime for the allocation 
 * @param endtime The endtime for the allocation 
 */
AnalogicalJobAllocation::AnalogicalJobAllocation(const deque< AnalogicalBucket * >& theValue,double starttime, double endtime)
 : JobAllocation(starttime,endtime)
{
  this->allocations = theValue;
  this->NextStartTimeToExplore = 0;
}

/**
 * The default constructor for the class
 */
AnalogicalJobAllocation::AnalogicalJobAllocation()
{
  this->NextStartTimeToExplore = 0;
}

/**
 * The default destructor for the class 
 */
AnalogicalJobAllocation::~AnalogicalJobAllocation()
{

}

/**
 * Returns the analogical buckets of the allocation 
 * @return A deque with the buckets 
 */
deque<AnalogicalBucket*> AnalogicalJobAllocation::getallocations() const
{
  return allocations;
}


/**
 * Sets the analogical buckets of the allocation 
 * @param theValue A deque with the allocation 
 */
void AnalogicalJobAllocation::setAllocations(const deque< AnalogicalBucket * >& theValue)
{
  allocations = theValue;
  
  /* for sanity */
  if(this->realAllocation)
  {
    //we have to check that all the starts and endtimes matches to the global onces ..
    for(deque<AnalogicalBucket*>::iterator iter = this->allocations.begin();iter != this->allocations.end();++iter)
    {
      Bucket* current = *iter;
      assert(current->getstartTime() == this->startTime && current->getstartTime() == this->endTime);
    }
  }
}

/**
 * Indicates if the current allocation is a real allocation. A real allocation is an allocation that has been asigned to a reservation table. A temporary allocation can be deleted with no worries concerning reservation table sanity. In case of realAllocation the reservation table will manage it. 
 * @param theValue Bool indicating if it is a real allocaiton. 
 */
void AnalogicalJobAllocation::setRealAllocation(bool theValue)
{
  realAllocation = theValue;
  
  /* for sanity */
  if(theValue)
  {
    //we have to check that all the starts and endtimes matches to the global onces ..
    for(deque<AnalogicalBucket*>::iterator iter = this->allocations.begin();iter != this->allocations.end();++iter)
    {
      Bucket* current = *iter;
      assert(current->getstartTime() == this->startTime && current->getstartTime() == this->endTime);
    }
  }
}

/**
 * When an allocation returned by the reservation table or by the scheduling policies has some problem it will return a double indicating when a possible allocation would be available that matches the requirement . For instance, that there are not enough cpus at the required time.
 * @return A double indicating a proposal of a time stamp when there would be an allocation matching the requirements.
 */
double AnalogicalJobAllocation::getNextStartTimeToExplore() const
{
  return NextStartTimeToExplore;
}


/**
 * Sets the next time to explore. 
 * @param theValue The next time to explore 
 * @see getNextStartTimeToExplore
 */
void AnalogicalJobAllocation::setNextStartTimeToExplore(double theValue)
{
  NextStartTimeToExplore = theValue;
}

}
