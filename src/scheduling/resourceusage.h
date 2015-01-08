#ifndef SIMULATORRESOURCEUSAGE_H
#define SIMULATORRESOURCEUSAGE_H

#include <utils/architectureconfiguration.h>

using namespace Utils;

namespace Simulator {

/**
	@author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/
/**
*   This class is a conveyor about the status of a given resource/host in a given timestamp. 
*/
class ResourceUsage{
public:
  ResourceUsage();
  ~ResourceUsage();
  void setNumberNodesUsed ( int theValue );
  int getNumberNodesUsed() const;
  void setNumberCpusUsed ( int theValue );
  int getNumberCpusUsed() const;
  void setNodeSizeUsed ( int theValue );
  int getnodeSizeUsed() const;
  void setMemoryUsed ( double theValue );
  double getMemoryUsed() const;
  void setDisKSizeUsed ( double theValue );
  double getdisKSizeUsed() const;
  void setArchitecture ( ArchitectureConfiguration* theValue );
  ArchitectureConfiguration* getarchitecture() const;	

private: 
   ArchitectureConfiguration* architecture;
   int NumberCpusUsed; /**< The total number of processors of the architecture used */
   int nodeSizeUsed; /**< The size of the nodes -- not always valid (see heterogeneous resources) used */
   int NumberNodesUsed; /**< The total number of nodes in the architecture used  */   
   
   double disKSizeUsed; /**< The amount of terabytes of capacity of the host used  */
   double MemoryUsed; /**< The global amount of Memory - this field is an agregated of all the information, in some situations may be it make no sense. used  */

};

}

#endif
