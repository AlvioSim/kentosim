#include <statistics/standarddeviation.h>

#include <statistics/average.h>

#include <algorithm>
#include <math.h>

namespace Statistics {


/**
 * The default constructor for the class
 */
StandardDeviation::StandardDeviation()
{
  this->values = NULL;
}

/**
 * The default destructor for the class 
 */
StandardDeviation::~StandardDeviation()
{
}

/**
 * Function that computes the standard deviation of all the values stored in the vector
 * @return Metric with the standard deviation
 */
Simulator::Metric* StandardDeviation::computeValue()
{
  double count = 0;
  double dvalue;
  
  /* first we compute the mean */
  Average avg;
  avg.setValues(this->values);
   
  double mean = avg.computeValue()->getnativeDouble();
    
  double sum_of_square_differences = 0;
  for ( vector<double>::iterator iter = this->values->begin();
	iter != this->values->end();
	++iter)
  {
      const double this_square_difference = *iter - mean;
      sum_of_square_differences += this_square_difference * this_square_difference;
  }
  
  dvalue = sqrt( sum_of_square_differences / ( this->values->size() - 1 ));  
    
  
  Simulator::Metric* performance = new Simulator::Metric();
  performance->setStatisticUsed(Simulator::STDEV);    
  performance->setNativeValue(dvalue);
  performance->setNativeType(Simulator::DOUBLE);
  
  return performance;
  
}



/**
 * Returns a reference to the vector that holds all the values to whom the stimator will be computed 
 * @return The reference to the vector 
 */
vector< double >* StandardDeviation::getvalues() const
{
  return values;
}


/**
 * Sets the reference to the vector that holds all the values to whom the stimator will be computed  
 * @param theValue The reference to the vector 
 */
void StandardDeviation::setValues(vector< double >* theValue)
{
  values = theValue;
}


}
