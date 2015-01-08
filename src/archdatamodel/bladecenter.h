#ifndef MNSIMULATORBLADECENTER_H
#define MNSIMULATORBLADECENTER_H

#include <map>


using namespace std;
using std::map;

namespace ArchDataModel {

//forward declaration
class Node;
class MareNostrum;

//type defs for the list stuff
typedef pair <int, Node*> pairNode;

/**
* @author Francesc Guim,C6-E201,93 401 16 50,
*/

/** Class that model a bladecenter of the marenostrum type architecture. */
class BladeCenter{

public:
  

  
  BladeCenter(map<int,Node*> BladeNodes,double IOBW);   
  BladeCenter(double IOBW); 
  BladeCenter();
  ~BladeCenter();  
  
  
  void setBladeNodes(map< int, Node* >& theValue);
  map< int, Node* > getBladeNodes();
  void setIOBW(double theValue);  
  double getIOBW();
  void setIOBWLoad(double theValue);  
  double getIOBWLoad();
  void setMareNostrum(MareNostrum* theValue);
  MareNostrum* getmareNostrum();
  void setId( int theValue);
  int getId() ;
  void setTotalCPUS(int theValue);
  int getTotalCPUS();
  int getTotalNodes();
  
  //Auxiliar data modification methods
  void addNode(Node* newNode,int id);
  void addNode(Node* newNode);
  Node* getNode(int id);  
  bool hasNode(int id);

  void reconfigureByNode(int node, int type);

private:
  //Capacity of the BladeCenter
  map<int,Node*> BladeNodes;  /**<Contains a hasmap table with all the nodes of the blade center, each node is codified by an int */
  double IOBW;  /**<Contains the capacity in MB/Second of the blades switch */
  int totalCPUS;
    
  //Current load of all the resource node
  double IOBWLoad;  /**<Contains the load in MB/Second of the blades switch (BW that is used) */
  
  //Auxiliar variables
  //Contains the last id assigned to the node.. it should go from [0..N]
  int lastId;  /**< last id asinged */
  int id;  /**<the identifier assigned to the blade */
  //References to other resources
  MareNostrum* mareNostrum; /**< Reference to mn */
};

}

#endif
