#include <scheduling/reservationtable.h>

namespace Simulator {

/***************************
  Constructor and destructors
***************************/

/**
 * @param theValue A reference to the configuration concerning the architecture
 * @param log A reference to the logging engine 
 * @param globaltime The globaltime of the simulation
 */
ReservationTable::ReservationTable(ArchitectureConfiguration* theValue,Log* log, double globaltime)
{
  this->architecture = theValue;
  this->log = log;
  this->globaltime = globaltime;
  this->initialGlobalTime = globaltime;
}

/**
 * The default constructor for the class
 */
ReservationTable::ReservationTable()
{

}

/**
 * The default destructor for the class 
 */
ReservationTable::~ReservationTable()
{
}

/***************************
  Sets and gets
***************************/

/**
 * Returns a reference to the architecture being used by the policy and where the jobs are being allocated 
 * @return A reference to the architecture 
 */
ArchitectureConfiguration* ReservationTable::getarchitecture() const
{
  return architecture;
}

/**
 * Sets the architecture used by the reservationtable 
 * @param theValue The architecture
 */
void ReservationTable::setArchitecture(ArchitectureConfiguration* theValue)
{
  architecture = theValue;

}

/**
 * Returns the allocation of a given job 
 * @param job The job to whom the allocation is required
 * @return The allocation of the job
 */
JobAllocation* ReservationTable::getJobAllocation(Job* job)
{
  map<Job*,JobAllocation*>::iterator iter = this->JobAllocationsMapping.find(job);
  
  JobAllocation* allocation = iter->second;
  
  return allocation;
}
 

/**
 * Returns the globaltime
 * @return A double containing the globaltime
 */
double ReservationTable::getglobaltime() const
{
  return globaltime;
}


/**
 * Sets the globaltime to the ReservationTable
 * @param theValue The globaltime
 */
void ReservationTable::setGlobaltime(double theValue)
{
  globaltime = theValue;
}

/**
 * Returns the initialGlobalTime
 * @return A double containing the initialGlobalTime
 */
double ReservationTable::getinitialGlobalTime() const
{
  return initialGlobalTime;
}


/**
 * Sets the initialGlobalTime to the ReservationTable
 * @param theValue The initialGlobalTime
 */
void ReservationTable::setInitialGlobalTime(double theValue)
{
  initialGlobalTime = theValue;
}

/**
 * Returns the log
 * @return A reference to the logging engine
 */
Log* ReservationTable::getlog() const
{
  return log;
}

/**
 * Sets the Log to the ReservationTable
 * @param theValue The reference to the logging engine
 */
void ReservationTable::setLog(Log* theValue)
{
  log = theValue;
}

}
