#include <utils/predictorconfigurationfile.h>


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
 * Function that initialize the variables for the predictor information 
 */
void PredictorConfigurationFile::init()
{  
  postDeadlineManager = OTHER_DM_MANAGER;
  preDeadlineManager = OTHER_DM_MANAGER;
  predictorType = OTHER_PREDICTOR;
  this->fileLoaded = false;
  this->classifierType = OTHER_CLASSIFIER;
  this->numberOfBins = -1;
  this->ResponseJobAttributes = 0;
  this->NewFilePerModel = false;
  this->PredictionVariable = "";
  this->ModelDirectoryFile = "";
  this->ModelIntervalGeneration = 0;
  this->discretizer = OTHER_DISCRETIZER;
  
}

/**
 * The constructor for the class 
 * @param ConfigFile The file path of the configuration file for the predictor 
 * @return 
 */
PredictorConfigurationFile::PredictorConfigurationFile(string ConfigFile)
{
 this->init();
  
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
    
    std::cout << "Loading the predictor configuration file "<< ConfigFile << endl;
  
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
  
    // OK, let's find the context node... the 
    
    /**************************************************************************************************************
    ***************************************GETTING THE USER RUNTIME PREDICTOR STUFF IN CASE IT IS  ***************
    **************************************************************************************************************/
    XalanNode*  theContextNode =
      theEvaluator.selectSingleNode(
        theDOMSupport,
       theDocument,
       XalanDOMString("PredictorConfiguration/UserRuntime").c_str(),
       thePrefixResolver);         
       
    if (theContextNode == 0)
    {
        printf("This configuration file is not about user estimate");        
    }
    else
    {
     this->predictorType = USER_RUNTIME_ESTIMATE;
     
     //probably in the future more stuff will be added here
     
    }
    
    /**************************************************************************************************************
    ***************************************GETTING THE HISTORICAL CLASSIFIER STUFF IN CASE IT IS  ***************
    **************************************************************************************************************/
    theContextNode =
      theEvaluator.selectSingleNode(
        theDOMSupport,
       theDocument,
       XalanDOMString("PredictorConfiguration/HistoricalClassifier").c_str(),
       thePrefixResolver);         
       
    if (theContextNode == 0)
    {
        printf("This configuration file is not about historical classifier");        
    }
    else
    {
     this->predictorType = HISTORICAL_CLASSIFIER;
     extractHistoricalClassifierFields(theDOMSupport,theContextNode,thePrefixResolver,theEvaluator);                                         
    }
    
    
    /**************************************************************************************************************
    ***************************************GETTING THE COMMON FILE CARACTERISTICS FOR THE PREDICTOR ***************
    **************************************************************************************************************/
    
    theContextNode =
      theEvaluator.selectSingleNode(
        theDOMSupport,
       theDocument,
       XalanDOMString("PredictorConfiguration/Common").c_str(),
       thePrefixResolver);         
       
    if (theContextNode == 0)
    {
        printf("The predictor configuration file %s is not valid, please check it",this->ConfigFile.c_str());
    }
    else
    {
     
     //************************We get the PreDeadlineMissManager *****************************/
    if(containsXPath(string("PreDeadlineMissManager"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator)) 
    {
      const XObjectPtr thepreDeadlineManagerResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("PreDeadlineMissManager").c_str(),
         thePrefixResolver));
         
     if(thepreDeadlineManagerResult.null())
     {
       printf("The filed PreDeadlineMissManager is not present in the configuration file ! ");       
     }
     else    
     { 
       stringstream stpreDeadlineManager;
       stpreDeadlineManager << thepreDeadlineManagerResult->str();
       string preDeadlineManagerS = stpreDeadlineManager.str();  
       
       if(preDeadlineManagerS == "GRADUAL")
        this->preDeadlineManager = GRADUAL;
       else if(preDeadlineManagerS == "FACTOR")
        this->preDeadlineManager = FACTOR;
       else
       {
         printf("The pre deadline manager specified in the configuration file is not valid.");
       }
     }
     
    }
        
      //************************We get the ModelIntervalGeneration *****************************/      
    if(containsXPath(string("ModelIntervalGeneration"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
    {    
     const XObjectPtr theModelIntervalGenerationResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("ModelIntervalGeneration").c_str(),
         thePrefixResolver));
         
     if(theModelIntervalGenerationResult.null())
     {
       printf("The filed ModelIntervalGeneration is not present in the configuration file !\n ");
       ModelIntervalGeneration = 1;
     }
     else
     {   
       stringstream stModelIntervalGeneration;
       stModelIntervalGeneration << theModelIntervalGenerationResult->str();
       string ModelIntervalGenerationS = stModelIntervalGeneration.str();  
       
       if(ModelIntervalGenerationS.compare("") != 0)
         this->ModelIntervalGeneration = atoi(ModelIntervalGenerationS.c_str());
       else
         this->ModelIntervalGeneration = 100; /* by default 100 */
     }           
    }    
        
        
        //************************We get the PostDeadlineMissManager *****************************/
    if(containsXPath(string("PostDeadlineMissManager"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator)) 
    { 
     const XObjectPtr thepostDeadlineManagerResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("PostDeadlineMissManager").c_str(),
         thePrefixResolver));
         
     if(thepostDeadlineManagerResult.null())
     {
      printf("The filed PostDeadlineMissManager is not present in the configuration file ! ");       
     }
     else    
     { 
       stringstream stpostDeadlineManager;
       stpostDeadlineManager << thepostDeadlineManagerResult->str();
       string postDeadlineManagerS = stpostDeadlineManager.str();  
       
       this->postDeadlineManager = this->getDeadLineManager(postDeadlineManagerS);
     }
    }
     
   }
   
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
 * The destrcutor for the class
 */
