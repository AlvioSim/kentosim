#include <utils/csvpolicyinfoconverter.h>

namespace Utils {

/**
 * The default constructor for the class
 * @param FilePath The file path where to store the cvs file 
 * @param log The logging engine 
 */
CSVPolicyInfoConverter::CSVPolicyInfoConverter(string FilePath,Log* log) : CSVConverter(FilePath,log)
{

  this->type = CSV_JOBS_INFO_CONVERTER;

  this->CSVHeaderFields = new vector<string>;
  
  this->CSVHeaderFields->push_back("TimeStamp");
  this->CSVHeaderFields->push_back("NumberJobsInTheQueue");
  this->CSVHeaderFields->push_back("NumberCPUSUsed");  
  this->CSVHeaderFields->push_back("BackfilledJobs");  
  this->CSVHeaderFields->push_back("PendingWorks");  
  this->CSVHeaderFields->push_back("RunningJobs");
  this->CSVHeaderFields->push_back("Center");
  

  
  this->fieldTypes = new vector<file_field_t>;
  
  this->fieldTypes->push_back(DOUBLE_FIELD);
  this->fieldTypes->push_back(DOUBLE_FIELD);
  this->fieldTypes->push_back(DOUBLE_FIELD);
  this->fieldTypes->push_back(DOUBLE_FIELD);
  this->fieldTypes->push_back(DOUBLE_FIELD);
  this->fieldTypes->push_back(DOUBLE_FIELD);
  this->fieldTypes->push_back(STRING_FIELD);

}

/**
 * The default destructor for the class 
 */
CSVPolicyInfoConverter::~CSVPolicyInfoConverter()
{
}

/**
 * Given the set of sampling for the center create the cvs output file 
 * @param collector The policy entity collector
 */
void CSVPolicyInfoConverter::createPolicyCSVInfoFile(PolicyEntityCollector* collector)
{
  this->open();
  
  int numEntries = collector->getnumberJobsInTheQueue()->size();
  
  for(int i = 0; i < numEntries; i++)
  {
     vector<string>* entry = new vector<string>;
     
     string stamp = ftos(collector->gettimeStamp()->operator[](i));
     string JobsWQ = ftos(collector->getnumberJobsInTheQueue()->operator[](i));
     string CPUSUsed = ftos(collector->getnumberCPUSUsed()->operator[](i));
     string bkJobs = ftos(collector->getbackfilledJobs()->operator[](i));
     string lfW = ftos(collector->getleftWork()->operator[](i));
     string rjobs = ftos(collector->getrunningJobs()->operator[](i));
     string center = collector->getcenterName()->operator[](i);
                         
     entry->push_back(stamp);
     entry->push_back(JobsWQ);
     entry->push_back(CPUSUsed);
     entry->push_back(bkJobs);
     entry->push_back(lfW);    
     entry->push_back(rjobs);
     entry->push_back(center);
     
     CSVConverter::addEntry(entry);
  }
  
  this->close();
}




}
