#ifndef SIMULATORRESOURCEAGGREGATEDINFO_H
#define SIMULATORRESOURCEAGGREGATEDINFO_H

#include <list>
#include <vector>
#include <map>
#include <string>

#include <scheduling/metric.h>

using namespace std;
using std::list;
using std::vector;
using std::map;

namespace Simulator {

/**
	@author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

/**
*  This class contains the aggregate info specifically about the centers that the borkers will be sending around the meta-brokering system for interchange information about their status and scheduling metrics.
*/

class ResourceAggregatedInfo{
public:
  ResourceAggregatedInfo();
  ~ResourceAggregatedInfo();

  void setResourceType ( const string& theValue );
  string getResourceType() const;
  void setResourceName ( const string& theValue );
  string getResourceName() const;
  void setAttributes ( vector< Metric * >* theValue );
  vector< Metric * >* getAttributes() const;
	

private:
 string ResourceName; /**< A string containing the resource name */
 string ResourceType; /**< A string containing the resource type */
 
 vector<Metric*>* Attributes; /**< A vector of metric  of the resource, each metric should be a string with the value containing the value for the attribute*/

};

}

#endif
