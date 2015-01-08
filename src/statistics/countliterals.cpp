#include <statistics/countliterals.h>

#include <algorithm>
#include <map>

using std::map;

namespace Statistics {

/**
 * The default constructor for the class
 */
CountLiterals::CountLiterals()
{
  this->values = NULL;
}

/**
 * The default destructor for the class 
 */
CountLiterals::~CountLiterals()
{
}

/**
 * Function that compute the number of times that each value is repeated in the set, the metric is returned as string. With the format X=2, Z=34 etc.
 * @return Metric with the count of literals 
 */
 
Simulator::Metric* CountLiterals::computeValue()
{
  double count = 0;
  double dvalue;
  
  map<string,int> literalCount;
  
  for(vector<string>::iterator itv = this->values->begin();itv != this->values->end();++itv)
  {
    string st = *itv;
    
    map<string,int>::iterator inserted = literalCount.find(st);
    
    if(inserted == literalCount.end())
      literalCount.insert(pair<string,int>(st,1));
    else
      literalCount[st] = literalCount[st]+1;
  }

  string ret  = "";
  
  //now we create the string and count them 
  for(map<string,int>::iterator itc = literalCount.begin(); itc != literalCount.end();++itc)
  {
    string literal = itc->first;
    string count = itos(itc->second);
    ret+=literal+"="+count+";";
  }
  
  Simulator::Metric* performance = new Simulator::Metric();
  performance->setStatisticUsed(Simulator::COUNT_LITERALS);    
  performance->setNativeString(ret);
  performance->setNativeType(Simulator::STRING);
  
  return performance;
  
}



/**
 * Returns a reference to the vector that holds all the values to whom the stimator will be computed 
 * @return The reference to the vector of strings 
 */
vector< string >* CountLiterals::getSvalues() const
{
  return values;
}


/**
 * Sets the reference to the vector that holds all the values to whom the stimator will be computed  
 * @param theValue The reference to the vector of strings 
 */
void CountLiterals::setSValues(vector< string >* theValue)
{
  values = theValue;
}


}
