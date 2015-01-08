#include <scheduling/isisdispatcher.h>

#include <scheduling/gridbackfilling.h>
#include <scheduling/gridanalogicalreservationtable.h>

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>




namespace Simulator {

/**
 * The default constructor for the class
 */
ISISDispatcherPolicy::ISISDispatcherPolicy()
{
  UsePredictionService = false;
  predictionService = NULL;
  this->schedulingPolicySimulated = ISIS_DISPATCHER;
}



/**
 * This is the constructor that should be used, invoques that constructor for the parent classes  
 * @param log A reference to the logging engine
 * @param UsePredictionService A bool indicating if the prediction service has to be used or not 
 * @param predictionService A reference to the prediction service that will be used by the policy for estimate the job runtime 
 */
ISISDispatcherPolicy::ISISDispatcherPolicy(Log* log,bool UsePredictionService,Predictor* predictionService)
{
  //by default we will create a FCFSC 
  this->waitQueue = new FCFSJobQueue();
  
  //as all the scheduling policies must have associated a Reservation table by definition 
  //we create one rt for the however in the future we may have a more sophisticate one 
  //this is a mather of the model.. the scheduling approach is not centralized 
  this->reservationTable = new GridAnalogicalReservationTable();

  this->log = log;
  
  this->UsePredictionService = UsePredictionService;
  this->predictionService = predictionService;
  this->schedulingPolicySimulated = ISIS_DISPATCHER;  
  this->ADSRandom = 0;
 
}


/**
 * The default destructor for the class 
 */
ISISDispatcherPolicy::~ISISDispatcherPolicy()
{
  //before destroy all the stuff, we will iterate over all the policies, and we will generate the traces 
  for(vector<SchedulingPolicy*>::iterator polit = this->ISISDispatcherPolicies->begin();
      polit != this->ISISDispatcherPolicies->end();
      ++polit)
  {
    SchedulingPolicy* pol = *polit;    
    delete pol;
  }
  
  delete this->ISISDispatcherPolicies;
  
  for(vector<ArchitectureConfiguration*>::iterator archit = this->ISISDispatcherArchitectures->begin();
     archit != this->ISISDispatcherArchitectures->end();
     ++archit)
  {
    ArchitectureConfiguration* arch = *archit;
    delete arch;
  }
   
  delete this->reservationTable;;  
  delete this->ISISDispatcherCentersName;
  delete this->ISISDispatcherParaverTracesName;
  delete this->ISISDispatcherEmulateCPUFactor;
  
}

/**
 * Invoqued when the global scheduling has finished it mainly creates the traces for each center 
 */
void ISISDispatcherPolicy::PostProcessScheduling()
{
  //before destroy all the stuff, we will iterate over all the policies, and we will generate the traces 
  for(vector<SchedulingPolicy*>::iterator polit = this->ISISDispatcherPolicies->begin();
      polit != this->ISISDispatcherPolicies->end();
      ++polit)
  {
    SchedulingPolicy* pol = *polit;  
    ParaverTrace* trace = pol->getprvTrace();
    
    if(pol->getuseParaver())
    {
      log->debug("Closing paraver traces and genarting it content...");
      trace->setTotal_jobs(this->SubmittedJobs[pol]);
      trace->setLast_arrival(this->globaltime);
      trace->close();
    }    
    
  }

}

/**
 * Adds a given job to the wait queue, usually when it arrives to the system.
 * this function does not call to the parent class scheduling policy due to it will be called in each policy instantiated    
 * The current approach will iterater over each center and will ask for a performance estimation 
 * that the given job would achieve in the given center with the current load. 
 * @see The class SchedulingPolicy 
 * @param job the job to be added to the wait queue
 */
void ISISDispatcherPolicy::jobAdd2waitq(Job* job)
{   
  Metric* bestMetric = NULL;
  SchedulingPolicy* bestPolicy = NULL;
  int choise = 0; 
  int count = 0;  
  
  //this map will store the predictions for the center
  map<int,double> predictions;
  map<int,double> predictionsWT;

  //for evaluate the impact of knowing only a part of the system we do a kind of random knownjob
  map<int,bool> knownCenters;
  map<int,bool> evaluatedCenters;


  if(this->amountOfKnownCenters != -1)
  {  

    int selected = 0; 
    
    while(selected < this->amountOfKnownCenters)
    {        
        int random_centr = (int) this->ADSRandom->DeltaProx();        
        
        

        SchedulingPolicy* pol = this->ISISDispatcherPolicies->operator[] (random_centr);
      
        map<int,bool>::iterator ite = evaluatedCenters.find(random_centr);

        log->debug("Evaluating center "+itos(random_centr),2);

        if(ite == evaluatedCenters.end())
        {
           log->debug("New center evaluated"+itos(random_centr));
           evaluatedCenters.insert(pair<int,bool>(random_centr,true));
        }
        else
        {
          log->debug("evaluated Centers "+itos(evaluatedCenters.size())+ "total "+itos(this->ISISDispatcherPolicies->size()),2);

          for( map<int,bool>::iterator itt = evaluatedCenters.begin() ; itt != evaluatedCenters.end();++itt )
          { int cen = itt->first; log->debug("Center "+itos(cen));}

           //if all the centers have been evaluated we go ahead
          if(evaluatedCenters.size() == this->ISISDispatcherPolicies->size())
          {
             log->debug("All the centers have been evaluated for the job.",2);
             break;
          }
        }
        
        map<int,bool>::iterator itc = knownCenters.find(random_centr);        

         //we suppose that the ADS can return as a valid center
        if(itc == knownCenters.end() && pol->getarchitecture()->getNumberCpus() >= job->getNumberProcessors())
        {        
          log->debug("New center selected "+itos(random_centr)+" form total of "+itos(this->amountOfKnownCenters),2); 
          knownCenters.insert(pair<int,bool>(random_centr,true));
          selected++;
        }
    }

  }
 
  for(vector<SchedulingPolicy*>::iterator polit = this->ISISDispatcherPolicies->begin();
      polit != this->ISISDispatcherPolicies->end();
      ++polit)
  {
    SchedulingPolicy* pol = *polit;
    
    string center = this->ISISDispatcherCentersName->operator[] (count);      
    
    //first we check if the dispathcer would be aware for this policy    
    if(this->amountOfKnownCenters != -1)
    {
      map<int,bool>::iterator itc = knownCenters.find(count);

      if(itc == knownCenters.end())           
      {
        count++;
        continue; 
      }
    }

    //second we have to check if there are enough cpus in the given center for this job 
    if(pol->getarchitecture()->getNumberCpus() < job->getNumberProcessors())
    {
      log->debug("The center "+center+" has not enough cpus for run the job "+itos(job->getJobNumber()),2);
      count++;
      continue;
    }

    
    
    Prediction* runtimePrediction = NULL;    

    //We will estimate the job runtime if it was set or not 
    if(this->UsePredictionService)
    {      
      job->setJobSimCenter(center); //this is only temporary 
      
      runtimePrediction  = this->predictionService->estimateJobPerformance(job);
      predictions.insert(pair<int,double>(count,runtimePrediction->getprediction()));

      assert(runtimePrediction->getprediction() > 0); 

      //we set the requested runtime according the new prediction for the job 
      job->setRequestedTime(runtimePrediction->getprediction());
      
      log->debug("The prediction for the job "+itos(job->getJobNumber())+" in the center "+center+" is "+ftos(runtimePrediction->getprediction()),2);
    }
    
    //in case the center has a maximum allowed runtime (for example if we are modelling queues) we have to check if the maximum allowed runtime for the job is less than the requested runtime 
    if(pol->getmaxAllowedRuntime() != -1 && job->getRequestedTime() > pol->getmaxAllowedRuntime())
    {
      log->debug("The center "+center+" a limit runtime lower than the runtime for the job "+itos(job->getJobNumber()),2);
      count++;
      continue;
    
    }


    double estimatedSlowdown = -1;

    /* depending on the mmetric 2 optmize required , more sohpisticate compuations have to be carried out  */
    if(this->ISISDispatcherMetric2Optimize == ESTIMATED_COMPUTATIONAL_COST || this->ISISDispatcherMetric2Optimize == SLD_PREDICTED)
    {
      //first we require to the scheduler to provide an estimation of how long the job will have to wait until get run 
      //we also set it to the job wait time estimation 
         
      Metric* waitTimeMetric = pol->EstimatePerformanceResponse(WAITTIME,job);
      double estimatedWT = waitTimeMetric->getnativeDouble();
      job->setWaitTimePrediction(estimatedWT);
      predictionsWT.insert(pair<int,double>(count,estimatedWT));
        //remember to set up the prediction service       
      assert(runtimePrediction != NULL);
      double estimatedRT = runtimePrediction->getprediction(); 
      assert(estimatedRT > 0);
 
      estimatedSlowdown = (estimatedWT+estimatedRT)/estimatedRT;
      log->debug("The predicted slowdown for the job "+itos(job->getJobNumber())+" in the center "+center+" is "+ftos(estimatedSlowdown),2);
      delete waitTimeMetric;
    }

    Metric* metricPol = NULL;
 
    /* the two initial metrics types are derived metrics */
    switch (this->ISISDispatcherMetric2Optimize)
    {
      case SLD_PREDICTED:
      {
        //set up the derived metric
        metricPol = new Metric(SLD_PREDICTED,CONSTANT,"SLD_PREDICTED",DOUBLE);
        metricPol->setNativeDouble(estimatedSlowdown);
        break;
     }

     case ESTIMATED_COMPUTATIONAL_COST:
     {
        metricPol = new Metric(ESTIMATED_COMPUTATIONAL_COST,CONSTANT,"ESTIMATED_COMPUTATIONAL_COST",DOUBLE);
        double estimatedRT = runtimePrediction->getprediction(); 
        
        double EstimatedCost = /*job->getNumberProcessors()**/(estimatedRT/3600)*pol->getCostAllocationUnit()*pol->getAllocationUnitHour();

        if(job->getMaximumAllowedCost() > EstimatedCost || bestMetric == NULL) 
        {                      
           metricPol->setNativeDouble(EstimatedCost);
           Metric* cost = new Metric(SLD_PREDICTED,CONSTANT,"SLD_PREDICTED",DOUBLE);
           cost->setNativeDouble(estimatedSlowdown);           
           metricPol->addComposedMetric(cost);
        }
        else
        {
          continue;
          count++;
        }
        break;
     }
 
     default: /* by default a simple metric is computed */
        metricPol = pol->EstimatePerformanceResponse(this->ISISDispatcherMetric2Optimize,job);
 
        if(ISISDispatcherMetric2Optimize == WAITTIME)
        {
          double estimatedWT = metricPol->getnativeDouble();
          predictionsWT.insert(pair<int,double>(count,estimatedWT));
          job->setWaitTimePrediction(estimatedWT); 
        }  
    
    } 

    if(bestMetric == NULL || metricPol->betterOrEqualThan(bestMetric))
    {
      //in case of tie break following a uniform of 0.5 we will change the policy, we do it due to we want to distribute the load among al the centers 
      float selection = (random() % 100)/100;
 
      if(bestMetric == NULL || metricPol->betterThan(bestMetric) || selection > 0.5)
      {
        bestMetric = metricPol;
        bestPolicy = pol;
      
        choise = count;
      }
    }
    else
    {
       delete metricPol;
    }


    count++;
  }
  
  if(bestPolicy == NULL)
  {
    //add a new status
    return;
  }

  //now the sumbission should be decided so.. just submit it !
  string centerName = this->ISISDispatcherCentersName->operator[] (choise);
  
  log->debug("The job "+itos(job->getJobNumber())+" has been submitted to the center "+centerName,2);
  
  
  //set the stuff for the job 
  job->setJobSimCenter(centerName);
  
  if(predictions.size()>0)
  {
    map<int,double>::iterator it = predictions.find(choise);
    assert(it != predictions.end());
  
    double prediction = it->second;
  
    job->setRequestedTime(prediction);
    job->setRuntimePrediction(prediction);
   
    assert(prediction > 0);

    //we set the estimated cost 
    job->setPredictedMonetaryCost(/*job->getNumberProcessors()**/(prediction/3600)*bestPolicy->getCostAllocationUnit()*bestPolicy->getAllocationUnitHour());

  }

  //in case the wait time has been predicted we set it to the job
  if(predictionsWT.size() > 0)
  {
    map<int,double>::iterator it = predictionsWT.find(choise);
    assert(it != predictionsWT.end());
    double wtpred = it->second;
   
    job->setWaitTimePrediction(wtpred);
    
  }


  bestPolicy->jobAdd2waitq(job);
  
  this->submissions.insert(PairSubmission(job,bestPolicy));
  
  //now that have been scheduled we insert it in the waitqueue 
  this->waitQueue->insert(job);

  //free the best metric 
  delete bestMetric;
  
}

/**
 * Moves a job from teh wait queue to the reservation table, using an specified set of resources
 * @see The class SchedulingPolicy  
 * @param job The job that has started
 */
void ISISDispatcherPolicy::jobWaitq2RT(Job* job)
{
  //we simply proxy the query of removing the job from the wait queue and add it to the Run table 
  map<Job*,SchedulingPolicy*>::iterator subit = this->submissions.find(job);
  
  assert(subit != this->submissions.end()); //the job should be already submitted !
  
  SchedulingPolicy* policy = subit->second;
  
  log->debug( " The job "+itos(job->getJobNumber())+" has to be moved to the run table.",2);
  
  policy->jobWaitq2RT(job); 
}

/**
 * Moves a job from teh wait queue to the reservation table, using an specified set of resources
 * @see The class SchedulingPolicy  
 * @param job The job that has started
 */
void ISISDispatcherPolicy::jobRemoveFromRT(Job* job)
{
  //We call to the base class - important it has to update some info   
  SchedulingPolicy::jobRemoveFromRT(job);

  //we simply proxy the query of removing the job from the wait queue and add it to the Run table 
  map<Job*,SchedulingPolicy*>::iterator subit = this->submissions.find(job);
  
  assert(subit != this->submissions.end()); //the job should be already submitted !
  
  SchedulingPolicy* policy = subit->second;
  
  log->debug( " The job "+itos(job->getJobNumber())+" has to be moved to the run table.",2);
  
  policy->jobRemoveFromRT(job); 
  
  //we remove the job from the table in case that the job is not still running 
  //remeber that some of the policies, i.e: those that are running with prediction 
  //   
  if(job->getJobSimStatus() != RUNNING)
  {
    this->waitQueue->erase(job);
    job->setMonetaryCost((job->getRunTime()/3600)*/*job->getNumberProcessors()**/policy->getCostAllocationUnit()*policy->getAllocationUnitHour());
  }

  //time to update the predictor service in case if it is being used in the simulation model
  //We will estimate the job runtime if it was set or not 
  if(this->UsePredictionService)
  {      
   
    this->predictionService->jobTerminated(job, this->globaltime,job->getJobSimStatus() == COMPLETED);
    this->predictionService->estimateJobPerformance(job);

  }
}


/**
 * Chooses the more appropiate job to run .
 * @see The class SchedulingPolicy  
 * @return The appropiate job to run 
 */
Job* ISISDispatcherPolicy::jobChoose()
{
  //for this version this remains empty, however it will be used for implemeent more sophisticated scheduling decisions 
}

/**
 * Function that is called once the job should start 
 * @see The class SchedulingPolicy  
 * @param job The job that to start 
 */
void ISISDispatcherPolicy::jobStart(Job* job)
{
   //we simply proxy the query of removing the job from the wait queue and add it to the Run table 
  map<Job*,SchedulingPolicy*>::iterator subit = this->submissions.find(job);
  
  assert(subit != this->submissions.end()); //the job should be already submitted !
  
  SchedulingPolicy* policy = subit->second;
  
  log->debug( " The job "+itos(job->getJobNumber())+" has to start.",2);
  
  policy->jobStart(job); 
  
  //we update the hashmap of the submitted jobs 
  map<SchedulingPolicy*,int>::iterator subjobsit = this->SubmittedJobs.find(policy);
  
  assert(subjobsit != this->SubmittedJobs.end());
  
  this->SubmittedJobs[policy] = this->SubmittedJobs[policy]+1;

}

/**
 * Returns the number of jobs currently allocated in the waitQueue
 * @return An integer containing the number of jobs in the WQ
 */
double ISISDispatcherPolicy::getJobsIntheWQ()
{
  return this->waitQueue->getjobs();
}

/**
 * Returns the number of jobs that are currently backfilled 
 * @return An integer containing the number of backfilled jobs 
 */
double ISISDispatcherPolicy::getBackfilledJobs()
{
  double backfilledJobs = 0;

  for(vector<SchedulingPolicy*>::iterator polit = this->ISISDispatcherPolicies->begin();
      polit != this->ISISDispatcherPolicies->begin();
      ++polit)
  {
    SchedulingPolicy* pol = *polit;    
    backfilledJobs+= pol->getBackfilledJobs();
  }
  
  return backfilledJobs;
}

/**
 * Returns the number of processors used 
 * @return An integer containing the number of processors used 
 */
int  ISISDispatcherPolicy::getNumberCPUSUsed()
{
   int used = 0;
   
   for(vector<SchedulingPolicy*>::iterator polit = this->ISISDispatcherPolicies->begin();
      polit != this->ISISDispatcherPolicies->begin();
      ++polit)
  {
    SchedulingPolicy* pol = *polit;    
    used+= pol->getreservationTable()->getNumberCPUSUsed();
  }  
  
  return used;
}
  

/**
 * Sets a reference to the simulation class, needed by some policies 
 * @param theValue A reference to the simulation class 
 */
void ISISDispatcherPolicy::setSim(Simulation* theValue)
{  
  //we have to iterate over all the policies and set the simulation reference
  for(vector<SchedulingPolicy*>::iterator polit = this->ISISDispatcherPolicies->begin();
      polit != this->ISISDispatcherPolicies->end();
      ++polit)
  {
    SchedulingPolicy* pol = *polit;    
    pol->setSim(theValue);
  }
} 

/**
 * Sets the global time of the simulation 
 * @param globaltime The double with the global time 
 */
void ISISDispatcherPolicy::setGlobalTime(double globaltime)
{
  this->log->debug("Setting the global time of the schdeuling policy and reservationTable to "+ftos(globaltime),6);
  this->globaltime = globaltime;
  
  /* we set also the global time for the reservationTable */
  this->reservationTable->setGlobaltime(globaltime);
  
  //we have to iterate over all the policies and set the simulation reference
  for(vector<SchedulingPolicy*>::iterator polit = this->ISISDispatcherPolicies->begin();
      polit != this->ISISDispatcherPolicies->end();
      ++polit)
  {
    SchedulingPolicy* pol = *polit;    
    pol->setGlobalTime(globaltime);
  }  
}

/***************************************************************************************************************************
******************************gets and sets ********************************************************************************
****************************************************************************************************************************/

/**
 * Returns the of the ISISDispatcherArchitectures ISISDispatcher policie
 * @return The ISISDispatcherArchitectures used by the policies
 */
vector< ArchitectureConfiguration * >* ISISDispatcherPolicy::getISISDispatcherArchitectures() const
{
  return ISISDispatcherArchitectures;
}  


/**
 * Function that sets ISISDispatcherArchitectures the of the ISISDispatcher policie
 * @param theValue The ISISDispatcherArchitectures
 */
void ISISDispatcherPolicy::setISISDispatcherArchitectures(vector< ArchitectureConfiguration * >* theValue)
{
  ISISDispatcherArchitectures = theValue;
}

/**
 * Returns the ISISDispatcherMetric2Optimizeof the ISISDispatcher policie
 * @return The ISISDispatcherMetric2Optimize
 */
metric_t ISISDispatcherPolicy::getISISDispatcherMetric2Optimize() const
{
  return ISISDispatcherMetric2Optimize;
}

/**
 * Function that sets the ISISDispatcherMetric2Optimize of the ISISDispatcher policie
 * @param theValue The ISISDispatcherMetric2Optimize
 */
void ISISDispatcherPolicy::setISISDispatcherMetric2Optimize(const metric_t& theValue)
{
  ISISDispatcherMetric2Optimize = theValue;
}


/**
 * Returns the ISISDispatcherParaverTracesName of the ISISDispatcher policie
 * @return The  ISISDispatcherParaverTracesName
 */
vector< string >* ISISDispatcherPolicy::getISISDispatcherParaverTracesName() const
{
  return ISISDispatcherParaverTracesName;
}

/**
 * Function that sets the ISISDispatcherParaverTracesName of the ISISDispatcher policie
 * @param theValue The ISISDispatcherParaverTracesName
 */
void ISISDispatcherPolicy::setISISDispatcherParaverTracesName(vector< string >* theValue)
{
  ISISDispatcherParaverTracesName = theValue;
}

/**
 * Returns the ISISDispatcherPolicies of the ISISDispatcher policie
 * @return The ISISDispatcherPolicies
 */

vector< SchedulingPolicy * >* ISISDispatcherPolicy::getISISDispatcherPolicies() const
{
  return ISISDispatcherPolicies;
}

/**
 * Function that sets the ISISDispatcherPolicies of the ISISDispatcher policie
 * @param theValue The ISISDispatcherPolicies
 */
void ISISDispatcherPolicy::setISISDispatcherPolicies(vector< SchedulingPolicy * >* theValue)
{
  ISISDispatcherPolicies = theValue;
  
  //we initialize the number of submitted jobs to zero
  //we also initialize the reservationTable 
  int pos = 0;
  for(vector<SchedulingPolicy*>::iterator polit = this->ISISDispatcherPolicies->begin();
      polit != this->ISISDispatcherPolicies->end();
      ++polit)
  {
    SchedulingPolicy* pol = *polit;    
    this->SubmittedJobs.insert(PairSubmitedJobs(pol,0));    
    
    string center = this->ISISDispatcherCentersName->operator[](pos);    
    ((GridAnalogicalReservationTable*)this->reservationTable)->addReservationTable(center,pol->getreservationTable());
    pos++;
  }  
  
}

/**
 * Return the the amount of pending work (area of requested time x requested processors for the queued jobs)
 * @return A integer containing the pending work
 */
double ISISDispatcherPolicy::getLeftWork()
{
  return 0;
}
  
/**
* Return the number of jobs in the run queue 
* @return A integer containing the number of running jobs 
*/
double ISISDispatcherPolicy::getJobsIntheRQ()
{
  return 0;
}

/**
 * Returns the ISISDispatcherStatistic2Optimize of the ISISDispatcher policie
 * @return The ISISDispatcherStatistic2Optimize
 */

statistic_t ISISDispatcherPolicy::getISISDispatcherStatistic2Optimize() const
{
  return ISISDispatcherStatistic2Optimize;
}

/**
 * Function that sets ISISDispatcherStatistic2Optimize the of the ISISDispatcher policie
 * @param theValue The ISISDispatcherStatistic2Optimize
 */
void ISISDispatcherPolicy::setISISDispatcherStatistic2Optimize(const statistic_t& theValue)
{
  ISISDispatcherStatistic2Optimize = theValue;
}

/**
 * Returns the metric that contains the estimation of which would be the performance of a given job if it would be allocated now. Now this is not implemented , but would be in the future
 * @param MetricTpye The metric that indicates which performance variable has to be computed 
 * @param job The job that will be checked
 * @return The metric containing the performance that the given job would experiment if it would be submitted right now
 */
 
Metric* ISISDispatcherPolicy::EstimatePerformanceResponse(metric_t MetricTpye,Job* job)
{
  return NULL;
}

/**
 * Returns the ISISDispatcherCentersName of the ISISDispatcher policie
 * @return The ISISDispatcherCentersName
 */
vector< string >* ISISDispatcherPolicy::getISISDispatcherCentersName() const
{
  return ISISDispatcherCentersName;
}

/**
 * Function that sets the ISISDispatcherCentersName of the ISISDispatcher policie
 * @param theValue The ISISDispatcherCentersName
 */
void ISISDispatcherPolicy::setISISDispatcherCentersName(vector< string >* theValue)
{
  ISISDispatcherCentersName = theValue;
}

/**
 * Returns the ISISDispatcherEmulateCPUFactor of the ISISDispatcher policie
 * @return The ISISDispatcherEmulateCPUFactor
 */
vector< double >* ISISDispatcherPolicy::getISISDispatcherEmulateCPUFactor() const
{
  return ISISDispatcherEmulateCPUFactor;
}

/**
 * Function that sets the of the ISISDispatcher policie
 * @param theValue The ISISDispatcherEmulateCPUFactor
 */
void ISISDispatcherPolicy::setISISDispatcherEmulateCPUFactor(vector< double >* theValue)
{
  ISISDispatcherEmulateCPUFactor = theValue;
}

/**
 * Returns the amountOfKnownCenters of the ISISDispatcher policie
 * @return The amountOfKnownCenters
 */
int ISISDispatcherPolicy::getamountOfKnownCenters() 
{
  return amountOfKnownCenters;
}

/**
 * Function that sets the amountOfKnownCenters of the ISISDispatcher policie
 * @param theValue The amountOfKnownCenters
 */
void ISISDispatcherPolicy::setAmountOfKnownCenters ( int theValue )
{
  amountOfKnownCenters = theValue;
  //this variable will help to select randomly uniformly the centers  
  this->ADSRandom = new ProcessUniform(0, this->ISISDispatcherPolicies->size(), 3);
  printf("Using the ADS with the amount of known members of %d\n",amountOfKnownCenters);
}


}
