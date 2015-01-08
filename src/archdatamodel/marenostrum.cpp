#include <archdatamodel/marenostrum.h>
#include <archdatamodel/bladecenter.h>
#include <assert.h>

namespace ArchDataModel {

/***************************
  Constructor and destructors
***************************/

/**
 * Constructor for the class 
 * @param BladeCenters The BladeCenters for the marenostrum architecture 
 */
MareNostrum::MareNostrum(map<int,BladeCenter*> BladeCenters  )
{
  this->BladeCenters = BladeCenters ;
  this->lastId = BladeCenters.size();
  this->lastId = 0;
  this->totalCPUS = 0;
  this->totalNodes = 0;
}


/**
 * The default constructor for the class
 */
MareNostrum::MareNostrum()
{
  this->lastId = 0;
  this->totalCPUS = 0;
  this->totalNodes = 0;
}


/**
 * The default destructor for the class
 */
MareNostrum::~MareNostrum()
{
}

/***************************
  Auxiliar data functions  
***************************/

/**
 * Adds a bladecenter with a given id to the marenostrum. (ids are unique, if false the assert raises)
 * @param newBladeCenter the new bladecenter of the mn 
 * @param id the id for the blade 
 */
void MareNostrum::addBladeCenter(BladeCenter* newBladeCenter,int id)
{
  assert(!this->hasBlade(id));

  this->BladeCenters.insert(pairBladeCenter(id,newBladeCenter));
  //Despite id is provide we also increment the lastid
  this->lastId++;
}

/**
 * Adds a bladecenter to the marenostrum, the id is automatically assigned
 * @param newBladeCenter the new bladecenter of the mn
 */
void MareNostrum::addBladeCenter(BladeCenter* newBladeCenter)
{
  this->BladeCenters.insert(pairBladeCenter(this->lastId,newBladeCenter));
  //We also increment the lastid
  this->lastId++;
}

/**
 * Returns the bladecenter of the mn with the provided id
 * @param id The id for the bladecenter
 * @return The bladecenter with the provided id
 */
BladeCenter* MareNostrum::getBladeCenter(int id)
{
  map <int, BladeCenter*>:: iterator iter;
 
  iter = this->BladeCenters.find(id);
  BladeCenter* terBlade = (BladeCenter*)(iter -> second);
  
  return terBlade;
}

/**
 * Checks if exists a bladecenter with the provided id
 * @param id The id for the blade 
 * @return True if exists, false otherwise
 */
bool MareNostrum::hasBlade(int id)
{
  if(this->BladeCenters.count(id) > 0)
    return true;
  else
    return false;
}

/***************************
   Sets and gets functions  
***************************/

/**
 * Returns the hashmap with the blades of the mn
 * @return The hashmap with bladecenters
 */
map< int, BladeCenter* > MareNostrum::getBladeCenters() 
{
  return BladeCenters;
}


/**
 * Sets the bladecenters to the mn, the olds are lost
 * @param theValue The hasmap with the blades 
 */
void MareNostrum::setBladeCenters( map< int, BladeCenter* >& theValue)
{
  BladeCenters = theValue;
  this->lastId = theValue.size();
}

/**
 * Setts the total number of cpus of the mn instantiation 
 * @param theValue an integer with the number of cpus 
 */
void MareNostrum::setTotalCPUS(int theValue)
{
  this->totalCPUS = theValue;
}

/**
 * Returns the number of cpus of the architecture
 * @return The number of cpus
 */
int MareNostrum::getTotalCPUS()
{
  return this->totalCPUS;
}

/**
 * Sets the total nodes of the marenostrum 
 * @param theValue The number of nodes in all mn
 */
void MareNostrum::setTotalNodes(int theValue)
{
  this->totalNodes = theValue;
}

/**
 * Returns the number of  nodes in the marenostrum
 * @return an integer with the totalNodes
 */
int MareNostrum::getTotalNodes()
{
  return this->totalNodes;
}

/**
 * Returns the number of blades of the architecture
 * @return the number of blades 
 */
int MareNostrum::getTotalBlades()
{
  return this->BladeCenters.size();
}

/**
 * Returns the node that has the provide cpu 
 * @param cpuId The global id for the cpu 
 * @return The number of node that has the cpu
 */
int MareNostrum::getNodeIdCpu(int cpuId)
{
  map<int,int>::iterator node = this->GlobalCPUIdNodeMapping.find(cpuId);
  
  if(node != this->GlobalCPUIdNodeMapping.end())
    return (int) node->second;
  else
    return -1;
}

/**
 * Sets the node that holds a given cpu with a given id
 * @param cpuId The global id for the cpu 
 * @param nodeId The global id for the node that has the cpu 
 */
void MareNostrum::setNodeIdCpu(int cpuId,int nodeId)
{
  this->GlobalCPUIdNodeMapping.insert(pairNodeCpuMap(cpuId,nodeId));
}

/**
 * Returns the node with the global id providede
 * @param id The id for the blade 
 * @return A reference to the blade 
 */
Node* MareNostrum::getNodeWithGlobalId(int id)
{
  map<int,Node*>::iterator it = this->GlobalIdNodeMapping.find(id);
  
  if(it == this->GlobalIdNodeMapping.end())
   return 0;
  else
   return (Node*) it->second;
}

/**
 * Sets the global id of a given node 
 * @param node A reference to the node 
 * @param nodeId The global id of the node 
 */
void MareNostrum::setGlobalNodeId(Node* node,int nodeId)
{
  this->GlobalIdNodeMapping.insert(pairNode(nodeId,node));
}

void MareNostrum::reconfigureByNode(int nodeId,int type)
{

// DO RECONFIGURATION!!


  map <int, BladeCenter*>:: iterator iter;
 
  iter = this->BladeCenters.find(0);
  BladeCenter* old = (BladeCenter*)(iter -> second);
  
  old->reconfigureByNode(nodeId, type);

  this->BladeCenters.erase(0);
  this->BladeCenters.insert(pairBladeCenter(0,old));


}


}
