#ifndef STATISTICSPERCENTILE95_H
#define STATISTICSPERCENTILE95_H

#include <statistics/statisticalestimator.h>

#include <vector>

using namespace std;
using std::vector;

namespace Statistics  {

/**
* @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

/** 
* Implements the statistic of Percentile95- given a set of values returns the 95th Percentile 
* @see the statisticalestimator class
*/
class Percentile95 : public StatisticalEstimator{
public:
  Percentile95();
  ~Percentile95();
  
  virtual Simulator::Metric* computeValue();	
  void setValues(vector< double >* theValue);
  vector< double >* getvalues() const;
	
private:
 vector<double>* values;

};

}

#endif
