#include <datamining/wekac45.h>
#include <assert.h>

namespace Datamining {

/**
 * The constructor for the class 
 * @param attributes A vector of string containing the names for the attributes that will be used for create the model 
 * @param attributesTypes A vector of native_t indicating the type of each attribute used for constructing the tree. 
 * @param ModelFilePath A string indicating the directory path where store the weka models 
 * @param NewFilePerModel A bool indicating if the different models have to be stored in different files 
 * @param type A classifier_t indicating the type of classifier implemented
 * @param Log A reference to the logging engine 
 * @param numberOfBins The number of bins to be used in the discretization method, -1 means that will be automatically computed   
 */
WekaC45::WekaC45(vector<string>* attributes, vector<native_t>* attributesTypes,string ModelFilePath,bool NewFilePerModel, Log* log,discretizer_t discretizer,int numberOfBins) : WekaClassifier(attributes,attributesTypes,ModelFilePath,NewFilePerModel,C45,log,discretizer,numberOfBins)
{
  this->WekaClassificationParameters =  " -p 0 ";
  this->WekaClassificationClass = "weka.classifiers.trees.J48" ;
  
  this->WekaModelGenerationParameters = "-C 0.25 -M 2" ;
  this->WekaModelGenerationClass = "weka.classifiers.trees.J48";
}

/**
 * The default constructor for the class 
 */
WekaC45::~WekaC45()
{
}


}
