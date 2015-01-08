#include <scheduling/swftraceextended.h>
#include <utils/utilities.h>
#include <utils/log.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>


namespace Simulator {

/**
 * The default constructor for the class
 */
SWFTraceExtended::SWFTraceExtended()
{
 this->JobRequirementsFile = "";
}

/**
 * The default destructor for the class 
 */
SWFTraceExtended::~SWFTraceExtended()
{
}

/**
 * The constructor that should be used 
 * @param FilePath The file path that contains the standard workload file with the jobs definition
 * @param JobRequirementsFile  The file path that contains the job requirements
 * @param log The logging engine 
 */
SWFTraceExtended::SWFTraceExtended(string FilePath,string JobRequirementsFile,Log* log) : SWFTraceFile(FilePath,log)
{
  this->JobRequirementsFile = JobRequirementsFile;
  
  assert(JobRequirementsFile.compare("") != 0);

  this->isExtended = true;
}


/**
 * Function that loads resource the requirements file for the workload 
 * @return True if no problem have occurred, false otherwise (in this case the error willl be shown in the logging engine)
 */
bool SWFTraceExtended::LoadCostRequirements()
{
  string line;
  string separator(" ");
  ifstream trace (this->JobRequirementsFile.c_str());
  int processed_lines = 0;
  int processed_fields = 0;
  int jobWithErrors = 0;  
  
  bool ok = true;
  
  this->LoadedJobs = 0;
  this->lastProcessedJob = NULL;

  bool goAhead = true;  

  if (trace.is_open())
  {
    while (! trace.eof() && LoadMoreJobs() && goAhead )
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
      if(log->getlevel() >= 2)  
        for(int i = 0;i < fields.size();i++)
         log->debug("Field number "+itos(i) +" has the value"+((string)fields[i]) ,6);
      
      if(fields.size() != 2)
      {
      	log->error("Line "+itos(processed_lines)+" of the workload job requirements file "+this->filePath+ " has less than 5 fields or some separator is different than a space");
	log->debug("Line "+itos(processed_lines)+" of the workload job requirements file "+this->filePath+ " has no good format. Avoiding job.");
	ok = false;
	jobWithErrors++;
	continue;	
      }          
      
      processed_fields = 0;     
	
      Job* job = NULL;
      bool cont=true;
      
      for(;!fields.empty() && cont ;fields.pop_front())
      {
        string field = (string) fields.front();
	
	switch(processed_fields)
	{
	case 0: 
	{
            int jobid=atoi(field.c_str());

	    if( jobid < this->LoadFromJob)
            {
              //the current job has not to be loaded .. se we just skip it 
              cont=false;
              break;
            }
	

            if(jobid > this->LoadTillJob && this->LoadTillJob != -1)
            {
              log->debug("The trace is load till the job "+itos(jobid),2);              
              goAhead = false;
              cont=false;
              break;
            }

	    //this key should be the job id 
	    map<int,Job*>::iterator itjob =  JobList.find(atoi(field.c_str()));
	    if(itjob == JobList.end())
	    {
	      log->error("There is no job with id "+field+ " in the main trace file .. please check the job requirements file.");
	      break;
	    }
	    
	    job = itjob->second;	
	    assert(job != NULL); 
	    
	    break;
	    
        }
        case 1: 
        {
          //this should be the BWMemory used 
          double costAllowed = atof(field.c_str());
          job->setMaximumAllowedCost(costAllowed);
          break;
        }

        }
        
        processed_fields++;                
     }
     
     if(cont)
     {
       this->lastProcessedJob = job;
       this->LoadedJobs++;
     }
  }
 }
}

/**
 * Function that loads resource the requirements file for the workload 
 * @return True if no problem have occurred, false otherwise (in this case the error willl be shown in the logging engine)
 */
bool SWFTraceExtended::LoadRequirements()
{
  string line;
  string separator(" ");
  ifstream trace (this->JobRequirementsFile.c_str());
  int processed_lines = 0;
  int processed_fields = 0;
  int jobWithErrors = 0;  
  
  bool ok = true;
  
  this->LoadedJobs = 0;
  this->lastProcessedJob = NULL;

  bool goAhead = true;  

  if (trace.is_open())
  {
    while (! trace.eof() && LoadMoreJobs() )
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
      if(log->getlevel() >= 2)  
        for(int i = 0;i < fields.size();i++)
         log->debug("Field number "+itos(i) +" has the value"+((string)fields[i]) ,6);
      
      if(fields.size() != 5)
      {
      	log->error("Line "+itos(processed_lines)+" of the workload job requirements file "+this->filePath+ " has less than 5 fields or some separator is different than a space");
	log->debug("Line "+itos(processed_lines)+" of the workload job requirements file "+this->filePath+ " has no good format. Avoiding job.");
	ok = false;
	jobWithErrors++;
	continue;	
      }          
      
      processed_fields = 0;     
	
      Job* job = NULL;
      bool cont=true;
      
      for(;!fields.empty() && cont ;fields.pop_front())
      {
        string field = (string) fields.front();
	
	switch(processed_fields)
	{
	case 0: 
	{
            int jobid=atoi(field.c_str());

	    if( jobid < this->LoadFromJob)
            {
              //the current job has not to be loaded .. se we just skip it 
              cont=false;
              break;
            }

            if(jobid > this->LoadTillJob && this->LoadTillJob != -1)
            {
              log->debug("The trace is load till the job "+itos(jobid),2);              
              goAhead = false;
              cont=false;
              break;
            }
	
	    //this key should be the job id 
	    map<int,Job*>::iterator itjob =  JobList.find(atoi(field.c_str()));
	    if(itjob == JobList.end())
	    {
	      log->error("There is no job with id "+field+ " in the main trace file .. please check the job requirements file.");
	      break;
	    }
	    
	    job = itjob->second;	
	    assert(job != NULL); 
	    
	    break;
	    
        }
        case 1: 
        {
          //this should be the BWMemory used 
          double BWMemory = atof(field.c_str());
          job->setBWMemoryUsed(BWMemory);
          break;
        }
        case 2:
        {
           //this should be the Memory needed
          double Memory = atof(field.c_str());
          job->setMemoryUsed(Memory);
          break;            
        }        
        case 3:
        {
          //this should be the BWNetwork
          double BWNetwork = atof(field.c_str());
          job->setBWNetworkUsed(BWNetwork);
          break;
        }
        case 4:
        {
           //this should be the BWEthernet
          double BWEthernet = atof(field.c_str());
          job->setBWEthernedUsed(BWEthernet);
          break;            
        }
        }
        
        processed_fields++;                
     }
     
     if(cont)
     {
       this->lastProcessedJob = job;
       this->LoadedJobs++;
     }
  }
}

}

}
