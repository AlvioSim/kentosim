#ifndef SIMULATORSIMULATIONCONFIGURATION_H
#define SIMULATORSIMULATIONCONFIGURATION_H

/* Auxiliar Sttuff includes */
#include <scheduling/swftracefile.h>
#include <scheduling/swftraceextended.h>
#include <scheduling/swfgridtrace.h>
#include <utils/log.h>
#include <utils/paravertrace.h>
#include <utils/utilities.h>

/* Simulation stuff includes */
#include <scheduling/simulation.h>
#include <scheduling/simulatorcharacteristics.h>

/* simulation statistics and output files stuff */
#include <statistics/statistics.h>
#include <utils/cvsjobsinfoconverter.h>
#include <utils/jobcsvrscriptanaliser.h>
#include <utils/csvpolicyinfoconverter.h>
#include <utils/rscript.h>

/*Architecture includes */
#include <utils/mnconfiguration.h>
#include <archdatamodel/node.h>
#include <archdatamodel/bladecenter.h>
#include <archdatamodel/marenostrum.h>

/*Parsing and configuration includes */

#include <utils/arg_parser.h>
#include <utils/configurationfile.h>
#include <statistics/statisticsconfigurationfile.h>
#include <utils/predictorconfigurationfile.h>

/*The policies includes */
#include <scheduling/fcfsschedulingpolicy.h>
#include <scheduling/easyschedulingpolicy.h>
#include <scheduling/deepsearchcollisionpolicy.h>
#include <scheduling/easypredschedulingpolicy.h>
#include <scheduling/gridbackfilling.h>
#include <scheduling/isisdispatcher.h>
#include <scheduling/easywithcollisionschedulingpolicy.h>
#include <scheduling/rua-backfilling.h>
#include <scheduling/brankpolicy.h>
#include <scheduling/metabrokeringsystem.h>
#include <scheduling/powerawarebackfilling.h>

/* prediction stuff */
#include <predictors/predictor.h>
#include <predictors/userruntimeestimatepred.h>
#include <predictors/preddeadlinemissmanager.h>
#include <predictors/preddeadlinemissmanagerexp.h>
#include <predictors/preddeadlinemissmanagergradual.h>
#include <utils/configurationfile.h>
#include <predictors/historicalifier.h>

/* xercesc and xalanc stuff */

#include <utils/utilities.h>
#include<sstream>
/*STUF INCLUDES RELATED TO XALAN*/

#include <xalanc/Include/PlatformDefinitions.hpp>


#include <cassert>

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xalanc/PlatformSupport/XSLException.hpp>
#include <xalanc/DOMSupport/XalanDocumentPrefixResolver.hpp>



#include <xalanc/XPath/NodeRefList.hpp>
#include <xalanc/XPath/XObject.hpp>
#include <xalanc/XPath/XPathEvaluator.hpp>



#include <xalanc/XalanSourceTree/XalanSourceTreeDOMSupport.hpp>
#include <xalanc/XalanSourceTree/XalanSourceTreeInit.hpp>
#include <xalanc/XalanSourceTree/XalanSourceTreeParserLiaison.hpp>
#include <xalanc/PlatformSupport/DOMStringHelper.hpp>


using namespace Simulator;
using namespace Utils;
using namespace Datamining;
using namespace xercesc;
using namespace xalanc;

namespace Utils {

/**
* @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

//forward declaration stuff for the xerces and xalan

/** Class that provides the functionalities for accessing to the information of a simulation configuration file*/
class SimulationConfiguration{
public:
  SimulationConfiguration(string ConfigFile,Log* log,ConfigurationFile* simulatorConfiguration);  
  ~SimulationConfiguration();
  SimulationConfiguration();
  void import(SimulationConfiguration* ConfigFile);
  void import(ConfigurationFile* config);
  
  /* auxiliar function for creating complex required objects for the simulation */
  PredDeadlineMissManager* CreateDeadlineManager(DeadlineMissManager_t type);
  Predictor* CreatePredictor(PredictorConfigurationFile* configPred);
  
  ArchitectureConfiguration* CreateArchitecture(Log*);
  ArchitectureConfiguration* CreateArchitecture(Log* log,architecture_type_t architecture,string architectureFile);
  
  TraceFile* CreateWorkload(Log* log);
  TraceFile* CreateWorkload(Log* log,workload_type_t workloadReq,string workloadPathReq,string jobRequirementsFileReq);
  
