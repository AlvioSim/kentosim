#include <scheduling/resourceaggregatedinfo.h>

namespace Simulator {

ResourceAggregatedInfo::ResourceAggregatedInfo()
{
 this->Attributes = new vector<Metric*>();
}


ResourceAggregatedInfo::~ResourceAggregatedInfo()
{

  //we free the metrics 
  for(vector<Metric*>::iterator it = this->Attributes->begin(); it != Attributes->end();++it)
  {
    Metric* met = *it;
    delete met; 
  }
  delete this->Attributes;
}

/**
 * Returns the Attributes
 * @return A vector reference containing the differents attributes that are configured in the current agregated method
 */
vector< Metric * >* ResourceAggregatedInfo::getAttributes() const
{
	return Attributes;
}

/**
 * Sets the Attributes to the job
 * @param theValue The Attributes
 */
void ResourceAggregatedInfo::setAttributes ( vector< Metric * >* theValue )
{
	Attributes = theValue;
}

/**
 * Returns the ResourceName
 * @return A string containing the ResourceName
 */
string ResourceAggregatedInfo::getResourceName() const
{
	return ResourceName;
}

/**
 * Sets the ResourceName to the job
 * @param theValue The ResourceName
 */
void ResourceAggregatedInfo::setResourceName ( const string& theValue )
{
	ResourceName = theValue;
}


/**
 * Returns the ResourceType
 * @return A string containing the ResourceType
 */
string ResourceAggregatedInfo::getResourceType() const
{
	return ResourceType;
}

/**
 * Sets the ResourceType to the job
 * @param theValue The ResourceType
 */
void ResourceAggregatedInfo::setResourceType ( const string& theValue )
{
	ResourceType = theValue;
}

}
