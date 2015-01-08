#include <utils/utilities.h>
#include <utils/configurationfile.h>

#include<sstream>

namespace Utils {

/**
 * The default constructor for the class
 * @param ConfigFile The configuration file that will be loaded
 */
ConfigurationFile::ConfigurationFile(string ConfigFile)
{
  try
  {
   XALAN_USING_XERCES(XMLPlatformUtils)
  
   XALAN_USING_XALAN(XPathEvaluator)
  
  
   XMLPlatformUtils::Initialize(); 
  
   XPathEvaluator::initialize();
  
   {
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
    
    std::cout << "Loading the configuration file "<< ConfigFile << endl;
  
    // Create an input source that represents a local file...
    const LocalFileInputSource theInputSource(theFileName.c_str()); 
    
    // Parse the document...
    XalanDocument* const theDocument =
      theLiaison.parseXMLStream(theInputSource);
    assert(theDocument != 0);
     
     XalanDocumentPrefixResolver  thePrefixResolver(theDocument); 
   /*
     Now that the document is loaded we go through all the elements of the XML document
   */
  
    XPathEvaluator theEvaluator;
  
    // OK, let's find the context node... the BladeCenters
    XalanNode* const theContextNode =
      theEvaluator.selectSingleNode(
        theDOMSupport,
       theDocument,
       XalanDOMString("Configuration").c_str(),
       thePrefixResolver);         
       
   if (theContextNode == 0)
   {
       printf("The simulator configuration file has important errors, the field Configuration does not exists");
       return;
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
         
     if(theErrorFilePathResult.null())
     {
       printf("The filed ErrorFilePath is not present in the configuration file ! ");          
       ErrorFilePath = "NOConfigERRORFILE.log";
     }
     else
     {
       stringstream stErrorFilePath;
       stErrorFilePath << theErrorFilePathResult->str();
       ErrorFilePath = stErrorFilePath.str();  
     } 
      
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
       printf("The filed DebugFilePath is not present in the configuration file ! ");
       ErrorFilePath = "NOConfigDEBUGFILE.log";
     }
     else   
     {   
       stringstream stDebugFilePath;
       stDebugFilePath << theDebugFilePathResult->str();
       DebugFilePath = stDebugFilePath.str();  
     }   
    }
   //************************We get the DefaultWorkloadPath *****************************/
    if(containsXPath(string("DefaultWorkloadPath"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
    {
      const XObjectPtr theDefaultWorkloadPathResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("DefaultWorkloadPath").c_str(),
         thePrefixResolver));
         
     if(theDefaultWorkloadPathResult.null())
     {
       printf("The filed DefaultWorkloadPath is not present in the configuration file ! ");
       DefaultWorkloadPath = "";
     }
     else    
     { 
       stringstream stDefaultWorkloadPath;
       stDefaultWorkloadPath << theDefaultWorkloadPathResult->str();
       DefaultWorkloadPath = stDefaultWorkloadPath.str();  
     }
   }
   //************************We get the DefaultArchitectureConfigurationFile *****************************/      
    if(containsXPath(string("DefaultArchitectureConfigurationFile"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
    {
      const XObjectPtr theDefaultArchitectureConfigurationFileResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("DefaultArchitectureConfigurationFile").c_str(),
         thePrefixResolver));
         
     if(theDefaultArchitectureConfigurationFileResult.null())
     {
       printf("The filed ArchitectureConfigurationFile is not present in the configuration file ! ");
       DefaultArchitectureConfigurationFile = "";
     }
     else  
     {   
       stringstream stDefaultArchitectureConfigurationFile;
       stDefaultArchitectureConfigurationFile << theDefaultArchitectureConfigurationFileResult->str();
       DefaultArchitectureConfigurationFile = stDefaultArchitectureConfigurationFile.str();  
     }
   }
   //************************We get the DefaultPolicy *****************************/      
    if(containsXPath(string("DefaultPolicy"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
    {   
      const XObjectPtr theDefaultPolicyResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("DefaultPolicy").c_str(),
         thePrefixResolver));
         
     if(theDefaultPolicyResult.null())
     {
       printf("The filed DefaultPolicy is not present in the configuration file ! ");
       DefaultPolicy = "";
     }
     else  
     {   
       stringstream stDefaultPolicy;
       stDefaultPolicy << theDefaultPolicyResult->str();
       DefaultPolicy = stDefaultPolicy.str();  
     }
   }
   //************************We get the DefaultParaverOutputDir *****************************/      
    if(containsXPath(string("DefaultParaverOutputDir"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
    {      
      const XObjectPtr theDefaultParaverOutputDirResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("DefaultParaverOutputDir").c_str(),
         thePrefixResolver));
         
     if(theDefaultParaverOutputDirResult.null())
     {
       printf("The filed DefaultParaverOutputDir is not present in the configuration file ! ");
       DefaultParaverOutputDir = "";
     }
     else  
     {   
       stringstream stDefaultParaverOutputDir;
       stDefaultParaverOutputDir << theDefaultParaverOutputDirResult->str();
       DefaultParaverOutputDir = stDefaultParaverOutputDir.str();  
     }
    }
   //************************We get the DefaultParaverTraceName *****************************/      
    if(containsXPath(string("DefaultParaverTraceName"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
    {   
      const XObjectPtr theDefaultParaverTraceNameResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("DefaultParaverTraceName").c_str(),
         thePrefixResolver));
         
     if(theDefaultParaverTraceNameResult.null())
     {
       printf("The filed DefaultParaverTraceName is not present in the configuration file ! ");
       DefaultParaverTraceName = "";
     }
     else   
     {   
       stringstream stDefaultParaverTraceName;
       stDefaultParaverTraceName << theDefaultParaverTraceNameResult->str();
       DefaultParaverTraceName = stDefaultParaverTraceName.str();       
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
         
     string DebugLevels;
   
     if(theDebugLevelResult.null())
     {
       printf("The filed DebugLevel is not present in the configuration file ! ");
       DebugLevels = "0";
     }
     else
     {   
       stringstream stDebugLevel;
       stDebugLevel << theDebugLevelResult->str();
       DebugLevels = stDebugLevel.str();  
     }
     
     DebugLevel = atoi(DebugLevels.c_str());
     
    }   

   //************************We get the LessConsumeAlternativeThreshold *****************************/      
    if(containsXPath(string("LessConsumeAlternativeThreshold"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
    {      
      const XObjectPtr theLessConsumeAlternativeThresholdResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("LessConsumeAlternativeThreshold").c_str(),
         thePrefixResolver));
         
     string LessConsumeAlternativeThresholds;
   
     if(theLessConsumeAlternativeThresholdResult.null())
     {
       printf("The filed LessConsumeAlternativeThreshold is not present in the configuration file ! ");
       LessConsumeAlternativeThresholds = "0";
     }
     else
     {   
       stringstream stLessConsumeAlternativeThreshold;
       stLessConsumeAlternativeThreshold << theLessConsumeAlternativeThresholdResult->str();
       LessConsumeAlternativeThresholds = stLessConsumeAlternativeThreshold.str();  
     }
     
     LessConsumeAlternativeThreshold = atof(LessConsumeAlternativeThresholds.c_str());
     
    }
      //************************We get the DefaultOutPutDir *****************************/      
    if(containsXPath(string("DefaultOutPutDir"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
    {   
      const XObjectPtr theDefaultOutPutDirResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("DefaultOutPutDir").c_str(),
         thePrefixResolver));
         
     if(theDefaultOutPutDirResult.null())
     {
       printf("The filed DefaultOutPutDir is not present in the configuration file ! ");
       DefaultOutPutDir = "";
     }
     else
     {   
       stringstream stDefaultOutPutDir;
       stDefaultOutPutDir << theDefaultOutPutDirResult->str();
       DefaultOutPutDir = stDefaultOutPutDir.str();  
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
       printf("The filed jobRequirementsFile is not present in the configuration file ! ");
       jobRequirementsFile = "";
     }
     else
     {   
        stringstream stjobRequirementsFile;
        stjobRequirementsFile << thejobRequirementsFileResult->str();
        jobRequirementsFile = stjobRequirementsFile.str();  
      }
    }
      //************************We get the JavaBinaryPath *****************************/      
    if(containsXPath(string("JavaBinaryPath"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
    {
      const XObjectPtr theJavaBinaryPathResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("JavaBinaryPath").c_str(),
         thePrefixResolver));
         
     if(theJavaBinaryPathResult.null())
     {
       printf("The filed JavaBinaryPath is not present in the configuration file ! ");
       JavaBinaryPath = "";
     } 
     else
     {   
       stringstream stJavaBinaryPath;
       stJavaBinaryPath << theJavaBinaryPathResult->str();
       JavaBinaryPath = stJavaBinaryPath.str();  
     }
    }  
  //************************We get the JavaClassPath *****************************/      
   if(containsXPath(string("JavaClassPath"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
   {   
      const XObjectPtr theJavaClassPathResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("JavaClassPath").c_str(),
         thePrefixResolver));
         
     if(theJavaClassPathResult.null())
     {
       printf("The filed JavaClassPath is not present in the configuration file ! ");
       JavaClassPath = "";
     }
     else
     {   
       stringstream stJavaClassPath;
       stJavaClassPath << theJavaClassPathResult->str();
       JavaClassPath = stJavaClassPath.str();  
     }      
   }
//************************We get the WekaJarPath *****************************/      
   if(containsXPath(string("WekaJarPath"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
   {    
      const XObjectPtr theWekaJarPathResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("WekaJarPath").c_str(),
         thePrefixResolver));
         
     if(theWekaJarPathResult.null())
     {
       printf("The filed WekaJarPath is not present in the configuration file ! ");
       WekaJarPath = "";
     }
     else
     {   
       stringstream stWekaJarPath;
       stWekaJarPath << theWekaJarPathResult->str();
       WekaJarPath = stWekaJarPath.str();  
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
         printf("The filed StatisticsConfigFile is not present in the configuration file ! ");
         architectureFile = "";
       }
       else  
       {   
         stringstream stStatisticsConfigFile;
         stStatisticsConfigFile << theStatisticsConfigFileResult->str();
         statisticsConfigFile = stStatisticsConfigFile.str();  
       }
    }   
   
   //************************We get the RBinaryPath *****************************/      
   if(containsXPath(string("RBinaryPath"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
   {       
      const XObjectPtr theRBinaryPathResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("RBinaryPath").c_str(),
         thePrefixResolver));
         
       if(theRBinaryPathResult.null())
       {
         printf("The filed RBinaryPath is not present in the configuration file ! ");
         architectureFile = "";
       }
       else  
       {   
         stringstream stRBinaryPath;
         stRBinaryPath << theRBinaryPathResult->str();
         RBinaryPath = stRBinaryPath.str();  
       }
    }   
    
   //************************We get the RScriptsBaseDir *****************************/      
   if(containsXPath(string("RScriptsBaseDir"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
   {       
      const XObjectPtr theRScriptsBaseDirResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("RScriptsBaseDir").c_str(),
         thePrefixResolver));
         
       if(theRScriptsBaseDirResult.null())
       {
         printf("The filed RScriptsBaseDir is not present in the configuration file ! ");
         architectureFile = "";
       }
       else  
       {   
         stringstream stRScriptsBaseDir;
         stRScriptsBaseDir << theRScriptsBaseDirResult->str();
         RScriptsBaseDir = stRScriptsBaseDir.str();  
       }
    }    
        
   //************************We get the TemporaryDirectory *****************************/      
   if(containsXPath(string("TemporaryDirectory"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
   {       
      const XObjectPtr theTemporaryDirectoryResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("TemporaryDirectory").c_str(),
         thePrefixResolver));
         
       if(theTemporaryDirectoryResult.null())
       {
         printf("The filed TemporaryDirectory is not present in the configuration file ! ");
         architectureFile = "";
       }
       else  
       {   
         stringstream stTemporaryDirectory;
         stTemporaryDirectory << theTemporaryDirectoryResult->str();
         TemporaryDirectory = stTemporaryDirectory.str();  
       }
    }     
    
   //************************We get the JobsToSimulate *****************************/      
    if(containsXPath(string("JobsToSimulate"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
    {      
      const XObjectPtr theJobsToSimulateResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("JobsToSimulate").c_str(),
         thePrefixResolver));
         
     string JobsToSimulates;
   
     if(theJobsToSimulateResult.null())
     {
       printf("The filed JobsToSimulate is not present in the configuration file ! ");
       JobsToSimulates = "-1";
     }
     else
     {   
       stringstream stJobsToSimulate;
       stJobsToSimulate << theJobsToSimulateResult->str();
       JobsToSimulates = stJobsToSimulate.str();  
     }
     
     JobsToSimulate = atoi(JobsToSimulates.c_str());
     
    }   

    /* We get the R scripts availables in the simulator and that can be used for analize the CVS files */
    if(containsXPath(string("RPostProcessingFiles"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator)) 
    {            
        NodeRefList estimatorsNodes; 
      
        estimatorsNodes = theEvaluator.selectNodeList (estimatorsNodes, theDOMSupport, theContextNode,  
                                             XalanDOMString("RPostProcessingFiles/Script").c_str(),
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
           
           string ScriptPath = "";

           if(containsXPath(string("ScriptPath"),theDOMSupport,estimatorNode,thePrefixResolver,theEvaluator)) 
           {    
             /* we get the CVS File Type to whom the file R file will be applied */
             
     
             const XObjectPtr theScriptPathResult(
               theEvaluator.evaluate(
               theDOMSupport,
               estimatorNode,
               XalanDOMString("ScriptPath").c_str(),
               thePrefixResolver));
         
             if(theScriptPathResult.null())
             { 
               printf("The ScriptPath is not present in the configuration file for one of the metrics ! ");
               ScriptPath = "";
             }
             else    
             { 
               stringstream stScriptPath;
               stScriptPath << theScriptPathResult->str();
               ScriptPath = stScriptPath.str();         
             }
           }
           
           if(ScriptPath.compare("") == 0 || AnalyzerType.compare("") ==  0)
           {
               continue;
               std::cout << "The configuration file has a wrong pair value of script path and csv file type";
           }
                      
           this->RScripts.insert(pair<r_scripts_type,string>(this->getAnalyzerType(AnalyzerType),ScriptPath));
        }
      }
      
   }
  } 
 catch(...)
 {
   
   std::cout << "Undefined error the document has some errors. Please check the architecture configuration file." << endl;
   std::cout << "Please check that the configuration file exists.." << endl;
  
 }
 
 /* setting default values for the others ..*/

  this->policy = FCFS;
  this->workload = SWF;
  this->workloadPath = this->DefaultWorkloadPath;
  this->paraverOutPutDir = this->DefaultParaverOutputDir;
  this->paraverTrace = this->DefaultParaverTraceName;
  this->architectureFile = this->DefaultArchitectureConfigurationFile;
  this->architecture = MARENOSTRUM;
}

/**
 * The default destructor for the class 
 */

ConfigurationFile::~ConfigurationFile()
{
}

/**
 * Imports the configuration from another configuration file 
 * @param ConfigFileImp The source configuration file 
 */
void ConfigurationFile::import(ConfigurationFile* ConfigFileImp)
{
  
  ConfigFile = ConfigFileImp->ConfigFile;
  DefaultWorkloadPath = ConfigFileImp->DefaultWorkloadPath; 
  DefaultArchitectureConfigurationFile = ConfigFileImp->DefaultArchitectureConfigurationFile;
  DefaultPolicy = ConfigFileImp->DefaultPolicy;
  DefaultParaverOutputDir = ConfigFileImp->DefaultParaverOutputDir;
  DefaultParaverTraceName = ConfigFileImp->DefaultParaverTraceName;
  ErrorFilePath = ConfigFileImp->ErrorFilePath;
  DebugFilePath = ConfigFileImp->DebugFilePath;
  DebugLevel = ConfigFileImp->DebugLevel;
  DefaultOutPutDir = ConfigFileImp->DefaultOutPutDir;
  policy = ConfigFileImp->policy;
  workload = ConfigFileImp->workload;
  workloadPath = ConfigFileImp->workloadPath;
  paraverOutPutDir = ConfigFileImp->paraverOutPutDir;
  paraverTrace  = ConfigFileImp->paraverTrace;
  jobRequirementsFile = ConfigFileImp->jobRequirementsFile;
}


/**
 * Given a r_analyzer returns the analyzer file type indicated in the string 
 * @param r_analyzer The string containing the analizer type
 * @return The r_analyzer indicating the analyzer type 
 */
r_scripts_type ConfigurationFile::getAnalyzerType(string r_analyzer)
{
    if(r_analyzer.compare("JOB_CSV_R_ANALYZER_BASIC") == 0)
      return JOB_CSV_R_ANALYZER_BASIC;
    else 
      return DEFAULT_CSV_R_ANALYER;
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
int ConfigurationFile::containsXPath(string XPath,DOMSupport &theDOMSupport, XalanNode *theContextNode, const PrefixResolver &thePrefixResolver, XPathEvaluator &theEvaluator)
{
      NodeRefList centerNodes; 
      
      centerNodes = theEvaluator.selectNodeList (centerNodes, theDOMSupport, theContextNode,  
                                             XalanDOMString(XPath.c_str()).c_str(),
                                             thePrefixResolver);
     
     //we simply iterate over all the metrics an create them for the job
     return centerNodes.getLength();

}

}
