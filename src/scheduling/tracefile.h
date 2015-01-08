#ifndef TRACEFILE_H
#define TRACEFILE_H

#include <map>
#include <vector>
#include <string>
#include <utils/log.h>
#include <scheduling/job.h>

using namespace std;
using namespace Utils;

using std::map;
using std::vector;

/**
* @author Francesc Guim,C6-E201,93 401 16 50,
*/
namespace Simulator {

//type defs for the list stuff
typedef pair <int, Job*> pairJob;

/** 
* This class loads a log file in the standard workload format and instantiates the jobs that are specified in the file.  */
class TraceFile{
public:
  //Constructors and desctructors
  TraceFile();  
  TraceFile(string FilePath,Log* log);	
  ~TraceFile();
  
  //Sets and gets method
  void setFilePath(string& theValue);	
  string getfilePath();
  void setError(string& theValue);
  string geterror();	
   
  //abstract functions to be implemented by the subclass
  virtual bool loadTrace() = 0;
  virtual void processHeaderEntry(string headerLine) = 0;
  void setLastArrival(double theValue);
  double getlastArrival() const;
  int getNumberOfJobs() const;	
  void setJobsToLoad(double theValue);
  double getJobsToLoad() const;	
  
  bool LoadMoreJobs();
  void setLoadFromJob(double theValue);
  double getLoadFromJob() const;
  void setLoadTillJob(double theValue);
  double getLoadTillJob() const;
  void setLoadedJobs ( double theValue );
  double getLoadedJobs() const;
	
	
	  
  map<int,Job*> JobList;   /**< A list of the jobs that are included in the workload */


protected:  
  string filePath; /**< FilePath of the trace where the Workload is stored */
  string error;/**<  If an error occurs this string will contains the value */
  Log* log; /**< A reference to the logging engine */
  double lastArrival; /**< A double that contains the last arrival in the workload */
  double JobsToLoad; /**< Indicates the amount of jobs to load */
  double LoadedJobs;/**< Indicates the number of jobs that have been loaded */
  double LoadFromJob; /**< Inidcates from which job the workload has to be loaded */
  double LoadTillJob; /**<  Indicates till which job the workload will be loaded */
  
  Job* lastProcessedJob; /**< Indicates the last processed job  */
  	
};

}
#endif
