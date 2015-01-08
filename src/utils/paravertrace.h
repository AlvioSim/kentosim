#ifndef UTILSPARAVERTRACE_H
#define UTILSPARAVERTRACE_H

#include <string>
#include <deque>
#include <vector>

#include <scheduling/bucket.h>
#include <utils/architectureconfiguration.h>
#include <utils/mnconfiguration.h>
#include <utils/log.h>
#include <utils/paraverentry.h>
#include <set>
#include <map>
#include <vector>
#include <utils/utilities.h>
#include <utils/configurationfile.h>

// several events
#define JOB_STATE_EVENT_TYPE 0
#define JOB_BWMEM_EVENT_TYPE 1
#define JOB_PENALIZEDTIMES_EVENT_TYPE 2


// several values for JOB_STATE_EVENT_TYPE
#define JOB_STATE_FINISH_EVENT 0
#define JOB_STATE_SUBMIT_EVENT 1
#define JOB_STATE_START_EVENT 2
#define JOB_STATE_KILLED_EVENT 3
//#define JOB_STATE_PENALIZED_RUNTIME_EVENT 4
//#define JOB_STATE_BACKFILLED_EVENT 2


#define JOB_EXIT_STATE 0
#define JOB_RUNNING_STATE 1
#define JOB_WAITTING_STATE 2

using namespace std;
using namespace Simulator;

using std::set;
using std::map;
using std::vector;

namespace Utils {

/** The struct containing the order operator for the paraverentry, the order is set by the event time */
struct paraverentry_lt_t {
    
    bool operator() (ParaverEntry* entr1,ParaverEntry* entr2) const {		
	
	double x = entr1->time;
	double y = entr2->time;
	assert( x >= 0 && y >= 0 );

	if( x != y )
	    return x < y;
	else
	   return entr1->id < entr2->id;
    }
};

/**
* @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

/** The class defines the functionalities for generate paraver traces files containing the workload of the simulation */
class ParaverTrace{

/** Defines the set of paraver entries */
typedef set<ParaverEntry*, paraverentry_lt_t> ParaverEntriesSet;

/** Defines a pair of the trace id and the trace definition */
typedef pair<int,string> pairJobHeaderStr;

public:
  ParaverTrace(std::string trace_name,string dir,int total_jobs,ArchitectureConfiguration* architecture, architecture_type_t archType );
  ~ParaverTrace();
  void ParaverJobRuns(int job_id,deque<Bucket*>& allocations);
  void ParaverJobExit(int job_id,deque<Bucket*>& allocations);
  void setLog(Log* log);
  void close();
  void setLast_arrival(double theValue);
  double getlast_arrival() const;
  void generateMNArchitectureDefinition();
  void setTotal_jobs(const int& theValue);
  int gettotal_jobs() const;	
  void setStartJobTrace(const int& theValue);
  int getstartJobTrace() const;
  void setEndJobTrace(const int& theValue);
  int getendJobTrace() const;
	
private:
  string filepath; /**< The file path where the traces files will be stored */
  string date;/**< The date when the simulation was carried out */
  int total_jobs;/**< The total number of jobs to be added to the simulation */
  double last_arrival;/**< The time stamp for the last arrival*/
  Log* log;/**< The reference to the logging egine */
  string paraverHeader;/**< The parever trace header (See paraver documentation for more information)*/
  ArchitectureConfiguration* architecture;/**< The architecture that has been simulated in the simulation (currently only the marenostrum is supported )*/
  ParaverEntriesSet paraverBody;/**< The paraver entries that contain the events that will be dump in the file */
  map<int,string> paraverHeaderEntries;/**< The entries concerning the job definition that have to be added to the header of the file */
  string architectureDefintion;/**< The architecture definition (See paraver documentation for more information) that will be stored in the paraver header */
  string dir;/**< The directory where all the paraver trace file will be generated */
  string trace_name;/**< The name of the trace files that will hold the paraver stuff*/
  architecture_type_t archType;/**< The architecture type being simulated (currently only the marenostrum is supported)*/
  int lastid;/**< The last id asigned to the event */
  map<int,int> idJob2Applid; /**< Store a paraver appl id associated to the job*/
  int upperJob;/**< Indicates the last job id used in the praver trace file (internal variable)*/
  
  //user can indicate if the paraver trace has to be generated in a given interval
  int startJobTrace; /**< Indicates the first job to be stored in the paraver trace (-1 means starts from 0)*/
  int endJobTrace;/**< Iniddicates the last job to be included in the parever trace (-1 means all )*/
};

}

#endif