PredictorConfigurationFile::~PredictorConfigurationFile()
{
}

/**
 * Returns the classifier type contained in the string 
 * @param strClassifier The string containing the classifier type 
 * @return A classifier_t containing the classifier type indicated in the string 
 */
classifier_t PredictorConfigurationFile::getClassifierType(string strClassifier)
{
  if(strClassifier == "ID3")
    return ID3;
  else if(strClassifier == "C45")
    return C45;
  else
  {
    return OTHER_CLASSIFIER;
  }

}

/**
 * Returns the discretizer type contained in the string 
 * @param strClassifier The string containing the discretizer type 
 * @return A discretizer_t containing the discretizer type indicated in the string 
 */
discretizer_t PredictorConfigurationFile::getDiscretizerType(string strDiscretizer)
{
  if(strDiscretizer == "SAME_INTERVAL_DISCRETIZER")
    return SAME_INTERVAL_DISCRETIZER;
  else if(strDiscretizer == "SAME_INSTANCES_PER_INTERVAL_DISCRETIZER")
    return SAME_INSTANCES_PER_INTERVAL_DISCRETIZER;
  else
  {
    return OTHER_DISCRETIZER;
  }
}

/**
 * Returns the deadline manager  type contained in the string 
 * @param strClassifier The string containing the deadline manager type 
 * @return A DeadlineMissManager_t containing the deadline manager type indicated in the string 
 */
DeadlineMissManager_t PredictorConfigurationFile::getDeadLineManager(string strManager)
{
  if(strManager == "GRADUAL")
    return GRADUAL;
  else if(strManager == "FACTOR")
    return FACTOR;
  else
  {
    return OTHER_DM_MANAGER;
  }
}
/***********************************************************************************************
********************PARSING XML FUNCTIONS  ****************************************************
***********************************************************************************************/

//replicated function from the simulationconfiguration , that given an xpath expression returns the number of nodes that matches the givne expression. 

/**
 * Auxiliar expression that checks the number of nodes in a xml file that matches the provided XPath expression. See that the context node indicates the reference node from where the XPath expression will be applied
 * @param XPath The xpath expression 
 * @param theDOMSupport See the xalanc documentation 
 * @param theContextNode The context node   
 * @param thePrefixResolver See the xalanc documentation 
 * @param theEvaluator See the xalanc documentation 
 * @return The number of nodes that matches the query
 */
int PredictorConfigurationFile::containsXPath(string XPath,DOMSupport &theDOMSupport, XalanNode *theContextNode, const PrefixResolver &thePrefixResolver, XPathEvaluator &theEvaluator)
{
      NodeRefList centerNodes; 
      
      centerNodes = theEvaluator.selectNodeList (centerNodes, theDOMSupport, theContextNode,  
                                             XalanDOMString(XPath.c_str()).c_str(),
                                             thePrefixResolver);
     
     //we simply iterate over all the metrics an create them for the job
     return centerNodes.getLength();

}

}

