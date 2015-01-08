#ifndef STATISTICSMAX_H
#define STATISTICSMAX_H

#include <statistics/statisticalestimator.h>

#include <vector>

using namespace std;
using std::vector;


namespace Statistics {

/**
* @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

/**
* Implements the statistic of max- given a set of values returns the maximmum of all the values. Can be applied to vectors of integers or doubles 
* @see the statisticalestimator class
*/
class Max{
public:
  Max();
  ~Max();
  
  virtual Simulator::Metric* computeValue();	
  void setValues(vector< double >* theValue);
  void setValues(vector< int >* theValue);
  vector< double >* getvalues() const;
  void setDvalues(vector< double >* theValue);
  vector< double >* getdvalues() const;
  void setIvalues(vector< int >* theValue);
  vector< int >* getivalues() const;
	
	

private:
 vector<double>* dvalues;  /**<  The reference to the vector of doubles that hols all the values to whom the stimator will be computed  */
 vector<int>* ivalues;  /**<  The reference to the vector of integers that hols all the values to whom the stimator will be computed  */

};

}

#endif
