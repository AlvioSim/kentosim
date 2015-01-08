#include <utils/cvsjobsinfoconverter.h>

namespace Utils {

/**
 * The default constructor for the class
 * @param FilePath The file path where to store the cvs file 
 * @param log The logging engine 
 */
CSVJobsInfoConverter::CSVJobsInfoConverter(string FilePath,Log* log) : CSVConverter(FilePath,log)
{

  this->type = CSV_JOBS_INFO_CONVERTER;

  this->CSVHeaderFields = new vector<string>;
  
  this->CSVHeaderFields->push_back("JobId");
  this->CSVHeaderFields->push_back("RequestedTime");  
  this->CSVHeaderFields->push_back("RuntimeTime");  
  this->CSVHeaderFields->push_back("UsedProcessors");  
  this->CSVHeaderFields->push_back("JobSimSubmitTime");
  this->CSVHeaderFields->push_back("JobSimStartTime");
  this->CSVHeaderFields->push_back("JobSimFinishTime");
  this->CSVHeaderFields->push_back("JobSimWaitTime");
  this->CSVHeaderFields->push_back("JobSimEstimateFinishTime");
  this->CSVHeaderFields->push_back("JobSimisKilled");
  this->CSVHeaderFields->push_back("JobSimisBackfilled");
  this->CSVHeaderFields->push_back("JobSimBackfillingTime");
  this->CSVHeaderFields->push_back("JobSimBSLD");
  this->CSVHeaderFields->push_back("JobSimSLD");
  this->CSVHeaderFields->push_back("JobSimCompleted");
  this->CSVHeaderFields->push_back("PenalizedRunTime");
  this->CSVHeaderFields->push_back("JobSimCenter");
  this->CSVHeaderFields->push_back("RSPolicy");  
  this->CSVHeaderFields->push_back("Status");  
  this->CSVHeaderFields->push_back("MonetaryCost");  
  this->CSVHeaderFields->push_back("PredictedMonetaryCost");  
  this->CSVHeaderFields->push_back("WaitTimePrediction");  
  this->CSVHeaderFields->push_back("RuntimePrediction");  
  this->CSVHeaderFields->push_back("SldPredicted");  
  this->CSVHeaderFields->push_back("WaitTimePredictionError");  
  this->CSVHeaderFields->push_back("RuntimePredictionError");  
  this->CSVHeaderFields->push_back("SldPredictedError");  
  this->CSVHeaderFields->push_back("PredictedMonetaryCostError");  
  

  
  this->fieldTypes = new vector<file_field_t>;
  
  this->fieldTypes->push_back(DOUBLE_FIELD);
  this->fieldTypes->push_back(DOUBLE_FIELD);
  this->fieldTypes->push_back(DOUBLE_FIELD);
  this->fieldTypes->push_back(DOUBLE_FIELD);
  this->fieldTypes->push_back(DOUBLE_FIELD);
  this->fieldTypes->push_back(DOUBLE_FIELD);
  this->fieldTypes->push_back(DOUBLE_FIELD);
  this->fieldTypes->push_back(DOUBLE_FIELD);
  this->fieldTypes->push_back(DOUBLE_FIELD);
  this->fieldTypes->push_back(DOUBLE_FIELD);
  this->fieldTypes->push_back(DOUBLE_FIELD);
  this->fieldTypes->push_back(DOUBLE_FIELD);
  this->fieldTypes->push_back(DOUBLE_FIELD);
  this->fieldTypes->push_back(DOUBLE_FIELD);
  this->fieldTypes->push_back(DOUBLE_FIELD);
  this->fieldTypes->push_back(DOUBLE_FIELD);
  this->fieldTypes->push_back(STRING_FIELD);
  this->fieldTypes->push_back(STRING_FIELD);
  this->fieldTypes->push_back(STRING_FIELD);
  this->fieldTypes->push_back(DOUBLE_FIELD);
  this->fieldTypes->push_back(DOUBLE_FIELD);
  this->fieldTypes->push_back(DOUBLE_FIELD);
  this->fieldTypes->push_back(DOUBLE_FIELD);
  this->fieldTypes->push_back(DOUBLE_FIELD);
  this->fieldTypes->push_back(DOUBLE_FIELD);
  this->fieldTypes->push_back(DOUBLE_FIELD);
  this->fieldTypes->push_back(DOUBLE_FIELD);
  this->fieldTypes->push_back(DOUBLE_FIELD);


  this->policy = policy;
}

/**
 * The default destructor for the class 
 */
CSVJobsInfoConverter::~CSVJobsInfoConverter()
{
}

/**
 * Given the sets of jobs create the cvs output file
 * @param JobList The job list that have to be stored to the file,
 */
void CSVJobsInfoConverter::createJobsCSVInfoFile(map<int,Job*>* JobList)
{
  this->open();

  for(map<int,Job*>::iterator itj = JobList->begin();itj != JobList->end();++itj)
  {
    Job* job = itj->second;
    this->addEntry(job);
  }

  this->close();
}


/**
 * Adds a new entry to the cvs file 
 * @param job The job that will be stored in the file 
 */
void CSVJobsInfoConverter::addEntry(Job* job)
{
  vector<string>* entry = new vector<string>;
 
 entry->push_back(ftos(job->getJobNumber()));
 entry->push_back(ftos(job->getRequestedTime()));
 entry->push_back(ftos(job->getRunTime()));
 entry->push_back(ftos(job->getNumberProcessors()));
 entry->push_back(ftos(job->getJobSimSubmitTime()));
 entry->push_back(ftos(job->getJobSimStartTime()));
 entry->push_back(ftos(job->getJobSimFinishTime()));
 entry->push_back(ftos(job->getJobSimWaitTime()));
 entry->push_back(ftos(job->getJobSimEstimateFinishTime()));
 entry->push_back(ftos(job->getJobSimisKilled()));
 entry->push_back(ftos(job->getJobSimisBackfilled()));
 entry->push_back(ftos(job->getJobSimBackfillingTime()));
 entry->push_back(ftos(job->getJobSimBSLD()));
 entry->push_back(ftos(job->getJobSimSLD()));
 entry->push_back(ftos(job->getJobSimCompleted()));
 entry->push_back(ftos(job->getPenalizedRunTime()));
 entry->push_back(job->getJobSimCenter());

 switch(job->getallocatedWith())
 {
   case FIRST_FIT:
     entry->push_back(string("FIRST_FIT"));  
     break;
   case FIRST_CONTINOUS_FIT:
     entry->push_back(string("FIRST_CONTINOUS_FIT"));  
     break;
   case FF_AND_FCF:
     entry->push_back(string("FF_AND_FCF"));  
     break;     
   case FIND_LESS_CONSUME:
     entry->push_back(string("FIND_LESS_CONSUME"));  
     break;          
   case EQUI_DISTRIBUTE_CONSUME:
     entry->push_back(string("EQUI_DISTRIBUTE_CONSUME"));  
     break;        
   case FIND_LESS_CONSUME_THRESSHOLD:
     entry->push_back(string("FIND_LESS_CONSUME_THRESSHOLD"));  
     break;
   case DC_FIRST_VM:
     entry->push_back(string("DC_FIRST_VM"));
     break;             
   default:
     entry->push_back(string("OTHER"));      
     break;     
      
 }
 
 switch(job->getJobSimStatus())
 {
   case COMPLETED:
     entry->push_back(string("COMPLETED"));  
     break;
   case KILLED_BACKFILLING:
     entry->push_back(string("KILLED_BACKFILLING"));  
     break;
   case KILLED_QUEUE_LIMIT:
     entry->push_back(string("KILLED_QUEUE_LIMIT"));  
     break;     
   case RUNNING:
     entry->push_back(string("RUNNING")); //can occur when stopping the simulation with a signal
     break;          
   case LOCAL_QUEUED:
     entry->push_back(string("LOCAL_QUEUED")); //can occur when stopping the simulation with a signal
     break;          
   case GRID_QUEUED:
     entry->push_back(string("GRID_QUEUED")); //can occur when stopping the simulation with a signal
     break;
   case FAILED:
     entry->push_back(string("FAILED"));  
     break;        
   case CANCELLED:
     entry->push_back(string("CANCELLED"));  
     break;
   case KILLED_NOT_ENOUGH_RESOURCES:
     entry->push_back(string("KILLED_NOT_ENOUGH_RESOURCES"));  
     break;
   default:
     entry->push_back(string("OTHER"));    
     break;     
 }

 entry->push_back(ftos((job->getMonetaryCost()))); 
 entry->push_back(ftos(job->getPredictedMonetaryCost())); 
 entry->push_back(ftos((job->getWaitTimePrediction()))); 
 entry->push_back(ftos(job->getRuntimePrediction()));
 entry->push_back(ftos((job->getRuntimePrediction()+job->getWaitTimePrediction())/job->getRuntimePrediction()));


 if(job->getJobSimWaitTime() != 0) 
   entry->push_back(ftos((double)(((job->getJobSimWaitTime()-job->getWaitTimePrediction())/job->getJobSimWaitTime())*100)));
 else
   entry->push_back("0");

 entry->push_back(ftos((double)(((job->getRunTime()-job->getRuntimePrediction())/job->getRunTime())*100)));

 if(job->getMonetaryCost() != 0)
   entry->push_back(ftos((double)(((job->getMonetaryCost()-job->getPredictedMonetaryCost())/job->getMonetaryCost())*100)));      
 else 
   entry->push_back("0");

 double sld_predicted = (job->getRuntimePrediction()+job->getWaitTimePrediction())/job->getRuntimePrediction();
 entry->push_back(ftos((double)(((job->getJobSimSLD()-sld_predicted)/job->getJobSimSLD())*100)));


 CSVConverter::addEntry(entry);
 
}


/**
 * Returns the policy used by the converter 
 * @return A reference to the policy 
 */
SchedulingPolicy* CSVJobsInfoConverter::getpolicy() const
{
  return policy;
}


/**
 * Sets the reference to the policy used by the converter 
 * @param theValue The policy
 */
void CSVJobsInfoConverter::setPolicy(SchedulingPolicy* theValue)
{
  policy = theValue;
}

}

