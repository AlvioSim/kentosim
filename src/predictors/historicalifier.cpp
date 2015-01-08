#include <predictors/historicalifier.h>
#include <utils/utilities.h>

#include <predictors/preddeadlinemissmanagergradual.h>

#include <datamining/wekac45.h>
#include <datamining/wekaid3.h>

namespace PredictionModule {

/**
 * The constructor for the prediction , the class creates ad Gradual deadline post manager and a Gradual deadline pre manager  
 * @param classifierType The classifier tpye that will be used for classify the job 
 * @param discretizer The classifier type that will be used for discretizing the continous variables  
 * @param numberOfBins The number of bins to be used in the discretization, -1 means that the number of bins will be automaticall  computed  
 * @param ResponseVariables The variables that will be used for creating the prediction model 
 * @param ResponseJobAttributesType The type for each of the variables of the ResponseVariables
 * @param ModelDirectoryFile The directory where the output model fields will be output 
 * @param NewFilePerModel Indicates if a new file has to be created per model
 * @param ModelIntervalGeneration A double indicating the amount of job finished between two different model generations 
 * @param PredictionVariable The string containing the prediction
 * @param PredictionVariableType Indicates the type for the predicton variable
 */
HistoricalClassifier::HistoricalClassifier(classifier_t classifierType,discretizer_t discretizer,int numberOfBins,Log* log,vector<string>* ResponseJobAtts,vector<native_t>* ResponseJobAttsType,string ModelDirectoryFile,bool NewFilePerModel, int ModelIntervalGeneration, string PredictionVariable, native_t PredictionVariableType) : Predictor(log)
{
  this->type = HISTORICAL_CLASSIFIER;
  //by default it has a exp pred deadline exp manager 
  this->preManager = new PredDeadlineMissManagerGradual();
  this->postManager = new PredDeadlineMissManagerGradual();

  this->classifierType = classifierType;
  this->discretizer = discretizer;
  this->numberOfBins = numberOfBins;
  
  this->NewFilePerModel = NewFilePerModel;
  this->ModelIntervalGeneration = ModelIntervalGeneration;
  this->ModelDirectoryFile = ModelDirectoryFile;
  
  this->PredictionVariable = PredictionVariable;
  this->PredictionVariableType = PredictionVariableType;    
 
  this->ResponseJobAttributesType = new vector<native_t>();
  this->ResponseJobAttributes = new vector<string>();
  
  *this->ResponseJobAttributes = *ResponseJobAtts;
  *this->ResponseJobAttributesType = *ResponseJobAttsType;
  
  this->ResponseJobAttributes->push_back(PredictionVariable);
  this->ResponseJobAttributesType->push_back(PredictionVariableType);
  
  switch(classifierType) 
  {
    case ID3:
      this->classifier = new WekaID3(this->ResponseJobAttributes,this->ResponseJobAttributesType,ModelDirectoryFile,NewFilePerModel, log, discretizer,numberOfBins);      
      break;
    case C45:
      this->classifier = new WekaC45(this->ResponseJobAttributes,this->ResponseJobAttributesType,ModelDirectoryFile,NewFilePerModel, log, discretizer,numberOfBins);
      break;
    default:
      assert(false);
  }
  
  this->NoInformationPrediction = 60; 
  this->NumberOfInstances = 0;
  this->NumberOfLastInstances = 0;
  this->ModelInitialized = false;
}

/**
 * The destructor for the class
 */
HistoricalClassifier::~HistoricalClassifier()
{
}


/**
 * This function can be used for estimate the performance variable, however the job is not being queued necesarily.
 * @param job The job that has to predicted
 * @return The prediction for the job runtime 
*/
Prediction* HistoricalClassifier::estimateJobPerformance(Job* job) 
{
  vector<string>* instance = new vector<string>();
    
  double prediction = this->NoInformationPrediction;
   
  if(this->ModelInitialized)
  {
    //we add the response values but not the predction variable 
    //but not the prediction variable since it is added as "?" in the wekaclassifier
  
    int processedAtts = 0;
    
    for(vector<string>::iterator it = this->ResponseJobAttributes->begin();
      it != this->ResponseJobAttributes->end() && processedAtts < this->ResponseJobAttributes->size()-1;
      ++it)
    {
      string attribute = *it;     
      instance->push_back(job->getAttributeValue(attribute));
      processedAtts++;
    }


    
   string classification = classifier->classify(instance);

   if(classification == "")
   {
     //in case no apporpiate prediction is returned by the classifier we retunr the default prediction. 
     prediction = this->NoInformationPrediction;
   }
   else
   {
     //The following formats can be returned by the classifier ..
     //  (-inf-XXXX] -> type 1  .. 
     //  (XXX-XXXX]  -> type 2 
     //  (XXXX-inf)  -> type 3
     //where the XXX is a string containing a double 
     string::size_type loc = classification.find("inf", 0 );
     string::size_type start;
     string::size_type end ;
      
     //as we want avoid underestimate the job runtime , the prediction may differ ..
     if(loc == string::npos)  //type 2
     {
       //we return the uppee bound 
       start = classification.find("-",0)+1;
       end  = classification.find("]",0);       
     }
     else if(loc < 4) //type 1
     {
       start = classification.find("-",3)+1;
       end  = classification.find("]",0);  
     }
     else //type 3
     {
       //we return the lower bound 
       start = classification.find("(",0)+1;
       end  = classification.find("-",0);          
     }
   
     string selectedPrediction = classification.substr(start,end-start);
   
     prediction = atoi(selectedPrediction.c_str());
    }
  }
  //create the prediction and return its 
  Prediction* pred = new Prediction(prediction,job);
  return pred;
}

/**
 * Function that initialize the parameters required for invoquing the weka command line interface
 * @param JavaBinary String containing the file path for the java binary file 
 * @param classpath String containing the class path for the java
 * @param wekaPath String containing the path for the weka jar file 
 * @param TemporaryDir String containing the path for the temporary 
 */
void HistoricalClassifier::setWekaEnv(string JavaBinary,string classpath,string wekaPath,string TemporaryDir)
{
 ((WekaObject*)this->classifier)->setWekaEnv(JavaBinary, classpath, wekaPath,TemporaryDir);
}



