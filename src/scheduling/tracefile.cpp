#include <scheduling/tracefile.h>
#include <assert.h>

namespace Simulator {

/***************************
  Constructor and destructors
***************************/

/**
 * The constructor the should be used when instantiating this class 
 * @param FilePath Tje filepath of the file that contains the workload 
 * @param logFile The logging engine 
 */
TraceFile::TraceFile(string FilePath,Log* logFile)
{
  this->filePath = FilePath;
  this->log = logFile;
  this->JobsToLoad = -1;
  
  this->LoadTillJob = -1;
  this->LoadFromJob = 1;
  
  this->lastProcessedJob = NULL;
  
  assert(filePath.compare("") != 0 && log != NULL);
}

/**
 * The default constructor for the class
 */
TraceFile::TraceFile()
{
  this->filePath = "";
  this->log = NULL;
  this->JobsToLoad = -1;  
  this->LoadTillJob = -1;
  this->LoadFromJob = 1;
  this->lastProcessedJob = NULL;
}

/**
 * The default destructor for the class 
 */
TraceFile::~TraceFile()
{
}


/***************************
  Auxiliar functions
***************************/


/***************************
   Sets and gets functions  
***************************/

/**
 * Returns the file path of the file that contains the workload
 * @return The file path
 */
string TraceFile::getfilePath()
{
  return filePath;
}

/**
 * Sets the file path for the file that contains the workload definition 
 * @param theValue The file path for the workload
 */
void TraceFile::setFilePath(string& theValue)
{
  filePath = theValue;
}

/**
 * Returns the error that has occurred 
 * @return A string containing the error 
 */
string TraceFile::geterror()
{
  return error;
}


/**
 * Sets the error 
 * @param theValue String containing the error  
 */
void TraceFile::setError(string& theValue)
{
  error = theValue;
}

/**
 * Returns the last time stamp for the last arrival
 * @return A double indicating the last arrival 
 */
double TraceFile::getlastArrival() const
{
  return lastArrival;
}


/**
 * Sets the stamp time for the last arrival
 * @param theValue 
 */
void TraceFile::setLastArrival(double theValue)
{
  lastArrival = theValue;
}

/**
 * Returns the number of jobs in the workload 
 * @return A double containing hthe jobs in the workload 
 */
int TraceFile::getNumberOfJobs() const
{
  return this->JobList.size();
}


/**
 * Returns the jobs to load from the tracefile 
 * @return A double indicating the jobs to load 
 */
double TraceFile::getJobsToLoad() const
{
  return JobsToLoad;
}


/**
 * Sets the jobs to load from the workload file 
 * @param theValue A double indicating the loads to job 
 */
void TraceFile::setJobsToLoad(double theValue)
{
  JobsToLoad = theValue;
}

/**
 * Returns true if it has to load more jobs 
 * @return A bool indicating if more jobs have t be loaded
 */
bool TraceFile::LoadMoreJobs()
{
  if(this->JobsToLoad == -1)
   return true;
   
  if(this->JobsToLoad > this->LoadedJobs /*&& !(this->lastProcessedJob != NULL && lastProcessedJob->getJobNumber() >= this->LoadTillJob)*/)
  {    
    return true;
  }
  else return false;
    
}

/**
 * Returns the number of job from where the jobs have to be loaded to the simulation 
 * @return A double indicating from where load the jobs 
 */
double TraceFile::getLoadFromJob() const
{
  return LoadFromJob;
}


/**
 * Sets from which job the workload have to be loaded 
 * @param theValue A double containing from which job the workload has to be loaded
 */
void TraceFile::setLoadFromJob(double theValue)
{
  LoadFromJob = theValue;
}

/**
 * Indicates till which job the workload has to be loaded 
 * @return A double containing the last job to be loaded 
 */
double TraceFile::getLoadTillJob() const
{
  return LoadTillJob;
}


/**
 * Sets the last job to be loaded from the workload
 * @param theValue The last job to be loaded 
 */
void TraceFile::setLoadTillJob(double theValue)
{
  LoadTillJob = theValue;
}


/**
 * Indicates the number of jobs that have been loaded
 * @return A double containing the last job loaded 
 */
double TraceFile::getLoadedJobs() const
{
	return LoadedJobs;
}

/**
 * Sets the number of jobs that have been loaded from the workload
 * @param theValue The last job to be loaded 
 */
void TraceFile::setLoadedJobs ( double theValue )
{
	LoadedJobs = theValue;
}


}
