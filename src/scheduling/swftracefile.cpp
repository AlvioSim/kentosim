#include <scheduling/swftracefile.h>
#include <scheduling/job.h>
#include <scheduling/gridjob.h>
#include <utils/utilities.h>
#include <utils/log.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <deque>

using namespace std;
using std::vector;
using std::deque;

namespace Simulator {

/***************************
  Constructor and destructors
***************************/
/**
 * The default constructor for the class
 */
SWFTraceFile::SWFTraceFile() : TraceFile()
{
}


/**
 * The constructor that should be used when using this class
 * @param FilePath The file path of the file that contains the workload definition
 * @param log The logging engine 
 */
SWFTraceFile::SWFTraceFile(string FilePath,Log* log) : TraceFile(FilePath,log)
{
  this->isGrid = false;
  this->isExtended = false;
}

/**
 * The default destructor for the class 
 */
SWFTraceFile::~SWFTraceFile()
{
}

/***************************
  Auxiliar data functions  
***************************/

/**
 * Function that loads the workload 
 * @return True if no problem has occurred, false otherwise
 */
bool SWFTraceFile::loadTrace()
{
  string line;
  string separator(" ");
  ifstream trace (this->filePath.c_str());
  int processed_lines = 0;
  int processed_fields = 0;
  int jobWithErrors = 0;
  
  Job* currentJob = NULL; //will be used to set up the last arrival in the sim
  
  bool ok = true;    
  this->LoadedJobs = 0;
  bool goAhead = true;
  
  if (trace.is_open())
  {
    while (LoadMoreJobs() && ! trace.eof() && goAhead )
    {         
    
      getline (trace,line);
      
      log->debug("Reading the trace file from the "+line,6);
      
      //for each line we try to parse the swf
      //if ";" means its header stuff so we should process it
      string::size_type loc = line.find(";", 0 );
      
      if( loc != string::npos )
      {
      	this->processHeaderEntry(line);
	log->debug("Processing header line "+line,6);
	continue;
      }
      
      //should contain information about a job
      deque<string> fields;     
      SplitLine(line,separator,fields);
      
      log->debug("The line has "+itos(fields.size())+ "parameters",6);
      
      /*for debug */
      if(log->getlevel() >= 6)  
        for(int i = 0;i < fields.size();i++)
         log->debug("Field number "+itos(i) +" has the value"+((string)fields[i]) ,6);
      
      if(fields.size() != 18)
      {
      	log->error("Line "+itos(processed_lines)+" of the workload file "+this->filePath+ " has less than 18 fields or some separator is different than a space");
	log->debug("Line "+itos(processed_lines)+" of the workload file "+this->filePath+ " has no good format. Avoiding job.");
	ok = false;
	jobWithErrors++;
	continue;	
      }          
      
      processed_fields = 17;     
	
      if(this->isGrid)
        currentJob = new GridJob();
      else 
        currentJob = new Job();
      
      for(;!fields.empty();fields.pop_back())
      {
        string field = (string) fields.back();
        
        log->debug("Processing the field with value "+field,6);
	
	switch(processed_fields)
	{
	case 0: currentJob->setJobNumber(atoi(field.c_str()));
		break;
	case 1: currentJob->setSubmitTime(atof(field.c_str()));	             
		break;
	case 2: currentJob->setWaitTime(atof(field.c_str()));
		break;
	case 3: currentJob->setRunTime(atof(field.c_str()));
		break;
	case 4: currentJob->setNumberProcessors(atoi(field.c_str()));
		break;
	case 5: currentJob->setAvgCPUTimeUsed(atoi(field.c_str()));
		break;
	case 6: currentJob->setUsedMemory(atof(field.c_str()));
		break;
	case 7: currentJob->setRequestedProcessors(atoi(field.c_str()));
		break;
	case 8: currentJob->setRequestedTime(atof(field.c_str()));
                currentJob->setOriginalRequestedTime(atof(field.c_str()));
		break;
	case 9: currentJob->setRequestedMemory(atof(field.c_str()));
		break;
	case 10: currentJob->setStatus(atoi(field.c_str())); 
		break;
	case 11: currentJob->setUserID(atoi(field.c_str()));
		break;
	case 12: currentJob->setGroupID(atoi(field.c_str()));
		break;
	case 13: currentJob->setExecutable(atoi(field.c_str())); 
		break;
	case 14: currentJob->setQueueNumber(atoi(field.c_str()));
		break;
	case 15: currentJob->setPartitionNumber(atoi(field.c_str()));
		break;
	case 16: currentJob->setPrecedingJobNumber(atoi(field.c_str()));
		break;
	case 17: currentJob->setThinkTimePrecedingJob(atoi(field.c_str()));
	break;	
	}	
	
        processed_fields--;
	
      }
      
      fields.clear();
      
      processed_lines++;       
      
      assert(currentJob != NULL);
      
      if(currentJob->getJobNumber() < this->LoadFromJob)
      {
        //the current job has not to be loaded .. se we just skip it 
        delete currentJob;
        continue;
      }

      if(currentJob->getJobNumber() > this->LoadTillJob && this->LoadTillJob != -1)
      {
         log->debug("The trace is load till the job "+itos(currentJob->getJobNumber()),2);
         delete currentJob;
         goAhead = false;
         break;
      }
      
      this->JobList.insert(pairJob(currentJob->getJobNumber(),currentJob));
      this->LoadedJobs++;
      
      lastProcessedJob = currentJob;
         
    }
    trace.close();
  }
  else
  { 
    this->error = "Unable to open file"; 	
    return false;
  }
    
  log->debug(itos(jobWithErrors)+ " lines of the workload trace have errors ");
  log->debug(itos(processed_lines)+ " jobs of the workload trace have been processed ");
  
  this->lastArrival = currentJob->getSubmitTime();
  
  return ok;
}


void SWFTraceFile::processHeaderEntry(string headerLine)
{
}


}
