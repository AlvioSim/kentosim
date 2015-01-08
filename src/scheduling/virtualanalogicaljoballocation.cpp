#include <scheduling/virtualanalogicaljoballocation.h>
#include <scheduling/virtualanalogicalrtable.h>

namespace Simulator {

/**
 * The default constructor for the class
 */
VirtualAnalogicalJobAllocation::VirtualAnalogicalJobAllocation()
{
}

/**
 * The default destructor for the class 
 */
VirtualAnalogicalJobAllocation::~VirtualAnalogicalJobAllocation()
{
}


/**
 * Returns the allocation criteria that has been used for allocate the job in the virtual reservationTable, see the allocationType. This is used to find the the allocation that maximize the job performance.
 * @param jobid The job id to whom the allocation type used is required 
 * @return A allocation_type_t containing the allocation type used 
 */
allocation_type_t VirtualAnalogicalJobAllocation::getAllocType(int jobid)
{
  map<int,allocation_type_t>::iterator it = this->allocationType.find(jobid);
  
  if(it != this->allocationType.end())
   return it->second;
  else
   return NOTYPE;
}

/**
 * Returns the start time for the job allocation in the current outcome evaluation
 * @param jobid The job id whom the allocation start time is required
 * @return The start time for the job
 */
double VirtualAnalogicalJobAllocation::getStartTime(int jobid)
{
 map<int,double>::iterator it = this->startTime.find(jobid);
 
 if(it != this->startTime.end())
  return it->second;
 else 
  return 0;

}

/**
 * Sets the allocation type of a given job 
 * @param jobid The job id 
 * @param alloc The allocation type 
 */
void VirtualAnalogicalJobAllocation::setAllocType(int jobid, allocation_type_t alloc)
{
  this->allocationType.insert(pairAllocType(jobid,alloc));
}



/**
 * Sets the start time for the alloction of a given job 
 * @param jobid The job id for the job 
 * @param startTime The start time for the allocation 
 */
void VirtualAnalogicalJobAllocation::setStartTime(int jobid, double startTime)
{
  this->startTime.insert(pairTime(jobid,startTime));
}

/**
 * Returns the end time for the job allocation in the current outcome evaluation
 * @param jobid The job id whom the allocation start time is required
 * @return The end time for the job
 */
double VirtualAnalogicalJobAllocation::getEndTime(int jobid)
{
 map<int,double>::iterator it = this->endTime.find(jobid);
 
 if(it != this->endTime.end())
  return it->second;
 else 
  return 0;

}

/**
 * Sets the job penalty  for the alloction of a given job 
 * @param jobid The job id for the job 
 * @param penalty The job penalty for the job in the allocation 
 */
void VirtualAnalogicalJobAllocation::setJobPenalty(int jobid, double penalty)
{
  this->jobPenalty.insert(pairTime(jobid,penalty));
}

/**
 * Returns the penalty for the job allocation in the current outcome evaluation
 * @param jobid The job id whom the allocation start time is required
 * @return The penalty for the job
 */
double VirtualAnalogicalJobAllocation::getjobPenalty(int jobid)
{
 map<int,double>::iterator it = this->jobPenalty.find(jobid);
 
 if(it != this->jobPenalty.end())
  return it->second;
 else 
  return 0;

}

/**
 * Sets the job end time  for the alloction of a given job 
 * @param jobid The job id for the job 
 * @param endTime The end time for the job in the allocation 
 */
void VirtualAnalogicalJobAllocation::setEndTime(int jobid, double endTime)
{
  this->endTime.insert(pairTime(jobid,endTime));
}


/**
 * Returns the performance metric that the given outcome has achieved in the reservationTable
 * @return A reference to a metric containing the performance
 */
Metric* VirtualAnalogicalJobAllocation::getperformance() const
{
  return performance;
}


/**
 * Sets the performance metric that the given outcome of the allocation has achived 
 * @param theValue The performance metric 
 */
void VirtualAnalogicalJobAllocation::setPerformance(Metric* theValue)
{
  performance = theValue;
}


/**
 * Returns the virtual reservationTable to whom the job allocations are linked 
 * @return A reference to the virtual reservationTable where the allocation is done 
 */
VirtualAnalogicalRTable* VirtualAnalogicalJobAllocation::getreservationTable() const
{
  return reservationTable;
}


/**
 * Sets the virtual reservationTable to whom the job allocations are linked 
 * @param theValue The virtualanalogicalrtable
 */
void VirtualAnalogicalJobAllocation::setReservationTable(VirtualAnalogicalRTable* theValue)
{
  reservationTable = theValue;
}

/**
 * Deletes the reservation table linked to the current outcome. 
 */
void VirtualAnalogicalJobAllocation::freeReservationTable()
{ 
  delete this->reservationTable;
}

}
