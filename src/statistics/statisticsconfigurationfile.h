#ifndef UTILSSTATISTICSCONFIGURATIONFILE_H
#define UTILSSTATISTICSCONFIGURATIONFILE_H

#include <scheduling/metric.h>
#include <utils/log.h>

#include <vector>

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
using namespace std;
using std::vector;

using namespace xercesc;
using namespace xalanc;

namespace Utils {

/**
* @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

/** This class contains the configuration file for the statistics definition */
class StatisticsConfigurationFile{
public:
  StatisticsConfigurationFile(string StatisticsConfigurationFilePath,Log* log);
  ~StatisticsConfigurationFile();
  vector< Metric*>* getjobMetrics();
  vector< Metric*>* getPolicyMetrics();
	

private:
  metric_t getMetricType(string variable);
  statistic_t getStatisticType(string estimator);
  native_t getNativeType(string variable);   
  void addMetric(string variable,string estimator,vector<Metric*>* metrics);
  void addJobMetric(string variable,string estimator);
  void addPolicyMetric(string variable,string estimator);
  int containsXPath(string XPath,DOMSupport &theDOMSupport, XalanNode *theContextNode, const PrefixResolver &thePrefixResolver, XPathEvaluator &theEvaluator);

  vector<Metric*> jobMetrics /**< The set of metrics to be collected for the job entity */;
  vector<Metric*> policyMetrics /** The set of metrics to be collected for the policy entity */;
  string StatisticsConfigurationFilePath; /** The statistics configuration file path*/

};

}

#endif
