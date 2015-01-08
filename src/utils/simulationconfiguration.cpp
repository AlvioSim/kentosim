#include <utils/simulationconfiguration.h>
#include <statistics/statisticsconfigurationfile.h>

#include <string>

using namespace xercesc;
using namespace xalanc;

namespace Utils {

/**
 * The default constructor for the class
 */
SimulationConfiguration::SimulationConfiguration()
{
  this->init();
}

/**
 * Initialize all the variables for the class 
 */
void SimulationConfiguration::init()
{   
  
  ConfigFile = "";
  StatisticsConfigFile = "";
  ErrorFilePath = "";
  DebugFilePath = "";
  DebugLevel = -1;
  
  DSCPparaverOutPutDir = "";
  DSCPparaverTrace = "";
  PredictorConfigFile = "";
  
  policy = FCFS; 
  workload = SWF;
  workloadPath = "";
  jobRequirementsFile = "";
  jobMonetaryRequirementsFile = "";
  paraverOutPutDir = "";         
  paraverTrace = "";
  startJobTrace = -1;
  endJobTrace = -1;  
  architectureFile = "";
  architecture = MARENOSTRUM;
  outPutDir = "";
  fileLoaded = false;
  GlobalPredictorConfigFile = "";
  GlobalPredictionService = NULL;
  
  architectureLoaded = false;
  policyLoaded = false;
  workloadLoaded = false;
  DSCPmetricTypeLoaded = false;
  DSCPstatisticTypeLoaded = false;
  fileLoaded = false;
  computeEnergy = false;
  
  collectStatisticsInterval = 10*60;

  numberOfReservations = 1;
  backfillingVariant = EASY_BACKFILLING;
  SimulateResourceUsage = false; 
  AllocateAllJobsInTheRT = false;
  
  PredictorConfigFile = "";
  JobsSimPerformanceCSVFile = "";
  GlobalStatisticsOutputFile = "";
  
  ArrivalFactor = 1;
  EmulateCPUFactor = 1;
  JobsToLoad = -1;
  LoadFromJob = 1;
  LoadTillJob = -1;
  LoadFactor = -1;
  
  showSimulationProgress = true;
  
  ResourceSelectionPolicy = FIRST_FIT;
  
  ISISDispatcherPolicies = NULL;
  ISISDispatcherArchitectures = NULL;
  ISISDispatcherParaverTracesName = NULL;
  ISISDispatcherCentersName = NULL;
  ISISDispatcherEmulateCPUFactor = NULL;
  
  log = NULL;
  
  maxAllowedRuntime = -1;
  CostAllocationUnit = -1;
  AllocationUnitHour = -1;
  
  LessConsumeAlternativeThreshold = -1;
  LessConsumeThreshold = -1;
  

  //we initialize all the job information fileds that can be used by the simulator 
  AttributeJobType["JobNumber"] = DOUBLE;
  AttributeJobType["JobSimSubmitTime"] = DOUBLE;
  AttributeJobType["JobSimWaitTime"] = DOUBLE;
  AttributeJobType["JobSimCenter"] = STRING;
  AttributeJobType["NumberProcessors"] = INTEGER;
  AttributeJobType["RequestedTime"] = DOUBLE;
  AttributeJobType["OriginalRequestedTime"] = DOUBLE;
  AttributeJobType["RequestedMemory"] = DOUBLE;
  AttributeJobType["UserID"] = INTEGER;
  AttributeJobType["GroupID"] = INTEGER;
  AttributeJobType["Executable"] = INTEGER;
  AttributeJobType["QueueNumber"] = INTEGER;
  AttributeJobType["BWMemoryUsed"] = INTEGER ;
  AttributeJobType["MemoryUsed"] = DOUBLE;
  AttributeJobType["BWEthernedUsed"] = DOUBLE;
  AttributeJobType["BWNetworkUsed"] = DOUBLE;
  AttributeJobType["JobSimStartTime"] = DOUBLE;
  AttributeJobType["JobSimStatus"] = DOUBLE;
  AttributeJobType["RunTime"] = DOUBLE  ;
  AttributeJobType["LessConsumeAlternativeThreshold"] = DOUBLE  ;

}


/**
 * The constructor that should be more used 
 * @param ConfigFile The path for the configuration file 
 * @param log The logging engine 
 */
SimulationConfiguration::SimulationConfiguration(string ConfigFile,Log* log,ConfigurationFile* simulatorConfiguration)
{
  this->ConfigFile = ConfigFile;
  this->init();
  this->simulatorConfiguration = simulatorConfiguration;
  
  this->log = log;
  
  try
  {
   XALAN_USING_XERCES(XMLPlatformUtils)
  
   XALAN_USING_XALAN(XPathEvaluator)
  
  
   XMLPlatformUtils::Initialize();
  
   XPathEvaluator::initialize();
  
   
    XALAN_USING_XERCES(LocalFileInputSource)
  
    XALAN_USING_XALAN(XalanDocument)
    XALAN_USING_XALAN(XalanDocumentPrefixResolver)
    XALAN_USING_XALAN(XalanDOMString)
    XALAN_USING_XALAN(XalanNode)
    XALAN_USING_XALAN(XalanSourceTreeInit)
    XALAN_USING_XALAN(XalanSourceTreeDOMSupport)
    XALAN_USING_XALAN(XalanSourceTreeParserLiaison)
    XALAN_USING_XALAN(XObjectPtr)
    XALAN_USING_XALAN(NodeRefList)
  
    // Initialize the XalanSourceTree subsystem...
    XalanSourceTreeInit  theSourceTreeInit;
  
    // We'll use these to parse the XML file.
    XalanSourceTreeDOMSupport  theDOMSupport;
    XalanSourceTreeParserLiaison theLiaison(theDOMSupport);
  
    // Hook the two together...
    theDOMSupport.setParserLiaison(&theLiaison);
  
    this->ConfigFile = ConfigFile;
    
    const XalanDOMString theFileName(this->ConfigFile.c_str());
    
    std::cout << "Loading the simulation configuration file "<< ConfigFile << endl;
  
    // Create an input source that represents a local file...
    const LocalFileInputSource theInputSource(theFileName.c_str()); 
    
    // Parse the document...
    XalanDocument* const theDocument =
      theLiaison.parseXMLStream(theInputSource);
    assert(theDocument != 0);
     
     XalanDocumentPrefixResolver  thePrefixResolver(theDocument); 
   /*
     Now that the document is loaded we go through all the elements of the XML document
      starting from the base one the policyConfiguration/Common
   */
  
    XPathEvaluator theEvaluator;
  
    // OK, let's find the context node... the BladeCenters
    XalanNode*  theContextNode =
      theEvaluator.selectSingleNode(
        theDOMSupport,
       theDocument,
       XalanDOMString("SimulationConfiguration/Policy/Common").c_str(),
       thePrefixResolver);         
       
    if (theContextNode == 0)
    {
        printf("There are no Common characteristics for the simulation\n");        
    }
    else
    {
       //************************We get the WorkloadPath *****************************/
     
      if(containsXPath(string("WorkloadPath"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator)) 
      {
         const XObjectPtr theWorkloadPathResult(
           theEvaluator.evaluate(
           theDOMSupport,
           theContextNode,
           XalanDOMString("WorkloadPath").c_str(),
           thePrefixResolver));
         
        if(theWorkloadPathResult.null())
        {
          printf("The filed WorkloadPath is not present in the configuration file ! \n");
          workloadPath = "";
        }
        else    
        { 
          stringstream stWorkloadPath;
          stWorkloadPath << theWorkloadPathResult->str();
          workloadPath = stWorkloadPath.str();  
        }
      }
        //************************We get the workload *****************************/
       
      if(containsXPath(string("workload"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
      {
       string workloadS;
     
       const XObjectPtr theworkloadResult(
         theEvaluator.evaluate(
           theDOMSupport,
           theContextNode,
           XalanDOMString("workload").c_str(),
           thePrefixResolver));
         
       if(theworkloadResult.null())
       {
         printf("The filed workload is not present in the configuration file ! \n");
         workloadS = "";
       }
       else    
       { 
        stringstream stworkload;
        stworkload << theworkloadResult->str();
        workloadS = stworkload.str();  
       
        this->workloadLoaded = true;
       }
     
       if(workloadS == "SWF")
        this->workload = SWF;
       else if(workloadS == "SWFExtReqs" || workloadS == "SWFExt" /* for backward compatibility */)
        this->workload = SWFExtReqs;
       else if(workloadS == "SWFExtCost")
        this->workload = SWFExtCosts;
       else if(workloadS == "SWFGrid")
        this->workload = SWFGrid;

       
     }  
   //************************We get the ArchitectureConfigurationFile *****************************/      
    if(containsXPath(string("ArchitectureConfigurationFile"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
    {
        const XObjectPtr theArchitectureConfigurationFileResult(
         theEvaluator.evaluate(
           theDOMSupport,
           theContextNode,
           XalanDOMString("ArchitectureConfigurationFile").c_str(),
           thePrefixResolver));
         
       if(theArchitectureConfigurationFileResult.null())
       {
       printf("The filed ArchitectureConfigurationFile is not present in the configuration file !\n ");
       architectureFile = "";
       }
       else  
       {   
         stringstream stArchitectureConfigurationFile;
         stArchitectureConfigurationFile << theArchitectureConfigurationFileResult->str();
         architectureFile = stArchitectureConfigurationFile.str();  
       }
    }
   //************************We get the StatisticsConfigFile *****************************/      
   
    if(containsXPath(string("StatisticsConfigFile"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
    {
        const XObjectPtr theStatisticsConfigFileResult(
         theEvaluator.evaluate(
           theDOMSupport,
           theContextNode,
           XalanDOMString("StatisticsConfigFile").c_str(),
           thePrefixResolver));
         
       if(theStatisticsConfigFileResult.null())
       {
         printf("The filed StatisticsConfigFile is not present in the configuration file ! \n ");
         architectureFile = "";
       }
       else  
       {   
         stringstream stStatisticsConfigFile;
         stStatisticsConfigFile << theStatisticsConfigFileResult->str();
         StatisticsConfigFile = stStatisticsConfigFile.str();  
       }
    }

   //************************We get the ArchitectureType *****************************/      
    if(containsXPath(string("ArchitectureType"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
    {   
       string ArchitectureType;
     
        const XObjectPtr theArchitectureTypeResult(
         theEvaluator.evaluate(
           theDOMSupport,
           theContextNode,
           XalanDOMString("ArchitectureType").c_str(),
           thePrefixResolver));
         
       if(theArchitectureTypeResult.null())
       {
         printf("The filed ArchitectureType is not present in the configuration file !\n ");
         ArchitectureType = "";
       }
       else  
       {   
         stringstream stArchitectureType;
         stArchitectureType << theArchitectureTypeResult->str();
         ArchitectureType = stArchitectureType.str();  
       
         this->architectureLoaded = true;
       }
     
       architecture = getArchitectureType(ArchitectureType);
 
     }
     
     
     //************************We get the Policy *****************************/      
    if(containsXPath(string("Policy"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
    {

      string spolicy; 
     
       const XObjectPtr thePolicyResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("Policy").c_str(),
         thePrefixResolver));
          
       if(thePolicyResult.null())
       {
         printf("The filed Policy is not present in the configuration file ! \n");
         spolicy = "";
       }
       else  
       {   
         stringstream stPolicy;
         stPolicy << thePolicyResult->str();
         spolicy = stPolicy.str();  
       
         this->policyLoaded = true;
       }
     
       this->policy = getPolicyType(spolicy);
      }
  
    //************************We get the ParaverOutputDir *****************************/      
    if(containsXPath(string("ParaverOutputDir"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
    {   
      const XObjectPtr theParaverOutputDirResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("ParaverOutputDir").c_str(),
         thePrefixResolver));
         
     if(theParaverOutputDirResult.null())
     {
       printf("The filed ParaverOutputDir is not present in the configuration file ! ");
       paraverOutPutDir = "";
     }
     else  
     {   
       stringstream stParaverOutputDir;
       stParaverOutputDir << theParaverOutputDirResult->str();
       paraverOutPutDir = stParaverOutputDir.str();  
     }
     
    }
     //************************We get the GlobalStatisticsOutputFile *****************************/      
    if(containsXPath(string("GlobalStatisticsOutputFile"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
    {
      const XObjectPtr theGlobalStatisticsOutputFileResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("GlobalStatisticsOutputFile").c_str(),
         thePrefixResolver));
         
     if(theGlobalStatisticsOutputFileResult.null())
     {
       printf("The filed GlobalStatisticsOutputFile is not present in the configuration file !\n ");
       GlobalStatisticsOutputFile= "";
     }
     else  
     {   
       stringstream stGlobalStatisticsOutputFile;
       stGlobalStatisticsOutputFile << theGlobalStatisticsOutputFileResult->str();
       GlobalStatisticsOutputFile = stGlobalStatisticsOutputFile.str();  
     }
   } 
     
     //************************We get the ParaverTraceName *****************************/      
    if(containsXPath(string("ParaverTraceName"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
    {
     const XObjectPtr theParaverTraceNameResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("ParaverTraceName").c_str(),
         thePrefixResolver));
         
     if(theParaverTraceNameResult.null())
     {
       printf("The filed ParaverTraceName is not present in the configuration file ! \n ");
       paraverTrace = "";
     }
     else   
     {   
       stringstream stParaverTraceName;
       stParaverTraceName << theParaverTraceNameResult->str();
       paraverTrace = stParaverTraceName.str();       
     }
   }
     //************************We get the startJobTrace *****************************/      
    if(containsXPath(string("startJobTrace"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
    {
     const XObjectPtr thestartJobTraceResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("startJobTrace").c_str(),
         thePrefixResolver));
         
     if(thestartJobTraceResult.null())
     {
       printf("The filed startJobTrace is not present in the configuration file ! \n ");
	startJobTrace = -1;
     }
     else   
     {   
       stringstream ststartJobTrace;
       ststartJobTrace << thestartJobTraceResult->str();
       startJobTrace = atoi(ststartJobTrace.str().c_str()); 
     }
   }
    
     //************************We get the endJobTrace *****************************/      
    if(containsXPath(string("endJobTrace"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
    {
     const XObjectPtr theendJobTraceResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("endJobTrace").c_str(),
         thePrefixResolver));
         
     if(theendJobTraceResult.null())
     {
       printf("The filed endJobTrace is not present in the configuration file ! \n ");
       endJobTrace = -1;
     }
     else   
     {   
       stringstream stendJobTrace;
       stendJobTrace << theendJobTraceResult->str();
       endJobTrace = atoi(stendJobTrace.str().c_str());       
     }
   }    
     //************************We get the DebugLevel *****************************/      
    if(containsXPath(string("DebugLevel"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
    {   
       const XObjectPtr theDebugLevelResult(
         theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("DebugLevel").c_str(),
         thePrefixResolver));
     
       string DebugLevel;
         
       if(theDebugLevelResult.null())
       {
         printf("The filed DebugLevel is not present in the configuration file ! \n");
         DebugLevel = "0";
       }
       else
       {   
         stringstream stDebugLevel;
         stDebugLevel << theDebugLevelResult->str();
         DebugLevel = stDebugLevel.str();  
       }
     
       this->DebugLevel = atoi(DebugLevel.c_str()); 
     }
     //************************We get the DebugFilePath *****************************/      
   
    if(containsXPath(string("DebugFilePath"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
    {   
      const XObjectPtr theDebugFilePathResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("DebugFilePath").c_str(),
         thePrefixResolver));
            
         
       if(theDebugFilePathResult.null())
       {
         printf("The filed DebugFilePath is not present in the configuration file ! \n");
         DebugFilePath = "0";
       }
       else
       {   
         stringstream stDebugFilePath;
         stDebugFilePath << theDebugFilePathResult->str();
         this->DebugFilePath = stDebugFilePath.str();  
       }         
     }
     //************************We get the ErrorFilePath *****************************/      
    if(containsXPath(string("ErrorFilePath"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
    {      
      const XObjectPtr theErrorFilePathResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("ErrorFilePath").c_str(),
         thePrefixResolver));
     
     string ErrorFilePath;
         
     if(theErrorFilePathResult.null())
     {
       printf("The filed ErrorFilePath is not present in the configuration file !\n ");
       ErrorFilePath = "0";
     }
     else
     {   
       stringstream stErrorFilePath;
       stErrorFilePath << theErrorFilePathResult->str();
       this->ErrorFilePath = stErrorFilePath.str();  


       this->log = new Log(this->ErrorFilePath,this->DebugFilePath,this->DebugLevel);
     }
    }
     
      //************************We get the OutPutDir *****************************/      
    if(containsXPath(string("OutPutDir"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
    {    
      const XObjectPtr theOutPutDirResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("OutPutDir").c_str(),
         thePrefixResolver));
         
     if(theOutPutDirResult.null())
     {
       printf("The filed OutPutDir is not present in the configuration file !\n ");
       outPutDir = "";
     }
     else
     {   
       stringstream stOutPutDir;
       stOutPutDir << theOutPutDirResult->str();
       outPutDir = stOutPutDir.str();  
     }
    }
      //************************We get the ArrivalFactor *****************************/      
    if(containsXPath(string("ArrivalFactor"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
    {   
      const XObjectPtr theArrivalFactorResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("ArrivalFactor").c_str(),
         thePrefixResolver));
         
     if(theArrivalFactorResult.null())
     {
       printf("The filed ArrivalFactor is not present in the configuration file !\n ");
       ArrivalFactor = 1;
     }
     else
     {   
       stringstream stArrivalFactor;
       stArrivalFactor << theArrivalFactorResult->str();
       string ArrivalFactorS = stArrivalFactor.str();  
       
       if(ArrivalFactorS.compare("") != 0)
        this->ArrivalFactor = atof(ArrivalFactorS.c_str());
       else 
        this->ArrivalFactor = 1;
     }
    }
    
     //************************We get the generateParaver *****************************/      
    if(containsXPath(string("generateParaver"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
    {  
      const XObjectPtr thegenerateParaverSResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("generateParaver").c_str(),
         thePrefixResolver));
         
     string generateParaverS;
         
     if(thegenerateParaverSResult.null())
     {
       printf("The filed generateParaverS is not present in the configuration file !\n ");
       generateParaverS = "";
     }
     else
     {
       stringstream stgenerateParaver;
       stgenerateParaver << thegenerateParaverSResult->str();
       generateParaverS = stgenerateParaver.str();  
       
       if(!generateParaverS.compare("") || !generateParaverS.compare("NO") )
         this->generateParaver = false;
       else
         this->generateParaver = true;
     }
   }

//************************We get the computeEnergy *****************************/      
    if(containsXPath(string("computeEnergy"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
    {  
      const XObjectPtr thecomputeEnergySResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("computeEnergy").c_str(),
         thePrefixResolver));
         
     string computeEnergyS;
         
     if(thecomputeEnergySResult.null())
     {
       printf("The filed computeEnergyS is not present in the configuration file !\n ");
       computeEnergyS = "";
     }
     else
     {
       stringstream stcomputeEnergy;
       stcomputeEnergy << thecomputeEnergySResult->str();
       computeEnergyS = stcomputeEnergy.str();  
       
       if(!computeEnergyS.compare("") || !computeEnergyS.compare("NO") )
         this->computeEnergy = false;
       else
       {
         std::cout << "Energy will be computed " << endl;
         this->computeEnergy = true;
	}
     }
   }  


 
     //************************We get the collectStatisticsInterval *****************************/      
    if(containsXPath(string("collectStatisticsInterval"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
    {  
     const XObjectPtr thecollectStatisticsIntervalResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("collectStatisticsInterval").c_str(),
         thePrefixResolver));
         
     if(thecollectStatisticsIntervalResult.null())
     {
       printf("The filed collectStatisticsInterval is not present in the configuration file ! \n");
     }
     else
     {   
       stringstream stcollectStatisticsInterval;
       stcollectStatisticsInterval << thecollectStatisticsIntervalResult->str();
       string interval = stcollectStatisticsInterval.str();
       
       if(interval.compare("") != 0)
         collectStatisticsInterval = atof(interval.c_str());
       else 
         collectStatisticsInterval = 18000;
       
     }
    }   
       
      //************************We get the jobRequirementsFile *****************************/      
    if(containsXPath(string("jobRequirementsFile"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
    { 
       const XObjectPtr thejobRequirementsFileResult(
         theEvaluator.evaluate(
           theDOMSupport,
           theContextNode,
           XalanDOMString("jobRequirementsFile").c_str(),
           thePrefixResolver));
         
       if(thejobRequirementsFileResult.null())
       {
         printf("The filed jobRequirementsFile is not present in the configuration file ! \n");
         jobRequirementsFile = "";
       }
       else
       {   
         stringstream stjobRequirementsFile;
         stjobRequirementsFile << thejobRequirementsFileResult->str();
         jobRequirementsFile = stjobRequirementsFile.str();  
       }   
     
       this->jobRequirementsFile = jobRequirementsFile;
    }
       
      //************************We get the jobMonetaryRequirementsFile *****************************/      
    if(containsXPath(string("jobMonetaryRequirementsFile"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
    { 
       const XObjectPtr thejobMonetaryRequirementsFileResult(
         theEvaluator.evaluate(
           theDOMSupport,
           theContextNode,
           XalanDOMString("jobMonetaryRequirementsFile").c_str(),
           thePrefixResolver));
         
       if(thejobMonetaryRequirementsFileResult.null())
       {
         printf("The filed jobMonetaryRequirementsFile is not present in the configuration file ! \n");
         jobMonetaryRequirementsFile = "";
       }
       else
       {   
         stringstream stjobMonetaryRequirementsFile;
         stjobMonetaryRequirementsFile << thejobMonetaryRequirementsFileResult->str();
         jobMonetaryRequirementsFile = stjobMonetaryRequirementsFile.str();  
       }   
     
       this->jobMonetaryRequirementsFile = jobMonetaryRequirementsFile;
    }
      
      //************************We get the EmulateCPUFactor *****************************/      
    if(containsXPath(string("EmulateCPUFactor"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
    {    
     const XObjectPtr theEmulateCPUFactorResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("EmulateCPUFactor").c_str(),
         thePrefixResolver));
         
     if(theEmulateCPUFactorResult.null())
     {
       printf("The filed EmulateCPUFactor is not present in the configuration file !\n ");
       EmulateCPUFactor = 1;
     }
     else
     {   
       stringstream stEmulateCPUFactor;
       stEmulateCPUFactor << theEmulateCPUFactorResult->str();
       string EmulateCPUFactorS = stEmulateCPUFactor.str();  
       
       if(EmulateCPUFactorS.compare("") != 0)
         this->EmulateCPUFactor = atof(EmulateCPUFactorS.c_str());
       else
         this->EmulateCPUFactor = 1;
     }           
    }
    
      //************************We get the   *****************************/      
    if(containsXPath(string("maxAllowedRuntime"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
    {    
     const XObjectPtr themaxAllowedRuntimeResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("maxAllowedRuntime").c_str(),
         thePrefixResolver));
         
     if(themaxAllowedRuntimeResult.null())
     {
       printf("The filed maxAllowedRuntime is not present in the configuration file !\n ");
       maxAllowedRuntime = 1;
     }
     else
     {   
       stringstream stmaxAllowedRuntime;
       stmaxAllowedRuntime << themaxAllowedRuntimeResult->str();
       string maxAllowedRuntimeS = stmaxAllowedRuntime.str();  
       
       if(maxAllowedRuntimeS.compare("") != 0)
         this->maxAllowedRuntime = atof(maxAllowedRuntimeS.c_str());
       else
         this->maxAllowedRuntime = -1;
     }           
    }    
    
    
    //************************We get the LoadFactor *****************************/      
    if(containsXPath(string("LoadFactor"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
    {    
     const XObjectPtr theLoadFactorResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("LoadFactor").c_str(),
         thePrefixResolver));
         
     if(theLoadFactorResult.null())
     {
       printf("The filed LoadFactor is not present in the configuration file !\n ");
       LoadFactor = -1;
     }
     else
     {   
       stringstream stLoadFactor;
       stLoadFactor << theLoadFactorResult->str();
       string LoadFactorS = stLoadFactor.str();  
       
       if(LoadFactorS.compare("") != 0)
         this->LoadFactor = atof(LoadFactorS.c_str());
       else
         this->LoadFactor = -1;
     }           
    }
          
    //************************We get the JobsToLoad *****************************/      
    if(containsXPath(string("JobsToLoad"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
    {    
     const XObjectPtr theJobsToLoadResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("JobsToLoad").c_str(),
         thePrefixResolver));
         
     if(theJobsToLoadResult.null())
     {
       printf("The filed JobsToLoad is not present in the configuration file !\n ");
       JobsToLoad = -1;
     }
     else
     {   
       stringstream stJobsToLoad;
       stJobsToLoad << theJobsToLoadResult->str();
       string JobsToLoadS = stJobsToLoad.str();  
       
       if(JobsToLoadS.compare("") != 0)
         this->JobsToLoad = atof(JobsToLoadS.c_str());
       else
         this->JobsToLoad = -1;
     }           
    }          
    //************************We get the LoadFromJob  *****************************/      
    if(containsXPath(string("LoadFromJob"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
    {    
     const XObjectPtr theLoadFromJobResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("LoadFromJob").c_str(),
         thePrefixResolver));
         
     if(theLoadFromJobResult.null())
     {
       printf("The filed LoadFromJob is not present in the configuration file !\n ");
       LoadFromJob = 1;
     }
     else
     {   
       stringstream stLoadFromJob;
       stLoadFromJob << theLoadFromJobResult->str();
       string LoadFromJobS = stLoadFromJob.str();  
       
       if(LoadFromJobS.compare("") != 0)
         this->LoadFromJob = atof(LoadFromJobS.c_str());
       else
         this->LoadFromJob = -1;
     }
    }         

    //************************We get the CostAllocationUnit  *****************************/      
    if(containsXPath(string("CostAllocationUnit"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
    {    
     const XObjectPtr theCostAllocationUnitResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("CostAllocationUnit").c_str(),
         thePrefixResolver));
         
     if(theCostAllocationUnitResult.null())
     {
       printf("The filed CostAllocationUnit is not present in the configuration file !\n ");
       CostAllocationUnit = 1;
     }
     else
     {   
       stringstream stCostAllocationUnit;
       stCostAllocationUnit << theCostAllocationUnitResult->str();
       string CostAllocationUnitS = stCostAllocationUnit.str();  
       
       if(CostAllocationUnitS.compare("") != 0)
         this->CostAllocationUnit = atof(CostAllocationUnitS.c_str());
       else
         this->CostAllocationUnit = -1;
     }
    }   


    //************************We get the AllocationUnitHour  *****************************/      
    if(containsXPath(string("AllocationUnitHour"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
    {    
     const XObjectPtr theAllocationUnitHourResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("AllocationUnitHour").c_str(),
         thePrefixResolver));
         
     if(theAllocationUnitHourResult.null())
     {
       printf("The filed AllocationUnitHour is not present in the configuration file !\n ");
       AllocationUnitHour = 1;
     }
     else
     {   
       stringstream stAllocationUnitHour;
       stAllocationUnitHour << theAllocationUnitHourResult->str();
       string AllocationUnitHourS = stAllocationUnitHour.str();  
       
       if(AllocationUnitHourS.compare("") != 0)
         this->AllocationUnitHour = atof(AllocationUnitHourS.c_str());
       else
         this->AllocationUnitHour = -1;
     }
    }          

    //************************We get the maxAllowedRuntime  *****************************/      
    if(containsXPath(string("maxAllowedRuntime"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
    {    
     const XObjectPtr themaxAllowedRuntimeResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("maxAllowedRuntime").c_str(),
         thePrefixResolver));
         
     if(themaxAllowedRuntimeResult.null())
     {
       printf("The filed maxAllowedRuntime is not present in the configuration file !\n ");
       maxAllowedRuntime = 1;
     }
     else
     {   
       stringstream stmaxAllowedRuntime;
       stmaxAllowedRuntime << themaxAllowedRuntimeResult->str();
       string maxAllowedRuntimeS = stmaxAllowedRuntime.str();  
       
       if(maxAllowedRuntimeS.compare("") != 0)
         this->maxAllowedRuntime = atof(maxAllowedRuntimeS.c_str());
       else
         this->maxAllowedRuntime = -1;
     }
    }   
      
    //************************We get the LoadTillJob  *****************************/      
    if(containsXPath(string("LoadTillJob"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
    {    
     const XObjectPtr theLoadTillJobResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("LoadTillJob").c_str(),
         thePrefixResolver));
         
     if(theLoadTillJobResult.null())
     {
       printf("The filed LoadTillJob is not present in the configuration file !\n ");
       LoadTillJob = 1;
     }
     else
     {   
       stringstream stLoadTillJob;
       stLoadTillJob << theLoadTillJobResult->str();
       string LoadTillJobS = stLoadTillJob.str();  
       
       if(LoadTillJobS.compare("") != 0)
         this->LoadTillJob = atof(LoadTillJobS.c_str());
       else
         this->LoadTillJob = -1;
     }           
    }                      
              
    //************************We get the showSimulationProgress  *****************************/      
    if(containsXPath(string("showSimulationProgress"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
    {    
     const XObjectPtr theshowSimulationProgressResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("showSimulationProgress").c_str(),
         thePrefixResolver));
         
     if(theshowSimulationProgressResult.null())
     {
       printf("The filed showSimulationProgress is not present in the configuration file !\n ");
       showSimulationProgress = false;
     }
     else
     {   
       stringstream stshowSimulationProgress;
       stshowSimulationProgress << theshowSimulationProgressResult->str();
       string showSimulationProgressS = stshowSimulationProgress.str();  
       
       if(showSimulationProgressS.compare("") != 0)
         this->showSimulationProgress = true;
       else
         this->showSimulationProgress = false;
     }           
    }                       
              
     //************************We get the ResourceSelectionPolicy *****************************/      
    if(containsXPath(string("ResourceSelectionPolicy"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
    {     
     const XObjectPtr theResourceSelectionPolicyResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("ResourceSelectionPolicy").c_str(),
         thePrefixResolver));
         
     if(theResourceSelectionPolicyResult.null())
     {
       printf("The filed ResourceSelectionPolicy is not present in the configuration file !\n ");
       ResourceSelectionPolicy = FF_AND_FCF;
     }
     else
     {   
       stringstream stResourceSelectionPolicy;
       stResourceSelectionPolicy << theResourceSelectionPolicyResult->str();
       this->ResourceSelectionPolicy = getResourceSelectionPolicyType(stResourceSelectionPolicy.str());
     }           
    }
    

      //************************We get the LessConsumeThreshold *****************************/      
    if(containsXPath(string("LessConsumeThreshold"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
    {   
      const XObjectPtr theThresholdResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("LessConsumeThreshold").c_str(),
         thePrefixResolver));        
         
     if(theThresholdResult.null())
     {
       printf("The filed Threshold is not present in the configuration file.\n");
       this->LessConsumeThreshold = 1;
     }
     else
     {   
       stringstream stThreshold;
       stThreshold << theThresholdResult->str();
       this->LessConsumeThreshold = atof((stThreshold.str()).c_str());
     }   
    } 

      //************************We get the LessConsumeAlternativeThreshold *****************************/      
    if(containsXPath(string("LessConsumeAlternativeThreshold"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
    {   
      const XObjectPtr theAlternativeThresholdResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("LessConsumeAlternativeThreshold").c_str(),
         thePrefixResolver));        
         
     if(theAlternativeThresholdResult.null())
     {
       printf("The filed AlternativeThreshold is not present in the configuration file.\n");
       this->LessConsumeAlternativeThreshold = 1;
     }
     else
     {   
       stringstream stAlternativeThreshold;
       stAlternativeThreshold << theAlternativeThresholdResult->str();
       this->LessConsumeAlternativeThreshold = atof((stAlternativeThreshold.str()).c_str());
     }   
    } 

     //************************We get the JobsSimPerformanceCSVFile *****************************/      
    if(containsXPath(string("JobsSimPerformanceCSVFile"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
    {      
     const XObjectPtr theJobsSimPerformanceCSVFileResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("JobsSimPerformanceCSVFile").c_str(),
         thePrefixResolver));
         
     if(theJobsSimPerformanceCSVFileResult.null())
     {
       printf("The filed JobsSimPerformanceCSVFile is not present in the configuration file !\n ");
       JobsSimPerformanceCSVFile = "";
     }
     else
     {   
       stringstream stJobsSimPerformanceCSVFile;
       stJobsSimPerformanceCSVFile << theJobsSimPerformanceCSVFileResult->str();
       this->JobsSimPerformanceCSVFile = stJobsSimPerformanceCSVFile.str();              
     }           
    }
    
     //************************We get the JobsSimPerformanceCSVFile *****************************/      
    if(containsXPath(string("PolicySimPerfomanceCSVFile"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
    {      
     const XObjectPtr thePolicySimPerformanceCSVFileResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("PolicySimPerfomanceCSVFile").c_str(),
         thePrefixResolver));
         
     if(thePolicySimPerformanceCSVFileResult.null())
     {
       printf("The filed PolicySimPerfomanceCSVFile is not present in the configuration file !\n ");
       JobsSimPerformanceCSVFile = "";
     }
     else
     {   
       stringstream stPolicySimPerformanceCSVFile;
       stPolicySimPerformanceCSVFile << thePolicySimPerformanceCSVFileResult->str();
       this->PolicySimPerformanceCSVFile = stPolicySimPerformanceCSVFile.str();                           
     }           
    }    
    
    
    
     //************************We get the GlobalPredictorConfigFile *****************************/      
    if(containsXPath(string("PredictionServiceConfigFile"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
    {      
     const XObjectPtr theGlobalPredictorConfigFileResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("PredictionServiceConfigFile").c_str(),
         thePrefixResolver));
         
     if(theGlobalPredictorConfigFileResult.null())
     {
       printf("The filed PredictionServiceConfigFile is not present in the configuration file !\n ");
       GlobalPredictorConfigFile = "";
     }
     else
     {   
       stringstream stGlobalPredictorConfigFile;
       stGlobalPredictorConfigFile << theGlobalPredictorConfigFileResult->str();
       this->GlobalPredictorConfigFile = stGlobalPredictorConfigFile.str();              
       
       PredictorConfigurationFile* configPred = new PredictorConfigurationFile(GlobalPredictorConfigFile);
       GlobalPredictionService = CreatePredictor(configPred);        
       GlobalPreDeadlineManager = CreateDeadlineManager(configPred->preDeadlineManager);
       GlobalPostDeadlineManager = CreateDeadlineManager(configPred->postDeadlineManager); 
     }
    }    
      
    
    /* We get the R scripts that will be used for postprocessing the CSV fields  */
    if(containsXPath(string("RAnalyzers"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator)) 
    {            
        NodeRefList estimatorsNodes; 
      
        estimatorsNodes = theEvaluator.selectNodeList (estimatorsNodes, theDOMSupport, theContextNode,  
                                             XalanDOMString("RAnalyzers/Analyzer").c_str(),
                                             thePrefixResolver);

         //for this variable we will gather all statistical estimators required
         for(int i = 0; i < estimatorsNodes.getLength();i++)
         {
           XalanNode* estimatorNode = estimatorsNodes.item(i);      
           
           string AnalyzerType = "";

           if(containsXPath(string("AnalyzerType"),theDOMSupport,estimatorNode,thePrefixResolver,theEvaluator)) 
           {    
             /* we get the CVS File Type to whom the file R file will be applied */
             
     
             const XObjectPtr theAnalyzerTypeTypeResult(
               theEvaluator.evaluate(
               theDOMSupport,
               estimatorNode,
               XalanDOMString("AnalyzerType").c_str(),
               thePrefixResolver));
         
             if(theAnalyzerTypeTypeResult.null())
             { 
               printf("The AnalyzerType is not present in the configuration file for one of the metrics ! ");
               AnalyzerType = "";
             }
             else    
             { 
               stringstream stAnalyzerType;
               stAnalyzerType << theAnalyzerTypeTypeResult->str();
               AnalyzerType = stAnalyzerType.str();         
             }
           }  
           else
           {
              log->error("Missing AnalyzerType item in one of the nodes of the configuration file");
           }
           
           r_scripts_type analyzer = simulatorConfiguration->getAnalyzerType(AnalyzerType);           
           map<r_scripts_type,string>::iterator has = simulatorConfiguration->RScripts.find(analyzer);
           
           if(has != simulatorConfiguration->RScripts.end())
             this->RAnalyzers.push_back(analyzer);
           else
             this->log->error("The analyser "+AnalyzerType+" is not descrived in the simulator configuration file");
        }
      }

    
    
    /* POLICY SPECIFIC FIELDS */
      
     
    // OK, let's find the context node... the DeepSearchCollisionPolicy
    theContextNode =
      theEvaluator.selectSingleNode(
        theDOMSupport,
       theDocument,
       XalanDOMString("SimulationConfiguration/Policy/DeepSearchCollisionPolicy").c_str(),
       thePrefixResolver);         
       
    if (theContextNode == 0)
    {
        printf("There are no DeepSearchCollisionPolicy characteristics for the simulation \n");        
    }
    else
    {
        extractDEEPFields(true,theDOMSupport,theContextNode,thePrefixResolver,theEvaluator,NULL);
    }
      
      

    // OK, let's find the context node... the DeepSearchCollisionPolicy
    theContextNode =
      theEvaluator.selectSingleNode(
        theDOMSupport,
       theDocument,
       XalanDOMString("SimulationConfiguration/Policy/EASY").c_str(),
       thePrefixResolver);         
       
    if (theContextNode == 0)
    {
        printf("There are no EASY characteristics for the simulation \n");        
    }
    else
    {
        extractEASYFields(true,theDOMSupport,theContextNode,thePrefixResolver,theEvaluator,NULL);
    }
     
    theContextNode =
      theEvaluator.selectSingleNode(
        theDOMSupport,
       theDocument,
       XalanDOMString("SimulationConfiguration/Policy/ISISDispatcher").c_str(),
       thePrefixResolver);         
       
    if (theContextNode == 0)
    {
        printf("The specified policy is not the ISIS-Dispatcher\n");        
    }
    else
    {
        extractISISDispatcherFields(true,theDOMSupport,theContextNode,thePrefixResolver,theEvaluator,NULL);
    }

    theContextNode =
      theEvaluator.selectSingleNode(
        theDOMSupport,
       theDocument,
       XalanDOMString("SimulationConfiguration/Policy/BRANKPolicy").c_str(),
       thePrefixResolver);         
       
    if (theContextNode == 0)
    {
        printf("The specified policy is not the BRANKPolicy\n");        
    }
    else
    {
        extractBRANKPolicyFields(true,theDOMSupport,theContextNode,thePrefixResolver,theEvaluator,NULL);
    }

    theContextNode =
      theEvaluator.selectSingleNode(
        theDOMSupport,
       theDocument,
       XalanDOMString("SimulationConfiguration/Policy/MetaBrokeringSystem").c_str(),
       thePrefixResolver);         
       
    if (theContextNode == 0)
    {
        printf("The specified policy is not the MetaBrokeringSystem\n");        
    }
    else
    {
        extractMetaBrokeringSystemFields(true,theDOMSupport,theContextNode,thePrefixResolver,theEvaluator,NULL);
    }

    /*********************************END OF POLICY STUFF *****************************************/
    
   fileLoaded = true;
    
   } 
  }
 catch(...)
 {
   
   std::cout << "Undefined error the document has some errors. Please check the architecture configuration file." << endl;
   std::cout << "Please check that the configuration file exists.." << endl;
  
 }
}

/**
 * The default destructor for the class 
 */
SimulationConfiguration::~SimulationConfiguration()
{
}


/**
 * Imports the information of a configuration file class if the same information in the simulation configuration file is not specified
 * @param config The configuration file 
 * @see The class ConfigurationFile
 */
void SimulationConfiguration::import(ConfigurationFile* config)
{
  //this function only import the values that are no present in the current simulation configuration
  //remember that the simulation file has more priority that the configration file with those fields
  //that are related the simulation 
  if(!policyLoaded) 
    policy = config->policy; 
  if(!workloadLoaded) 
    workload = config->workload;
  if(workloadPath.size() <=1) 
    workloadPath = config->workloadPath;
  if(!workloadLoaded)
    this->workload = config->workload;
  if(jobRequirementsFile.size()  <= 1)
    jobRequirementsFile = config->jobRequirementsFile;
  if(paraverOutPutDir.size() <= 1)
    paraverOutPutDir = config->paraverOutPutDir;
  if(paraverTrace.size() <= 1)
    paraverTrace = config->paraverTrace;
  if(architectureFile.size() <= 1)
    architectureFile = config->architectureFile;
  if(!architectureLoaded) 
    architecture = config->architecture;
  if(outPutDir.size()  <= 1)
    outPutDir = config->DefaultOutPutDir;
  if(DebugLevel == -1) 
    DebugLevel = config->DebugLevel;  
  if(DebugFilePath.size() <= 1)
     DebugFilePath = config->DebugFilePath; 
  if(ErrorFilePath.size() <= 1)
     ErrorFilePath = config->ErrorFilePath;
  if(StatisticsConfigFile.size() <= 1)
     StatisticsConfigFile = config->statisticsConfigFile;
  if(LessConsumeAlternativeThreshold < 0)
     LessConsumeAlternativeThreshold = config->LessConsumeAlternativeThreshold;

}

/**
 * Imports the information of a simulation configuration  class if the same information in the simulation configuration file is not specified
 * @param ConfigFile The configuration file 
 * @see The class ConfigurationFile
 */
void SimulationConfiguration::import(SimulationConfiguration* ConfigFile)
{

}

    /***********************************************************************************************
    ********************PARSING TYPE FUNCTIONS  ****************************************************
    ************************************************************************************************/

/**
 * Given an string returns the architecture type 
 * @param ArchitectureType The architecture type in string format 
 * @return The architecture_type_t containing the architecture type specified in the parameter
 */
architecture_type_t  SimulationConfiguration::getArchitectureType(string ArchitectureType)
{
  if(ArchitectureType =="MN1")
  {
    return MARENOSTRUM;
  }
  if(ArchitectureType =="LOCAL_SYSTEM")
  {
    return LOCAL_SYSTEM;
  }
  else
  {
    printf("The architecture model is incorrect --> %s\n",ArchitectureType.c_str());
  }

  return OTHER_ARCHITECTURE;
}


/**
 * Given an string returns the policy type 
 * @param spolicy The policy type in string format 
 * @return The policy_type_t containing the policy type specified in the parameter
 */
policy_type_t  SimulationConfiguration::getPolicyType(string spolicy)
{
  if(spolicy == "EASY") 
    return EASY;
  else if(spolicy == "DEEP") 
    return DEEP;
  else if(spolicy == "FCFS") 
    return FCFS;
  else if(spolicy == "GRID_BACKFILLING") 
    return GRID_BACKFILLING;
  else if(spolicy == "ISIS_DISPATCHER") 
    return ISIS_DISPATCHER;
  else if(spolicy == "BRANK_POLICY")
    return BRANK_POLICY;
  else if(spolicy == "META_BROKERING_SYSTEM")
    return META_BROKERING_SYSTEM;  
  else  
    printf("Error with the policy field in the common part .. %s\n",spolicy.c_str());

  return OTHER_POLICY;
}



/**
 * Given an string returns the statistic type 
 * @param estimator The policy type in string format 
 * @return The statistic_t containing the statistic type specified in the parameter
 */
statistic_t SimulationConfiguration::getStatisticType(string estimator)
{
   if(estimator == "MEAN")
   {
     return AVG;     
   }
   else if(estimator == "MEDIAN")
   {     
     return MEDIAN;     
   }   
   else if(estimator == "PERCENTILE95")
   {     
     return PERCENTILE95;     
   }   
   else if(estimator == "MIN")
   {     
     return MIN;     
   }   
   else if(estimator == "MAX")
   {     
     return MAX;     
   }   
   else if(estimator == "STDEV")
   {     
     return STDEV;     
   }  
   else if(estimator == "IQR")
   {     
     return IQR;     
   }  
   else if(estimator == "ACCUMULATE")
   {     
     return ACCUMULATE;     
   }        
   else
    return OTHER_STAT;  

}

/**
 * Given an string returns the Resource Selection Policy type 
 * @param rspolicy The Resource Selection Policy type in string format 
 * @return The RS_policy_type_t containing the policy type specified in the parameter
 */
RS_policy_type_t  SimulationConfiguration::getResourceSelectionPolicyType(string rspolicy)
{
  if(rspolicy == "FIRST_FIT")
    return FIRST_FIT;
  else if(rspolicy == "FIRST_CONTINOUS_FIT")
    return FIRST_CONTINOUS_FIT;
  else if(rspolicy == "FF_AND_FCF")
    return FF_AND_FCF;
  else if(rspolicy == "FIND_LESS_CONSUME")
    return FIND_LESS_CONSUME;
  else if(rspolicy == "FIND_LESS_CONSUME_THRESSHOLD")
    return FIND_LESS_CONSUME_THRESSHOLD;
  else if(rspolicy == "EQUI_DISTRIBUTE_CONSUME")
    return EQUI_DISTRIBUTE_CONSUME;        
  else if(rspolicy == "DEFAULT_RS_P")
    return DEFAULT_RS_P;  
  else if(rspolicy == "DC_FIRST_VM")
    return DC_FIRST_VM;
  else   
    FF_AND_FCF; //by default we use the First fit and first continous fit 
 
}

/**
 * Given an string returns the metric type 
 * @param variable The metric type in string format 
 * @return The metric_t containing the metric type specified in the parameter
 */
metric_t SimulationConfiguration::getMetricType(string variable)
{
   if(variable == "SLD")
   {
     return SLD;     
   }
   else if(variable == "BSLD")
   {     
     return BSLD;     
   }   
   else if(variable == "WAITTIME")
   {     
     return WAITTIME;     
   }  
   else if(variable == "BACKFILLED_JOB")
   {     
     return BACKFILLED_JOB;     
   }     
   else if(variable == "kILLED_JOB")
   {     
     return kILLED_JOB;     
   }   
   else if(variable == "BACKFILLED_JOBS")
   {     
     return BACKFILLED_JOBS;     
   }        
   else if(variable == "JOBS_IN_WQUEUE")
   {     
     return JOBS_IN_WQUEUE;     
   }        
   else if(variable == "CPUS_USED")
   {     
     return CPUS_USED;     
   }      
   else if(variable == "LEAST_WORK_LEFT")
   {     
     return LEAST_WORK_LEFT;     
   }   
   else if(variable == "SUBMITED_JOBS")
   {     
     return SUBMITED_JOBS;     
   }      
   else if(variable == "SUBMITED_JOBS_CENTER")
   {     
     return SUBMITED_JOBS_CENTER;     
   }   
   else if(variable == "RSP_USED")
   {     
     return RSP_USED;     
   }         
   else if(variable == "PENALIZED_RUNTIME")
   {     
     return PENALIZED_RUNTIME;     
   }       
   else if(variable == "PERCENTAGE_PENALIZED_RUNTIME")
   {     
     return PERCENTAGE_PENALIZED_RUNTIME;     
   }            
   else if(variable == "LOAD_JOBS_CENTERS")
   {     
     return LOAD_JOBS_CENTERS;     
   }       
   else if(variable == "COMPUTATIONAL_COST")
   {     
     return COMPUTATIONAL_COST;     
   }  
   else if(variable == "ESTIMATED_COMPUTATIONAL_COST")
   {     
     return ESTIMATED_COMPUTATIONAL_COST;     
   }  
   else if(variable == "PREDICTED_WT")
   {     
     return PREDICTED_WT;     
   }  
   else if(variable == "PREDICTED_RUNTIME")
   {     
     return PREDICTED_RUNTIME;     
   }     
   else if(variable == "SLD_PREDICTED")
   {     
     return SLD_PREDICTED;     
   } 
   else if(variable == "ESTIMATED_COMPUTATIONAL_COST_ERROR")
   {     
     return ESTIMATED_COMPUTATIONAL_COST_ERROR;     
   }  
   else if(variable == "COMPUTATIONAL_COST_RESPECTED")
   {     
     return COMPUTATIONAL_COST_RESPECTED;     
   }  
   else if(variable == "PREDICTED_WT_ERROR")
   {     
     return PREDICTED_WT_ERROR;     
   }  
   else if(variable == "PREDICTED_RUNTIME_ERROR")
   {     
     return PREDICTED_RUNTIME_ERROR;
   }
   else if(variable == "SLD_PREDICTED_ERROR")
   {     
     return SLD_PREDICTED_ERROR;
   }
   else if(variable == "ESTIMATED_START_TIME_FIRST_FIT")
   {
     return ESTIMATED_START_TIME_FIRST_FIT;
   }
   else
    assert(false);  
}


/**
 * Given an string returns the backfilling variant type 
 * @param sbackfillingVariant The metric type in string format 
 * @return The Backfilling_variant_t containing the backfilling variant type specified in the parameter
 */
Backfilling_variant_t SimulationConfiguration::getBackfillingVariantType(string sbackfillingVariant)
{

  if(sbackfillingVariant =="EASY_BACKFILLING")
  {
    return EASY_BACKFILLING;
  }
  else if(sbackfillingVariant == "SJF_BACKFILLING")
  {
    return SJF_BACKFILLING;
  }
  else if(sbackfillingVariant == "LXWF_BACKFILLING")
  {
    return LXWF_BACKFILLING;
  }
  else if(sbackfillingVariant == "RUA_BACKFILLING")
  {
    return RUA_BACKFILLING;
  }
  else if(sbackfillingVariant == "POWER_AWARE_BACKFILLING")
  {
    return POWER_AWARE_BACKFILLING;
  }

  return EASY_BACKFILLING; //by default
}


    /***********************************************************************************************
    ********************PARSING XML FUNCTIONS  ****************************************************
    ************************************************************************************************/

/**
 * Auxiliar expression that checks the number of nodes in a xml file that matches the provided XPath expression. See that the context node indicates the reference node from where the XPath expression will be applied
 * @param XPath The xpath expression 
 * @param theDOMSupport See the xalanc documentation 
 * @param theContextNode The context node   
 * @param thePrefixResolver See the xalanc documentation 
 * @param theEvaluator See the xalanc documentation 
 * @return 
 */
int SimulationConfiguration::containsXPath(string XPath,DOMSupport &theDOMSupport, XalanNode *theContextNode, const PrefixResolver &thePrefixResolver, XPathEvaluator &theEvaluator)
{
      NodeRefList centerNodes; 
      
      centerNodes = theEvaluator.selectNodeList (centerNodes, theDOMSupport, theContextNode,  
                                             XalanDOMString(XPath.c_str()).c_str(),
                                             thePrefixResolver);
     
     //we simply iterate over all the metrics an create them for the job
     return centerNodes.getLength();

}


/**
 * Function that given a XML node reference extracts the ISISDispatcherPolicy information 
 * @param here Indicates if the information should be extraced to the global variables or if a new grid backfilling class should be created 
 * @param theDOMSupport See the xalanc documentation 
 * @param theContextNode The context node   
 * @param thePrefixResolver See the xalanc documentation 
 * @param theEvaluator See the xalanc documentation 
 * @param architectureConfig The architecture configuration that will be used by the scheduling policy 
 * @return A reference to the class ISISDispatcherPolicy containing the information specified in the XML node or null in case that the information has been dumped in the global variables.
 */
ISISDispatcherPolicy* SimulationConfiguration::extractISISDispatcherFields(bool here,DOMSupport &theDOMSupport, XalanNode *theContextNode, const PrefixResolver &thePrefixResolver, XPathEvaluator &theEvaluator,ArchitectureConfiguration* architectureConfig)
{

     ISISDispatcherPolicy* policy = NULL;
     vector<SchedulingPolicy*>* ISISDispatcherPoliciesCur;
     vector<ArchitectureConfiguration*>* ISISDispatcherArchitecturesCur;
     vector<string>* ISISDispatcherParaverTracesNameCur;
     vector<string>* ISISDispatcherCentersNameCur;
     vector<double> * ISISDispatcherEmulateCPUFactorCur;
     metric_t ISISDispatcherMetric2OptimizeCur = WAITTIME;
     statistic_t ISISDispatcherStatistic2OptimizeCur = AVG;
     int amountOfKnownCentersCur = -1;
          
     
     if(!here)
     {
       policy = new ISISDispatcherPolicy(log,this->GlobalPredictionService != NULL, this->GlobalPredictionService);
       ISISDispatcherArchitecturesCur = new vector<ArchitectureConfiguration*>;
       ISISDispatcherPoliciesCur = new vector<SchedulingPolicy*>;
       ISISDispatcherParaverTracesNameCur = new vector<string>;
       ISISDispatcherCentersNameCur = new vector<string>;
       ISISDispatcherEmulateCPUFactorCur  = new vector<double>;
     }
     else
     {
       ISISDispatcherArchitecturesCur = this->ISISDispatcherArchitectures = new vector<ArchitectureConfiguration*>;  
       ISISDispatcherPoliciesCur = this->ISISDispatcherPolicies = new vector<SchedulingPolicy*>;
       ISISDispatcherParaverTracesNameCur = this->ISISDispatcherParaverTracesName = new vector<string>;
       ISISDispatcherCentersNameCur = this->ISISDispatcherCentersName = new vector<string>;
       ISISDispatcherEmulateCPUFactorCur = this->ISISDispatcherEmulateCPUFactor = new vector<double>;     
     }

    XALAN_USING_XALAN(XalanDocument)
    XALAN_USING_XALAN(XalanDocumentPrefixResolver)
    XALAN_USING_XALAN(XalanDOMString)
    XALAN_USING_XALAN(XalanNode)
    XALAN_USING_XALAN(XalanSourceTreeInit)
    XALAN_USING_XALAN(XalanSourceTreeDOMSupport)
    XALAN_USING_XALAN(XalanSourceTreeParserLiaison)
    XALAN_USING_XALAN(XObjectPtr)
    XALAN_USING_XALAN(NodeRefList)
    
    XALAN_USING_XERCES(XMLPlatformUtils)
    XALAN_USING_XALAN(XPathEvaluator)
    
    
     //WE GET THE GENERIC STUFF FOR THE POLICY 
    if(containsXPath(string("MetricToOptmize"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
    { 
       string MetricType;
     
       const XObjectPtr theMetricTypeResult(
           theEvaluator.evaluate(           
           theDOMSupport,
           theContextNode,
           XalanDOMString("MetricToOptmize").c_str(),
           thePrefixResolver));
         
       if(theMetricTypeResult.null())
       {
           printf("  No architecture type for the center ! \n");
           MetricType = "";
       }
       else    
       { 
         stringstream stMetricType;
         stMetricType << theMetricTypeResult->str();
         MetricType = stMetricType.str();                  
       }

      ISISDispatcherMetric2OptimizeCur = getMetricType(MetricType);
    }  


    if(containsXPath(string("AmountOfKnownCenters"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
    { 
       string amountOfKnownCenterStr;
     
       const XObjectPtr theamountOfKnownCenterResult(
           theEvaluator.evaluate(           
           theDOMSupport,
           theContextNode,
           XalanDOMString("AmountOfKnownCenters").c_str(),
           thePrefixResolver));
         
       if(theamountOfKnownCenterResult.null())
       {
           printf("  No architecture type for the center ! \n");
           amountOfKnownCentersCur = -1;
       }
       else    
       { 
         stringstream sttheamountOfKnownCenterResult;
         sttheamountOfKnownCenterResult << theamountOfKnownCenterResult->str();
         amountOfKnownCenterStr = sttheamountOfKnownCenterResult.str();                  
        
         amountOfKnownCentersCur = atoi(amountOfKnownCenterStr.c_str());
       }
      
    }  


     //WE GET THE SPECIFIC INFORMATION FOR EACH CENTER 

      NodeRefList centerNodes; 
      
      centerNodes = theEvaluator.selectNodeList (centerNodes, theDOMSupport, theContextNode,  
                                             XalanDOMString("Center").c_str(),
                                             thePrefixResolver);
     
     //we simply iterate over all the metrics an create them for the job
     for(int i = 0; i < centerNodes.getLength();i++)
     {
       XalanNode* centerNode = centerNodes.item(i);      
       
       policy_type_t centerPolicy;
       architecture_type_t centerArchitecture;
       string architectureFileCenter;
       string paraverTraceCenter;
       string nameCenter;
       double EmulateCPUFactorCenter = 1;
       RS_policy_type_t centerResourceSelectionPolicy = FIRST_FIT;
       double MaxAllowedRuntimeCenter = -1;
       double CostAllocationUnitCenter = -1;
       double AllocationUnitHourCenter = -1;
      
       
       /* WE GET THE GENERIC INFORMATION FOR THE CENTER */
       string Type;
     
      if(containsXPath(string("ArchitectureType"),theDOMSupport,centerNode,thePrefixResolver,theEvaluator))  
      {      
       const XObjectPtr theTypeResult(
           theEvaluator.evaluate(           
           theDOMSupport,
           centerNode,
           XalanDOMString("ArchitectureType").c_str(),
           thePrefixResolver));
         
       if(theTypeResult.null())
       {
           printf("  No architecture type for the center ! \n");
           Type = "";
       }
       else    
       { 
         stringstream stType;
         stType << theTypeResult->str();
         Type = stType.str();                  
       }
       
       centerArchitecture = this->getArchitectureType(Type);
      } 
       //************************We get the CenterName *****************************/      
      if(containsXPath(string("CenterName"),theDOMSupport,centerNode,thePrefixResolver,theEvaluator))  
      {         
       const XObjectPtr thenameCenterResult(
         theEvaluator.evaluate(
           theDOMSupport,
           centerNode,
           XalanDOMString("CenterName").c_str(),
           thePrefixResolver));
         
       if(thenameCenterResult.null())
       {
         printf("The filed nameCenter is not present in the configuration file !\n ");
         nameCenter = "";
       }
       else   
       {   
         stringstream stnameCenter;
         stnameCenter << thenameCenterResult->str();
         nameCenter = stnameCenter.str();       
       }
      }
       //************************We get the policy type *****************************/      
      if(containsXPath(string("Policy"),theDOMSupport,centerNode,thePrefixResolver,theEvaluator))  
      {         
       string spolicy; 
     
        const XObjectPtr thePolicyResult(
         theEvaluator.evaluate(
           theDOMSupport,
           centerNode,
           XalanDOMString("Policy").c_str(),
           thePrefixResolver));
          
       if(thePolicyResult.null())
       {
         printf("The filed Policy is not present in the configuration file !\n ");
         spolicy = "";
       }
       else  
       {   
         stringstream stPolicy;
         stPolicy << thePolicyResult->str();
         spolicy = stPolicy.str();  
       
         this->policyLoaded = true;
       }
     
       centerPolicy  = getPolicyType(spolicy);
      } 
       //************************We get the ResourceSelectionPolicy *****************************/      
       if(containsXPath(string("ResourceSelectionPolicy"),theDOMSupport,centerNode,thePrefixResolver,theEvaluator))  
      {     
        const XObjectPtr theResourceSelectionPolicyResult(
         theEvaluator.evaluate(
           theDOMSupport,
           centerNode,
           XalanDOMString("ResourceSelectionPolicy").c_str(),
           thePrefixResolver));
          
       if(theResourceSelectionPolicyResult.null())
       {
         printf("The filed ResourceSelectionPolicy is not present in the configuration file !\n ");
         centerResourceSelectionPolicy = FIRST_FIT;
       }
       else
       {   
         stringstream stResourceSelectionPolicy;
         stResourceSelectionPolicy << theResourceSelectionPolicyResult->str();
         string RSP = stResourceSelectionPolicy.str();
         if(RSP.compare("") != 0)  
           centerResourceSelectionPolicy = getResourceSelectionPolicyType(RSP);
       }           
     }
     
      //************************We get the maxAllowedRuntime *****************************/      
    if(containsXPath(string("maxAllowedRuntime"),theDOMSupport,centerNode,thePrefixResolver,theEvaluator))  
    {    
     const XObjectPtr themaxAllowedRuntimeResult(
       theEvaluator.evaluate(
         theDOMSupport,
         centerNode,
         XalanDOMString("maxAllowedRuntime").c_str(),
         thePrefixResolver));
         
     if(themaxAllowedRuntimeResult.null())
     {
       printf("The filed maxAllowedRuntime is not present in the configuration file !\n ");
       MaxAllowedRuntimeCenter = -1;
     }
     else
     {   
       stringstream stmaxAllowedRuntime;
       stmaxAllowedRuntime << themaxAllowedRuntimeResult->str();
       string maxAllowedRuntimeS = stmaxAllowedRuntime.str();  
       
       if(maxAllowedRuntimeS.compare("") != 0)
         MaxAllowedRuntimeCenter = atof(maxAllowedRuntimeS.c_str());
       else
         MaxAllowedRuntimeCenter = -1;
     }           
    }    
     
      //************************We get the CostAllocationUnit *****************************/      
    if(containsXPath(string("CostAllocationUnit"),theDOMSupport,centerNode,thePrefixResolver,theEvaluator))  
    {    
     const XObjectPtr theCostAllocationUnitResult(
       theEvaluator.evaluate(
         theDOMSupport,
         centerNode,
         XalanDOMString("CostAllocationUnit").c_str(),
         thePrefixResolver));
         
     if(theCostAllocationUnitResult.null())
     {
       printf("The filed CostAllocationUnit is not present in the configuration file !\n ");
       CostAllocationUnitCenter = -1;
     }
     else
     {   
       stringstream stCostAllocationUnit;
       stCostAllocationUnit << theCostAllocationUnitResult->str();
       string CostAllocationUnitS = stCostAllocationUnit.str();  
       
       if(CostAllocationUnitS.compare("") != 0)
         CostAllocationUnitCenter = atof(CostAllocationUnitS.c_str());
       else
         CostAllocationUnitCenter = -1;
     }           
    }    
          
      //************************We get the AllocationUnitHour *****************************/      
    if(containsXPath(string("AllocationUnitHour"),theDOMSupport,centerNode,thePrefixResolver,theEvaluator))  
    {    
     const XObjectPtr theAllocationUnitHourResult(
       theEvaluator.evaluate(
         theDOMSupport,
         centerNode,
         XalanDOMString("AllocationUnitHour").c_str(),
         thePrefixResolver));
         
     if(theAllocationUnitHourResult.null())
     {
       printf("The filed AllocationUnitHour is not present in the configuration file !\n ");
       AllocationUnitHourCenter = -1;
     }
     else
     {   
       stringstream stAllocationUnitHour;
       stAllocationUnitHour << theAllocationUnitHourResult->str();
       string AllocationUnitHourS = stAllocationUnitHour.str();  
       
       if(AllocationUnitHourS.compare("") != 0)
         AllocationUnitHourCenter = atof(AllocationUnitHourS.c_str());
       else
         AllocationUnitHourCenter = -1;
     }           
    }    

       
       //************************We get the ParaverTraceName *****************************/      
      if(containsXPath(string("ParaverTraceName"),theDOMSupport,centerNode,thePrefixResolver,theEvaluator))  
      {    
       const XObjectPtr theParaverTraceNameResult(
         theEvaluator.evaluate(
           theDOMSupport,
           centerNode,
           XalanDOMString("ParaverTraceName").c_str(),
           thePrefixResolver));
         
       if(theParaverTraceNameResult.null())
       {
         printf("The filed ParaverTraceName is not present in the configuration file !\n ");
         paraverTrace = "";
       }
       else   
       {   
         stringstream stParaverTraceName;
         stParaverTraceName << theParaverTraceNameResult->str();
         paraverTraceCenter = stParaverTraceName.str();       
       }
     }


   //************************We get the ArchitectureConfigurationFile *****************************/      
   if(containsXPath(string("ArchitectureConfigurationFile"),theDOMSupport,centerNode,thePrefixResolver,theEvaluator))  
   {     
      const XObjectPtr theArchitectureConfigurationFileResult(
       theEvaluator.evaluate(
         theDOMSupport,
         centerNode,
         XalanDOMString("ArchitectureConfigurationFile").c_str(),
         thePrefixResolver));
         
     if(theArchitectureConfigurationFileResult.null())
     {
       printf("The filed ArchitectureConfigurationFile is not present in the configuration file !\n ");
       architectureFileCenter = "";
     }
     else  
     {   
       stringstream stArchitectureConfigurationFile;
       stArchitectureConfigurationFile << theArchitectureConfigurationFileResult->str();
       architectureFileCenter = stArchitectureConfigurationFile.str();  
     }
     
    }
    
    //we load the architecture and set it to the scheduling policy 
    ArchitectureConfiguration * architectureCenterConfig = this->CreateArchitecture(this->log,centerArchitecture,architectureFileCenter);  
    
           //************************We get the EmulateCPUFactor *****************************/      
   if(containsXPath(string("EmulateCPUFactor"),theDOMSupport,centerNode,thePrefixResolver,theEvaluator))  
   {       
     const XObjectPtr theEmulateCPUFactorResult(
       theEvaluator.evaluate(
         theDOMSupport,
         centerNode,
         XalanDOMString("EmulateCPUFactor").c_str(),
         thePrefixResolver));
         
     if(theEmulateCPUFactorResult.null())
     {
       printf("The filed emule cpu factor is not present in the configuration file !\n ");
       EmulateCPUFactorCenter = 1;
     }
     else
     {   
       stringstream stEmulateCPUFactor;
       stEmulateCPUFactor << theEmulateCPUFactorResult->str();
       string EmulateCPUFactorS = stEmulateCPUFactor.str();  
       
       if(EmulateCPUFactorS.compare("") != 0)
         EmulateCPUFactorCenter = atof(EmulateCPUFactorS.c_str());
       else 
         EmulateCPUFactorCenter = 1;
     }           
   } 
     
      /* NOW WE GET THE POLICY INFORMATION FOR BE RETRIEVED IN THE CENTER */
      SchedulingPolicy* policy = NULL; 
       
         
       theContextNode =
         theEvaluator.selectSingleNode(
         theDOMSupport,
         centerNode,
         XalanDOMString("DeepSearchCollisionPolicy").c_str(),
         thePrefixResolver);         
       
      if (theContextNode == 0)
      {
          printf("There are no DeepSearchCollisionPolicy characteristics for the simulation\n");        
      }
      else
      {
          policy = extractDEEPFields(false,theDOMSupport,theContextNode,thePrefixResolver,theEvaluator,architectureCenterConfig);
      } 
      
      

      // OK, let's find the context node... the DeepSearchCollisionPolicy
      theContextNode =
        theEvaluator.selectSingleNode(
         theDOMSupport,
         centerNode,
         XalanDOMString("EASY").c_str(),
         thePrefixResolver);         
       
      if (theContextNode == 0)
      {
          printf("There are no EASY characteristics for the simulation\n");        
      }
      else
      {
          policy = extractEASYFields(false,theDOMSupport,theContextNode,thePrefixResolver,theEvaluator,architectureCenterConfig);
      }
      
      if(centerPolicy == FCFS)
      {
          policy = new FCFSSchedulingPolicy(architectureCenterConfig,log,0);
      }
            
      policy->setEmulateCPUFactor(EmulateCPUFactorCenter);
      policy->setRS_policyUsed(centerResourceSelectionPolicy);
      
      if(paraverTraceCenter.compare("") != 0)
      {
        ParaverTrace* paravertrace = new ParaverTrace(paraverTraceCenter,
                                                 this->paraverOutPutDir,
                                                 0, //right now the number of jobs that will run o the center is unkown 
                                                 architectureCenterConfig,
                                                 centerArchitecture);
        paravertrace->setLog(this->log);
        policy->setPrvTrace(paravertrace); 
      }
            
      //we set the common stuff for each policy 
      policy->setEmulateCPUFactor(EmulateCPUFactorCenter);
      policy->setRS_policyUsed(centerResourceSelectionPolicy);
      policy->setMaxAllowedRuntime(MaxAllowedRuntimeCenter);
      policy->setCostAllocationUnit(CostAllocationUnitCenter);
      policy->setAllocationUnitHour(AllocationUnitHourCenter);
            
      ISISDispatcherArchitecturesCur->push_back(architectureCenterConfig);
      ISISDispatcherPoliciesCur->push_back(policy);
      ISISDispatcherParaverTracesNameCur->push_back(paraverTraceCenter);
      ISISDispatcherCentersNameCur->push_back(nameCenter);
      ISISDispatcherEmulateCPUFactorCur->push_back(EmulateCPUFactorCenter);      
     
     }
     
     if(!here)
     {
       //must be done in this order, due to setting the policies, requires to know previously the centers name 
       policy = new ISISDispatcherPolicy(log,this->GlobalPredictionService != NULL, this->GlobalPredictionService);
       policy->setISISDispatcherArchitectures( ISISDispatcherArchitecturesCur);
       policy->setISISDispatcherCentersName(ISISDispatcherCentersName);
       policy->setISISDispatcherPolicies( ISISDispatcherPoliciesCur);
       policy->setISISDispatcherParaverTracesName(ISISDispatcherParaverTracesNameCur);
       policy->setISISDispatcherMetric2Optimize(ISISDispatcherMetric2OptimizeCur);
       policy->setISISDispatcherStatistic2Optimize(ISISDispatcherStatistic2OptimizeCur);            
       policy->setISISDispatcherEmulateCPUFactor(ISISDispatcherEmulateCPUFactorCur);
       policy->setAmountOfKnownCenters(amountOfKnownCentersCur);
     }
     else
     {
       this->ISISDispatcherPolicies = ISISDispatcherPoliciesCur;
       this->ISISDispatcherArchitectures = ISISDispatcherArchitecturesCur;
       this->ISISDispatcherParaverTracesName = ISISDispatcherParaverTracesNameCur;
       this->ISISDispatcherMetric2Optimize = ISISDispatcherMetric2OptimizeCur;
       this->ISISDispatcherStatistic2Optimize = ISISDispatcherStatistic2OptimizeCur;
       this->ISISDispatcherCentersName = ISISDispatcherCentersNameCur;
       this->ISISDispatcherEmulateCPUFactor = ISISDispatcherEmulateCPUFactorCur;
       this->amountOfKnownCenters = amountOfKnownCentersCur;
     }
     
     
     return policy;
}


/**
 * Function that given a XML node reference extracts the BRANKPolicy information 
 * @param here Indicates if the information should be extraced to the global variables or if a new grid backfilling class should be created 
 * @param theDOMSupport See the xalanc documentation 
 * @param theContextNode The context node   
 * @param thePrefixResolver See the xalanc documentation 
 * @param theEvaluator See the xalanc documentation 
 * @param architectureConfig The architecture configuration that will be used by the scheduling policy 
 * @return A reference to the class BRANKPolicy containing the information specified in the XML node or null in case that the information has been dumped in the global variables.
 */
MetaBrokeringSystem* SimulationConfiguration::extractMetaBrokeringSystemFields(bool here,DOMSupport &theDOMSupport, XalanNode *theContextNode, const PrefixResolver &thePrefixResolver, XPathEvaluator &theEvaluator,ArchitectureConfiguration* architectureConfig)
{

    MetaBrokeringSystem* policy = NULL;
    vector<SchedulingPolicy*>* MBVirtualOrganitzationsCur = new vector<SchedulingPolicy*>();
    vector<string> VONamesCur;
          
    XALAN_USING_XALAN(XalanDocument)
    XALAN_USING_XALAN(XalanDocumentPrefixResolver)
    XALAN_USING_XALAN(XalanDOMString)
    XALAN_USING_XALAN(XalanNode)
    XALAN_USING_XALAN(XalanSourceTreeInit)
    XALAN_USING_XALAN(XalanSourceTreeDOMSupport)
    XALAN_USING_XALAN(XalanSourceTreeParserLiaison)
    XALAN_USING_XALAN(XObjectPtr)
    XALAN_USING_XALAN(NodeRefList)
    
    XALAN_USING_XERCES(XMLPlatformUtils)
    XALAN_USING_XALAN(XPathEvaluator)
    
     //WE GET THE SPECIFIC INFORMATION FOR EACH Virtual Organitzation 

      NodeRefList centerNodes; 
      
      centerNodes = theEvaluator.selectNodeList (centerNodes, theDOMSupport, theContextNode,  
                                             XalanDOMString("VirtualOrganitzation").c_str(),
                                             thePrefixResolver);
     
     //we simply iterate over all the metrics an create them for the job
     for(int i = 0; i < centerNodes.getLength();i++)
     {
       XalanNode* centerNode = centerNodes.item(i);      
       
       policy_type_t brokeringPolicy;
       string VOName;
      
       
       //************************We get the VOName *****************************/      
      if(containsXPath(string("VOName"),theDOMSupport,centerNode,thePrefixResolver,theEvaluator))  
      {         
       const XObjectPtr theVONameResult(
         theEvaluator.evaluate(
           theDOMSupport,
           centerNode,
           XalanDOMString("VOName").c_str(),
           thePrefixResolver));
         
       if(theVONameResult.null())
       {
         printf("The filed VOName is not present in the configuration file !\n ");
         VOName = "";
       }
       else   
       {   
         stringstream stVOName;
         stVOName << theVONameResult->str();
         VOName = stVOName.str();       
       }
      }
      //************************We get the policy type *****************************/      
      if(containsXPath(string("MetaBrokeringPolicy"),theDOMSupport,centerNode,thePrefixResolver,theEvaluator))  
      {         
       string spolicy; 
     
        const XObjectPtr thePolicyResult(
         theEvaluator.evaluate(
           theDOMSupport,
           centerNode,
           XalanDOMString("Policy").c_str(),
           thePrefixResolver));
          
       if(thePolicyResult.null())
       {
         printf("The filed Policy is not present in the configuration file !\n ");
         spolicy = "";
       }
       else  
       {   
         stringstream stPolicy;
         stPolicy << thePolicyResult->str();
         spolicy = stPolicy.str();  
       
         this->policyLoaded = true;
       }
     
       brokeringPolicy  = getPolicyType(spolicy);
      } 

     
      /* NOW WE GET THE POLICY INFORMATION FOR BE RETRIEVED VIRTUAL Organitzation  */
      SchedulingPolicy* policy = NULL; 
       
         
       theContextNode =
         theEvaluator.selectSingleNode(
         theDOMSupport,
         centerNode,
         XalanDOMString("BRANKPolicy").c_str(),
         thePrefixResolver);         
       
      if (theContextNode == 0)
      {
          printf("There are no BRANK policy characteristics for the simulation\n");        
      }
      else
      {
          policy = extractBRANKPolicyFields(false,theDOMSupport,theContextNode,thePrefixResolver,theEvaluator,NULL);
      } 
      
      policy->setCenterName(VOName);
                  
      MBVirtualOrganitzationsCur->push_back(policy);
      VONamesCur.push_back(VOName);     
     }
     
     if(!here)
     {
       //must be done in this order, due to setting the policies, requires to know previously the centers name 
       policy = new MetaBrokeringSystem(log,this->GlobalPredictionService != NULL, this->GlobalPredictionService);
       ((MetaBrokeringSystem*)policy)->setVirtualOrganitzations(MBVirtualOrganitzationsCur);
       ((MetaBrokeringSystem*)policy)->setVONames(VONamesCur);
     }
     else
     {
       this->MBVirtualOrganitzations = MBVirtualOrganitzationsCur;
       this->VONames = VONamesCur;
     }
     
     
     return policy;
}

  


/**
 * Function that given a XML node reference extracts the BRANKPolicy information 
 * @param here Indicates if the information should be extraced to the global variables or if a new grid backfilling class should be created 
 * @param theDOMSupport See the xalanc documentation 
 * @param theContextNode The context node   
 * @param thePrefixResolver See the xalanc documentation 
 * @param theEvaluator See the xalanc documentation 
 * @param architectureConfig The architecture configuration that will be used by the scheduling policy 
 * @return A reference to the class BRANKPolicy containing the information specified in the XML node or null in case that the information has been dumped in the global variables.
 */
BRANKPolicy* SimulationConfiguration::extractBRANKPolicyFields(bool here,DOMSupport &theDOMSupport, XalanNode *theContextNode, const PrefixResolver &thePrefixResolver, XPathEvaluator &theEvaluator,ArchitectureConfiguration* architectureConfig)
{

     BRANKPolicy* policy = NULL;
     vector<SchedulingPolicy*>* BRANKPolicyPoliciesCur;
     vector<ArchitectureConfiguration*>* BRANKPolicyArchitecturesCur;
     vector<string>* BRANKPolicyParaverTracesNameCur;
     vector<string>* BRANKPolicyCentersNameCur;
     vector<double> * BRANKPolicyEmulateCPUFactorCur;
     metric_t BRANKPolicyMetric2OptimizeCur = WAITTIME;
     statistic_t BRANKPolicyStatistic2OptimizeCur = AVG;
          
     if(!here)
     {       
       BRANKPolicyArchitecturesCur = new vector<ArchitectureConfiguration*>;  
       BRANKPolicyPoliciesCur = new vector<SchedulingPolicy*>;
       BRANKPolicyParaverTracesNameCur = new vector<string>;
       BRANKPolicyCentersNameCur = new vector<string>;
       BRANKPolicyEmulateCPUFactorCur  = new vector<double>;
   
     }
     else
     {
       BRANKPolicyArchitecturesCur = this->BRANKPolicyArchitectures = new vector<ArchitectureConfiguration*>;  
       BRANKPolicyPoliciesCur = this->BRANKPolicyPolicies = new vector<SchedulingPolicy*>;
       BRANKPolicyParaverTracesNameCur = this->BRANKPolicyParaverTracesName = new vector<string>;
       BRANKPolicyCentersNameCur = this->BRANKPolicyCentersName = new vector<string>;
       BRANKPolicyEmulateCPUFactorCur = this->BRANKPolicyEmulateCPUFactor = new vector<double>;     
     }

    XALAN_USING_XALAN(XalanDocument)
    XALAN_USING_XALAN(XalanDocumentPrefixResolver)
    XALAN_USING_XALAN(XalanDOMString)
    XALAN_USING_XALAN(XalanNode)
    XALAN_USING_XALAN(XalanSourceTreeInit)
    XALAN_USING_XALAN(XalanSourceTreeDOMSupport)
    XALAN_USING_XALAN(XalanSourceTreeParserLiaison)
    XALAN_USING_XALAN(XObjectPtr)
    XALAN_USING_XALAN(NodeRefList)
    
    XALAN_USING_XERCES(XMLPlatformUtils)
    XALAN_USING_XALAN(XPathEvaluator)
    
     //WE GET THE SPECIFIC INFORMATION FOR EACH CENTER 

      NodeRefList centerNodes; 
      
      centerNodes = theEvaluator.selectNodeList (centerNodes, theDOMSupport, theContextNode,  
                                             XalanDOMString("Center").c_str(),
                                             thePrefixResolver);
     
     //we simply iterate over all the metrics an create them for the job
     for(int i = 0; i < centerNodes.getLength();i++)
     {
       XalanNode* centerNode = centerNodes.item(i);      
       
       policy_type_t centerPolicy;
       architecture_type_t centerArchitecture;
       string architectureFileCenter;
       string paraverTraceCenter;
       string nameCenter;
       double EmulateCPUFactorCenter = 1;
       RS_policy_type_t centerResourceSelectionPolicy = FIRST_FIT;
       double MaxAllowedRuntimeCenter = -1;
       double CostAllocationUnitCenter = -1;
       double AllocationUnitHourCenter = -1;
      
       
       /* WE GET THE GENERIC INFORMATION FOR THE CENTER */
       string Type;
     
      if(containsXPath(string("ArchitectureType"),theDOMSupport,centerNode,thePrefixResolver,theEvaluator))  
      {      
       const XObjectPtr theTypeResult(
           theEvaluator.evaluate(           
           theDOMSupport,
           centerNode,
           XalanDOMString("ArchitectureType").c_str(),
           thePrefixResolver));
         
       if(theTypeResult.null())
       {
           printf("  No architecture type for the center ! \n");
           Type = "";
       }
       else    
       { 
         stringstream stType;
         stType << theTypeResult->str();
         Type = stType.str();                  
       }
       
       centerArchitecture = this->getArchitectureType(Type);
      } 
       //************************We get the CenterName *****************************/      
      if(containsXPath(string("CenterName"),theDOMSupport,centerNode,thePrefixResolver,theEvaluator))  
      {         
       const XObjectPtr thenameCenterResult(
         theEvaluator.evaluate(
           theDOMSupport,
           centerNode,
           XalanDOMString("CenterName").c_str(),
           thePrefixResolver));
         
       if(thenameCenterResult.null())
       {
         printf("The filed nameCenter is not present in the configuration file !\n ");
         nameCenter = "";
       }
       else   
       {   
         stringstream stnameCenter;
         stnameCenter << thenameCenterResult->str();
         nameCenter = stnameCenter.str();       
       }
      }
       //************************We get the policy type *****************************/      
      if(containsXPath(string("Policy"),theDOMSupport,centerNode,thePrefixResolver,theEvaluator))  
      {         
       string spolicy; 
     
        const XObjectPtr thePolicyResult(
         theEvaluator.evaluate(
           theDOMSupport,
           centerNode,
           XalanDOMString("Policy").c_str(),
           thePrefixResolver));
          
       if(thePolicyResult.null())
       {
         printf("The filed Policy is not present in the configuration file !\n ");
         spolicy = "";
       }
       else  
       {   
         stringstream stPolicy;
         stPolicy << thePolicyResult->str();
         spolicy = stPolicy.str();  
       
         this->policyLoaded = true;
       }
     
       centerPolicy  = getPolicyType(spolicy);
      } 
       //************************We get the ResourceSelectionPolicy *****************************/      
       if(containsXPath(string("ResourceSelectionPolicy"),theDOMSupport,centerNode,thePrefixResolver,theEvaluator))  
      {     
        const XObjectPtr theResourceSelectionPolicyResult(
         theEvaluator.evaluate(
           theDOMSupport,
           centerNode,
           XalanDOMString("ResourceSelectionPolicy").c_str(),
           thePrefixResolver));
          
       if(theResourceSelectionPolicyResult.null())
       {
         printf("The filed ResourceSelectionPolicy is not present in the configuration file !\n ");
         centerResourceSelectionPolicy = FIRST_FIT;
       }
       else
       {   
         stringstream stResourceSelectionPolicy;
         stResourceSelectionPolicy << theResourceSelectionPolicyResult->str();
         string RSP = stResourceSelectionPolicy.str();
         if(RSP.compare("") != 0)  
           centerResourceSelectionPolicy = getResourceSelectionPolicyType(RSP);
       }           
     }
     
      //************************We get the maxAllowedRuntime *****************************/      
    if(containsXPath(string("maxAllowedRuntime"),theDOMSupport,centerNode,thePrefixResolver,theEvaluator))  
    {    
     const XObjectPtr themaxAllowedRuntimeResult(
       theEvaluator.evaluate(
         theDOMSupport,
         centerNode,
         XalanDOMString("maxAllowedRuntime").c_str(),
         thePrefixResolver));
         
     if(themaxAllowedRuntimeResult.null())
     {
       printf("The filed maxAllowedRuntime is not present in the configuration file !\n ");
       MaxAllowedRuntimeCenter = -1;
     }
     else
     {   
       stringstream stmaxAllowedRuntime;
       stmaxAllowedRuntime << themaxAllowedRuntimeResult->str();
       string maxAllowedRuntimeS = stmaxAllowedRuntime.str();  
       
       if(maxAllowedRuntimeS.compare("") != 0)
         MaxAllowedRuntimeCenter = atof(maxAllowedRuntimeS.c_str());
       else
         MaxAllowedRuntimeCenter = -1;
     }           
    }    
     
      //************************We get the CostAllocationUnit *****************************/      
    if(containsXPath(string("CostAllocationUnit"),theDOMSupport,centerNode,thePrefixResolver,theEvaluator))  
    {    
     const XObjectPtr theCostAllocationUnitResult(
       theEvaluator.evaluate(
         theDOMSupport,
         centerNode,
         XalanDOMString("CostAllocationUnit").c_str(),
         thePrefixResolver));
         
     if(theCostAllocationUnitResult.null())
     {
       printf("The filed CostAllocationUnit is not present in the configuration file !\n ");
       CostAllocationUnitCenter = -1;
     }
     else
     {   
       stringstream stCostAllocationUnit;
       stCostAllocationUnit << theCostAllocationUnitResult->str();
       string CostAllocationUnitS = stCostAllocationUnit.str();  
       
       if(CostAllocationUnitS.compare("") != 0)
         CostAllocationUnitCenter = atof(CostAllocationUnitS.c_str());
       else
         CostAllocationUnitCenter = -1;
     }           
    }    
          
      //************************We get the AllocationUnitHour *****************************/      
    if(containsXPath(string("AllocationUnitHour"),theDOMSupport,centerNode,thePrefixResolver,theEvaluator))  
    {    
     const XObjectPtr theAllocationUnitHourResult(
       theEvaluator.evaluate(
         theDOMSupport,
         centerNode,
         XalanDOMString("AllocationUnitHour").c_str(),
         thePrefixResolver));
         
     if(theAllocationUnitHourResult.null())
     {
       printf("The filed AllocationUnitHour is not present in the configuration file !\n ");
       AllocationUnitHourCenter = -1;
     }
     else
     {   
       stringstream stAllocationUnitHour;
       stAllocationUnitHour << theAllocationUnitHourResult->str();
       string AllocationUnitHourS = stAllocationUnitHour.str();  
       
       if(AllocationUnitHourS.compare("") != 0)
         AllocationUnitHourCenter = atof(AllocationUnitHourS.c_str());
       else
         AllocationUnitHourCenter = -1;
     }           
    }    
             
            
       
       //************************We get the ParaverTraceName *****************************/      
      if(containsXPath(string("ParaverTraceName"),theDOMSupport,centerNode,thePrefixResolver,theEvaluator))  
      {    
       const XObjectPtr theParaverTraceNameResult(
         theEvaluator.evaluate(
           theDOMSupport,
           centerNode,
           XalanDOMString("ParaverTraceName").c_str(),
           thePrefixResolver));
         
       if(theParaverTraceNameResult.null())
       {
         printf("The filed ParaverTraceName is not present in the configuration file !\n ");
         paraverTrace = "";
       }
       else   
       {   
         stringstream stParaverTraceName;
         stParaverTraceName << theParaverTraceNameResult->str();
         paraverTraceCenter = stParaverTraceName.str();       
       }
     }


   //************************We get the ArchitectureConfigurationFile *****************************/      
   if(containsXPath(string("ArchitectureConfigurationFile"),theDOMSupport,centerNode,thePrefixResolver,theEvaluator))  
   {     
      const XObjectPtr theArchitectureConfigurationFileResult(
       theEvaluator.evaluate(
         theDOMSupport,
         centerNode,
         XalanDOMString("ArchitectureConfigurationFile").c_str(),
         thePrefixResolver));
         
     if(theArchitectureConfigurationFileResult.null())
     {
       printf("The filed ArchitectureConfigurationFile is not present in the configuration file !\n ");
       architectureFileCenter = "";
     }
     else  
     {   
       stringstream stArchitectureConfigurationFile;
       stArchitectureConfigurationFile << theArchitectureConfigurationFileResult->str();
       architectureFileCenter = stArchitectureConfigurationFile.str();  
     }
     
    }
    
    //we load the architecture and set it to the scheduling policy 
    ArchitectureConfiguration * architectureCenterConfig = this->CreateArchitecture(this->log,centerArchitecture,architectureFileCenter);  
    
           //************************We get the EmulateCPUFactor *****************************/      
   if(containsXPath(string("EmulateCPUFactor"),theDOMSupport,centerNode,thePrefixResolver,theEvaluator))  
   {       
     const XObjectPtr theEmulateCPUFactorResult(
       theEvaluator.evaluate(
         theDOMSupport,
         centerNode,
         XalanDOMString("EmulateCPUFactor").c_str(),
         thePrefixResolver));
         
     if(theEmulateCPUFactorResult.null())
     {
       printf("The filed jobRequirementsFile is not present in the configuration file !\n ");
       EmulateCPUFactorCenter = 1;
     }
     else
     {   
       stringstream stEmulateCPUFactor;
       stEmulateCPUFactor << theEmulateCPUFactorResult->str();
       string EmulateCPUFactorS = stEmulateCPUFactor.str();  
       
       if(EmulateCPUFactorS.compare("") != 0)
         EmulateCPUFactorCenter = atof(EmulateCPUFactorS.c_str());
       else 
         EmulateCPUFactorCenter = 1;
     }           
   } 
     
      /* NOW WE GET THE POLICY INFORMATION FOR BE RETRIEVED IN THE CENTER */
      SchedulingPolicy* policy = NULL; 
       
         
       theContextNode =
         theEvaluator.selectSingleNode(
         theDOMSupport,
         centerNode,
         XalanDOMString("DeepSearchCollisionPolicy").c_str(),
         thePrefixResolver);         
       
      if (theContextNode == 0)
      {
          printf("There are no DeepSearchCollisionPolicy characteristics for the simulation\n");        
      }
      else
      {
          policy = extractDEEPFields(false,theDOMSupport,theContextNode,thePrefixResolver,theEvaluator,architectureCenterConfig);
      } 
      
      

      // OK, let's find the context node... the DeepSearchCollisionPolicy
      theContextNode =
        theEvaluator.selectSingleNode(
         theDOMSupport,
         centerNode,
         XalanDOMString("EASY").c_str(),
         thePrefixResolver);         
       
      if (theContextNode == 0)
      {
          printf("There are no EASY characteristics for the simulation\n");        
      }
      else
      {
          policy = extractEASYFields(false,theDOMSupport,theContextNode,thePrefixResolver,theEvaluator,architectureCenterConfig);
      }
      
      if(centerPolicy == FCFS)
      {
          policy = new FCFSSchedulingPolicy(architectureCenterConfig,log,0);
      }
            
      policy->setEmulateCPUFactor(EmulateCPUFactorCenter);
      policy->setRS_policyUsed(centerResourceSelectionPolicy);
      
      if(paraverTraceCenter.compare("") != 0)
      {
        ParaverTrace* paravertrace = new ParaverTrace(paraverTraceCenter,
                                                 this->paraverOutPutDir,
                                                 0, //right now the number of jobs that will run o the center is unkown 
                                                 architectureCenterConfig,
                                                 centerArchitecture);
        paravertrace->setLog(this->log);
        policy->setPrvTrace(paravertrace); 
      }
            
      //we set the common stuff for each policy 
      policy->setEmulateCPUFactor(EmulateCPUFactorCenter);
      policy->setRS_policyUsed(centerResourceSelectionPolicy);
      policy->setMaxAllowedRuntime(MaxAllowedRuntimeCenter);
      policy->setCostAllocationUnit(CostAllocationUnitCenter);
      policy->setAllocationUnitHour(AllocationUnitHourCenter);
            
      BRANKPolicyArchitecturesCur->push_back(architectureCenterConfig);
      BRANKPolicyPoliciesCur->push_back(policy);
      BRANKPolicyParaverTracesNameCur->push_back(paraverTraceCenter);
      BRANKPolicyCentersNameCur->push_back(nameCenter);
      BRANKPolicyEmulateCPUFactorCur->push_back(EmulateCPUFactorCenter);      
     
     }
     
     if(!here)
     {
       //must be done in this order, due to setting the policies, requires to know previously the centers name 
       policy = new BRANKPolicy(log,this->GlobalPredictionService != NULL, this->GlobalPredictionService);
       policy->setBRANKPolicyArchitectures( BRANKPolicyArchitecturesCur);
       policy->setBRANKPolicyCentersName(BRANKPolicyCentersNameCur);
       policy->setBRANKPolicyPolicies( BRANKPolicyPoliciesCur);
       policy->setBRANKPolicyParaverTracesName(BRANKPolicyParaverTracesNameCur);
       policy->setBRANKPolicyEmulateCPUFactor(BRANKPolicyEmulateCPUFactorCur);
     }
     else
     {
       this->BRANKPolicyPolicies = BRANKPolicyPoliciesCur;
       this->BRANKPolicyArchitectures = BRANKPolicyArchitecturesCur;
       this->BRANKPolicyParaverTracesName = BRANKPolicyParaverTracesNameCur;
       this->BRANKPolicyCentersName = BRANKPolicyCentersNameCur;
       this->BRANKPolicyEmulateCPUFactor = BRANKPolicyEmulateCPUFactorCur;
     }
     
     
     return policy;
}

  

/**
 * Function that given a XML node reference extracts the DeepSearchCollisionPolicy information 
 * @param here Indicates if the information should be extraced to the global variables or if a new grid backfilling class should be created 
 * @param theDOMSupport See the xalanc documentation 
 * @param theContextNode The context node   
 * @param thePrefixResolver See the xalanc documentation 
 * @param theEvaluator See the xalanc documentation 
 * @param architectureConfig The architecture configuration that will be used by the scheduling policy 
 * @return A reference to the class DeepSearchCollisionPolicy containing the information specified in the XML node or null in case that the information has been dumped in the global variables.
 */
DeepSearchCollisionPolicy* SimulationConfiguration::extractDEEPFields(bool here,DOMSupport &theDOMSupport, XalanNode *theContextNode, const PrefixResolver &thePrefixResolver, XPathEvaluator &theEvaluator,ArchitectureConfiguration * architectureConfig)
{
     metric_t DSCPmetricTypeCur;
     statistic_t DSCPstatisticTypeCur;
     bool DSCPgenerateAllParaversRTablesCur;
     string DSCPparaverOutPutDirCur;
     string DSCPparaverTraceCur;
     
     DeepSearchCollisionPolicy* policy; 

     
     //************************We get the metricType *****************************/      
   if(containsXPath(string("metricType"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
   {          
      const XObjectPtr themetricTypeSResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("metricType").c_str(),
         thePrefixResolver));
         
     string metricTypeS;
         
     if(themetricTypeSResult.null())
     {
       printf("The filed metricTypeS is not present in the configuration file ! \n");
       metricTypeS = "";
     }
     else
     {   
       stringstream stmetricTypeS;
       stmetricTypeS << themetricTypeSResult->str();
       metricTypeS = stmetricTypeS.str();
       
       this->DSCPmetricTypeLoaded = true;
     }
            
     DSCPmetricTypeCur = getMetricType(metricTypeS);
   }
     //************************We get the ParaverTraceName *****************************/      
   if(containsXPath(string("ParaverTraceName"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
   {      
     const XObjectPtr theParaverTraceNameResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("ParaverTraceName").c_str(),
         thePrefixResolver));
         
     if(theParaverTraceNameResult.null())
     {
       printf("The filed ParaverTraceName is not present in the configuration file !\n ");
       paraverTrace = "";
     }
     else   
     {   
       stringstream stParaverTraceName;
       stParaverTraceName << theParaverTraceNameResult->str();
       DSCPparaverTraceCur = stParaverTraceName.str();       
     }
    }   
     //************************We get the ParaverOutputDir *****************************/      
   if(containsXPath(string("ParaverOutputDir"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
   {        
      const XObjectPtr theParaverOutputDirResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("ParaverOutputDir").c_str(),
         thePrefixResolver));
         
     if(theParaverOutputDirResult.null())
     {
       printf("The filed ParaverOutputDir is not present in the configuration file !\n ");
       paraverOutPutDir = "";
     }
     else  
     {   
       stringstream stParaverOutputDir;
       stParaverOutputDir << theParaverOutputDirResult->str();
       DSCPparaverOutPutDirCur = stParaverOutputDir.str();  
     }     
    } 
      //************************We get the statisticTypeS *****************************/      
   if(containsXPath(string("statisticType"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
   {   
      const XObjectPtr thestatisticTypeSResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("statisticType").c_str(),
         thePrefixResolver));
          
     string statisticTypeS; 
     
     if(thestatisticTypeSResult.null())
     {
       printf("The filed statisticTypeS is not present in the configuration file ! \n");
       statisticTypeS = "";
     }
     else
     {   
       stringstream ststatisticTypeS;
       ststatisticTypeS << thestatisticTypeSResult->str();
       statisticTypeS = ststatisticTypeS.str();  
       
       this->DSCPstatisticTypeLoaded = true;
     }
     
     DSCPstatisticTypeCur = getStatisticType(statisticTypeS); 
   }    
     //************************We get the generateAllParaversRTables *****************************/      
   if(containsXPath(string("generateAllParaversRTables"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
   {      
      const XObjectPtr thegenerateAllParaversRTablesSResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("generateAllParaversRTables").c_str(),
         thePrefixResolver));
         
     string generateAllParaversRTablesS;
         
     if(thegenerateAllParaversRTablesSResult.null())
     {
       printf("The filed generateAllParaversRTablesS is not present in the configuration file ! \n");
       generateAllParaversRTablesS = "";
     }
     else
     {
	DSCPgenerateAllParaversRTablesCur = true;
     }   
   }  
   
   if(here)
   {
     this->DSCPmetricType = DSCPmetricTypeCur;
     this->DSCPgenerateAllParaversRTables = DSCPgenerateAllParaversRTablesCur;
     this->DSCPstatisticType = DSCPstatisticTypeCur;
     this->DSCPparaverOutPutDir = DSCPparaverOutPutDirCur;
     this->DSCPparaverTrace = DSCPparaverTraceCur;
   }
   else
     //the rest of stuff will have to be set 
    return (DeepSearchCollisionPolicy*) this->CreateDEEPPolicy(architectureConfig,this->log,0,DSCPmetricTypeCur,DSCPstatisticTypeCur,DSCPgenerateAllParaversRTablesCur,DSCPparaverOutPutDirCur,DSCPparaverTraceCur);

   return policy;
}

/**
 * Function that given a XML node reference extracts the EASY information 
 * @param here Indicates if the information should be extraced to the global variables or if a new grid backfilling class should be created 
 * @param theDOMSupport See the xalanc documentation 
 * @param theContextNode The context node   
 * @param thePrefixResolver See the xalanc documentation 
 * @param theEvaluator See the xalanc documentation 
 * @param architectureConfig The architecture configuration that will be used by the scheduling policy 
 * @return A reference to the class EASY containing the information specified in the XML node or null in case that the information has been dumped in the global variables.
 */
EASYSchedulingPolicy* SimulationConfiguration::extractEASYFields(bool here,DOMSupport &theDOMSupport, XalanNode *theContextNode, const PrefixResolver &thePrefixResolver, XPathEvaluator &theEvaluator,ArchitectureConfiguration* architectureConfig)
{
    XALAN_USING_XALAN(XalanDocument)
    XALAN_USING_XALAN(XalanDocumentPrefixResolver)
    XALAN_USING_XALAN(XalanDOMString)
    XALAN_USING_XALAN(XalanNode)
    XALAN_USING_XALAN(XalanSourceTreeInit)
    XALAN_USING_XALAN(XalanSourceTreeDOMSupport)
    XALAN_USING_XALAN(XalanSourceTreeParserLiaison)
    XALAN_USING_XALAN(XObjectPtr)
    XALAN_USING_XALAN(NodeRefList)
    
    XALAN_USING_XERCES(XMLPlatformUtils)
    XALAN_USING_XALAN(XPathEvaluator)
    
    int numberOfReservationsCur = -1;
    Backfilling_variant_t backfillingVariantCur = EASY_BACKFILLING;
    string PredictorConfigFileCur = "";
    bool AllocateAllJobsInTheRTCur = false;
    bool SimulateResourceUsageCur = false;
    bool tryToBackfillAllwaysCur = false;
    
    EASYSchedulingPolicy* policy = NULL;
   
   //If here is true, means that the extracted fields must be extract in the local variables rather than creating the appropiate scheduling policy 
      //************************We get the Number Of reservations *****************************/      
    if(containsXPath(string("numberOfReservations"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
    {   
      const XObjectPtr thenumberOfReservationsResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("numberOfReservations").c_str(),
         thePrefixResolver));        
         
     if(thenumberOfReservationsResult.null())
     {
       printf("The filed numberOfReservations is not present in the configuration file.\n");
       numberOfReservations = 1;
     }
     else
     {   
       stringstream stnumberOfReservations;
       stnumberOfReservations << thenumberOfReservationsResult->str();
       numberOfReservationsCur = atoi((stnumberOfReservations.str()).c_str());
     }   
    } 
    
    //************************We get the tryToBackfillAllways *****************************/      
    if(containsXPath(string("tryToBackfillAllways"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
    {   
      const XObjectPtr thetryToBackfillAllwaysResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("tryToBackfillAllways").c_str(),
         thePrefixResolver));        
         
     if(thetryToBackfillAllwaysResult.null())
     {
       printf("The filed tryToBackfillAllways is not present in the configuration file.\n");
       tryToBackfillAllways = 1;
     }
     else
     {   
       stringstream sttryToBackfillAllways;
       sttryToBackfillAllways << thetryToBackfillAllwaysResult->str();
       if((sttryToBackfillAllways.str()).compare("TRUE") == 0)
         tryToBackfillAllwaysCur = true;
       else
         tryToBackfillAllwaysCur = false;
     }   
    } 
    
      //************************We get the predictor type to be used in rather user stimates *****************************/           
    if(containsXPath(string("PredictorConfigurationFile"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
    {    
      const XObjectPtr thePredictorConfigFileResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
		XalanDOMString("PredictorConfigurationFile").c_str(),
         thePrefixResolver));        
         
     if(thePredictorConfigFileResult.null())
     {
       printf("The filed PredictorConfigurationFile is not present in the configuration file.\n");
       PredictorConfigFile = "";
     }
     else
     {   
       stringstream stPredictorConfigFile;
       stPredictorConfigFile << thePredictorConfigFileResult->str();
       PredictorConfigFileCur = stPredictorConfigFile.str();
     }   
    }
     //************************We get if all the jobs have to be allocated in the RT  *****************************/           
          
    if(containsXPath(string("AllocateAllJobsInTheRT"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
    {       
      const XObjectPtr theAllocateAllJobsInTheRTResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("AllocateAllJobsInTheRT").c_str(),
         thePrefixResolver));        
         
     if(theAllocateAllJobsInTheRTResult.null())
     {
       printf("The filed AllocateAllJobsInTheRT is not present in the configuration file.\n");
       PredictorConfigFile = "";
     }
     else
     {   
       stringstream stAllocateAllJobsInTheRT;
       stAllocateAllJobsInTheRT << theAllocateAllJobsInTheRTResult->str();
       string AllocateAllJobsInTheRTS = stAllocateAllJobsInTheRT.str();
       
       if(AllocateAllJobsInTheRTS.compare("YES") == 0)
         AllocateAllJobsInTheRTCur = true;
     }  
    }  
     //************************We get if the resource consumption has to be used in the simulation   *****************************/           
          
    if(containsXPath(string("SimulateResourceUsage"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
    {    
      const XObjectPtr theSimulateResourceUsageResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("SimulateResourceUsage").c_str(),
         thePrefixResolver));        
         
     if(theSimulateResourceUsageResult.null())
     {
       printf("The filed PredictorConfigurationFile is not present in the configuration file.\n");
       PredictorConfigFile = "";
     }
     else
     {   
       stringstream stSimulateResourceUsage;
       stSimulateResourceUsage << theSimulateResourceUsageResult->str();
       string SimulateResourceUsageS = stSimulateResourceUsage.str();
       
       if(SimulateResourceUsageS.compare("YES") == 0)
         SimulateResourceUsageCur = true;
       
     }  
    }     



        //************************We get the backfillingVariant *****************************/      
    if(containsXPath(string("backfillingVariant"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
    {     
      const XObjectPtr thebackfillingVariantResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("backfillingVariant").c_str(),
         thePrefixResolver));        
         
     if(thebackfillingVariantResult.null())
     {
       printf("The filed backfillingVariant is not present in the configuration file. \n");       
     }
     else
     {   
       stringstream stbackfillingVariant;
       stbackfillingVariant << thebackfillingVariantResult->str();
       string sbackfillingVariant = (stbackfillingVariant.str()).c_str();
       
       backfillingVariantCur = getBackfillingVariantType(sbackfillingVariant);
     }
   }  

   if(containsXPath(string("powerAwareBackfilling"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
   {  
      const XObjectPtr thepowerAwareBackfillingSResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("powerAwareBackfilling").c_str(),
         thePrefixResolver));
         
     string powerAwareBackfillingS;
         
     if(thepowerAwareBackfillingSResult.null())
     {
       printf("The filed powerAwareBackfillingS is not present in the configuration file !\n ");
       powerAwareBackfilling = "";
     }
     else
     {
       stringstream stpowerAwareBackfilling;
       stpowerAwareBackfilling << thepowerAwareBackfillingSResult->str();
       powerAwareBackfillingS = stpowerAwareBackfilling.str();  
       
       if(!powerAwareBackfillingS.compare("") || !powerAwareBackfillingS.compare("NO") )
         this->powerAwareBackfilling = false;
       else
         this->powerAwareBackfilling = true;
     }
   }  

   if(here)
   {
     backfillingVariant = backfillingVariantCur ;
     numberOfReservations = numberOfReservationsCur;
     PredictorConfigFile = PredictorConfigFileCur;
     AllocateAllJobsInTheRT = AllocateAllJobsInTheRTCur;
     SimulateResourceUsage = SimulateResourceUsageCur;
     tryToBackfillAllways = tryToBackfillAllwaysCur;
   }
   else
   {          
     //the architecture, the log and the startime will have to be set for other functionality
     return (EASYSchedulingPolicy*) this->CreateEASYPolicy(architectureConfig,this->log,0,PredictorConfigFileCur,numberOfReservationsCur,backfillingVariantCur,AllocateAllJobsInTheRTCur, SimulateResourceUsageCur,this->LessConsumeAlternativeThreshold,this->LessConsumeThreshold,tryToBackfillAllwaysCur, this->powerAwareBackfilling);
   }
     
   return policy;

}
/**
 * Creates a predictor 
 * @param type The type of predictor to be instantiated 
 * @param configPred The configuration information for the predictor
 * @return A reference to the predictor created 
 */

Predictor* SimulationConfiguration::CreatePredictor(PredictorConfigurationFile* configPred)
{
  Predictor* ret = NULL; 

  switch(configPred->predictorType)
  {
    case USER_RUNTIME_ESTIMATE:
    {
      ret = new UserRuntimeEstimatePred(this->log);
      break;
    }
    case HISTORICAL_CLASSIFIER:
    {
      //we add the prediction variable to the back of the response variable 
      vector<native_t>* ResponseJobAttributesType = this->GetAttributeTypes(configPred->ResponseJobAttributes);
      
      //we get thre prediction variable type 
      map<string,native_t>::iterator itAtType = this->AttributeJobType.find(configPred->PredictionVariable);
      assert(itAtType != this->AttributeJobType.end());
      native_t predT = itAtType->second;
      
      
      ret = new HistoricalClassifier(configPred->classifierType,configPred->discretizer,configPred->numberOfBins,log,configPred->ResponseJobAttributes,ResponseJobAttributesType,configPred->ModelDirectoryFile,configPred->NewFilePerModel,configPred->ModelIntervalGeneration,configPred->PredictionVariable,predT);
      
      //now we set the weka envs to the classifier 
      ((HistoricalClassifier*)ret)->setWekaEnv(this->simulatorConfiguration->JavaBinaryPath, 
                      this->simulatorConfiguration->JavaClassPath,
                      this->simulatorConfiguration->WekaJarPath,
                      this->simulatorConfiguration->TemporaryDirectory
                      );
      
      break;
    }   
    default: 
      assert(false);    
  }
  
  ret->setModelIntervalGeneration(configPred->ModelIntervalGeneration);
  
  return ret;
}

/**
 * Create a deadlinemiss manager
 * @param type The file type for the manager that has to be created 
 * @return A reference to the deadlinemiss manager created 
 */
PredDeadlineMissManager* SimulationConfiguration::CreateDeadlineManager(DeadlineMissManager_t type)
{
  PredDeadlineMissManager* ret = NULL;

  switch(type)
  {
    case GRADUAL:
    {
      ret = new PredDeadlineMissManagerGradual();
      break;
    }
    case FACTOR:
    {
      ret = new PredDeadlineMissManagerExp();
      break;
    }
    default:
      assert(false);
  }
  
  return ret;
}

/**
 * Creates the architecture specified in the configuration file
 * @param log The logging engine 
 * @return A reference to the created architecture
 */
ArchitectureConfiguration* SimulationConfiguration::CreateArchitecture(Log* log)
{ 
 
 //the grid backfilling does not have any kind of architecutre .. right now
 if(this->policy == ISIS_DISPATCHER || this->policy == BRANK_POLICY || this->policy == META_BROKERING_SYSTEM)
    return 0;     
  
  return CreateArchitecture(log,this->architecture,this->architectureFile);
}

/**
 * Creates the architecture specified in the parameters 
 * @param log The logging engine 
 * @param architectureReq The architecture tpye 
 * @param architectureFileReq The file path for the architecture definition
 * @return  A reference to the created architecture
 */
ArchitectureConfiguration* SimulationConfiguration::CreateArchitecture(Log* log,architecture_type_t architectureReq,string architectureFileReq)
{ 
  ArchitectureConfiguration*  architectureConfig = NULL;
  switch(architectureReq)
  {
    case MARENOSTRUM:
      printf("Loading the architecture marenostrum model file\n");
      architectureConfig = (ArchitectureConfiguration*) new MNConfiguration(architectureFileReq,log);
      architectureConfig->loadConfiguration();      
      break;
   case LOCAL_SYSTEM:
      printf("Loading the architecture local system model file\n");
      architectureConfig = (ArchitectureConfiguration*) new ArchitectureConfiguration(architectureFileReq,log);
      architectureConfig->loadConfiguration();      
      break;
    default:
      return NULL; //in some policies, like grid backfilling policy there is no architecutre file for the policy 
      break;
  }
  
  return architectureConfig;
}


/**
 * Creates the workload specified in the configuration file
 * @param log The logging engine 
 * @return A reference to the created workload 
 */
TraceFile* SimulationConfiguration::CreateWorkload(Log* log)
{
  if(this->workload == SWFExtCosts)
    return CreateWorkload(log,this->workload,this->workloadPath,this->jobMonetaryRequirementsFile); 
  else
    return CreateWorkload(log,this->workload,this->workloadPath,this->jobRequirementsFile);
}

/**
 * Creates the workload specified in the parameters 
 * @param log The logging engine
 * @param workloadReq The workload type required 
 * @param workloadPathReq The file path for the workload
 * @param jobRequirementsFileReq The job requirements for the workload path (may be null)
 * @return A reference to the created workload 
 */
TraceFile* SimulationConfiguration::CreateWorkload(Log* log,workload_type_t workloadReq,string workloadPathReq,string jobRequirementsFileReq)
{

  TraceFile* workloadRet  = NULL ;  
   
  
  switch(workloadReq)
  {
    case SWF:
      workloadRet = (TraceFile*) new SWFTraceFile(workloadPathReq,log);
      break;
    case SWFExtCosts:
    case SWFExtReqs:
      workloadRet = (TraceFile*) new SWFTraceExtended(workloadPathReq,jobRequirementsFileReq,log);
      std::cout << "The requesting trace is " << workloadPathReq << endl;      
    case SWFGrid:
      workloadRet = (TraceFile*) new SWFGridTrace(workloadPathReq,jobRequirementsFileReq,log);      
      break;
    default:
     assert(0);
  }
  
  if(this->JobsToLoad == 0)
  {
  	std::cout << " ERROR: At least one job has to be loaded " << endl;
	exit(-1);
  }
  
  workloadRet->setJobsToLoad(this->JobsToLoad);
  workloadRet->setLoadFromJob(this->LoadFromJob);
  workloadRet->setLoadTillJob(this->LoadTillJob);
  
  std::cout << "Loading "<< ftos(workloadRet->getJobsToLoad())<<" jobs (-1 means all the jobs )" << endl;
  
  if(workloadRet->loadTrace())
    printf("The requirements trace has been load correctly \n");
  else
    printf("Some errors has occurred when loading the resource trace file .. please check the error log.\n");
 
  if(this->workload == SWFExtReqs)
    if(((SWFTraceExtended*)workloadRet)->LoadRequirements())
      printf("The requirements trace has been load correctly \n");
    else
      printf("Some errors has occurred when loading the trace requirements file .. please check the error log.\n");

  if(this->workload == SWFGrid)
    if(((SWFGridTrace*)workloadRet)->LoadRequirements())
      printf("The requirements grid trace has been load correctly \n");
    else
      printf("Some errors has occurred when loading the grid trace requirements file .. please check the error log.\n");


  else if(this->workload == SWFExtCosts)
    if(((SWFTraceExtended*)workloadRet)->LoadCostRequirements())
      printf("The cost requirements trace has been load correctly \n");
    else
      printf("Some errors has occurred when loading the cost trace requirements file .. please check the error log.\n");  
  
  return workloadRet;
}

/**
 * Creates a FCFS policy based on the global variables 
 * @param architectureConfig The architecture that will be used in the simulation 
 * @param log The logging engine 
 * @param startime The global startime
 * @return A reference to the FCFS policy created 
 */
SchedulingPolicy* SimulationConfiguration::CreateFCFSPolicy(ArchitectureConfiguration* architectureConfig, Log* log, double startime)
{
  printf("Creating a FCFS policy simulator.. \n");
  SchedulingPolicy* policyRet = new FCFSSchedulingPolicy(architectureConfig,log,startime);
  
  return policyRet;
      
}

/**
  * Creates a EASY policy based in the global variables 
 * @param architectureConfig The architecture that will be used in the simulation 
 * @param log The logging engine 
 * @param startime The global startime 
 * @return The reference to the EASY policy 
 */
SchedulingPolicy* SimulationConfiguration::CreateEASYPolicy(ArchitectureConfiguration* architectureConfig, Log* log, double startime)
{
   return CreateEASYPolicy(architectureConfig,log,startime,this->PredictorConfigFile,this->numberOfReservations,this->backfillingVariant,this->AllocateAllJobsInTheRT,SimulateResourceUsage,LessConsumeAlternativeThreshold,LessConsumeThreshold,tryToBackfillAllways, this->powerAwareBackfilling);


}

/**
 * Creates a EASY policy based in the provided parameters 
 * @param architectureConfig The architecture that will be used in the simulation 
 * @param log The logging engine 
 * @param startime The global startime 
 * @param PredictorConfigFileReq The predictor configuration file, in case that is required 
 * @param numberOfReservationsReq The number of reservations to be used 
 * @param variantReq The backfilling variant 
 * @param AllocateAllJobsInTheRT Indicates is the allocations must be done in all the queued jobs 
 * @param SimulateResourceUsage If the resource usage and job collisions have to be simulated 
 * @param LessConsumeAlternativeThreshold Indicates the alternative threshold that can be used in the LessConsumeThreshold, when the basic once is not specified 
 * @param LessConsumeThreshold Indicates the maximum allowed threshold that can be used in the LessConsumeThreshold, if -1 means that the LessConsume has to be used.
 * @param tryToBackfillAllways Indicates if all the jobs have tried to be backfilled 
 * @param powerAware Indicates while this is a power aware variant 
 * @return The reference to the EASY policy
 */
SchedulingPolicy* SimulationConfiguration::CreateEASYPolicy(ArchitectureConfiguration* architectureConfig, Log* log, double startime,string PredictorConfigFileReq, int numberOfReservationsReq,Backfilling_variant_t variantReq, bool AllocateAllJobsInTheRT,bool SimulateResourceUsage,double LessConsumeAlternativeThreshold, double LessConsumeThreshold, bool tryToBackfillAllways, bool powerAwareBackFilling)
{
  SchedulingPolicy* policyRet = NULL;
  
  printf("Creating a EASY policy simulator.. \n");
  
  if(SimulateResourceUsage)
  {
    printf("Simulate Resources enabled\n");
    if(backfillingVariant == RUA_BACKFILLING)
      policyRet = new RUABackfilling(architectureConfig,log,startime,backfillingVariant);
    else if(backfillingVariant == POWER_AWARE_BACKFILLING)
      policyRet = new PowerAwareBackfilling(architectureConfig,log,startime,backfillingVariant);
    else
      policyRet = new EASYWithCollisionSchedulingPolicy(architectureConfig,log,startime,backfillingVariant);
   
    if(LessConsumeThreshold > 0)
    {
      ((EASYWithCollisionSchedulingPolicy*) policyRet)->setThresshold(LessConsumeThreshold);
      
      if(LessConsumeAlternativeThreshold > 0)
         ((EASYWithCollisionSchedulingPolicy*) policyRet)->setAlternativeThreshold(LessConsumeAlternativeThreshold);
    }
  }  
  else if(PredictorConfigFileReq.compare("") != 0 || this->GlobalPredictorConfigFile.compare("") != 0)
  {
    assert(backfillingVariant != RUA_BACKFILLING); /* this policy requires to use the resource usage modelling*/

    printf("%s", ("Creating a EASY policy simulator with prediction and reservations = "+itos(numberOfReservationsReq)+"\n").c_str());
    
    Predictor* predictor;
    PredDeadlineMissManager* pre;
    PredDeadlineMissManager* post;
    PredictorConfigurationFile* configPred;
    
    //we give more priority the local predictor
    if(PredictorConfigFileReq.compare("") != 0 )
    {
      configPred = new PredictorConfigurationFile(PredictorConfigFileReq);
      predictor = CreatePredictor(configPred);        
      pre = CreateDeadlineManager(configPred->preDeadlineManager);
      post = CreateDeadlineManager(configPred->postDeadlineManager); 
    }
    else
    { 
      predictor = GlobalPredictionService;
      pre = GlobalPreDeadlineManager;
      post = GlobalPostDeadlineManager;
        
    }   
        
    policyRet = new EASYPredSchedulingPolicy(architectureConfig,log,startime,variantReq);
    ((EASYPredSchedulingPolicy*)policyRet)->setPredictor(predictor);
    predictor->setPreManager(pre);           
    predictor->setPostManager(post);
        
  }
 else
  {
    printf("%s", ("Creating the EASY policy with no prediction and reservations = "+itos(numberOfReservationsReq)+" \n").c_str());
    policyRet = new EASYSchedulingPolicy(architectureConfig,log,startime,variantReq);        
  }
     
  ((EASYSchedulingPolicy*)policyRet)->setPowerAware(powerAwareBackFilling);
  ((EASYSchedulingPolicy*)policyRet)->setNumberOfReservations(numberOfReservationsReq);
  ((EASYSchedulingPolicy*)policyRet)->setAllocateAllJobs(AllocateAllJobsInTheRT);
  ((EASYSchedulingPolicy*)policyRet)->setTryToBackfillAllways(tryToBackfillAllways);
    
  return policyRet;
}

/**
 * Creates a deep policy based on the global parameters 
 * @param architectureConfig The architecture that will be used in the simulation 
 * @param log The logging engine 
 * @param startime The global startime 
 * @return A reference to the DEEP created policy 
 */
SchedulingPolicy* SimulationConfiguration::CreateDEEPPolicy(ArchitectureConfiguration* architectureConfig, Log* log,double startime)
{
  return CreateDEEPPolicy(architectureConfig,log,startime,this->DSCPmetricType,this->DSCPstatisticType,this->DSCPgenerateAllParaversRTables,this->DSCPparaverOutPutDir,this->DSCPparaverTrace);
}


/**
 * Creates a ISIS Dispactcher policy based on the global variables
 * @param log The logging engine 
 * @param startime The global startime 
 * @return A reference to the isis dispatcher policy 
 */
SchedulingPolicy* SimulationConfiguration::CreateISISDispatcherPolicy(Log* log,double startime)
{    
  ISISDispatcherPolicy* policy = new ISISDispatcherPolicy(log,this->GlobalPredictionService != NULL, this->GlobalPredictionService);
  
  //must be done in this order, due to setting the policies, requires to know previously the centers name 
  policy->setISISDispatcherArchitectures(ISISDispatcherArchitectures);
  policy->setISISDispatcherCentersName(ISISDispatcherCentersName);
  policy->setISISDispatcherPolicies( ISISDispatcherPolicies);
  policy->setISISDispatcherParaverTracesName(ISISDispatcherParaverTracesName);  policy->setISISDispatcherMetric2Optimize(ISISDispatcherMetric2Optimize);
  policy->setISISDispatcherStatistic2Optimize(ISISDispatcherStatistic2Optimize);
  policy->setISISDispatcherEmulateCPUFactor(ISISDispatcherEmulateCPUFactor);
  policy->setAmountOfKnownCenters(amountOfKnownCenters);
  
  policy->setGlobalTime(startime);
       
  //we also set the log and 
  policy->setLog(log);
  
  for(vector<SchedulingPolicy*>::iterator itpol = ISISDispatcherPolicies->begin();
      itpol != ISISDispatcherPolicies->end();
      ++itpol)
  {
    SchedulingPolicy* centerPolicy = *itpol;
    centerPolicy->setLog(log);
    centerPolicy->setGlobalTime(startime);
  }    
  
  return policy;

}

/**
 * Creates a BRANKpolicy based on the global variables
 * @param log The logging engine 
 * @param startime The global startime 
 * @return A reference to the Grid backfilling policy 
 */
SchedulingPolicy* SimulationConfiguration::CreateBRANKPolicy(Log* log,double startime)
{    
  BRANKPolicy* policy = new BRANKPolicy(log,this->GlobalPredictionService != NULL, this->GlobalPredictionService);
  
  //must be done in this order, due to setting the policies, requires to know previously the centers name 
  policy->setBRANKPolicyArchitectures(BRANKPolicyArchitectures);
  policy->setBRANKPolicyCentersName(BRANKPolicyCentersName);
  policy->setBRANKPolicyPolicies( BRANKPolicyPolicies);
  policy->setBRANKPolicyParaverTracesName(BRANKPolicyParaverTracesName);  
  policy->setBRANKPolicyEmulateCPUFactor(BRANKPolicyEmulateCPUFactor);
  
  policy->setGlobalTime(startime);
       
  //we also set the log and 
  policy->setLog(log);
  
  for(vector<SchedulingPolicy*>::iterator itpol = BRANKPolicyPolicies->begin();
      itpol != BRANKPolicyPolicies->end();
      ++itpol)
  {
    SchedulingPolicy* centerPolicy = *itpol;
    centerPolicy->setLog(log);
    centerPolicy->setGlobalTime(startime);
  }    
  
  return policy;

}

/**
 * Creates a BRANKpolicy based on the global variables
 * @param log The logging engine 
 * @param startime The global startime 
 * @return A reference to the Grid backfilling policy 
 */
SchedulingPolicy* SimulationConfiguration::CreateMetaBrokeringSystem(Log* log,double startime)
{
  SchedulingPolicy* policy = new MetaBrokeringSystem(log,this->GlobalPredictionService != NULL, this->GlobalPredictionService);
  ((MetaBrokeringSystem*)policy)->setVirtualOrganitzations(MBVirtualOrganitzations);
  ((MetaBrokeringSystem*)policy)->setVONames(VONames);

  return policy;
}

/**
 * Creates a DeepSearch based on the global variables
 * @param log The logging engine 
 * @param startime The global startime 
 * @return A reference to the DeepSearch policy 
 */
SchedulingPolicy* SimulationConfiguration::CreateDEEPPolicy(ArchitectureConfiguration* architectureConfig, Log* log,double startime,metric_t DSCPmetricTypeReq,statistic_t DSCPstatisticTypeReq,bool DSCPgenerateAllParaversRTablesReq,string paraverOutPutDirReq,string paraverTraceReq)
{
  SchedulingPolicy* policyRet  = NULL;
   
  printf("Creating a DEEP SEARCH policy simulator.. \n");
  DeepSearchCollisionPolicy * policyDeep = new DeepSearchCollisionPolicy(architectureConfig,log,startime,DSCPmetricTypeReq,DSCPstatisticTypeReq);      
      
  if(DSCPgenerateAllParaversRTablesReq)
  {
    policyDeep->setGenerateAllParaversRTables(true);
    policyDeep->setParaverOutPutDir(paraverOutPutDirReq);
    policyDeep->setParaverTrace(paraverTraceReq);          
  }
      
  policyRet = (SchedulingPolicy*) policyDeep;
  
  return policyRet;

}


/**
 * Creates the policy based on the global parameters 
 * @param architectureConfig The architecture that will be used in the simulation 
 * @param log The logging engine 
 * @param startime The global startime 
 * @return 
 */
SchedulingPolicy* SimulationConfiguration::CreatePolicy(ArchitectureConfiguration* architectureConfig, Log* log, double startime)
{
  
  SchedulingPolicy* policyRet  = NULL;

  switch(this->policy)
  {
    case FCFS: 
      policyRet = CreateFCFSPolicy(architectureConfig,log,startime) ;
      break;
    case EASY:    
      policyRet = CreateEASYPolicy(architectureConfig,log,startime);
      break;
    case DEEP:
      policyRet = CreateDEEPPolicy(architectureConfig,log,startime);
      break;      
    case ISIS_DISPATCHER:
      policyRet = CreateISISDispatcherPolicy(log,startime);
      break;
    case BRANK_POLICY:
      policyRet = CreateBRANKPolicy(log,startime);
      break;
    case META_BROKERING_SYSTEM:
      policyRet = CreateMetaBrokeringSystem(log,startime);
      break;
    default:
      assert(false);
      break;
  }
  
  //here we set all the common fields for the policy that are already known by the simulator 
  policyRet->setEmulateCPUFactor(this->EmulateCPUFactor);
  policyRet->setRS_policyUsed(this->ResourceSelectionPolicy);
  policyRet->setMaxAllowedRuntime(this->maxAllowedRuntime);
  policyRet->setCostAllocationUnit(this->CostAllocationUnit);
  policyRet->setAllocationUnitHour(this->AllocationUnitHour);  


  // take account compute Enery, for all scheduling jobs disciplines 
  if (this->computeEnergy)  
  {
    // architecture config file has a freq and voltage description ?
    assert(architectureConfig->Frequencies.size() > 0 && architectureConfig->Frequencies.size()==architectureConfig->Voltages.size());

    PowerConsumption * powerinf = new PowerConsumption(architectureConfig->Activity, architectureConfig->Portion);
    powerinf->setGears(architectureConfig->Frequencies, architectureConfig->Voltages);
    policyRet->initializationPower(powerinf);    
  }
 
   
  return policyRet;
}


/**
 * Creates the cvs job dumper class if it was indicated in the configuration file 
 * @return A reference to the class 
 */
CSVJobsInfoConverter* SimulationConfiguration::createJobCSVDumper()
{
  CSVJobsInfoConverter* JobCSVDumper = NULL;
  //here we also create the cvs dump file for the jobs in case it was specified   
  if(this->JobsSimPerformanceCSVFile.compare("") != 0)
  {
    JobCSVDumper = new CSVJobsInfoConverter(this->JobsSimPerformanceCSVFile,log);      
  }
  
  return JobCSVDumper;
}

/**
 * Creates the cvs job dumper class if it was indicated in the configuration file 
 * @return A reference to the class 
 */
CSVPolicyInfoConverter* SimulationConfiguration::createPolicyCSVDumper()
{
  CSVPolicyInfoConverter* PolicyCSVDumper = NULL;
  //here we also create the cvs dump file for the jobs in case it was specified   
  if(this->JobsSimPerformanceCSVFile.compare("") != 0)
  {
    PolicyCSVDumper = new CSVPolicyInfoConverter(this->PolicySimPerformanceCSVFile,log);      
  }
  
  return PolicyCSVDumper;
}

/**
 * Given a set of strings containing the name for the job attributes the function will return the native type associated to each of the types 
 * @param AttributeTypes The vector containing the string with the attributes names 
 * @return A reference to the vector containing the vector types for the attributes 
 */
vector<native_t>* SimulationConfiguration::GetAttributeTypes(vector<string>* AttributeTypes)
{
  vector<native_t>* types = new vector<native_t>();
  
  for(vector<string>::iterator it = AttributeTypes->begin(); it != AttributeTypes->end();++it)
  {
    string att = *it;
    map<string,native_t>::iterator itAtType = this->AttributeJobType.find(att);    
    assert(itAtType != this->AttributeJobType.end());
    types->push_back(itAtType->second);  
  }
  
  return types;
}

/**
 * Function that will create all the CSV  R analyzers that analyze the CSVJobs files 
 * @param csvfile The csv file that have to be analyzed 
 * @return The set of analyzers for the CSVJobs files 
 */
vector<RScript*>* SimulationConfiguration::CreateJobCSVRAnalyzers(CSVJobsInfoConverter* csvfile,ArchitectureConfiguration* arch)
{
   vector<RScript*> * analyzers = new vector<RScript*>();
   
   for(vector<r_scripts_type>::iterator it = this->RAnalyzers.begin(); it != this->RAnalyzers.end();++it)
   {
     r_scripts_type type = *it;
     
     if(type == JOB_CSV_R_ANALYZER_BASIC)
     {
       map<r_scripts_type,string>::iterator has = this->simulatorConfiguration->RScripts.find(type);
       
       assert(has != this->simulatorConfiguration->RScripts.end());
       
       string scriptPath = has->second;
          
       JobCSVRScriptAnaliser* analyzer = new JobCSVRScriptAnaliser(csvfile->getFilePath(),scriptPath, this->outPutDir , arch ,this->log,this->simulatorConfiguration->TemporaryDirectory,this->simulatorConfiguration->RBinaryPath,this->simulatorConfiguration->RScriptsBaseDir);
       
       analyzers->push_back((RScript*)analyzer);
     }
   }
   
   return analyzers;

}

}
