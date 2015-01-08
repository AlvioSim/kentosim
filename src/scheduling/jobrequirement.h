#ifndef SIMULATORJOBREQUIREMENT_H
#define SIMULATORJOBREQUIREMENT_H

#include <scheduling/metric.h>

/** This enums indicates the available operators that can be associated to a job requirement */
enum operator_t {
    LESS_THAN=0,
    EQUAL,	    	 
    HIGHER_THAN,
    LESS_EQ_THAN,
    HIGHER_EQ_THAN,
    NONE
};


namespace Simulator {

/**
	@author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

/** 
This class conatins information about the job requirements. Note that a job requirement is basically grid oriented. It allows to specify the requirement of a given resource and the operator.
*/


class JobRequirement{
public:
  JobRequirement();
  ~JobRequirement();

  void setOperatorRequired ( const operator_t& theValue );
  operator_t getoperatorRequired() const;
  void setMetric ( Metric* theValue );
  Metric* getmetric() const;
	

private:
  operator_t operatorRequired; /**< The operator used for specify the requirement */
  Metric* metric; /**< The metric that contains the value related to the requiment  */

};

}

#endif
