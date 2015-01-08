#ifndef STATISTICSPERCENTILES_H
#define STATISTICSPERCENTILES_H

#include <statistics/statisticalestimator.h>

#include <vector>

using namespace std;
using std::vector;

namespace Statistics  {

/**
* @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

/**
* Implements the statistic of Percentiles- given a set values returns the percentiles from [0..100] incrementing by interval value. For example if the interval is 20: P(20)=2;P(40)=45;...P(100) =12232
* @see the statisticalestimator class
*/
class Percentiles : public StatisticalEstimator{
public:
  Percentiles();
  ~Percentiles();
  
  virtual Simulator::Metric* computeValue();	
  void setValues(vector< double >* theValue);
  vector< double >* getvalues() const;
	
private:
  vector<double>* values; /**<  The reference to the vector that hols all the values to whom the stimator will be computed  */
  double interval; /**< Indicates the lenght of each percentile to be computed */
  
};

}


#endif
