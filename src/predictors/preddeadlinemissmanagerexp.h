#ifndef PREDICTIONMODULEPREDDEADLINEMISSMANAGEREXP_H
#define PREDICTIONMODULEPREDDEADLINEMISSMANAGEREXP_H

#include <predictors/preddeadlinemissmanager.h>

namespace PredictionModule {

/**
* @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

/** A preddeadlinemissmanager that increments the prediction in a exponential factor*/
class PredDeadlineMissManagerExp : public PredDeadlineMissManager{
public:
  PredDeadlineMissManagerExp();
  PredDeadlineMissManagerExp(double factor);
  ~PredDeadlineMissManagerExp();
  
  virtual double deadlineMiss(Job* job, double elapsed, double estimate, int    miss_count);    

private:
  double factor; /**< the exponent factor (>1)*/

};

}

#endif
