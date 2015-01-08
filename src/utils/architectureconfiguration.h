#ifndef SIMULATORARCHITECTURECONFIGURATION_H
#define SIMULATORARCHITECTURECONFIGURATION_H

#include <string>
#include <vector>
#include <utils/log.h>
#include <utils/configurationfile.h>

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

using namespace std;
using namespace Utils;
using namespace xercesc;
using namespace xalanc;

namespace Utils {

/**
* @author Francesc Guim,C6-E201,93 401 16 50,
*/

/**
 * This class implements the definition of the architecutre used in the simulation. This is only the pure abstract class, the subclasses may implement submethods for gathering more detailed information concerning the architecture.
*/
class ArchitectureConfiguration{
public:
  //Constructors
  ArchitectureConfiguration(string ConfigurationFile,Log* log);
  ArchitectureConfiguration();
  ~ArchitectureConfiguration();
  
  //virtual classes to be implemented if required by the local classes 
  virtual bool loadConfiguration();
  
  //Sets and gets methods
  void setConfigurationFile(string& theValue);
  string getConfigurationFile();   	
  void setNumberCpus(const int& theValue);
  int getNumberCpus() const;
  virtual int getNodeIdCpu(int cpuId);
  void setNodeSize(const int& theValue);
  int getnodeSize() const;
  void setNumberNodes(const int& theValue);
  int getNumberNodes() const;
  void setClockSpeed ( double theValue );
  double getClockSpeed() const;
  void setVendor ( const string& theValue );
  string getVendor() const;
  void setOSName ( const string& theValue );
  string getOSName() const;
  void setDisKSize ( double theValue );
  double getdisKSize() const;	
  void setMemory ( double theValue );
  double getMemory() const;	
  void setArchitectureType ( const architecture_type_t& theValue );
  architecture_type_t getArchitectureType() const;
  vector<double>* getFrequencies();
  vector<double>* getVoltages();
	
	
protected:
  string ConfigurationFile; /**< The file path for the configuration file used for create the architecture  */
  Log* log; /**< The logging engine   */
  /* minimum architectural information needed for a simulation*/
  int NumberCpus; /**< The total number of processors of the architecture */
  int nodeSize; /**< The size of the nodes -- not always valid (see heterogeneous resources) */
  int NumberNodes; /**< The total number of nodes in the architecture  */

  string Vendor; /**< The vendor of the machine  */
  double ClockSpeed; /**< Indicates the clock speed  */    
  string OSName; /**< The operating system running on the host */
  double disKSize; /**< The amount of terabytes of capacity of the host */
  double Memory; /**< The global amount of Memory - this field is an agregated of all the information, in some situations may be it make no sense. */

  int containsXPath(string XPath,DOMSupport &theDOMSupport, XalanNode *theContextNode, const PrefixResolver  &thePrefixResolver, XPathEvaluator &theEvaluator);
  void extractCommonFields(DOMSupport &theDOMSupport, XalanNode *theContextNode, const PrefixResolver  &thePrefixResolver, XPathEvaluator &theEvaluator);

  architecture_type_t ArchitectureType; /**< The architecture type - by default it is a LOCAL_SYSTEM */
  


public:
    double Activity;
    double Portion;
    vector<double> Frequencies; /**< Frequencies supported by CPUs */
    vector<double> Voltages;    /**< Corresponding voltages */
};


}

#endif
