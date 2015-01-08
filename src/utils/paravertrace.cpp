#include <utils/paravertrace.h>
#include <assert.h>
#include <utils/utilities.h>
#include <time.h>
#include <fstream>
#include <archdatamodel/node.h>
#include <archdatamodel/bladecenter.h>
#include <archdatamodel/marenostrum.h>

#include <math.h>

namespace Utils {

/**
 * The constructor for the class that should be used 
 * @param trace_name The filename for the trace files 
 * @param dir The directory where the files have to be stored 
 * @param total_jobs The total jobs that will be included in the trace 
 * @param architecture The architecture that has been simulated 
 * @param archType The architecture type used 
 */
ParaverTrace::ParaverTrace(string trace_name,string dir,int total_jobs,ArchitectureConfiguration* architecture, architecture_type_t archType)
{
  this->filepath = dir+"/"+trace_name+".prv";
  this->dir = dir;
  this->trace_name = trace_name;
  this->lastid = 0;
  //by default there is no log
  this->log = 0;
  this->log = 0;
  
  this->total_jobs = 0;
  this->architecture = architecture;
  this->archType = archType;
  
  this->startJobTrace = -1;
  this->endJobTrace = -1;
  
  //Creating the paraver header. 
  //The ftime is the total time of the trace, in our case is computed with the last arrival in the simulation;
  //obviously we do not know how much will time will take the overall simulation time, however we approximate it, by adding to the
  //provided time the running time for the last job (this amount have to be included in the parameter)
  

  if(date == "")
  {
    time_t tim = time(NULL);
    struct tm *tmsp = localtime(&tim);
     
    this->date = itos(tmsp->tm_mday)+"/"+itos(tmsp->tm_mon)+"/"+itos(tmsp->tm_year+1900)+" at "+itos(tmsp->tm_hour)+":"+ itos(tmsp->tm_min);
  }
      
  this->upperJob = 1;
}

/**
 * The default destructor for the class 
 */
ParaverTrace::~ParaverTrace()
{

}

/**
 * Generates the traces files with the jobs that have been simulated. 
 */
void ParaverTrace::close()
{
  //we add 30 seconds to the last arrival for allow full visualitzation
  switch(this->archType)
  { 
    case MARENOSTRUM: 
      this->generateMNArchitectureDefinition();
      break;
    default:
     assert(false);
     break;
  }

  this->paraverHeader = "#Paraver("+this->date+"):"+ftos(this->last_arrival+30)+":"+architectureDefintion+":"+itos(this->total_jobs)+ this->paraverHeader;

  for(int i = 0; i<= this->upperJob;i++)
  {
    map<int,string>::iterator it = this->paraverHeaderEntries.find(i);
    
    if(it == this->paraverHeaderEntries.end())
      continue;
    
    this->paraverHeader+= it->second;
  }

  ofstream fout( this->filepath.c_str(), ios::trunc );
  fout << this->paraverHeader << endl;
  log->debug("The PARAVER IS: "+this->paraverHeader,2);
  for(ParaverEntriesSet::iterator it = this->paraverBody.begin();it != this->paraverBody.end();++it)  
  {  
     ParaverEntry* en = *it;
     fout << en->entry << endl;
     log->debug(en->entry,2);
  }
  
  fout.close();
}

/**
 * Indicates to the class that a job has started in the provided allocation.  The function generates the events concerning the job.
 * @param job_id The job id  
 * @param allocations The set of buckets containing where the processes for the job have been allocated
 */
void ParaverTrace::ParaverJobRuns(int job_id, deque<Bucket*>& allocations)
{

  //We add the job if it belongs to the desired interval 
  if((job_id < this->startJobTrace && this->startJobTrace != -1) || ( job_id > this->endJobTrace && this->endJobTrace != -1))
   return;
 

  // store application id for this job
  this->idJob2Applid[job_id] = this->upperJob;

  this->total_jobs++;

  //We add the events for running job. 
  //At this time the given job acquires the number of cpus that it requires.
  //There should be no conflicts due to the the scheduler has to maintain one cpu per process		
  int task = 1;         
  
  int jobsize = allocations.size();
  
  string paraverHeaderEntry  = ":"+itos(jobsize)+"(";
  
  bool first = true;
  
  for(deque<Bucket*>::iterator it = allocations.begin();
      it != allocations.end(); ++it)
  {
    Bucket* bucket = *it;
   
    // event traces

    this->paraverBody.insert(new ParaverEntry("2:0:" + itos(this->upperJob) + ":" + itos(task) +
                            ":1:" + ftos(bucket->getjob()->getJobSimSubmitTime()) + ":" + 
                            itos(JOB_STATE_EVENT_TYPE) + ":" + itos(JOB_STATE_SUBMIT_EVENT),
                            bucket->getjob()->getJobSimSubmitTime(), ++this->lastid));

    /*
    this->paraverBody.insert(new ParaverEntry("2:" + itos(bucket->getcpu()+1) + ":" +
                            itos(this->upperJob) + ":" + itos(task) + ":1:" + ftos(bucket->getstartTime()) + ":" +
                            itos(JOB_STATE_EVENT_TYPE) + ":" + itos(JOB_STATE_START_EVENT),
                            bucket->getstartTime(), ++this->lastid));

    if ( bucket->getjob()->getBWMemoryUsed() != -1 )
    {
        this->paraverBody.insert(new ParaverEntry("2:" + itos(bucket->getcpu() + 1 ) + ":" +
                                itos(this->upperJob) + ":" + itos(task) + ":1:" +
                                ftos(bucket->getstartTime()) +":"+ itos(JOB_BWMEM_EVENT_TYPE) + ":" +
                                ftos(bucket->getjob()->getBWMemoryUsed()), bucket->getstartTime(), ++this->lastid));
    }

    // record trace
    //this->paraverBody.insert(new ParaverEntry("1:" + itos(bucket->getcpu()+1) + ":" + 
    //                        itos(this->upperJob) + ":" + itos(task) + ":1:" + ftos(bucket->getstartTime()) +
    //                        ":" + ftos(bucket->getendTime()) + ":" + itos(JOB_RUNNING_STATE),
    //                        bucket->getstartTime(), ++this->lastid));
    //

    */
    
     if(!first)
     {
       paraverHeaderEntry+=",";       
     }
     else
       first = false;
     
     paraverHeaderEntry += "1:"+itos(this->architecture->getNodeIdCpu(bucket->getcpu())+1);
     task++;               
  } 
  
  paraverHeaderEntry+=")";
  
  this->paraverHeaderEntries.insert(pairJobHeaderStr(this->upperJob,paraverHeaderEntry));
  
  this->upperJob++;
  
}

/**
 * Indicates to the class that a job has quit and leave her provided allocation.  
 * @param job_id The job id  
 * @param allocations The set of buckets containing where the processes for the job had been allocated
 */
void ParaverTrace::ParaverJobExit(int job_id, deque<Bucket*>& allocations)
{

  const jobListBucketPenalizedTime_t * pTimes;

  // search paraver application id regarded job_id
  map<int, int>::iterator it = this->idJob2Applid.find(job_id);


  if ( it == this->idJob2Applid.end() )
    return; // it doesn't exist
    
  int applid = it->second;
  int task = 1;         

  

  for(deque<Bucket*>::iterator it = allocations.begin();
      it != allocations.end(); ++it)
  {
    Bucket* bucket = *it;
    Job * j = bucket->getjob();

    // real record trace 
    this->paraverBody.insert(new ParaverEntry("1:" + itos(bucket->getcpu()+1) + ":" + 
                            itos(applid) + ":" + itos(task) + ":1:" + ftos(bucket->getstartTime()) +
                            ":" + /*ftos(bucket->getendTime())*/ ftos(j->getJobSimFinishTime()) + ":" + itos(JOB_RUNNING_STATE),
                            bucket->getstartTime(), ++this->lastid));


    // get all proably penaltie times for this bucket
    if ( ( pTimes = j->getBucketPenalizedTimes(bucket->getId())) != NULL )
    {
        // this bucket has some penalized times
        for ( jobListBucketPenalizedTime_t::const_iterator itp = pTimes->begin(); itp != pTimes->end(); itp++)
        {
            double startTime = itp->first;
            double penalizedTime = itp->second;

            this->paraverBody.insert(new ParaverEntry("2:" + itos(bucket->getcpu() + 1 ) + ":" +
                            itos(applid) + ":" + itos(task) + ":1:" +
                            ftos(startTime) +":"+ itos(JOB_PENALIZEDTIMES_EVENT_TYPE) + ":" + ftos(penalizedTime)
                            , startTime, ++this->lastid));
        }
    }

    // mark end of penalyzed times with 0
    this->paraverBody.insert(new ParaverEntry("2:" + itos(bucket->getcpu() + 1 ) + ":" +
                        itos(applid) + ":" + itos(task) + ":1:" +
                        ftos(j->getJobSimFinishTime())+":"+ itos(JOB_PENALIZEDTIMES_EVENT_TYPE) + ":" + ftos(0)
                        , bucket->getendTime(), ++this->lastid));
 
   
    /*
    if ( bucket->getjob()->getBWMemoryUsed() != -1 )
    {
        this->paraverBody.insert(new ParaverEntry("2:" + itos(bucket->getcpu() + 1 ) + ":" +
                                itos(applid) + ":" + itos(task) + ":1:" +
                                ftos(j->getJobSimFinishTime())+":"+ itos(JOB_BWMEM_EVENT_TYPE) + ":0"
                                , bucket->getendTime(), ++this->lastid));
    }

    // this job has been killed ?
    if ( j->getJobSimisKilled() == true)
    {
        this->paraverBody.insert(new ParaverEntry("2:" + itos(bucket->getcpu()+1) + ":" + itos(applid) + ":" +
                            itos(task) + ":1:" + ftos(j->getJobSimFinishTime()) + ":" +
                            itos(JOB_STATE_EVENT_TYPE) + ":" + itos(JOB_STATE_KILLED_EVENT), bucket->getendTime(),
                            ++this->lastid));
    }
    else
    {
        this->paraverBody.insert(new ParaverEntry("2:" + itos(bucket->getcpu()+1) + ":" + itos(applid) + ":" +
                            itos(task) + ":1:" + ftos(j->getJobSimFinishTime()) + ":" +
                            itos(JOB_STATE_EVENT_TYPE) + ":" + itos(JOB_STATE_FINISH_EVENT), bucket->getendTime(),
                            ++this->lastid));
    }
    */
 
    task++;               
  } 
  
}

/**
 * Sets the Log to the ParaverTrace 
 * @param theValue The Log
 */
void ParaverTrace::setLog(Log* log)
{
  this->log = log;  
}


/**
 * Returns the last_arrival
 * @return A double containing the last_arrival
 */
double ParaverTrace::getlast_arrival() const
{
  return last_arrival;
}
	
/**
 * Sets the last_arrival to the ParaverTrace 
 * @param theValue The last_arrival
 */
void ParaverTrace::setLast_arrival(double theValue)
{
  last_arrival = theValue;
}

/**
 * Generates the header for the architecture 
 */
void ParaverTrace::generateMNArchitectureDefinition()
{ 
  MNConfiguration* config = (MNConfiguration*) this->architecture;


  /* row file stuff*/
  vector<string> rowCpus;
  vector<string> rowNodes;
  
  rowCpus.push_back("LEVEL CPU SIZE "+itos(config->getNumberCpus()));
  rowNodes.push_back("LEVEL NODE SIZE "+itos(config->getmarenostrum()->getTotalNodes()));
  
  /* Generating the architecture definition*/

  int nodes = config->getmarenostrum()->getTotalNodes();
  
  string sarchitecture = itos(nodes)+"(";
  
  for(int i = 0; i < config->getmarenostrum()->getTotalBlades() ; i++)
  {
    BladeCenter* blade =  config->getmarenostrum()->getBladeCenter(i);
         
    
    for(int j = 0; j < blade->getTotalNodes(); j++)
    {
      rowNodes.push_back("Blade "+itos(i)+".Node "+itos(j));
      log->debug("Paraver Header ROW Nodes -> Blade "+itos(i)+".Node "+itos(j),2);
      
      Node* node = blade->getNode(j);
      if(!(j == 0 && i == 0)) 
        sarchitecture+=","+itos(node->getNCPUS());      
      else
        sarchitecture+=itos(node->getNCPUS());              
        
      for(int k = 0; k < node->getNCPUS();k++)
      {
        rowCpus.push_back("Blade "+itos(i)+".Node "+itos(j)+".cpu "+itos(k));
        log->debug("Paraver Header ROW CPUS -> Blade "+itos(j)+".Node "+itos(i)+".cpu "+itos(k),2);
      }
    }
  }
  
  sarchitecture+=")";
  
  this->architectureDefintion = sarchitecture;
  
  /* generating the row file */

  string rowPath = this->dir+"/"+this->trace_name+".row";  
  ofstream row( rowPath.c_str(),ios::trunc);   
  
  assert(!row.fail());
  
  for(vector<string>::iterator it = rowCpus.begin(); it != rowCpus.end();++it)
   row << *it << endl;
  
  for(vector<string>::iterator it = rowNodes.begin(); it != rowNodes.end();++it)
   row << *it << endl;
  
  row.close();
  
  /* now we generate the cfg configuration file */
  string cfgPath = this->dir+"/"+this->trace_name+".pcf";  
  ofstream cfg( cfgPath.c_str(), ios::trunc );   
 
  assert(!cfg.fail());

  cfg << "STATES" << endl << endl;

  cfg << "0       NOT CREATED" << endl;
  cfg << "1       RUN" << endl;
  cfg << "2       WAITTING" << endl;
  cfg << "3       BLOCKED" << endl;
  cfg << "4       IDLE" << endl;
  cfg << "5       STOPPED" << endl;
  cfg << "6       NO_INFO" << endl;
  cfg << "7       FINISHED"  << endl  << endl;

  cfg << "STATES_COLOR" << endl << endl;

  cfg << "0       {0,0,0}" << endl;
  cfg << "1       {0,0,255}" << endl;
  cfg << "2       {150,240,240}" << endl;
  cfg << "3       {255,0,0}" << endl;
  cfg << "4       {255,255,102}" << endl;
  cfg << "5       {0,0,0}" << endl;
  cfg << "6       {204,204,204}" << endl;
  cfg << "6       {204,204,204}" << endl  << endl;

  cfg << "EVENT_TYPE" << endl << endl; 
  cfg << "10      "<<JOB_STATE_EVENT_TYPE<< "     JOB_STATE" << endl;
  cfg << "10      "<<JOB_BWMEM_EVENT_TYPE<< "     JOB_MEM" << endl;
  cfg << "10      "<<JOB_PENALIZEDTIMES_EVENT_TYPE<< "     JOB_PENALIZEDTIMES" << endl;
  cfg << endl;
  cfg << "VALUES" << endl << endl; 
  cfg << JOB_STATE_FINISH_EVENT<< "     JOB_FINISH" << endl;
  cfg << JOB_STATE_SUBMIT_EVENT<< "     JOB_SUBMIT" << endl;
  cfg << JOB_STATE_START_EVENT<< "     JOB_START" << endl;
  cfg << JOB_STATE_KILLED_EVENT<< "     JOB_STATE_KILLED_EVENT" << endl;
 
  cfg.close();
}

/**
 * Returns the total_jobs
 * @return A integer containing the total_jobs
 */
int ParaverTrace::gettotal_jobs() const
{
  return total_jobs;
}

/**
 * Sets the total_jobs to the ParaverTrace 
 * @param theValue The total_jobs
 */
void ParaverTrace::setTotal_jobs(const int& theValue)
{
  total_jobs = max(theValue,theValue);
}

/**
 * Returns the startJobTrace
 * @return A double containing the startJobTrace
 */
int ParaverTrace::getstartJobTrace() const
{
  return startJobTrace;
}

/**
 * Sets the startJobTrace to the ParaverTrace 
 * @param theValue The startJobTrace
 */
void ParaverTrace::setStartJobTrace(const int& theValue)
{
  startJobTrace = theValue;
}

/**
 * Returns the endJobTrace
 * @return A double containing the endJobTrace
 */
int ParaverTrace::getendJobTrace() const
{
  return endJobTrace;
}


/**
 * Sets the endJobTrace to the ParaverTrace 
 * @param theValue The endJobTrace
 */
void ParaverTrace::setEndJobTrace(const int& theValue)
{
  endJobTrace = theValue;
}

}