  CSVJobsInfoConverter* createJobCSVDumper();
  CSVPolicyInfoConverter* createPolicyCSVDumper();
  
  SchedulingPolicy* CreatePolicy(ArchitectureConfiguration*, Log* log, double startime);
  SchedulingPolicy* CreateFCFSPolicy(ArchitectureConfiguration* architectureConfig, Log* log, double startime);
  SchedulingPolicy* CreateEASYPolicy(ArchitectureConfiguration* architectureConfig, Log* log, double startime);
  SchedulingPolicy* CreateEASYPolicy(ArchitectureConfiguration* architectureConfig, Log* log, double startime,string PredictorConfigFileReq, int numberOfReservationsReq,Backfilling_variant_t variantReq, bool AllocateAllJobsInTheRT,bool SimulateResourceUsageCur,double AlternativeThresholdCur, double LessConsumeThreshold, bool tryToBackfillAllways, bool powerAwareBackFilling);
  SchedulingPolicy* CreateDEEPPolicy(ArchitectureConfiguration* architectureConfig, Log* log, double startime);
  SchedulingPolicy* CreateDEEPPolicy(ArchitectureConfiguration* architectureConfig, Log* log, double startime,metric_t DSCPmetricTypeReq,statistic_t DSCPstatisticTypeReq,bool DSCPgenerateAllParaversRTablesReq,string paraverOutPutDirReq,string paraverTraceReq);
  SchedulingPolicy* CreateISISDispatcherPolicy(Log* log,double startime);
  SchedulingPolicy* CreateBRANKPolicy(Log* log,double startime);
  SchedulingPolicy* CreateMetaBrokeringSystem(Log* log,double startime);
  vector<RScript*>* CreateJobCSVRAnalyzers(CSVJobsInfoConverter* csvfile,ArchitectureConfiguration* arch);
  
  
  
private:
  //stuff for getting information concerning the jobs
  EASYSchedulingPolicy* extractEASYFields(bool here,DOMSupport &domSupport, XalanNode *contextNode, const PrefixResolver &prefixResolver, XPathEvaluator &theEvaluator,ArchitectureConfiguration* architectureConfig);
  ISISDispatcherPolicy* extractISISDispatcherFields(bool here,DOMSupport &theDOMSupport, XalanNode *theContextNode, const PrefixResolver &thePrefixResolver, XPathEvaluator &theEvaluator, ArchitectureConfiguration* architectureConfig);
  BRANKPolicy* extractBRANKPolicyFields(bool here,DOMSupport &theDOMSupport, XalanNode *theContextNode, const PrefixResolver &thePrefixResolver, XPathEvaluator &theEvaluator,ArchitectureConfiguration* architectureConfig);

  MetaBrokeringSystem* extractMetaBrokeringSystemFields(bool here,DOMSupport &theDOMSupport, XalanNode *theContextNode, const PrefixResolver &thePrefixResolver, XPathEvaluator &theEvaluator,ArchitectureConfiguration* architectureConfig);

  DeepSearchCollisionPolicy* extractDEEPFields(bool here,DOMSupport &theDOMSupport, XalanNode *theContextNode, const PrefixResolver &thePrefixResolver, XPathEvaluator &theEvaluator, ArchitectureConfiguration* architectureConfig);
  int containsXPath(string XPath,DOMSupport &theDOMSupport, XalanNode *theContextNode, const PrefixResolver &thePrefixResolver, XPathEvaluator &theEvaluator);
  
  //stuff for parsing types
  architecture_type_t  getArchitectureType(string ArchitectureType);
  policy_type_t  getPolicyType(string spolicy);
  RS_policy_type_t  getResourceSelectionPolicyType(string spolicy);
  Backfilling_variant_t getBackfillingVariantType(string sbackfillingVariant);
  statistic_t getStatisticType(string estimator);
  metric_t getMetricType(string variable);
  vector<native_t>* GetAttributeTypes(vector<string>* AttributeTypes);

  ConfigurationFile* simulatorConfiguration; /**< A reference to the simulator configuration */
  
public:
  /*we make them public since it won't be common to acces or modify it .. only in the inital phase*/
  string ConfigFile; /**< The file path for the simulation configuration file */
  string StatisticsConfigFile;/**< */
  
