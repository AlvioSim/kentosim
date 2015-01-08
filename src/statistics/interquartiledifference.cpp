#include <statistics/interquartiledifference.h>

#include <algorithm>

namespace Statistics {

/**
 * The default constructor for the class
 */
InterquartileDifference::InterquartileDifference()
{
  this->values = NULL;
}

/**
 * The default destructor for the class 
 */
InterquartileDifference::~InterquartileDifference()
{
}



/**
 * Function that compute the Interquartile Difference of all the values stored in the vector
 * @return Metric with the IQR
 */
Simulator::Metric* InterquartileDifference::computeValue()
{
  double count = 0;
  double dvalue;
  
  /* first we sort all the elements of the vector */
  if ( this->values->size() > 0 )
  {
    sort(this->values->begin(), this->values->end());  
    double q1 =*(this->values->begin()+int(this->values->size()*0.25));
    double q2 =*(this->values->begin()+int(this->values->size()*0.75));
    dvalue = q2-q1;
  }
  else
    dvalue = 0;
  
  Simulator::Metric* performance = new Simulator::Metric();
  performance->setStatisticUsed(Simulator::IQR);    
  performance->setNativeValue(dvalue);
  performance->setNativeType(Simulator::DOUBLE);
  
  return performance;
  
}


/**
 * Returns a reference to the vector that holds all the values to whom the stimator will be computed 
 * @return The reference to the vector 
 */
vector< double >* InterquartileDifference::getvalues() const
{
  return values;
}

/**
 * Sets the reference to the vector that holds all the values to whom the stimator will be computed  
 * @param theValue The reference to the vector 
 */
void InterquartileDifference::setValues(vector< double >* theValue)
{
  values = theValue;
}


}