/**
 * Function that given a XML node reference extracts the EASY information 
 * @param theDOMSupport See the xalanc documentation 
 * @param theContextNode The context node   
 * @param thePrefixResolver See the xalanc documentation 
 * @param theEvaluator See the xalanc documentation 
*/
void PredictorConfigurationFile::extractHistoricalClassifierFields(DOMSupport &theDOMSupport, XalanNode *theContextNode, const PrefixResolver &thePrefixResolver, XPathEvaluator &theEvaluator)
{
     //Getting the classifier type 
     if(containsXPath(string("ClassifierType"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator)) 
     {
        const XObjectPtr theClassifierTypeResult(
         theEvaluator.evaluate(
           theDOMSupport,
           theContextNode,
           XalanDOMString("ClassifierType").c_str(),
           thePrefixResolver));
         
       if(theClassifierTypeResult.null())
       {
         printf("The filed ClassifierType is not present in the configuration file ! ");       
       }
       else    
       { 
         stringstream stClassifierType;
         stClassifierType << theClassifierTypeResult->str();
         this->classifierType = getClassifierType(stClassifierType.str());  
       }     
      }
      
      //Getting the discretizer type  
      if(containsXPath(string("DiscretizerType"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator)) 
      {
        const XObjectPtr theDiscretizerTypeResult(
         theEvaluator.evaluate(
           theDOMSupport,
           theContextNode,
           XalanDOMString("DiscretizerType").c_str(),
           thePrefixResolver));
         
       if(theDiscretizerTypeResult.null())
       {
         printf("The filed DiscretizerType is not present in the configuration file ! ");       
       }
       else    
       { 
         stringstream stDiscretizerType;
         stDiscretizerType << theDiscretizerTypeResult->str();
         this->discretizer = getDiscretizerType(stDiscretizerType.str());  
       }     
      }
      
      //Getting the number of bins 
      if(containsXPath(string("NumberOfBins"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator)) 
      {
        const XObjectPtr theNumberOfBinsResult(
         theEvaluator.evaluate(
           theDOMSupport,
           theContextNode,
           XalanDOMString("NumberOfBins").c_str(),
           thePrefixResolver));
         
       if(theNumberOfBinsResult.null())
       {
         printf("The filed NumberOfBins is not present in the configuration file ! ");       
       }
       else    
       { 
         stringstream stNumberOfBins;
         stNumberOfBins << theNumberOfBinsResult->str();
         this->numberOfBins = atoi(stNumberOfBins.str().c_str());
       }     
      }
      
      //Getting the PredictionVariable
      if(containsXPath(string("PredictionVariable"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator)) 
      {
        const XObjectPtr thePredictionVariableResult(
         theEvaluator.evaluate(
           theDOMSupport,
           theContextNode,
           XalanDOMString("PredictionVariable").c_str(),
           thePrefixResolver));
         
       if(thePredictionVariableResult.null())
       {
         printf("The filed PredictionVariable is not present in the configuration file ! ");       
       }
       else    
       { 
         stringstream stPredictionVariable;
         stPredictionVariable << thePredictionVariableResult->str();
         this->PredictionVariable = stPredictionVariable.str();
       }     
      }
      
      //Getting the ModelDirectoryFile 
      if(containsXPath(string("ModelDirectoryFile"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator)) 
      {
        const XObjectPtr theModelDirectoryFileResult(
         theEvaluator.evaluate(
           theDOMSupport,
           theContextNode,
           XalanDOMString("ModelDirectoryFile").c_str(),
           thePrefixResolver));
         
       if(theModelDirectoryFileResult.null())
       {
         printf("The filed ModelDirectoryFile is not present in the configuration file ! ");       
       }
       else    
       { 
         stringstream stModelDirectoryFile;
         stModelDirectoryFile << theModelDirectoryFileResult->str();
         this->ModelDirectoryFile = stModelDirectoryFile.str();
       }     
      }
      
      //Getting the  NewFilePerModel 
      if(containsXPath(string("NewFilePerModel"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator)) 
      {
        const XObjectPtr theNewFilePerModelResult(
         theEvaluator.evaluate(
           theDOMSupport,
           theContextNode,
           XalanDOMString("NewFilePerModel").c_str(),
           thePrefixResolver));
         
       if(theNewFilePerModelResult.null())
       {
         printf("The filed NewFilePerModel is not present in the configuration file ! ");       
       }
       else    
       { 
         stringstream stNewFilePerModel;
         stNewFilePerModel << theNewFilePerModelResult->str();
         if(stNewFilePerModel.str().compare("TRUE") == 0)  
           this->NewFilePerModel = true;
         else 
           this->NewFilePerModel = false;
       }                   
      }  
          
      
      //Getting the ModelIntervalGeneration 
      if(containsXPath(string("ModelIntervalGeneration"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator)) 
      {
        const XObjectPtr theModelIntervalGenerationResult(
         theEvaluator.evaluate(
           theDOMSupport,
           theContextNode,
           XalanDOMString("ModelIntervalGeneration").c_str(),
           thePrefixResolver));
         
       if(theModelIntervalGenerationResult.null())
       {
         printf("The filed ModelIntervalGeneration is not present in the configuration file ! ");       
       }
       else    
       { 
         stringstream stModelIntervalGeneration;
         stModelIntervalGeneration << theModelIntervalGenerationResult->str();
         this->ModelIntervalGeneration = atoi(stModelIntervalGeneration.str().c_str());
       }     
      }          
        
      //time to get the variables that will be used for construct the model 
      if(containsXPath(string("ResponseVariables/Variable"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator)) 
      {  
      
       this->ResponseJobAttributes = new vector<string>();
       NodeRefList metricsNodes; 
      
       metricsNodes = theEvaluator.selectNodeList (metricsNodes, theDOMSupport, theContextNode,  
                                             XalanDOMString("ResponseVariables/Variable").c_str(),
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
               continue;
            }
            else    
            { 
              stringstream stType;
              stType << theTypeResult->str();
              string AttName = stType.str();
              
              ResponseJobAttributes->push_back(AttName);
           }
          }      
         }
       }
}
