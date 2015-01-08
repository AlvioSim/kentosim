#ifndef SIMULATOREASYPREDSCHEDULINGPOLICY_H
#define SIMULATOREASYPREDSCHEDULINGPOLICY_H

#include <scheduling/easyschedulingpolicy.h>
#include <predictors/predictor.h>

using namespace PredictionModule;

namespace Simulator {

/**
* @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

/**
* This class implements the EASY scheduling policy but that uses prediction techniques for the runtime rather than the user estimates. It inherits all methods from the EASYSchedulingPolicy.
* @see The class EASYSchedulingPolicy
*/
class EASYPredSchedulingPolicy: public EASYSchedulingPolicy{
public:
  EASYPredSchedulingPolicy(ArchitectureConfiguration* theValue,Log* log,double globaltime,Backfilling_variant_t backfillingType);
  EASYPredSchedulingPolicy();
  ~EASYPredSchedulingPolicy();

  /* iherited functions from the SchedulingPolicy class */  
  virtual void jobAdd2waitq(Job* job); //Adds a given job to the wait queue, usually when it arrives to the system
  virtual void jobRemoveFromRT(Job* job); //Removes a job from the RT
  virtual void jobStart(Job* job); //Starts a job    
  
  
  void deadlineMissed(Job* job); 
  
  //sets and gets 
  void setPredictor(Predictor* theValue);
  Predictor* getpredictor() const;
  void setPredictorTypeUsed(const Predictor_type_t& theValue);
  Predictor_type_t getpredictorTypeUsed() const;
	

private:
  double computeRuntimeExtension(Job* job); 
  double predictRequestedRuntime(Job* job);

  Predictor_type_t predictorTypeUsed; /**< Indicates the predictor type used in the current simulation for estimate the job runtime  */
  Predictor* predictor;/**< Contains a reference to an object of type predictor, @see class predictor */
};

}

#endif
