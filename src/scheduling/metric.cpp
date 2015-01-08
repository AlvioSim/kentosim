#include <scheduling/metric.h>
#include <assert.h>

namespace Simulator {

/**
 * The default constructor for the class
 */
Metric::Metric()
{
}


/**
 * The constructor for the metric that is more commontly used 
 * @param type The metric used for compute the value 
 * @param statisticUsed The statistic used for compute the metric 
 * @param name The name of the metric 
 * @param nativeType The native tpye for the value conveyed in the metric 
 */
Metric::Metric(metric_t type,statistic_t statisticUsed,string name,native_t nativeType)
{
  this->type = type;
  this->statisticUsed = statisticUsed;
  this->name = name;
  this->nativeType = nativeType;
  this->estimatorName = "";

}

/**
 * The default destructor for the class 
 */
Metric::~Metric()
{
  for(vector<Metric*>::iterator it = this->otherMetrics.begin(); it != this->otherMetrics.end();++it)
  {
    Metric* toFree = *it;
    delete toFree;
  }
}

/**
 * Returngs The statistic used by the metric 
 * @return A statistic_t containing the statistic used for compute the metric 
 */
statistic_t Metric::getstatisticUsed() const
{
  return statisticUsed; 
}


/**
 * Sets the stastistic that will be used for compute the metric 
 * @param theValue The statistic_t type 
 */
void Metric::setStatisticUsed(const statistic_t& theValue)
{
  statisticUsed = theValue;
}


/**
 * Returns the metric type used 
 * @return A metric_t containing the metric used 
 */
metric_t Metric::gettype() const
{
  return type;
}


/**
 * Sets the metric type used 
 * @param theValue The metric type 
 */
void Metric::setType(const metric_t& theValue)
{
  type = theValue;
}


/**
 * Returns a reference to the value of the metric
 * @return A void* to the data 
 */
void* Metric::getvalue() const
{
  return value;
}


/**
 * Sets the reference to address that holds the value for of the metric 
 * @param theValue The address of the data
 */
void Metric::setValue(void* theValue)
{
  value = theValue;
}

/**
 * A function that is used for compare two metrics. By default we have provided this function by a set of predifined metrics, the most common metrics.. however if a more specific metric have to be computed the programmer must derive a new metric and overwrite this function.
 * @param b1 The metric to compare
 * @return Will return true if the current metric is better than b1
 */
bool Metric::betterThan(Metric* b1)
{
  //by default we have provided this function by a set of predifined metrics ..
  //the most common metrics.. however if a more specific metric have to be computed
  //the programmer must derive a new metric and overwrite this function
     switch(this->type)
     {
       
       /* all the doublean stuff that complies that a1 is better than a2 if a1 < a2*/
       case SLD:         
       case BSLD:      
       case BSLD_PRT:
       case JOBS_IN_WQUEUE:
       case LEAST_WORK_LEFT:
       case SUBMITED_JOBS:
       case PENALIZED_RUNTIME:
       case CPUS_USED:
       case WAITTIME:
       case SLD_PREDICTED:
       case PREDICTED_WT:
       case PREDICTED_RUNTIME:       
       case ESTIMATED_START_TIME_FIRST_FIT:       
       {
         //this metric is better if the waittime, the sld, and bsld is  lowerthan the provided 
         double waittime =  b1->getnativeDouble();
         if(waittime > this->getnativeDouble())
           return true;
         else
           return false;
         break;
       }
       case COMPUTATIONAL_COST:
       case ESTIMATED_COMPUTATIONAL_COST:
       {
         //it's not as easy as the other one, if the metric has another variable , if the slowdown is 
         //this metric is better if the sld provided is better 
         double sld =  b1->getComposedMetric(0)->getnativeDouble(); 
         double cost = b1->getnativeDouble();


         double factor_sld = sld/this->getComposedMetric(0)->getnativeDouble(); //this indicates that the local metric has N factor_sld times less slowdown
         double factor_cost = this->getnativeDouble()/cost; //this indicates that the current solution is M times more expensive than the initital solution


         //we say that the current metric is better if the increasment of the cost is proporionatlly or better to the incremenet of cost
         if(factor_sld/factor_cost > 1)
           return true;
         else
           return false;
        
         break;
       }
       case BACKFILLED_JOB:
       case kILLED_JOB:
       case COMPUTATIONAL_COST_RESPECTED:
       {
         if(!b1->getnativeBool() && this->getnativeBool())
          return true;
         else
          return false;
       }
       case MEMORY_AVAILABLE:
       case DISK_AVAILABLE:
       case FREE_CPUS:
       case BACKFILLED_JOBS:       
       {         
         double backfilledj =  b1->getnativeDouble();
         if(backfilledj < this->getnativeDouble())
           return true;
         else
           return false;
         break;
       } 
       default:
       	assert(0);
      }
}

/**
 * A function that is used for compare two metrics. By default we have provided this function by a set of predifined metrics, the most common metrics.. however if a more specific metric have to be computed the programmer must derive a new metric and overwrite this function.
 * @param b1 The metric to compare
 * @return Will return true if the current metric is better o equal than b1
 */
bool Metric::betterOrEqualThan(Metric* b1)
{
  //by default we have provided this function by a set of predifined metrics ..
  //the most common metrics.. however if a more specific metric have to be computed
  //the programmer must derive a new metric and overwrite this function
     switch(this->type)
     {
       
       /* all the doublean stuff that complies that a1 is better than a2 if a1 < a2*/
       case SLD:         
       case BSLD:      
       case BSLD_PRT:
       case JOBS_IN_WQUEUE:
       case LEAST_WORK_LEFT:
       case SUBMITED_JOBS:
       case PENALIZED_RUNTIME:
       case CPUS_USED:
       case WAITTIME:
       case SLD_PREDICTED:
       case PREDICTED_WT:
       case PREDICTED_RUNTIME:
       case ESTIMATED_START_TIME_FIRST_FIT:
       {
         //this metric is better if the waittime, the sld, and bsld is  lowerthan the provided 
         double waittime =  b1->getnativeDouble();
         if(waittime >= this->getnativeDouble())
           return true;
         else
           return false;
        
         break;
       }
       case COMPUTATIONAL_COST:       
       case ESTIMATED_COMPUTATIONAL_COST:
       {
         //it's not as easy as the other one, if the metric has another variable , if the slowdown is 
         //this metric is better if the sld provided is better 
         double sld =  b1->getComposedMetric(0)->getnativeDouble(); 
         double cost = b1->getnativeDouble();

         double factor_sld = sld/this->getComposedMetric(0)->getnativeDouble(); //this indicates that the local metric has N factor_sld times less slowdown
         double factor_cost = this->getnativeDouble()/cost; //this indicates that the current solution is M times more expensive than the initital solution

         //we say that the current metric is better if the increasment of the cost is proporionatlly or better to the incremenet of cost
         if(factor_sld/factor_cost >= 1)
           return true;
         else
           return false;
        
         break;
       }
       case MEMORY_AVAILABLE:
       case DISK_AVAILABLE:
       case FREE_CPUS:   
       case BACKFILLED_JOBS:
       {
         //this metric is better if the waittime, the sld, and bsld is  lowerthan the provided 
         double backfilledj =  b1->getnativeDouble();
         
         if(backfilledj <= this->getnativeDouble())
           return true;
         else
           return false;
         break;
       }        
       default:
       	assert(0);
      }
}

/**
 * Sets the native double 
 * @param value The value 
 */
void Metric::setNativeValue(double value)
{
  this->nativeDouble = value;
  this->value = (void*) &this->nativeDouble;
}


/**
 * Returns the name of the metric 
 * @return A string containing the name 
 */
string Metric::getname() const
{
  return name;
}


/**
 * Sets the name of the metric 
 * @param theValue The name for the metric 
 */
void Metric::setName(const string& theValue)
{
  name = theValue;
}

/**
 * Returns the native type for the value that is stored in the metric 
 * @return A native_t with the native type.
 */
native_t Metric::getnativeType() const
{
  return nativeType;
}


/**
 * Sets the native type for the value that is stored in the metric 
 * @param theValue A native_t with the native type.
 */
void Metric::setNativeType(const native_t& theValue)
{
  nativeType = theValue;
}


/**
 * Returns the name for the metric 
 * @return A string containing the name for the metric
 */
string Metric::getestimatorName() const
{
  return estimatorName;
}


/**
 * Sets the estimator name used in the metric 
 * @param theValue The name 
 */
void Metric::setEstimatorName(const string& theValue)
{
  estimatorName = theValue;
}


/**
 * Returns the native double value 
 * @return A double with the metric value 
 */
double Metric::getnativeDouble() const
{
  return nativeDouble;
}


/**
 * Sets the native double of the metric 
 * @param theValue The native value 
 */
void Metric::setNativeDouble(double theValue)
{
  nativeDouble = theValue;
}


/**
 * Returns the native bool value 
 * @return A bool with the metric value 
 */
bool Metric::getnativeBool() const
{
  return nativeBool;
}

/**
 * Sets the native bool of the metric 
 * @param theValue The native value 
 */
void Metric::setNativeBool(bool theValue)
{
  nativeBool = theValue;
}

/**
 * Returns the native string value 
 * @return A string with the metric value 
 */
string Metric::getnativeString() const
{
  return nativeString;
}

/**
 * Sets the native string of the metric 
 * @param theValue The native value 
 */
void Metric::setNativeString(const string& theValue)
{
  nativeString = theValue;
}

/**
 * Returns the i metric of this composed metric
 * @return A string with the metric value 
 */
Metric* Metric::getComposedMetric(int i)
{
  assert(this->otherMetrics.size() >= i+1);

  return this->otherMetrics[i];

}

/**
 * Sets another metric of this composed metric 
 * @param theValue The metric to add
 */
void Metric::addComposedMetric(Metric* metric)
{
  this->otherMetrics.push_back(metric);

}

}
