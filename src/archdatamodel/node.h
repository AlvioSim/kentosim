#ifndef MNSIMULATORNODE_H
#define MNSIMULATORNODE_H


namespace ArchDataModel {

  //Foward declaration
  class BladeCenter;

/**
@author Francesc Guim,C6-E201,93 401 16 50,
*/

/** This class implements the node model  for a mn architecture (may be usefull for other architectures) */
class Node{
public:

  //Constructors and destructors
  Node(double EthernetBW, double MemoryCapcacity, double NetworkBW,double MemoryBW,int NumberCPUS);
  Node();
  ~Node();

  //Sets and gets methods
  void setNetworkBWLoad(double theValue);  
  double getNetworkBWLoad();
  void setNetworkBW(double theValue);  
  double getNetworkBW();
  void setMemoryCapacityLoad(double theValue);  
  double getMemoryCapacityLoad();
  void setMemoryCapacity(double theValue);
  double getMemoryCapacity();
  void setMemoryBWLoad(double theValue);
  double getMemoryBWLoad();
  void setMemoryBW(double theValue);
  double getMemoryBW();
  void setEthernetBW(double theValue);
  double getEthernetBW() ;
  void setEthernetBWLoad(double theValue);
  double getEthernetBWLoad() ;
  void setBladeCenter(BladeCenter* theValue);
  BladeCenter* getbladeCenter();
  void setId(int theValue);
  int getId() ;
  void setNCPUS( int theValue);
  int getNCPUS() ;
  void setLockedCPUS( int theValue);
  int getlockedCPUS() ;
  void setUsedCPUS( int theValue);
  int getusedCPUS() ;
	
	
	
  
	
	

private:
  /*Capacity of the node*/
  double MemoryBW; /**< MBs/second capacity of the Bus Memory*/
  double MemoryCapacity; /**<Size of the main memory in MB*/
  double NetworkBW; /**< MBs/second of the capacity of the fibre that convey the msgs to the main network*/
  double EthernetBW; /**< MBs/second of the capacity of the node etherned */
  
  /* Current load of all resource of the node */   
  double MemoryBWLoad; /**< MBs/second load of the Bus Memory*/
  double MemoryCapacityLoad; /**<Size of the main memory used in MB*/
  double NetworkBWLoad; /**< MBs/second load of the capacity of the fibre that convey the msgs to the main network */
  double EthernetBWLoad; /**< MBs/second load of the capacity of the node etherned */
  
  int NCPUS; /**<Indicates the number of cpus available on the node*/
  int lockedCPUS; /**<Indicates the number of cpus that are locked (no process can use them)*/
  int usedCPUS;  /**<Indicates the number of used cpus at a given moment*/
  
  //References to other resources
  BladeCenter* bladeCenter; /**<The blade center where the node is situated*/
  
  int Id;
  
  
};

}

#endif
