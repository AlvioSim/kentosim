#include <utils/mnconfiguration.h>
#include <archdatamodel/marenostrum.h>
#include <archdatamodel/node.h>
#include <archdatamodel/bladecenter.h>
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


/*END OF STUFF RELATED TO XALAN*/
using namespace xercesc;
using namespace xalanc;

namespace Utils {

/***************************
  Constructor and destructors
***************************/
/**
 * The default constructor for the class
 */
MNConfiguration::MNConfiguration()
{
  this->marenostrum = NULL;
}

/**
 * The default destructor for the class 
 */
MNConfiguration::~MNConfiguration()
{
}

/**
 * The constructor for the architectureconfiguration
 * @param ConfigurationFile The filepath for the configuration file 
 * @param log A reference to the logging engine 
 */
MNConfiguration::MNConfiguration(string ConfigurationFile,Log* log) : ArchitectureConfiguration(ConfigurationFile,log)
{
  this->marenostrum = NULL;
  this->ArchitectureType = MARENOSTRUM;
}

/***************************
  Auxiliar and main methods
***************************/

/**
 * Extracts the common fileds, basically calling the upper class.
 */
bool MNConfiguration::extractCommonFields()
{
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
       XalanDOMString("/MareNostrum").c_str(),
       thePrefixResolver);         
       
   if (theBaseContextNode == 0)
   {
       log->error("The element /MareNostrum/BladeCenters is missing or the document has some errors. Please check the architecture configuration file.");
       return false;
    }

   ArchitectureConfiguration::extractCommonFields(theDOMSupport,theBaseContextNode,thePrefixResolver,theEvaluator);


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

 return true;
 
}

/**
 * Loads the configuration file and creates the marenostrum objects 
 * @return A bool indicating if the load has been done correctly.
 */
bool MNConfiguration::loadConfiguration()
{
 //The configuration is load from an XML File using the Xalan library
 int globalCpuId = 0;
 int globalNodeId = 0;
 this->NumberCpus = 0;
 this->NumberNodes = 0;
 
 //we did it separate do to from some reason when calling the extractComonFields inside here the xerces crashes in a very strange way ..
 if(!this->extractCommonFields())
 {
   log->error("Some error occurred when parsing the common fileds.");
   return false;
 }
   
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
       XalanDOMString("/MareNostrum").c_str(),
       thePrefixResolver);         
       
   if (theBaseContextNode == 0)
   {
       log->error("The element /MareNostrum/BladeCenters is missing or the document has some errors. Please check the architecture configuration file.");
       return false;
    }
   
    // OK, let's find the context node... the BladeCenters
   XalanNode* const theContextNode =
      theEvaluator.selectSingleNode(
        theDOMSupport,
       theDocument,
       XalanDOMString("/MareNostrum/BladeCenters").c_str(),
       thePrefixResolver);         
       
   if (theContextNode == 0)
   {
       log->error("The element /MareNostrum/BladeCenters is missing or the document has some errors. Please check the architecture configuration file.");
       return false;
    }


 
