#include <scheduling/joballocation.h>

namespace Simulator {


/**
 * The default constructor for the class
 */
 
JobAllocation::JobAllocation()
{
  this->startTime = -1;
  this->endTime = -1;
  this->allocationProblem = false;
  this->allocationReason = NOPROBLEM;
  this->realAllocation = false;
}

/**
 * The constructor for the class that will be more commontly used.
 * @param starttime The startime for the allocation 
 * @param endtime The endtime for the allocation
 */
JobAllocation::JobAllocation(double starttime, double endtime)
{
  this->startTime = starttime;
  this->endTime = endtime;
  this->allocationProblem = false;
  this->allocationReason = NOPROBLEM;
  this->realAllocation = false;
}

/**
 * The default destructor for the class 
 */
JobAllocation::~JobAllocation()
{
}

/**
 * Returns the endTime
 * @return A double containing the endTime
 */
double JobAllocation::getendTime() const
{
  return endTime;
}


/**
 * Sets the endTime for the allocation 
 * @param theValue A double endTime for the job 
 */
void JobAllocation::setEndTime(double theValue)
{
  endTime = theValue;
}


/**
 * Returns if the allocation is done in the reservation table 
 * @return Bool that indicates if its real 
 */
bool JobAllocation::getrealAllocation() const
{
  return realAllocation; 
}


/**
 * Sets if the current allocation is real 
 * @param theValue If the allocation is real 
 */
void JobAllocation::setRealAllocation(bool theValue)
{
  realAllocation = theValue;  
}


/**
 * Returns the startTime 
 * @return A double containing the startTime
 */
double JobAllocation::getstartTime() const
{
  return startTime;
}


/**
 * Sets the start time for the allocation 
 * @param theValue The startime for the allocation 
 */
void JobAllocation::setStartTime(double theValue)
{
  startTime = theValue;
}


/**
 * Returns the allocation problem reason (if not problem in will be NOPROBLEM)
 * @return Returns a allocation_problem_t with the allocation problem
 */
allocation_problem_t JobAllocation::getallocationReason() const
{
  return allocationReason;
}


/**
 * Sets the allocation problem reason 
 * @param theValue The allocation problem
 */
void JobAllocation::setAllocationReason(const allocation_problem_t& theValue)
{
  allocationReason = theValue;
}

/**
 * Returns if there have been occurred any problem during the allocation 
 * @return A bool indicating if the allocation has any problem
 */
bool JobAllocation::getallocationProblem() const
{
  return allocationProblem;
}

/**
 * Sets if there has been any allocation problem 
 * @param theValue If there has been occured any problem 
 */
void JobAllocation::setAllocationProblem(bool theValue)
{
  allocationProblem = theValue;
}

}
