#ifndef SIMULATORMETRIC_H
#define SIMULATORMETRIC_H


#include <cstdlib>
#include <string>
#include <vector>

using namespace std;

namespace Simulator {

/** The metrics that have been defined and that are currently used in the simulator */
enum metric_t {
    BSLD = 0, /* JOB METRICS */
    WAITTIME,
    SLD,
    BSLD_PRT,
    BACKFILLED_JOB, 
    PENALIZED_RUNTIME,
    RESPONSE_TIME,
    PERCENTAGE_PENALIZED_RUNTIME,
    RSP_USED, 
    BACKFILLED_JOBS, /* POLICY METRICS */
    JOBS_IN_WQUEUE,    
    LEAST_WORK_LEFT,
    SUBMITED_JOBS,
    SUBMITED_JOBS_CENTER,    
    LOAD_JOBS_CENTERS,
    kILLED_JOB,   
    RUNNING_JOBS,
    SLD_PREDICTED,
    COMPUTATIONAL_COST,
    PREDICTED_WT,
    PREDICTED_RUNTIME,
    ESTIMATED_COMPUTATIONAL_COST,
    COMPUTATIONAL_COST_RESPECTED,
    SLD_PREDICTED_ERROR,
    PREDICTED_WT_ERROR,
    PREDICTED_RUNTIME_ERROR,
    ESTIMATED_COMPUTATIONAL_COST_ERROR,
    ESTIMATED_START_TIME_FIRST_FIT,
    AVG_SLD_FINISHED_JOBS, /* historical information of the center - oriented to provide some QoS information to the borker level */
    AVG_RT_FINISHED_JOBS,
    AVG_WT_FINISHED_JOBS,
    AVG_RESPT_FINISHED_JOBS,
    MEMORY_AVAILABLE,       /* dynamic  system metrics */
    DISK_AVAILABLE,    
    FREE_CPUS, 
    CPUS_USED,
    VENDOR, /* static system metrics */
    OS_NAME,
    TOTAL_CPUS,
    CLOCK_SPEED,
    DISK_SIZE,
    MEMORY_SIZE,
    MEMORY,
    OTHER_VAR,
};

/**  The statistics that have been defined in the simulator */
enum statistic_t
{
   AVG = 0,
   MEDIAN,
   PERCENTILE95,
   STDEV,
   IQR,
   CONSTANT,
   MIN,
   MAX,
   ACCUMULATE,
   OTHER_STAT,
   COUNT_LITERALS, /* this to be applied to strings */
   ACCUMULATE_LITERALS,
   PERCENTILES,
};

/** The native values that are being currently taked into account in the statistics */
enum native_t
{
   INTEGER = 0,
   DOUBLE,
   STRING,
   COMPOSED_DOUBLE,
   OTHER_NATIVE_T,
};

/**
@author Francesc Guim,C6-E201,93 401 16 50,
*/

/** This class implements the metric definition and basic comparision functions */
class Metric{
public:
  Metric();
  Metric(metric_t type,statistic_t statisticUsed,string name,native_t nativeType);
  ~Metric();
  void setStatisticUsed(const statistic_t& theValue);
  statistic_t getstatisticUsed() const;
  void setType(const metric_t& theValue);
  metric_t gettype() const;
  void setValue(void* theValue);
  void* getvalue() const;
  virtual bool betterThan(Metric* b1);
  virtual bool betterOrEqualThan(Metric* b1) ;
  void setNativeValue(double value);
  void setName(const string& theValue);
  string getname() const;
  void setNativeType(const native_t& theValue);
  native_t getnativeType() const;
  void setEstimatorName(const string& theValue);
  string getestimatorName() const;	
  void setNativeDouble(double theValue);
  double getnativeDouble() const;
  void setNativeBool(bool theValue);
  bool getnativeBool() const;
  void setNativeString(const string& theValue);
  string getnativeString() const;
  void setInputNativeType(const native_t& theValue);
  native_t getInputNativeType() const;
  Metric* getComposedMetric(int i);
  void addComposedMetric(Metric* metric);
	
private:
  metric_t type; /** The metric implemented */
  statistic_t statisticUsed;/** The statitic used for compute this metric  */
  native_t nativeType;/** The native type for value that is conveyed in the metric  */
  native_t InputNativeType; /** The native type for the input values in the metric*/
  string name;/** The name associated to the current metric */
  string estimatorName;/** The estimatorName for the static used for compute the metric */
  void* value;/** A pointer to the value of the metric */
  double nativeDouble;/** The native value for the metric in case that the metric is a double */
  bool nativeBool; /** The native value for the metric in case that the metric is a bool */
  string nativeString;/** The native value for the metric in case that the metric is a string */
  vector<Metric*> otherMetrics; /**< In some situations the metric may be a composed metrics .. */
  
};

}

#endif
