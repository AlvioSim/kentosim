#include <datamining/classifier.h>
#include <utils/utilities.h>
#include <assert.h>

namespace Datamining {

/**
 * The constructor for the class. IMPORTANT ! The last attribute in the list will be the variable to classify  
 * @param attributes A vector of string containing the names for the attributes that will be used for create the model 
 * @param attributesTypes A vector of native_t indicating the type of each attribute used for constructing the tree. 
 * @param Log A reference to the logging engine 
 * @param discretizer The discretization method to be used with the continue values  
 * @param numberOfBins The number of bins to be used in the discretization method, -1 means that will be automatically computed 
 */
Classifier::Classifier( vector<string>* attributes, vector<native_t>* attributesTypes,Log* log,discretizer_t discretizer,int numberOfBins)
{
  this->attributes = attributes;
  this->attributesTypes = attributesTypes;
  this->discretizer = discretizer;
  this->numberOfBins = numberOfBins;
  
  //Checking that both have the same size
  log->debug("Creating a classifier "+itos(attributes->size())+" "+itos(attributesTypes->size()),2);
  assert(attributes->size() == attributesTypes->size());
  
  this->log = log;
}


/**
 * The destructor for the class 
 */
Classifier::~Classifier()
{
 delete this->attributes;
 delete this->attributesTypes;
}

/**
 * Retruns the classifier type implemented by the current class 
 * @return A classifier_t indicating the classifier type implemented 
 */
classifier_t Classifier::getclassifierType() const
{
  return classifierType;         
}


/**
 * Sets the classifier type implementented by the current class 
 * @param theValue The classifier type
 */
void Classifier::setClassifierType(const classifier_t& theValue)
{
  classifierType = theValue;
}

/**
 * Given a classifier_t type returns an string containing a human readable classifier type
 * @param classifier The classifier type 
 * @return A string containing a human readable classifierType.
 */
string Classifier::getClassifierName(classifier_t classifier)
{
  switch(classifier)
  {
    case ID3:
      return "ID·";
    case C45:
      return "C45";
    case OTHER_CLASSIFIER:
      return "OTHER_CLASSIFIER";
    default:
      assert(false);
  }
}

}
