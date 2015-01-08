#include <predictors/prediction.h>
#include <assert.h>

namespace PredictionModule {

/**
 * The constructor for the prediction
 * @param prediction The prediction
 * @param job The job to whom concers the prediction 
 */
Prediction::Prediction(double prediction,Job* job)
{
  this->prediction = prediction;
  this->job = job;
}


/**
 * The destructor for the class
 */
Prediction::~Prediction()
{
}

/**
 * Returns the job to who concers the prediction
 * @return A reference to the job 
 */
Job* Prediction::getjob() const
{
  return job;
}


/**
 * Setts the job to whom concerst the prediction
 * @param theValue A reference to the job
 */
void Prediction::setJob(Job* theValue)
{
  job = theValue;
}


/**
 * Returns the prediction
 * @return A double with the prediction 
 */
double Prediction::getprediction() const
{
  return prediction;
}


/**
 * Sets the prediction for the job 
 * @param theValue A double containing the prediction 
 */
void Prediction::setPrediction(double theValue)
{
  prediction = theValue;
}

}
