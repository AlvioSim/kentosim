#include <statistics/percentile95.h>

#include <algorithm>

namespace Statistics {

/**
 * The default constructor for the class
 */
Percentile95::Percentile95()
{
  this->values = NULL;
}

/**
 * The default destructor for the class 
 */
Percentile95::~Percentile95()
{
}

/**
 * Function that computes the 95th Percentile of all the values stored in the vector
 * @return Metric with the 95Percentile 
 */
Simulator::Metric* Percentile95::computeValue()
{
  double count = 0;
  double dvalue;
  
  if ( this->values->size() > 0 )
  {
    /* first we sort all the elements of the vector */
    sort(this->values->begin(), this->values->end());  
    dvalue =*(this->values->begin()+int(this->values->size()*0.95));
  }
  else
    dvalue = 0;
    
  
  Simulator::Metric* performance = new Simulator::Metric();
  performance->setStatisticUsed(Simulator::PERCENTILE95);    
  performance->setNativeValue(dvalue);
  performance->setNativeType(Simulator::DOUBLE);
  
  return performance;
  
}


/**
 * Returns a reference to the vector that holds all the values to whom the stimator will be computed 
 * @return The reference to the vector 
 */
vector< double >* Percentile95::getvalues() const
{
  return values;
}

/**
 * Sets the reference to the vector that holds all the values to whom the stimator will be computed  
 * @param theValue The reference to the vector 
 */
void Percentile95::setValues(vector< double >* theValue)
{
  values = theValue;
}

}
