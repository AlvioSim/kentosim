#ifndef SIMULATORRUA_BACKFILLING_H
#define SIMULATORRUA_BACKFILLING_H

#include <scheduling/easywithcollisionschedulingpolicy.h>
#include <scheduling/metric.h>

namespace Simulator {

/**
	@author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/
class RUABackfilling : public EASYWithCollisionSchedulingPolicy
{

/**
* The class implements the Resource Usage Aware Backfilling policy. For more information about it design and global algorithms, see the thesis document of Francesc Guim - chapter "New Scheduling Strategies for the Resource Usage Optimization"
*/
public:
  RUABackfilling();
  RUABackfilling(ArchitectureConfiguration* theValue,Log* log,double globaltime,Backfilling_variant_t backfillingType);
  ~RUABackfilling();
  void setOptimize ( const metric_t& theValue );
  metric_t getOptimize() const;
	

protected:
  virtual AnalogicalJobAllocation* findAllocation(Job* job, double starttime,double runtime);
  virtual void makeJobReservation(Job* job); 
  virtual Job* pushNextJobReservation(); 
  virtual void rescheduleAllQueuedJobs();
  AnalogicalJobAllocation* evaluateBetterOutCome(Job* job);
  void evaluateBackfillingHints();

private:
  vector<float>* factors; /**< This vector contains the factors that will be evaluated in the backfilling process */
  metric_t Optimize; /**< When the rua is selecting one threshold or another one, can base it desicions on which metric (that considers the wait time and runtime) has to be optimized. In the current version of the rua backfilling it considers two differents metrics to be optimized, the slowdown and the response time. Note that the first metric is basically optimizing both wait time and runtime, while the second one the relevance of the wait time is being normizalized by the runtime. This last once has, in some situations a negligible effect. For instance, in those cases that the wt i 0, the slowdown will be 1 independing on how much the job runtime is penalized. Thus, in many situations seems more logical tu use the response time. */
};

}

#endif
