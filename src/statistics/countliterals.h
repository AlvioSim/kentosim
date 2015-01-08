#ifndef STATISTICSCOUNTLITERALS_H
#define STATISTICSCOUNTLITERALS_H

#include <statistics/statisticalestimator.h>
#include <utils/utilities.h>

#include <vector>

using namespace std;
using std::vector;

namespace Statistics {

/**
* @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

/**
* Implements the statistic of CountLiterals- given a set of values returns the count that each value is repeated. To be applied to string vectors. 
* @see the statisticalestimator class
*/
class CountLiterals : public StatisticalEstimator{
public:
  CountLiterals();
  ~CountLiterals();


 virtual Simulator::Metric* computeValue();	
 void setSValues(vector< string >* theValue);
 vector< string >* getSvalues() const;
	
private:
 vector<string>* values; /**<  The reference to the vector that hols all the values to whom the stimator will be computed  */

};

}

#endif
