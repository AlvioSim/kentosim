#ifndef SIMULATORVIRTUALANALOGICALRTABLE_H
#define SIMULATORVIRTUALANALOGICALRTABLE_H

#include <scheduling/analogicalresertaviontable.h>
#include <archdatamodel/marenostrum.h>
#include <utils/mnconfiguration.h>
#include <scheduling/sharedshadow.h>
#include <scheduling/virtualanalogicaljoballocation.h>
#include <scheduling/metric.h>

#include <set>
#include <map>
#include <algorithm>
#include <iterator>

using namespace std;
using std::set;
using std::map;
using namespace ArchDataModel;


namespace Simulator {

/**
* @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

/** Defines a pair of job and runtime extension */
typedef map<int,double> Extensions;

/** Generic pair int int */
typedef pair<int,int> pairInt;

/** Defines an operator of less than between to sharedshadows. Such order is implemented by its start time*/
struct Shadow_StartTime_lt_t {
    bool operator() (SharedShadow* b1, SharedShadow* b2) const {
	
	double x = b1->getstartShadow();
	double y = b2->getstartShadow();
	
	assert( x >= 0 && y >= 0 );

	if(x != y)
	    return x < y;
	else
	   return b1->getId() < b2->getId();
    }
};

/** Defines an operator of order of buckets based on their penalty*/
struct JobPenalty_lt_t {
    bool operator() (Job* b1, Job* b2) const {
	
        //we index using the OldPenalized runtime due to the penalizedruntime filed may changes over different iterations see the source code of EvaluateProcessorAllocation
	double x = b1->getOldPenalizedRunTime();
	double y = b2->getOldPenalizedRunTime();
	
	assert( x >= 0 && y >= 0 );

	if(x != y)
	    return x < y;
	else
	   return b1->getJobNumber() < b2->getJobNumber();
    }
};

/** Defines a set of shadows ordered by its start time */
typedef set<SharedShadow*, Shadow_StartTime_lt_t> ShadowSetOrdered;

/**
* This class implements the virtual reservation table. A virtual reservation table have been mainly designed for evaluate the diferent allocations that can be done in a given point of time. Different virtual reservation table are used , due to each reservation table has also take into account the collisions that jobs allocated in the same shared shadows. 
* @see The class sharedshadow
* @see the class virtualanalogicaljoballocation
*/
class VirtualAnalogicalRTable : public AnalogicalResertavionTable 
{
public:
  VirtualAnalogicalRTable();
  VirtualAnalogicalRTable(ArchitectureConfiguration* configuration,Log* log, double globaltime);
  virtual ~VirtualAnalogicalRTable();
  
  virtual bool deAllocateJob(Job* job); /* Inherited from  AnalogicalResertavionTable reservation table - see there, not allways all the buckets will be freed , depending on the   FreeBucketsOnJobCompletion variable*/
  virtual bool  allocateJob(Job* job,JobAllocation* genericallocation);

  
  /* we overload the operator of = , due to when copying a reservation table we just wanna copy the jobs that are running*/
  VirtualAnalogicalRTable& operator=(const AnalogicalResertavionTable& source);
  VirtualAnalogicalRTable& operator=(const VirtualAnalogicalRTable& source);
  void copyRunningJobs(const AnalogicalResertavionTable& source);
  
  void setAllBuckets(BucketSetOrdered* theValue);
  BucketSetOrdered* getallBuckets() const;  
  //polimorphims of the same function
  AnalogicalBucket* findUsedBucketLowerBoundWithCpu(int cpu,AnalogicalBucket* current);
  AnalogicalBucket* findUsedBucketUpperBoundWithCpu(int cpu,AnalogicalBucket* current);
  AnalogicalBucket* findFreeBucketLowerBoundWithCpu(int cpu,AnalogicalBucket* current);
  AnalogicalBucket* findFreeBucketUpperBoundWithCpu(int cpu,AnalogicalBucket* current);
  //All the allocated buckets in all cpus ... used for computing the conflicts.. we leave it public for facility..
  BucketSetOrdered* allBuckets;
  //Generate a dump with the current allocation .. mainly for compare policies
  void DumpAllocatedBuckets();  
  void DumpAllocatedBuckets(AnalogicalJobAllocation* allocation);  //this is more cost computational that the previous one
  //sets and gets
  void setDumpDefaultName( const string& theValue);
  string getdumpDefaultName() const ;
  void setDumpNumber(const  int& theValue);
  int getdumpNumber() const ;
  void setDumpHeader(const string& theValue);
  string getdumpHeader() const;
	
  //this function checks that no holes have appeared in the buckets , this holes can be created when the new scheduling appears, 
  //and the collisions modify their lenght
  void updateFreeBuckets();
	