  /* general parameters common to all scheduling policies ..*/
  policy_type_t policy; /**< The policy that will be simulated */
  workload_type_t workload;/**< The worload type that will used as an input */
  string workloadPath;/**< The path where the workload is stored*/
  string paraverOutPutDir; /**< The directory path where the parver output files will be stored*/ 
  string paraverTrace; /**< The paraver trace file name */
  int startJobTrace; /**<indicates the initial number of job where the paravertrace trace has to be generated */
  int endJobTrace; /**<indicates the last number of job where the paravertrace trace has to be generated */
  string architectureFile;/**< The file containing the architecture definition */
  architecture_type_t architecture;/**< The type of architecture that will be used in the simulation */
  string outPutDir;/**< The directory path where the output files will be stored*/ 
  string ErrorFilePath;/**< The pathfile for the error file */
  string DebugFilePath;/**< The pathfile for the debug file */
  int DebugLevel;/**< The debug level used in the simulation */
  string GlobalStatisticsOutputFile;  /**< The path for the output simulation statistics file */
  string JobsSimPerformanceCSVFile;/**< The path for the output cvs file that will contain the information for the simulated jobs  */
  string PolicySimPerformanceCSVFile; /**< The path for the output cvs file that will contain the ifnroamtion for the policies*/
  double collectStatisticsInterval;/**< The interval between to statistical collection events  */
  string jobRequirementsFile;/**< The filepath for the job requirements files */
  double EmulateCPUFactor; /**<see scheduling policy for its definition.*/
  double ArrivalFactor; /**<indicates if the submitt time has to be multiplied by a factor*/
  double JobsToLoad; /**<by default, -1, indicates that all the jobs must be loaded in the simulation, otherwise indicates the number of jobs to be loaded*/
  double LoadFromJob; /**<by default 1 indicates from which job it has to  be loaded                     */
  double LoadTillJob; /**<by default -1 indicates that there is no limit */
  double LoadFactor; /**<indicates the load (obtained due to the jobs arrival) have to be obtained, usefull for test how a given configuration response on a given load. By default is the once configured in the original trace.*/
  double maxAllowedRuntime; /**< Indicates the maximum amount of runtime allowed runtime for the submitted jobs, it mainly models the job queue of the local resource, -1 means no limit*/ 
  double CostAllocationUnit; /**< Indicates the cost per allocation unit */
  double AllocationUnitHour; /**< Indicates the number of allocations unit per hour that the execution of one single processor requires */
  
  bool showSimulationProgress; /**<if true the the simulation progress will be shown in the STD OUT by defaul false*/
  
  RS_policy_type_t ResourceSelectionPolicy; /**<the resource selection policy that the policy will use for select where to allocate the jobs */
  
  //indicate for some fields if loaded or not
  bool architectureLoaded;/**< Indicates if the architecture file has been loaded correctly*/
  bool policyLoaded;/**< Indicates if the policy  has been loaded correctly*/
  bool workloadLoaded;/**< Indicates if the workload has been loaded correctly*/
  bool DSCPmetricTypeLoaded;/**< Indicates if the deepsearchcollisionpolicy metric has been loaded correctly*/
  bool DSCPstatisticTypeLoaded;/**< Indicates if the deepsearchcollisionpolicy statistics has has been loaded correctly*/
  bool fileLoaded;/**< Indicates if the file has been loaded correctly*/
  bool generateParaver; /**<indicates if the paraver trace must be generated*/
  
  /*specific parameters for each policy .. if this field list grows a lot we may create subclasses */
  metric_t DSCPmetricType;/**< The metric to optmize in case that the deepsearchcollisionpolicy is used (@see deepsearchcollisionpolicy)*/
  statistic_t DSCPstatisticType;/**< The statistics to use in the metric computation in case that the deepsearchcollisionpolicy is used (@see deepsearchcollisionpolicy)*/
  bool DSCPgenerateAllParaversRTables; //indicates if all the intermediate paraver trace files must be generated .. this is basically for debug purposes .. be aware becasue for each job allocation a paraver trace file will be generated !
  string DSCPparaverOutPutDir;/**< The output dir where all the intermediate dumps of the reservation tables will be dump in case that the deepsearchcollisionpolicy is used, and the dummping is required*/
  string DSCPparaverTrace;/**< The output dir where all the intermediate paraver traces of the reservation tables will be dump in case that the deepsearchcollisionpolicy is used, and the dummping is required */
    
