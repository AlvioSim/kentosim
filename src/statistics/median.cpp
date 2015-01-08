#include <statistics/median.h>

#include <algorithm>

namespace Statistics {

/**
 * The default constructor for the class
 */
Median::Median()
{
  this->values = NULL;
}

/**
 * The default destructor for the class 
 */
Median::~Median()
{
}

/**
 * Function that computes the median of all the values stored in the vector
 * @return Metric with the median
 */
Simulator::Metric* Median::computeValue()
{
  double count = 0;
  double dvalue;
 
  if ( this->values->size() > 0 )
  {
    /* first we sort all the elements of the vector */
    sort(this->values->begin(), this->values->end());  
    dvalue =*(this->values->begin()+this->values->size()/2);
  }
  else
    dvalue = 0;

  
  Simulator::Metric* performance = new Simulator::Metric();
  performance->setStatisticUsed(Simulator::MEDIAN);    
  performance->setNativeValue(dvalue);
  performance->setNativeType(Simulator::DOUBLE);
  
  return performance;
  
}


/**
 * Returns a reference to the vector that holds all the values to whom the stimator will be computed 
 * @return The reference to the vector 
 */
vector< double >* Median::getvalues() const
{
  return values;
}

/**
 * Sets the reference to the vector that holds all the values to whom the stimator will be computed  
 * @param theValue The reference to the vector 
 */
void Median::setValues(vector< double >* theValue)
{
  values = theValue;
}


}
