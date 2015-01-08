#ifndef SIMULATORSHAREDSHADOW_H
#define SIMULATORSHAREDSHADOW_H

#include <scheduling/analogicalbucket.h>

#include <map>

using namespace std;
using std::map;



namespace Simulator {

typedef map<int,vector<AnalogicalBucket*>*> JobBucketsMapping;
typedef pair <int, vector<AnalogicalBucket*>*> pairJobBuckets;

/**
* @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

/**
* A shared shadow is a container that holds a set of buckets that shares an mount of time. (the hole time) in the same computational indivisible resource: normaly it would be a node, howerver it may modelize other architectures. This class is used for compute the collision of resource usage with a set of jobs. 
*/
class SharedShadow{
public:
  SharedShadow(double startime, double endtime,int numnode); 
  ~SharedShadow();
  void setStartShadow(double theValue);
  double getstartShadow() const;
  void setEndShadow(double theValue);
  double getendShadow() const;
  void insert(AnalogicalBucket* bucket);
  int getId();
  void setId(int id);
  int getNumberProcesses();
  int getJobNumberProcesses(int jobid);
  void setNumberJobs(const int& theValue);
  int getnumberJobs() const;
  void setTotalBuckets(const int& theValue);
  int gettotalBuckets() const;
  void setMbwShReq(double theValue);
  double getmbwShReq() const;
  void setNbwShReq(double theValue);
  double getnbwShReq() const;
  void setEthShReq(double theValue);
  double getethShReq() const;
  
  SharedShadow& operator=(const SharedShadow& source);


  //we keep this variable as public for simplicity
  JobBucketsMapping buckets;  /**< All the buckets that belongs to the shadow (note that several of the buckets of the shadow may belong to the same job ) */
  
private:
  double startShadow; /**< The start time for the shadow */
  double endShadow;  /**< The end time for the shadow */
  int numberJobs; /**<  The number of jobs that are included in the shadow */
  int numnode; /**< The node (or in fact, the id for the computational resource), to where the buckets are stored*/
  int id; /**< The global id for the shadow */
  int totalBuckets;  /**< */
  
  //resources demanded by the current shadow 
  double ethShReq; /**< The amount of ethernet BW  consumed by all the jobs in the shadow */
  double mbwShReq; /**< The amount of memory BW  consumed by all the jobs in the shadow */
  double nbwShReq; /**< The amount of network BW  consumed by all the jobs in the shadow */
};

}

#endif
