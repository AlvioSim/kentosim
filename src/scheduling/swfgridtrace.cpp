#include <scheduling/swfgridtrace.h>
#include <utils/utilities.h>
#include <scheduling/jobrequirement.h>
#include <utils/log.h>
#include <scheduling/gridjob.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>


namespace Simulator {

/**
 * The default constructor for the class
 */
SWFGridTrace::SWFGridTrace()
{
 this->JobRequirementsFile = "";
}

/**
 * The default destructor for the class 
 */
SWFGridTrace::~SWFGridTrace()
{
}

/**
 * The constructor that should be used 
 * @param FilePath The file path that contains the standard workload file with the jobs definition
 * @param JobRequirementsFile  The file path that contains the job requirements
 * @param log The logging engine 
 */
SWFGridTrace::SWFGridTrace(string FilePath,string JobRequirementsFile,Log* log) : SWFTraceFile(FilePath,log)
{
  this->JobRequirementsFile = JobRequirementsFile;
  
  assert(JobRequirementsFile.compare("") != 0);

  this->isGrid = true;
}


/**
 * Function that loads resource the requirements file for the workload based on the grid format 
 * @return True if no problem have occurred, false otherwise (in this case the error willl be shown in the logging engine)
 */
bool SWFGridTrace::LoadRequirements()
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
      
      if(fields.size() != 15)
      {
      	log->error("Line "+itos(processed_lines)+" of the workload job requirements file "+this->filePath+ " has less than 5 fields or some separator is different than a space");
	log->debug("Line "+itos(processed_lines)+" of the workload job requirements file "+this->filePath+ " has no good format. Avoiding job.");
	ok = false;
	jobWithErrors++;
	continue;	
      }          
      
      processed_fields = 0;     
	
      GridJob* job = NULL;
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
	    
	    job = (GridJob*) itjob->second;	
	    assert(job != NULL); 
	    
	    break;
	    
        }
        case 1: 
        {
          //this should be the ClockSpeed
          double ClockSpeedMemory = atof(field.c_str());
          Metric* cspeed = job->getClockSpeedRequirement()->getmetric();
          cspeed->setNativeDouble(ClockSpeedMemory);
          cspeed->setNativeType(DOUBLE);
          cspeed->setType(CLOCK_SPEED);          
          break;
        }
        case 2:
        {
          //this should be the operator of the ClockSpeed
          operator_t op = this->getOperatorType(field.c_str());
          job->getClockSpeedRequirement()->setOperatorRequired(op);
          break;            
        }        
        case 3:
        {
          //this should be the getVendorRequirement
          string VendorRequirement = field.c_str();
          Metric* cspeed = job->getVendorRequirement()->getmetric();
          cspeed->setNativeString(VendorRequirement);
          cspeed->setNativeType(STRING);
          cspeed->setType(VENDOR);          
          break;
        }
        case 4:
        {
          //this should be the operator of the VendorRequirement
          operator_t op = this->getOperatorType(field.c_str());
          job->getVendorRequirement()->setOperatorRequired(op);
          break;                  
        }
        case 5:
        {
          //this should be the OSNameRequirement
          string OSNameRequirement = field.c_str();
          Metric* cspeed = job->getOSNameRequirement()->getmetric();
          cspeed->setNativeString(OSNameRequirement);
          cspeed->setNativeType(STRING);
          cspeed->setType(OS_NAME);          
          break;
        }
        case 6:
        {
          //this should be the operator of the VendorRequirement
          operator_t op = this->getOperatorType(field.c_str());
          job->getOSNameRequirement()->setOperatorRequired(op);
          break;                  
        }
        case 7: 
        {
          //this should be the MemoryRequirement
          double MemoryRequirement = atof(field.c_str());
          Metric* cspeed = job->getMemoryRequirement()->getmetric();
          cspeed->setNativeDouble(MemoryRequirement);
          cspeed->setNativeType(DOUBLE);
          cspeed->setType(MEMORY);          
          break;
        }
        case 8:
        {
          //this should be the operator of the MemoryRequirement
          operator_t op = this->getOperatorType(field.c_str());
          job->getMemoryRequirement()->setOperatorRequired(op);
          break;            
        }   
        case 9: 
        {
          //this should be the NumberProcessorsRequirement
          double NumberProcessorsRequirement = atof(field.c_str());
          Metric* cspeed = job->getNumberProcessorsRequirement()->getmetric();
          cspeed->setNativeDouble(NumberProcessorsRequirement);
          cspeed->setNativeType(DOUBLE);
          cspeed->setType(TOTAL_CPUS);          
          break;
        }
        case 10:
        {
          //this should be the operator of the NumberProcessorsRequirement
          operator_t op = this->getOperatorType(field.c_str());
          job->getNumberProcessorsRequirement()->setOperatorRequired(op);
          break;            
        }   
        case 11: 
        {
          //this should be the DiskRequirement
          double DiskRequirement = atof(field.c_str());
          Metric* cspeed = job->getDiskRequirement()->getmetric();
          cspeed->setNativeDouble(DiskRequirement);
          cspeed->setNativeType(DOUBLE);
          cspeed->setType(DISK_SIZE);          
          break;
        }
        case 12:
        {
          //this should be the operator of the NumberProcessorsRequirement
          operator_t op = this->getOperatorType(field.c_str());
          job->getDiskRequirement()->setOperatorRequired(op);
          break;            
        }           
        case 13: 
        {
          //this should be the DiskRequirement
          double DiskUsage = atof(field.c_str());
          job->setDisckUsed(DiskUsage);
          break;
        }
        case 14: 
        {
          //this should be the virtual organitzation where the job was submitted
          string vo = field.c_str();
          job->setVOSubmitted(vo);
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
 * Given the string description returns the operator_t associated to the string
 * @param operatorString The string containing the operator
 * @return The operator_t indicating the operator specified in the trace 
 */
operator_t  SWFGridTrace::getOperatorType(string variable)
{
   if(variable == "LESS_THAN")
   {
     return LESS_THAN;     
   }
   else if(variable == "EQUAL")
   {     
     return EQUAL;     
   }
   else if(variable == "HIGHER_THAN")
   {     
     return HIGHER_THAN;     
   }      
   else if(variable == "LESS_EQ_THAN")
   {     
     return LESS_EQ_THAN;     
   } 
   else if(variable == "HIGHER_EQ_THAN")
   {     
     return HIGHER_EQ_THAN;     
   }      
   else 
   {     
     assert(false);    
   } 
}

}
