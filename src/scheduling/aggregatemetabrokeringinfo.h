#ifndef SIMULATORAGGREGATEMETABROKERINGINFO_H
#define SIMULATORAGGREGATEMETABROKERINGINFO_H


#include <list>
#include <vector>
#include <map>

#include <scheduling/resourceaggregatedinfo.h>
#include <scheduling/relationshipaggregatedinfo.h>


using namespace std;
using std::list;
using std::vector;
using std::map;

namespace Simulator {

/**
	@author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/
/**
*  This class contains the aggregate info that the borkers will be sending around the meta-brokering system for interchange information about their status and scheduling metrics.
*/
class AggregateMetabrokeringInfo{
public:
  AggregateMetabrokeringInfo();
  ~AggregateMetabrokeringInfo();

  void setRelationshipsInfo ( vector< RelationshipAggregatedInfo * >* theValue );
  vector< RelationshipAggregatedInfo * >* getRelationshipsInfo() const;
  void setResourcesInfo ( vector< ResourceAggregatedInfo * >* theValue );
  vector< ResourceAggregatedInfo * >* getResourcesInfo() const;
	

private:
  vector<RelationshipAggregatedInfo*>* RelationshipsInfo; /**< Vector that contains all the aggregated information for the relationships */
  vector<ResourceAggregatedInfo*>* ResourcesInfo; /**< Vector that contains all the aggregated information for the resources */

};

}

#endif