  //regarding the EASY policy
  int numberOfReservations;/**< The number of reservations for the policy, in case EASY is specified*/
  Backfilling_variant_t backfillingVariant;/**< The backfilling variant, in case EASY is specified */
  string PredictorConfigFile; /**in case of the policy that uses prediction */
  bool AllocateAllJobsInTheRT; /**indicates if all the jobs must be allocated in the RT , see the easy cpp files */
  bool SimulateResourceUsage;/**< Indicates if the job collisions and resource usage have to be simulation  */
  double LessConsumeThreshold; /**< Indicates the threashold that has to be used in the find less consume threshold function, if it is set to -1 means that is the LessConsume */
  double LessConsumeAlternativeThreshold;/**< Indicates the alternative threashold that has to be used in the find less consume threshold function  when the default one can not be satisfied .. in this situations it use to be more relaxed*/
  bool tryToBackfillAllways; /**< Indicates if all the jobs have to be allocated if they can be backfilled although they won't start run now. It can occurs that a job would start earlier than the once that are in the reservation table, but it will not start inmediately. In some policies is needed to know the approximate backfilling time .. However it is exprensive ! */
  
  //regarding the ISIS-Dispatcher policy 
  vector<SchedulingPolicy*>* ISISDispatcherPolicies;/**< The set of policies to be used in each center, in case that GridBackfillingPolicy is used  */
  vector<ArchitectureConfiguration*>* ISISDispatcherArchitectures;/**< The set of architectures to be used in each center, in case that GridBackfillingPolicy is used  */
  vector<string>* ISISDispatcherParaverTracesName;/**< The set of paraver output traces to be used in each center, in case that ISISDispatcherPolicy is used  */
  metric_t ISISDispatcherMetric2Optimize;/**< The metric to optimize in the ISISDispatcherPolicy dispaching algorithm */
  statistic_t ISISDispatcherStatistic2Optimize;/**<  The statistic to use for compute the metric in the ISISDispatcherPolicy dispaching algorithm */
  vector<string>* ISISDispatcherCentersName;/**< The set of names of each center, in case that ISISDispatcherPolicy is used  */
  vector<double>* ISISDispatcherEmulateCPUFactor;/**< The set of architectures to be used in each center, in case that ISISDispatcherPolicy is used  */
  int amountOfKnownCenters; /**< Indicates the amount of centers that are known by the isis dispatcher, this is the first approach to model the amount of information that the prediction service can provide. */
  
  //regarding the BRANKPolicy policy 
  vector<SchedulingPolicy*>* BRANKPolicyPolicies;/**< The set of policies to be used in each center, in case that GridBackfillingPolicy is used  */
  vector<ArchitectureConfiguration*>* BRANKPolicyArchitectures;/**< The set of architectures to be used in each center, in case that GridBackfillingPolicy is used  */
  vector<string>* BRANKPolicyParaverTracesName;/**< The set of paraver output traces to be used in each center, in case that BRANKPolicyPolicy is used  */
  vector<string>* BRANKPolicyCentersName;/**< The set of names of each center, in case that BRANKPolicyPolicy is used  */
  vector<double>* BRANKPolicyEmulateCPUFactor;/**< The set of architectures to be used in each center, in case that BRANKPolicyPolicy is used  */
  
  map<string,native_t> AttributeJobType; /**< Cotains the mapping of each of the job attribute with its type, to be used for the datamining models creation */
  
  string GlobalPredictorConfigFile; /**< Contains the global predictor configuration file that can be used by all the simulation entities. In the case of local policies, if a specific configuration is specified it will be used rather than the global */
  Predictor * GlobalPredictionService; /**< Contains the global predictior that models a global prediction service */
  PredDeadlineMissManager* GlobalPreDeadlineManager; /**< The pre deadline miss manager for the prediction service  */
  PredDeadlineMissManager * GlobalPostDeadlineManager; /**< The post deadline miss manager for the prediction service  */
  
  vector<r_scripts_type> RAnalyzers; /**< The set of analyzers that have be used once the simulation will finish */

  string jobMonetaryRequirementsFile; /**< The file that contains the monetary requirements for the jobs */

  //regarding a meta-brokering system
  vector<SchedulingPolicy*>* MBVirtualOrganitzations; /**< The definition of the different virtual organitzations */
  vector<string> VONames; /**< The name of the different virtual organitzations */

  

  //initialize all the variables  
  void init();
  
  Log* log;
    bool computeEnergy;
    bool powerAwareBackfilling;

};

}

#endif
