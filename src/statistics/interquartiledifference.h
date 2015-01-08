#ifndef STATISTICSINTERQUARTILEDIFFERENCE_H
#define STATISTICSINTERQUARTILEDIFFERENCE_H

#include <statistics/statisticalestimator.h>

#include <vector>

using namespace std;
using std::vector;

namespace Statistics {

/**
* @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

/** 
  * Implements the statistic of InterquartileDifference- given a set of values returns the interquartile difference (computed as the 75th percentile - 25th percentile)
  * @see the statisticalestimator class */
class InterquartileDifference : public StatisticalEstimator{
public:
  InterquartileDifference();
  ~InterquartileDifference();

 virtual Simulator::Metric* computeValue();	
 void setValues(vector< double >* theValue);
 vector< double >* getvalues() const;
	
private:
 vector<double>* values; /**<  The reference to the vector that hols all the values to whom the stimator will be computed  */
 
};

}

#endif
