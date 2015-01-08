#include <statistics/accumulate.h>

#include <algorithm>
#include <numeric>

namespace Statistics {

/**
 * The default constructor for the class
 */
Accumulate::Accumulate()
{
  this->values = NULL;
}

/**
 * The default destructor for the class 
 */
Accumulate::~Accumulate()
{
}


/**
 * Function that computes the accumulate of all the values stored in the vector
 * @return Metric with the accumulate
 */
Simulator::Metric* Accumulate::computeValue()
{
  double dvalue = accumulate(this->values->begin(),this->values->end(),0.0);
  
  Simulator::Metric* performance = new Simulator::Metric();
  performance->setStatisticUsed(Simulator::ACCUMULATE);    
  performance->setNativeValue(dvalue);
  performance->setNativeType(Simulator::DOUBLE);
    
  return performance;
  
}



/**
 * Returns a reference to the vector that holds all the values to whom the stimator will be computed 
 * @return The reference to the vector 
 */
vector< double >* Accumulate::getvalues() const
{
  return values;
}


/**
 * Sets the reference to the vector that holds all the values to whom the stimator will be computed  
 * @param theValue The reference to the vector 
 */
void Accumulate::setValues(vector< double >* theValue)
{
  values = theValue;
}



}
