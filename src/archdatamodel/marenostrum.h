#ifndef MNSIMULATORMARENOSTRUM_H
#define MNSIMULATORMARENOSTRUM_H

#include <map>

#include <archdatamodel/bladecenter.h>
#include <archdatamodel/node.h>


using namespace std;
using std::map;

namespace ArchDataModel {

//type defs for the list stuff
typedef pair <int, BladeCenter*> pairBladeCenter;
typedef pair <int,int> pairNodeCpuMap;
typedef pair <int, Node*> pairNode;

/**
@author Francesc Guim,C6-E201,93 401 16 50,
*/

/** This class implements the definition for the marenostrum architecutre */
class MareNostrum{
public:
  //Constructors and destructors
  MareNostrum(map<int,BladeCenter*> BladeCenters);
  MareNostrum();
  ~MareNostrum();
  
  //Auxiliar methods
  void addBladeCenter(BladeCenter* newBladeCenter,int id);
  void addBladeCenter(BladeCenter* newBladeCenter);
  BladeCenter* getBladeCenter(int id);
  bool hasBlade(int id);
  
  //Sets and gets methods
  void setBladeCenters(map< int, BladeCenter* >& theValue);	
  map< int, BladeCenter* > getBladeCenters();
  void setTotalCPUS(int theValue);
  int getTotalCPUS();
  void setTotalNodes(int theValue);
  int getTotalNodes();
  int getTotalBlades();
  int getNodeIdCpu(int cpuId);
  void setNodeIdCpu(int cpuId,int nodeId);
  Node* getNodeWithGlobalId(int id);
  void setGlobalNodeId(Node* node,int nodeId); //having a binding of a global id..

  void reconfigureByNode(int nodeId, int type);

private:
  //Capacity of mernostrum -- future stuff should model the optical fibre network!
  map<int,BladeCenter*> BladeCenters; /**< Contains a hashmap table with all the blades available on the system*/
  
  int lastId; /**< Last id assigned */
    
  int totalCPUS; /**< In total cpus */
  
  int totalNodes; /**<   the number of total nodes  */
  
  map<int,int> GlobalCPUIdNodeMapping; /**< Mapping global cpu id node */
  map<int,Node*> GlobalIdNodeMapping; /**< Mapping global cpu id node */
};

}

#endif
