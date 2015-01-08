#include <scheduling/mnresourceusage.h>
#include <assert.h>

namespace Simulator {
	
/***************************
  Constructor and destructors
***************************/
/**
 * The default constructor for the class
 */
MNResourceUsage::MNResourceUsage() : ResourceUsage()
{
  this->EthernetBWLoad = 0;  
  this->NetworkBWLoad = 0;
  this->MemoryBWLoad = 0;
  this->MemoryCapacityLoad = 0;

}

/**
 * The default destructor for the class 
 */
MNResourceUsage::~MNResourceUsage()
{
}

/***************************
   Sets and gets functions  
***************************/

/**
 * Returns the NetworkBWLoad
 * @return A double containing the NetworkBWLoad
 */
double MNResourceUsage::getNetworkBWLoad() 
{
  return NetworkBWLoad;
}

 
/**
 * Sets the NetworkBWLoad to the MNResourceUsage
 * @param theValue The NetworkBWLoad
 */
void MNResourceUsage::setNetworkBWLoad(double theValue)
{
  assert(theValue >= 0); /* should never be less than zero */
  NetworkBWLoad = theValue;  
}

/**
 * Returns the MemoryCapacityLoad
 * @return A double containing the MemoryCapacityLoad
 */
double MNResourceUsage::getMemoryCapacityLoad() 
{
  return MemoryCapacityLoad;
}


/**
 * Sets the MemoryCapacityLoad to the MNResourceUsage
 * @param theValue The MemoryCapacityLoad
 */
void MNResourceUsage::setMemoryCapacityLoad(double theValue)
{
  assert(theValue >= 0); /* should never be less than zero */
  MemoryCapacityLoad = theValue;
}

/**
 * Returns the MemoryBWLoad
 * @return A double containing the MemoryBWLoad
 */
double MNResourceUsage::getMemoryBWLoad() 
{
  return MemoryBWLoad;
}


/**
 * Sets the MemoryBWLoad to the MNResourceUsage
 * @param theValue The MemoryBWLoad
 */
void MNResourceUsage::setMemoryBWLoad(double theValue)
{
  assert(theValue >= 0); /* should never be less than zero */
  MemoryBWLoad = theValue;
}

/**
 * Returns the EthernetBWLoad
 * @return A double containing the EthernetBWLoad
 */
double MNResourceUsage::getEthernetBWLoad() 
{
  return EthernetBWLoad;
}


/**
 * Sets the EthernetBWLoad to the MNResourceUsage
 * @param theValue The EthernetBWLoad
 */
void MNResourceUsage::setEthernetBWLoad(double theValue)
{
  assert(theValue >= 0); /* should never be less than zero */
  EthernetBWLoad = theValue;
}

/**
 * Returns the NumberOfJobs
 * @return A int containing the NumberOfJobs
 */
int MNResourceUsage::getNumberOfJobs() 
{
  return NumberOfJobs;
}


/**
 * Sets the NumberOfJobs to the MNResourceUsage
 * @param theValue The NumberOfJobs
 */
void MNResourceUsage::setNumberOfJobs(int theValue)
{
  NumberOfJobs = theValue;
}

}
