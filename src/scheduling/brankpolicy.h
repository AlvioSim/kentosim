#ifndef SIMULATORBRANKPOLICY_H
#define SIMULATORBRANKPOLICY_H


#include <scheduling/schedulingpolicy.h>
#include <utils/fcfsjobqueue.h>
#include <predictors/predictor.h>

#include <scheduling/jobrequirement.h>
#include <scheduling/aggregatemetabrokeringinfo.h>
#include <scheduling/metabrokeringsystem.h>

#include <list>
#include <vector>
#include <map>

using namespace std;
using namespace PredictionModule;
using std::list;
using std::vector;
using std::map;

namespace Simulator {

/**
* @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

typedef pair<Job*,SchedulingPolicy*>PairSubmission;
typedef pair<SchedulingPolicy*,int> PairSubmitedJobs;




class reducedAggregated{
public:
  reducedAggregated();
  //~reducedAggregated();

  void setCS(string theValue);
  string getCS();
  void setOS(string theValue);
  string getOS();
  void setFS(string theValue);
  string getFS();


private:

  string CS;
  string OS;
  string FS;

};



/** 
* Implementes BRANK Policy - IVAN POSA AQUI L'ABSTRACT DE LA SEVA DEFINICIO !! QUAN TINGUIS EL PAPER ESCRIT !
*
*
*
*    TO BE DONE!!
*
*/
class BRANKPolicy : public SchedulingPolicy{
public:
  BRANKPolicy();
  BRANKPolicy(Log* log,bool UsePredictionService,Predictor* predictionService);
  ~BRANKPolicy();
  
  /* inherited methods from the scheduling abstract policy */ 
  double getRANK(double resource_data, double job_data, operator_t op, int factor);

  double getJobRank(Job* job);

  void jobAdd2waitq_REGULAR_LOCAL(Job* job);

  void jobAdd2waitq_REGULAR(Job* job);
  void jobAdd2waitq_AGGREGATED(Job* job);
  void jobAdd2waitq_RANDOM(Job* job);
  void jobAdd2waitq_RR(Job* job);

  ResourceAggregatedInfo* findAggregatedResource(AggregateMetabrokeringInfo* info, string resourceName);

  virtual void jobAdd2waitq(Job* job); //Adds a given job to the wait queue, usually when it arrives to the system
  virtual void jobWaitq2RT(Job* job); //Moves a job from teh wait queue to the reservation table, using an specified set of resources
  virtual void jobRemoveFromRT(Job* job); //Removes a job from the RT
  virtual Job* jobChoose(); //Chooses the more appropiate job to run 
  virtual void jobStart(Job* job); //Starts a job    
  virtual  double getJobsIntheWQ();
  virtual  double getBackfilledJobs();
  virtual double getLeftWork();
  virtual double getJobsIntheRQ();
  virtual Metric* EstimatePerformanceResponse(metric_t MetricTpye,Job* job); 
  virtual int  getNumberCPUSUsed();
  
  virtual void setSim(Simulation* theValue); //the policy has to propagate the simulator link to all the instantiated policies
  virtual void setGlobalTime(double globaltime); //the same
  
 
  void setBRANKPolicyArchitectures(vector< ArchitectureConfiguration * >* theValue);
  vector< ArchitectureConfiguration * >* getBRANKPolicyArchitectures() const;
 
  void setBRANKPolicyParaverTracesName(vector< string >* theValue);
  vector< string >* getBRANKPolicyParaverTracesName() const;
  
  void setBRANKPolicyPolicies(vector< SchedulingPolicy * >* theValue);
  vector< SchedulingPolicy * >* getBRANKPolicyPolicies() const;
  
 
  void setBRANKPolicyCentersName(vector< string >* theValue);
  vector< string >* getBRANKPolicyCentersName() const;
  
  void setBRANKPolicyEmulateCPUFactor(vector< double >* theValue);
  vector< double >* getBRANKPolicyEmulateCPUFactor() const;	

  virtual void PostProcessScheduling(); //the post processing function will generate all the paraver traces 

  void setMetaSystem ( MetaBrokeringSystem* theValue );
  MetaBrokeringSystem* getmetaSystem() const;
	
  AggregateMetabrokeringInfo* getAggregateMetabrokeringInfo();
  AggregateMetabrokeringInfo* getAggregateMetabrokeringInfoSimple();
  AggregateMetabrokeringInfo* getAggregateMetabrokeringInfoCategorized();
  vector <reducedAggregated*>* getSubvector(vector <reducedAggregated*>* data, string theValue);

private:
  vector<SchedulingPolicy*>* BRANKPolicyPolicies; /**<  Vector with all the centers that are simulated in the grid */
  vector<ArchitectureConfiguration*>* BRANKPolicyArchitectures;/**<  Vector with all the arechitectures used by each policy */
  vector<string>* BRANKPolicyParaverTracesName;/**<  The paraver trace file names of each center if required to be generated */
  vector<string>* BRANKPolicyCentersName;/**<  The name of each center being simulated (each policy and architecture belong to one center )*/
  vector<double>* BRANKPolicyEmulateCPUFactor;/**<  The cpu factora that has to be applied to each center in the simulation*/
  
