#include <statistics/statisticsconfigurationfile.h>

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


namespace Utils {

/**
 * The constructor for the class
 * @param StatisticsConfigurationFilePath The statistic configuration file path
 * @param log The logging engine
 */
StatisticsConfigurationFile::StatisticsConfigurationFile(string StatisticsConfigurationFilePath,Log* log)
{
  this->StatisticsConfigurationFilePath = StatisticsConfigurationFilePath;
  
  assert(StatisticsConfigurationFilePath.compare("") != 0);
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
    
    const XalanDOMString theFileName(this->StatisticsConfigurationFilePath.c_str());
    
    log->debug("Loading the simulation configuration file "+StatisticsConfigurationFilePath);
  
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
  
    // OK, let's find the context node... the StatisticsSimulationConfiguration/MetricsToCollect
    XalanNode*  theContextNode =
      theEvaluator.selectSingleNode(
        theDOMSupport,
       theDocument,
       XalanDOMString("StatisticsSimulationConfiguration/MetricsToCollect").c_str(),
       thePrefixResolver);         
       
    if (theContextNode == 0)
    {
        log->error("There are no MetricsToCollect for the simulation");        
    }
    else
    {
     //************************We get all the metrics  *****************************/
     if(containsXPath(string("JobsMetrics/Variable"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator)) 
     {  
      NodeRefList metricsNodes; 
      
      metricsNodes = theEvaluator.selectNodeList (metricsNodes, theDOMSupport, theContextNode,  
                                             XalanDOMString("JobsMetrics/Variable").c_str(),
                                             thePrefixResolver);
     
       //we simply iterate over all the metrics an create them for the job
       for(int i = 0; i < metricsNodes.getLength();i++)
       {
         XalanNode* metricNode = metricsNodes.item(i);      
        
         /* we do the same stuff for the Type */
         string Type;
     
          
        if(containsXPath(string("Name"),theDOMSupport,metricNode,thePrefixResolver,theEvaluator)) 
        {
          const XObjectPtr theTypeResult(
             theEvaluator.evaluate(
             theDOMSupport,
             metricNode,
             XalanDOMString("Name").c_str(),
             thePrefixResolver));
         
           if(theTypeResult.null())
           {
               log->error("The field type is not present in the configuration file for one of the metrics ! ");
               Type = "";
           }
           else    
           { 
             stringstream stType;
             stType << theTypeResult->str();
             Type = stType.str();                  
          }
        }
                      
      
        if(containsXPath(string("Statistic"),theDOMSupport,metricNode,thePrefixResolver,theEvaluator)) 
        {      
          NodeRefList estimatorsNodes; 
          estimatorsNodes = theEvaluator.selectNodeList (estimatorsNodes, theDOMSupport, metricNode,  
                                             XalanDOMString("Statistic").c_str(),
                                             thePrefixResolver);

           //for this variable we will gather all statistical estimators required
           for(int i = 0; i < estimatorsNodes.getLength();i++)
           {
             XalanNode* estimatorNode = estimatorsNodes.item(i);      

             /* we get the statistic used */
             string statisticType;
           
             if(containsXPath(string("Type"),theDOMSupport,estimatorNode,thePrefixResolver,theEvaluator)) 
             {      
               const XObjectPtr thestatisticTypeResult(
                 theEvaluator.evaluate(
                 theDOMSupport,
                 estimatorNode,
                 XalanDOMString("Type").c_str(),
                 thePrefixResolver));
         
               if(thestatisticTypeResult.null())
               { 
                 log->error("The field statistic is not present in the configuration file for one of the metrics ! ");
                 statisticType = "";
               }
               else    
               { 
                 stringstream ststatisticType;
                 ststatisticType << thestatisticTypeResult->str();
                 statisticType = ststatisticType.str();         
               }

               log->debug("Loading metric "+ Type+" with statistic " + statisticType);
             //Now we have to add the metric to the corresponding entity type
             this->addJobMetric(Type,statisticType);
           }         
         }
       }
     }
    }
    if(containsXPath(string("PolicyMetrics/Variable"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator)) 
    {  
      /* Now we get the policy metrics */    
      NodeRefList metricsNodes; 
        
      metricsNodes = theEvaluator.selectNodeList (metricsNodes, theDOMSupport, theContextNode,  
                                             XalanDOMString("PolicyMetrics/Variable").c_str(),
                                             thePrefixResolver);
     
     //we simply iterate over all the metrics an create them for the job
     for(int i = 0; i < metricsNodes.getLength();i++)
     {
       XalanNode* metricNode = metricsNodes.item(i);      
       string Type;
       /* we do the same stuff for the Type */
       if(containsXPath(string("Name"),theDOMSupport,metricNode,thePrefixResolver,theEvaluator)) 
       { 
         
     
         const XObjectPtr theTypeResult(
             theEvaluator.evaluate(
             theDOMSupport,
             metricNode,
             XalanDOMString("Name").c_str(),
             thePrefixResolver));
         
         if(theTypeResult.null())
         {
           printf("The field type is not present in the configuration file for one of the metrics ! ");
           Type = "";
         }
         else    
         { 
           stringstream stType;
           stType << theTypeResult->str();
           Type = stType.str();                  
         }
      }
      
      if(containsXPath(string("Statistic"),theDOMSupport,metricNode,thePrefixResolver,theEvaluator)) 
      {            
        NodeRefList estimatorsNodes; 
      
        estimatorsNodes = theEvaluator.selectNodeList (estimatorsNodes, theDOMSupport, metricNode,  
                                             XalanDOMString("Statistic").c_str(),
                                             thePrefixResolver);

         //for this variable we will gather all statistical estimators required
         for(int i = 0; i < estimatorsNodes.getLength();i++)
         {
           XalanNode* estimatorNode = estimatorsNodes.item(i);      

           if(containsXPath(string("Type"),theDOMSupport,estimatorNode,thePrefixResolver,theEvaluator)) 
           {    
             /* we get the statistic used */
             string statisticType;
     
             const XObjectPtr thestatisticTypeResult(
               theEvaluator.evaluate(
               theDOMSupport,
               estimatorNode,
               XalanDOMString("Type").c_str(),
               thePrefixResolver));
         
             if(thestatisticTypeResult.null())
             { 
               printf("The field statistic is not present in the configuration file for one of the metrics ! ");
               statisticType = "";
             }
             else    
             { 
               stringstream ststatisticType;
               ststatisticType << thestatisticTypeResult->str();
               statisticType = ststatisticType.str();         
             }

             log->debug("Loading metric " + Type + " with statistic "+ statisticType);
             //Now we have to add the metric to the corresponding entity type
             this->addPolicyMetric(Type,statisticType);
           }
         }         
       }
      }      
     }
    }
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
StatisticsConfigurationFile::~StatisticsConfigurationFile()
{
}

/**
 * Adds a new job metric to the definition 
 * @param variable The string containing the type of variable 
 * @param estimator The string containing the statistic to use
 */
void StatisticsConfigurationFile::addJobMetric(string variable,string estimator)
{
   this->addMetric(variable,estimator,&this->jobMetrics);
}


/**
 * Returns the policy metrics 
 * @return A reference to the policy metrics
 */
vector< Metric*>* StatisticsConfigurationFile::getPolicyMetrics()
{
   return &this->policyMetrics;
}


/**
 * Adds a new job metric to the definition 
 * @param variable The string containing the type of variable 
 * @param estimator The string containing the statistic to use
 */
void StatisticsConfigurationFile::addPolicyMetric(string variable,string estimator)
{
   this->addMetric(variable,estimator,&this->policyMetrics); 
}


/**
 * Adds a new metric to the definition 
 * @param variable The string containing the type of variable 
 * @param estimator The string containing the statistic to use 
 * @param metrics The vector where the metric has to be added
 */
void StatisticsConfigurationFile::addMetric(string variable,string estimator,vector<Metric*>* metrics)
{
   metric_t type = this->getMetricType(variable);
   statistic_t statisticUsed = this->getStatisticType(estimator);   
   native_t nativeType = this->getNativeType(variable);
   
   if(statisticUsed == OTHER_STAT)
   {
     std::cout << "The statistical estimator " << estimator << " specified in the var " << variable << " does not exists in the simulator, please check the file." << std::endl;
     return;
     
   }
   
   Metric* newMetric = new Metric(type,statisticUsed,variable,nativeType);
   newMetric->setEstimatorName(estimator);
   metrics->push_back(newMetric);

}


/**
 * Returns the statistic type given its string format 
 * @param estimator Thes statistic type in string format 
 * @return A statistic_t containing the type of the estimator 
 */
statistic_t StatisticsConfigurationFile::getStatisticType(string estimator)
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
   else if(estimator == "PERCENTILES")
   {     
     return PERCENTILES;     
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
   else if(estimator == "COUNT_LITERALS")
   {     
     return COUNT_LITERALS;          
   }          
   else if(estimator == "ACCUMULATE_LITERALS")
   {     
     return ACCUMULATE_LITERALS;     
   }    
   else
    return OTHER_STAT;  

}

/**
 * Returns the native type given the metric string format 
 * @param variable Thes metric type in string format 
 * @return A native_t containing the native type of the metric 
 */
native_t StatisticsConfigurationFile::getNativeType(string variable)
{   if(variable == "SLD")
   {
     return DOUBLE;     
   }
   else if(variable == "BSLD")
   {     
     return DOUBLE;     
   }
   else if(variable == "BSLD_PRT")
   {     
     return DOUBLE;     
   }      
   else if(variable == "WAITTIME")
   {     
     return DOUBLE;     
   }  
   else if(variable == "BACKFILLED_JOB")
   {     
     return DOUBLE;     
   }     
   else if(variable == "kILLED_JOB")
   {     
     return DOUBLE;     
   }   
   else if(variable == "BACKFILLED_JOBS")
   {     
     return DOUBLE;     
   }        
   else if(variable == "JOBS_IN_WQUEUE")
   {     
     return DOUBLE;     
   }        
   else if(variable == "CPUS_USED")
   {     
     return DOUBLE;     
   }      
   else if(variable == "LEAST_WORK_LEFT")
   {     
     return DOUBLE;     
   }   
   else if(variable == "SUBMITED_JOBS")
   {     
     return DOUBLE;     
   }      
   else if(variable == "SUBMITED_JOBS_CENTER")
   {     
     return STRING;     
   }   
   else if(variable == "RSP_USED")
   {     
     return STRING;     
   }
   else if(variable == "COMPUTATIONAL_COST_RESPECTED")
   {     
     return STRING;     
   }  
   else if(variable == "PENALIZED_RUNTIME")
   {     
     return DOUBLE;     
   }       
   else if(variable == "PERCENTAGE_PENALIZED_RUNTIME")
   {     
     return DOUBLE;     
   }            
   else if(variable == "LOAD_JOBS_CENTERS")
   {     
     return COMPOSED_DOUBLE;     
   }       
   else if(variable == "COMPUTATIONAL_COST")
   {     
     return DOUBLE;     
   }  
   else if(variable == "ESTIMATED_COMPUTATIONAL_COST")
   {     
     return DOUBLE;     
   }  
   else if(variable == "PREDICTED_WT")
   {     
     return DOUBLE;     
   }  
   else if(variable == "PREDICTED_RUNTIME")
   {     
     return DOUBLE;
   }
   else if(variable == "SLD_PREDICTED")
   {     
     return DOUBLE;
   }
   else if(variable == "ESTIMATED_COMPUTATIONAL_COST_ERROR")
   {     
     return DOUBLE;     
   }  
   else if(variable == "PREDICTED_WT_ERROR")
   {     
     return DOUBLE;     
   }  
   else if(variable == "PREDICTED_RUNTIME_ERROR")
   {     
     return DOUBLE;
   }
   else if(variable == "SLD_PREDICTED_ERROR")
   {     
     return DOUBLE;
   }
   else if(variable == "ESTIMATED_START_TIME_FIRST_FIT")
   {
     return DOUBLE;
   }
   else
    return OTHER_NATIVE_T;

}


/**
 * Returns the metric type given its string format 
 * @param estimator Thes metric type in string format 
 * @return A metric_t containing the type of the metric 
 */
metric_t StatisticsConfigurationFile::getMetricType(string variable)
{
   if(variable == "SLD")
   {
     return SLD;     
   }
   else if(variable == "BSLD")
   {     
     return BSLD;     
   }   
   else if(variable == "BSLD_PRT")
   {     
     return BSLD_PRT;     
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
 * The job metrics defined in the configuration file 
 * @return A reference to the vector containing the list of jobs 
 */
vector< Metric * >* StatisticsConfigurationFile::getjobMetrics() 
{
  return &jobMetrics;
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
 * @return The number of nodes that matches the query
 */
int StatisticsConfigurationFile::containsXPath(string XPath,DOMSupport &theDOMSupport, XalanNode *theContextNode, const PrefixResolver &thePrefixResolver, XPathEvaluator &theEvaluator)
{
      NodeRefList centerNodes; 
      
      centerNodes = theEvaluator.selectNodeList (centerNodes, theDOMSupport, theContextNode,  
                                             XalanDOMString(XPath.c_str()).c_str(),
                                             thePrefixResolver);
     
     //we simply iterate over all the metrics an create them for the job
     return centerNodes.getLength();

}

}
