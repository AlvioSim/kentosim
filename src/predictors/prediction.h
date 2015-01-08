#ifndef PREDICTIONMODULEPREDICTION_H
#define PREDICTIONMODULEPREDICTION_H

#include <scheduling/job.h>

using namespace Simulator;

namespace PredictionModule {

/**
* @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

/** This class is a container for a prediction of a given job */
class Prediction{
public:
  Prediction(double prediction,Job* job);
  ~Prediction();

  void setJob(Job* theValue);
  Job* getjob() const;
  void setPrediction(double theValue);
  double getprediction() const;
		
private:
  Job* job; /**< The job to whom the prediction refers*/
  double prediction;/**< the prediction */
  
};

}

#endif