  /**
   * Invoked immediately upon the job's arrival, when it is placed in the waitqueue. `pred_v' will contain at least one runtime prediction for the given job.
   * @param job The job that has arrived
   * @return The prediction for the job runtime 
   */
Prediction* HistoricalClassifier::jobArrived(Job* job)
{
  return this->estimateJobPerformance(job);
}

  /**
   * Invoked when a job is scheduled to run, i.e. when it is moved from thewaitqueue to the runqueue.
   * @param job The job that has started
   * @param time The startime for the job 
   */
void HistoricalClassifier::jobStarted(Job* job, double time)
{
  //Nothing to do ..
}

  /**
   * The given RUNNING job has just exceeded its prediction (i.e. the previous
   * prediction was too short).
   * Produce a new prediction to this job (and possibly to other jobs which
   * are effected) using the appropriate missed-deadlines manager ("pre" if
   * the current missed prediction is smaller than the user estimate; "post"
   * otherwise).  
   * Need to generate a new (longer) estimate at least for this job, and
   * possibly to others. This means that after this function is finished,
   * pred_v will contain at least one Pred_t associated with job_id.
   * 
   * @param job The job that has reached the deadline 
   * @param time The time when the deadline has been reached 
   * @return 
   */
Prediction* HistoricalClassifier::jobDeadlineMissed(Job* job, double     time)
{
  //first we increment the deadline misses for this job     
  double elapsed = time-job->getJobSimStartTime();
  double uest = job->getOriginalRequestedTime();
  int cnt = this->updateMissedCountJob(job);

  assert(cnt >= 0 && elapsed > 0);
  
  double new_prdct = postManager->deadlineMiss(job,elapsed,uest,cnt);
  
  return new Prediction(new_prdct,job);
  
}

  /**
   * 
   * The given RUNNING job has just terminated. This information may (or may
   * not) help in producing more accurate predictions for currently waiting or
   * running jobs. The returned prev_v may be empty (which means that this
   * Predictor_t has decided not to change any previous predictions).
   *   
   * @param job The job that has terminated 
   * @param time The time 
   * @param completed If it has completed or not 
   * @return if no nulll means that no prediction have been changed 
   */
Prediction* HistoricalClassifier::jobTerminated(Job* job, double     time, bool       completed)
{
  //we create the instance information 
  vector<string>* instance = new vector<string>();
  
  for(vector<string>::iterator it = this->ResponseJobAttributes->begin(); it != this->ResponseJobAttributes->end();++it)
  {
    string attribute = *it; 
    
    log->debug("adding attribute "+attribute+" to the instance.",6);
    
    instance->push_back(job->getAttributeValue(attribute));
  }
  
  this->classifier->addInstance(instance);
  
  if(this->NumberOfLastInstances == ModelIntervalGeneration)
  {
    this->classifier->generateModel();
    this->ModelInitialized = true;
  
    NumberOfLastInstances = 0;
  }
  else
    NumberOfLastInstances++;
  
  this->NumberOfInstances++;
}



/**
 * Function that returns the basic prediction used in the classifier predictor
 * @return A double containing the basic prediction 
 */
double HistoricalClassifier::getNoInformationPrediction() const
{
  return NoInformationPrediction;
}


/**
 * Sets the basic prediction. The basic prediction is the constant value that will be returned when the model has not been initialized. The current basic prediction is 60 this may be changed depending on the nature of the prediction variable. A Basic prediction of 60 makes sense in terms of job runtime, however may not for a memory prediction 
 * @param theValue The basic Prediction 
 */
void HistoricalClassifier::setNoInformationPrediction(double theValue)
{
  NoInformationPrediction = theValue;
}

}


