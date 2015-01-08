#ifndef SIMULATORPOLICYENTITYCOLLECTOR_H
#define SIMULATORPOLICYENTITYCOLLECTOR_H

#include <scheduling/schedulingpolicy.h>


#include <vector>

using namespace std;
using std::vector;

namespace Simulator {

/**
* @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

/** 
* This function collects information form the policy each time that is required. This collector is used for make a sampling for several policy metrics of the system.
*/
class PolicyEntityCollector{
public:
  PolicyEntityCollector(SchedulingPolicy* policy,Log* log);
  ~PolicyEntityCollector();
  
  void pullValues(double timeStamp); //gathers all the values from the policy 
 
  vector<double>* getbackfilledJobs();
  vector<double>* getnumberJobsInTheQueue();
  vector<double>* getnumberCPUSUsed();
  vector<double>* getrunningJobs();
  vector<double>* getleftWork();
  vector<double>* gettimeStamp();
  vector<string>* getcenterName();
  
  Log* log;

protected: 
  SchedulingPolicy* policy;
  
public:  
  vector<double> backfilledJobs; /**< number of backfilled jobs in each interval of time*/
  vector<double> numberJobsInTheQueue; /**< number of the jobs that where waitting in the queue*/
  vector<double> numberCPUSUsed; /**< Tne number of cpus that are currenty used */
  vector<double> leftWork; /**< The amount of pending work at the current point of time **/
  vector<double> runningJobs; /**< The number of runing jobs at the current point */
  vector<double> timeStamp; /**< The timestap when a given collection has been done */
  vector<string> centerName; /**< The center where the collection has been obtained  */
  
  double lastNumberOfBackfilledJobs; /**< The last number of backfilledJobs the last time that was requested (used for compute the amount of backfilledJobs since the last query)*/

  void pullBrankInfo(SchedulingPolicy* policy,double timeStamp);
  
};

}

#endif
