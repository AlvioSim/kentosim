#ifndef STATISTICSACCUMULATELITERALS_H
#define STATISTICSACCUMULATELITERALS_H


#include <statistics/statisticalestimator.h>
#include <map>
#include <vector>

using namespace Simulator;
using namespace std;
using std::vector;
using std::map;

namespace Statistics {

/**
* @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>*/

/**
* Implements the statistic of Accumulate- given a set of pairs -string,double- returns the adding.group by the string 
* @see the statisticalestimator class
*/
class AccumulateLiterals{
public:
  AccumulateLiterals();
  ~AccumulateLiterals();

 virtual Simulator::Metric* computeValue();	
 void setSPValues(map<string,vector<double>*>* theValue);
 map<string,vector<double>*>* getSPvalues() const;
	
private:
 map<string,vector<double>*>* valuesSP; /**<  The reference to the vector that hols all the values to whom the stimator will be computed  */
 statistic_t accumulateToCompute; /**<  The type of statisticalestimator that will be applied per group */
 
};

}

#endif
