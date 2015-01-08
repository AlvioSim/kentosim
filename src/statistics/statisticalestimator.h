#ifndef STATISTICSSTATISTICALESTIMATOR_H
#define STATISTICSSTATISTICALESTIMATOR_H

#include <scheduling/metric.h>


namespace Statistics {

/**
* @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
* This class implements an statistical estimator. Given a set of values it  will compute a given statistic.
*/
class StatisticalEstimator{
public:
  StatisticalEstimator();
  ~StatisticalEstimator();
  void setMetricType(const Simulator::metric_t& theValue);
  Simulator::metric_t getmetricType() const;
  void setStatisticType(const Simulator::statistic_t& theValue);
  Simulator::statistic_t getstatisticType() const;
  /**
   * Function that computes the statistic to the current set values 
   * @return A reference to the metric class that contains the statistic value (the metric object should be destroyed by the object that requires it)
   */
  virtual Simulator::Metric* computeValue() = 0;	

private:
  Simulator::metric_t metricType; /**< the metric type @see the Metric Class for the simulator (the semantic type for the date being computed ) */
  Simulator::statistic_t statisticType; /**< The statistic type that the statististic implements, @see Metric class  */

};

}

#endif
