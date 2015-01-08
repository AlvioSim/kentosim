#ifndef SIMULATORRELATIONSHIPAGGREGATEDINFO_H
#define SIMULATORRELATIONSHIPAGGREGATEDINFO_H

#include <string>

using namespace std;

namespace Simulator {

/**
	@author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/
/**
*  This class contains the aggregate info specifically about the relationship that the borkers will be sending around the meta-brokering system for interchange information about their status and scheduling metrics.
*/

class RelationshipAggregatedInfo{
public:
  RelationshipAggregatedInfo();
  ~RelationshipAggregatedInfo();
  
  void setTargetType ( const string& theValue );
  string getTargetType() const;
  void setTargetName ( const string& theValue );
  string getTargetName() const;	
  void setSourceType ( const string& theValue );
  string getSourceType() const;
  void setSourceName ( const string& theValue );
  string getSourceName() const;
	

private:
  string SourceType; /**< String containing the source type for the relation ship   */
  string SourceName ; /**<  String containing the source name for the relation ship  */
  string TargetType; /**< String containing the Target type for the relation ship   */
  string TargetName ; /**<  String containing the Target name for the relation ship  */

};

}

#endif
