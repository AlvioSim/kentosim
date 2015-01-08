#ifndef SIMULATORSIMULATORCHARACTERISTICS_H
#define SIMULATORSIMULATORCHARACTERISTICS_H

#include <utils/architectureconfiguration.h>
#include <statistics/statistics.h>
#include <scheduling/tracefile.h>

namespace Simulator {

/**
* @author Francesc Guim,C6-E201,93 401 16 50,
*/

/** This class contains a the caracteritzation of the simulation */
class SimulatorCharacteristics{
public:
  /* Constructors and desctructors*/

  SimulatorCharacteristics();
  ~SimulatorCharacteristics();
  
  /* Get and sets methods*/
  
  void setArchConfiguration(ArchitectureConfiguration* theValue);	
  ArchitectureConfiguration* getarchConfiguration() ;
  void setSimStatistics(SimStatistics* theValue);
  SimStatistics* getsimStatistics() ;
  void setWorkload(TraceFile* theValue);
  TraceFile* getworkload();
	
    
private:
   /*The following files provide information about the characteristics of the simulator*/
   ArchitectureConfiguration* archConfiguration; /**< Contains the characteritzation of the architecture used in the current simulator */
   TraceFile* workload; /**< Contains the workload trace file that will be used in the simulation */
   SimStatistics* simStatistics; /**< Contains the statistics configuration that will be computed for the simulation. In this class the user can specify which statistical information wants to retrieve and for wich metric.*/
};

}

#endif
