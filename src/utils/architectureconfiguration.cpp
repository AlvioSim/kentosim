#include <utils/architectureconfiguration.h>
#include <math.h>
#include <assert.h>
#include<sstream>

namespace Utils {

/***************************
  Constructor and destructors
***************************/

/**
 * The constructor for the architectureconfiguration
 * @param ConfigurationFile The filepath for the configuration file 
 * @param log A reference to the logging engine 
 */
ArchitectureConfiguration::ArchitectureConfiguration(string ConfigurationFile,Log* log)
{
  this->ConfigurationFile = ConfigurationFile;
  this->log = log;
  this->nodeSize = 1;
  this->ArchitectureType = LOCAL_SYSTEM;
  
  assert(log != NULL && ConfigurationFile.compare("") != 0);
}

/**
 * The default constructor for the class
 */
ArchitectureConfiguration::ArchitectureConfiguration()
{
 this->log = NULL;
}

/**
 * The default destructor for the class 
 */
ArchitectureConfiguration::~ArchitectureConfiguration()
{
}

    /***********************************************************************************************
    ********************PARSING XML FUNCTIONS  ****************************************************
    ************************************************************************************************/


/**
 * Loads the configuration file and creates the marenostrum objects 
 * @return A bool indicating if the load has been done correctly.
 */
bool ArchitectureConfiguration::loadConfiguration()
{
 //The configuration is load from an XML File using the Xalan library
 int globalCpuId = 0;
 int globalNodeId = 0;
 this->NumberCpus = 0;
 this->NumberNodes = 0;
 
   
 XALAN_USING_XALAN(XSLException)

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
  
    const XalanDOMString theFileName(this->ConfigurationFile.c_str());
  
    // Create an input source that represents a local file...
    const LocalFileInputSource theInputSource(theFileName.c_str());
 
    log->debug("Loading the architecture file: "+ConfigurationFile);
    
    // Parse the document...
    XalanDocument* const theDocument =
      theLiaison.parseXMLStream(theInputSource);
    assert(theDocument != 0);
     
     XalanDocumentPrefixResolver  thePrefixResolver(theDocument); 
   /*
     Now that the document is loaded we go through all the elements of the XML document
      starting from the base one the BladeCenters
   */

  
    XPathEvaluator theEvaluator;
 

   //let's extract the common stuff
    XalanNode* const theBaseContextNode =
      theEvaluator.selectSingleNode(
        theDOMSupport,
       theDocument,
       XalanDOMString("/ArchictectureConfiguration").c_str(),
       thePrefixResolver);         
       
   if (theBaseContextNode == 0)
   {
       log->error("The element /ArchictectureConfiguration is missing or the document has some errors. Please check the architecture configuration file.");
       return false;
    }

 


   extractCommonFields(theDOMSupport,theBaseContextNode,thePrefixResolver,theEvaluator);
   }
  } 
  catch(const XSLException&   theException)
  {
     string errorMsg(((char*)(theException.getMessage()).c_str()));
   log->error("XSL exception: "+errorMsg+" The document has some errors. Please check the architecture configuration file.");
   return false;
  }
 catch(...)
 {
   log->error("Undefined error the document has some errors. Please check the architecture configuration file.");
   return false;
 }
 }

/**
 * Auxiliar expression that checks the number of nodes in a xml file that matches the provided XPath expression. See that the context node indicates the reference node from where the XPath expression will be applied
 * @param XPath The xpath expression 
 * @param theDOMSupport See the xalanc documentation 
 * @param theContextNode The context node   
 * @param thePrefixResolver See the xalanc documentation 
 * @param theEvaluator See the xalanc documentation 
 * @return 
 */
int ArchitectureConfiguration::containsXPath(string XPath,DOMSupport &theDOMSupport, XalanNode *theContextNode, const PrefixResolver &thePrefixResolver, XPathEvaluator &theEvaluator)
{
      NodeRefList centerNodes; 
      
      centerNodes = theEvaluator.selectNodeList (centerNodes, theDOMSupport, theContextNode,  
                                             XalanDOMString(XPath.c_str()).c_str(),
                                             thePrefixResolver);
     
     //we simply iterate over all the metrics an create them for the job
     return centerNodes.getLength();

}

/**
 * Auxiliar function that extract all the common fields. 
 * @param XPath The xpath expression 
 * @param theDOMSupport See the xalanc documentation 
 * @param theContextNode The context node   
 * @param thePrefixResolver See the xalanc documentation 
 * @param theEvaluator See the xalanc documentation 
 * @return 
 */
