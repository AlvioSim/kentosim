#include <scheduling/simulatorcharacteristics.h>


namespace Simulator {

/**
 * The default constructor for the class
 */
SimulatorCharacteristics::SimulatorCharacteristics()
{
  this->archConfiguration = NULL;
  this->simStatistics = NULL;
  this->workload = NULL;
}

/**
 * The default destructor for the class 
 */
SimulatorCharacteristics::~SimulatorCharacteristics()
{
}


/**
 * Returns the configuration for the architecture that is being simulated 
 * @return A reference to the architecture configuration
 */
ArchitectureConfiguration* SimulatorCharacteristics::getarchConfiguration() 
{
  return archConfiguration;
}


/**
 * Sets the architecture configuration that will be used in the simulation 
 * @param theValue A reference to the architecture configuration
 */
void SimulatorCharacteristics::setArchConfiguration(ArchitectureConfiguration* theValue)
{
  archConfiguration = theValue;
}


/**
 * Returns the simStatistics that will be used in the simulation 
 * @return A reference to the simStatistics 
 */
SimStatistics* SimulatorCharacteristics::getsimStatistics() 
{
  return simStatistics;
}


/**
 * Sets the simStatistics that will be used in the simulation 
 * @param theValue A reference to the simStatistics
 */
void SimulatorCharacteristics::setSimStatistics(SimStatistics* theValue)
{
  simStatistics = theValue;
}


/**
 * Returns the workload that will be used as in input for the simulation 
 * @return A reference to the workload 
 */
TraceFile* SimulatorCharacteristics::getworkload() 
{
  return workload;
}


/**
 * Sets the workload that will be used in the simulation 
 * @param theValue A reference to the workload 
 */
void SimulatorCharacteristics::setWorkload(TraceFile* theValue)
{
  workload = theValue;
}

}
