#include <scheduling/simulation.h>
#include <statistics/statistics.h>
#include <scheduling/gridjob.h>

namespace Simulator {

/**
 * The default constructor for the class
 */

Simulation::Simulation() 
{
}

/**
 * The constructor that should be used.  
 * @param SimInfo A reference to the simulation configuration 
 * @param log A reference to the logging engince
 * @param policy A reference to the policy that will be simulated 
 */
Simulation::Simulation(SimulatorCharacteristics* SimInfo,Log* log,SchedulingPolicy* policy) 
{
  this->lastEventId = 0;
  this->policy = policy;
  this->globaltime = 0; 
  this->policy = policy;
  this->SimInfo = SimInfo;
  this->log = log;
  this->ArrivalFactor = 1;
  this->pendigStatsCollection = 0;  
  
  //we copy a reference of the job list in to the scheduling policy that will be used by some sched policies
  this->policy->setJobList(& this->SimInfo->getworkload()->JobList);
  this->collectStatisticsInterval = 10*60;  //by default each 10 minuts the policy statistics will be collected
  
  this->policyCollector = new PolicyEntityCollector(policy,log);
  this->JobsSimulationPerformance  = NULL;
  this->PolicySimulationPerformance = NULL;
  
  this->StopSimulation = false;
  
  //regarding the progress stuff 
  this->firstShow = true;
  this->showSimulationProgress = false;  
  this->totalItems = 100;
  this->lastEventType = EVENT_OTHER;
  this->lastProcessedJob = 0;

  this->usefull = 0;
  
  this->numberOfFinishedJobs = 0;
}



/**
 * The default destructor for the class 
 */

Simulation::~Simulation()
{
}

/**
 * Function that initialize the simulation. It mainly create all the arrival events for all the jobs that have to be simulated. 
 * @return A bool indicating if the initilitzation has been done with  no errors.
 */
bool Simulation::initSimulation()
{
  /******************************
  * initialize global data structures.
  * this includes loading all jobs into the event queue according to their
  * arrival times.
  */
  TraceFile* workload =  this->SimInfo->getworkload();
  
  /* we create one event arrival for eahc of the jobs */
  map<int,Job*>::iterator iter = workload->JobList.begin();         
     
  Job* firstArrival = iter->second;   
     
  for(;iter != workload->JobList.end();++iter)
  {
    Job* job = (Job*) iter->second;
    
    /*at this time we do not modelize what happends with jobs with runtime 0 */
    if(job->getRunTime() == 0)
    {
      log->debug("The job "+itos(job->getJobNumber())+" has runtime zero, we skip it",2);
      continue;
    }        
    
    /* we compute the submitt time according the arrival factor */
    double submittime = ceil(job->getJobSimSubmitTime()*this->ArrivalFactor);
    job->setJobSimSubmitTime(submittime);
    
    /*it's time to create an event arrival for each job*/
    if((this->SimInfo->getarchConfiguration() == 0 || 
       job->getNumberProcessors() <= this->SimInfo->getarchConfiguration()->getNumberCpus()) &&
       job->getNumberProcessors() != -1)  
    {
      SimulationEvent* arrivalEvent = new SimulationEvent(EVENT_ARRIVAL,job,++this->lastEventId,submittime);
      log->debug("Adding an EVENT_ARRIVAL event for the job "+ itos(job->getJobNumber())+ " the event id is "+itos(this->lastEventId)+ " the time for the event is "+ftos(submittime));
      this->events.insert(arrivalEvent);
    }
    else
    {  
      job->setJobSimStatus(KILLED_NOT_ENOUGH_RESOURCES); 
      log->error("The job "+itos(job->getJobNumber())+" uses "+itos(job->getNumberProcessors())+" and the architecture has less processors.");
    }
        
  }
  
  //we set the simulation start time to the initial starttime 
  this->globaltime = firstArrival->getJobSimSubmitTime();
  
  //we create the event for collecting the statistics
  SimulationEvent* nextCollection = new SimulationEvent(EVENT_COLLECT_STATISTICS,0,++this->lastEventId,globaltime+this->collectStatisticsInterval);
  this->events.insert(nextCollection);
  this->pendigStatsCollection++;
}

/**
 * This is the main loop simulation function. Here all the events are being treated by the simulator. 
 * @return A bool indicating if the simulation has been run succesfully.
 */
bool Simulation::doSimulation()
{
  bool getStartTimeFirstJob = false;
  double startTimeFirstJob;

double total_usage = 0;


  //we tell to the policy that the scheduling will start now 
  this->policy->PreProcessScheduling();
      
  int ConsecutiveStatisticsCollection = 0;
  
  double terminatedJobs = 0;
  double totalJobs = this->SimInfo->getworkload()->getLoadedJobs();
  
  /* As we are working with an FCFSF scheduling class we convert the pointer for commodity*/  
  while(this->events.size() > 0 && continueSimulation())
  {
    /*event treatment */    
    EventQueue::iterator iter = this->events.begin();    
    
    SimulationEvent* event = *iter;
    this->currentEvent = event;
    
    /* update the global to the current time for all the instances that use this time */
    assert(globaltime <= event->getTime());

    this->globaltime = event->getTime();
    this->policy->setGlobalTime(event->getTime());
    Job* job = event->getjob();
    this->currentJob = job;

/*
    GridJob *gjob = (GridJob *)job;


Metric* infxMetric = gjob->getClockSpeedRequirement()->getmetric();
Metric* infyMetric = gjob->getMemoryRequirement()->getmetric();
Metric* supxMetric = gjob->getNumberProcessorsRequirement()->getmetric();
Metric* supyMetric = gjob->getDiskRequirement()->getmetric();

double infx = infxMetric->getnativeDouble();
double infy = infyMetric->getnativeDouble();
double supx = supxMetric->getnativeDouble();
double supy = supyMetric->getnativeDouble();

int window = job->getUserID();
int cluster = job->getGroupID();
*/
//delete infxMetric;
//delete infyMetric;
//delete supxMetric;
//delete supyMetric;

    
    if(log->getlevel() > 8)
      this->dumpEvents();
  
    
    
    
    /*we update the global time to the current event time .. */        
    switch(event->gettype())
    {    
      case EVENT_TERMINATION:
      {
        if(policy->computePower())
        {
       	  policy->updateEnergyConsumedInPower(globaltime, job);

          /* rigth now the unique stuff to do is to terminate the job */      
         //RUTGERS 
	 /*if (policy->getPowerAware() && job->getRunsAtReducedFrequency())
            policy->decreaseNumberOfReducedModeCPUS(job);
	*/
        }

        log->debug("EVENT_TERMINATION for the job "+ itos(job->getJobNumber()),2);
        policy->jobRemoveFromRT(job);
        
        /* we set the variables related to the job */
        setJobPerformanceVariables(job);
        
	double wtime = job->getJobSimWaitTime();
	double rtime = job->getRunTime();
	int cpus = job->getNumberProcessors();
	double response = wtime + rtime;

	double usage = cpus*rtime;

	total_usage+=usage;

        /* for progress indication */
        terminatedJobs++;
        numberOfFinishedJobs++;
        
        
        break;
      }  
      case EVENT_ABNORMAL_TERMINATION:
      {
        if(policy->computePower())
        {
          policy->updateEnergyConsumedInPower(globaltime, job);

          /*
	  if (policy->getPowerAware() && job->getRunsAtReducedFrequency())
            policy->decreaseNumberOfReducedModeCPUS(job);        
	  */
        }
 
        log->debug("EVENT_ABNORMAL_TERMINATION for the job "+ itos(job->getJobNumber()),2);
        /* rigth now the unique stuff to do is to terminate the job */      
        policy->jobRemoveFromRT(job);
        
        /* we set the variables related to the job */
        setJobPerformanceVariables(job);
        
        /* for progress indication */
        terminatedJobs++;

       
        
        break;
      } 
      case EVENT_START:
      {
        /* we just more the job from the wait queue to the runque
           and also allocate the processes to the real processors            
        */
      
        if (!getStartTimeFirstJob)
        {
          getStartTimeFirstJob = true;
          startTimeFirstJob = globaltime;

          // policy can play with this kind of data
          policy->setFirstStartTime(startTimeFirstJob);
        }  
 

	//RUTGERS
	/*
        usefull += job->getNumberProcessors() * job->getRunTime();

        if(policy->computePower())
        {
          policy->updateEnergyConsumedInPower(globaltime);

          if (policy->getPowerAware() && job->getRunsAtReducedFrequency()) 
            policy->increaseNumberOfReducedModeCPUS(job);
        }
	*/


        log->debug("EVENT_START for the job "+ itos(job->getJobNumber()),2);
        policy->jobWaitq2RT(job);
        policy->jobStart(job); 
               
        /* we also will show the progress information at this point */       
        if(this->showSimulationProgress)
          showProgress(terminatedJobs,totalJobs);       

        
          
        break;
      }      
      case EVENT_ARRIVAL:
      { 

/*
if(job->getJobNumber() == 911){

int a = 2*3;

}
*/
        log->debug("EVENT_ARRIVAL for the job "+ itos(job->getJobNumber()),2);
        /* first put the job to the wait queue and allocate it to the reservationTable */       
        policy->jobAdd2waitq(job);
        /*create  event for the job start - in case the start time and finish time has been assigned */        
          			
          if(job->getJobSimStartTime() != -1 && job->getJobSimFinishTime() != -1)
        {  
          SimulationEvent* startjob = new SimulationEvent(EVENT_START,job,++this->lastEventId,job->getJobSimStartTime());
        
          log->debug("Adding an EVENT_START event for the job "+ itos(job->getJobNumber())+ " the event id is "+itos(this->lastEventId)+ " the time for the event is "+ftos(job->getJobSimStartTime()),2);
        
        /*we create a job termination for the job .. if it fails, or must be killed*/
          SimulationEvent* finishjob;
          if(job->getStatus() == COMPLETED)   
          {
            finishjob = new SimulationEvent(EVENT_TERMINATION,job,++this->lastEventId,job->getJobSimFinishTime());
            log->debug("Adding an EVENT_TERMINATION event for the job "+ itos(job->getJobNumber())+ " the event id is "+itos(this->lastEventId)+ " the time for the event is "+ftos(job->getJobSimFinishTime()),2);
          }
          else
          {  
            finishjob = new SimulationEvent(EVENT_ABNORMAL_TERMINATION,job,++this->lastEventId,job->getJobSimFinishTime());
            log->debug("Adding an EVENT_ABNORMAL_TERMINATION event for the job "+ itos(job->getJobNumber())+ " the event id is "+itos(this->lastEventId)+ " the time for the event is "+ftos(job->getJobSimFinishTime()),2);
          }
          this->events.insert(startjob);
          this->events.insert(finishjob);
        }
        
        break;
      }
      case EVENT_COLLECT_STATISTICS:
      {        
                      
        log->debug("Processing a EVENT_COLLECT_STATISTICS envent",2);
        this->pendigStatsCollection--;        
        //we generate and event for the next collection only if there is more than one event , coz in case there is one element in the event queue
        //will mean that this event is the collection event 
        if(this->events.size() > 1 && pendigStatsCollection <= MAX_ALLOWED_STATSISTIC_EVENTS)
        {
          SimulationEvent* nextCollection = new SimulationEvent(EVENT_COLLECT_STATISTICS,0,++this->lastEventId,globaltime+this->collectStatisticsInterval);
          this->events.insert(nextCollection);
          this->pendigStatsCollection++;
        }                
        
        if(lastEventType == EVENT_COLLECT_STATISTICS)
        {
          ConsecutiveStatisticsCollection++;
        }
        else
          ConsecutiveStatisticsCollection = 0;
        
        //we collect the current statistics
        if(ConsecutiveStatisticsCollection < 5)        
          this->collectStatistics();
        break;
      }      
      default: 
      {  assert(false);
        break;
      }
    }
    
    this->events.erase(iter);
    lastEventType = event->gettype();
    
    delete event;
    
    if(job != NULL)
      /* this is only for debug stuff for check the last processed job in case of failure */
      lastProcessedJob = job;
    
  }
  
  log->debug("Simulation has finished, we generate all the statistics and information for the analysis.",2);

/*
	double wtime = job->getJobSimWaitTime();
	double rtime = job->getRunTime();
	int cpus = job->getNumberProcessors();
	double response = wtime + rtime;

	double usage = cpus*rtime;
*/


  double utilization = total_usage*100/((globaltime-policy->starttime)*policy->architecture->getNumberCpus());

  int cpuss = policy->architecture->getNumberCpus();

  cout<<endl<<endl<<"Total usage in cpus*s="<<ftos(total_usage)<<" and util percentage is:"<<utilization<<"%"<<endl<<endl;

  this->collectFinalStatistics();
  this->policy->PostProcessScheduling();

  if (policy->computePower())
    /* print some information about power consumed during workload */
    policy->dumpGlobalPowerInformation(globaltime, usefull);
  
}

/**
 * Function currently not used - it would do some preproces before the simulation start 
 * @return A bool indicating if some problem has raised. 
 */
bool Simulation::loadSimulation()
{
}

/**
 * Shows that current statistics concerning the jobs that have been simulated and to the policy. The statistics are shown in the STDOUT.
 */
void Simulation::showCurrentStatistics()
{

  /* first we generate the statistics */
  SimStatistics* stats = this->SimInfo->getsimStatistics();
  TraceFile* workload =  this->SimInfo->getworkload();
  
  stats->computeJobMetrics(&workload->JobList);
  stats->computePolicyMetrics(this->policyCollector);
  stats->ShowCurrentStatisticsStd();
  
  std::cout << "Current global time:"+ ftos(this->globaltime)  << endl;
  
  if(lastProcessedJob != NULL)
    std::cout << "Last Job treated: "+itos(lastProcessedJob->getJobNumber())  << endl; ;
  
  std::cout << "Current Event:"+getStringEvent(lastEventType) << endl;
  
  this->firstShow = true;
  
}

/**
 * Returns the human readable description of an event type.
 * @param type The type for the event.
 * @return The string with the human readable 
 */
string Simulation::getStringEvent(event_t type)
{
  switch(type)
  {  
    case EVENT_TERMINATION:
    {
      return "EVENT_TERMINATION";
      break;
    }
    case EVENT_ABNORMAL_TERMINATION:
    {
      return "EVENT_ABNORMAL_TERMINATION";
      break;
    }
    case EVENT_START:
    {
      return "EVENT_START";
      break;
    }
    case EVENT_ARRIVAL:
    {
      return "EVENT_ARRIVAL";
      break;
    } 
    case EVENT_COLLECT_STATISTICS:
    {
      return "EVENT_COLLECT_STATISTICS";
      break;
    }
    default:
   {
      return "OTHER";
      break;
   }
  }
}

/**
 * Function that gathers both job and policy statistics and saves to the output file if specified, otherwise to the standard output.
 */
void Simulation::collectFinalStatistics()
{

  if(numberOfFinishedJobs < 1)
  {
    log->debug("No finished jobs. We do not compute the statistics.");
    return;
  }
  
  SimStatistics* stats = this->SimInfo->getsimStatistics();
  TraceFile* workload =  this->SimInfo->getworkload();

  
  /*in case of required we dump the performance for the jobs */
  if(this->JobsSimulationPerformance != NULL)
  {
    this->JobsSimulationPerformance->createJobsCSVInfoFile(&workload->JobList);
  }
  
  if(this->PolicySimulationPerformance != NULL)
  {
    this->PolicySimulationPerformance->createPolicyCSVInfoFile(this->policyCollector);
  }
  
  /*  we generate the statistics */
  
  stats->computeJobMetrics(&workload->JobList);
  stats->computePolicyMetrics(this->policyCollector);
  stats->saveGlobalStatistics();

}

/**
 * Indicates if the simulation should be finished 
 * @return A bool indicating if the simulation should be stopped 
 */
bool Simulation::continueSimulation()
{
  
  bool continueSimulation = !this->getStopSimulation();
  
  return continueSimulation;
}


/**
 * Sets the paraver trace for the current simulation
 * @param trace A reference to the paraver trace 
 */
void Simulation::setParaverTrace(ParaverTrace* trace)
{
  this->policy->setPrvTrace(trace); 
}

/**
 * Returns the current globaltime of the simulation 
 * @return An integer with the global time 
 */
double Simulation::getglobaltime() const
{
  return globaltime;
}


/**
 * Sets the globaltime for the simulation 
 * @param theValue The integer containing the global time for the simulation 
 */
void Simulation::setGlobaltime(double theValue)
{
  globaltime = theValue;
}

/**
 * This function is mainly used for debugging or for state that the simulation is performing as expected. It will dump all the pending events to the debug engine.
 */
void Simulation::dumpEvents() 
{

  
  EventQueue::iterator it = this->events.begin(); // should be optimized !
  
  for(;it != this->events.end();++it)
  {
    SimulationEvent* cEvent = *it;
    
    /* debug stuff */
    if(this->log->getlevel() > 8 && (cEvent->gettype() == EVENT_START || cEvent->gettype() == EVENT_TERMINATION))
        this->log->debug("Event type "+itos(cEvent->gettype())+" with the id "+itos(cEvent->getid()) +" that is scheduled by "+ftos(cEvent->getTime())+" for the job "+itos(cEvent->getjob()->getJobNumber()),2);
        

  }
}


/**
 * Delete all the events that concerns the provided job. This is used in situations when the job has been rescheduled and the events that was previously generated have lost its validity.
 * @param job The job to whom events have to be deleted 
 */
void Simulation::deleteJobEvents(Job* job) 
{
  vector<SimulationEvent*> toDelete;
  EventQueue::iterator it = this->events.begin(); // should be optimized !
  
  for(;it != this->events.end();++it)
  {
    SimulationEvent* cEvent = *it;
    
    /* debug stuff */
    if(this->log->getlevel() > 8 && (cEvent->gettype() == EVENT_START || cEvent->gettype() == EVENT_TERMINATION))
        this->log->debug("Event type "+itos(cEvent->gettype())+" with the id "+itos(cEvent->getid()) +" that is scheduled by "+ftos(cEvent->getTime())+" for the job "+itos(cEvent->getjob()->getJobNumber()),2);
        
    if(cEvent->gettype() != EVENT_COLLECT_STATISTICS  && cEvent->getjob()->getJobNumber() == job->getJobNumber() && currentEvent->getid() != cEvent->getid())
    {
       this->log->debug("Deleting the event type "+itos(cEvent->gettype())+" with the id "+itos(cEvent->getid()) +" that is scheduled by "+ftos(cEvent->getTime())+" for the job "+itos(cEvent->getjob()->getJobNumber()),2);
       toDelete.push_back(cEvent);
       continue;
    }
    
   /* debug stuff */
   if(this->log->getlevel() > 8 && (cEvent->gettype() == EVENT_START || cEvent->gettype() == EVENT_TERMINATION))
        this->log->debug("Event type "+itos(cEvent->gettype())+" with the id "+itos(cEvent->getid()) +" that is scheduled by "+ftos(cEvent->getTime())+" for the job "+itos(cEvent->getjob()->getJobNumber()),2);
  }
  
  for(vector<SimulationEvent*>::iterator itdel = toDelete.begin(); itdel != toDelete.end();++itdel)
  {
    SimulationEvent* ev = *itdel;
    EventQueue::iterator it = this->events.find(ev); // should be optimized !
    assert(it != this->events.end());
    
    this->events.erase(it);
  }
}

/**
 * Delete the finish pending events for the required job. 
 * @param job The job to whom events have to be deleted
 */
void Simulation::deleteJobFinishEvent(Job* job) 
{

  if((job == this->currentJob && this->currentEvent->gettype() == EVENT_ARRIVAL))
   return;
  
  
  EventQueue::iterator it = this->events.begin(); // should be optimized !
  
  for(;it != this->events.end();++it)
  {
    SimulationEvent* cEvent = *it;
    
    if( cEvent->gettype() != EVENT_COLLECT_STATISTICS  && cEvent->getjob()->getJobNumber() == job->getJobNumber() && currentEvent->getid() != cEvent->getid()
       && (cEvent->gettype() == EVENT_TERMINATION || cEvent->gettype() == EVENT_ABNORMAL_TERMINATION) 
      )
    {
       this->log->debug("Deleting the event type "+itos(cEvent->gettype())+" with the id "+itos(cEvent->getid()) +" that is scheduled by "+ftos(cEvent->getTime())+" for the job "+itos(cEvent->getjob()->getJobNumber()),2);
       this->events.erase(it);
       delete cEvent;
       //we can return due to there should not be more than one event termination per jobs 
       return;
    }
    
  }
}


/**
 * Function that generate all the required events for force a job finish in the provided time. 
 * @param endTime The time when the job will finish 
 * @param job The job to whom finish event has to be created
 */
void Simulation::forceJobFinish(double endTime,Job* job)
{
  //debugging stuff
  if(this->log->getlevel() > 8)
    this->dumpEvents();

  /* If the job is the one that is currently processed and it is a start time we do not have to create a finish event 
     it will be created in the 
  */
  if(!(job == this->currentJob && this->currentEvent->gettype() == EVENT_START))
  {
    log->debug("Reescheduling the job "+ itos(job->getJobNumber())+ ". Adding an EVENT_TERMINATE "+ftos(endTime),2);

    /*we create a job termination for the job .. if it fails, or must be killed*/
    SimulationEvent* finishjob  = NULL;
  
    if(job->getStatus() == COMPLETED)   
    {
      finishjob = new SimulationEvent(EVENT_TERMINATION,job,++this->lastEventId,endTime);
      log->debug("Adding an EVENT_TERMINATION event for the job "+ itos(job->getJobNumber())+ " the event id is "+itos(this->lastEventId)+ " the time for the event is "+ftos(endTime),2);
    }
    else
    {  
      finishjob = new SimulationEvent(EVENT_ABNORMAL_TERMINATION,job,++this->lastEventId,endTime);
      log->debug("Adding an EVENT_ABNORMAL_TERMINATION event for the job "+ itos(job->getJobNumber())+ " the event id is "+itos(this->lastEventId)+ " the time for the event is "+ftos(endTime),2);
    }
   
    this->events.insert(finishjob);

  }
  
  //debugging stuff
  if(this->log->getlevel() > 8)
    this->dumpEvents();
}

/**
 * Function that generate all the required events for force a job start in the provided time. 
 * @param startTime The start time for the new start event 
 * @param endTime The end time for the new  finialization event 
 * @param job The job to whom finish event has to be created 
 */
void Simulation::forceJobStart(double startTime,double endTime,Job* job)
{  
  //debugging stuff
  if(this->log->getlevel() > 8)
    this->dumpEvents();

  //In caset that this job is the once that is beeing scheduled and the current event is EVENT_ARRIVAL we do not 
  //create any EVENT TERMINATION due to this events will be created in the main doSimulation method
  if(!(job == this->currentJob && this->currentEvent->gettype() == EVENT_ARRIVAL))
  {
    /*create  event for the job start*/        
    SimulationEvent* startjob = new SimulationEvent(EVENT_START,job,++this->lastEventId,startTime);
                
    /*we create a job termination for the job .. if it fails, or must be killed*/
    SimulationEvent* finishjob  = NULL;

    
    log->debug("Reescheduling the job "+ itos(job->getJobNumber())+ ". Adding an EVENT_START  the event id is "+itos(this->lastEventId)+ " the time for the event is "+ftos(startTime),2);

  
    if(job->getStatus() == COMPLETED)   
    {
      finishjob = new SimulationEvent(EVENT_TERMINATION,job,++this->lastEventId,endTime);
      log->debug("Adding an EVENT_TERMINATION event for the job "+ itos(job->getJobNumber())+ " the event id is "+itos(this->lastEventId)+ " the time for the event is "+ftos(endTime),2);
    }
    else
    {  
      finishjob = new SimulationEvent(EVENT_ABNORMAL_TERMINATION,job,++this->lastEventId,endTime);
      log->debug("Adding an EVENT_ABNORMAL_TERMINATION event for the job "+ itos(job->getJobNumber())+ " the event id is "+itos(this->lastEventId)+ " the time for the event is "+ftos(endTime),2);
    }
    
    this->events.insert(startjob);
    this->events.insert(finishjob);

  
  }
  
  //debugging stuff
  if(this->log->getlevel() > 8)
    this->dumpEvents();
   
}

/**
 * This function updates the performance variables for the job 
 * @param job A reference to the job that has finished 
 */
void Simulation::setJobPerformanceVariables(Job* job)
{
  job->setJobSimWaitTime(job->getJobSimStartTime()-job->getJobSimSubmitTime());

  if(policy->getCostAllocationUnit() > 0)
  {
    job->setMonetaryCost((job->getRunTime()/3600)*policy->getCostAllocationUnit());
  }

  //compute the BSLD considering the frequency and set it to the job 
// it is more fair to divide with the original runtime (execution time at the top frequency)
  if(job->getRunsAtReducedFrequency())
  {
    double runtime = job->getRunTime()* this->policy->getRatio(job->getJobNumber(),job->getRunsAtFrequency());
 
    job->setJobSimSLD((job->getJobSimWaitTime()+runtime)/job->getRunTime());
    job->setJobSimBSLD((job->getJobSimWaitTime()+runtime)/max(job->getRunTime(),(double)BSLD_THRESHOLD));

  }

}

/**
 * Function that shows to the standard output the progress of the simulation , the progress of the simulation is the ratio of jobs that have run and the jobs that have already finishied 
 * @param current The current number of finished jobs 
 * @param total The total number of jobs in the simulation
 */
void Simulation::showProgress (double current,double total)
{
  double progress = ceil((current/total)*100);
  double items = ceil(totalItems*(current/total));
  
  string prg_str = ftos(progress);
  
  if(prg_str.size() == 1)
    prg_str = "  "+prg_str;
  else if(prg_str.size() == 2)
    prg_str = " "+prg_str;
    
  if(this->firstShow)
  {    
    this->firstShow = false;
    std::cout << "Progress simulation:";
  }
  else
  {
    std::cout << ("\b\b\b\b\b\b");
    
    for(int i = 0; i < this->totalItems;i++)
    {
      std::cout << ("\b");
    }
  }
  
  std::cout.flush();
  std::cout <<  prg_str+" %"; 
  
  for(int i = 0;i < this->totalItems; i++)
  {
    if(i < items)  
     std::cout << "*" ;
    else 
     std::cout << " " ;     
  }
  
  std::cout << "|";
  
  std::cout.flush();

}

/**
 * Returns the amount of time between two statistics collection 
 * @return  A double containing the invertal between two statistics collection 
 */
double Simulation::getcollectStatisticsInterval() const
{
  return collectStatisticsInterval;
}


/**
 * Sets the interval of statistics collection 
 * @param theValue The interval of statiscs collection
 */
void Simulation::setCollectStatisticsInterval(double theValue)
{
  collectStatisticsInterval = theValue;
}

/**
 * Invoqued when the statistics event is raised. 
 */
void Simulation::collectStatistics()
{
  this->policyCollector->pullValues(this->globaltime);
}

double Simulation::getArrivalFactor() const
{
  return ArrivalFactor;
}


/**
 * Sets the arrival factor for the jobs 
 * @param theValue The arrival factor
 */
void Simulation::setArrivalFactor(double theValue)
{
  ArrivalFactor = theValue;
}

/**
 * Returns the CVS streamer for the job information
 * @return A reference to the job cvs job infor converter 
 */
CSVJobsInfoConverter* Simulation::getJobsSimulationPerformance() const
{
  return JobsSimulationPerformance;
}


/**
 * Sets the CVS streamer for the job information
 * @param theValue The reference to the converter
 */
void Simulation::setJobsSimulationPerformance(CSVJobsInfoConverter* theValue)
{
  JobsSimulationPerformance = theValue;
}


/**
 * Returns true if the simulation has to be stoped or not 
 * @return A bool indicating if the simulation has to be stoped 
 */
bool Simulation::getStopSimulation() const
{
  return StopSimulation;
}


/**
 * Sets if the simulation has to be stoped or not.
 * @param theValue The bool indicating if to stop the simulation
 */
void Simulation::setStopSimulation(bool theValue)
{
  StopSimulation = theValue;
}

/**
 * Returns true if the progress of the simulation has to be shown in the standard output
 * @return A bool indicating if the progress will be shown in the std out 
 */
bool Simulation::getshowSimulationProgress() const
{
  return showSimulationProgress;
}


/**
 * Sets if the simulation progress has to be shown in the standard output
 * @param theValue Indicates if the progress has to be shown 
 */
void Simulation::setShowSimulationProgress(bool theValue)
{
  showSimulationProgress = theValue;
}



/**
 * Returns the CVS streamer for the policy information
 * @return A reference to the job cvs policy infor converter 
 */
CSVPolicyInfoConverter* Simulation::getPolicySimulationPerformance() const
{
  return PolicySimulationPerformance;
}

/**
 * Sets the CVS streamer for the policy information
 * @param theValue The reference to the converter
 */
void Simulation::setPolicySimulationPerformance(CSVPolicyInfoConverter* theValue)
{
  PolicySimulationPerformance = theValue;
}

void Simulation::initPower(PowerConsumption* powerinf)
{
  policy->initializationPower(powerinf);
}


}
