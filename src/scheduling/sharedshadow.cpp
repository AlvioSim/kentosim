#include <scheduling/sharedshadow.h>
#include <iostream>


namespace Simulator {

/**
 * The default constructor for the class
 */
SharedShadow::SharedShadow(double startime, double endtime,int numnode)
{
  this->startShadow = startime;
  this->endShadow = endtime;
  this->numnode = numnode;
  this->totalBuckets = 0;
  this->numberJobs = 0;
  
  //setting to zero the requirements for 
  this->ethShReq = 0;
  this->nbwShReq = 0;
  this->mbwShReq = 0;
}

/**
 * The default destructor for the class 
 */
SharedShadow::~SharedShadow()
{
  for(map<int,vector<AnalogicalBucket*>*>::iterator it = this->buckets.begin(); it != this->buckets.end();++it)
  {
    vector<AnalogicalBucket*>* vectDel = it->second;
    delete vectDel;
  }
}

/**
 * Returns the mbwShReq
 * @return A double containing the mbwShReq
 */
double SharedShadow::getendShadow() const
{
  return endShadow;
}


/**
 * The operator "=" has been overloaded. 
 * @param source The source sharedshadow 
 */
SharedShadow& SharedShadow::operator=(const SharedShadow& source)
{  

 
  //we must copy all the vectors ! otherwise the application will crash on their delete 
  for(JobBucketsMapping::const_iterator it = source.buckets.begin();it != source.buckets.end();++it)
  {
    vector<AnalogicalBucket*>* orig = it->second;
    
    vector<AnalogicalBucket*>* dest = new vector<AnalogicalBucket*>();
    *dest = *orig;
    
    this->buckets.insert(pairJobBuckets(it->first,dest));
   
  }
  
  setNumberJobs(source.getnumberJobs());
  setTotalBuckets(source.gettotalBuckets());
  setMbwShReq(source.getmbwShReq());
  setNbwShReq(source.getnbwShReq());
  setEthShReq(source.getethShReq());

}

/**
 * Sets the endShadow to the SharedShadow
 * @param theValue The endShadow
 */
void SharedShadow::setEndShadow(double theValue)
{
  endShadow = theValue;
}


/**
 * Returns the startShadow
 * @return A double containing the startShadow
 */
double SharedShadow::getstartShadow() const
{
  return startShadow;
}

/**
 * Sets the startShadow to the SharedShadow
 * @param theValue The startShadow
 */
void SharedShadow::setStartShadow(double theValue)
{
  startShadow = theValue;
}

/**
 * Function that adds a new bucket to the shared shadow 
 * @param bucket The bucket to be added 
 */
void SharedShadow::insert(AnalogicalBucket* bucket)
{
  Job* job = bucket->getjob();
  //first we try to find if the job has been allocated for the shadow 
  JobBucketsMapping::iterator it = this->buckets.find(job->getJobNumber());
  
  if(it != this->buckets.end())
  {
    //the vector has been initalized so we just add the bucket
    vector<AnalogicalBucket*>* ShadowJobs =  it->second;
    //we just add it
    ShadowJobs->push_back(bucket);
    
  }
  else
  {
    vector<AnalogicalBucket*>* ShadowJobs = new vector<AnalogicalBucket*>();
    ShadowJobs->push_back(bucket);
    this->buckets.insert(pairJobBuckets(job->getJobNumber(),ShadowJobs));
    this->numberJobs++;
  } 
  
      //we update the demand of resources for the current shadow 
  this->mbwShReq+= job->getBWMemoryUsed();
  this->nbwShReq+= job->getBWNetworkUsed();
  this->ethShReq+= job->getBWEthernedUsed();    
  
  totalBuckets++;
}

/**
 * Returns the id
 * @return A integer containing the id
 */
int SharedShadow::getId()
{
  return this->id;
}

/**
 * Sets the id to the SharedShadow
 * @param theValue The id
 */
void SharedShadow::setId(int id)
{
  this->id = id;
}

/**
 * Returns the number of processes 
 * @return A integer containing the number of processes 
 */
int SharedShadow::getNumberProcesses()
{
  return totalBuckets; 
}

/**
 * Sets the NumberProcesses to the SharedShadow
 * @param theValue The NumberProcesses
 */
int SharedShadow::getJobNumberProcesses(int jobid)
{
  JobBucketsMapping::iterator it = this->buckets.find(jobid);
  
  if(it != this->buckets.end())
  {
    //the vector has been initalized so we just add the bucket
    vector<AnalogicalBucket*>* ShadowJobs =  it->second;
    
    return ShadowJobs->size();
  }
  else return 0;
}

/**
 * Returns the numberJobs
 * @return A integer containing the numberJobs
 */
int SharedShadow::getnumberJobs() const
{
  return numberJobs;
}

/**
 * Sets the numberJobs to the SharedShadow
 * @param theValue The numberJobs
 */
void SharedShadow::setNumberJobs(const int& theValue)
{
  numberJobs = theValue;
}


/**
 * Returns the totalBuckets 
 * @return A integer containing the totalBuckets
 */
int SharedShadow::gettotalBuckets() const
{
  return totalBuckets;
}

/**
 * Sets the totalBuckets to the SharedShadow
 * @param theValue The totalBuckets
 */
void SharedShadow::setTotalBuckets(const int& theValue)
{
  totalBuckets = theValue;
}

/**
 * Returns the mbwShReq
 * @return A double containing the mbwShReq
 */
double SharedShadow::getmbwShReq() const
{
  return mbwShReq;
}

/**
 * Sets the totalBuckets to the SharedShadow
 * @param theValue The totalBuckets
 */
void SharedShadow::setMbwShReq(double theValue)
{
  mbwShReq = theValue;
}


/**
 * Returns the nbwShReq
 * @return A double containing the nbwShReq
 */
double SharedShadow::getnbwShReq() const
{
  return nbwShReq;
}

/**
 * Sets the nbwShReq to the SharedShadow
 * @param theValue The nbwShReq
 */
void SharedShadow::setNbwShReq(double theValue)
{
  nbwShReq = theValue;
}


/**
 * Returns the ethShReq
 * @return A double containing the ethShReq
 */
double SharedShadow::getethShReq() const
{
  return ethShReq;
}

/**
 * Sets the ethShReq to the SharedShadow
 * @param theValue The ethShReq
 */
void SharedShadow::setEthShReq(double theValue)
{
  ethShReq = theValue;
}

}
