#ifndef SIMULATORGRIDANALOGICALRESERVATIONTABLE_H
#define SIMULATORGRIDANALOGICALRESERVATIONTABLE_H

#include <scheduling/reservationtable.h>

#include <map>

using namespace std;
using std::map;

namespace Simulator {

/**
* @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>*/

/** This class represents a grid reservation table - would be used by the GridBackfilling policy */
class GridAnalogicalReservationTable : public ReservationTable{
public:
  GridAnalogicalReservationTable();
  ~GridAnalogicalReservationTable();
  
  virtual bool allocateJob(Job* job, JobAllocation* allocation); /* Inherited from reservation table - see there */
  virtual bool deAllocateJob(Job* job); /* Inherited from reservation table - see there */
  virtual bool killJob(Job* job);
  virtual bool extendRuntime(Job* job,double length);
  virtual int  getNumberCPUSUsed();
  void addReservationTable(string center,ReservationTable* rtable);
  
private:

  map<string,ReservationTable*> rTables;/**<  A hashmap with the reservationtables of each of the underlying centers */
  
};

}

#endif
