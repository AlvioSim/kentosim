#include <statistics/average.h>

#include <algorithm>
#include <numeric>

namespace Statistics {

/**
 * The default constructor for the class
 */
Average::Average()
{
  this->values = NULL;
}

/**
 * The default destructor for the class 
 */
Average::~Average()
{
}


/**
 * Function that compute the average of all the values stored in the vector
 * @return Metric with the average
 */
Simulator::Metric* Average::computeValue()
{
  double count = 0;
  double dvalue = accumulate(this->values->begin(),this->values->end(),0.0) /double(this->values->size());
  
  Simulator::Metric* performance = new Simulator::Metric();
  performance->setStatisticUsed(Simulator::AVG);    
  performance->setNativeValue(dvalue);
  performance->setNativeType(Simulator::DOUBLE);
  
  return performance;
  
}


/**
 * Returns a reference to the vector that holds all the values to whom the stimator will be computed 
 * @return The reference to the vector 
 */
vector< double >* Average::getvalues() const
{
  return values;
}

/**
 * Sets the reference to the vector that holds all the values to whom the stimator will be computed  
 * @param theValue The reference to the vector 
 */
void Average::setValues(vector< double >* theValue)
{
  values = theValue;
}

}
