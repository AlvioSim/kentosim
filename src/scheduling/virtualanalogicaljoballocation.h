#ifndef SIMULATORVIRTUALANALOGICALJOBALLOCATION_H
#define SIMULATORVIRTUALANALOGICALJOBALLOCATION_H

#include <scheduling/metric.h>

#include <map>

using namespace std;
using std::map;

namespace Simulator {

/** This is an enum that enumerates the different set of resource allocation policies that are used by the simulator- probably the will be moved to another class in the clse future.*/
enum allocation_type_t {
    CPUS_CONTINOUS = 0,
    FIRST_BYTIME,
    LESS_CONSUME,
    NOTYPE,
};

/** This is a pair of job id and allocation type */
typedef pair <int, allocation_type_t> pairAllocType;
/** This is a pair of job id and a given time (used mainly for start time and end time)*/
typedef pair <int, double> pairTime;

//forward declaration 
class VirtualAnalogicalRTable;

/**
* @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

/**
* This class defines a set of job allocation that is linked to a virtual reservation table. This allocations are allocations that are not allocated in real scheduling decisions are used for take decision. Thus, for carry out allocations to a reservation table, use the analogical reservation table. The idea is that once an outcome is evaluated the resultant allocations used for each job in the outcome are stored in this class 
* @see The class reservation table
*/

class VirtualAnalogicalJobAllocation{
public:
  VirtualAnalogicalJobAllocation();
  ~VirtualAnalogicalJobAllocation();
   
  allocation_type_t getAllocType(int jobid);
  double getStartTime(int jobid);
  double getEndTime(int jobid);
  double getjobPenalty(int jobid);
  void setAllocType(int jobid, allocation_type_t allocation_type);
  void setStartTime(int jobid, double startTime);
  void setEndTime(int jobid, double endTime);
  void setJobPenalty(int jobid, double penalty);
  void setPerformance(Metric* theValue);
  Metric* getperformance() const;
  void setReservationTable(VirtualAnalogicalRTable* theValue);
  VirtualAnalogicalRTable* getreservationTable() const;
  void freeReservationTable(); //as may be this reservation table will be used .. we must free it explicitily..
  
private:    
  map<int,allocation_type_t> allocationType; /**< for each job we store the allocation  is the best */
  map<int,double> startTime; /**< for each job we store the  starttime that is the best */
  map<int,double> endTime; /**< for each job we store the  starttime that is the best */
  map<int,double> jobPenalty;  /**< we store the penalty for the job */
  
  Metric* performance; /**< Contains the performance that the job would experiment using this allocation. Be aware due to this is only an estimation. */
  VirtualAnalogicalRTable* reservationTable; /**< The virtual reservation table to whom the allocation is linked. This reservationTable is only temporarly and its managed by the current virtual job allocation. Once that the evaluation of a given outcome has been done in such reservation table it will be deleted  */
  
  
};

}

#endif
