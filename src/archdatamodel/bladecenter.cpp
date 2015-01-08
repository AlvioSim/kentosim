#include <archdatamodel/bladecenter.h>
#include <archdatamodel/node.h>
#include <archdatamodel/marenostrum.h>
#include <assert.h>

namespace ArchDataModel {

/***************************
  Constructor and destructors
***************************/

/**
* Constructor for the bladecenter
* @param .BladeNodes A hasmap containing 
* @param .IOBW A double indicating the MB/S capacity for the blade  
*/
BladeCenter::BladeCenter(map<int,Node*> BladeNodes,double IOBW)
{
  //Setting the capacity and static characterisitcs of the BladeCenter
  this->BladeNodes = BladeNodes;
  this->IOBW = IOBW;

  //Setting the dynamic stuff
  this->IOBWLoad = 0;  
  
  //Setting auxiliar variables 
  this->lastId = 0;
  
  this->mareNostrum = 0;
}

BladeCenter::BladeCenter()
{
	
}

/**
* Constructor for the bladecenter
* @param .IOBW A double indicating the MB/S capacity for the blade  
* @see BladeCenter(map<int,Node*> BladeNodes,double IOBW);
*/
BladeCenter::BladeCenter(double IOBW)
{
  //Shouldn't be invoked
  //Setting the capacity and static characterisitcs of the BladeCenter
  this->IOBW = IOBW;

  //Setting the dynamic stuff
  this->IOBWLoad = 0;  
  
  //Setting auxiliar variables 
  this->lastId = 0;
  this->mareNostrum = 0;
}

BladeCenter::~BladeCenter()
{
}

/***************************
  Auxiliar data functions  
***************************/

/**
 * Function that adds a computational node to the blade 
 * @param newNode A reference to de node 
 * @param id The id for the node. The application will fail if a node with the same id has been added to the blade.
 */
void BladeCenter::addNode(Node* newNode,int id)
{  
  assert(!this->hasNode(id));  /*check that the node does not exist*/
  this->BladeNodes.insert(pairNode(id,newNode));
  //Despite id is provide we also increment the lastid
  this->lastId++;
}

/**
 * Function that adds a compuatational node to the blade and automatically asigns the id
 * @param newNode A reference to de node 
 */
void BladeCenter::addNode(Node* newNode)
{
  this->BladeNodes.insert(pairNode(this->lastId,newNode));
  //We also increment the lastid
  this->lastId++;
}

/**
 * Returns the node of the blade with the id
 * @param id The id of the requested node
 * @return Returns the node with the provided id
 */
Node* BladeCenter::getNode(int id)
{
  map <int, Node*>:: iterator iter;
 
  iter = this->BladeNodes.find( id);
  Node* terJob = (Node*)(iter -> second);
  
  return terJob;
}

/**
 * Checks if there is a node with the local id provided 
 * @param id The id of the requested node
 * @return True if exists, false otherwise
 */
bool BladeCenter::hasNode(int id)
{
  if(this->BladeNodes.count(id) > 0)
   return true;
  else
   return false;
}
/***************************
   Sets and gets functions  
***************************/

/**
 * Returns a hasmap with all the nodes of the blade 
 * @return hasmap with the nodes
 */
map< int, Node* > BladeCenter::getBladeNodes() 
{
  return BladeNodes;
}


/**
 * Sets the nodes of the blades, the already asigned nodes are lost 
 * @param Blades the hasmap with the blades 
 */
void BladeCenter::setBladeNodes(map< int, Node* >& theValue)
{
  BladeNodes = theValue;
  //We assign the last id to the size of the map 
  this->lastId = BladeNodes.size();
}


/**
 * Returns the IOBW of the blade 
 * @return IOBW in MB/S
 */
double BladeCenter::getIOBW() 
{
  return IOBW;
}


/**
 * Sets the IOBW of the blade 
 * @param theValue IOBW in MB/S
 */
void BladeCenter::setIOBW(double theValue)
{
  assert(theValue >= 0);
  IOBW = theValue;
}


/**
 * Returns the load of the blade in the current moment 
 * @return IOBW in MB/S
 */
double BladeCenter::getIOBWLoad() 
{
  return IOBWLoad;
}


/**
 * Sets the IOBW of the current moment 
 * @param theValue IOBW in MB/S
 */
void BladeCenter::setIOBWLoad(double theValue)
{
  assert(theValue >= 0);
  IOBWLoad = theValue;
}

/**
 * Returns a reference to the marenostrum archicteure that holds the blade 
 * @return MareNostrum reference
 */
MareNostrum* BladeCenter::getmareNostrum() 
{
  return mareNostrum;
}


/**
 * Sets the marenostrum that holds the Blade 
 * @param theValue the mareNostrum reference
 */
void BladeCenter::setMareNostrum(MareNostrum* theValue)
{
  mareNostrum = theValue;
}

/**
 * Returns the id of the blade 
 * @return The id 
 */
int BladeCenter::getId() 
{
  return id;
}


/**
 * Sets the id of the blade 
 * @param theValue The id
 */
void BladeCenter::setId(int theValue)
{
  id = theValue;
}

/**
 * Sets the total number fo cpus of the blade 
 * @param theValue total cpus 
 */
void BladeCenter::setTotalCPUS(int theValue)
{
  this->totalCPUS = theValue;		
}


/**
 * Returns all the cpus of the blade
 * @return The number of cpus 
 */
int BladeCenter::getTotalCPUS()
{
  return this->totalCPUS;
}		
		
/**
 * Returns the number of nodes of the blade 
 * @return the number of nodes
 */
int BladeCenter::getTotalNodes()
{
  return this->BladeNodes.size();
}


void BladeCenter::reconfigureByNode(int node, int type)
{

  map <int, Node*>:: iterator iter;
 
  iter = this->BladeNodes.find(node);
  Node* theNode = (Node*)(iter -> second);
  theNode->setNetworkBW(type);
  

  this->BladeNodes.erase(node);
  this->BladeNodes.insert(pairNode(node,theNode));
}
 
}