void ArchitectureConfiguration::extractCommonFields(DOMSupport &theDOMSupport, XalanNode *theContextNode, const PrefixResolver &thePrefixResolver, XPathEvaluator &theEvaluator)
{
       //************************We get the Vendor *****************************/     
      if(containsXPath(string("Vendor"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator)) 
      {
         const XObjectPtr theVendorResult(
           theEvaluator.evaluate(
           theDOMSupport,
           theContextNode,
           XalanDOMString("Vendor").c_str(),
           thePrefixResolver));
         
        if(theVendorResult.null())
        {
          printf("The filed Vendor is not present in the configuration file ! \n");
          Vendor = "";
        }
        else    
        { 
          stringstream stVendor;
          stVendor << theVendorResult->str();
          Vendor = stVendor.str();  
        }
      }
       //************************We get the OSName *****************************/     
      if(containsXPath(string("OSName"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator)) 
      {
         const XObjectPtr theOSNameResult(
           theEvaluator.evaluate(
           theDOMSupport,
           theContextNode,
           XalanDOMString("OSName").c_str(),
           thePrefixResolver));
         
        if(theOSNameResult.null())
        {
          printf("The filed OSName is not present in the configuration file ! \n");
          OSName = "";
        }
        else    
        { 
          stringstream stOSName;
          stOSName << theOSNameResult->str();
          OSName = stOSName.str();  
        }
      }
       //************************We get the ClockSpeed *****************************/
     
      if(containsXPath(string("ClockSpeed"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator)) 
      {
         const XObjectPtr theClockSpeedResult(
           theEvaluator.evaluate(
           theDOMSupport,
           theContextNode,
           XalanDOMString("ClockSpeed").c_str(),
           thePrefixResolver));
         
        if(theClockSpeedResult.null())
        {
          printf("The filed ClockSpeed is not present in the configuration file ! \n");
          ClockSpeed = -1;
        }
        else    
        { 
          stringstream stClockSpeed;
          stClockSpeed << theClockSpeedResult->str();
          ClockSpeed = atof(stClockSpeed.str().c_str());  
        }
      }

      //************************We get the disKSize *****************************/
     
      if(containsXPath(string("disKSize"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator)) 
      {
         const XObjectPtr thedisKSizeResult(
           theEvaluator.evaluate(
           theDOMSupport,
           theContextNode,
           XalanDOMString("disKSize").c_str(),
           thePrefixResolver));
         
        if(thedisKSizeResult.null())
        {
          printf("The filed disKSize is not present in the configuration file ! \n");
          disKSize =  -1;
        }
        else    
        { 
          stringstream stdisKSize;
          stdisKSize << thedisKSizeResult->str();
          disKSize = atof(stdisKSize.str().c_str());  
        }
      }

      //************************We get the Memory *****************************/
     
      if(containsXPath(string("Memory"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator)) 
      {
         const XObjectPtr theMemoryResult(
           theEvaluator.evaluate(
           theDOMSupport,
           theContextNode,
           XalanDOMString("Memory").c_str(),
           thePrefixResolver));
         
        if(theMemoryResult.null())
        {
          printf("The filed Memory is not present in the configuration file ! \n");
          Memory =  -1;
        }
        else    
        { 
          stringstream stMemory;
          stMemory << theMemoryResult->str();
          Memory = atof(stMemory.str().c_str());  
        }
      }

      //************************We get the NumberCPUS *****************************/
     
      if(containsXPath(string("NumberCPUS"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator)) 
      {
         const XObjectPtr theNumberCPUSResult(
           theEvaluator.evaluate(
           theDOMSupport,
           theContextNode,
           XalanDOMString("NumberCPUS").c_str(),
           thePrefixResolver));
         
        if(theNumberCPUSResult.null())
        {
          printf("The filed NumberCPUS is not present in the configuration file ! \n");
          NumberCpus =  0;
        }
        else    
        { 
          stringstream stNumberCPUS;
          stNumberCPUS << theNumberCPUSResult->str();
          NumberCpus = atoi(stNumberCPUS.str().c_str());  
        }
      }

}


/***************************
  Gets and sets method
***************************/
/**
 * Returns the ConfigurationFile
 * @return A string containing the ConfigurationFile
 */
string ArchitectureConfiguration::getConfigurationFile() 
{
  return ConfigurationFile;
}

/**
 * Sets the ConfigurationFile to the ArchitectureConfiguration
 * @param theValue The ConfigurationFile
 */
void ArchitectureConfiguration::setConfigurationFile(string& theValue)
{
  ConfigurationFile = theValue;
}

/**
 * Returns the NumberCpus
 * @return A integer containing the NumberCpus
 */
int ArchitectureConfiguration::getNumberCpus() const
{
  return NumberCpus;
}

/**
 * Sets the NumberCpus to the ArchitectureConfiguration
 * @param theValue The NumberCpus
 */
void ArchitectureConfiguration::setNumberCpus(const int& theValue)
{
  NumberCpus = theValue;
}

/**
 * Returns the node to which the cpu with the provided global id belongs 
 * @return A integer containing the node id 
 */
int ArchitectureConfiguration::getNodeIdCpu(int cpuId)
{
  return (int) ceil((float)cpuId/this->nodeSize);
}
/**
 * Returns the nodeSize
 * @return A integer containing the nodeSize
 */
int ArchitectureConfiguration::getnodeSize() const
{
  return nodeSize;
}

/**
 * Sets the nodeSize to the ArchitectureConfiguration
 * @param theValue The nodeSize
 */
void ArchitectureConfiguration::setNodeSize(const int& theValue)
{
  nodeSize = theValue;
}

/**
 * Returns the NumberNodes
 * @return A integer containing the NumberNodes
 */
int ArchitectureConfiguration::getNumberNodes() const
{
  return NumberNodes;
}

/**
 * Returns the NumberNodes
 * @return A integer containing the NumberNodes
 */
void ArchitectureConfiguration::setNumberNodes(const int& theValue)
{
  NumberNodes = theValue;
}

/**
 * Returns the ClockSpeed
 * @return An double containing the ClockSpeed
 */
double ArchitectureConfiguration::getClockSpeed() const
{
	return ClockSpeed;
}

/**
 * Sets the ClockSpeed to the ArchitectureConfiguration
 * @param theValue The ClockSpeed
 */
void ArchitectureConfiguration::setClockSpeed ( double theValue )
{
	ClockSpeed = theValue;
}

/**
 * Returns the disKSize
 * @return An integer containing the disKSize
 */
double ArchitectureConfiguration::getdisKSize() const
{
	return disKSize;
}

/**
 * Sets the disKSize to the ArchitectureConfiguration
 * @param theValue The disKSize
 */
void ArchitectureConfiguration::setDisKSize ( double theValue )
{
	disKSize = theValue;
}

/**
 * Returns the OSName
 * @return An string containing the OSName
 */
string ArchitectureConfiguration::getOSName() const
{
	return OSName;
}

/**
 * Sets the OSName to the ArchitectureConfiguration
 * @param theValue The OSName
 */
void ArchitectureConfiguration::setOSName ( const string& theValue )
{
	OSName = theValue;
}

/**
 * Returns the Vendor
 * @return A string containing the Vendor
 */
string ArchitectureConfiguration::getVendor() const
{
	return Vendor;
}

/**
 * Sets the Vendor to the ArchitectureConfiguration
 * @param theValue The Vendor
 */
void ArchitectureConfiguration::setVendor ( const string& theValue )
{
	Vendor = theValue;
}

/**
 * Returns the Memory
 * @return A double containing the Memory
 */
double ArchitectureConfiguration::getMemory() const
{
	return Memory;
}

/**
 * Sets the Memory to the ArchitectureConfiguration
 * @param theValue The Memory
 */
void ArchitectureConfiguration::setMemory ( double theValue )
{
	Memory = theValue;
}


/**
 * Returns the ArchitectureType
 * @return A achitecture_type_t containing the ArchitectureType
 */
architecture_type_t ArchitectureConfiguration::getArchitectureType() const
{
	return ArchitectureType;
}

/**
 * Sets the ArchitectureType to the ArchitectureConfiguration
 * @param theValue The ArchitectureType
 */
void ArchitectureConfiguration::setArchitectureType ( const architecture_type_t& theValue )
{
	ArchitectureType = theValue;
}

vector<double>* ArchitectureConfiguration::getFrequencies()
{
   return &this->Frequencies;
}
vector<double>* ArchitectureConfiguration::getVoltages()
{
   return &this->Voltages;
}


}
