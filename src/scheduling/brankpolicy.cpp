#include <scheduling/brankpolicy.h>
#include <scheduling/gridjob.h>
#include <scheduling/gridanalogicalreservationtable.h>
#include <utils/utilities.h>

#include <string>
#include <map>

//using std::map;
using namespace std;

//#include<iostream>
//#include<string>
//#include<sstream>



namespace Simulator {

/**
 * The default constructor for the class
 */
BRANKPolicy::BRANKPolicy()
{
  UsePredictionService = false;
  predictionService = NULL;
  this->schedulingPolicySimulated = BRANK_POLICY;
}

/**
 * This is the constructor that should be used, invoques that constructor for the parent classes  
 * @param log A reference to the logging engine
 * @param UsePredictionService A bool indicating if the prediction service has to be used or not 
 * @param predictionService A reference to the prediction service that will be used by the policy for estimate the job runtime 
 */
BRANKPolicy::BRANKPolicy(Log* log,bool UsePredictionService,Predictor* predictionService)
{
  //by default we will create a FCFSC 
  this->waitQueue = new FCFSJobQueue();
  
  //as all the scheduling policies must have associated a Reservation table by definition 
  this->log = log;

  this->reservationTable = new GridAnalogicalReservationTable();
  
  this->UsePredictionService = UsePredictionService;
  this->predictionService = predictionService;
  this->schedulingPolicySimulated = BRANK_POLICY;

  BRANKPolicyPolicies  = 0;
  BRANKPolicyArchitectures = 0;
  BRANKPolicyParaverTracesName = 0;
  BRANKPolicyCentersName = 0;
  BRANKPolicyEmulateCPUFactor = 0;
  metaSystem = 0;
}


/**
 * The default destructor for the class 
 */
BRANKPolicy::~BRANKPolicy()
{
  //before destroy all the stuff, we will iterate over all the policies, and we will generate the traces 
  for(vector<SchedulingPolicy*>::iterator polit = this->BRANKPolicyPolicies->begin();
      polit != this->BRANKPolicyPolicies->end();
      ++polit)
  {
    SchedulingPolicy* pol = *polit;    
    delete pol;
  }
  
  delete this->BRANKPolicyPolicies;
  
  for(vector<ArchitectureConfiguration*>::iterator archit = this->BRANKPolicyArchitectures->begin();
     archit != this->BRANKPolicyArchitectures->end();
     ++archit)
  {
    ArchitectureConfiguration* arch = *archit;
    delete arch;
  }
   
  delete this->reservationTable;;  
  delete this->BRANKPolicyCentersName;
  delete this->BRANKPolicyParaverTracesName;
  delete this->BRANKPolicyEmulateCPUFactor;
  
}

/**
 * Invoqued when the global scheduling has finished it mainly creates the traces for each center 
 */
void BRANKPolicy::PostProcessScheduling()
{
  //before destroy all the stuff, we will iterate over all the policies, and we will generate the traces 
  for(vector<SchedulingPolicy*>::iterator polit = this->BRANKPolicyPolicies->begin();
      polit != this->BRANKPolicyPolicies->end();
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
 * Auxiliar function that returns the RANK value given two values, an operator, and a factor.
 * @param resource_data Is the value to compare of a resource
 * @param job_data Is the target value to compare of a job
 * @param op Is the operator for comparison
 * @param factor Is the factor to apply when obtaining the RANK value
 * @return An integer containing the value of the RANK or -1 if the resource does not fit the job requirements
 */

double BRANKPolicy::getRANK(double resource_data, double job_data, operator_t op, int factor)
{

  // We define which weight has the RANK when the resource fits the requirement (the minimum positive RANK value)
  int REQUIREMENT_WEIGHT=1;

  /**
    If job_data == -1 means that this is not a requirement!
  */

  if(job_data == -1)
  {
	// for the moment we only 0 but we should return the relativeRANK with the total resource_data!
	return 0;
  }
  else{

    /**
      We check the given expression
      For ">" and ">=" operators we calculate the "relative" RANK (factor x difference between the given job requirement data and total resource)
    */

    switch(op)
    {
      case LESS_THAN:
			if(resource_data<job_data){
				return REQUIREMENT_WEIGHT;
			}
			else{
				return -1;
			}
      break;
      case LESS_EQ_THAN:
			if(resource_data<=job_data){
				return REQUIREMENT_WEIGHT;
			}
			else{
				return -1;
			}
      break;
      case EQUAL:
			if(resource_data==job_data){
				return REQUIREMENT_WEIGHT;
			}
			else{
				return -1;
			}
      break;
      case HIGHER_THAN:
			if(resource_data>job_data){
				double relativeRANK = REQUIREMENT_WEIGHT;
				
				relativeRANK += (resource_data - job_data) * factor;

				return relativeRANK;
			}
			else{
				return -1;
			}
      break;
      case HIGHER_EQ_THAN:
			if(resource_data>=job_data){
				double relativeRANK = REQUIREMENT_WEIGHT;
				
				relativeRANK += (resource_data - job_data) * factor;

				return relativeRANK;
			}
			else{
				return -1;
			}
      break;
    }

  }

  // should not be reached
  return -1;

}



/**
 * Adds a given job to the wait queue, usually when it arrives to the system.
 * this function does not call to the parent class scheduling policy due to it will be called in each policy instantiated    
 * The current approach will iterater over each center and will ask for a performance estimation 
 * that the given job would achieve in the given center with the current load. 
 * @see The class SchedulingPolicy 
 * @param job the job to be added to the wait queue
 */
void BRANKPolicy::jobAdd2waitq(Job* job)
{  


   //jobAdd2waitq_REGULAR_LOCAL(job);

   //jobAdd2waitq_REGULAR(job); NO USE ANYMORE...


   //jobAdd2waitq_AGGREGATED(job);  

   jobAdd2waitq_RANDOM(job);

   //jobAdd2waitq_RR(job); 
}





/**
 * TO COMMENT...
 */

void BRANKPolicy::jobAdd2waitq_REGULAR_LOCAL(Job* job){
 

  int queue_policy = 2;

  /* To determine policy :
	1: FCFS
	2: BACKFILLING STARVATION
	3: BACKFILLING WITH RANG
  */


  Metric* bestMetric = NULL;
  //SchedulingPolicy* bestPolicy = NULL;
  
  SchedulingPolicy* bestResource = NULL;
  SchedulingPolicy* goodResource = NULL;
  double bestRANK = -1;
  double tmpRANK = -1;  

  int choise = 0; 
  int good_choise = 0;

  int count = 0; //controls the resource number in the loop
  GridJob* gjob = (GridJob*) job;


  // If there is any waiting job I can not submit another one (because of FCFS)
  if(this->waitQueue->getjobs() > 0){

    // If the waiting queue contains the job means that is rescheduled...
    if(!this->waitQueue->contains(job)){
	switch(queue_policy){
		case 1:
	      		this->waitQueue->insert(job);
			cout<<"WAIT_QUEUE SIZE:"<<this->waitQueue->getjobs()<<endl;
			delete bestMetric;
 
			return;
		case 2: // nothing to do
			break;
		case 3:
			if(this->waitQueue->getjobs() > 0){
				if(getJobRank(job) >= getJobRank(this->waitQueue->begin())){

	      				this->waitQueue->insert(job);
					cout<<"HIGHER JOB RANK:"<<endl;
					delete bestMetric;
 
					return;
				}
			}
			break;
		default:
			break;
	} 
    }
    else{
      this->waitQueue->erase(job);
    }

  }

  /**
      now its time to process the job grid specific fields. 
  */

  Metric* cspeedMetric = gjob->getClockSpeedRequirement()->getmetric();    
  double cspeedRequired = cspeedMetric->getnativeDouble();
  operator_t cspeedOperator = gjob->getClockSpeedRequirement()->getoperatorRequired();

  Metric* vendorMetric = gjob->getVendorRequirement()->getmetric();    
  string vendorRequired = vendorMetric->getnativeString();;
  operator_t vendorOperator = gjob->getVendorRequirement()->getoperatorRequired();

  Metric* osNameMetric = gjob->getOSNameRequirement()->getmetric();    
  string osNameRequired = osNameMetric->getnativeString();;
  operator_t osNameOperator = gjob->getOSNameRequirement()->getoperatorRequired();

  Metric* memoryMetric = gjob->getMemoryRequirement()->getmetric();    
  double memoryRequired = memoryMetric->getnativeDouble();
  operator_t memoryOperator = gjob->getMemoryRequirement()->getoperatorRequired();

  Metric* totalCpusMetric = gjob->getNumberProcessorsRequirement()->getmetric();    
  double totalCpusRequired = totalCpusMetric->getnativeDouble();
  operator_t totalCpusOperator = gjob->getNumberProcessorsRequirement()->getoperatorRequired();

  Metric* diskSizeMetric = gjob->getDiskRequirement()->getmetric();    
  double diskSizeRequired = diskSizeMetric->getnativeDouble();
  operator_t diskSizeOperator = gjob->getDiskRequirement()->getoperatorRequired();


  //cout<<"Processing Job number "<<job->getJobNumber()<<endl;


  /**
      We have in the following variables:

	double cspeedRequired -> CPU Speed in MhZ (e.g. 3200)
	string vendorRequired -> CPU architecture (e.g. "Intel")
	string osNameRequired -> OS Name (e.g. "Linux")
	double memoryRequired -> RAM memory in MB (e.g. 2048)
	double totalCpusRequired -> Number of CPU (e.g. 4)
	double diskSizeRequired -> Disk size in MB (e.g. 4000)
  	
     And we have a operator_t for each one (=, <, >, <=, >=)
  */

  /**
	RANK Algorithm (given a job requirements + "recommendations " + FACTORS, choose the appropiate resource)

	1. For resource{
		2. For each of the previous variables{
			3. Obtain a RANK using variables and FACTORS
				3.1. If the resource does not fit the requirements -> RANK = -1 (we can go ahead with the following resource)
				3.2. If the operator_t is different than "=", obtains RANK value (e.g. for FreeMemory, if it asks for 100MB and there are 200MB available MB, multiply the factor per this additional 100MB).
		}
		4. Calculates the resource with maximum RANK value
	}
	5. Selects the resource with maximum RANK value if it is different than -1
  */


///////////////// FACTOR FOR OWN BROKER POTENTIATION

double own_broker_factor = 1.05;


int broker_count = 0;
int current_broker = -1;
int selected_broker = -1;
double bestBrokerRANK = 0;

vector<SchedulingPolicy*>* Brokers = this->metaSystem->getVirtualOrganitzations();
BRANKPolicy* toForward = NULL;

for(vector<SchedulingPolicy*>::iterator it = Brokers->begin(); it != Brokers->end();++it)
{
  BRANKPolicy* broker = (BRANKPolicy*) *it;



  // To calculate the AVG Slowdown of the broker
  double total_slowdown = 0;

  bestRANK = -1;
  tmpRANK = -1;  
  choise = 0; 
  count = 0;


  /* We iterate to each cluster */
  for(vector<SchedulingPolicy*>::iterator polit = broker->BRANKPolicyPolicies->begin();
      polit != broker->BRANKPolicyPolicies->end();
      ++polit)
  {
    SchedulingPolicy* currentResource = *polit;

    string center = broker->BRANKPolicyCentersName->operator[] (count);      
    //cout<<"Analysing resource: "+center <<endl;

    /**
       for the center we get the dynamic information 
    */
    Metric* memoryAVMetric = currentResource->EstimatePerformanceResponse(MEMORY_AVAILABLE,job);
    double memory = memoryAVMetric->getnativeDouble();

    Metric* diskAvailable = currentResource->EstimatePerformanceResponse(DISK_AVAILABLE,job);
    double disk = diskAvailable->getnativeDouble();

    Metric* cpusAvailable = currentResource->EstimatePerformanceResponse(FREE_CPUS,job);
    double cpus = cpusAvailable->getnativeDouble();

    Metric* prevSldMetric = currentResource->EstimatePerformanceResponse(AVG_SLD_FINISHED_JOBS,job);
    double prevSld = prevSldMetric->getnativeDouble();


//cout<<"BROKER:"<<broker->CenterName<<", center:"<<center<<", prevSld="<<prevSld<<", total_slowdown="<<total_slowdown<<endl;

    total_slowdown += prevSld;

    double running = currentResource->getJobsIntheRQ();;

    //Metric* prevRTMetric = currentResource->EstimatePerformanceResponse(AVG_RT_FINISHED_JOBS,job);
    //double prevRT = prevRTMetric->getnativeDouble();

    //Metric* prevRespTMetric = currentResource->EstimatePerformanceResponse(AVG_RESPT_FINISHED_JOBS,job);
    //double prevRespT = prevRespTMetric->getnativeDouble();

    //Metric* prevWTMetric = currentResource->EstimatePerformanceResponse(AVG_WT_FINISHED_JOBS,job);
    //double prevWT = prevWTMetric->getnativeDouble();


  
    /**
       for the center we get the static information 
    */
    Metric* vendorMetric = currentResource->getStaticSystemInformation(VENDOR);
    string vendor = vendorMetric->getnativeString();

    Metric* osNameMetric = currentResource->getStaticSystemInformation(OS_NAME);
    string osName = osNameMetric->getnativeString();

    Metric* totalCpusMetric = currentResource->getStaticSystemInformation(TOTAL_CPUS);
    double totalCpus = totalCpusMetric->getnativeDouble();

    Metric* clockSpeedMetric = currentResource->getStaticSystemInformation(CLOCK_SPEED);
    double clockSpeed = clockSpeedMetric->getnativeDouble();

    Metric* diskSizeMetric = currentResource->getStaticSystemInformation(DISK_SIZE);
    double diskSize = diskSizeMetric->getnativeDouble();

    Metric* memorySizeMetric = currentResource->getStaticSystemInformation(MEMORY_SIZE);
    double memorySize = memorySizeMetric->getnativeDouble();



    /**
	We define the FACTORS for calculating the RANK values
    */

    //100 is the reference value (25 -> 0.25)


/* DISK 


    int CSPEED_FACTOR=200;
    int TOTAL_CSPEED_FACTOR=50;
    int FREECPUS_FACTOR=100;
    int TOTALCPUS_FACTOR=100;

    int MEMORY_FACTOR=4;

    int DISKSIZE_FACTOR=64; //1

    int RUNNING_JOBS_FACTOR = 40000; //1k
    int AVG_SLD_FINISHED_JOBS_FACTOR=10000; //10k
*/




/* MEM

    int CSPEED_FACTOR=200;
    int TOTAL_CSPEED_FACTOR=50;
    int FREECPUS_FACTOR=100;
    int TOTALCPUS_FACTOR=100;

    int MEMORY_FACTOR=64;//4

    int DISKSIZE_FACTOR=1;

    int RUNNING_JOBS_FACTOR = 40000; //1k
    int AVG_SLD_FINISHED_JOBS_FACTOR=10000; //10k
*/



/* CPU

    int CSPEED_FACTOR=800;
    int TOTAL_CSPEED_FACTOR=200;
    int FREECPUS_FACTOR=200;
    int TOTALCPUS_FACTOR=200;

    int MEMORY_FACTOR=4;

    int DISKSIZE_FACTOR=1;

    int RUNNING_JOBS_FACTOR = 40000; //1k
    int AVG_SLD_FINISHED_JOBS_FACTOR=10000; //10k
*/

/* BALANCED: 
*/
    int CSPEED_FACTOR=200;//100
    int TOTAL_CSPEED_FACTOR=50;//25
    int FREECPUS_FACTOR=100;
    int TOTALCPUS_FACTOR=100;

    int MEMORY_FACTOR=4;

    int DISKSIZE_FACTOR=1;

    int RUNNING_JOBS_FACTOR = 40000; //1k
    int AVG_SLD_FINISHED_JOBS_FACTOR=100000; //10k


    /**
      Process job REQUIREMENTS (job extension)

      For VENDOR and OS_NAME we do not need operator (just "=")
      They are only used for requirements
    */

    // currentRANK is the RANK for the current resource depending on the job requirements
    double currentRANK = 0;

    if(vendorRequired.compare("ANY")){
      // if it is empty it is not a requirement
      if(vendor.compare(vendorRequired)){
	// different strings vendors
	currentRANK = -1;
      }
      // if not, there is not anything to do
    } 
   

    if(currentRANK!=-1){
      if(osNameRequired.compare("ANY")){
        if(osName.compare(osNameRequired)){
	  // different strings vendors
	  currentRANK = -1;	
        }
      }	
    }

    /**
      The rest of GridJob requirements are double type
    */

    // CPU speed (difference between the total and the required)
    if(currentRANK!=-1 && cspeedRequired!=-1){
      tmpRANK = getRANK(clockSpeed, cspeedRequired, cspeedOperator, CSPEED_FACTOR);
      if(tmpRANK != -1){
        currentRANK += tmpRANK;
      }
      else{
        currentRANK = -1;
      }
    }


/*
    // # Free CPUs 
    if(currentRANK!=-1 && totalCpusRequired!=-1){
      tmpRANK = getRANK(cpus, totalCpusRequired, totalCpusOperator, FREECPUS_FACTOR);
      if(tmpRANK != -1){
        currentRANK += tmpRANK;
      }
      else{
        currentRANK = -1;
        cout<<"Job "<<  job->getJobNumber() << ": Not enough CPUS in resource:" << center << endl;
      }
    }

*/

    // % Free memory
    if(currentRANK!=-1 && memoryRequired!=-1){
      tmpRANK = getRANK(memory, memoryRequired, memoryOperator, 1);
      if(tmpRANK != -1){
        currentRANK += MEMORY_FACTOR*tmpRANK/memorySize;
      }
      else{
        currentRANK = -1;
        cout<<"Job "<<  job->getJobNumber() << ": Not enough MEMORY in resource:" << center << endl;
      }
    }

    // % Free Disk
    if(currentRANK!=-1 && diskSizeRequired!=-1){
      tmpRANK = getRANK(disk, diskSizeRequired, diskSizeOperator, 1);
      if(tmpRANK != -1){
        currentRANK += DISKSIZE_FACTOR*tmpRANK/diskSize;
      }
      else{
        currentRANK = -1;
        cout<<"Job "<<  job->getJobNumber() << ": Not enough DISK in resource:" << center << endl;
      }
    }


//POTSER HAURIA DE FILTRAR ELS QUE NO TENEN PROU MEMORIA O DISK...




    /**
      The regular job (no extension) has to be considered for obtaining the RANK value
    */



     // enough #CPUS for being executed in this resource...

    double jobCpus = gjob->getNumberProcessors();

    if(currentRANK!=-1){
      tmpRANK = getRANK(cpus, jobCpus, HIGHER_EQ_THAN, FREECPUS_FACTOR);
      if(tmpRANK != -1){
        // currentRANK += tmpRANK;
	// only to fit requirement...
      }
      else{
        currentRANK = -1;
        cout<<"Job "<<  job->getJobNumber() << ": Not enough CPUS to allocate job in resource:" << center << endl;
      }
    }





    // #FREE CPUS

    // We determine if there are enough free CPUs if it is not a requirement
    // cpus is #available CPUs 

/* NOT reachable because we always specify the required number of procs
    double jobCpus = gjob->getNumberProcessors();

    if(currentRANK!=-1 && totalCpusRequired==-1){
      tmpRANK = getRANK(cpus, jobCpus, HIGHER_EQ_THAN, FREECPUS_FACTOR);
      if(tmpRANK != -1){
        currentRANK += tmpRANK;
      }
      else{
        currentRANK = -1;
      }
    }
*/

    // %FREE CPUS

    // totalCpus is the number of the resource CPUs
    // cpus is #available CPUs 

/*
    double percent = cpus / totalCpus;

    if(currentRANK!=-1){
      tmpRANK = getRANK(percent, 0, HIGHER_EQ_THAN, TOTALCPUS_FACTOR*512);
      if(tmpRANK != -1){
        currentRANK += tmpRANK;
      }
      else{
        //currentRANK = -1;
      }
    }

*/

    /**
      The static values can be also taken into account for obtaining the RANK value, but not for the moment... 
    */


    // #CPUS

    //totalCpus is the number of the resource CPUs

/*
    int myfactor = (int) totalCpus / 10 +1;

    if(currentRANK!=-1){
      tmpRANK = getRANK(myfactor, 0, HIGHER_EQ_THAN, TOTALCPUS_FACTOR);
      if(tmpRANK != -1){
        currentRANK += tmpRANK;
      }
      else{
        currentRANK = -1;
      }
    }
*/

    // CPU Speed (MHz)

    //clockSpeed is the CPUs speed
    //myfactor = (int) clockSpeed / 4;

    if(currentRANK!=-1){
      tmpRANK = getRANK(clockSpeed, 0, HIGHER_EQ_THAN, TOTAL_CSPEED_FACTOR);
      if(tmpRANK != -1){
        currentRANK += tmpRANK;
      }
      else{
        currentRANK = -1;
      }
    }


    // Total Memory size
    // Total Disk size





    /**
      Go ahead with performance metrics...
    */


    // #RUNNING JOBS (inverse)

    if(currentRANK!=-1){
      tmpRANK = getRANK(running, 0, HIGHER_EQ_THAN, 1);
      if(tmpRANK != -1){
        currentRANK += RUNNING_JOBS_FACTOR/tmpRANK;
      }
      else{
        currentRANK = -1;
      }
    }

    // AVG Slowdown of local schedulers

    if(currentRANK!=-1){
cout<<endl<<"CLUSTER_AVG_SLD: "<<center<<"="<<prevSld<<endl;

      tmpRANK = getRANK(prevSld, 0, HIGHER_EQ_THAN, 1);
      if(tmpRANK != -1){
        currentRANK += AVG_SLD_FINISHED_JOBS_FACTOR/tmpRANK;
      }
      else{
        //currentRANK = -1;
      }
    }


    /** 
      Perform the selection based on RANK value 
    */

    //cout<<"Total current RANK value for resource number "<<count<<" is: "<<currentRANK<<endl;


    if(currentRANK > bestRANK){
      bestResource = currentResource;
      bestRANK = currentRANK;
      choise = count;
    }

    count++;

    // FREE MEMORY


    delete memoryAVMetric;
    delete diskAvailable;
    delete cpusAvailable;
    delete prevSldMetric;
    delete vendorMetric;
    delete osNameMetric;
    delete totalCpusMetric;
    delete clockSpeedMetric;
    delete diskSizeMetric;
    delete memorySizeMetric;

  }  //end foreach resource
  

  // Computes Average Slowdown of the resources (for my resources)
  double AVG_slowdown = 1;
  if(count){
    AVG_slowdown = (double) total_slowdown/(double)count;
  }

  cout<<"BROKER:"<<broker->CenterName<<", AVG_slowdown="<<AVG_slowdown<<endl;


 if(broker == this){
  bestRANK *= own_broker_factor; 
  current_broker = broker_count;
  good_choise = choise;
  goodResource = bestResource;
 }

 if(bestRANK>bestBrokerRANK){
   selected_broker = broker_count;
   bestBrokerRANK = bestRANK;
   toForward = broker;
   //good_choise = choise;
   //goodResource = bestResource;
   //goodResource is where to submit in case that it select the current broker!
 }

 broker_count++;

} //brokers




if(toForward != NULL && toForward != this){

  cout << "FORWARDING: JobID:"<< job->getJobNumber() << " OriginalVO:" << gjob->getVOSubmitted() << " FromVO:" << this->metaSystem->getVONames()[current_broker] << " ToVO:" << this->metaSystem->getVONames()[selected_broker] << endl;

   metaSystem->jobForwarded(job,toForward);
   toForward->jobAdd2waitq(job); 
   //the job has been forwarded, so we just return 
   return;
}


  /**
    We have in MaxRankResource the most appropiate resource
  */

  /**
    If bestResource==NULL there is not available resources for this job. 
    It should be queued in the waiting queue for retrying later (when a job finishes -> resources are become free
  */
  if(bestResource==NULL){
    this->waitQueue->insert(job);

cout<<"QUEUED JOB: "<< job->getJobNumber() << endl;

  }
  else{


//cout<<"choise="<<choise<<endl;

    //now the sumbission should be decided so.. just submit it !
    string centerName = this->BRANKPolicyCentersName->operator[] (good_choise);

cout << "LOCAL EXECUTION JobID:" << job->getJobNumber() << " on resource:"<<centerName<<endl;

    //cout<<"The job "<<itos(job->getJobNumber())<<" has been submitted to the center "<<centerName<<endl;
    log->debug("The job "+itos(job->getJobNumber())+" has been submitted to the center "+centerName,2);
  
    bestResource->jobAdd2waitq(job);
  
    this->submissions.insert(PairSubmission(job,/*goodResource*/bestResource));
  
  }

  //free the best metric 
  delete bestMetric;

}



/**
 * TO COMMENT...
 */

void BRANKPolicy::jobAdd2waitq_REGULAR(Job* job){
 

  int queue_policy = 3;

  /* To determine policy :
	1: FCFS
	2: BACKFILLING STARVATION
	3: BACKFILLING WITH RANG
  */


  Metric* bestMetric = NULL;
  //SchedulingPolicy* bestPolicy = NULL;
  
  SchedulingPolicy* bestResource = NULL;
  double bestRANK = -1;
  double tmpRANK = -1;  

  int choise = 0; 

  int count = 0; //controls the resource number in the loop
  GridJob* gjob = (GridJob*) job;


  // If there is any waiting job I can not submit another one (because of FCFS)
  if(this->waitQueue->getjobs() > 0){

    // If the waiting queue contains the job means that is rescheduled...
    if(!this->waitQueue->contains(job)){
	switch(queue_policy){
		case 1:
	      		this->waitQueue->insert(job);
			cout<<"WAIT_QUEUE SIZE:"<<this->waitQueue->getjobs()<<endl;
			delete bestMetric;
 
			return;
		case 2: // nothing to do
			break;
		case 3:
			if(this->waitQueue->getjobs() > 0){
				if(getJobRank(job) >= getJobRank(this->waitQueue->begin())){

	      				this->waitQueue->insert(job);
					cout<<"HIGHER JOB RANK:"<<endl;
					delete bestMetric;
 
					return;
				}
			}
			break;
		default:
			break;
	} 
    }
    else{
      this->waitQueue->erase(job);
    }

  }

  /**
      now its time to process the job grid specific fields. 
  */

  Metric* cspeedMetric = gjob->getClockSpeedRequirement()->getmetric();    
  double cspeedRequired = cspeedMetric->getnativeDouble();
  operator_t cspeedOperator = gjob->getClockSpeedRequirement()->getoperatorRequired();

  Metric* vendorMetric = gjob->getVendorRequirement()->getmetric();    
  string vendorRequired = vendorMetric->getnativeString();;
  operator_t vendorOperator = gjob->getVendorRequirement()->getoperatorRequired();

  Metric* osNameMetric = gjob->getOSNameRequirement()->getmetric();    
  string osNameRequired = osNameMetric->getnativeString();;
  operator_t osNameOperator = gjob->getOSNameRequirement()->getoperatorRequired();

  Metric* memoryMetric = gjob->getMemoryRequirement()->getmetric();    
  double memoryRequired = memoryMetric->getnativeDouble();
  operator_t memoryOperator = gjob->getMemoryRequirement()->getoperatorRequired();

  Metric* totalCpusMetric = gjob->getNumberProcessorsRequirement()->getmetric();    
  double totalCpusRequired = totalCpusMetric->getnativeDouble();
  operator_t totalCpusOperator = gjob->getNumberProcessorsRequirement()->getoperatorRequired();

  Metric* diskSizeMetric = gjob->getDiskRequirement()->getmetric();    
  double diskSizeRequired = diskSizeMetric->getnativeDouble();
  operator_t diskSizeOperator = gjob->getDiskRequirement()->getoperatorRequired();


  //cout<<"Processing Job number "<<job->getJobNumber()<<endl;


  /**
      We have in the following variables:

	double cspeedRequired -> CPU Speed in MhZ (e.g. 3200)
	string vendorRequired -> CPU architecture (e.g. "Intel")
	string osNameRequired -> OS Name (e.g. "Linux")
	double memoryRequired -> RAM memory in MB (e.g. 2048)
	double totalCpusRequired -> Number of CPU (e.g. 4)
	double diskSizeRequired -> Disk size in MB (e.g. 4000)
  	
     And we have a operator_t for each one (=, <, >, <=, >=)
  */

  /**
	RANK Algorithm (given a job requirements + "recommendations " + FACTORS, choose the appropiate resource)

	1. For resource{
		2. For each of the previous variables{
			3. Obtain a RANK using variables and FACTORS
				3.1. If the resource does not fit the requirements -> RANK = -1 (we can go ahead with the following resource)
				3.2. If the operator_t is different than "=", obtains RANK value (e.g. for FreeMemory, if it asks for 100MB and there are 200MB available MB, multiply the factor per this additional 100MB).
		}
		4. Calculates the resource with maximum RANK value
	}
	5. Selects the resource with maximum RANK value if it is different than -1
  */


///////////////// FACTOR FOR OWN BROKER POTENTIATION

double own_broker_factor = 1.05;


int broker_count = 0;
int current_broker = -1;
int selected_broker = -1;
double bestBrokerRANK = 0;

vector<SchedulingPolicy*>* Brokers = this->metaSystem->getVirtualOrganitzations();
BRANKPolicy* toForward = NULL;

for(vector<SchedulingPolicy*>::iterator it = Brokers->begin(); it != Brokers->end();++it)
{
  BRANKPolicy* broker = (BRANKPolicy*) *it;



  // To calculate the AVG Slowdown of the broker
  double total_slowdown = 0;

  bestRANK = -1;
  tmpRANK = -1;  
  choise = 0; 
  count = 0;


  /* We iterate to each cluster */
  for(vector<SchedulingPolicy*>::iterator polit = broker->BRANKPolicyPolicies->begin();
      polit != broker->BRANKPolicyPolicies->end();
      ++polit)
  {
    SchedulingPolicy* currentResource = *polit;

    string center = broker->BRANKPolicyCentersName->operator[] (count);      
    //cout<<"Analysing resource: "+center <<endl;

    /**
       for the center we get the dynamic information 
    */
    Metric* memoryAVMetric = currentResource->EstimatePerformanceResponse(MEMORY_AVAILABLE,job);
    double memory = memoryAVMetric->getnativeDouble();

    Metric* diskAvailable = currentResource->EstimatePerformanceResponse(DISK_AVAILABLE,job);
    double disk = diskAvailable->getnativeDouble();

    Metric* cpusAvailable = currentResource->EstimatePerformanceResponse(FREE_CPUS,job);
    double cpus = cpusAvailable->getnativeDouble();

    Metric* prevSldMetric = currentResource->EstimatePerformanceResponse(AVG_SLD_FINISHED_JOBS,job);
    double prevSld = prevSldMetric->getnativeDouble();


cout<<"prevSld="<<prevSld<<", total_slowdown="<<total_slowdown<<endl;

    total_slowdown += prevSld;

    double running = currentResource->getJobsIntheRQ();;

    //Metric* prevRTMetric = currentResource->EstimatePerformanceResponse(AVG_RT_FINISHED_JOBS,job);
    //double prevRT = prevRTMetric->getnativeDouble();

    //Metric* prevRespTMetric = currentResource->EstimatePerformanceResponse(AVG_RESPT_FINISHED_JOBS,job);
    //double prevRespT = prevRespTMetric->getnativeDouble();

    //Metric* prevWTMetric = currentResource->EstimatePerformanceResponse(AVG_WT_FINISHED_JOBS,job);
    //double prevWT = prevWTMetric->getnativeDouble();


  
    /**
       for the center we get the static information 
    */
    Metric* vendorMetric = currentResource->getStaticSystemInformation(VENDOR);
    string vendor = vendorMetric->getnativeString();

    Metric* osNameMetric = currentResource->getStaticSystemInformation(OS_NAME);
    string osName = osNameMetric->getnativeString();

    Metric* totalCpusMetric = currentResource->getStaticSystemInformation(TOTAL_CPUS);
    double totalCpus = totalCpusMetric->getnativeDouble();

    Metric* clockSpeedMetric = currentResource->getStaticSystemInformation(CLOCK_SPEED);
    double clockSpeed = clockSpeedMetric->getnativeDouble();

    Metric* diskSizeMetric = currentResource->getStaticSystemInformation(DISK_SIZE);
    double diskSize = diskSizeMetric->getnativeDouble();

    Metric* memorySizeMetric = currentResource->getStaticSystemInformation(MEMORY_SIZE);
    double memorySize = memorySizeMetric->getnativeDouble();



    /**
	We define the FACTORS for calculating the RANK values
    */

    //100 is the reference value (25 -> 0.25)


/* DISK 


    int CSPEED_FACTOR=200;
    int TOTAL_CSPEED_FACTOR=50;
    int FREECPUS_FACTOR=100;
    int TOTALCPUS_FACTOR=100;

    int MEMORY_FACTOR=4;

    int DISKSIZE_FACTOR=64; //1

    int RUNNING_JOBS_FACTOR = 40000; //1k
    int AVG_SLD_FINISHED_JOBS_FACTOR=10000; //10k
*/




/* MEM

    int CSPEED_FACTOR=200;
    int TOTAL_CSPEED_FACTOR=50;
    int FREECPUS_FACTOR=100;
    int TOTALCPUS_FACTOR=100;

    int MEMORY_FACTOR=64;//4

    int DISKSIZE_FACTOR=1;

    int RUNNING_JOBS_FACTOR = 40000; //1k
    int AVG_SLD_FINISHED_JOBS_FACTOR=10000; //10k
*/



/* CPU

    int CSPEED_FACTOR=800;
    int TOTAL_CSPEED_FACTOR=200;
    int FREECPUS_FACTOR=200;
    int TOTALCPUS_FACTOR=200;

    int MEMORY_FACTOR=4;

    int DISKSIZE_FACTOR=1;

    int RUNNING_JOBS_FACTOR = 40000; //1k
    int AVG_SLD_FINISHED_JOBS_FACTOR=10000; //10k
*/

/* BALANCED: 
*/
    int CSPEED_FACTOR=200;//100
    int TOTAL_CSPEED_FACTOR=50;//25
    int FREECPUS_FACTOR=100;
    int TOTALCPUS_FACTOR=100;

    int MEMORY_FACTOR=4;

    int DISKSIZE_FACTOR=1;

    int RUNNING_JOBS_FACTOR = 40000; //1k
    int AVG_SLD_FINISHED_JOBS_FACTOR=10000; //10k


    /**
      Process job REQUIREMENTS (job extension)

      For VENDOR and OS_NAME we do not need operator (just "=")
      They are only used for requirements
    */

    // currentRANK is the RANK for the current resource depending on the job requirements
    double currentRANK = 0;

    if(vendorRequired.compare("ANY")){
      // if it is empty it is not a requirement
      if(vendor.compare(vendorRequired)){
	// different strings vendors
	currentRANK = -1;
      }
      // if not, there is not anything to do
    } 
   

    if(currentRANK!=-1){
      if(osNameRequired.compare("ANY")){
        if(osName.compare(osNameRequired)){
	  // different strings vendors
	  currentRANK = -1;	
        }
      }	
    }

    /**
      The rest of GridJob requirements are double type
    */

    // CPU speed (difference between the total and the required)
    if(currentRANK!=-1 && cspeedRequired!=-1){
      tmpRANK = getRANK(clockSpeed, cspeedRequired, cspeedOperator, CSPEED_FACTOR);
      if(tmpRANK != -1){
        currentRANK += tmpRANK;
      }
      else{
        currentRANK = -1;
      }
    }

    // # Free CPUs 
    if(currentRANK!=-1 && totalCpusRequired!=-1){
      tmpRANK = getRANK(cpus, totalCpusRequired, totalCpusOperator, FREECPUS_FACTOR);
      if(tmpRANK != -1){
        currentRANK += tmpRANK;
      }
      else{
        currentRANK = -1;
        cout<<"Job "<<  job->getJobNumber() << ": Not enough CPUS in resource:" << center << endl;
      }
    }

    // % Free memory
    if(currentRANK!=-1 && memoryRequired!=-1){
      tmpRANK = getRANK(memory, memoryRequired, memoryOperator, 1);
      if(tmpRANK != -1){
        currentRANK += MEMORY_FACTOR*tmpRANK/memorySize;
      }
      else{
        currentRANK = -1;
        cout<<"Job "<<  job->getJobNumber() << ": Not enough MEMORY in resource:" << center << endl;
      }
    }

    // % Free Disk
    if(currentRANK!=-1 && diskSizeRequired!=-1){
      tmpRANK = getRANK(disk, diskSizeRequired, diskSizeOperator, 1);
      if(tmpRANK != -1){
        currentRANK += DISKSIZE_FACTOR*tmpRANK/diskSize;
      }
      else{
        currentRANK = -1;
        cout<<"Job "<<  job->getJobNumber() << ": Not enough DISK in resource:" << center << endl;
      }
    }



    /**
      The regular job (no extension) has to be considered for obtaining the RANK value
    */

    // #FREE CPUS

    // We determine if there are enough free CPUs if it is not a requirement
    // cpus is #available CPUs 

/* NOT reachable because we always specify the required number of procs
    double jobCpus = gjob->getNumberProcessors();

    if(currentRANK!=-1 && totalCpusRequired==-1){
      tmpRANK = getRANK(cpus, jobCpus, HIGHER_EQ_THAN, FREECPUS_FACTOR);
      if(tmpRANK != -1){
        currentRANK += tmpRANK;
      }
      else{
        currentRANK = -1;
      }
    }
*/

    // %FREE CPUS

    // totalCpus is the number of the resource CPUs
    // cpus is #available CPUs 

    double percent = cpus / totalCpus;

    if(currentRANK!=-1){
      tmpRANK = getRANK(percent, 0, HIGHER_EQ_THAN, TOTALCPUS_FACTOR*512);
      if(tmpRANK != -1){
        currentRANK += tmpRANK;
      }
      else{
        currentRANK = -1;
      }
    }



    /**
      The static values can be also taken into account for obtaining the RANK value, but not for the moment... 
    */


    // #CPUS

    //totalCpus is the number of the resource CPUs

    int myfactor = (int) totalCpus / 10 +1;

    if(currentRANK!=-1){
      tmpRANK = getRANK(myfactor, 0, HIGHER_EQ_THAN, TOTALCPUS_FACTOR);
      if(tmpRANK != -1){
        currentRANK += tmpRANK;
      }
      else{
        currentRANK = -1;
      }
    }

    // CPU Speed (MHz)

    //clockSpeed is the CPUs speed
    //myfactor = (int) clockSpeed / 4;

    if(currentRANK!=-1){
      tmpRANK = getRANK(clockSpeed, 0, HIGHER_EQ_THAN, TOTAL_CSPEED_FACTOR);
      if(tmpRANK != -1){
        currentRANK += tmpRANK;
      }
      else{
        currentRANK = -1;
      }
    }


    // Total Memory size
    // Total Disk size





    /**
      Go ahead with performance metrics...
    */


    // #RUNNING JOBS (inverse)

    if(currentRANK!=-1){
      tmpRANK = getRANK(running, 0, HIGHER_EQ_THAN, 1);
      if(tmpRANK != -1){
        currentRANK += RUNNING_JOBS_FACTOR/tmpRANK;
      }
      else{
        currentRANK = -1;
      }
    }

    // AVG Slowdown of local schedulers

    if(currentRANK!=-1){
//cout<<"AVG Slowdown: "<<prevSld<<endl;

      tmpRANK = getRANK(prevSld, 0, HIGHER_EQ_THAN, 1);
      if(tmpRANK != -1){
        currentRANK += AVG_SLD_FINISHED_JOBS_FACTOR/tmpRANK;
      }
      else{
        currentRANK = -1;
      }
    }


    /** 
      Perform the selection based on RANK value 
    */

    //cout<<"Total current RANK value for resource number "<<count<<" is: "<<currentRANK<<endl;


    if(currentRANK > bestRANK){
      bestResource = currentResource;
      bestRANK = currentRANK;
      choise = count;
    }

    count++;

    // FREE MEMORY


    delete memoryAVMetric;
    delete diskAvailable;
    delete cpusAvailable;
    delete prevSldMetric;
    delete vendorMetric;
    delete osNameMetric;
    delete totalCpusMetric;
    delete clockSpeedMetric;
    delete diskSizeMetric;
    delete memorySizeMetric;

  }  //end foreach resource
  

  // Computes Average Slowdown of the resources (for my resources)
  double AVG_slowdown = 1;
  if(count){
    AVG_slowdown = (double) total_slowdown/(double)count;
  }

  cout<<"AVG_slowdown="<<AVG_slowdown<<endl;


 if(broker == this){
  bestRANK *= own_broker_factor; 
  current_broker = broker_count;
 }

 if(bestRANK>bestBrokerRANK){
   selected_broker = broker_count;
   bestBrokerRANK = bestRANK;
   toForward = broker;
   //bestResource is where to submit in case that it select the current broker!
 }

 broker_count++;

} //brokers




if(toForward != NULL && toForward != this){

  cout << "FORWARDING: JobID:"<< job->getJobNumber() << " OriginalVO:" << gjob->getVOSubmitted() << " FromVO:" << this->metaSystem->getVONames()[current_broker] << " ToVO:" << this->metaSystem->getVONames()[selected_broker] << endl;

   metaSystem->jobForwarded(job,toForward);
   toForward->jobAdd2waitq(job); 
   //the job has been forwarded, so we just return 
   return;
}

  /**
    We have in MaxRankResource the most appropiate resource
  */

  /**
    If bestResource==NULL there is not available resources for this job. 
    It should be queued in the waiting queue for retrying later (when a job finishes -> resources are become free
  */
  if(bestResource==NULL){
    this->waitQueue->insert(job);

cout<<"QUEUED JOB: "<< job->getJobNumber() << endl;

  }
  else{

cout << "LOCAL EXECUTION JobID:" << job->getJobNumber() << endl;

    //now the sumbission should be decided so.. just submit it !
    string centerName = this->BRANKPolicyCentersName->operator[] (choise);

    //cout<<"The job "<<itos(job->getJobNumber())<<" has been submitted to the center "<<centerName<<endl;
    log->debug("The job "+itos(job->getJobNumber())+" has been submitted to the center "+centerName,2);
  
    bestResource->jobAdd2waitq(job);
  
    this->submissions.insert(PairSubmission(job,bestResource));
  
  }

  //free the best metric 
  delete bestMetric;
}




/**
 * TO COMMENT...
 */

void BRANKPolicy::jobAdd2waitq_AGGREGATED(Job* job){
 
  Metric* bestMetric = NULL;
  //SchedulingPolicy* bestPolicy = NULL;
  
  SchedulingPolicy* bestResource = NULL;
  double bestRANK = -1;
  double tmpRANK = -1;  

  int choise = 0; 
  int good_choise = 0;

  int count = 0; //controls the resource number in the loop
  GridJob* gjob = (GridJob*) job;


  // If there is any waiting job I can not submit another one (because of FCFS)
  if(this->waitQueue->getjobs() > 0){

    // If the waiting queue contains the job means that is rescheduled...
    if(!this->waitQueue->contains(job)){ 
      this->waitQueue->insert(job);
      delete bestMetric;
 
      return;
    }
    else{
      this->waitQueue->erase(job);
    }

  }

  /**
      now its time to process the job grid specific fields. 
  */

  Metric* cspeedMetric = gjob->getClockSpeedRequirement()->getmetric();    
  double cspeedRequired = cspeedMetric->getnativeDouble();
  operator_t cspeedOperator = gjob->getClockSpeedRequirement()->getoperatorRequired();

  Metric* vendorMetric = gjob->getVendorRequirement()->getmetric();    
  string vendorRequired = vendorMetric->getnativeString();;
  operator_t vendorOperator = gjob->getVendorRequirement()->getoperatorRequired();

  Metric* osNameMetric = gjob->getOSNameRequirement()->getmetric();    
  string osNameRequired = osNameMetric->getnativeString();;
  operator_t osNameOperator = gjob->getOSNameRequirement()->getoperatorRequired();

  Metric* memoryMetric = gjob->getMemoryRequirement()->getmetric();    
  double memoryRequired = memoryMetric->getnativeDouble();
  operator_t memoryOperator = gjob->getMemoryRequirement()->getoperatorRequired();

  Metric* totalCpusMetric = gjob->getNumberProcessorsRequirement()->getmetric();    
  double totalCpusRequired = totalCpusMetric->getnativeDouble();
  operator_t totalCpusOperator = gjob->getNumberProcessorsRequirement()->getoperatorRequired();

  Metric* diskSizeMetric = gjob->getDiskRequirement()->getmetric();    
  double diskSizeRequired = diskSizeMetric->getnativeDouble();
  operator_t diskSizeOperator = gjob->getDiskRequirement()->getoperatorRequired();


  ///////////////// FACTOR FOR OWN BROKER POTENTIATION

  // AQUI
  double own_broker_factor = 1.10;


  int broker_count = 0;
  int current_broker = -1;
  //int selected_broker = -1;
  BRANKPolicy* selected_broker = NULL;
  double bestBrokerRANK = 0;

  vector<SchedulingPolicy*>* Brokers = this->metaSystem->getVirtualOrganitzations();

  //this->getCenterName(); name of own broker!!!!

  //BRANKPolicy* toForward = NULL;

  /* BROKER SELECTION WITH AGGREGATED */

  bestRANK = -1;
  tmpRANK = -1;  

  //choise = 0; 
  //count = 0;

  int broker_counter = 0;
  for(vector<SchedulingPolicy*>::iterator it = Brokers->begin(); it != Brokers->end();++it, ++broker_counter)
  {
	BRANKPolicy* broker = (BRANKPolicy*) *it;

    	//we gather the aggregate 
    	AggregateMetabrokeringInfo* info = broker->getAggregateMetabrokeringInfo();

    	// generate a vector of comprensible aggregated resources -> completeAggregated


   	vector <reducedAggregated*>* info_aggregated = new vector <reducedAggregated*>;

	//vector <completeAggregated*> aggregated;

	vector<completeAggregated*> aggregated;

	reducedAggregated* myAggr;

	// From relationships we can find resources to fulfill aggregated
   	vector<RelationshipAggregatedInfo*>* rels = info->getRelationshipsInfo();
   	for(vector<RelationshipAggregatedInfo*>::iterator it2 = rels->begin() ; it2!=rels->end(); it2++){
        
		RelationshipAggregatedInfo* rai = (RelationshipAggregatedInfo*) *it2;
		
		vector <reducedAggregated *>* tmp_aggregated = this->getSubvector(info_aggregated, rai->getSourceName());

		if( tmp_aggregated->size() == 0){
			// Insert CS with the OS/FS
			myAggr = new reducedAggregated();
			if( rai->getTargetType() == "OperatingSystem" ){
				myAggr->setCS(rai->getSourceName());
				myAggr->setOS(rai->getTargetName());
			}
			if( rai->getTargetType() == "FileSystem" ){
				myAggr->setCS(rai->getSourceName());
				myAggr->setFS(rai->getTargetName());
			}
			info_aggregated->push_back(myAggr);
		}
		else{
			// REST OF THE ALGORITHM... (HOJA)

			if( rai->getTargetType() == "OperatingSystem" ){
				int found = 0;
				for(vector<reducedAggregated *>::iterator it3 = tmp_aggregated->begin(); it3!=tmp_aggregated->end(); it3++){
					reducedAggregated* tmpAggr = (reducedAggregated*) *it3;
					if(tmpAggr->getOS() == ""){
						// it means that there is already a FS 
						tmpAggr->setOS(rai->getTargetName()); 
						found = 1;
					}
				}
				if(!found){
					// Create a new because there is already another aggregated with this CS and OS
					myAggr = new reducedAggregated();
					myAggr->setCS(rai->getSourceName());
					myAggr->setOS(rai->getTargetName());
					info_aggregated->push_back(myAggr);	
				}
			}
			if( rai->getTargetType() == "FileSystem" ){
				int found = 0;
				for(vector<reducedAggregated *>::iterator it3 = tmp_aggregated->begin(); it3!=tmp_aggregated->end(); it3++){
					reducedAggregated* tmpAggr = (reducedAggregated*) *it3;
					if(tmpAggr->getFS() == ""){
						// it means that there is already a OS 
						tmpAggr->setFS(rai->getTargetName()); 
						found = 1;
					}
				}
				if(!found){
					// Create a new because there is already another aggregated with this CS and FS
					myAggr = new reducedAggregated();
					myAggr->setCS(rai->getSourceName());
					myAggr->setFS(rai->getTargetName());
					info_aggregated->push_back(myAggr);
				}

			}
		}


		delete tmp_aggregated;

        	// to print relationships...
		/*
        	cout<<"Relationship:"<<endl;	
		cout<<"SourceType: "<<rai->getSourceType()<<", SourceName: "<<rai->getSourceName()<<endl;
		cout<<"TargetType: "<<rai->getTargetType()<<", TargetName: "<<rai->getTargetName()<<endl;
		*/


    	} //relationships


	/* Convert "info_aggregated" (reducedAggregated) to "aggregated" (completeAggregated)*/

	for(vector<reducedAggregated *>::iterator it3 = info_aggregated->begin(); it3!=info_aggregated->end(); it3++)
	{
		reducedAggregated* tmpAggr = (reducedAggregated*) *it3;

		// find aggregated resource 
		ResourceAggregatedInfo *myCS = findAggregatedResource( info, tmpAggr->getCS());
		ResourceAggregatedInfo *myOS = findAggregatedResource( info, tmpAggr->getOS());
		ResourceAggregatedInfo *myFS = findAggregatedResource( info, tmpAggr->getFS());
	
		completeAggregated *newAggr = new completeAggregated();
		newAggr->insertCS(myCS);
		newAggr->insertOS(myOS);
		newAggr->insertFS(myFS);

		aggregated.push_back(newAggr);

		delete tmpAggr;
	} 

	delete info_aggregated;



	// To print completeAggregated resources...

/*
	int counter = 0;
	for(vector<completeAggregated *>::iterator it3 = aggregated.begin(); it3!=aggregated.end(); it3++, ++counter)
 	{
    		completeAggregated* currentAggr = (completeAggregated*) *it3;
	
		cout<<"BROKER("<<broker_counter<<") and RESOURCE ("<<counter<<"):"<<endl;
		cout<<"============="<<endl;
		cout<<"ProcType: "<<currentAggr->getCS()->getProcType()<<endl;
		cout<<"Number: "<<currentAggr->getCS()->getNum()<<endl;
		cout<<"NumProc(MAX): "<<currentAggr->getCS()->getNumProc_max()<<endl;
		cout<<"CpuSpeed(MAX): "<<currentAggr->getCS()->getCpuSpeed_max()<<endl;
		cout<<"CpuUtil(MAX): "<<currentAggr->getCS()->getCpuUtil_max()<<endl;
		cout<<"OSType: "<<currentAggr->getOS()->getOsType()<<endl;
		cout<<"TotalMem(MAX): "<<currentAggr->getOS()->getTotalMem_max()<<endl;
		cout<<"FreeMem(MAX): "<<currentAggr->getOS()->getFreeMem_max()<<endl;
		cout<<"TotalStorage(MAX): "<<currentAggr->getFS()->getTotalStorageCapacity_max()<<endl;
		cout<<"FreeStorage(MAX): "<<currentAggr->getFS()->getFreeStorageCapacity_max()<<endl;
	}
*/


   	// to print resources...
    	/*
    	vector<ResourceAggregatedInfo*>* ress = info->getResourcesInfo();
    	for(vector<ResourceAggregatedInfo*>::iterator it2 = ress->begin() ; it2!=ress->end(); it2++){
		ResourceAggregatedInfo* rai = (ResourceAggregatedInfo*) *it2;
		cout<<"ResourceType: "<<rai->getResourceType()<<endl;
		cout<<"ResourceName: "<<rai->getResourceName()<<endl;
		for(vector<Metric*>::iterator it3 = rai->getAttributes()->begin() ; it3!=rai->getAttributes()->end(); it3++){
			Metric* attr = (Metric*) *it3;
			cout<<"Attribute: "<<(string)attr->getnativeString()<<endl;
		}
     	}
     	*/



		/* BALANCED: 
		*/
   		int AGGR_CSPEED_FACTOR=5;//75
    		int AGGR_MAX_CSPEED_FACTOR=50;//25
    		int AGGR_FREECPUS_FACTOR=150; //100
    		int AGGR_TOTALCPUS_FACTOR=10; //25

    		int AGGR_MEMORY_FACTOR=4;//4

    		int AGGR_DISKSIZE_FACTOR=1; //1

    		//int AGGR_RUNNING_JOBS_FACTOR = 40000; //1k
    		int AGGR_AVG_SLD_FINISHED_JOBS_FACTOR=100000; //100k



	// In "aggregated" is all the info for matching and computing RANKs

	// compute tmpRANK for each completeResource

	tmpRANK = -1;
	double new_rank = -1;

	for(vector< completeAggregated*>::iterator it3 = aggregated.begin(); it3 != aggregated.end(); it3++)
 	{
		//string current_res = (*it3).first; //key value
    		completeAggregated* currentAggr = (completeAggregated*) *it3;
	
		/* LIKE REGULAR RESOURCES ... */

		/**
		  Process job REQUIREMENTS (job extension)

      		  For VENDOR and OS_NAME we do not need operator (just "=")
	      	  They are only used for requirements
    		*/

    		// currentRANK is the RANK for the current resource depending on the job requirements
    		double currentRANK = 0;


    		if(vendorRequired.compare("ANY")){
      		// if it is empty it is not a requirement
      			if(currentAggr->getCS()->getProcType().compare(vendorRequired)){
				// different strings vendors
				currentRANK = -1;
      			}
      			// if not, there is not anything to do
    		} 


		if(currentRANK!=-1){
			if(osNameRequired.compare("ANY")){
        			if(currentAggr->getOS()->getOsType().compare(osNameRequired)){
					// different strings vendors
					currentRANK = -1;	
        			}
      			}	
    		}

    		/**
	      	  The rest of GridJob requirements are double type
    		*/

    		// CPU speed (difference between the total and the required)
    		if(currentRANK!=-1 && cspeedRequired!=-1){
			double avg_speed = (double) currentAggr->getCS()->getCpuSpeed_sum() / currentAggr->getCS()->getNum();
      			tmpRANK = getRANK(avg_speed, cspeedRequired, cspeedOperator, AGGR_CSPEED_FACTOR);
			if(tmpRANK != -1){
	        		currentRANK += tmpRANK;
      			}
      			else{
        			currentRANK = -1;
      			}
    		}

    		// # Free CPUs 
    		if(currentRANK!=-1 && totalCpusRequired!=-1){
			long CpuUtil_avg = currentAggr->getCS()->getCpuUtil_sum()/currentAggr->getCS()->getNum();
			long NumProc_avg = currentAggr->getCS()->getNumProc_sum()/currentAggr->getCS()->getNum();
			//double my_freeCPUs = (double) (currentAggr->getCS()->getNumProc_max()*100 - currentAggr->getCS()->getCpuUtil_max())/100;
			//double my_freeCPUs = (double) (NumProc_avg*100 - CpuUtil_avg)/100;

			double my_freeCPUs = currentAggr->getCS()->getNumProc_max();

      			tmpRANK = getRANK(my_freeCPUs, totalCpusRequired, totalCpusOperator, AGGR_FREECPUS_FACTOR);
      			if(tmpRANK != -1){
       		 		//currentRANK += tmpRANK;
				// only to fit requirements
      			}
      			else{
        			currentRANK = -1;
        			cout<<"Job "<<  job->getJobNumber() << ": Not enough CPUS in AGGREGATED resource:" << endl;
      			}
    		}

    		// % Free memory
    		if(currentRANK!=-1 && memoryRequired!=-1){
			long FreeMem_avg = currentAggr->getOS()->getFreeMem_sum()/currentAggr->getOS()->getNum();
			long TotalMem_avg = currentAggr->getOS()->getTotalMem_sum()/currentAggr->getOS()->getNum();
      			tmpRANK = getRANK(FreeMem_avg, memoryRequired, memoryOperator, 1);
			tmpRANK = getRANK(currentAggr->getOS()->getFreeMem_max(), memoryRequired, memoryOperator, 1);
      			if(tmpRANK != -1){
				//currentRANK += AGGR_MEMORY_FACTOR*tmpRANK/TotalMem_avg;
				// el de arriba era el bueno
        			//currentRANK += AGGR_MEMORY_FACTOR*tmpRANK/currentAggr->getOS()->getTotalMem_max();
      			}
      			else{
        			currentRANK = -1;
        			cout<<"Job "<<  job->getJobNumber() << ": Not enough MEMORY in AGGREGATED resource:" << endl;
      			}
    		}

    		// % Free Disk
    		if(currentRANK!=-1 && diskSizeRequired!=-1){
			long FreeStorageCapacity_avg = currentAggr->getFS()->getFreeStorageCapacity_sum()/currentAggr->getFS()->getNum();
			long TotalStorageCapacity_avg = currentAggr->getFS()->getTotalStorageCapacity_sum()/currentAggr->getFS()->getNum();
      			//tmpRANK = getRANK(currentAggr->getFS()->getFreeStorageCapacity_max(), diskSizeRequired, diskSizeOperator, 1);
			tmpRANK = getRANK(FreeStorageCapacity_avg, diskSizeRequired, diskSizeOperator, 1);
      			if(tmpRANK != -1){
        			//currentRANK += AGGR_DISKSIZE_FACTOR*tmpRANK/currentAggr->getFS()->getTotalStorageCapacity_max();
				//currentRANK += AGGR_DISKSIZE_FACTOR*tmpRANK/TotalStorageCapacity_avg;
      			}
      			else{
        			currentRANK = -1;
        			cout<<"Job "<<  job->getJobNumber() << ": Not enough DISK in AGGREGATED resource:" << endl;
      			}
    		}

    		/**
      		  The regular job (no extension) has to be considered for obtaining the RANK value
    		*/

    		// #FREE CPUS

   		// We determine if there are enough free CPUs if it is not a requirement
    		// cpus is #available CPUs 

	
		/* NOT REACHABLE -> we specify always the totalCpusRequired...

    		double jobCpus = gjob->getNumberProcessors();

    		if(currentRANK!=-1 && totalCpusRequired==-1){
			double my_freeCPUs = (double) (currentAggr->getCS()->getNumProc_max() - currentAggr->getCS()->getCpuUtil_max())/100;
      			tmpRANK = getRANK(my_freeCPUs, jobCpus, HIGHER_EQ_THAN, AGGR_FREECPUS_FACTOR);
      			if(tmpRANK != -1){
        			currentRANK += tmpRANK;
      			}
      			else{
        			currentRANK = -1;
      			}
    		}
		*/


    		// %FREE CPUS

    		// totalCpus is the number of the resource CPUs
    		// cpus is #available CPUs 
		/*
    		double percent = cpus / totalCpus;

    		if(currentRANK!=-1){
      			tmpRANK = getRANK(percent, 0, HIGHER_EQ_THAN, TOTALCPUS_FACTOR*512);
      			if(tmpRANK != -1){
        			currentRANK += tmpRANK;
      			}
      			else{
        			currentRANK = -1;
      			}
    		}
		*/

    		/**
      		  The static values can be also taken into account for obtaining the RANK value, but not for the moment... 
    		*/

    		// #CPUS

    		//totalCpus is the number of the resource CPUs


		/*
		double my_freeCPUs = (double) (currentAggr->getCS()->getNumProc_sum() / currentAggr->getCS()->getNum());

    		int myfactor = (int) my_freeCPUs / 10 +1;

    		if(currentRANK!=-1){
      			tmpRANK = getRANK(myfactor, 0, HIGHER_EQ_THAN, AGGR_TOTALCPUS_FACTOR);
      			if(tmpRANK != -1){
        			currentRANK += tmpRANK;
      			}
      			else{
        			currentRANK = -1;
      			}
    		}
		*/


    		// CPU Speed (MHz) - (MAXIMUM)
    		
    		if(currentRANK!=-1){
      			tmpRANK = getRANK((double) currentAggr->getCS()->getCpuSpeed_sum(), 0, HIGHER_EQ_THAN, AGGR_MAX_CSPEED_FACTOR);
      			if(tmpRANK != -1){
        			//currentRANK += tmpRANK;
      			}
      			else{
        			currentRANK = -1;
      			}
    		}


    		// Total Memory size
    		// Total Disk size

    		/**
      		   Performance metrics...
    		*/

		// Here we could use broker AVG slowdown as performance metric...


    		if(currentRANK!=-1){
			//cout<<"AVG Slowdown: "<<prevSld<<endl;

			double avg_slowd = 0; 
			double total_slowd = 0;

  			int new_count = 0;


  			/* We iterate to each cluster */
  			for(vector<SchedulingPolicy*>::iterator new_polit = broker->BRANKPolicyPolicies->begin();
      	    		new_polit != broker->BRANKPolicyPolicies->end();
      	    		++new_polit)
			{
    				SchedulingPolicy* new_currentResource = *new_polit;
		
    				Metric* new_prevSldMetric = new_currentResource->EstimatePerformanceResponse(AVG_SLD_FINISHED_JOBS,job);
    				double new_prevSld = new_prevSldMetric->getnativeDouble();

    				total_slowd += new_prevSld;

				delete new_prevSldMetric;

				new_count++;
			}


			avg_slowd = ( total_slowd / new_count ) + 1;

      			//tmpRANK = getRANK(avg_slowd, 0, HIGHER_EQ_THAN, 1);
      			//if(tmpRANK != -1){
        			//currentRANK += AGGR_AVG_SLD_FINISHED_JOBS_FACTOR/avg_slowd;
				//currentRANK = currentRANK / avg_slowd;
				currentRANK += AGGR_AVG_SLD_FINISHED_JOBS_FACTOR / avg_slowd;

      			//}
      			//else{
        		//	currentRANK = -1;
      			//}

			cout<<"avg_slowd="<<avg_slowd<<endl;

    		}

		if( currentRANK>new_rank ){
			new_rank = currentRANK;
		}

	} // aggregated



	/* Broker selection depending on the rank */

 	if(broker == this){
  		new_rank *= own_broker_factor; 
  		//current_broker = broker_count;
 	}


	if( new_rank > bestRANK ){

		bestRANK = new_rank;
		selected_broker = broker;
	}



cout<<"KKK: JobID:"<<job->getJobNumber()<<", VO:"<<broker->getCenterName()<<", RANK="<<new_rank<<endl;



   	//we free the information 
  	delete info;

  	for(vector<completeAggregated *>::iterator it3 = aggregated.begin(); it3!=aggregated.end(); it3++){
		completeAggregated* currentAggr = (completeAggregated*) *it3;
		delete currentAggr;
  	}

  }



  if(selected_broker == this){

  	// To calculate the AVG Slowdown of the broker
  	double total_slowdown = 0;

  	bestRANK = -1;
  	tmpRANK = -1;  
  	choise = 0; 
  	count = 0;


  	/* We iterate to each cluster */
  	for(vector<SchedulingPolicy*>::iterator polit = this->BRANKPolicyPolicies->begin();
      	    polit != this->BRANKPolicyPolicies->end();
      	    ++polit)
	{
    		SchedulingPolicy* currentResource = *polit;

    		string center = this->BRANKPolicyCentersName->operator[] (count);      
    		//cout<<"Analysing resource: "+center <<endl;

    		/**
       		  for the center we get the dynamic information 
    		*/

    		Metric* memoryAVMetric = currentResource->EstimatePerformanceResponse(MEMORY_AVAILABLE,job);
    		double memory = memoryAVMetric->getnativeDouble();

		Metric* diskAvailable = currentResource->EstimatePerformanceResponse(DISK_AVAILABLE,job);
    		double disk = diskAvailable->getnativeDouble();

    		Metric* cpusAvailable = currentResource->EstimatePerformanceResponse(FREE_CPUS,job);
    		double cpus = cpusAvailable->getnativeDouble();

    		Metric* prevSldMetric = currentResource->EstimatePerformanceResponse(AVG_SLD_FINISHED_JOBS,job);
    		double prevSld = prevSldMetric->getnativeDouble();

    		total_slowdown += prevSld;

    		double running = currentResource->getJobsIntheRQ();;

    		//Metric* prevRTMetric = currentResource->EstimatePerformanceResponse(AVG_RT_FINISHED_JOBS,job);
    		//double prevRT = prevRTMetric->getnativeDouble();

    		//Metric* prevRespTMetric = currentResource->EstimatePerformanceResponse(AVG_RESPT_FINISHED_JOBS,job);
    		//double prevRespT = prevRespTMetric->getnativeDouble();

    		//Metric* prevWTMetric = currentResource->EstimatePerformanceResponse(AVG_WT_FINISHED_JOBS,job);
    		//double prevWT = prevWTMetric->getnativeDouble();

  
    		/**
       		  for the center we get the static information 
    		*/

		Metric* vendorMetric = currentResource->getStaticSystemInformation(VENDOR);
    		string vendor = vendorMetric->getnativeString();

    		Metric* osNameMetric = currentResource->getStaticSystemInformation(OS_NAME);
    		string osName = osNameMetric->getnativeString();

    		Metric* totalCpusMetric = currentResource->getStaticSystemInformation(TOTAL_CPUS);
    		double totalCpus = totalCpusMetric->getnativeDouble();

    		Metric* clockSpeedMetric = currentResource->getStaticSystemInformation(CLOCK_SPEED);
    		double clockSpeed = clockSpeedMetric->getnativeDouble();

    		Metric* diskSizeMetric = currentResource->getStaticSystemInformation(DISK_SIZE);
    		double diskSize = diskSizeMetric->getnativeDouble();

    		Metric* memorySizeMetric = currentResource->getStaticSystemInformation(MEMORY_SIZE);
    		double memorySize = memorySizeMetric->getnativeDouble();



    		/**
		  We define the FACTORS for calculating the RANK values
    		*/

    		//100 is the reference value (25 -> 0.25)

		/* DISK 
	    	int CSPEED_FACTOR=200;
    		int TOTAL_CSPEED_FACTOR=50;
    		int FREECPUS_FACTOR=100;
    		int TOTALCPUS_FACTOR=100;

    		int MEMORY_FACTOR=4;

    		int DISKSIZE_FACTOR=64; //1

    		int RUNNING_JOBS_FACTOR = 40000; //1k
    		int AVG_SLD_FINISHED_JOBS_FACTOR=10000; //10k
		*/

		/* MEM
    		int CSPEED_FACTOR=200;
    		int TOTAL_CSPEED_FACTOR=50;
    		int FREECPUS_FACTOR=100;
    		int TOTALCPUS_FACTOR=100;

    		int MEMORY_FACTOR=64;//4

    		int DISKSIZE_FACTOR=1;

    		int RUNNING_JOBS_FACTOR = 40000; //1k
    		int AVG_SLD_FINISHED_JOBS_FACTOR=10000; //10k
		*/

		/* CPU

    		int CSPEED_FACTOR=800;
    		int TOTAL_CSPEED_FACTOR=200;
    		int FREECPUS_FACTOR=200;
    		int TOTALCPUS_FACTOR=200;

    		int MEMORY_FACTOR=4;

    		int DISKSIZE_FACTOR=1;

    		int RUNNING_JOBS_FACTOR = 40000; //1k
    		int AVG_SLD_FINISHED_JOBS_FACTOR=10000; //10k
		*/

		/* BALANCED: 
		*/
   		int CSPEED_FACTOR=200;//100
    		int TOTAL_CSPEED_FACTOR=50;//25
    		int FREECPUS_FACTOR=100;
    		int TOTALCPUS_FACTOR=100;

    		int MEMORY_FACTOR=4;

    		int DISKSIZE_FACTOR=1;

    		int RUNNING_JOBS_FACTOR = 40000; //1k
    		int AVG_SLD_FINISHED_JOBS_FACTOR=100000; //10k


    		/**
      		  Process job REQUIREMENTS (job extension)

      		  For VENDOR and OS_NAME we do not need operator (just "=")
      		  They are only used for requirements
    		*/

    		// currentRANK is the RANK for the current resource depending on the job requirements
    		double currentRANK = 0;

    		if(vendorRequired.compare("ANY")){
      		// if it is empty it is not a requirement
      			if(vendor.compare(vendorRequired)){
				// different strings vendors
				currentRANK = -1;
      			}
      			// if not, there is not anything to do
    		} 
   

    		if(currentRANK!=-1){
      			if(osNameRequired.compare("ANY")){
        			if(osName.compare(osNameRequired)){
	  				// different strings vendors
	  				currentRANK = -1;	
        			}
      			}	
    		}

    		/**
      		  The rest of GridJob requirements are double type
    		*/

    		// CPU speed (difference between the total and the required)
    		if(currentRANK!=-1 && cspeedRequired!=-1){
			tmpRANK = getRANK(clockSpeed, cspeedRequired, cspeedOperator, CSPEED_FACTOR);
      			if(tmpRANK != -1){
	        		currentRANK += tmpRANK;
      			}
      			else{
        			//currentRANK = -1;
	      		}
    		}

    		// # Free CPUs 
		/*
    		if(currentRANK!=-1 && totalCpusRequired!=-1){
      			tmpRANK = getRANK(cpus, totalCpusRequired, totalCpusOperator, FREECPUS_FACTOR);
      			if(tmpRANK != -1){
        			currentRANK += tmpRANK;
      			}
      			else{
        			currentRANK = -1;
        			cout<<"Job "<<  job->getJobNumber() << ": Not enough CPUS in resource:" << center << endl;
      			}
    		}
		*/

    		// % Free memory
    		if(currentRANK!=-1 && memoryRequired!=-1){
      			tmpRANK = getRANK(memory, memoryRequired, memoryOperator, 1);
      			if(tmpRANK != -1){
        			currentRANK += MEMORY_FACTOR*tmpRANK/memorySize;
      			}
      			else{
        			//currentRANK = -1;
        			cout<<"Job "<<  job->getJobNumber() << ": Not enough MEMORY in resource:" << center << endl;
      			}
    		}

    		// % Free Disk
    		if(currentRANK!=-1 && diskSizeRequired!=-1){
      			tmpRANK = getRANK(disk, diskSizeRequired, diskSizeOperator, 1);
      			if(tmpRANK != -1){
        			currentRANK += DISKSIZE_FACTOR*tmpRANK/diskSize;
      			}
      			else{
        			//currentRANK = -1;
        			cout<<"Job "<<  job->getJobNumber() << ": Not enough DISK in resource:" << center << endl;
      			}
    		}

    		/**
      		  The regular job (no extension) has to be considered for obtaining the RANK value
    		*/

    		// #FREE CPUS

   		// We determine if there are enough free CPUs if it is not a requirement
    		// cpus is #available CPUs 

    		double jobCpus = gjob->getNumberProcessors();

    		if(currentRANK!=-1 && totalCpusRequired==-1){
      			tmpRANK = getRANK(cpus, jobCpus, HIGHER_EQ_THAN, FREECPUS_FACTOR);
      			if(tmpRANK != -1){
        			//currentRANK += tmpRANK;
				// only to fit requirement...
      			}
      			else{
        			currentRANK = -1;
				cout << "Job" << job->getJobNumber() << ": Not enough CPUS to allocate job in resource:"<<center<<endl;
      			}
    		}

    		// %FREE CPUS

    		// totalCpus is the number of the resource CPUs
    		// cpus is #available CPUs 

		/*
    		double percent = cpus / totalCpus;

    		if(currentRANK!=-1){
      			tmpRANK = getRANK(percent, 0, HIGHER_EQ_THAN, TOTALCPUS_FACTOR*512);
      			if(tmpRANK != -1){
        			currentRANK += tmpRANK;
      			}
      			else{
        			currentRANK = -1;
      			}
    		}
		*/

    		/**
      		  The static values can be also taken into account for obtaining the RANK value, but not for the moment... 
    		*/

    		// #CPUS

    		//totalCpus is the number of the resource CPUs

		/*
    		int myfactor = (int) totalCpus / 10 +1;

    		if(currentRANK!=-1){
      			tmpRANK = getRANK(myfactor, 0, HIGHER_EQ_THAN, TOTALCPUS_FACTOR);
      			if(tmpRANK != -1){
        			currentRANK += tmpRANK;
      			}
      			else{
        			currentRANK = -1;
      			}
    		}
		*/

    		// CPU Speed (MHz)

    		//clockSpeed is the CPUs speed
    		//myfactor = (int) clockSpeed / 4;

		/*
    		if(currentRANK!=-1){
      			tmpRANK = getRANK(clockSpeed, 0, HIGHER_EQ_THAN, TOTAL_CSPEED_FACTOR);
      			if(tmpRANK != -1){
        			currentRANK += tmpRANK;
      			}
      			else{
        			currentRANK = -1;
      			}
    		}
		*/

    		// Total Memory size
    		// Total Disk size

    		/**
      		   Go ahead with performance metrics...
    		*/


   		// #RUNNING JOBS (inverse)

    		if(currentRANK!=-1){
      			tmpRANK = getRANK(running, 0, HIGHER_EQ_THAN, 1);
      			if(tmpRANK != -1){
        			currentRANK += RUNNING_JOBS_FACTOR/tmpRANK;
      			}
      			else{
        			//currentRANK = -1;
      			}
    		}

    		// AVG Slowdown of local schedulers

    		if(currentRANK!=-1){
			//cout<<"AVG Slowdown: "<<prevSld<<endl;
			/*
      			tmpRANK = getRANK(prevSld, 0, HIGHER_EQ_THAN, 1);
      			if(tmpRANK != -1){
			*/
        			currentRANK += AVG_SLD_FINISHED_JOBS_FACTOR/tmpRANK;
      			/*
			}
      			else{
        			currentRANK = -1;
      			}
			*/
    		}

    		/** 
      		  Perform the selection based on RANK value 
    		*/

    		//cout<<"Total current RANK value for resource number "<<count<<" is: "<<currentRANK<<endl;

    		if(currentRANK > bestRANK){
      			bestResource = currentResource;
      			bestRANK = currentRANK;
      			choise = count;
			good_choise = count;
    		}

    		count++;


		// FREE MEMORY


    		delete memoryAVMetric;
		delete diskAvailable;
   		delete cpusAvailable;
    		delete prevSldMetric;
		delete vendorMetric;
    		delete osNameMetric;
    		delete totalCpusMetric;
    		delete clockSpeedMetric;
    		delete diskSizeMetric;
    		delete memorySizeMetric;


	}  //end foreach resource
  

  	// Computes Average Slowdown of the resources (for my resources)
/*
  	double AVG_slowdown = 1;
  	if(count){
    		AVG_slowdown = (double) total_slowdown/(double)count;
  	}
*/
	if( bestRANK == -1){

		// Any resource match requirements -> should be rescheduled or submitted to another broker (the following one that matches requirements and that has best rank with aggregated

    		this->waitQueue->insert(job);

		cout<<"LOCAL_QUEUED JOB: "<< job->getJobNumber() << endl;

	}
	else{

		// SUBMIT LOCALLY

    		cout << "LOCAL EXECUTION JobID:" << job->getJobNumber() << endl;

    		//now the sumbission should be decided so.. just submit it !
    		string centerName = this->BRANKPolicyCentersName->operator[] (good_choise);

    		//cout<<"The job "<<itos(job->getJobNumber())<<" has been submitted to the center "<<centerName<<endl;
    		log->debug("The job "+itos(job->getJobNumber())+" has been submitted to the center "+centerName,2);
  
   		bestResource->jobAdd2waitq(job);
  
    		this->submissions.insert(PairSubmission(job,bestResource));

	}
  } // (selected_broker == this)
  else{

	if( selected_broker == NULL ){

		// ANY BROKER MATCH REQUIREMENTS -> QUEUE

    		this->waitQueue->insert(job);

		cout<<"AGGR_QUEUED JOB: "<< job->getJobNumber() << endl;

	}
	else{

		// Forwarding to another broker

  		cout << "FORWARDING: JobID:"<< job->getJobNumber() << " OriginalVO:" << gjob->getVOSubmitted() << " FromVO:" << this->getCenterName() << " ToVO:" << selected_broker->getCenterName() << endl;

   		metaSystem->jobForwarded(job,selected_broker);
   		selected_broker->jobAdd2waitq(job); 
   		//the job has been forwarded, so we just return 
   		return;
	}
  }

  // FREE MEMORY

  //free the best metric 
  delete bestMetric;
  // delete ...
}







/**
 * TO COMMENT...
 */

void BRANKPolicy::jobAdd2waitq_RANDOM(Job* job){
 
  Metric* bestMetric = NULL;
  //SchedulingPolicy* bestPolicy = NULL;
  
  SchedulingPolicy* bestResource = NULL;
  double bestRANK = -1;
  double tmpRANK = -1;  

  int choise = 0; 
  int good_choise = 0;

  int count = 0; //controls the resource number in the loop
  GridJob* gjob = (GridJob*) job;


  // If there is any waiting job I can not submit another one (because of FCFS)
  if(this->waitQueue->getjobs() > 0){

    // If the waiting queue contains the job means that is rescheduled...
    if(!this->waitQueue->contains(job)){ 
      this->waitQueue->insert(job);
      delete bestMetric;
 
      return;
    }
    else{
      this->waitQueue->erase(job);
    }

  }

  /**
      now its time to process the job grid specific fields. 
  */

  Metric* cspeedMetric = gjob->getClockSpeedRequirement()->getmetric();    
  double cspeedRequired = cspeedMetric->getnativeDouble();
  operator_t cspeedOperator = gjob->getClockSpeedRequirement()->getoperatorRequired();

  Metric* vendorMetric = gjob->getVendorRequirement()->getmetric();    
  string vendorRequired = vendorMetric->getnativeString();;
  operator_t vendorOperator = gjob->getVendorRequirement()->getoperatorRequired();

  Metric* osNameMetric = gjob->getOSNameRequirement()->getmetric();    
  string osNameRequired = osNameMetric->getnativeString();;
  operator_t osNameOperator = gjob->getOSNameRequirement()->getoperatorRequired();

  Metric* memoryMetric = gjob->getMemoryRequirement()->getmetric();    
  double memoryRequired = memoryMetric->getnativeDouble();
  operator_t memoryOperator = gjob->getMemoryRequirement()->getoperatorRequired();

  Metric* totalCpusMetric = gjob->getNumberProcessorsRequirement()->getmetric();    
  double totalCpusRequired = totalCpusMetric->getnativeDouble();
  operator_t totalCpusOperator = gjob->getNumberProcessorsRequirement()->getoperatorRequired();

  Metric* diskSizeMetric = gjob->getDiskRequirement()->getmetric();    
  double diskSizeRequired = diskSizeMetric->getnativeDouble();
  operator_t diskSizeOperator = gjob->getDiskRequirement()->getoperatorRequired();


  ///////////////// FACTOR FOR OWN BROKER POTENTIATION

  // AQUI
  double own_broker_factor = 1.10;


  int broker_count = 0;
  int current_broker = -1;
  //int selected_broker = -1;
  BRANKPolicy* selected_broker = NULL;
  double bestBrokerRANK = 0;

  vector<SchedulingPolicy*>* Brokers = this->metaSystem->getVirtualOrganitzations();

  //this->metaSystem->getVONames().size();

  bestRANK = -1;
  tmpRANK = -1;  



  int numVOs = Brokers->size();


  srand ( time(NULL) );

  int randomBroker = rand() % numVOs + 1;

  int broker_counter = 1;
  for(vector<SchedulingPolicy*>::iterator it = Brokers->begin(); it != Brokers->end();++it, ++broker_counter)
  {
	BRANKPolicy* broker = (BRANKPolicy*) *it;

	if( broker_counter == randomBroker ){

		selected_broker = broker;
	}

  }


  if(selected_broker == this){

  	// To calculate the AVG Slowdown of the broker
  	double total_slowdown = 0;

  	bestRANK = -1;
  	tmpRANK = -1;  
  	choise = 0; 
  	count = 0;


  	/* We iterate to each cluster */
  	for(vector<SchedulingPolicy*>::iterator polit = this->BRANKPolicyPolicies->begin();
      	    polit != this->BRANKPolicyPolicies->end();
      	    ++polit)
	{
    		SchedulingPolicy* currentResource = *polit;

    		string center = this->BRANKPolicyCentersName->operator[] (count);      
    		//cout<<"Analysing resource: "+center <<endl;

    		/**
       		  for the center we get the dynamic information 
    		*/

    		Metric* memoryAVMetric = currentResource->EstimatePerformanceResponse(MEMORY_AVAILABLE,job);
    		double memory = memoryAVMetric->getnativeDouble();

		Metric* diskAvailable = currentResource->EstimatePerformanceResponse(DISK_AVAILABLE,job);
    		double disk = diskAvailable->getnativeDouble();

    		Metric* cpusAvailable = currentResource->EstimatePerformanceResponse(FREE_CPUS,job);
    		double cpus = cpusAvailable->getnativeDouble();

    		Metric* prevSldMetric = currentResource->EstimatePerformanceResponse(AVG_SLD_FINISHED_JOBS,job);
    		double prevSld = prevSldMetric->getnativeDouble();

    		total_slowdown += prevSld;

    		double running = currentResource->getJobsIntheRQ();;


    		/**
       		  for the center we get the static information 
    		*/

		Metric* vendorMetric = currentResource->getStaticSystemInformation(VENDOR);
    		string vendor = vendorMetric->getnativeString();

    		Metric* osNameMetric = currentResource->getStaticSystemInformation(OS_NAME);
    		string osName = osNameMetric->getnativeString();

    		Metric* totalCpusMetric = currentResource->getStaticSystemInformation(TOTAL_CPUS);
    		double totalCpus = totalCpusMetric->getnativeDouble();

    		Metric* clockSpeedMetric = currentResource->getStaticSystemInformation(CLOCK_SPEED);
    		double clockSpeed = clockSpeedMetric->getnativeDouble();

    		Metric* diskSizeMetric = currentResource->getStaticSystemInformation(DISK_SIZE);
    		double diskSize = diskSizeMetric->getnativeDouble();

    		Metric* memorySizeMetric = currentResource->getStaticSystemInformation(MEMORY_SIZE);
    		double memorySize = memorySizeMetric->getnativeDouble();



		/* BALANCED: 
		*/
   		int CSPEED_FACTOR=200;//100
    		int TOTAL_CSPEED_FACTOR=50;//25
    		int FREECPUS_FACTOR=100;
    		int TOTALCPUS_FACTOR=100;

    		int MEMORY_FACTOR=4;

    		int DISKSIZE_FACTOR=1;

    		int RUNNING_JOBS_FACTOR = 40000; //1k
    		int AVG_SLD_FINISHED_JOBS_FACTOR=100000; //10k


    		/**
      		  Process job REQUIREMENTS (job extension)

      		  For VENDOR and OS_NAME we do not need operator (just "=")
      		  They are only used for requirements
    		*/

    		// currentRANK is the RANK for the current resource depending on the job requirements
    		double currentRANK = 0;

    		if(vendorRequired.compare("ANY")){
      		// if it is empty it is not a requirement
      			if(vendor.compare(vendorRequired)){
				// different strings vendors
				currentRANK = -1;
      			}
      			// if not, there is not anything to do
    		} 
   

    		if(currentRANK!=-1){
      			if(osNameRequired.compare("ANY")){
        			if(osName.compare(osNameRequired)){
	  				// different strings vendors
	  				currentRANK = -1;	
        			}
      			}	
    		}

    		/**
      		  The rest of GridJob requirements are double type
    		*/

    		// CPU speed (difference between the total and the required)
    		if(currentRANK!=-1 && cspeedRequired!=-1){
			tmpRANK = getRANK(clockSpeed, cspeedRequired, cspeedOperator, CSPEED_FACTOR);
      			if(tmpRANK != -1){
	        		currentRANK += tmpRANK;
      			}
      			else{
        			//currentRANK = -1;
	      		}
    		}


    		// % Free memory
    		if(currentRANK!=-1 && memoryRequired!=-1){
      			tmpRANK = getRANK(memory, memoryRequired, memoryOperator, 1);
      			if(tmpRANK != -1){
        			currentRANK += MEMORY_FACTOR*tmpRANK/memorySize;
      			}
      			else{
        			//currentRANK = -1;
        			cout<<"Job "<<  job->getJobNumber() << ": Not enough MEMORY in resource:" << center << endl;
      			}
    		}

    		// % Free Disk
    		if(currentRANK!=-1 && diskSizeRequired!=-1){
      			tmpRANK = getRANK(disk, diskSizeRequired, diskSizeOperator, 1);
      			if(tmpRANK != -1){
        			currentRANK += DISKSIZE_FACTOR*tmpRANK/diskSize;
      			}
      			else{
        			//currentRANK = -1;
        			cout<<"Job "<<  job->getJobNumber() << ": Not enough DISK in resource:" << center << endl;
      			}
    		}

    		/**
      		  The regular job (no extension) has to be considered for obtaining the RANK value
    		*/

    		// #FREE CPUS

   		// We determine if there are enough free CPUs if it is not a requirement
    		// cpus is #available CPUs 

    		double jobCpus = gjob->getNumberProcessors();

    		if(currentRANK!=-1 && totalCpusRequired==-1){
      			tmpRANK = getRANK(cpus, jobCpus, HIGHER_EQ_THAN, FREECPUS_FACTOR);
      			if(tmpRANK != -1){
        			//currentRANK += tmpRANK;
				// only to fit requirement...
      			}
      			else{
        			currentRANK = -1;
				cout << "Job" << job->getJobNumber() << ": Not enough CPUS to allocate job in resource:"<<center<<endl;
      			}
    		}

  


    		if(currentRANK!=-1){
      			tmpRANK = getRANK(running, 0, HIGHER_EQ_THAN, 1);
      			if(tmpRANK != -1){
        			currentRANK += RUNNING_JOBS_FACTOR/tmpRANK;
      			}
      			else{
        			//currentRANK = -1;
      			}
    		}

    		// AVG Slowdown of local schedulers

    		if(currentRANK!=-1){
			//cout<<"AVG Slowdown: "<<prevSld<<endl;
			/*
      			tmpRANK = getRANK(prevSld, 0, HIGHER_EQ_THAN, 1);
      			if(tmpRANK != -1){
			*/
        			currentRANK += AVG_SLD_FINISHED_JOBS_FACTOR/tmpRANK;
      			/*
			}
      			else{
        			currentRANK = -1;
      			}
			*/
    		}

    		/** 
      		  Perform the selection based on RANK value 
    		*/

    		//cout<<"Total current RANK value for resource number "<<count<<" is: "<<currentRANK<<endl;

    		if(currentRANK > bestRANK){
      			bestResource = currentResource;
      			bestRANK = currentRANK;
      			choise = count;
			good_choise = count;
    		}

    		count++;


		// FREE MEMORY


    		delete memoryAVMetric;
		delete diskAvailable;
   		delete cpusAvailable;
    		delete prevSldMetric;
		delete vendorMetric;
    		delete osNameMetric;
    		delete totalCpusMetric;
    		delete clockSpeedMetric;
    		delete diskSizeMetric;
    		delete memorySizeMetric;


	}  //end foreach resource
  

	if( bestRANK == -1){

		// Any resource match requirements -> should be rescheduled or submitted to another broker (the following one that matches requirements and that has best rank with aggregated

    		this->waitQueue->insert(job);

		cout<<"LOCAL_QUEUED JOB: "<< job->getJobNumber() << endl;

	}
	else{

		// SUBMIT LOCALLY

    		cout << "LOCAL EXECUTION JobID:" << job->getJobNumber() << endl;

    		//now the sumbission should be decided so.. just submit it !
    		string centerName = this->BRANKPolicyCentersName->operator[] (good_choise);

    		//cout<<"The job "<<itos(job->getJobNumber())<<" has been submitted to the center "<<centerName<<endl;
    		log->debug("The job "+itos(job->getJobNumber())+" has been submitted to the center "+centerName,2);
  
   		bestResource->jobAdd2waitq(job);
  
    		this->submissions.insert(PairSubmission(job,bestResource));

	}
  } // (selected_broker == this)
  else{

	if( selected_broker == NULL ){

		// ANY BROKER MATCH REQUIREMENTS -> QUEUE

    		this->waitQueue->insert(job);

		cout<<"AGGR_QUEUED JOB: "<< job->getJobNumber() << endl;

	}
	else{

		// Forwarding to another broker

  		cout << "FORWARDING: JobID:"<< job->getJobNumber() << " OriginalVO:" << gjob->getVOSubmitted() << " FromVO:" << this->getCenterName() << " ToVO:" << selected_broker->getCenterName() << endl;

   		metaSystem->jobForwarded(job,selected_broker);
   		selected_broker->jobAdd2waitq(job); 
   		//the job has been forwarded, so we just return 
   		return;
	}
  }

  // FREE MEMORY

  //free the best metric 
  delete bestMetric;
  // delete ...
}













/**
 * TO COMMENT...
 */

void BRANKPolicy::jobAdd2waitq_RR(Job* job){
 
  Metric* bestMetric = NULL;
  //SchedulingPolicy* bestPolicy = NULL;
  
  SchedulingPolicy* bestResource = NULL;
  double bestRANK = -1;
  double tmpRANK = -1;  

  int choise = 0; 
  int good_choise = 0;

  int count = 0; //controls the resource number in the loop
  GridJob* gjob = (GridJob*) job;


  // If there is any waiting job I can not submit another one (because of FCFS)
  if(this->waitQueue->getjobs() > 0){

    // If the waiting queue contains the job means that is rescheduled...
    if(!this->waitQueue->contains(job)){ 
      this->waitQueue->insert(job);
      delete bestMetric;
 
      return;
    }
    else{
      this->waitQueue->erase(job);
    }

  }

  /**
      now its time to process the job grid specific fields. 
  */

  Metric* cspeedMetric = gjob->getClockSpeedRequirement()->getmetric();    
  double cspeedRequired = cspeedMetric->getnativeDouble();
  operator_t cspeedOperator = gjob->getClockSpeedRequirement()->getoperatorRequired();

  Metric* vendorMetric = gjob->getVendorRequirement()->getmetric();    
  string vendorRequired = vendorMetric->getnativeString();;
  operator_t vendorOperator = gjob->getVendorRequirement()->getoperatorRequired();

  Metric* osNameMetric = gjob->getOSNameRequirement()->getmetric();    
  string osNameRequired = osNameMetric->getnativeString();;
  operator_t osNameOperator = gjob->getOSNameRequirement()->getoperatorRequired();

  Metric* memoryMetric = gjob->getMemoryRequirement()->getmetric();    
  double memoryRequired = memoryMetric->getnativeDouble();
  operator_t memoryOperator = gjob->getMemoryRequirement()->getoperatorRequired();

  Metric* totalCpusMetric = gjob->getNumberProcessorsRequirement()->getmetric();    
  double totalCpusRequired = totalCpusMetric->getnativeDouble();
  operator_t totalCpusOperator = gjob->getNumberProcessorsRequirement()->getoperatorRequired();

  Metric* diskSizeMetric = gjob->getDiskRequirement()->getmetric();    
  double diskSizeRequired = diskSizeMetric->getnativeDouble();
  operator_t diskSizeOperator = gjob->getDiskRequirement()->getoperatorRequired();


  ///////////////// FACTOR FOR OWN BROKER POTENTIATION

  // AQUI
  double own_broker_factor = 1.10;


  int broker_count = 0;
  int current_broker = -1;
  //int selected_broker = -1;
  BRANKPolicy* selected_broker = NULL;
  double bestBrokerRANK = 0;

  vector<SchedulingPolicy*>* Brokers = this->metaSystem->getVirtualOrganitzations();

  //this->metaSystem->getVONames().size();

  bestRANK = -1;
  tmpRANK = -1;  



  int numVOs = Brokers->size();


  //srand ( time(NULL) );

  //int randomBroker = rand() % numVOs + 1;

  int broker_counter = 1;
  int nextBroker = 0;
  BRANKPolicy* firstBroker = NULL;
  for(vector<SchedulingPolicy*>::iterator it = Brokers->begin(); it != Brokers->end();++it, ++broker_counter)
  {
	BRANKPolicy* broker = (BRANKPolicy*) *it;
	//BRANKPolicy* firstBroker = NULL;

	if(broker_counter == 1){
		firstBroker = broker;
	}

	if(nextBroker){
		selected_broker = broker;
		nextBroker = 0;
	}
	else{
		if(broker == this){
			if(broker_counter == numVOs){
				selected_broker = firstBroker;
			}
			else{
				nextBroker = 1;
			}
		}
	}

  }


  if(selected_broker == this){

  	// To calculate the AVG Slowdown of the broker
  	double total_slowdown = 0;

  	bestRANK = -1;
  	tmpRANK = -1;  
  	choise = 0; 
  	count = 0;


  	/* We iterate to each cluster */
  	for(vector<SchedulingPolicy*>::iterator polit = this->BRANKPolicyPolicies->begin();
      	    polit != this->BRANKPolicyPolicies->end();
      	    ++polit)
	{
    		SchedulingPolicy* currentResource = *polit;

    		string center = this->BRANKPolicyCentersName->operator[] (count);      
    		//cout<<"Analysing resource: "+center <<endl;

    		/**
       		  for the center we get the dynamic information 
    		*/

    		Metric* memoryAVMetric = currentResource->EstimatePerformanceResponse(MEMORY_AVAILABLE,job);
    		double memory = memoryAVMetric->getnativeDouble();

		Metric* diskAvailable = currentResource->EstimatePerformanceResponse(DISK_AVAILABLE,job);
    		double disk = diskAvailable->getnativeDouble();

    		Metric* cpusAvailable = currentResource->EstimatePerformanceResponse(FREE_CPUS,job);
    		double cpus = cpusAvailable->getnativeDouble();

    		Metric* prevSldMetric = currentResource->EstimatePerformanceResponse(AVG_SLD_FINISHED_JOBS,job);
    		double prevSld = prevSldMetric->getnativeDouble();

    		total_slowdown += prevSld;

    		double running = currentResource->getJobsIntheRQ();;


    		/**
       		  for the center we get the static information 
    		*/

		Metric* vendorMetric = currentResource->getStaticSystemInformation(VENDOR);
    		string vendor = vendorMetric->getnativeString();

    		Metric* osNameMetric = currentResource->getStaticSystemInformation(OS_NAME);
    		string osName = osNameMetric->getnativeString();

    		Metric* totalCpusMetric = currentResource->getStaticSystemInformation(TOTAL_CPUS);
    		double totalCpus = totalCpusMetric->getnativeDouble();

    		Metric* clockSpeedMetric = currentResource->getStaticSystemInformation(CLOCK_SPEED);
    		double clockSpeed = clockSpeedMetric->getnativeDouble();

    		Metric* diskSizeMetric = currentResource->getStaticSystemInformation(DISK_SIZE);
    		double diskSize = diskSizeMetric->getnativeDouble();

    		Metric* memorySizeMetric = currentResource->getStaticSystemInformation(MEMORY_SIZE);
    		double memorySize = memorySizeMetric->getnativeDouble();



		/* BALANCED: 
		*/
   		int CSPEED_FACTOR=200;//100
    		int TOTAL_CSPEED_FACTOR=50;//25
    		int FREECPUS_FACTOR=100;
    		int TOTALCPUS_FACTOR=100;

    		int MEMORY_FACTOR=4;

    		int DISKSIZE_FACTOR=1;

    		int RUNNING_JOBS_FACTOR = 40000; //1k
    		int AVG_SLD_FINISHED_JOBS_FACTOR=100000; //10k


    		/**
      		  Process job REQUIREMENTS (job extension)

      		  For VENDOR and OS_NAME we do not need operator (just "=")
      		  They are only used for requirements
    		*/

    		// currentRANK is the RANK for the current resource depending on the job requirements
    		double currentRANK = 0;

    		if(vendorRequired.compare("ANY")){
      		// if it is empty it is not a requirement
      			if(vendor.compare(vendorRequired)){
				// different strings vendors
				currentRANK = -1;
      			}
      			// if not, there is not anything to do
    		} 
   

    		if(currentRANK!=-1){
      			if(osNameRequired.compare("ANY")){
        			if(osName.compare(osNameRequired)){
	  				// different strings vendors
	  				currentRANK = -1;	
        			}
      			}	
    		}

    		/**
      		  The rest of GridJob requirements are double type
    		*/

    		// CPU speed (difference between the total and the required)
    		if(currentRANK!=-1 && cspeedRequired!=-1){
			tmpRANK = getRANK(clockSpeed, cspeedRequired, cspeedOperator, CSPEED_FACTOR);
      			if(tmpRANK != -1){
	        		currentRANK += tmpRANK;
      			}
      			else{
        			//currentRANK = -1;
	      		}
    		}


    		// % Free memory
    		if(currentRANK!=-1 && memoryRequired!=-1){
      			tmpRANK = getRANK(memory, memoryRequired, memoryOperator, 1);
      			if(tmpRANK != -1){
        			currentRANK += MEMORY_FACTOR*tmpRANK/memorySize;
      			}
      			else{
        			//currentRANK = -1;
        			cout<<"Job "<<  job->getJobNumber() << ": Not enough MEMORY in resource:" << center << endl;
      			}
    		}

    		// % Free Disk
    		if(currentRANK!=-1 && diskSizeRequired!=-1){
      			tmpRANK = getRANK(disk, diskSizeRequired, diskSizeOperator, 1);
      			if(tmpRANK != -1){
        			currentRANK += DISKSIZE_FACTOR*tmpRANK/diskSize;
      			}
      			else{
        			//currentRANK = -1;
        			cout<<"Job "<<  job->getJobNumber() << ": Not enough DISK in resource:" << center << endl;
      			}
    		}

    		/**
      		  The regular job (no extension) has to be considered for obtaining the RANK value
    		*/

    		// #FREE CPUS

   		// We determine if there are enough free CPUs if it is not a requirement
    		// cpus is #available CPUs 

    		double jobCpus = gjob->getNumberProcessors();

    		if(currentRANK!=-1 && totalCpusRequired==-1){
      			tmpRANK = getRANK(cpus, jobCpus, HIGHER_EQ_THAN, FREECPUS_FACTOR);
      			if(tmpRANK != -1){
        			//currentRANK += tmpRANK;
				// only to fit requirement...
      			}
      			else{
        			currentRANK = -1;
				cout << "Job" << job->getJobNumber() << ": Not enough CPUS to allocate job in resource:"<<center<<endl;
      			}
    		}

  


    		if(currentRANK!=-1){
      			tmpRANK = getRANK(running, 0, HIGHER_EQ_THAN, 1);
      			if(tmpRANK != -1){
        			currentRANK += RUNNING_JOBS_FACTOR/tmpRANK;
      			}
      			else{
        			//currentRANK = -1;
      			}
    		}

    		// AVG Slowdown of local schedulers

    		if(currentRANK!=-1){
			//cout<<"AVG Slowdown: "<<prevSld<<endl;
			/*
      			tmpRANK = getRANK(prevSld, 0, HIGHER_EQ_THAN, 1);
      			if(tmpRANK != -1){
			*/
        			currentRANK += AVG_SLD_FINISHED_JOBS_FACTOR/tmpRANK;
      			/*
			}
      			else{
        			currentRANK = -1;
      			}
			*/
    		}

    		/** 
      		  Perform the selection based on RANK value 
    		*/

    		//cout<<"Total current RANK value for resource number "<<count<<" is: "<<currentRANK<<endl;

    		if(currentRANK > bestRANK){
      			bestResource = currentResource;
      			bestRANK = currentRANK;
      			choise = count;
			good_choise = count;
    		}

    		count++;


		// FREE MEMORY


    		delete memoryAVMetric;
		delete diskAvailable;
   		delete cpusAvailable;
    		delete prevSldMetric;
		delete vendorMetric;
    		delete osNameMetric;
    		delete totalCpusMetric;
    		delete clockSpeedMetric;
    		delete diskSizeMetric;
    		delete memorySizeMetric;


	}  //end foreach resource
  

	if( bestRANK == -1){

		// Any resource match requirements -> should be rescheduled or submitted to another broker (the following one that matches requirements and that has best rank with aggregated

    		this->waitQueue->insert(job);

		cout<<"LOCAL_QUEUED JOB: "<< job->getJobNumber() << endl;

	}
	else{

		// SUBMIT LOCALLY

    		cout << "LOCAL EXECUTION JobID:" << job->getJobNumber() << endl;

    		//now the sumbission should be decided so.. just submit it !
    		string centerName = this->BRANKPolicyCentersName->operator[] (good_choise);

    		//cout<<"The job "<<itos(job->getJobNumber())<<" has been submitted to the center "<<centerName<<endl;
    		log->debug("The job "+itos(job->getJobNumber())+" has been submitted to the center "+centerName,2);
  
   		bestResource->jobAdd2waitq(job);
  
    		this->submissions.insert(PairSubmission(job,bestResource));

	}
  } // (selected_broker == this)
  else{

	if( selected_broker == NULL ){

		// ANY BROKER MATCH REQUIREMENTS -> QUEUE

    		this->waitQueue->insert(job);

		cout<<"AGGR_QUEUED JOB: "<< job->getJobNumber() << endl;

	}
	else{

		// Forwarding to another broker

  		cout << "FORWARDING: JobID:"<< job->getJobNumber() << " OriginalVO:" << gjob->getVOSubmitted() << " FromVO:" << this->getCenterName() << " ToVO:" << selected_broker->getCenterName() << endl;

   		metaSystem->jobForwarded(job,selected_broker);
   		selected_broker->jobAdd2waitq(job); 
   		//the job has been forwarded, so we just return 
   		return;
	}
  }

  // FREE MEMORY

  //free the best metric 
  delete bestMetric;
  // delete ...
}














/**
 * Auxiliar function. To comment...
 */

double BRANKPolicy::getJobRank(Job* job){

  // compute rank with CPUs, MEM, DISK with factors

  int cpu_factor = 1;
  int mem_factor = 0;
  int disk_factor = 0;

  double myrank = 0;

  GridJob* gjob = (GridJob*) job;  

  Metric* totalCpusMetric = gjob->getNumberProcessorsRequirement()->getmetric();    
  double totalCpusRequired = totalCpusMetric->getnativeDouble();

  Metric* memoryMetric = gjob->getMemoryRequirement()->getmetric();    
  double memoryRequired = memoryMetric->getnativeDouble();

  Metric* diskSizeMetric = gjob->getDiskRequirement()->getmetric();    
  double diskSizeRequired = diskSizeMetric->getnativeDouble();

  myrank += totalCpusRequired * cpu_factor;
  myrank += memoryRequired * memoryRequired;
  myrank += diskSizeRequired * disk_factor;

  //delete totalCpusMetric;
  //delete memoryMetric;
  //delete diskSizeMetric;

  return myrank;

}


/**
 * Auxiliar function. To comment...
 */
ResourceAggregatedInfo* BRANKPolicy::findAggregatedResource(AggregateMetabrokeringInfo* info, string resourceName){

   vector<ResourceAggregatedInfo*>* ress = info->getResourcesInfo();

   for(vector<ResourceAggregatedInfo*>::iterator it2 = ress->begin() ; it2!=ress->end(); it2++){
	ResourceAggregatedInfo* rai = (ResourceAggregatedInfo*) *it2;

	if( rai->getResourceName() == resourceName ){
		return rai;
	}
   }

   return NULL;

}



/**
 * This class returns the aggregated information for the resources and relationships
 * @return A reference to the AggregateMetabrokeringInfo class that contains the agregated information, the reference must be deleted by the invoquer!
 */
AggregateMetabrokeringInfo* BRANKPolicy::getAggregateMetabrokeringInfo()
{

   return getAggregateMetabrokeringInfoSimple();

   //return getAggregateMetabrokeringInfoCategorized();

}



/**
 * This class returns the aggregated information for the resources and relationships, SIMPLE implementation
 * @return A reference to the AggregateMetabrokeringInfo class that contains the agregated information, the reference must be deleted by the invoquer!
 */
AggregateMetabrokeringInfo* BRANKPolicy::getAggregateMetabrokeringInfoSimple()
{

  //typedef map<int, OperatingSystemInfo*> osData;

  typedef vector<string> oss;

  long count = 0; // count the number of resources (for example clusters)

  map<string, ComputingSystemInfo*> CSs;
  map<string, OperatingSystemInfo*> OSs;

  map<string, oss*> Rels;
  map<string, string> OStoAggr;

  long totalDisk_min = -1;
  long totalDisk_max = 0;
  long totalDisk_sum = 0;

  long freeDisk_min = -1;
  long freeDisk_max = 0;
  long freeDisk_sum = 0;

  int num_FS = 0;
 
  double broker_slowdown = 0;  // To calculate the AVG Slowdown of the broker

  // Collect data (query all the resources!)


  /* We iterate to each cluster */
  for(vector<SchedulingPolicy*>::iterator polit = this->BRANKPolicyPolicies->begin();
      polit != this->BRANKPolicyPolicies->end();
      ++polit)
  {
    SchedulingPolicy* currentResource = *polit;

    string center = this->BRANKPolicyCentersName->operator[] (count);      
    //cout<<"Resource: "+center <<endl;

    /**
       for the center we get the dynamic information 
    */
    Metric* memoryAVMetric = currentResource->EstimatePerformanceResponse(MEMORY_AVAILABLE, NULL);
    double memory = memoryAVMetric->getnativeDouble();

    Metric* diskAvailable = currentResource->EstimatePerformanceResponse(DISK_AVAILABLE, NULL);
    double disk = diskAvailable->getnativeDouble();

    Metric* cpusAvailable = currentResource->EstimatePerformanceResponse(FREE_CPUS, NULL);
    double cpus = cpusAvailable->getnativeDouble();

    Metric* prevSldMetric = currentResource->EstimatePerformanceResponse(AVG_SLD_FINISHED_JOBS, NULL);
    double prevSld = prevSldMetric->getnativeDouble();

    broker_slowdown += prevSld;
  
    /**
       for the center we get the static information 
    */
    Metric* vendorMetric = currentResource->getStaticSystemInformation(VENDOR);
    string vendor = vendorMetric->getnativeString();

    Metric* osNameMetric = currentResource->getStaticSystemInformation(OS_NAME);
    string osName = osNameMetric->getnativeString();

    Metric* totalCpusMetric = currentResource->getStaticSystemInformation(TOTAL_CPUS);
    double totalCpus = totalCpusMetric->getnativeDouble();

    Metric* clockSpeedMetric = currentResource->getStaticSystemInformation(CLOCK_SPEED);
    double clockSpeed = clockSpeedMetric->getnativeDouble();

    Metric* diskSizeMetric = currentResource->getStaticSystemInformation(DISK_SIZE);
    double diskSize = diskSizeMetric->getnativeDouble();

    Metric* memorySizeMetric = currentResource->getStaticSystemInformation(MEMORY_SIZE);
    double memorySize = memorySizeMetric->getnativeDouble();

    count++;

    // ComputingSystem

    map<string, ComputingSystemInfo*>::iterator CS = CSs.find(vendor);
    if( CS == CSs.end()){
      ComputingSystemInfo* myCS = new ComputingSystemInfo();
      CSs.insert(pair<string, ComputingSystemInfo*> (vendor, myCS));
      //delete myCS;
    }

    CSs[vendor]->setProcType(vendor);
    CSs[vendor]->setNum(CSs[vendor]->getNum()+1);

    //numProc

    if( (long)totalCpus < CSs[vendor]->getNumProc_min() || CSs[vendor]->getNumProc_min() == -1 ){
      CSs[vendor]->setNumProc_min((long)totalCpus);
    }
    if( (long)totalCpus > CSs[vendor]->getNumProc_max() ){
      CSs[vendor]->setNumProc_max((long)totalCpus);
    }
    CSs[vendor]->setNumProc_sum(CSs[vendor]->getNumProc_sum() + (long)totalCpus);

    //cpuSpeed

    if( (long)clockSpeed < CSs[vendor]->getCpuSpeed_min() || CSs[vendor]->getCpuSpeed_min() == -1 ){
      CSs[vendor]->setCpuSpeed_min((long)clockSpeed);
    }
    if( (long)clockSpeed > CSs[vendor]->getCpuSpeed_max() ){
      CSs[vendor]->setCpuSpeed_max((long)clockSpeed);
    }
    CSs[vendor]->setCpuSpeed_sum(CSs[vendor]->getCpuSpeed_sum() + (long)clockSpeed);

    //cpuUtil = usedCPUs * 100

    long usedCPUs = (long) (totalCpus - cpus) * 100;

    if( usedCPUs < CSs[vendor]->getCpuUtil_min() || CSs[vendor]->getCpuUtil_min() == -1 ){
      CSs[vendor]->setCpuUtil_min(usedCPUs);
    }
    if( usedCPUs > CSs[vendor]->getCpuUtil_max() ){
      CSs[vendor]->setCpuUtil_max(usedCPUs);
    }
    CSs[vendor]->setCpuUtil_sum(CSs[vendor]->getCpuUtil_sum() + usedCPUs);


    // For Relationships CS -> OS (vendor -> osName)

    map<string, oss*>::iterator rel = Rels.find(vendor);
    if( rel == Rels.end() ){
      oss* myRel = new oss();
      Rels.insert(pair<string, oss*> (vendor, myRel));
    }

    oss* currentRel = Rels[vendor];
    int found = 0;
    for(vector<string>::iterator it = currentRel->begin();it != currentRel->end() && !found;++it)
    {
      string myos = (string) *it;

      if(myos == osName ){
        found = 1;
      }
    }

    if(!found){
      currentRel->push_back(osName);
    }


   // OperatingSystem

    map<string,OperatingSystemInfo*>::iterator OS = OSs.find(osName);
    if( OS == OSs.end()){
      OperatingSystemInfo* myOS = new OperatingSystemInfo();
      OSs.insert(pair<string, OperatingSystemInfo*> (osName, myOS));
    }

    OSs[osName]->setOsType(osName);
    OSs[osName]->setNum(OSs[osName]->getNum()+1);

    //totalMem

    if( (long)memorySize < OSs[osName]->getTotalMem_min() || OSs[osName]->getTotalMem_min() == -1 ){
      OSs[osName]->setTotalMem_min((long)memorySize);
    }
    if( (long)memorySize > OSs[osName]->getTotalMem_max() ){
      OSs[osName]->setTotalMem_max((long)memorySize);
    }
    OSs[osName]->setTotalMem_sum(OSs[osName]->getTotalMem_sum() + (long)memorySize);

    //freeMem

    if( (long)memory < OSs[osName]->getFreeMem_min() || OSs[osName]->getFreeMem_min() == -1){
      OSs[osName]->setFreeMem_min((long)memory);
    }
    if( (long)memory > OSs[osName]->getFreeMem_max() ){
      OSs[osName]->setFreeMem_max((long)memory);
    }
    OSs[osName]->setFreeMem_sum(OSs[osName]->getFreeMem_sum() + (long)memory);


   // FileSystem

    if( (long)diskSize < totalDisk_min || totalDisk_min == -1 ){
      totalDisk_min = (long)diskSize;
    }
    if( (long)diskSize > totalDisk_max ){
     totalDisk_max = (long)diskSize;
    }
    totalDisk_sum += (long) diskSize;

    if( (long)disk < freeDisk_min || freeDisk_min == -1){
      freeDisk_min = (long)disk;
    }
    if( (long)disk > freeDisk_max ){
     freeDisk_max = (long)disk;
    }
    freeDisk_sum += (long) disk;

    num_FS++;
/*
cout<<"cpus_min: "<<CSs[vendor]->getNumProc_min()<<endl;
cout<<"cpus_max: "<<CSs[vendor]->getNumProc_max()<<endl;
cout<<"cpus_sum: "<<CSs[vendor]->getNumProc_sum()<<endl;
*/


  // FREE MEMORY


    delete memoryAVMetric;
    delete diskAvailable;
    delete cpusAvailable;
    delete prevSldMetric;
    delete vendorMetric;
    delete osNameMetric;
    delete totalCpusMetric;
    delete clockSpeedMetric;
    delete diskSizeMetric;
    delete memorySizeMetric;



  }  //end foreach resource
  

  // Computes Average Slowdown of the resources (for aggregated resources)
  if(count>0){
    broker_slowdown = broker_slowdown / count;
  }

  // generate aggregated resources

  AggregateMetabrokeringInfo* info = new AggregateMetabrokeringInfo();

  vector<ResourceAggregatedInfo*>* aggregated_resources = new vector<ResourceAggregatedInfo*>;
  vector<RelationshipAggregatedInfo*>* aggregated_relationships = new vector<RelationshipAggregatedInfo*>;


  // First aggregate OS and FS to define relationships when aggregating CS

  // For each OS Type (OSName)
  int OS_num = 0;

  for(map<string, OperatingSystemInfo*>::iterator it = OSs.begin(); it != OSs.end(); ++it, ++OS_num)
  {
    
    string current_ostype = (*it).first; //key value
    OperatingSystemInfo* currentOS = (*it).second;

    ResourceAggregatedInfo* aggregated_info = new ResourceAggregatedInfo();

    //Attributes:
    vector<Metric*>* attrs = new vector<Metric*>;

    //////////////OperatingSystemType
    Metric *attr = new Metric();
    
    ostringstream os;
    os << "OperatingSystemType={(" << current_ostype << ",<count=" << currentOS->getNum() << ">)}";
    string mystring = os.str();
    //cout<<"mystring: "<<mystring<<endl;
    attr->setNativeString(mystring);
    attrs->push_back(attr);


    ///////////// TotalPhysicalMemory
    attr = new Metric();
    os.str( "" );//reset ostringstream
    os << "TotalPhysicalMemory={(";
    //if( currentOS->getTotalMem_min() == currentOS->getTotalMem_max()){
    //  os << currentOS->getTotalMem_min();
    //}
    //else{
      os << currentOS->getTotalMem_min() << "-" << currentOS->getTotalMem_max();
    //}
    os << ",<count=" << currentOS->getNum() << ">,<total=" << currentOS->getTotalMem_sum() << ">)}";
    mystring = os.str();
    //cout<<"mystring: "<<mystring<<endl;
    attr->setNativeString(os.str());
    attrs->push_back(attr);

    ///////////// FreePhysicalMemory
    attr = new Metric();
    os.str( "" );//reset ostringstream
    os << "FreePhysicalMemory={(";
    //if( currentOS->getFreeMem_min() == currentOS->getFreeMem_max()){
    //  os << currentOS->getFreeMem_min();
    //}
    //else{
      os << currentOS->getFreeMem_min() << "-" << currentOS->getFreeMem_max();
    //}
    os << ",<count=" << currentOS->getNum() << ">,<total=" << currentOS->getFreeMem_sum() << ">)}";
    mystring = os.str();
    //cout<<"mystring: "<<mystring<<endl;
    attr->setNativeString(os.str());
    attrs->push_back(attr);


    // Put all aggregated resource info

    aggregated_info->setResourceType("OperatingSystem");
    os.str( "" );//reset ostringstream
    os << "Aggregated_OSR_" << OS_num;
    aggregated_info->setResourceName(os.str());
    aggregated_info->setAttributes(attrs);

    aggregated_resources->push_back(aggregated_info);

    // having a map<string, string> : current_ostype, aggregated_info->getResourceName()
    // for defining relationships later

    OStoAggr.insert(pair<string, string> (current_ostype, aggregated_info->getResourceName()));

  }


  // FS is trivial (only one)

    ResourceAggregatedInfo* aggregated_info = new ResourceAggregatedInfo();

    //Attributes:
    vector<Metric*>* attrs = new vector<Metric*>;

    //////////////TotalStorageCapacity
    Metric *attr = new Metric();
    ostringstream os;
    attr = new Metric();
    os << "TotalStorageCapacity={(";
    //if( totalDisk_min == totalDisk_max){
    //  os << totalDisk_min;
    //}
    //else{
      os << totalDisk_min << "-" << totalDisk_max;
    //}
    os << ",<count=" << num_FS << ">,<total=" << totalDisk_sum << ">)}";
    string mystring = os.str();
    //cout<<"mystring: "<<mystring<<endl;
    attr->setNativeString(os.str());
    attrs->push_back(attr);


    //////////////FreeStorageCapacity
    attr = new Metric();
    os.str( "" );//reset ostringstream
    attr = new Metric();
    os << "FreeStorageCapacity={(";
    //if( freeDisk_min == freeDisk_max){
    //  os << freeDisk_min;
    //}
    //else{
      os << freeDisk_min << "-" << freeDisk_max;
    //}
    os << ",<count=" << num_FS << ">,<total=" << freeDisk_sum << ">)}";
    mystring = os.str();
    //cout<<"mystring: "<<mystring<<endl;
    attr->setNativeString(os.str());
    attrs->push_back(attr);



    // Put all aggregated resource info

    aggregated_info->setResourceType("FileSystem");
    os.str( "" );//reset ostringstream
    os << "Aggregated_FSR_0";
    aggregated_info->setResourceName(os.str());
    aggregated_info->setAttributes(attrs);

    aggregated_resources->push_back(aggregated_info);


  // For each CPU Type (vendor)
  int CS_num = 0;

  for(map<string, ComputingSystemInfo*>::iterator it = CSs.begin(); it != CSs.end(); ++it, ++CS_num)
  {
    
    string current_proctype = (*it).first; //key value
    ComputingSystemInfo* currentCS = (*it).second;

    ResourceAggregatedInfo* aggregated_info = new ResourceAggregatedInfo();

    //Attributes:
    vector<Metric*>* attrs = new vector<Metric*>;

    //////////////ProcessorType
    Metric *attr = new Metric();
    
    ostringstream os;
    //os << "ProcessorType={(" << currentCS->getProcType() << ",<count=" << currentCS->getNum() << ">)}";
    os << "ProcessorType={(" << current_proctype << ",<count=" << currentCS->getNum() << ">)}";
    string mystring = os.str();
    //cout<<"mystring: "<<mystring<<endl;
    attr->setNativeString(mystring);
    attrs->push_back(attr);


    /////////// NumOfProcessors
    attr = new Metric();
    os.str( "" );//reset ostringstream
    os << "NumOfProcessors={(";
    //if( currentCS->getNumProc_min() == currentCS->getNumProc_max()){
    //  os << currentCS->getNumProc_min();
    //}
    //else{
      os << currentCS->getNumProc_min() << "-" << currentCS->getNumProc_max();
    //}
    os << ",<count=" << currentCS->getNum() << ">,<total=" << currentCS->getNumProc_sum() << ">)}";
    mystring = os.str();
    //cout<<"mystring: "<<mystring<<endl;
    attr->setNativeString(os.str());
    attrs->push_back(attr);

    ///////////// ProcessingSpeed
    attr = new Metric();
    os.str( "" );//reset ostringstream
    os << "ProcessingSpeed={(";
    //if( currentCS->getCpuSpeed_min() == currentCS->getCpuSpeed_max()){
    //  os << currentCS->getCpuSpeed_min();
    //}
    //else{
      os << currentCS->getCpuSpeed_min() << "-" << currentCS->getCpuSpeed_max();
    //}
    os << ",<count=" << currentCS->getNum() << ">,<total=" << currentCS->getCpuSpeed_sum() << ">)}";
    mystring = os.str();
    //cout<<"mystring: "<<mystring<<endl;
    attr->setNativeString(os.str());
    attrs->push_back(attr);


    ///////////// CPUUtilization
    attr = new Metric();
    os.str( "" );//reset ostringstream
    os << "CPUUtilization={(";
    //if( currentCS->getCpuUtil_min() == currentCS->getCpuUtil_max()){
    //  os << currentCS->getCpuUtil_min();
    //}
    //else{
      os << currentCS->getCpuUtil_min() << "-" << currentCS->getCpuUtil_max();
    //}
    os << ",<count=" << currentCS->getNum() << ">,<total=" << currentCS->getCpuUtil_sum() << ">)}";
    //os << ",<count=" << currentCS->getNum() << ">)}";
    mystring = os.str();
    //cout<<"mystring: "<<mystring<<endl;
    attr->setNativeString(os.str());
    attrs->push_back(attr);


    // Put all aggregated resource info

    aggregated_info->setResourceType("ComputingSystem");
    os.str( "" );//reset ostringstream
    os << "Aggregated_CSR_" << CS_num;
    aggregated_info->setResourceName(os.str());
    aggregated_info->setAttributes(attrs);

    aggregated_resources->push_back(aggregated_info);

    // generate relationships with aggregated 

    // With each Aggregated OS

    oss* currentRel = Rels[current_proctype];
    for(vector<string>::iterator it = currentRel->begin();it != currentRel->end();++it)
    {
      string myos = (string) *it;
      string myAggrOS = OStoAggr[myos];

      // Define relationship

      RelationshipAggregatedInfo* aggregated_rel = new RelationshipAggregatedInfo();

      aggregated_rel->setSourceType("ComputingSystem");
      aggregated_rel->setSourceName(aggregated_info->getResourceName());
      aggregated_rel->setTargetType("OperatingSystem");
      aggregated_rel->setTargetName(myAggrOS);

      aggregated_relationships->push_back(aggregated_rel);

    }

    // With the unique FS

    RelationshipAggregatedInfo* aggregated_rel = new RelationshipAggregatedInfo();
    aggregated_rel->setSourceType("ComputingSystem");
    aggregated_rel->setSourceName(aggregated_info->getResourceName());
    aggregated_rel->setTargetType("FileSystem");
    aggregated_rel->setTargetName("Aggregated_FSR_0");

    aggregated_relationships->push_back(aggregated_rel);

  }


  info->setResourcesInfo(aggregated_resources);
  info->setRelationshipsInfo(aggregated_relationships);


  // FREE MEM
  // CSs,OSs,Rels

  for(map<string, ComputingSystemInfo*>::iterator it = CSs.begin(); it != CSs.end(); ++it)
  {
    
    ComputingSystemInfo* currentCS = (*it).second;
    delete currentCS;
   }

  for(map<string, OperatingSystemInfo*>::iterator it = OSs.begin(); it != OSs.end(); ++it)
  {
    
    OperatingSystemInfo* currentOS = (*it).second;
    delete currentOS;
   }

  for(map<string, oss*>::iterator it = Rels.begin(); it != Rels.end(); ++it)
  {
    
    oss* currentRel = (*it).second;
    delete currentRel;
   }



  return info;

}


/**
 * This class returns the aggregated information for the resources and relationships, CATEGORIZED implementation
 * @return A reference to the AggregateMetabrokeringInfo class that contains the agregated information, the reference must be deleted by the invoquer!
 */
AggregateMetabrokeringInfo* BRANKPolicy::getAggregateMetabrokeringInfoCategorized()
{

   AggregateMetabrokeringInfo* info = new AggregateMetabrokeringInfo();


   ResourceAggregatedInfo* rai = new ResourceAggregatedInfo();   


   vector<Metric*>* attrs = new vector<Metric*>;

   Metric *attr = new Metric();
   attr->setNativeString("ProcessorType={(Intel,<count=16>)}");

   attrs->push_back(attr);

   rai->setResourceType("ComputingSystem");
   rai->setResourceName("Aggregated_CSR_0");
   rai->setAttributes(attrs);

   
   vector<ResourceAggregatedInfo*>* ress = new vector<ResourceAggregatedInfo*>;

   ress->push_back(rai);

   info->setResourcesInfo(ress);

   return info;

}




/**
 * Moves a job from the wait queue to the reservation table, using an specified set of resources
 * @see The class SchedulingPolicy  
 * @param job The job that has started
 */
void BRANKPolicy::jobWaitq2RT(Job* job)
{
  //we simply proxy the query of removing the job from the wait queue and add it to the Run table 
  map<Job*,SchedulingPolicy*>::iterator subit = this->submissions.find(job);
  
  assert(subit != this->submissions.end()); //the job should be already submitted !
  
  SchedulingPolicy* policy = subit->second;
  
  log->debug( " The job "+itos(job->getJobNumber())+" has to be moved to the run table.",2);
  
  policy->jobWaitq2RT(job); 
  //this->waitQueue->erase(job);
  this->RunningQueue.insert(job);

  // When a job finished we have to try to schedule the waiting jobs (that it was not possible to schedule them)
/*
  if(this->waitQueue->getjobs() >0){
    Job *myjob = this->waitQueue->headJob();
    this->jobAdd2waitq(myjob);
  }
*/

}



// When a Job finishes this method is invoked

/**
 * Moves a job from teh wait queue to the reservation table, using an specified set of resources
 * @see The class SchedulingPolicy  
 * @param job The job that has started
 */
void BRANKPolicy::jobRemoveFromRT(Job* job)
{
  //We call to the base class - important it has to update some info   
  SchedulingPolicy::jobRemoveFromRT(job);

  //we simply proxy the query of removing the job from the wait queue and add it to the Run table 
  map<Job*,SchedulingPolicy*>::iterator subit = this->submissions.find(job);
  
  assert(subit != this->submissions.end()); //the job should be already submitted !
  
  SchedulingPolicy* policy = subit->second;
  
  log->debug( " The job "+itos(job->getJobNumber())+" has to be moved to the run table.",2);
  
  policy->jobRemoveFromRT(job); 
  
  //time to update the predictor service in case if it is being used in the simulation model
  //We will estimate the job runtime if it was set or not 
  if(this->UsePredictionService)
  {      
   
    this->predictionService->jobTerminated(job, this->globaltime,job->getJobSimStatus() == COMPLETED);
    this->predictionService->estimateJobPerformance(job);
  }

  this->RunningQueue.erase(job);


  // When a job finished we have to try to schedule the waiting jobs (that it was not possible to schedule them)
  if(this->waitQueue->getjobs() >0){
    Job *myjob = this->waitQueue->headJob();
    this->jobAdd2waitq(myjob);
  }

}


/**
 * Chooses the more appropiate job to run .
 * @see The class SchedulingPolicy  
 * @return The appropiate job to run 
 */
Job* BRANKPolicy::jobChoose()
{
  //for this version this remains empty, however it will be used for implemeent more sophisticated scheduling decisions 
}

/**
 * Function that is called once the job should start 
 * @see The class SchedulingPolicy  
 * @param job The job that to start 
 */
void BRANKPolicy::jobStart(Job* job)
{
   //we simply proxy the query of removing the job from the wait queue and add it to the Run table 
  map<Job*,SchedulingPolicy*>::iterator subit = this->submissions.find(job);
  
  assert(subit != this->submissions.end()); //the job should be already submitted !
  
  SchedulingPolicy* policy = subit->second;
  
  log->debug( " The job "+itos(job->getJobNumber())+" has to start.",2);
  
  policy->jobStart(job); 
  
  //we update the hashmap of the submitted jobs 
  map<SchedulingPolicy*,int>::iterator subjobsit = this->SubmittedJobs.find(policy);
  
  //  ULL   assert(subjobsit != this->SubmittedJobs.end());
  
  //  ULL   this->SubmittedJobs[policy] = this->SubmittedJobs[policy]+1;

}

/**
 * Returns the number of jobs currently allocated in the waitQueue
 * @return An integer containing the number of jobs in the WQ
 */
double BRANKPolicy::getJobsIntheWQ()
{
  return this->waitQueue->getjobs();
}

/**
 * Returns the number of jobs that are currently backfilled 
 * @return An integer containing the number of backfilled jobs 
 */
double BRANKPolicy::getBackfilledJobs()
{
  double backfilledJobs = 0;

  for(vector<SchedulingPolicy*>::iterator polit = this->BRANKPolicyPolicies->begin();
      polit != this->BRANKPolicyPolicies->begin();
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
int  BRANKPolicy::getNumberCPUSUsed()
{
   int used = 0;
   
   for(vector<SchedulingPolicy*>::iterator polit = this->BRANKPolicyPolicies->begin();
      polit != this->BRANKPolicyPolicies->begin();
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
void BRANKPolicy::setSim(Simulation* theValue)
{  
  //we have to iterate over all the policies and set the simulation reference
  for(vector<SchedulingPolicy*>::iterator polit = this->BRANKPolicyPolicies->begin();
      polit != this->BRANKPolicyPolicies->end();
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
void BRANKPolicy::setGlobalTime(double globaltime)
{
  this->log->debug("Setting the global time of the schdeuling policy and reservationTable to "+ftos(globaltime),2);
  this->globaltime = globaltime;
  
  /* we set also the global time for the reservationTable */
  this->reservationTable->setGlobaltime(globaltime);
  
  //we have to iterate over all the policies and set the simulation reference
  for(vector<SchedulingPolicy*>::iterator polit = this->BRANKPolicyPolicies->begin();
      polit != this->BRANKPolicyPolicies->end();
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
 * Returns the of the BRANKPolicyArchitectures BRANKPolicy policie
 * @return The BRANKPolicyArchitectures used by the policies
 */
vector< ArchitectureConfiguration * >* BRANKPolicy::getBRANKPolicyArchitectures() const
{
  return BRANKPolicyArchitectures;
}  


/**
 * Function that sets BRANKPolicyArchitectures the of the BRANKPolicy policie
 * @param theValue The BRANKPolicyArchitectures
 */
void BRANKPolicy::setBRANKPolicyArchitectures(vector< ArchitectureConfiguration * >* theValue)
{
  BRANKPolicyArchitectures = theValue;
}

/**
 * Returns the BRANKPolicyParaverTracesName of the BRANKPolicy policie
 * @return The  BRANKPolicyParaverTracesName
 */
vector< string >* BRANKPolicy::getBRANKPolicyParaverTracesName() const
{
  return BRANKPolicyParaverTracesName;
}

/**
 * Function that sets the BRANKPolicyParaverTracesName of the BRANKPolicy policie
 * @param theValue The BRANKPolicyParaverTracesName
 */
void BRANKPolicy::setBRANKPolicyParaverTracesName(vector< string >* theValue)
{
  BRANKPolicyParaverTracesName = theValue;
}

/**
 * Returns the BRANKPolicyPolicies of the BRANKPolicy policie
 * @return The BRANKPolicyPolicies
 */

vector< SchedulingPolicy * >* BRANKPolicy::getBRANKPolicyPolicies() const
{
  return BRANKPolicyPolicies;
}

/**
 * Function that sets the BRANKPolicyPolicies of the BRANKPolicy policie
 * @param theValue The BRANKPolicyPolicies
 */
void BRANKPolicy::setBRANKPolicyPolicies(vector< SchedulingPolicy * >* theValue)
{
  BRANKPolicyPolicies = theValue;
  
  //we initialize the number of submitted jobs to zero
  //we also initialize the reservationTable 
  int pos = 0;
  for(vector<SchedulingPolicy*>::iterator polit = this->BRANKPolicyPolicies->begin();
      polit != this->BRANKPolicyPolicies->end();
      ++polit)
  {
    SchedulingPolicy* pol = *polit;    
    this->SubmittedJobs.insert(PairSubmitedJobs(pol,0));    
    
    string center = this->BRANKPolicyCentersName->operator[](pos);    
    ((GridAnalogicalReservationTable*)this->reservationTable)->addReservationTable(center,pol->getreservationTable());
    pos++;
    

  }  
  
}

/**
 * Return the the amount of pending work (area of requested time x requested processors for the queued jobs)
 * @return A integer containing the pending work
 */
double BRANKPolicy::getLeftWork()
{
  return 0;
}
  
/**
* Return the number of jobs in the run queue 
* @return A integer containing the number of running jobs 
*/
double BRANKPolicy::getJobsIntheRQ()
{
  return 0;
}


/**
 * Returns the metric that contains the estimation of which would be the performance of a given job if it would be allocated now. Now this is not implemented , but would be in the future
 * @param MetricTpye The metric that indicates which performance variable has to be computed 
 * @param job The job that will be checked
 * @return The metric containing the performance that the given job would experiment if it would be submitted right now
 */
 
Metric* BRANKPolicy::EstimatePerformanceResponse(metric_t MetricTpye,Job* job)
{
  return NULL;
}

/**
 * Returns the BRANKPolicyCentersName of the BRANKPolicy policie
 * @return The BRANKPolicyCentersName
 */
vector< string >* BRANKPolicy::getBRANKPolicyCentersName() const
{
  return BRANKPolicyCentersName;
}

/**
 * Function that sets the BRANKPolicyCentersName of the BRANKPolicy policie
 * @param theValue The BRANKPolicyCentersName
 */
void BRANKPolicy::setBRANKPolicyCentersName(vector< string >* theValue)
{
  BRANKPolicyCentersName = theValue;
}

/**
 * Returns the BRANKPolicyEmulateCPUFactor of the BRANKPolicy policie
 * @return The BRANKPolicyEmulateCPUFactor
 */
vector< double >* BRANKPolicy::getBRANKPolicyEmulateCPUFactor() const
{
  return BRANKPolicyEmulateCPUFactor;
}

/**
 * Function that sets the of the BRANKPolicy policie
 * @param theValue The BRANKPolicyEmulateCPUFactor
 */
void BRANKPolicy::setBRANKPolicyEmulateCPUFactor(vector< double >* theValue)
{
  BRANKPolicyEmulateCPUFactor = theValue;
}

/**
 * Returns the system that is being modeled in the simulation
 * @return A reference to the MetaBrokering system 
 */
MetaBrokeringSystem* BRANKPolicy::getmetaSystem() const
{
	return metaSystem;
}

/**
 * Function that sets the of the metaSystem that is instantiated in the simulation
 * @param theValue The BRANKPolicyEmulateCPUFactor
 */
void BRANKPolicy::setMetaSystem ( MetaBrokeringSystem* theValue )
{
	metaSystem = theValue;
}


}


/////////////// Xapusa!!!


ComputingSystemInfo::ComputingSystemInfo(){

  this->procType = "";

  this->num = 0;

  this->numProc_min = -1;
  this->numProc_max = 0;
  this->numProc_sum = 0;

  this->cpuSpeed_min = -1;
  this->cpuSpeed_max = 0;
  this->cpuSpeed_sum = 0;

  this->cpuUtil_min = -1;
  this->cpuUtil_max = 0;
  this->cpuUtil_sum = 0;

}

ComputingSystemInfo::~ComputingSystemInfo(){

}

void ComputingSystemInfo::setProcType(string thevalue){

  procType = thevalue;	

}

string ComputingSystemInfo::getProcType(void){

  return procType;

}

void ComputingSystemInfo::setNum(long thevalue){

  num = thevalue;	

}

long ComputingSystemInfo::getNum(void){

  return num;

}

void ComputingSystemInfo::setNumProc_min(long thevalue){

  numProc_min = thevalue;	

}

long ComputingSystemInfo::getNumProc_min(void){

  return numProc_min;

}

void ComputingSystemInfo::setNumProc_max(long thevalue){

  numProc_max = thevalue;	

}

long ComputingSystemInfo::getNumProc_max(void){

  return numProc_max;

}

void ComputingSystemInfo::setNumProc_sum(long thevalue){

  numProc_sum = thevalue;	

}

long ComputingSystemInfo::getNumProc_sum(void){

  return numProc_sum;

}

void ComputingSystemInfo::setCpuSpeed_min(long thevalue){

  cpuSpeed_min = thevalue;	

}

long ComputingSystemInfo::getCpuSpeed_min(void){

  return cpuSpeed_min;

}

void ComputingSystemInfo::setCpuSpeed_max(long thevalue){

  cpuSpeed_max = thevalue;	

}

long ComputingSystemInfo::getCpuSpeed_max(void){

  return cpuSpeed_max;

}

void ComputingSystemInfo::setCpuSpeed_sum(long thevalue){

  cpuSpeed_sum = thevalue;	

}

long ComputingSystemInfo::getCpuSpeed_sum(void){

  return cpuSpeed_sum;

}

void ComputingSystemInfo::setCpuUtil_min(long thevalue){

  cpuUtil_min = thevalue;	

}

long ComputingSystemInfo::getCpuUtil_min(void){

  return cpuUtil_min;

}

void ComputingSystemInfo::setCpuUtil_max(long thevalue){

  cpuUtil_max = thevalue;	

}

long ComputingSystemInfo::getCpuUtil_max(void){

  return cpuUtil_max;

}

void ComputingSystemInfo::setCpuUtil_sum(long thevalue){

  cpuUtil_sum = thevalue;	

}

long ComputingSystemInfo::getCpuUtil_sum(void){

  return cpuUtil_sum;

}




////////////////


OperatingSystemInfo::OperatingSystemInfo(){

  this->osType = "";

  this->num = 0;

  this->totalMem_min = -1;
  this->totalMem_max = 0;
  this->totalMem_sum = 0;

  this->freeMem_min = -1;
  this->freeMem_max = 0;
  this->freeMem_sum = 0;

}

OperatingSystemInfo::~OperatingSystemInfo(){

}

void OperatingSystemInfo::setOsType(string thevalue){

  osType = thevalue;	

}

string OperatingSystemInfo::getOsType(void){

  return osType;

}

void OperatingSystemInfo::setNum(long thevalue){

  num = thevalue;	

}

long OperatingSystemInfo::getNum(void){

  return num;

}

void OperatingSystemInfo::setTotalMem_min(long thevalue){

  totalMem_min = thevalue;	

}

long OperatingSystemInfo::getTotalMem_min(void){

  return totalMem_min;

}

void OperatingSystemInfo::setTotalMem_max(long thevalue){

  totalMem_max = thevalue;	

}

long OperatingSystemInfo::getTotalMem_max(void){

  return totalMem_max;

}

void OperatingSystemInfo::setTotalMem_sum(long thevalue){

  totalMem_sum = thevalue;	

}

long OperatingSystemInfo::getTotalMem_sum(void){

  return totalMem_sum;

}

void OperatingSystemInfo::setFreeMem_min(long thevalue){

  freeMem_min = thevalue;	

}

long OperatingSystemInfo::getFreeMem_min(void){

  return freeMem_min;

}

void OperatingSystemInfo::setFreeMem_max(long thevalue){

  freeMem_max = thevalue;	

}

long OperatingSystemInfo::getFreeMem_max(void){

  return freeMem_max;

}

void OperatingSystemInfo::setFreeMem_sum(long thevalue){

  freeMem_sum = thevalue;	

}

long OperatingSystemInfo::getFreeMem_sum(void){

  return freeMem_sum;

}

////////////////



FileSystemInfo::FileSystemInfo(){

  this->TotalStorageCapacity_min = 0;
  this->TotalStorageCapacity_max = 0;
  this->TotalStorageCapacity_sum = 0;
  
  this->FreeStorageCapacity_min = 0;
  this->FreeStorageCapacity_max = 0;
  this->FreeStorageCapacity_sum = 0;

}

FileSystemInfo::~FileSystemInfo(){

}

int FileSystemInfo::getNum() 
{
	return Num;
}


void FileSystemInfo::setNum ( int theValue )
{
	Num = theValue;
}

long FileSystemInfo::getTotalStorageCapacity_min() 
{
	return TotalStorageCapacity_min;
}


void FileSystemInfo::setTotalStorageCapacity_min ( long theValue )
{
	TotalStorageCapacity_min = theValue;
}


long FileSystemInfo::getTotalStorageCapacity_max() 
{
	return TotalStorageCapacity_max;
}


void FileSystemInfo::setTotalStorageCapacity_max ( long theValue )
{
	TotalStorageCapacity_max = theValue;
}


long FileSystemInfo::getTotalStorageCapacity_sum() 
{
	return TotalStorageCapacity_sum;
}


void FileSystemInfo::setTotalStorageCapacity_sum ( long theValue )
{
	TotalStorageCapacity_sum = theValue;
}


long FileSystemInfo::getFreeStorageCapacity_min()
{
	return FreeStorageCapacity_min;
}


void FileSystemInfo::setFreeStorageCapacity_min ( long theValue )
{
	FreeStorageCapacity_min = theValue;
}


long FileSystemInfo::getFreeStorageCapacity_max() 
{
	return FreeStorageCapacity_max;
}


void FileSystemInfo::setFreeStorageCapacity_max ( long theValue )
{
	FreeStorageCapacity_max = theValue;
}


long FileSystemInfo::getFreeStorageCapacity_sum() 
{
	return FreeStorageCapacity_sum;
}


void FileSystemInfo::setFreeStorageCapacity_sum ( long theValue )
{
	FreeStorageCapacity_sum = theValue;
}




///////////////////////


completeAggregated::completeAggregated(){

  this->CS = new ComputingSystemInfo();
  this->OS = new OperatingSystemInfo();
  this->FS = new FileSystemInfo();

}


completeAggregated::~completeAggregated(){

  delete this->CS;
  delete this->OS;
  delete this->FS;

}

ComputingSystemInfo* Simulator::completeAggregated::getCS() 
{
	return CS;
}


void Simulator::completeAggregated::setCS ( ComputingSystemInfo* theValue )
{
	CS = theValue;
}


OperatingSystemInfo* Simulator::completeAggregated::getOS() 
{
	return OS;
}


void Simulator::completeAggregated::setOS ( OperatingSystemInfo* theValue )
{
	OS = theValue;
}


FileSystemInfo* Simulator::completeAggregated::getFS()
{
	return FS;
}


void Simulator::completeAggregated::setFS ( FileSystemInfo* theValue )
{
	FS = theValue;
}


void Simulator::completeAggregated::insertCS(ResourceAggregatedInfo* cs){

	ComputingSystemInfo* myCS = this->getCS();
	
	for(vector<Metric*>::iterator it3 = cs->getAttributes()->begin() ; it3!=cs->getAttributes()->end(); it3++){
		Metric* my_metric = (Metric*) *it3;
		string attr = (string)my_metric->getnativeString();
		//split attribute

		deque<string> fields;
		SplitLine(attr, "=",fields);
		//fields.pop_front();
		string type = (string) fields.front();

		if( type == "ProcessorType"){
			fields.pop_front();
			deque<string> fields2;
			string rest = (string) fields.front();
			string rest2(rest, 2, rest.size());
			SplitLine(rest2, ",", fields2);
			string theType = (string) fields2.front();

			myCS->setProcType(theType);
			myCS->setNum(extractCount(attr));
		}
		if( type == "NumOfProcessors"){
			myCS->setNumProc_min(extractMin(attr));
			myCS->setNumProc_max(extractMax(attr));
			myCS->setNumProc_sum(extractTotal(attr));
		}
		if( type == "ProcessingSpeed"){
			myCS->setCpuSpeed_min(extractMin(attr));
			myCS->setCpuSpeed_max(extractMax(attr));
			myCS->setCpuSpeed_sum(extractTotal(attr));
		}
		if( type == "CPUUtilization"){
			myCS->setCpuUtil_min(extractMin(attr));
			myCS->setCpuUtil_max(extractMax(attr));
			myCS->setCpuUtil_sum(extractTotal(attr));
		}

	}

}


void Simulator::completeAggregated::insertOS(ResourceAggregatedInfo* os){

	OperatingSystemInfo* myOS = this->getOS();
	
	for(vector<Metric*>::iterator it3 = os->getAttributes()->begin() ; it3!=os->getAttributes()->end(); it3++){
		Metric* my_metric = (Metric*) *it3;
		string attr = (string)my_metric->getnativeString();
		//split attribute

		deque<string> fields;
		SplitLine(attr, "=",fields);
		//fields.pop_front();
		string type = (string) fields.front();

		if( type == "OperatingSystemType"){
			fields.pop_front();
			deque<string> fields2;
			string rest = (string) fields.front();
			string rest2(rest, 2, rest.size());
			SplitLine(rest2, ",", fields2);
			string theType = (string) fields2.front();

			myOS->setOsType(theType);
			myOS->setNum(extractCount(attr));
		}
		if( type == "TotalPhysicalMemory"){
			myOS->setTotalMem_min(extractMin(attr));
			myOS->setTotalMem_max(extractMax(attr));
			myOS->setTotalMem_sum(extractTotal(attr));
		}
		if( type == "FreePhysicalMemory"){
			myOS->setFreeMem_min(extractMin(attr));
			myOS->setFreeMem_max(extractMax(attr));
			myOS->setFreeMem_sum(extractTotal(attr));
		}
	}

}


void Simulator::completeAggregated::insertFS(ResourceAggregatedInfo* fs){

	FileSystemInfo* myFS = this->getFS();
	
	for(vector<Metric*>::iterator it3 = fs->getAttributes()->begin() ; it3!=fs->getAttributes()->end(); it3++){
		Metric* my_metric = (Metric*) *it3;
		string attr = (string)my_metric->getnativeString();
		//split attribute

		deque<string> fields;
		SplitLine(attr, "=",fields);
		//fields.pop_front();
		string type = (string) fields.front();

		if( type == "TotalStorageCapacity"){
			myFS->setNum(extractCount(attr));
			myFS->setTotalStorageCapacity_min(extractMin(attr));
			myFS->setTotalStorageCapacity_max(extractMax(attr));
			myFS->setTotalStorageCapacity_sum(extractTotal(attr));
		}
		if( type == "FreeStorageCapacity"){
			myFS->setFreeStorageCapacity_min(extractMin(attr));
			myFS->setFreeStorageCapacity_max(extractMax(attr));
			myFS->setFreeStorageCapacity_sum(extractTotal(attr));
		}
	}

}


long Simulator::completeAggregated::extractMin(string orig){

	deque<string> fields;
	SplitLine(orig, "{(",fields);
	fields.pop_front();
	string rest = (string) fields.front();

	deque<string> fields2;
	SplitLine(rest, "-", fields2);
	string value = (string) fields2.front();

	long theValue = atol((char *)value.c_str());

	return theValue;

}


long Simulator::completeAggregated::extractMax(string orig){


	deque<string> fields;
	SplitLine(orig, "{(",fields);
	fields.pop_front();
	string rest = (string) fields.front();

	deque<string> fields2;
	SplitLine(rest, ",", fields2);
	string rest2 = (string) fields2.front();
	deque<string> fields3;
	SplitLine(rest2, "-", fields3);
	fields3.pop_front();
	string value = (string) fields3.front();

	long theValue = atol((char *)value.c_str());

	return theValue;

}


long Simulator::completeAggregated::extractCount(string orig){

	deque<string> fields;
	SplitLine(orig, "<count=",fields);
	fields.pop_front();
	string rest = (string) fields.front();

	deque<string> fields2;
	SplitLine(rest, ">", fields2);
	string value = (string) fields2.front();

	long theValue = atol((char *)value.c_str());

	return theValue;

}


long Simulator::completeAggregated::extractTotal(string orig){


	deque<string> fields;
	SplitLine(orig, "<total=",fields);
	fields.pop_front();
	string rest = (string) fields.front();

	deque<string> fields2;
	SplitLine(rest, ">", fields2);
	string value = (string) fields2.front();

	long theValue = atol((char *)value.c_str());

	return theValue;

}



reducedAggregated::reducedAggregated(){

  this->CS = "";
  this->OS = "";
  this->FS = "";

}

string Simulator::reducedAggregated::getCS()
{
  return CS;
}


void Simulator::reducedAggregated::setCS(string theValue)
{
  CS = theValue;
}


string Simulator::reducedAggregated::getOS() 
{
  return OS;
}


void Simulator::reducedAggregated::setOS(string theValue)
{
  OS = theValue;
}


string Simulator::reducedAggregated::getFS()
{
  return FS;
}


void Simulator::reducedAggregated::setFS(string theValue)
{
  FS = theValue;
}


// Returns the subvector of reducedAggregated resources that matches with the CS string value

vector <reducedAggregated*>* BRANKPolicy::getSubvector(vector <reducedAggregated*>* data, string theValue){

	vector <reducedAggregated *>* newInfo = new vector <reducedAggregated *>;

	for(vector<reducedAggregated*>::iterator it = data->begin(); it != data->end(); ++it)
  	{
    		reducedAggregated* reduced = (reducedAggregated*) *it;

		if( reduced->getCS() == theValue ){
			newInfo->push_back(reduced);
		}
  	}

	return newInfo;
}
