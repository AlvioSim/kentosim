#ifndef UTILSCSVJOBSINFOCONVERTER_H
#define UTILSCSVJOBSINFOCONVERTER_H


#include <scheduling/job.h>
#include <scheduling/schedulingpolicy.h>
#include <utils/cvsconverter.h>

#include <map>

using namespace std;
using std::map;
using namespace Simulator;

namespace Utils {

/**
* @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

/**
* This class will generate a cvs file with the information about all the jobs that have been simulated.
*/
class CSVJobsInfoConverter : public CSVConverter{
public:
  CSVJobsInfoConverter(string FilePath,Log* log);
  ~CSVJobsInfoConverter();

  void createJobsCSVInfoFile(map<int,Job*>* JobList);
  void addEntry(Job* job);
  void setPolicy(SchedulingPolicy* theValue);
  SchedulingPolicy* getpolicy() const;
	

private:
  SchedulingPolicy* policy; /**< The scheduling policy that will provide information about the jobs  */

};


}

#endif