  map<SchedulingPolicy*, int> SubmittedJobs;/**<  The number of submitted jobs to the given policy */
  
  
  JobQueue* waitQueue;/**< the wait queue */
  map<Job*,SchedulingPolicy*> submissions;/**<  Hasmap that indicates where the jobs have been submitted*/
  
  bool UsePredictionService; /** Indicates if the prediction service has to be used for estimate the job runtime in a given center */
  Predictor* predictionService; /** Contains the prediction service that will be used for estimate the job. */

  //This is used when evaluating the job meta-brokering environament 
  MetaBrokeringSystem* metaSystem; /**< This is a component that represents the global metaSystem - it should be notified each time a forward is done, and it provides information about the other brokering entities available in the system */
  
};




/*
 cpuType_num = 0;

  numProc_min = 0;
  numProc_max = 0;
  numProc_sum = 0;

  cpuSpeed_min = 0;
  cpuSpeed_max = 0;
  cpuSpeed_sum = 0;

  cpuUtil_min = 0;
  cpuUtil_max = 0;
  cpuUtil_sum = 0;
*/

class ComputingSystemInfo{
public:
  ComputingSystemInfo();
  ~ComputingSystemInfo();

  void setProcType (string);
  string getProcType (void);

  void setNum (long);
  long getNum (void);

  void setNumProc_min (long);
  long getNumProc_min (void);

  void setNumProc_max (long);
  long getNumProc_max (void);

  void setNumProc_sum (long);
  long getNumProc_sum (void);	

  void setCpuSpeed_min (long);
  long getCpuSpeed_min (void);	

  void setCpuSpeed_max (long);
  long getCpuSpeed_max (void);	

  void setCpuSpeed_sum (long);
  long getCpuSpeed_sum (void);	

  void setCpuUtil_min (long);
  long getCpuUtil_min (void);	

  void setCpuUtil_max (long);
  long getCpuUtil_max (void);

  void setCpuUtil_sum (long);
  long getCpuUtil_sum (void);

  //void insertOSRelation(string);

private:

  string procType;

  long num;

  long numProc_min;
  long numProc_max;
  long numProc_sum;

  long cpuSpeed_min;
  long cpuSpeed_max;
  long cpuSpeed_sum;

  long cpuUtil_min;
  long cpuUtil_max;
  long cpuUtil_sum;

  //vector os OSTypes
  //vector<string>* OSs;

};


class OperatingSystemInfo{
public:
  OperatingSystemInfo();
  ~OperatingSystemInfo();

  
  void setOsType (string);
  string getOsType (void);

  void setNum (long);
  long getNum (void);

  void setTotalMem_min (long);
  long getTotalMem_min (void);	

  void setTotalMem_max (long);
  long getTotalMem_max (void);

  void setTotalMem_sum (long);
  long getTotalMem_sum (void);

  void setFreeMem_min (long);
  long getFreeMem_min (void);	

  void setFreeMem_max (long);
  long getFreeMem_max (void);

  void setFreeMem_sum (long);
  long getFreeMem_sum (void);

private:

  string osType;

  long num;

  long totalMem_min;
  long totalMem_max;
  long totalMem_sum;

  long freeMem_min;
  long freeMem_max;
  long freeMem_sum;

};



class FileSystemInfo{
public:
  FileSystemInfo();
  ~FileSystemInfo();

	void setNum ( int theValue );
	int getNum();

	void setTotalStorageCapacity_min ( long theValue );
	long getTotalStorageCapacity_min();
	void setTotalStorageCapacity_max ( long theValue );
	long getTotalStorageCapacity_max();
	void setTotalStorageCapacity_sum ( long theValue );
	long getTotalStorageCapacity_sum();
	void setFreeStorageCapacity_min ( long theValue );
	long getFreeStorageCapacity_min();
	void setFreeStorageCapacity_max ( long theValue );
	long getFreeStorageCapacity_max();
	void setFreeStorageCapacity_sum ( long theValue );
	long getFreeStorageCapacity_sum();	

private:

  int Num;

  long TotalStorageCapacity_min;
  long TotalStorageCapacity_max;
  long TotalStorageCapacity_sum;
  long FreeStorageCapacity_min;
  long FreeStorageCapacity_max;
  long FreeStorageCapacity_sum;

};



class completeAggregated{
public:
  completeAggregated();
  ~completeAggregated();

  void setCS ( ComputingSystemInfo* theValue );
  ComputingSystemInfo* getCS();
  void setOS ( OperatingSystemInfo* theValue );
  OperatingSystemInfo* getOS();
  void setFS ( FileSystemInfo* theValue );
  FileSystemInfo* getFS();

  void insertCS(ResourceAggregatedInfo* cs);
  void insertOS(ResourceAggregatedInfo* os);
  void insertFS(ResourceAggregatedInfo* fs);

  long extractMin(string orig);
  long extractMax(string orig);
  long extractCount(string orig); 
  long extractTotal(string orig);

private:

  ComputingSystemInfo *CS;
  OperatingSystemInfo *OS;
  FileSystemInfo *FS;

  long TotalStorageCapacity_min;
  long TotalStorageCapacity_max;
  long TotalStorageCapacity_sum;
  long FreeStorageCapacity_min;
  long FreeStorageCapacity_max;
  long FreeStorageCapacity_sum;

};



}

#endif
