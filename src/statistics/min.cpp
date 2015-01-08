#include <statistics/min.h>

#include <math.h>

namespace Statistics {

/**
 * The default constructor for the class
 */
Min::Min()
{
  this->dvalues = NULL;
  this->ivalues = NULL;
}

/**
 * The default destructor for the class 
 */
Min::~Min()
{
}

/**
 * Returns a reference to the vector of doubles (if set) that holds all the values to whom the stimator will be computed 
 * @return The reference to the vector of integers 
 */
vector< double >* Min::getdvalues() const
{
  return dvalues;
}

/**
 * Sets the reference to the vector of doubles that holds all the values to whom the stimator will be computed  
 * @param theValue The reference to the vector of doubles 
 */
void Min::setDvalues(vector< double >* theValue)
{
  dvalues = theValue;
}


/**
 * Returns a reference to the vector of integers (if set) that holds all the values to whom the stimator will be computed 
 * @return The reference to the vector 
 */
vector< int >* Min::getivalues() const
{
  return ivalues;
}

/**
 * Sets the reference to the vector of integersthat holds all the values to whom the stimator will be computed  
 * @param theValue The reference to the vector of integers 
 */
void Min::setIvalues(vector< int >* theValue)
{
  ivalues = theValue;
}

/**
 * Function that compute the minimum of all the values stored in the vector
 * @return Metric with the minimum
 */
Simulator::Metric* Min::computeValue()
{
  double count = 0;
  double dvalue;
  double minimum = 0;
  bool set = false;
  
  dvalue = 0;

  if(this->ivalues != NULL)
    for(vector<int>::iterator it = this->ivalues->begin();it != this->ivalues->end();++it)
    {
      double value = (double)*it;
      
      if(!set)
      {
        minimum = value;
        set = true;
      }
      else
      	minimum = min(value,minimum);      
    }
  else
    for(vector<double>::iterator it = this->dvalues->begin();it != this->dvalues->end();++it)
    {
      double value = *it;
      
      if(!set)
      {
        minimum = value;
        set = true;
      }
      else
      	minimum = min(value,minimum);      

    }
       
  Simulator::Metric* performance = new Simulator::Metric();
  performance->setStatisticUsed(Simulator::MIN);    
  performance->setNativeType(Simulator::DOUBLE);
  performance->setNativeValue(minimum);
  
  return performance;
}

}

