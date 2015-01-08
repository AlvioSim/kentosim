#include <scheduling/gridanalogicalreservationtable.h>

namespace Simulator {

/**
 * The default constructor for the class
 */
GridAnalogicalReservationTable::GridAnalogicalReservationTable()
{
}

/**
 * The default destructor for the class 
 */
GridAnalogicalReservationTable::~GridAnalogicalReservationTable()
{
}

/**
 * Allocates the job to the reservation table. Currently non functional.
 * @param job The job to allocate
 * @param allocation The allocation 
 * @see The reservation table class 
 * @return True if no problem have raised 
 */
bool GridAnalogicalReservationTable::allocateJob(Job* job, JobAllocation* allocation)
{
}

/**
 * Deallocate a job from the reservation table. Currently non functional.
 * @param job The job to be deallocted
 * @see The reservation table class 
 * @return True if no problem have rised. 
 */
bool GridAnalogicalReservationTable::deAllocateJob(Job* job)
{
}


/**
 * Invoqued when the given job has been killed by the scheduler. Currently non functional.
 * @param job The job that has been killed 
 * @see The reservation table class 
 * @return True if no problem have rised. 
 */
bool GridAnalogicalReservationTable::killJob(Job* job)
{
}

/**
 * Function that extends the runtime of the allocation for the job in the reservation talbe 
 * @param job the job whom job runtime has to be extended
 * @param length The lenght that of the RT that has to be added
 * @see The reservation table class 
 * @return True if no problem have rised. 
 */
bool GridAnalogicalReservationTable::extendRuntime(Job* job,double length)
{
}

/**
 * Returns the number of cpus used in the reservation table.
 * @return An Integer indicating the number of cpus used 
 */
int  GridAnalogicalReservationTable::getNumberCPUSUsed()
{
  int NumCpusUsed = 0;
  
  for(map<string,ReservationTable*>::iterator it = this->rTables.begin(); it != this->rTables.end();++it)
  {
    ReservationTable* rtable = it->second;
    NumCpusUsed+= rtable->getNumberCPUSUsed();    
  }
  
  return NumCpusUsed;
}

/**
 * Adds a reservation table of a given center 
 * @param center The center to whom the reservation table belongs 
 * @param rtable A reference to the reservation table 
 */
void GridAnalogicalReservationTable::addReservationTable(string center,ReservationTable* rtable)
{
  this->rTables.insert(pair<string,ReservationTable*>(center,rtable));  

}


}
