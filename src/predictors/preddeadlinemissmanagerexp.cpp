#include <predictors/preddeadlinemissmanagerexp.h>

#include <algorithm>
#include <math.h>
#include <assert.h>

using namespace std;

namespace PredictionModule {

/**
 * The default constructor for the pre missmanager 
 */
PredDeadlineMissManagerExp::PredDeadlineMissManagerExp()
{
}

/**
 * The constructor of the deadlineMiss pre manager 
 * @param factor The factor that will be applied to the past estimation of the jobs for return the new one 
 */
PredDeadlineMissManagerExp::PredDeadlineMissManagerExp(double factor)
{
  this->factor = factor;
  assert(factor>1);
}

/**
 * The destructor for the premanager 
 */
PredDeadlineMissManagerExp::~PredDeadlineMissManagerExp()
{
}

/**
 * 
 * @param job The job about who concerns the estimation
 * @param elapsed The elapsed time for the job
 * @param estimate The estimation for the job 
 * @param miss_count The times that the estimation for the job has been wrong
 * @return The new prediction for the job runtime 
 */
double PredDeadlineMissManagerExp::deadlineMiss(Job* job, double elapsed, double estimate, int  miss_count)
{     
  preCond (job, elapsed, estimate, miss_count);
 
  double pred;
	
  if( elapsed > 1.0)
    pred = pow( elapsed, this->factor );
  else if(elapsed == 1.0)
    pred = 2.0;
  else 
  {
    assert(elapsed<1.0);
    pred = 1.0;
  }
	    
  double ret  = elapsed<estimate ? min(pred,estimate) : pred;

  assert( ret > elapsed);
	
  postCond(job, elapsed, estimate, miss_count, ret);
   
  return ret;
}


}
