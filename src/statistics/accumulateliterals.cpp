#include <statistics/accumulateliterals.h>
#include <utils/utilities.h>
#include <statistics/accumulate.h>

#include <algorithm>
#include <map>

using std::map;

namespace Statistics {

/**
 * The default constructor for the class. By default currently the accumulate statisticall estimator is implemented (that will be applied to each group)
 */
AccumulateLiterals::AccumulateLiterals()
{
  this->valuesSP = NULL;
  this->accumulateToCompute = ACCUMULATE;
}

/**
 * The default destructor for the class 
 */
AccumulateLiterals::~AccumulateLiterals()
{
}


/**
 * Function that compute the statistic of all the values stored in the hashmap group by the key
 * @return Metric with the accumulate by group 
 */
Simulator::Metric* AccumulateLiterals::computeValue()
{
  double count = 0;
  
  map<string,double> literalCount;
  
  for(map<string,vector<double>*>::iterator itv = this->valuesSP->begin();itv != this->valuesSP->end();++itv)
  {
    string st = itv->first;
    vector<double>* vect = itv->second;
    
    double amount = 0;
    
    switch(this->accumulateToCompute)
    {
      case ACCUMULATE: 
      { 
        Accumulate * est = new Accumulate();
        est->setValues(vect); 
        Metric* res = est->computeValue();
        amount = res->getnativeDouble(); 
        break;
      }
      default:
        assert(false);
    }
    
    map<string,double>::iterator inserted = literalCount.find(st);
    
    if(inserted == literalCount.end())
      literalCount.insert(pair<string,double>(st,amount));
    else
      assert(false);
  }

  string ret  = "";
  
  //now we create the string and count them 
  for(map<string,double>::iterator itc = literalCount.begin(); itc != literalCount.end();++itc)
  {
    string literal = itc->first;
    string count = ftos(itc->second);
    ret+=literal+"="+count+";";
  }
  
  Simulator::Metric* performance = new Simulator::Metric();
  performance->setStatisticUsed(Simulator::ACCUMULATE_LITERALS);    
  performance->setNativeString(ret);
  performance->setNativeType(Simulator::STRING);
  
  return performance;
  
}


/**
 * Returns a reference to the hahmap of vector that holds all the values and groups to whom the stimator will be computed 
 * @return The reference to the vector 
 */
map<string,vector<double>*>* AccumulateLiterals::getSPvalues() const
{
  return valuesSP;
}

/**
 * Sets the reference to the hashmap that holds all the values and groups to whom the stimator will be computed  
 * @param theValue The reference to the vector 
 */
void AccumulateLiterals::setSPValues(map<string,vector<double>*>* theValue)
{
  valuesSP = theValue;
}


}
