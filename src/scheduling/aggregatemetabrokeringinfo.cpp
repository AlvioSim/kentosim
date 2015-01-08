#include <scheduling/aggregatemetabrokeringinfo.h>

namespace Simulator {

AggregateMetabrokeringInfo::AggregateMetabrokeringInfo()
{
  this->ResourcesInfo = new vector< ResourceAggregatedInfo * >();
  this->RelationshipsInfo = new vector< RelationshipAggregatedInfo * >();
}


AggregateMetabrokeringInfo::~AggregateMetabrokeringInfo()
{
  
  //we free the resources 
  for(vector< ResourceAggregatedInfo *>::iterator it = this->ResourcesInfo->begin(); it != this->ResourcesInfo->end();++it)
  {
    ResourceAggregatedInfo* rs = *it;
    delete rs;
  }

  //we free the relations
  for(vector< RelationshipAggregatedInfo *>::iterator it = this->RelationshipsInfo->begin(); it != this->RelationshipsInfo->end();++it)
  {
    RelationshipAggregatedInfo* rs = *it;
    delete rs;
  }

  delete this->ResourcesInfo;
  delete this->RelationshipsInfo;
}


/**
 * Returns the ResourcesInfo
 * @return A vector reference containing the differents resource attributes that are configured in the current agregated method
 */
vector< ResourceAggregatedInfo * >* AggregateMetabrokeringInfo::getResourcesInfo() const
{
	return ResourcesInfo;
}

/**
 * Sets the ResourcesInfo to the job
 * @param theValue The ResourcesInfo
 */
void AggregateMetabrokeringInfo::setResourcesInfo ( vector< ResourceAggregatedInfo * >* theValue )
{
	ResourcesInfo = theValue;
}

/**
 * Returns the RelationshipsInfo
 * @return A vector reference containing the differents relationshpis attributes that are configured in the current agregated method
 */
vector< RelationshipAggregatedInfo * >* AggregateMetabrokeringInfo::getRelationshipsInfo() const
{
	return RelationshipsInfo;
}

/**
 * Sets the RelationshipsInfo to the job
 * @param theValue The RelationshipsInfo
 */
void AggregateMetabrokeringInfo::setRelationshipsInfo ( vector< RelationshipAggregatedInfo * >* theValue )
{
	RelationshipsInfo = theValue;
}


}
