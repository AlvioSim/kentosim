#include <datamining/discretizer.h>
#include <assert.h>

namespace Datamining {

/**
 * @param Log A reference to the logging engine 
 */
Discretizer::Discretizer(Log* log)
{  
  this->log = log;
}


/**
 * The destructor for the class 
 */
Discretizer::~Discretizer()
{
}

/**
 * Retruns the discretizer type implemented by the current class 
 * @return A discretizer_t indicating the discretizer type implemented 
 */
discretizer_t Discretizer::getdiscretizerType() const
{
  return discretizerType;         
}


/**
 * Sets the discretizer type implementented by the current class 
 * @param theValue The discretizer type
 */
void Discretizer::setDiscretizerType(const discretizer_t& theValue)
{
  discretizerType = theValue;
}

/**
 * Given a discretizer_t type returns an string containing a human readable discretizer type
 * @param discretizer The discretizer type 
 * @return A string containing a human readable discretizerType.
 */
string Discretizer::getDiscretizerName(discretizer_t discretizer)
{
  switch(discretizer)
  {
    case SAME_INTERVAL_DISCRETIZER:
      return "SAME_INTERVAL_DISCRETIZER";
    case SAME_INSTANCES_PER_INTERVAL_DISCRETIZER:
      return "SAME_INSTANCES_PER_INTERVAL_DISCRETIZER";
    case OTHER_DISCRETIZER:
      return "OTHER_DISCRETIZER";
    default:
      assert(false);
  }
}


}
