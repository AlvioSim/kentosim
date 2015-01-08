#include <datamining/wekaclassifier.h>
#include <utils/utilities.h>
#include <datamining/wekaefdiscretizer.h>
#include <datamining/wekaeldiscretizer.h>
#include <assert.h>

namespace Datamining {

/**
 * The constructor for the class 
 * @param attributes A vector of string containing the names for the attributes that will be used for create the model 
 * @param attributesTypes A vector of native_t indicating the type of each attribute used for constructing the tree. 
 * @param ModelFilePath A string indicating the directory path where store the weka models 
 * @param NewFilePerModel A bool indicating if the different models have to be stored in different files 
 * @param type A classifier_t indicating the type of classifier implemented
 * @param discretizer The discretization method to be used with the continue values 
 * @param Log A reference to the logging engine 
 * @param numberOfBins The number of bins to be used in the discretization method, -1 means that will be automatically computed  
 */
WekaClassifier::WekaClassifier(vector<string>* attributes, vector<native_t>* attributesTypes,string ModelFilePath,bool NewFilePerModel,  classifier_t type, Log* log,discretizer_t discretizer,int numberOfBins) :Classifier(attributes,attributesTypes,log,discretizer,numberOfBins), WekaObject(log)
{
  this->ModelFilePath = ModelFilePath;
  this->NewFilePerModel = NewFilePerModel;
  this->classifierType = type;
  
  /* Last Id starts from zero */
  this->LastModelId = 0;
  
  /* The prefix for the model is "model"*/
  this->ModelNamePrefix = this->getClassifierName(type);
  InputFileName = string("inputSet");
  
  /* We initialise the data set */
  this->theSetOfInstances = new InstancesSet();
  
  for(int atts = attributes->size();atts > 0;atts--)
  {
    this->theSetOfInstances->push_back(new vector<string>);
  }
  
  this->numberOfInstances = 0;
  
  /* Regarding weka stuff */
  this->WekaModelGenerationClass = "";
  this->WekaModelGenerationParameters = "";
  this->LastPredictedClasse = "";
  this->countLastPredictedClassUsed = 0;
}


/**
 * The destructor for the class 
 */
WekaClassifier::~WekaClassifier()
{
  //we free the memory used by the set   
  for(InstancesSet::iterator it = this->theSetOfInstances->begin(); it != this->theSetOfInstances->end(); ++it)
  {
    vector<string>* instance = *it;
    delete instance;
  }
  
  delete this->theSetOfInstances;
}

/**
* Adds a new instance to the data model. This is a knowledge of an known attribute classification.
* @param Instances The values foreach of the attributes.
* @return A bool indicating if the instance has been added to the current model or not.
*/
bool WekaClassifier::addInstance(vector<string>* Instance)
{
  this->numberOfInstances++;
  
  //we check that the size of the provided instance is correct accorsing the definition 
  assert(Instance->size() == this->theSetOfInstances->size());
  
  int attProc = 0;
  
  for(vector<string>::iterator it = Instance->begin();it != Instance->end(); ++it)
  {
    string value = *it;
    
    log->debug("Adding the value "+value+" to the attribute "+itos(attProc)+ " instance data set ",6);    
    this->theSetOfInstances->operator[](attProc)->push_back(value);
    attProc++;
  }
}
  
/**
* Given a set of response attribute values for a given instance, the model will classify it to a given class 
* @param ResponseAttributes 
* @return An string containing the classification 
*/
string WekaClassifier::classify(vector<string>* ResponseAttributes)
{
  string predictedClass;

  /* The last one is should not be provided since is the classification*/
  assert(ResponseAttributes->size() == this->attributes->size()-1);
  
  int index = 0;

  vector<string> headerLines;

  string line="";

  for(vector<string>::iterator it = ResponseAttributes->begin(); it != ResponseAttributes->end();++it)
  {
    //here we should go through the attribut type and based on the AtAttributesInterval we should get the correct string interval
    //the discretizer should not be used due to the the provided intervals probably would differ from the once obtained in the model generation , we have saved the interval definition for this purpose. Another option was to generate each time the discretization, but obviously this is not a good solution in terms of performance     
    string attDesc = AttributesInterval[index];

    string value = *it;
    
    /* the real value should be classified in a nominal value taking into account the current model */
    if(attributesTypes->operator[](index) != STRING)
      value = getIntervalFromReal(AttributesInterval[index],value);

    line+=value+",";

    log->debug(attDesc+" and value "+value,3);    
    
    //now we check that the string value is in the attribute definition  
    string::size_type init = attDesc.find("{"+value+",",0); 
    string::size_type mid = attDesc.find(","+value+",",0);  
    string::size_type end = attDesc.find(","+value+"}",0);  
    string::size_type all = attDesc.find("{"+value+"}",0); 

    if(init == string::npos && mid == string::npos && end == string::npos && all == string::npos)
    {
      //the value is not inside the definition .. we must add it to the center
      string newPart = ","+value+"}";
      end = attDesc.find("}",0);
      attDesc.replace( end, newPart.length(), newPart );
    }
    
    headerLines.push_back(attDesc);
    
    index++;
  }
  
  /* we add the response variable as "?" */
  log->debug(AttributesInterval[index],3);
  headerLines.push_back(AttributesInterval[index]);
  line+="?";
  
  /* The fileName will be reused  */
  string InputFile = this->ModelFilePath+"toClassify.arff";

  /* The header is created using the same deffition that the original, but we've to check if new attributes have to be added */
  vector<string> InstanceLine; 
  InstanceLine.push_back(line);

  this->saveArffFile(InputFile,&headerLines,&InstanceLine);
 
  
  /* Now we can invoque the classifier with the input parameter */
  string params = this->WekaClassificationParameters+" -l "+CurrentModel+" -T "+InputFile;    
  vector<string>* prediction = this->InvoqueWekaFunctionality(WekaClassificationClass,params,true);
  
  /* the line should provide the class */
  if(prediction->size() != 1)
  {
    //if not, we provide the last valid prediction 
    countLastPredictedClassUsed++; 
    return this->LastPredictedClasse;
  }

  line = prediction->operator[](0);
  
  deque<string> fields;     
  SplitLine(line,string(" "),fields);
  
  
  //the second element should contain the prediction   
  int i = 0;
  /* we only dump the information in case it is required for debugging purposes */
  for(deque<string>::iterator it = fields.begin(); it != fields.end();++it)
  {
  
    //we iterator over all the fields that are included in the fields .. use the needed once 
    string prediction_item = *it;
    
    switch(i)
    {
      case 1:
        predictedClass = prediction_item;
    }
    
    log->debug("Prediction output field "+prediction_item,3);
    i++;
  }   
  
  //freeing the resource 
  delete prediction;
  
  return predictedClass;
  
}
	
/**
* Given all the instances that have been added the data base stored in the class, the new model will be created.
* @return A bool indicating if the model has been correctly classified.
*/
bool WekaClassifier::generateModel()
{
  assert(WekaModelGenerationClass != "" && WekaModelGenerationParameters != "");
  
  this->CurrentModel = this->ModelFilePath+this->ModelNamePrefix+itos(this->LastModelId)+".model";
  this->CurrentArff = this->ModelFilePath+"inputFile"+itos(this->LastModelId)+".arff";
  
  string NoDiscretized = this->ModelFilePath+"inputFileNoDiscrete"+itos(this->LastModelId)+".arff";
  
  //if a new file has to be generated per model, we increment the last model id 
  if(this->NewFilePerModel)
    this->LastModelId++;
    
  if(this->numberOfInstances ==  0)
  {
    log->error("No instance for create the classifier model ");
    return false;
  }
  
  this->generateArffFile(NoDiscretized,this->theSetOfInstances,this->attributes,this->attributesTypes,numberOfInstances);
   
  //now its time to discretize the continous variables
  vector<string> attributesToDiscretize;
  int index = 0;
  for(vector<string>::iterator it = this->attributes->begin(); it != this->attributes->end();++it) 
  {
    if(this->attributesTypes->operator[](index) != STRING)
    { 
     attributesToDiscretize.push_back(itos(index+1));    
     this->log->debug("Discretizing the attribute"+*it+" with index "+itos(index),3);
    }
     
    index++; 
  }
  
  this->discretizeInputFile(NoDiscretized,this->CurrentArff,&attributesToDiscretize);  
   
  //now its time to construct the parameters 
  //-t points to the input model , and -d to the output model 
  string params = this->WekaModelGenerationParameters+" -t "+CurrentArff+" -d "+CurrentModel;  
  
  this->InvoqueWekaFunctionality(this->WekaModelGenerationClass,params,false);
  this->readAttributesDeffinition(CurrentArff,&this->AttributesInterval);
  
}



/**
 * This function given an input file and a set of attributes to be discretized generates a new arff file with the original arff file but contanining the discretized attributes
 * @param NoDiscretized The file path for the to be discretized 
 * @param CurrentArff The file path for the output file that will contain the discretization 
 * @param attributesToDiscretize The vector of attributes that have to be discretized 
 */
void WekaClassifier::discretizeInputFile(string NoDiscretized,string CurrentArff,vector<string>* attributesToDiscretize)
{
  WekaDiscretizer* disc = NULL;

  switch(this->discretizer)
  {
    case SAME_INTERVAL_DISCRETIZER:
      disc = new WekaELDiscretizer(log,this->ModelFilePath,this->numberOfBins);         
      break;    
    case SAME_INSTANCES_PER_INTERVAL_DISCRETIZER:
      disc = new WekaEFDiscretizer(log,this->ModelFilePath,this->numberOfBins);
      break;
    default:
      assert(false);  
  }
  
  //set the weka envs 
  ((WekaObject*)disc)->setWekaEnv(this->JavaBinary,this->classpath,this->wekaPath,this->TemporaryDir);
  
  //time to discretize the attributtes 
  disc->discretize(NoDiscretized,CurrentArff,attributesToDiscretize);  
}

}
