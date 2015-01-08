#ifndef UTILSPREDICTORCONFIGURATIONFILE_H
#define UTILSPREDICTORCONFIGURATIONFILE_H

#include <string>

#include <predictors/predictor.h>
#include <datamining/classifier.h>
#include <datamining/discretizer.h>


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

using namespace PredictionModule;
using namespace Datamining;
using namespace std;

using namespace xercesc;
using namespace xalanc;

namespace Utils {

/**
*@author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/


/** This class loads a definition of a predictor from a configuration file */
class PredictorConfigurationFile{
public:
  PredictorConfigurationFile(string predictorConfigurationFile);
  ~PredictorConfigurationFile();
  
  void init(); //initialize all the variables of the class 

  string ConfigFile; /**< The file path of the predictor definition */

  /*Common fileds for all the predictors */
  Predictor_type_t predictorType; /**< Indicates the predictor type defined in the configuration file  */
  DeadlineMissManager_t preDeadlineManager; /**< Indicates the type of pre deadline mananager for the predictor */
  DeadlineMissManager_t postDeadlineManager; /**< Indicates the type of the post deadline manager for the predictor */
  
  bool fileLoaded; /**< Indicates if the configuration file has been loaded correctly */
  
  
  
  
  /* STUFF REGARDING THE HISTORICAL CLASSIFIER PREDICTOR STUFF */
  
  classifier_t classifierType; /**< Defines the type of classifier the is implemented, see classifier_t enum definition.*/ 
  discretizer_t discretizer; /**< Defines the discretization method to be applied to the continous values */
  int numberOfBins; /**< Defines the number of bins to be used in the discretization method*/  
  
  
  vector<string>*  ResponseJobAttributes; /** The vector containing the jobs variables that will be used for construct the prediction model */
  bool NewFilePerModel; /**< If true, all the models generated for carrying out the prediction will be dump in a separate filed  */
  int ModelIntervalGeneration; /**< A double indicating the amount of job finished between two different model generations  */
  string PredictionVariable; /** The variable to be predicted, it should be the runtime in almost all the cases, however we leave it free just in case the developed would like to use the predictor for any other goal */
  string ModelDirectoryFile; /**< The dir where all the predictor weka files will be dumped  */
  
 
  
private:
  int containsXPath(string XPath,DOMSupport &theDOMSupport, XalanNode *theContextNode, const PrefixResolver &thePrefixResolver, XPathEvaluator &theEvaluator);
  void extractHistoricalClassifierFields(DOMSupport &theDOMSupport, XalanNode *theContextNode, const PrefixResolver &thePrefixResolver, XPathEvaluator &theEvaluator);
  
  classifier_t getClassifierType(string strClassifier);
  discretizer_t getDiscretizerType(string strDiscretizer);
  DeadlineMissManager_t getDeadLineManager(string strManager);

};

}

#endif
