#ifndef PREDICTIONMODULEPREDDEADLINEMISSMANAGERGRADUAL_H
#define PREDICTIONMODULEPREDDEADLINEMISSMANAGERGRADUAL_H


#include <predictors/preddeadlinemissmanager.h>

#include <vector>

using namespace std;
using std::vector;

namespace PredictionModule {

/**
@author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>	  
*/

/**
* This a pre deadlineMiss manager that behaves as follows 
* If deadline was already missed n times, then the next prediction
* would be to add GRADUAL[n] to the current elapsed (while honoring
* the estimate postcondition.-	
*/
class PredDeadlineMissManagerGradual  : public PredDeadlineMissManager {
public:
  PredDeadlineMissManagerGradual();
  ~PredDeadlineMissManagerGradual();
  
  virtual double deadlineMiss(Job* job, double elapsed, double estimate, int    miss_count);    
  
private:
  vector<double> increment; /**< The factors that will be applied to the prediction */
 
    
};

}

#endif
