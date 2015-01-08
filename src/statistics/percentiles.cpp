#include <statistics/percentiles.h>
#include <utils/utilities.h>
#include <math.h>

#include <algorithm>

namespace Statistics {

/**
 * The default constructor for the class - by default the interval is 10 (in the future it may be changed)
 */
Percentiles::Percentiles()
{
  this->values = NULL;
  this->interval = 10;
}

/**
 * The default destructor for the class 
 */
Percentiles::~Percentiles()
{
}

/**
 * Function that computes the percentiles of all the values stored in the vector
 * @return Metric with the Percentiles as a string 
 */
Simulator::Metric* Percentiles::computeValue()
{
  double count = 0;
  double dvalue;
  
  string percentiles = "";
  
  //then we compute all the percentiles 
  for(double curPerc = interval ; curPerc <= 100; curPerc+=interval)
  {
    /* first we sort all the elements of the vector */
    if ( this->values->size() > 0 )
    {
      sort(this->values->begin(), this->values->end());  
      dvalue =*(this->values->begin()+int(this->values->size()*min((double)99,curPerc/100)));
    }
    else
      dvalue = 0;

    percentiles +="P("+ftos(curPerc)+")="+ftos(dvalue)+";";
  }  
  
  Simulator::Metric* performance = new Simulator::Metric();
  performance->setStatisticUsed(Simulator::PERCENTILES);    
  performance->setNativeString(percentiles);
  performance->setNativeType(Simulator::STRING);
  
  return performance;
  
}


/**
 * Returns a reference to the vector that holds all the values to whom the stimator will be computed 
 * @return The reference to the vector 
 */
vector< double >* Percentiles::getvalues() const
{
  return values;
}


/**
 * Sets the reference to the vector that holds all the values to whom the stimator will be computed  
 * @param theValue The reference to the vector 
 */
void Percentiles::setValues(vector< double >* theValue)
{
  values = theValue;
}

}
