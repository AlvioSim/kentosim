#include <scheduling/relationshipaggregatedinfo.h>

namespace Simulator {

RelationshipAggregatedInfo::RelationshipAggregatedInfo()
{
}


RelationshipAggregatedInfo::~RelationshipAggregatedInfo()
{
}

/**
 * Returns the SourceType
 * @return A string containing the SourceType
 */
string RelationshipAggregatedInfo::getSourceName() const
{
	return SourceName;
}

/**
 * Sets the SourceName to the job
 * @param theValue The SourceName
 */
void RelationshipAggregatedInfo::setSourceName ( const string& theValue )
{
	SourceName = theValue;
}


/**
 * Returns the SourceType
 * @return A string containing the SourceType
 */
string RelationshipAggregatedInfo::getSourceType() const
{
	return SourceType;
}

/**
 * Sets the SourceType to the job
 * @param theValue The SourceType
 */
void RelationshipAggregatedInfo::setSourceType ( const string& theValue )
{
	SourceType = theValue;
}

/**
 * Returns the TargetName
 * @return A string containing the TargetName
 */
string RelationshipAggregatedInfo::getTargetName() const
{
	return TargetName;
}

/**
 * Sets the TargetName to the job
 * @param theValue The TargetName
 */
void RelationshipAggregatedInfo::setTargetName ( const string& theValue )
{
	TargetName = theValue;
}

/**
 * Returns the TargetType
 * @return A string containing the SourceType
 */
string RelationshipAggregatedInfo::getTargetType() const
{
	return TargetType;
}

/**
 * Sets the TargetType to the job
 * @param theValue The TargetType
 */
void RelationshipAggregatedInfo::setTargetType ( const string& theValue )
{
	TargetType = theValue;
}

}
