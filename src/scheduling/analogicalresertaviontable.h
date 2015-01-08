#ifndef SIMULATORANALOGICALRESERTAVIONTABLE_H
#define SIMULATORANALOGICALRESERTAVIONTABLE_H


#include <scheduling/analogicalbucket.h>
#include <scheduling/analogicaljoballocation.h>

#include <scheduling/reservationtable.h>


#include <archdatamodel/marenostrum.h>
#include <utils/mnconfiguration.h>
#include <scheduling/sharedshadow.h>
#include <scheduling/virtualanalogicaljoballocation.h>
#include <scheduling/metric.h>
#include <scheduling/schedulingpolicy.h>







#include <set>
#include <vector>
#include <deque>
#include <list>
#include <assert.h>

using namespace std;

using std::vector;
using std::set;
using std::deque;
using std::list;

namespace Simulator {

/** This struct implements the order function for the set of buckets ordered by the startime of the buckets. In case of same startime the buckets are compared with its id.*/
struct ABucket_StartTime_lt_t {
    bool operator() (AnalogicalBucket* b1, AnalogicalBucket* b2) const {
	
	double x = b1->getstartTime();
	double y = b2->getstartTime();
	
	assert( x >= 0 && y >= 0 );

	if( x != y )
	    return x < y;
	else if(b1->getcpu() != b2->getcpu())
	    return b1->getcpu() < b2->getcpu();
	else
	   return b1->getId() < b2->getId();
    }
};

/** This struct implements the order function for the set of buckets ordered by the cpus used 
  * the order in this set is based in three criterias:
  *  - by its starttime
  *  - by its jobs identifier
  *  - finally by its identifier that must be unique	
 */
struct ABucket_CpuNumber_lt_t {
    bool operator() (AnalogicalBucket* b1, AnalogicalBucket* b2) const {
	
	double x = b1->getcpu();
	double y = b2->getcpu();			
	
	assert( x >= 0 && y >= 0 );

	if(x != y)
	  return x < y;
	else if(b1->getjob()->getJobNumber() != b2->getjob()->getJobNumber())	  
	  //if not we order by it's job id
	  return b1->getjob()->getJobNumber() < b2->getjob()->getJobNumber();
	else 
	   //finally sorted by its id..
	   return b1->getId() < b2->getId();
    }
};

/** Defines a BucketSetOrdered that is a set of analogicalbucket ordered with the startime */
typedef set<AnalogicalBucket*, ABucket_StartTime_lt_t> BucketSetOrdered;
/** Defines a BucketSetOrdered that is a set of analogicalbucket ordered with the processors */
typedef set<AnalogicalBucket*, ABucket_CpuNumber_lt_t> BucketSetCPUOrdered;
/** Defines a pair of job and job allocation */
typedef pair<Job*, JobAllocation*> pairJobAlloc;
/** Defines a pair of job and AnalogicalBucket */
typedef pair<AnalogicalBucket*, bool> pairBucketBool;


/**
 * @author Francesc Guim,C6-E201,93 401 16 50,
*/


/**  
 * This function implements a analogical reservationtable. This will be used by the scheduler for map jobs in the cpus along the time. 
 * IMPORTANT ! ALWAYS THAT THE STARTTIME FOR A GIVEN BUCKET CHANGES IT MUST BE ERASED AND REINSERTED IN THE LIST 
 * IF NOT , THE STRIC ORDER OF THE FOLLOWING FUNCTION MAY LOSE ITS FUNCTIONALITY 
 *  
 * USE void updateStartTimeFreeBucket(AnalogicalBucket* bucket,double startTime) for update safely the bucket startime for an already inserted bucket
*/
class AnalogicalResertavionTable : public  ReservationTable {
public:
    
  /* Constructors and desctructors*/
  AnalogicalResertavionTable(ArchitectureConfiguration* theValue,Log* log,double globaltime);
  AnalogicalResertavionTable();
  virtual ~AnalogicalResertavionTable();
  
  
  /* auxiliar functions mainly used by the deepsearch policy*/
  
  void allocateBucket(double starttime,double endtime);

  /* Main and auxiliar methods */
  virtual bool allocateJob(Job* job, JobAllocation* allocation); /* Inherited from reservation table - see there */
  virtual bool deAllocateJob(Job* job); /* Inherited from reservation table - see there */
  virtual bool killJob(Job* job);
  virtual bool extendRuntime(Job* job,double length);
  virtual bool reduceRuntime(Job* job,double length);
  virtual int  getNumberCPUSUsed();
  
  AnalogicalBucket* findLowerBound(BucketSetOrdered* bucketSet,AnalogicalBucket* current);
  AnalogicalBucket* findUpperBound(BucketSetOrdered* bucketSet,AnalogicalBucket* current);   
  
  
  /* auxuliar methods */    
  BucketSetOrdered findFirstBucketCpus(double time,double length);   
  AnalogicalBucket* findFirstBucketProcessor(double time, double length, int processor);
//RUTGERS
  AnalogicalJobAllocation* findDcFirstVmAllocation(BucketSetOrdered* suitablebuckets,double length,int numcpus,double initialShadow, int type, SchedulingPolicy* sched);//PowerConsumption* power);
  AnalogicalJobAllocation* findFastMethodAllocation(BucketSetOrdered* suitablebuckets,double length,int numcpus,double initialShadow);
  /* this tries to find out the allocation that allocates the processes as closer as possible */
  AnalogicalJobAllocation* findFastConsecutiveMethodAllocation(BucketSetOrdered* suitablebuckets,double length,int numcpus,double initialShadow);
  bool shareEnoughTime(AnalogicalBucket* bucket1, AnalogicalBucket* bucket2,double length);
  void freeAllocation(AnalogicalJobAllocation* allocationForCheckingReq);
  
  vector<AnalogicalBucket*> CPUMapping; /**< current allocation to job cpu*/
  void setLastBucketId(const int& theValue);
  int getlastBucketId() const;
  vector<BucketSetOrdered*>* getallocations() ;
  vector<BucketSetOrdered*>* getbuckets() ;
 
  //we keep it public for some problems i have with the copy operator of the virtual reservation table, to be fixed and moved to the protected domain 
  
  vector<BucketSetOrdered*> buckets; /**< the analogical reservation table is represented by a vector of sets of buckets that are currently free */ 
  vector<BucketSetOrdered*> allocations; /**< this variable contains the current allocations for the jobs*/
  vector<BucketSetOrdered*> globalView;  /**< global view both type of buckets */  
 
protected: 
 
 int lastBucketId;  /**< indicates the last id assigned to the buckets */
   
 void dumpBucketSet(BucketSetOrdered* setBuckets); 
 void checkBucketSet(BucketSetOrdered* setBuckets);  
  
 void updateStartTimeFreeBucket(AnalogicalBucket* bucket,double startTime);/**< function that updates safely a bucket already inserted in the local sets - avoids violate the strict order of the set*/
	
};

}

#endif
