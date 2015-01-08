#include <statistics/statisticalestimator.h>

namespace Statistics {

/**
 * The default constructor for the class
 */
StatisticalEstimator::StatisticalEstimator()
{
}


/**
 * The default destructor for the class 
 */
StatisticalEstimator::~StatisticalEstimator()
{
}


/**
 * Returns the metric type of the values that are stored in the estimator
 * @return The estatistic type
 */
Simulator::metric_t StatisticalEstimator::getmetricType() const
{
  return metricType;
}


/**
 * Sets the metric type of the values that are stored in the estimator
 * @param theValue The metric type for the current statisticalestimator
 */
void StatisticalEstimator::setMetricType(const Simulator::metric_t& theValue)
{
  metricType = theValue;
}


/**
 * Returns the statisticType type that the current statisticalestimator implements 
 * @return The statisticType implemented 
 */
Simulator::statistic_t StatisticalEstimator::getstatisticType() const
{
  return statisticType;
}


/**
 * Sets the type that the current statisticalestimator implements 
 * @param theValue 
 */
void StatisticalEstimator::setStatisticType(const Simulator::statistic_t& theValue)
{
  statisticType = theValue;
}

}

