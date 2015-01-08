#include <archdatamodel/node.h>
#include <archdatamodel/bladecenter.h>
#include <assert.h>

namespace ArchDataModel { 

/***************************
  Constructor and destructors
***************************/

/**
 * The default constructor for the node 
 */
Node::Node()
{
  //default capacity - this ructor should not be invoqued
  this->EthernetBW= 0;
  this->MemoryCapacity = 0;
  this->NetworkBW = 0;
  this->MemoryBW = 0;
  
  this->EthernetBWLoad = 0;  
  this->NetworkBWLoad = 0;
  this->MemoryBW = 0;
  this->MemoryCapacityLoad = 0;
  
  this->bladeCenter = 0 ;
}


/**
 * The constructor for the Node 
 * @param EthernetBW The ethernetBW in MB/S of the node 
 * @param MemoryCapcacity The memoryCapacity  in MB/S  of the node 
 * @param NetworkBW The networkBW  in MB/S  of the node 
 * @param MemoryBW The MemoryBW  in MB/S of the node 
 * @param NumberCPUS The number of cpus of the node 
 * @return 
 */
Node::Node(double EthernetBW, double MemoryCapcacity, double NetworkBW,double MemoryBW,int NumberCPUS)
{
    
  this->EthernetBW = EthernetBW;  
  this->NetworkBW = NetworkBW;
  this->MemoryBW = MemoryBW;
  this->MemoryCapacity = MemoryCapcacity;
  this->NCPUS = NumberCPUS;
  
  this->EthernetBWLoad = 0;  
  this->NetworkBWLoad = 0;  
  this->MemoryBWLoad = 0;
  this->MemoryCapacityLoad = 0;
  this->bladeCenter = 0;
}
        
Node::~Node()
{
}

/***************************
   Sets and gets functions  
***************************/


/**
 * Returns the bw netowrk load of the node 
 * @return the networkBW in MB/S of the node
 */
double Node::getNetworkBWLoad() 
{
  return NetworkBWLoad;
}


/**
 * Sets the networkBW load in MB/S of the node
 * @param theValue the load in MB/S of the node
 */
void Node::setNetworkBWLoad(double theValue)
{
  assert(theValue >= 0); /* should never be less than zero */
  NetworkBWLoad = theValue;  
}


/**
 * Returns the networkBW in MB/S of the node
 * @return The networkBW in MB/S of the node
 */
double Node::getNetworkBW() 
{ 
  return NetworkBW;
}


/**
 * Sets the networkBW of the node 
 * @param theValue the networkBW in MB/S of the node
 */
void Node::setNetworkBW(double theValue)
{
  assert(theValue >= 0); /* should never be less than zero */  
  NetworkBW = theValue;
}


/**
 * Returns the memoryCapacity used of the node
 * @return The MB used of the capacity of the node
 */
double Node::getMemoryCapacityLoad() 
{
  return MemoryCapacityLoad;
}


/**
 * Sets the memoryCapacity Load of the node 
 * @param theValue Sets the MB Load of capacity of the node 
 */
void Node::setMemoryCapacityLoad(double theValue)
{
  assert(theValue >= 0); /* should never be less than zero */
  MemoryCapacityLoad = theValue;
}


/**
 * The memoryCapacity of the node 
 * @return The MB of memoryCapacity of the node 
 */
double Node::getMemoryCapacity() 
{
  return MemoryCapacity;
}


/**
 * Sets the memoryCapacity of the node 
 * @param theValue The MB of memoryCapacity of the node 
 */
void Node::setMemoryCapacity(double theValue)
{
  assert(theValue >= 0); /* should never be less than zero */
  MemoryCapacity = theValue;
}


/**
 * Returns the memory bw used 
 * @return The mem in MB/S of the node used 
 */
double Node::getMemoryBWLoad() 
{
  return MemoryBWLoad;
}


/**
 * Sets the mem bw used 
 * @param theValue The mem in MB/S of the node used 
 */
void Node::setMemoryBWLoad(double theValue)
{
  assert(theValue >= 0); /* should never be less than zero */
  MemoryBWLoad = theValue;
}


/**
 * Returns mem bw of capacity
 * @return The mem bw in MB/S of the node
 */
double Node::getMemoryBW() 
{
  return MemoryBW;
}


/**
 * Sets the mem bw of the node 
 * @param theValue the mem bw in MB/S of the node
 */
void Node::setMemoryBW(double theValue)
{
  assert(theValue >= 0); /* should never be less than zero */
  MemoryBW = theValue;
}



/**
 * Returns the ehternet BW of the node 
 * @return the eth BW in MB/S of the node
 */
double Node::getEthernetBW() 
{
  return EthernetBW;
}


/**
 * Sets the eth bw of the node 
 * @param theValue The eth bw in MB/S of the node
 */
void Node::setEthernetBW(double theValue)
{
  assert(theValue >= 0); /* should never be less than zero */
  EthernetBW = theValue;
}


/**
 * Returns the load of the ethernetBW
 * @return the eth bw load in MB/S of the node
 */
double Node::getEthernetBWLoad() 
{
  return EthernetBWLoad;
}


/**
 * Sets the ethernetBW load of the node 
 * @param theValue eth bw load in MB/S of the node
 */
void Node::setEthernetBWLoad(double theValue)
{
  assert(theValue >= 0); /* should never be less than zero */
  EthernetBWLoad = theValue;
}

/**
 * Returns the bladecenter to whom the node belongs 
 * @return The reference to the bladecenter
 */
BladeCenter* Node::getbladeCenter() 
{
  return bladeCenter;
}


/**
 * Sets the bladecenter to whom the node belongs
 * @param theValue A reference to the bladecenter
 */
void Node::setBladeCenter(BladeCenter* theValue)
{
  bladeCenter = theValue;
}


/**
 * Returns the id of the node 
 * @return The id of the node
 */
int Node::getId()
{
  return Id;
}


/**
 * Set the id of the node 
 * @param theValue 
 */
void Node::setId(int theValue)
{
  Id = theValue;
}


/**
 * Returns the number of the cpus of the node 
 * @return the number of cpus 
 */
int Node::getNCPUS() 
{
  return NCPUS;
}


/**
 * Set the number of cpus 
 * @param theValue Number of cpus 
 */
void Node::setNCPUS( int theValue)
{
  NCPUS = theValue;
}


/**
 * Returns the number of cpus that are not used (due to locking or failure)
 * @return number of locked cpus 
 */
int Node::getlockedCPUS() 
{
  return lockedCPUS;
}


/**
 * Set the number of cpus to lockedCPUS
 * @see int Node::getlockedCPUS() 
 * @param theValue the number of cpus to lock 
 */
void Node::setLockedCPUS( int theValue)
{
  lockedCPUS = theValue;
}


/**
 * Set the number of cpus used 
 * @return The number of cpus used 
 */
int Node::getusedCPUS() 
{
  return usedCPUS;
}


/**
 * Set the number of cpus used 
 * @param theValue Number of cpus 
 */
void Node::setUsedCPUS( int theValue)
{
  usedCPUS = theValue;
}

};

