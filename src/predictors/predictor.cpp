#include <predictors/predictor.h>

namespace PredictionModule {

Predictor::Predictor(Log* log)
{
 maxEstimate = -1; 
 this->log = log;
}


Predictor::~Predictor()
{
}

double Predictor::getmaxEstimate() const
{
  return maxEstimate;
}


void Predictor::setMaxEstimate(double theValue)
{
  maxEstimate = theValue;
}


PredDeadlineMissManager* Predictor::getpostManager() const
{
  return postManager;
}

 
void Predictor::setPostManager(PredDeadlineMissManager* theValue)
{
  postManager = theValue;
}


PredDeadlineMissManager* Predictor::getpreManager() const
{
  return preManager;
}


void Predictor::setPreManager(PredDeadlineMissManager* theValue)
{
  preManager = theValue;
}

Predictor_type_t Predictor::gettype() const
{
  return type;
}


int Predictor::updateMissedCountJob(Job* job)
{
  map<Job*,int>::iterator jobit = this->missedDeadlineCount.find(job);

  if(jobit ==  this->missedDeadlineCount.end())
  {
    this->missedDeadlineCount.insert(pairJobInt(job,1));
    return 0;
  }
  else  
  { 
    assert(missedDeadlineCount[job] > 0);
    int miss_counts = missedDeadlineCount[job];
    this->missedDeadlineCount[job] = miss_counts+1;
    return miss_counts;
  }
}

/**
 * Returns the number of jobs that have to finish for rebuilt the prediction model 
 * @return A integer conatining the amount of jobs to bee finished 
 */
int Predictor::getModelIntervalGeneration() const
{
  return ModelIntervalGeneration;
}


/**
 * Sets the amount of jobs that have to finish for update the prediction model 
 * @param theValue A integer containing the amount of jobs to be finished 
 */
void Predictor::setModelIntervalGeneration(const int& theValue)
{
  ModelIntervalGeneration = theValue;
}



}
