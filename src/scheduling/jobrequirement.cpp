#include <scheduling/jobrequirement.h>

namespace Simulator {

JobRequirement::JobRequirement()
{
  this->metric = new Metric();
  this->operatorRequired = NONE;
}


JobRequirement::~JobRequirement()
{
  delete metric;
}

/**
 * Returns the metric that models the job requirement (this includes the type of the requirement and its content) 
 * @return An metric containing the information
 */
Metric* JobRequirement::getmetric() const
{
	return metric;
}

/**
 * Sets the metric that models the job requirement 
 * @param theValue The metric used
 */
void JobRequirement::setMetric ( Metric* theValue )
{
	metric = theValue;
}

/**
 * Returns the operation specified in the requirement
 * @return An operator_t containing metric
 */
operator_t JobRequirement::getoperatorRequired() const
{
	return operatorRequired;
}

/**
 * Sets the operator specified in the requirement
 * @param theValue The metric used
 */
void JobRequirement::setOperatorRequired ( const operator_t& theValue )
{
	operatorRequired = theValue;
}

}