//************************We get the activity factor *****************************/      
    if(containsXPath(string("Activity"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
    {
     const XObjectPtr theActivityResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("Activity").c_str(),
         thePrefixResolver));
         
     if(theActivityResult.null())
     {
       printf("The filed activity is not present in the configuration file ! \n ");
       Activity=-1;
     }
     else   
     {   
       stringstream stActivity;
       stActivity << theActivityResult->str();
       Activity = atof(stActivity.str().c_str()); 
     }
   }


//************************We get the portion of static in the total CPU power *****************************/      
    if(containsXPath(string("Portion"),theDOMSupport,theContextNode,thePrefixResolver,theEvaluator))  
    {
     const XObjectPtr thePortionResult(
       theEvaluator.evaluate(
         theDOMSupport,
         theContextNode,
         XalanDOMString("Portion").c_str(),
         thePrefixResolver));
         
     if(thePortionResult.null())
     {
       printf("The filed portion is not present in the configuration file ! \n ");
       Activity=-1;
     }
     else   
     {   
       stringstream stPortion;
       stPortion << thePortionResult->str();
       Portion = atof(stPortion.str().c_str()); 
     }
   }


//************************We get the number of gears supported *****************************/      
    

  NodeRefList Gears;
  
  theEvaluator.selectNodeList (
       Gears,
       theDOMSupport,
       theContextNode,
       XalanDOMString("Gear").c_str(),
       thePrefixResolver);

  for(int i = 0; i < Gears.getLength();i++)
     {
       XalanNode* XMLgear = Gears.item(i);

      const XObjectPtr theFrequencyResult(
      theEvaluator.evaluate(
        theDOMSupport,
        XMLgear,
        XalanDOMString("Frequency").c_str(),
        thePrefixResolver));
        
     if(theFrequencyResult.null())
     {
       log->error("The gear "+itos(i)+" has no frequency.. please check the configuration architecture file ");
       continue;
     }
     
    
     stringstream sFrequency;
     sFrequency << theFrequencyResult->str();
     Frequencies.push_back(atof(sFrequency.str().c_str()));
    



      const XObjectPtr theVoltageResult(
      theEvaluator.evaluate(
        theDOMSupport,
        XMLgear,
        XalanDOMString("Voltage").c_str(),
        thePrefixResolver));
        
     if(theVoltageResult.null())
     {
       log->error("The gear "+itos(i)+" has no voltage.. please check the configuration architecture file ");
       continue;
     }
     
    
     stringstream sVoltage;
     sVoltage << theVoltageResult->str();
     Voltages.push_back(atof(sVoltage.str().c_str()));
      } 



    //Now we have to go thourg all the Blade centers !
    NodeRefList BladeCenters;     
    
   theEvaluator.selectNodeList (
       BladeCenters,
       theDOMSupport,
       theContextNode,
       XalanDOMString("BladeCenter").c_str(),
       thePrefixResolver);

   
   if (BladeCenters.getLength() == 0)
   {
       log->error("The elements /MareNostrum/BladeCenters/BladeCenter are missing or the document has some errors. Please check the architecture configuration file.");
       return false;
    }    
   
   this->marenostrum = new MareNostrum();
   
   int totalCPUS = 0;
   int totalNodes = 0;
   
   /*It's time to process each of the blades to get the information */
   for(int i = 0; i < BladeCenters.getLength();i++)
   {
      XalanNode* XMLblade = BladeCenters.item(i);

     int totalCPUSBlade = 0;
     //We get the id
      const XObjectPtr theIdResult(
      theEvaluator.evaluate(
        theDOMSupport,
        XMLblade,
        XalanDOMString("@Id").c_str(),
        thePrefixResolver));
        
     if(theIdResult.null())
     {
       log->error("The blade center "+itos(i)+" has no identifier.. please check the configuration architecture file ");
       continue;
     }
     
     stringstream bladeIds;
     bladeIds << theIdResult->str();
     string bladeId = bladeIds.str();
       
     //We get the SwitchBW
      const XObjectPtr theSIOBWResult(
      theEvaluator.evaluate(
        theDOMSupport,
        XMLblade,
        XalanDOMString("IOBW").c_str(),
        thePrefixResolver));
        
     if(theSIOBWResult.null())
     {
       log->error("The blade center "+itos(i)+" has no IOBW.. please check the configuration architecture file ");
       continue;
     }
     
    
     stringstream sBWIO;
     sBWIO << theSIOBWResult->str();
     string IOBW = sBWIO.str();
    
     
     //It's time to get all the information of all the nodes
     NodeRefList nodes;
         
     theEvaluator.selectNodeList (
          nodes,
       theDOMSupport,
       XMLblade,
       XalanDOMString("BladeNodes/Node").c_str(),
       thePrefixResolver);
       
    if(nodes.getLength() == 0)
    {
      log->error("The blade "+itos(i)+" has no nodes.. please check the architecture configuration file.");
      continue;
    }
    else
      log->debug("There are "+itos(nodes.getLength())+ " nodes in the blade");
    
    BladeCenter* blade = new BladeCenter(atof(IOBW.c_str()));
    blade->setId(atoi(bladeId.c_str())); 
     
    int totalNodesBlade = nodes.getLength(); 
     
    for(int j = 0; j < totalNodesBlade;j++)
    {           	
       XalanNode* XMLnode = nodes.item(j);
       
       //We get the id
       const XObjectPtr theIdNodeResult(
       theEvaluator.evaluate(
         theDOMSupport,
         XMLnode,
         XalanDOMString("@Id").c_str(),
         thePrefixResolver));
         
      if(theIdNodeResult.null())
      {
        log->error("The node "+ itos(j)+ " of the blade center "+itos(i)+" has no identifier.. please check the configuration architecture file ");
        continue;
      }
      
       stringstream nIdS;
       nIdS << theIdNodeResult->str();
       string nodeId = nIdS.str();     
      
      //We get the number of times that this structure has to be repeated 
      const XObjectPtr theRepeatResult(
      theEvaluator.evaluate(
        theDOMSupport,
        XMLnode,
        XalanDOMString("@Repeat").c_str(),
        thePrefixResolver));
        
     if(theRepeatResult.null())
     {
       log->error("The blade center "+itos(i)+" has no identifier.. please check the configuration architecture file ");
       continue;
     }
     
     stringstream Repeats;
     Repeats << theRepeatResult->str();
     string RepeatSt = Repeats.str();
     int repeatNode= atoi(RepeatSt.c_str());      
       
     if(repeatNode == 0)  
      repeatNode = 1;
       
       //We get the MemoryBW
       const XObjectPtr theMemBWResult(
       theEvaluator.evaluate(
         theDOMSupport,
         XMLnode,
         XalanDOMString("MemoryBW").c_str(),
         thePrefixResolver));
         
      if(theMemBWResult.null())
      {
        log->error("The node "+ itos(j)+ " of the blade center "+itos(i)+" has no node memory BW.. please check the configuration architecture file ");
        continue;
      }              

      stringstream nMBW;
      nMBW << theMemBWResult->str();
      string nodeMemoryBW = nMBW.str();       
       
       //We get the MemoryCapacity
       const XObjectPtr theMCResult(
       theEvaluator.evaluate(
         theDOMSupport,
         XMLnode,
         XalanDOMString("MemoryCapacity").c_str(),
         thePrefixResolver));
         
      if(theMCResult.null())
      {
        log->error("The node "+ itos(j)+ " of the blade center "+itos(i)+" has no node MemoryCapacity.. please check the configuration architecture file ");
        continue;
      }
      
      stringstream nMC;
      nMC << theMCResult->str();
      string nodeMemoryCapacity = nMC.str();   

       //We get the NetworkBW
       const XObjectPtr theNBWResult(
       theEvaluator.evaluate(
         theDOMSupport,
         XMLnode,
         XalanDOMString("NetworkBW").c_str(),
         thePrefixResolver));
         
      if(theNBWResult.null())
      {
        log->error("The node "+ itos(j)+ " of the blade center "+itos(i)+" has no node NetworkBW.. please check the configuration architecture file ");
        continue;
      }
       
      stringstream nNBW;
      nNBW << theNBWResult->str();
      string nodeNetworkBW = nNBW.str();  
      
       //We get the EthernetBW
       const XObjectPtr theEBWResult(
       theEvaluator.evaluate(
         theDOMSupport,
         XMLnode,
         XalanDOMString("EthernetBW").c_str(),
         thePrefixResolver));
         
      if(theEBWResult.null())
      {
        log->error("The node "+ itos(j)+ " of the blade center "+itos(i)+" has no node EthernetBW.. please check the configuration architecture file ");
        continue;
      }
      
      stringstream nEBW;
      nEBW << theEBWResult->str();
      string nodeEthernetBW = nEBW.str();  

       //We get the NumberCPUS
       const XObjectPtr theNCPUSResult(
       theEvaluator.evaluate(
         theDOMSupport,
         XMLnode,
         XalanDOMString("NumberCPUS").c_str(),
         thePrefixResolver));
         
      if(theNCPUSResult.null())
      {
        log->error("The node "+ itos(j)+ " of the blade center "+itos(i)+" has no node NumberCPUS.. please check the configuration architecture file ");
        continue;
      }
      
      stringstream nCPUs;
      nCPUs << theNCPUSResult->str();
      string nodeNumberCPUS = nCPUs.str();  
            
       //int id = atoi(nodeId.c_str());
       
	int id = totalNodes;

       for(int k = 0;k<repeatNode;k++)  
       {  
         Node* newNode = new Node(atof(nodeEthernetBW.c_str()),atof(nodeMemoryCapacity.c_str()),atof(nodeNetworkBW.c_str()),atof(nodeMemoryBW.c_str()),atoi(nodeNumberCPUS.c_str()));
       
         this->log->debug("The node "+itos(id) + " of the blade center "+bladeId+"has the following characteristics: ");
         this->log->debug("nodeEthernetBW-->"+nodeEthernetBW);
         this->log->debug("nodeMemoryCapacity-->"+nodeMemoryCapacity);
         this->log->debug("nodeNetworkBW-->"+nodeNetworkBW);
         this->log->debug("nodeMemoryBW-->"+nodeMemoryBW);
         this->log->debug("numberOfCpus-->"+nodeNumberCPUS);
    
         //if(!blade->hasNode(id))
         //{
           blade->addNode(newNode,id);
           //this->marenostrum->setGlobalNodeId(newNode,globalNodeId);
	   this->marenostrum->setGlobalNodeId(newNode,totalNodes);
         
           //create the virtual mapping 
           for(int i = 0; i < atoi(nodeNumberCPUS.c_str()); i++) 
           {
             //this->marenostrum->setNodeIdCpu(globalCpuId,globalNodeId); 
	     this->marenostrum->setNodeIdCpu(totalCPUS,totalNodes);
             //globalCpuId++;
	     totalCPUS++;
           } 
         
           globalNodeId++;         
         //}
         //else 
           //log->error("There are more than one nodes with the id "+string(nodeId)+". Please check the architecture");

         //Updatting the global number of cpus 
         totalCPUSBlade += atoi(nodeNumberCPUS.c_str());
         id++;
         totalNodes++;
       }
     }
     
     //We can add the blade to the marenostrum
     if(!marenostrum->hasBlade((atoi(bladeId.c_str()))))	
       marenostrum->addBladeCenter(blade,atoi(bladeId.c_str()));
     else
     {
       log->error("There are more than one blades with the id "+string(bladeId)+". Please check the architecture");
       continue;
     }  
     
     blade->setTotalCPUS(totalCPUSBlade);
     
     //totalCPUS+= totalCPUSBlade;
     
    }
   
    this->marenostrum->setTotalCPUS(totalCPUS);

    this->marenostrum->setTotalNodes(totalNodes);
    this->NumberCpus = totalCPUS;
    this->NumberNodes = totalNodes;
    
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

 /***************************
  Gets and Sets
***************************/
 
/**
 * Returns a reference to the loaded marenostrum configuration 
 * @return A reference to the marenostrum object 
 */
MareNostrum* MNConfiguration::getmarenostrum() const
{
  return marenostrum;
}


/**
 * Sets the reference to the marenostrum architectureconfiguration
 * @param theValue The reference to the architectureconfiguration
 */
void MNConfiguration::setMarenostrum(MareNostrum* theValue)
{
  marenostrum = theValue;
}

/**
 * Returns the node that contains the cpu id provided
 * @param cpuId The global id for the cpu 
 * @return The id for the node that contains the cpu
 */
int MNConfiguration::getNodeIdCpu(int cpuId)
{
  return this->marenostrum->getNodeIdCpu(cpuId);
}

}



