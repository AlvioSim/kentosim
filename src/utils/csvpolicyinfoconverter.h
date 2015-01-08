#ifndef UTILSCSVPOLICYINFOCONVERTER_H
#define UTILSCSVPOLICYINFOCONVERTER_H



#include <scheduling/job.h>
#include <scheduling/schedulingpolicy.h>
#include <utils/cvsconverter.h>
#include <scheduling/policyentitycollector.h>

#include <map>

using namespace std;
using std::map;
using namespace Simulator;

/**
* This class will generate a cvs file with the information about all the states for the centers that have been simulated.
*/
namespace Utils {

/**
	@author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/
class CSVPolicyInfoConverter : public CSVConverter{
public:
  CSVPolicyInfoConverter(string FilePath,Log* log);
  ~CSVPolicyInfoConverter();
    
  void createPolicyCSVInfoFile(PolicyEntityCollector* collector);

};

}

#endif