  //this allocation tries to allocate the processes for the job in different nodes in case that the required resources are higher that the once that would be assigned per job
  AnalogicalJobAllocation* findLessConsumeMethodAllocation(BucketSetOrdered* suitablebuckets,double length,int numcpus,double initialShadow,Job* job);
  AnalogicalJobAllocation* findLessThresholdConsumeMethodAllocation(BucketSetOrdered* suitablebuckets,double length,int numcpus,double initialShadow,Job* job,double thresshold);
  AnalogicalJobAllocation* findDistributeConsumeMethodAllocation(BucketSetOrdered* suitablebuckets,double length,int numcpus,double initialShadow,Job* job);
  Job* EvaluateProcessorAllocation(VirtualAnalogicalRTable* table, double starttime, double length,Job* job,int processor,double maxPenalty,bool allocate,AnalogicalBucket* bucket);
  //this function computes the desirable number of processes that can be allocated to the same node without punishing to much the rest
  int DecideNumberProcessesPerNode(Job* job,Node* node);
  
  /* the following functions are the core functions for the manage the resource consumption and collision */
  void freeProcessesAllocations();
  void insertTemporalProcess(Job * job);
  void freeTemporalProcess(Job* job,bool allocated);
  ShadowSetOrdered::iterator lower_Shadow_iterator(ShadowSetOrdered* shadowSet,AnalogicalBucket* current);
  void ComputeShadowPenalties(vector<ShadowSetOrdered*>* shadows);
  vector<ShadowSetOrdered*> ComputeShadows(AnalogicalJobAllocation* allocation,Job* ingnoreJob,set<int>* checkOnlyNode);
  void updateFreeBuckets(AnalogicalJobAllocation* allocation);
  double computePenalty(double availableMBW,double availableNBW,double availableETH, double memReq,double netReq, double ethReq,SharedShadow* shadow);  
  Metric* computeAllocationPenalties(AnalogicalJobAllocation* allocation);  //The talbe now is not copied in the computeAlloctionPenalties !! So each time that the metrics are computed based on the shadows, these shadows must be previolusly recomputed !
  bool Overlapping(AnalogicalJobAllocation* allocation);
  Metric* computePerformanceMetric(AnalogicalJobAllocation* allocation);
  void resetPenalties(AnalogicalJobAllocation* allocation,set<int>* OnlyNodes); //this function can be used for clear the penalties associated to the buckets.. this is important when evaluating the different possible outcomes..

  
  AnalogicalJobAllocation* CutBuckets(AnalogicalJobAllocation* allocation,Job* job);

  /* sets and gets stuff*/
  void setFreeBucketsOnJobCompletion(bool theValue);
  bool getFreeBucketsOnJobCompletion() const;
  void setJobList(map< int, Job * >* theValue);
  map< int, Job * >* getJobList() const;
  void setMetricType(const metric_t& theValue);
  metric_t getmetricType() const;
  void setStatisticType(const statistic_t& theValue);
  statistic_t getstatisticType() const;	
	
  //Memory management functions 
  void deleteShadows(vector<ShadowSetOrdered*>* shadows); //frees all the memory used by a set of shadows 
	
	
private:
  bool deleteRemovableAllocations(); //deletes all the allocations for the finished jobs that won't interfere with the current allocation of jobs that have not finished. Remeber that due to the shadows and penalty computation we can not delete all the jobs, coz may be some of the allocated and finished jobs had collisions with other already running jobs.
  void deleteAllocationFromAllBuckets(AnalogicalJobAllocation* allocation);

 
protected:
  
  Extensions jobExtensions;/**< Contains the last computed extesions for the allocation*/
  string dumpDefaultName; /**< Contains the prefix name of the files where the temporarly allocations will be dump (if required )*/
  int dumpNumber; /**< Indicates the last number of dump generated, used for generate the sufix for the dumps file name  */
  string dumpHeader; /**<  The header that will be stored at the begining of each dump file */
  
  int lastShadowId;/**< The last shadow id assigned, each sharedshadow has a unique id */
  map<int,Job*>* JobList; /**< The reference to the jobs that are being simulated  */
  map<int,Job*>* ProcessJobTempAllocations; /**< The reference to the temporary processes of the jobs that are temporarly allocated */
  
  metric_t metricType;/**< The metric type that wants to be optimized in the current simulation */
  statistic_t statisticType; /**< The statistic used for optmize the current allocations */
  
  bool FreeBucketsOnJobCompletion; /**<  when a job is deallocated, not always it has to be unallocated from the reservation table. This occurs when the job has finished. In this case, depending on the scheduling policy used , it's important to keep the buckets for the job in the reservation talbe coz deleting this buckets may change the collision computation with other still running jobs, while this collision had a real effect.*/ 
  
  map<Job*,bool> finishedJobs; /**< this contains the list of jobs that have already finished and not deallocated from the table, its important to keep those jobs that are afecting the runtime of other running jobs.*/
  double MinSTimeNoFinishedJob; /**< contains the first Start time for all the allocations regarding those jobs that have not finished.*/
};

}

#endif
