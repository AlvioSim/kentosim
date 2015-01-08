#ifndef SIMULATORMNRESOURCEUSAGE_H
#define SIMULATORMNRESOURCEUSAGE_H

#include <scheduling/job.h>
#include <scheduling/resourceusage.h>

#include <map>

using namespace std;
using std::map;



namespace Simulator {

/**
* @author Francesc Guim,C6-E201,93 401 16 50,
*/

/** * This class contains information about the node resource usage in a given moment of time for a set of jobs. Its a possible picture of how will be the usage of a node in the future time.. a MNResourceUsage is assocaited to the buckets associated to the node in a given period of time.*/
class MNResourceUsage : public ResourceUsage{
public:
	
  MNResourceUsage();
  ~MNResourceUsage();

  void setNetworkBWLoad(double theValue);  
  double getNetworkBWLoad();
  void setMemoryCapacityLoad(double theValue);  
  double getMemoryCapacityLoad();
  void setMemoryBWLoad(double theValue);
  double getMemoryBWLoad();
  void setEthernetBWLoad(double theValue);
  double getEthernetBWLoad() ;
  void setNumberOfJobs(  int theValue);
  int getNumberOfJobs()  ;	 
  
  
  map<int,Job*> assignedJobs; /**< Map containing all the jobs allocated to this node at this bucket time */
  
private:
  /* Current load of all resource of the node */   
  double MemoryBWLoad; /**< MBs/second load of the Bus Memory */
  double MemoryCapacityLoad;  /**<Size of the main memory used in MB*/
  double NetworkBWLoad;  /**< MBs/second load of the capacity of the fibre that convey the msgs to the main network */
  double EthernetBWLoad;  /**< MBs/second load of the capacity of the node etherned   */
  int NumberOfJobs;  /**< the number of jobs assigned in a given moment to this node */
  

};

}

#endif
