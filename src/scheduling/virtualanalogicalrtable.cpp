#include <scheduling/virtualanalogicalrtable.h>
#include <utils/utilities.h>
#include <fstream>
#include <assert.h>
#include <time.h>
#include <math.h>
#include <statistics/average.h>

#include <algorithm>
#include <iterator>

using namespace Statistics;

namespace Simulator {

/**
 * The default constructor for the class
 */
VirtualAnalogicalRTable::VirtualAnalogicalRTable()
 : AnalogicalResertavionTable ()
{
  this->dumpNumber = 0;
}

/**
 * @param configuration A reference to the configuration concerning the architecture
 * @param log A reference to the logging engine 
 * @param globaltime The globaltime of the simulation
 */
VirtualAnalogicalRTable::VirtualAnalogicalRTable(ArchitectureConfiguration* configuration,Log* log, double globaltime)
 : AnalogicalResertavionTable (configuration,log,globaltime)
{
  this->dumpNumber = 0;
  this->lastShadowId = 0;
  map<int,Job*>* JobList = NULL;
  this->allBuckets = new BucketSetOrdered();
  this->metricType = WAITTIME;
  this->statisticType = AVG;
  this->FreeBucketsOnJobCompletion = true; //by default the buckets are freed 
  this->MinSTimeNoFinishedJob = globaltime;
  ProcessJobTempAllocations = NULL;
  this->ProcessJobTempAllocations = new map<int,Job*>();
}

/**
 * The default destructor for the class 
 */
VirtualAnalogicalRTable::~VirtualAnalogicalRTable()
{
  delete this->ProcessJobTempAllocations;
  delete this->allBuckets;
}

/**
 * This function frees all the processes (that are modeled as one sequential jobs that are assoicated to allocations to the current table)
 */
void VirtualAnalogicalRTable::freeProcessesAllocations()
{
  for(map<int,Job*>::iterator it = ProcessJobTempAllocations->begin();it != ProcessJobTempAllocations->end();++it)
  {    
    Job* tempProc = it->second;    
    this->deAllocateJob(tempProc);
    delete tempProc;
  }

  ProcessJobTempAllocations->clear();
    
}

/**
 * This function insert a job to the temporal process list to the virtual reservation table. Each job inserted in this list emulates a process of a given job, 
 * It is used for evaluating the impact of allocating a given processes of a given job to a given processors. 
 * @param job The process to be added (emulated like a sequential job)
 */
void VirtualAnalogicalRTable::insertTemporalProcess(Job * job)
{
  this->ProcessJobTempAllocations->insert(pair<int,Job*>(job->getJobNumber(),job));
  
  map<int,Job*>::iterator procit =  this->ProcessJobTempAllocations->find(job->getJobNumber());
  
}

/**
 * This function delete a job to the temporal process list to the virtual reservation table. 
 * @see See also the function insertTemporalProcess.
 * @param job The process to be added (emulated like a sequential job)
 * @param allocated Indicates that it has been allocated 
 */

void VirtualAnalogicalRTable::freeTemporalProcess(Job* job,bool allocated = true)
{
  map<int,Job*>::iterator it = this->ProcessJobTempAllocations->find(job->getJobNumber());
  assert(it != ProcessJobTempAllocations->end());

  if(allocated) deAllocateJob(job);  

  this->ProcessJobTempAllocations->erase(it);
  delete job;
}

/**
 * The operator of asignament has ben overloaded due to when an assignament from one virtualanalogicalrtable to another virtualanalogicalrtable is done, the objects referenced  inside the sets and hashmaps have to be copied to another virtual space.
 * @param source The source reservation table.
 * @return The reference to "this"
 */
VirtualAnalogicalRTable& VirtualAnalogicalRTable::operator=(const VirtualAnalogicalRTable& source)
{
  *this = (AnalogicalResertavionTable) source;
  this->JobList = source.getJobList();

}

/**
 * The operator of asignament has ben overloaded due to when an assignament from one analogical reservation table to another virtualanalogicalrtable is done, the objects referenced  inside the sets and hashmaps have to be copied to another virtual space.
 * @param source The source reservation table.
 * @return The reference to "this"
 */
VirtualAnalogicalRTable& VirtualAnalogicalRTable::operator=(const AnalogicalResertavionTable& source)
{
  if(this == &source)
    return *this;
  
  //firts we have to copy all the references of architecture and so on
  this->architecture = source.getarchitecture();
  this->freeNodes = 0;
  this->dumpNumber = 0;
  this->log = source.getlog();
  this->globaltime = source.getglobaltime();
  this->initialGlobalTime = source.getinitialGlobalTime();
  
      
  this->lastBucketId = source.getlastBucketId();
  //Initialize the allBuckets 
  this->allBuckets = new BucketSetOrdered();
  this->ProcessJobTempAllocations = new map<int,Job*>();
  
  //second we have to startup all the bucket structures doing same stuff as the constructor of the AnalogicalReservationTable
  /* first we get the architecture .. to go through all the processors */

  ArchitectureConfiguration* architecture = this->getarchitecture(); 

  /* this value should never be null and there always have to be cpus*/
  assert(architecture != 0 && architecture->getNumberCpus() > 0);


  //Initialize the vector of cpu mapping job
  for(int i = 0; i < architecture->getNumberCpus();i++)
  {
    this->CPUMapping.push_back(0);
  }  

  /* creating the sets  */
  for(int i = 0; i <  architecture->getNumberCpus();i++)
  {    
    BucketSetOrdered* globalBuckets = new BucketSetOrdered();
    //time to copy the globalView[i] of the source - remember the allBuckets    
    this->globalView.push_back(globalBuckets);
    
   
   /*******************************************the buckets ***************************************+*/    
    BucketSetOrdered* freeBuckets = new BucketSetOrdered();
    this->buckets.push_back(freeBuckets);
            
    
    //time to copy the buckets[i] of the source 
    for(BucketSetOrdered::iterator itbuckets = source.buckets[i]->begin();
        itbuckets !=source.buckets[i]->end(); ++itbuckets)        
    {
      AnalogicalBucket* allocBucket = *itbuckets;
      
      AnalogicalBucket* copy = new AnalogicalBucket();
      *copy = *allocBucket; /* check that the copy operator works as expected */
      
      freeBuckets->insert(copy);
      globalBuckets->insert(copy);
            
    }        
    
    /*******************************************the allocations  ****************************************/        
    //time to copy the allocations[i] of the source - remember the allBuckets
    BucketSetOrdered* allocations = new BucketSetOrdered();
    this->allocations.push_back(allocations);    
    
    for(BucketSetOrdered::iterator itbuckets = source.allocations[i]->begin();
        itbuckets != source.allocations[i]->end(); ++itbuckets)        
    {
      AnalogicalBucket* allocBucket = *itbuckets;
      
      AnalogicalBucket* copy = new AnalogicalBucket();
      *copy = *allocBucket; /* check that the copy operator works as expected */
      
      allocations->insert(copy);
      allBuckets->insert(copy);      
      globalBuckets->insert(copy);            
      
      //in case the bucket is running .. we add it to the cpu mapping -- should occur only once 
      if(source.CPUMapping[i] == allocBucket)
        this->CPUMapping.push_back(copy);
    }
    
    
    /* incrementing the last assigned bucket */
    this->lastBucketId++;
    
  }        
  
  return (*this);
}


/**
 * This function is similar to the = operator however, given a reservation table will only copy the buckets and the structures about the jobs that are currently running 
 * @param source The source reservation table
 */
void VirtualAnalogicalRTable::copyRunningJobs(const AnalogicalResertavionTable& source)
{

  //firts we have to copy all the references of architecture and so on
  this->architecture = source.getarchitecture();
  this->freeNodes = 0;
  this->dumpNumber = 0;
  this->log = source.getlog();
  this->globaltime = source.getglobaltime();
  this->initialGlobalTime = source.getinitialGlobalTime();
  
      
  this->lastBucketId = source.getlastBucketId();
  //Initialize the allBuckets 
  this->allBuckets = new BucketSetOrdered();
  this->ProcessJobTempAllocations = new map<int,Job*>();
  
  //second we have to startup all the bucket structures doing same stuff as the constructor of the AnalogicalReservationTable
  /* first we get the architecture .. to go through all the processors */

  ArchitectureConfiguration* architecture = this->getarchitecture(); 

  /* this value should never be null and there always have to be cpus*/
  assert(architecture != 0 && architecture->getNumberCpus() > 0);

  /* creating the sets  */
  for(int i = 0; i <  architecture->getNumberCpus();i++)
  {    
   /* the free buckets */
    BucketSetOrdered* freeBuckets = new BucketSetOrdered();
    AnalogicalBucket* free = new AnalogicalBucket(0,-1);
    free->setId(this->lastBucketId);
    free->setCpu(i);
    freeBuckets->insert(free);     
    this->buckets.push_back(freeBuckets);
    
    this->log->debug("The size of the freeBuckets is "+itos(freeBuckets->size())+ " the buckets size is "+itos(this->buckets.size()),6);
    
    /*the global view */    
    BucketSetOrdered* globalBuckets = new BucketSetOrdered();
    globalBuckets->insert(free);   
    this->globalView.push_back(globalBuckets);
    
     this->log->debug("The size of the freeBuckets is "+itos(freeBuckets->size())+ " the globalView size is "+itos(this->globalView.size()),6);
    
    /*by default no allocations are currently assigned*/
    BucketSetOrdered* allocations = new BucketSetOrdered();
    this->allocations.push_back(allocations);    
    
    this->log->debug("The size of the allocations is "+itos(allocations->size()),6);
    
    /* incrementing the last assigned bucket */
    this->lastBucketId++;
    
  } 
   
  //Initialize the vector of cpu mapping job
  for(int i = 0; i < architecture->getNumberCpus();i++)
  {
    this->CPUMapping.push_back(0);
  }  
    
  //We just copy those jobs that are running -- so those buckets that are in the 
  for(int i = 0; i < source.CPUMapping.size();i++)
  {
    AnalogicalBucket* current =  source.CPUMapping[i];
 
   
   if(current == 0)
    continue;
   
   /*we have to find out if this time can be added to the previous free bucket it should be the initial one from 0 till -1 */      
   AnalogicalBucket* previousFreeBckt = this->findLowerBound(buckets[current->getcpu()],current);
      
   assert(previousFreeBckt->getstartTime() == 0 && previousFreeBckt->getendTime() == -1);
   
  if(current->getstartTime() > 0) 
     previousFreeBckt->setEndTime(current->getstartTime()-1);
  else 
  {
     this->buckets[i]->erase(previousFreeBckt); //This free time won't be used 
     delete previousFreeBckt;
  }
     
  //we create the free bucket from the end of the allocated bucket till the end 
  AnalogicalBucket* lastFreeBckt = new AnalogicalBucket(current->getendTime()+1,-1);
  lastFreeBckt->setCpu(i);
  lastFreeBckt->setId(++this->lastBucketId);
  lastFreeBckt->setUsed(false); 
  
  //we insert the free bucket to the buckets free and the global view 
  buckets[i]->insert(lastFreeBckt);  
  globalView[i]->insert(lastFreeBckt);
  
  //we create a copy of the provided bucket and we add it to allocations
  AnalogicalBucket* allocated = new AnalogicalBucket(current->getstartTime(),current->getendTime());
  
  allocated->setCpu(i);
  allocated->setId(++this->lastBucketId);
  allocated->setUsed(true);
  allocated->setJob(current->getjob());
  
  allocations[i]->insert(allocated);
  allBuckets->insert(allocated);
  globalView[i]->insert(allocated);
  
  }
  
}

/**
 * this function deallocates a given job, that has been allocated to the reservation table. The allocation can be for a job that is running or for a job that is still queued in the queue waitting for its startime.
 * @param job The job to deallocate 
 * @return True if the job has succesfully deallocated
*/
bool VirtualAnalogicalRTable::deAllocateJob(Job* job)
{
  if(job->getJobSimStatus() == COMPLETED || job->getJobSimStatus() == KILLED_BACKFILLING || job->getJobSimStatus() == KILLED_QUEUE_LIMIT || job->getJobSimStatus() == FAILED || job->getJobSimStatus() == CANCELLED)
  {
     //in this case the job will be released depending on the behaviour specified in the policy 
     if(this->FreeBucketsOnJobCompletion)
       AnalogicalResertavionTable::deAllocateJob(job);  
       
     this->finishedJobs.insert(pair<Job*,bool>(job,true));
     deleteRemovableAllocations(); //we remove only those allocations that for sure won't interfere the penalized runtime of the still running jobs 
       
     return true;
  }
    
    
  //we free the allocation jobs from the reservatio table 
  //but we have to free the buckets from the allBuckets set 
  map<Job*,JobAllocation*>::iterator allocIter = this->JobAllocationsMapping.find(job);   
  assert(allocIter != this->JobAllocationsMapping.end());
  AnalogicalJobAllocation* allocation = (AnalogicalJobAllocation*) allocIter->second;
  deleteAllocationFromAllBuckets(allocation);
  
  //otherwise we will realease the resources for the job 
  AnalogicalResertavionTable::deAllocateJob(job);  
  return true;
  
}

/**
 * This function frees all the memory used by a given analogical job allocation. However, the function only frees it contents, the allocation memory should be released by the caller function.
 * @param allocation The allocation that has to be released.
 */
void VirtualAnalogicalRTable::deleteAllocationFromAllBuckets(AnalogicalJobAllocation* allocation)
{

  for(deque<AnalogicalBucket*>::iterator itbuck = allocation->allocations.begin();
         itbuck != allocation->allocations.end(); ++itbuck)
    {
     AnalogicalBucket* bucket = *itbuck;
     BucketSetOrdered::iterator it = this->allBuckets->find(bucket);
     
     assert(it != this->allBuckets->end());     
     this->allBuckets->erase(bucket);
  }

}

/**
 * Deletes all the allocations from the reservation table that can be deleted without interfering the collision computation for the already running jobs. Thus, the allocations for those jobs that have been finished are relaesed when they do not share any shared shadow with any of the running jobs.
 * @return True if no problem has raised, false otherwise.
 */
bool VirtualAnalogicalRTable::deleteRemovableAllocations()
{
  vector<Job*> toDeallocate;

  for(map<Job*,bool>::iterator it = this->finishedJobs.begin(); it != this->finishedJobs.end();++it)
  {
    Job* job = it->first;
  
    map<Job*,JobAllocation*>::iterator allocIter = this->JobAllocationsMapping.find(job);   
    assert(allocIter != this->JobAllocationsMapping.end());
    AnalogicalJobAllocation* allocation = (AnalogicalJobAllocation*) allocIter->second;
    
    //if true we are sure that the allocation won't interfere with the penality computation of already running jobs 
    if(this->MinSTimeNoFinishedJob > allocation->getendTime())
    {
      toDeallocate.push_back(job);       
    }
  }
  
  log->debug("The current jobs in the finishedJobs is "+itos(finishedJobs.size()));
  
  for(vector<Job*>::iterator it = toDeallocate.begin();it != toDeallocate.end();++it)
  {
    Job* job = *it;
    
    log->debug("Removing permanently the finished job "+itos(job->getJobNumber())+" from the RTable",2);    
    //we find the allocation and free it 
    map<Job*,JobAllocation*>::iterator allocIter = this->JobAllocationsMapping.find(job);   
    AnalogicalJobAllocation* allocation = (AnalogicalJobAllocation*) allocIter->second;
    
    map<Job*,bool>::iterator it_check = this->finishedJobs.find(job);
    assert(it_check != this->finishedJobs.end());
    
    this->finishedJobs.erase(job);
    
    //we free the allocation jobs from the reservatio table 
    //but we have to free the buckets from the allBuckets set
    deleteAllocationFromAllBuckets(allocation);
    
    AnalogicalResertavionTable::deAllocateJob(job);

    
  }

}

/**
 * Inherited from reservation table. This function calls to the parental function allocatedJob
 * @see the reservation table class 
 * @param job The job to be allocated to the reservation table 
 * @param genericallocation The job allocation that has to be allocated 
 * @return True is the job has been correctly allocated 
 */

bool  VirtualAnalogicalRTable::allocateJob(Job* job,JobAllocation* genericallocation)
{
   AnalogicalResertavionTable::allocateJob(job,genericallocation);
   
   double min = -1;
   
   //we update the mininum start time for the jobs    
   for(map<Job*,JobAllocation*>::iterator allocIter = this->JobAllocationsMapping.begin();
       allocIter != this->JobAllocationsMapping.end();
       ++allocIter 
      )
    {
      AnalogicalJobAllocation* allocation = (AnalogicalJobAllocation*) allocIter->second;
      Job* allocatedJob = allocIter->first;
    
      if(allocatedJob->getJobSimStatus() == RUNNING || allocatedJob->getJobSimStatus() == LOCAL_QUEUED)
      {
         if(min == -1)
           min = allocation->getstartTime();
         else if(min > allocation->getstartTime())
           min = allocation->getstartTime();
      }
      
      if(this->log->getlevel() > 6)
      {
         for(deque<AnalogicalBucket*>::iterator itbuck = allocation->allocations.begin();
         itbuck != allocation->allocations.end(); ++itbuck)
         {
           AnalogicalBucket* bucket = *itbuck;
           log->debug("Job"+itos(bucket->getjob()->getJobNumber())+" running on the cpu "+itos(bucket->getcpu())+" from "+ftos(bucket->getstartTime())+" till "+ftos(bucket->getendTime()));
         }
      }
    }
    
    this->MinSTimeNoFinishedJob = min;
     
   AnalogicalJobAllocation* allocation = (AnalogicalJobAllocation*) genericallocation;

     
   //we have to add the buckets to the allBuckets structure 
   for(deque<AnalogicalBucket*>::iterator itbuck = allocation->allocations.begin();
         itbuck != allocation->allocations.end(); ++itbuck)
   {
     AnalogicalBucket* bucket = *itbuck;
     this->allBuckets->insert(bucket);
   }
}

/**
 * Returns the reference to all the buckets that currently asigned in the reservation table.
 * @return A reference to the BucketSetOrdered that contains the buckets 
 */
BucketSetOrdered* VirtualAnalogicalRTable::getallBuckets() const
{
  return allBuckets;
}


/**
 * Sets a reference to all the buckets that are currently asigned in the reservation table 
 * @param theValue The set of buckets 
 */
void VirtualAnalogicalRTable::setAllBuckets(BucketSetOrdered* theValue)
{
  allBuckets = theValue;
}

//using the polomorphism
/**
 * Function that finds the inmediate predecessor (at the time) bucket of the provided bucket in the set of buckets allocated in the given cpu that is not used 
 * @param cpu The id for the cpu that contains the buckets 
 * @param current The reference bucket 
 * @return Returns a reference to the predecessor for the provided bucket 
 */
AnalogicalBucket* VirtualAnalogicalRTable::findFreeBucketLowerBoundWithCpu(int cpu,AnalogicalBucket* current)
{
  return this->findLowerBound(buckets[cpu],current); 
}

/**
 * Function that finds the inmediate (at the time) bucket of the provided bucket in the set of buckets allocated in the given cpu that is not used 
 * @param cpu The id for the cpu that contains the buckets 
 * @param current The reference bucket 
 * @return Returns a reference to the next bucket of the provided bucket 
 */
AnalogicalBucket* VirtualAnalogicalRTable::findFreeBucketUpperBoundWithCpu(int cpu,AnalogicalBucket* current)
{
  return this->findUpperBound(buckets[cpu],current); 
}

/**
 * Function that finds the inmediate predecessor (at the time) bucket of the provided bucket in the set of buckets allocated in the given cpu that is used 
 * @param cpu The id for the cpu that contains the buckets 
 * @param current The reference bucket 
 * @return Returns a reference to the predecessor for the provided bucket 
 */
AnalogicalBucket* VirtualAnalogicalRTable::findUsedBucketLowerBoundWithCpu(int cpu,AnalogicalBucket* current)
{
  return this->findLowerBound(allocations[cpu],current); 
}

/**
 * Function that finds the inmediate (at the time) bucket of the provided bucket in the set of buckets allocated in the given cpu that is  used 
 * @param cpu The id for the cpu that contains the buckets 
 * @param current The reference bucket 
 * @return Returns a reference to the next bucket of the provided bucket 
 */
AnalogicalBucket* VirtualAnalogicalRTable::findUsedBucketUpperBoundWithCpu(int cpu,AnalogicalBucket* current)
{
  return this->findUpperBound(allocations[cpu],current); 
}

/**
 * Returns the default prefix name for the output dump files 
 * @return A string containing the prefix
 */
string VirtualAnalogicalRTable::getdumpDefaultName() const 
{
  return dumpDefaultName;
}


/**
 * Setst the default prefix name for the output dump files 
 * @param theValue The prefix name 
 */
void VirtualAnalogicalRTable::setDumpDefaultName(  const string& theValue)
{
  dumpDefaultName = theValue;
}

/**
 * Dumps all the buckets that are currently allocated in the reservation table to a file. The file name would composed by the default name and the dumpNumber. The file will contain a dump of all the allocations that are currently stored by the reservation table
 */
void VirtualAnalogicalRTable::DumpAllocatedBuckets()
{

  time_t rawtime;
  time ( &rawtime );
  char * time = ctime (&rawtime);
  time[24] = ' '; //chomp te \n character

  this->dumpNumber++;

  string file = this->dumpDefaultName+" ID "+itos(this->dumpNumber)+".dump";
  ofstream fout( file.c_str(), ios::trunc );

  fout << this->dumpHeader << endl;
  fout << "#job  #cpu startT  endT  joborigRT" << endl;
  for(BucketSetOrdered::iterator it = this->allBuckets->begin();it != this->allBuckets->end();++it)
  {  
     AnalogicalBucket* bucket = *it;
     
     fout << bucket->getjob()->getJobNumber() << " " << bucket->getcpu() << " " << bucket->getstartTime()  << " "<< bucket->getendTime() << " "<< bucket->getjob()->getRequestedTime() << endl;
  }
  
  fout.close();

}

/**
 * Dumps all the buckets that are currently allocated in the reservation table plus the buckets of the provided allocation to a file. The file name would composed by the default name and the dumpNumber. The file will contain a dump of all the allocations that are currently stored by the reservation table plus the buckets stored in the allocation.
 * @param allocation The allocation that has the buckets to be dummped to the file.
 */
void VirtualAnalogicalRTable::DumpAllocatedBuckets(AnalogicalJobAllocation* allocation)
{

  time_t rawtime;
  time ( &rawtime );
  char * time = ctime (&rawtime);
  time[24] = ' '; //chomp te \n character

  this->dumpNumber++;

  string file = this->dumpDefaultName+" ID "+itos(this->dumpNumber)+".dump";
  ofstream fout( file.c_str(), ios::trunc );

  fout << this->dumpHeader  << endl;
  fout << "#job  #cpu startT  endT  penalizedT" << endl;
  
  BucketSetOrdered temporal;

  for(BucketSetOrdered::iterator it = this->allBuckets->begin();it != this->allBuckets->end();++it)
  {  
    temporal.insert(*it);
  }
  
  for(deque<AnalogicalBucket*>::iterator it = allocation->allocations.begin();it != allocation->allocations.end();++it)
  {
    temporal.insert(*it);
  }
  
  fout << this->dumpHeader << endl;
  fout << "#job  #cpu startT  endT  joborigRT" << endl;
  for(BucketSetOrdered::iterator it = this->allBuckets->begin();it != this->allBuckets->end();++it)
  {  
     AnalogicalBucket* bucket = *it;
     
     fout << bucket->getjob()->getJobNumber() << " " << bucket->getcpu() << " " << bucket->getstartTime()  << " "<< bucket->getendTime() << " "<< bucket->getjob()->getRequestedTime() << endl;
  }
  
  fout.close();

}


/**
 * Retunrs the current dumpNumber
 * @return An integer containing the dump number.
 */
int VirtualAnalogicalRTable::getdumpNumber() const 
{
  return dumpNumber;
}


/**
 * Sets the current dump Number 
 * @param theValue The dump number 
 */
void VirtualAnalogicalRTable::setDumpNumber( const int& theValue)
{
  dumpNumber = theValue;
}



/**
 * Returns the header that will be stored at the beggining of the dump files 
 * @return A string containing the header 
 */
string VirtualAnalogicalRTable::getdumpHeader() const
{
  return dumpHeader;
}


/**
 * Sets the header that will be stored at the beggining of the dump file 
 * @param theValue The header for the dump file 
 */
void  VirtualAnalogicalRTable::setDumpHeader(const string& theValue)
{
  dumpHeader = theValue;
}

/**
 * When the bucket size is extended or reduced due to some collions have appeared or disappeared we must check while the free bucket that follows the ocupied bucket has to be extended (in it starttime ) when the penalty is reduced, or when it has to be reducted, when its penalty is extended. This function updates the free buckets according the new penalities, since no overlapping between buckets can occur, netiher can appear holes between two consecutive buckets. 
 */
void VirtualAnalogicalRTable::updateFreeBuckets()
{ 
  /* we just have to check if the given allocation interfers with the rest of the buckets   
  */  
  for(BucketSetOrdered::iterator it = allBuckets->begin(); it != allBuckets->end();++it)
  {
    AnalogicalBucket* bucket = *it;
       
   
   AnalogicalBucket* NextFreebucket = this->findFreeBucketUpperBoundWithCpu(bucket->getcpu(),bucket);
   AnalogicalBucket* NextUsedbucket = this->findUsedBucketUpperBoundWithCpu(bucket->getcpu(),bucket);
   
   
    /*
    
        _______________________________________________
    1) |___bucket1___|_ _ _ _ |_____bucket2|__free_bucket
    bucket1.endtime_î        î__ bucket2.entime
    
      This happends when the bucket1 has been reduced, and after such reduction, the bucket2 started exactly when bucket1 finished.
      In this case we must create a new free bucket from bucket1.endtime till bucket2.starttime                           
      
   */ 
      
   if(NextUsedbucket != NULL && bucket->getendTime()+1 < NextUsedbucket->getstartTime() &&  NextUsedbucket->getstartTime() < NextFreebucket->getstartTime())
   {
      //we just create a new free bucket from bucket1 till bucket2 and insert it 
      AnalogicalBucket* newFree = new AnalogicalBucket(bucket->getendTime()+1,NextUsedbucket->getendTime()-1);
      newFree->setCpu(bucket->getcpu());
      newFree->setId(++this->lastBucketId);
      newFree->setUsed(false);
      
      continue;
   }
   
   /*
       __________________________________
   2) |___bucket___|_ _ _ _ |free_bucket|
                    î__ bucket.entime
   
         In this case the free bucket has to start free_bucket.starttime = bucket.endtime. In this case we increment the size
        
      ____________________________________
   3) |___bucket___!_____|____free_bucket_|
     free_bucket.st_î     î__ bucket.entime
      
      In this case the freebucket.starttime = bucket.endtime, as the previous case. We are reducing the free freebucket size 
         
  */
        
   if((bucket->getendTime()+1 < NextFreebucket->getstartTime() && (NextUsedbucket == NULL || NextFreebucket->getstartTime() < NextUsedbucket->getstartTime())) || (bucket->getendTime() >= NextFreebucket->getstartTime())
   )
   {
     this->updateStartTimeFreeBucket(NextFreebucket,bucket->getendTime()+1);
     
     //We have to check if the free bucket can be extended till the following used bucket
     if(NextUsedbucket != NULL && NextFreebucket->getendTime()+1 < NextUsedbucket->getstartTime())
       NextFreebucket->setEndTime(NextUsedbucket->getstartTime()-1);

     
     continue;
   }        
   
   /*                  oldfree bucket
      ________________/____________________
   4) |___bucket_____!__|__!__free_bucket_|
     free_bucket.st_____î                î__ bucket.entime
   
     The final situation is given when teh current bucket overloads completly the next free bucket 
     it can rarely occurs, but it can.
   
   */
   
   if(NextUsedbucket != NULL && bucket->getendTime() == NextUsedbucket->getstartTime()+1 && NextFreebucket->getendTime() < NextUsedbucket->getendTime())
   {
     BucketSetOrdered::iterator toDelete = buckets[bucket->getcpu()]->find(NextFreebucket);
     
     if(toDelete != buckets[bucket->getcpu()]->end())
     {
       delete *toDelete;
       buckets[bucket->getcpu()]->erase(toDelete);       
     }
     else
       //should not happend !
       assert(false); 
   }
  }
}

/**
 * Decides the number of processes for the given job that should be allocated to the given node. This decision is based on the computational resources of the node and the computational requirments of the job.
 * @param job The job to be allocated.
 * @param node The node where the process of the job would be allocated.
 * @return The number of processes to be allocated in the node 
 */
int VirtualAnalogicalRTable::DecideNumberProcessesPerNode(Job* job,Node* node)
{
  
  //this stuff should be updated following the criteria that jesus has suggested to us..
  //the new approach is clearly more powerfull than the previous one.
  
  double ethreq = job->getBWEthernedUsed();
  double memreq = job->getBWMemoryUsed();
  double netreq = job->getBWNetworkUsed();
    
  int cpusNode = node->getNCPUS();
    
  double ethBWAv = node->getEthernetBW()/cpusNode;
  double memBWAv = node->getMemoryBW()/cpusNode;
  double netBWAv = node->getNetworkBW()/cpusNode;
  
  double BWmemPenalty = memreq/min(memreq,memBWAv) - 1;
  double BWnetPenalty = netreq/min(netreq,netBWAv) - 1;
  double BWethPenalty = ethreq/min(ethreq,ethBWAv) - 1;
  
  BWmemPenalty = (memreq == 0?  0 : BWmemPenalty);
  BWnetPenalty = (netreq == 0?  0 : BWnetPenalty);
  BWethPenalty = (ethreq == 0?  0 : BWethPenalty);
  
  double penalty_factor = 1+(BWmemPenalty +BWnetPenalty+ BWethPenalty);
  
  if(ceil(penalty_factor) < penalty_factor)
   penalty_factor = ceil(penalty_factor)+1;
  else
   penalty_factor = ceil(penalty_factor);
  
  log->debug("The penalty_factor that decides how many processes can be stored in the same node is "+ftos(penalty_factor),6);
  
  int reqProcs = min(job->getNumberProcessors(),cpusNode);
  
  if(penalty_factor == 0)
  {
    log->debug("All the processes can be allocated to same nodes, with no restrictions",6);
    return reqProcs;
  }
  
  double decision = ceil(reqProcs/penalty_factor);    
  
  if(decision < reqProcs/penalty_factor)
    return (int) decision+1;
  else
    return (int) decision;
  
}

/**
 * This a resource selection policy that tries to reduce the conflicts between the allocated jobs by distributting the processes among different processors based on their demand. It tries to allocate the processes for the job in different nodes in case that the required resources are higher that the once that would be assigned per processer.
 * @param suitablebuckets A set of buckets that are suitable for the allocation, each analogical bucket is for one different processors 
 * @param length The duration required for the allocation 
 * @param numcpus The number of cpus required for the allocation 
 * @param initialShadow The point of time that the allocation is required 
 * @param job The job that is being allocated 
 * @return The allocation that matches the creiteria (if there is not allocation that matches the requirements it will be shown in the allocation problem). The returned allocation is a not real allocation. 
 */
AnalogicalJobAllocation* VirtualAnalogicalRTable::findDistributeConsumeMethodAllocation(BucketSetOrdered* suitablebuckets,double length,int numcpus,double initialShadow,Job* job)
{		  
  deque<AnalogicalBucket*> sharedShadow; 
  double startShadow = initialShadow;
  double endShadow = startShadow+length;
  int assignedcpus = 0;
  
  AnalogicalJobAllocation* allocation = NULL;
  AnalogicalJobAllocation* earlierAllocation = NULL;	
	  

  
  map<int,int> RemainingProcesNode; //this variable will indicate how many processes we will have allocated for the given job to the given nodes 
                              //with this variable we will try to avoid to put a lot processes to the same node
  
  /* we try to find out the buckets ordered by the time */
  MareNostrum* mn = ((MNConfiguration*) this->architecture)->getmarenostrum();
  	   
  for(BucketSetOrdered::iterator i= suitablebuckets->begin(); i!=suitablebuckets->end(); ++i)
  {
    /*we have to compare with the first element */
    AnalogicalBucket*  current = *i;    
    
    log->debug("findDistributeConsumeMethodAllocation: cheking the bucket CPU "+itos(current->getcpu())+" to the suitablebuckets list whom start time is "+ftos(current->getstartTime())+" and whom endtime is "+ftos(current->getendTime())+" and bucket id "+itos(current->getId()),6);
    
    deque<AnalogicalBucket*>::iterator shbuckets = sharedShadow.begin();    
    AnalogicalBucket* firstBucket = *shbuckets;
    

    //we compute the sutff available in the bucket     
    int nodeId = mn->getNodeIdCpu(current->getcpu());
    
    map<int,int>::iterator nodeAllocs = RemainingProcesNode.find(nodeId);
    int remainingAllocs = 0;
    if(nodeAllocs == RemainingProcesNode.end())
    {
      //first time that the node is evaluated 
      Node* node = mn->getNodeWithGlobalId(nodeId);
      int decision = this->DecideNumberProcessesPerNode(job,node);
      log->debug("findDistributeConsumeMethodAllocation: The decision is to allocate "+ftos(decision)+" processes in the same node",6);
      remainingAllocs = decision;
      RemainingProcesNode.insert(pairInt(nodeId,decision--));
    }
    else
    {          
      //in case no more allocations can be carried out in the same node we iterate till we change of node ..
      if(nodeAllocs->second <= 1)
        continue;
      
      nodeAllocs->second--;
    }
    

    
    //we simple insert the bucket    
    sharedShadow.push_back(current);
    assignedcpus++;
    
    /* for sanity */
    assert(assignedcpus >= 0);    
    
    if(sharedShadow.size() != 0)
    {
      //we just have to check that the buckets of the vector shares the required window of time with the current 
      //bucket, be aware that by construction the buckets are ordered by they start time
      
      double maxStartShadow = initialShadow;
      
      for(;sharedShadow.end() != shbuckets;++shbuckets)
      {
        AnalogicalBucket* bucketShared = *shbuckets;
        //if the current bucket and the new processed bucket does not share the required period of time
        //we have to push popfront it .. otherwise.. by construction (ordered by startime) the rest of 
        //buckets shares the same period of time .. so we have finished
           
        
        if(bucketShared->getendTime() - current->getstartTime() < length && 
           !(bucketShared->getendTime() == -1 && bucketShared->getendTime() == -1 )) //if both buckets are to infinity the share all the time..                      
        {          
          //we must delete all the previous buckets .. since the allocation can't be continous ,,
          for(deque<AnalogicalBucket*>::iterator deleteIt = sharedShadow.begin();deleteIt != shbuckets;++deleteIt)
          {
            sharedShadow.erase(deleteIt);
            assignedcpus--;
          }  
          //we delete the bucket 
          sharedShadow.erase(shbuckets);
          
          assignedcpus--;
          
          log->debug("findDistributeConsumeMethodAllocation: UNselecting the bucket with id "+itos(current->getId()),6);
        }
        else
        {          
          //we must to take care that the bucket shadow is after the given limit
           if(bucketShared->getstartTime() > maxStartShadow)                             
           {
             maxStartShadow = bucketShared->getstartTime();            
           }
        }
      }
      
      startShadow = maxStartShadow;
      endShadow = startShadow+length;   
    }
    else
    {
      //initialize start the shadow according to this bucket .. the unique one at this moment
      //we must to take care that the bucket shadow is after the given limit
      if(startShadow < current->getstartTime())
        startShadow = current->getstartTime();
       
      endShadow = startShadow+length;
      
      /* just for sanity */
      assert(endShadow >= current->getendTime());
    }
                                     
    
    if(assignedcpus == numcpus)
    {      
      allocation = new AnalogicalJobAllocation(sharedShadow,startShadow,endShadow);
      break;
      
    } 
    
  }
  
  if(log->getlevel() >= 4)
   for(deque<AnalogicalBucket*>::iterator it = allocation->allocations.begin();it != allocation->allocations.end();++it)
   {
     AnalogicalBucket* logbuck = (AnalogicalBucket*) *it;     
     log->debug("findDistributeConsumeMethodAllocation: Selected the CPU "+itos(logbuck->getcpu())+" from "+ftos(allocation->getstartTime())+ " till "+ftos(allocation->getendTime()),4);
   }  
  
  /*in the case that there are not enough buckets that satisties the given search criteria */
  if(assignedcpus < numcpus)
  {
    allocation = new AnalogicalJobAllocation();
    allocation->setAllocationProblem(true);
    allocation->setAllocationReason(NOT_ENOUGH_CPUS); 
  }
  
  return allocation;
  
}

/**
 * Returns an iterator with pointing the last bucket whom start time is bigger than the provided 
 * @param shadowSet The set of buckets 
 * @param current The reference bucket 
 * @return A ShadowSetOrdered::itertor pointing to the desired bucket 
 */
ShadowSetOrdered::iterator VirtualAnalogicalRTable::lower_Shadow_iterator(ShadowSetOrdered* shadowSet,AnalogicalBucket* current)
{
  
  /* RIGHT NOW ITS NOT USED -- WE SHOULD FIND OUT THE MORE OPTIMAL SOLUCTION .. OR ALMOST OPTIMAL */
  
  ShadowSetOrdered::iterator itfreeb = shadowSet->begin();
  
  for(;itfreeb != shadowSet->end();++itfreeb)
  {
     SharedShadow *  sh = *itfreeb;
     double startTime = sh->getstartShadow();
    if(!( startTime < current->getstartTime())) 
    {
      return itfreeb;
    }
  }
  return itfreeb;    

}


/**
 * Compute the penalties for each job according the penalities that are added into each shadow. 
 * @param shadows The set of shadows that will be used for compute the penalties for the jobs 
 */
void VirtualAnalogicalRTable::ComputeShadowPenalties(vector<ShadowSetOrdered*>* shadows)
{    
  int nodeid = 0;
  
 ArchDataModel::MareNostrum* mn = ((MNConfiguration*) this->getarchitecture())->getmarenostrum();
    
  log->debug("Computing all the penalties according to the shadows..",6);
  log->debug("There are "+itos(shadows->size())+" sets of shadows to be computed according to the number of nodes..",6)  ;
  
  map<int,double> maxJobPenalty;
  typedef pair<int,double> pairPenalty;
  typedef pair<int,bool> pairBool;
  
  for(vector<ShadowSetOrdered*>::iterator it = shadows->begin();it != shadows->end();++it)
  {
    ShadowSetOrdered* shadowsNode = *it;
    
    map<int,double> JobPenalty;
        
    //We get the node id ..
    Node* node = mn->getNodeWithGlobalId(nodeid);
    
    log->debug("Computing penalties for jobs running on node with global id "+itos(nodeid),6);  
    
    double ethAv = node->getEthernetBW();
    double mbAv = node->getMemoryBW();
    double nbAv = node->getNetworkBW();
    
    log->debug("The node capabilities are: ethBW "+ftos(ethAv)+" : memBW "+ftos(mbAv)+" : netBW "+ftos(nbAv),6);
    
    for(ShadowSetOrdered::iterator itsh = shadowsNode->begin();itsh != shadowsNode->end();++itsh)
    {   
      SharedShadow* shadow = *itsh;
      
      //the capacity must be divided for each process for all jobs
      int jobsShadow =  shadow->getNumberProcesses() == 0? 1 : shadow->getNumberProcesses();
      double shadowPenalty = this->computePenalty(mbAv,nbAv,ethAv,
                                                  shadow->getmbwShReq(),
                                                  shadow->getnbwShReq(),
                                                  shadow->getethShReq(),
                                                  shadow);   
      
      log->debug("Computing the job penalities for the shadow with startime "+ftos(shadow->getstartShadow())+" and endtime "+ftos(shadow->getendShadow()),6);
      log->debug("The penalty time that will be added to all the jobs belonging to the Shadow is "+ftos(shadowPenalty),6);
      
      map<int,bool> jobProcessed;
           
      for(JobBucketsMapping::iterator vctBucket = shadow->buckets.begin(); vctBucket != shadow->buckets.end();++vctBucket)
      {  
        vector<AnalogicalBucket*>* jobBuckets = vctBucket->second;
        
        for(vector<AnalogicalBucket*>::iterator itbuck = jobBuckets->begin(); itbuck != jobBuckets->end();++itbuck)
        {
           AnalogicalBucket* bucket = *itbuck;
           
           log->debug("Processing the bucket with startime "+ftos(bucket->getstartTime())+ " and endtime "+ftos(bucket->getendTime())+ " (id "+itos(bucket->getId())+")",6);
         
           int jobid = bucket->getjob()->getJobNumber();
           
           if(jobProcessed.count(jobid) == 0)
           { 
              log->debug("First bucket for this node and for the job "+itos(jobid),6);
                           
              log->debug("The penalty that is currently being added to the job by the shadow is "+ftos(shadowPenalty),6);
              
              map<int,double>::iterator itpen =  JobPenalty.find(jobid);
              
              if(itpen != JobPenalty.end())
              {
                JobPenalty[jobid] = itpen->second+shadowPenalty;
                log->debug("Incrementeing to "+ftos(itpen->second+shadowPenalty),6);
              } 
              else
              {
               JobPenalty.insert(pairPenalty(jobid,shadowPenalty));                
               log->debug("Inserting the penalty to "+ftos(shadowPenalty)+" to the job "+itos(jobid),6);
              } 
              jobProcessed.insert(pairBool(jobid,true));
           }
           else
             log->debug("The job where this bucket belongs has been already checked.",6);
         }
      }
    }
    
     
    //it's time to update the maximium penalties for each job 
   for(map<int,double>::iterator itpenalty = JobPenalty.begin();itpenalty != JobPenalty.end();++itpenalty)
   {
      log->debug("The penalty for the job "+itos(itpenalty->first)+" in the node is "+ftos(itpenalty->second),6);
      
      double TotalNodePenalty = itpenalty->second;
      int jobid = itpenalty->first;
      //the first time that the job is treated in the current node, so we must compute its penalty 
      map<int,double>::iterator it = maxJobPenalty.find(itpenalty->first);
            
      
      log->debug(" The maximum penalty for the job till now is "+ftos(it->second),6);
            
      if(it == maxJobPenalty.end())
      {
        maxJobPenalty.insert(pairPenalty(jobid,itpenalty->second));
        log->debug("Inserting "+ftos(itpenalty->second)+" for the job "+itos(itpenalty->first),6);
      }  
      else if(itpenalty->second > it->second)
      {        
        maxJobPenalty[jobid]=itpenalty->second;
        log->debug("Inserting "+ftos(itpenalty->second)+" for the job "+itos(itpenalty->first),6); 
      } 
          
      //for debug    
      it = maxJobPenalty.find(itpenalty->first);
      log->debug(" The maximum penalty for the job now is "+ftos(it->second),6);
   
    } 
    
    nodeid++;
  }
  
  //now each penalty we must update all the buckets provided in the shadows
  map<int,bool> bucketProcessed;
  
  for(vector<ShadowSetOrdered*>::iterator it = shadows->begin();it != shadows->end();++it)
  {
     ShadowSetOrdered* shadowsNode = *it;

     for(ShadowSetOrdered::iterator itsh = shadowsNode->begin();itsh != shadowsNode->end();++itsh)
     {   
       SharedShadow* shadow = *itsh;  

       for(JobBucketsMapping::iterator vctBucket = shadow->buckets.begin(); vctBucket != shadow->buckets.end();++vctBucket)
       {  
         vector<AnalogicalBucket*>* jobBuckets = vctBucket->second;
        
         for(vector<AnalogicalBucket*>::iterator itbuck = jobBuckets->begin(); itbuck != jobBuckets->end();++itbuck)
         {
            AnalogicalBucket* bucket = *itbuck;
           
           //we check that the bucket has not been already processed
           if(bucketProcessed.count(bucket->getId()) > 0)
             continue;
    
           map<int,double>::iterator penit = maxJobPenalty.find(bucket->getjob()->getJobNumber());
    
           double penalty = 0;
    
           if(penit != maxJobPenalty.end())
           {
             penalty = penit->second;
           }
     
           bucket->setPenalizedTime(penalty); //automatically updates the penalized end time 
           bucket->setPenalizedEndTime(bucket->getendTime()+penalty); 
           log->debug("Setting the penalizedEndtime for the bucket with startime "+ftos(bucket->getstartTime())+ " and endtime "+ftos(bucket->getendTime())+ " (id "+itos(bucket->getId())+") to "+ftos(penalty),6);
           
           bucketProcessed.insert(pairBool(bucket->getId(),true));
         }
       }
    }
  }
  
  //finally we upadate the penalized runtime of all the jobs that have been computed
  for(map<int,double>::iterator itpen =  maxJobPenalty.begin();itpen != maxJobPenalty.end();++itpen)
  {
    int jobid = itpen->first;
  
    //we find the job and update its penalty
    map<int,Job*>::iterator jobit =  this->JobList->find(jobid);
    Job* job;        
    
    if(jobit != this->JobList->end())
    {
      job = jobit->second;
    }
    //if we do not find it means that it is a temporal process or temporal job,     
    else
    {
       map<int,Job*>::iterator procit =  this->ProcessJobTempAllocations->find(jobid);
    
       if(procit != this->ProcessJobTempAllocations->end())
       {
         job = procit->second;        
       }
       else
        //shouldn't happend 
        assert(false); 
       
    } 
    job->setPenalizedRunTime(itpen->second);
    log->debug("Setting the penalized runtime for the job "+itos(itpen->first)+" to "+ftos(itpen->second),6);
  }; 
}

//the igonore job allows to check "wait if" if the job has been already allocated 
//for example, the job 72 has been allocated, in the RT, but we have a new allocation 
//for the same job that wants to be tested, in this case, as the job has been already inserted 
//interfers with the allocation that is provided. 

vector<ShadowSetOrdered*>  VirtualAnalogicalRTable::ComputeShadows(AnalogicalJobAllocation* allocation,Job* ingnoreJob = 0,set<int>* checkOnlyNodes = NULL)
{
  
  /* we must find out if the current allocation buckets collates with any job -- using the assignament "=" operator */
  
  BucketSetOrdered allBucketsPlusAl;
  
  if(this->allBuckets->size() > 0)
  {    
    for(BucketSetOrdered::iterator it = this->allBuckets->begin();it != this->allBuckets->end();++it)
    {
      AnalogicalBucket* bucket = *it;

      //if the checkOnlyNode provided is -1 means that all the nodes need to be cheked
      int node = this->architecture->getNodeIdCpu(bucket->getcpu());

      //if the job does not want to be tested we continue, or it is not the node that we wanna check .
      if(bucket->getjob() != ingnoreJob && (checkOnlyNodes == NULL || checkOnlyNodes->find(node) != checkOnlyNodes->end()))
        allBucketsPlusAl.insert(bucket);
      
    }
      
  }

  vector<ShadowSetOrdered*> collisions;
  
  log->debug("Initializing the shared shadows containers for all "+itos(this->architecture->getNumberNodes())+" nodes",6);
  
  for(int i = 0;i < this->architecture->getNumberNodes();i++)
  {          
     collisions.push_back(new ShadowSetOrdered());
  }
  
  //we put the buckets as we would have allocated them in case an allocation is provided
  if(allocation != NULL)
  {
    log->debug("Computing the Shadows for the virtual allocation with startime "+ftos(allocation->getstartTime())+ " and end time "+ftos(allocation->getendTime()),6);

    for(deque<AnalogicalBucket*>::iterator it = allocation->allocations.begin();it != allocation->allocations.end();++it)
    {
      AnalogicalBucket* allocBucket = *it;

      int node = this->architecture->getNodeIdCpu(allocBucket->getcpu());

      if( checkOnlyNodes == NULL || checkOnlyNodes->find(node) != checkOnlyNodes->end())
      {
         
         allBucketsPlusAl.insert(allocBucket);
      }

    } 
  }
  
  
  /* we just have to check if the given allocation interfers with the rest of the buckets   
  */  
  for(BucketSetOrdered::iterator it = allBucketsPlusAl.begin(); it != allBucketsPlusAl.end();++it)
  {
    AnalogicalBucket* bucket = *it;
    
    /*we must check if the current bucket collates with the allocation */
    int node = this->architecture->getNodeIdCpu(bucket->getcpu());
    
    log->debug("Treating the bucket with start time "+ftos(bucket->getstartTime())+" and finish time "+ ftos(bucket->getpenalizedEndTime())+ " (bucket id "+itos(bucket->getId())+")",6);
    
    if(collisions[node]->size() == 0)
    {
      //no shadow so we create one shadwo and follow with the following bucket..
      SharedShadow* firstShadow = new SharedShadow(bucket->getstartTime(),bucket->getendTime(),node) ;
      firstShadow->setId(this->lastShadowId++);
      firstShadow->insert(bucket);
      
      log->debug("Initializing the shadow container for the node "+itos(node),6);
      log->debug("Creating a first shared shadow including the bucket with start time"+ftos(bucket->getstartTime())+" and finish time "+ ftos(bucket->getpenalizedEndTime())+ " (bucket id "+itos(bucket->getId())+")",6);
      collisions[node]->insert(firstShadow);
      continue;
    }
    
    //here we know that at least the node where the bucket is allocated has one sharedshadow ..
    //so we have to check wether they are interfered by the current bucket.
    //we have to go through all the shadows that goes from bucket start time till bucket end time 
    //and check what's going on.. 
    ShadowSetOrdered::iterator suitable = collisions[node]->begin();
    SharedShadow* lastShadow = NULL;
    
    
    log->debug("Looking for the suitable shared shadows for the bucket "+itos(bucket->getId()),6);
   
    int collisionChecked = 0;     
    
    //when creating a new shadow we must avoid to check the same shadow agains the current 
    map<int,bool> ShadowChecked;
    typedef pair<int,bool> pairBool;
 
    for(;suitable!=collisions[node]->end();++suitable)
    {
      SharedShadow* current = *suitable;
      lastShadow = current;
      collisionChecked++;
     
      if(ShadowChecked.count(current->getId()) > 0)
        continue;
     
       log->debug("Checking the shadow with start time "+ftos(current->getstartShadow())+ " and end time "+ftos(current->getendShadow()),6);
     
     /*
       FIGURE 0: 
       The first situation we will treat is when the current bucket is exactly in the middele of the current bucket
       
       a____________________b 
       |____________________|
       !_ _ _ |_________|_ _!
             c         d
       in this case we will have to create two new shadows and reduce the current shadows (that goes from a till b).
       We will create a shadow from c to d and insert the bucket there.. we will create a shadow from d till b, and will 
       will reduce the current bucket from b till c.
     */
     
     if(bucket->getstartTime() > current->getstartShadow() && bucket->getendTime() < current->getendShadow())
     {
       
       log->debug("The bucket is prefectly inside the current evaluate shadow.",6);
       log->debug("The bucket is in the middle of the shadow. We split the shadow in three shadows.",6);
       log->debug("Creating a shadow from "+ftos(bucket->getstartTime()) +" till "+ftos(bucket->getendTime()),6);
       
       SharedShadow* firstShadow = new SharedShadow(bucket->getstartTime(),bucket->getendTime(),node);
       *firstShadow = *current; //the operator = has been overloaded
       firstShadow->setId(++this->lastShadowId);
       
       log->debug("Inserting the bucket to this intermediate shadow.",6);
       firstShadow->insert(bucket);
       
       ShadowChecked.insert(pairBool(firstShadow->getId(),true));
       
       collisions[node]->insert(firstShadow);
       
       log->debug("Creating a shadow from "+ftos(bucket->getendTime()) +" till "+ftos(current->getendShadow()),6);
       SharedShadow* secondShadow = new SharedShadow(bucket->getendTime()+1,current->getendShadow(),node);
       *firstShadow = *current; //the operator = has been overloaded
       firstShadow->setId(++this->lastShadowId);
       
       collisions[node]->insert(secondShadow);
       
       ShadowChecked.insert(pairBool(secondShadow->getId(),true));
       
       log->debug("Reducing the current a shadow. It goes from "+ftos(current->getstartShadow()) +" till "+ftos(bucket->getstartTime()),6);
       current->setEndShadow(bucket->getstartTime()-1);
       
       
       
       continue;
     }
     
     
     
     /* FIGURE 1:     
       This case the last shadow shares an amount of time with the current bucket..so we must update the shadow and see if the runtime of all the jobs that form part of the shadow is being affected.. see the figure (insertin bucket 2)
     
     a
     |___________b___c
     |__bucket_1_!___!__________d  --> there was only one shadow and we must create on more and reduce the current a and b
     |___________!_bu!cket_2___| 
     |
       - We must move copy all the buckets of the shadow 1 to the shadow 2 and reduce it's lenght , and put to the shadow two the initial part of the bucket 2 form b-c, we leavet the c - d interval for the following shadow if it exists (if not we will create a new shadow in the last iteration)
     
      */
     
     if(current->getendShadow() > bucket->getstartTime() && current->getstartShadow() < bucket->getstartTime())     
     {
       /* at maximmum we will have three shadows*/
       
       //first we must create the second bucket
       log->debug("The bucket has the first part in the last part of the shadow.",6);
       log->debug("Creating a shadow from "+ftos(bucket->getstartTime()) +" till "+ftos(current->getendShadow()),6);
       SharedShadow* secondShadow = new SharedShadow(bucket->getstartTime(),current->getendShadow(),node);       
       *secondShadow = *current; //the operator = has been overloaded
       secondShadow->setId(++this->lastShadowId);
       
       //we copy the resources used
       
       log->debug("Inserting the bucket to this  last created shadow.",6);
       secondShadow->insert(bucket);
       //we insert the created shadow       
       collisions[node]->insert(secondShadow);
       
       ShadowChecked.insert(pairBool(secondShadow->getId(),true));
       
       //then third bucket will be create or not .. in the follow itertion
       //finally we decrease the first shadow  
       log->debug("Reducing the current a shadow. It goes from "+ftos(current->getstartShadow()) +" till "+ftos(bucket->getstartTime()-1),6);
       current->setEndShadow(bucket->getstartTime()-1);
       
       
       
       continue;
     }
     
     /*
        FIGURE 2: 
       
        We can find the other case, where the bucket shares the initial part of the shadows but not the second parts
        
        a____________c__b
        |___bucket 1_!__|
        |___bucket 2_|
        |
        
        In this case the current bucket goes from a to b and the current bucket goes from a to c. So we create a new shadow from a till c and insert the bucket 2 and copy all the buckets from the shadow. And we reduce the starttime for the shdadow from a till c. Be aware that may be the current bucket starts earlier than a .. however this fact would have already treated by the before iteration.
     */
     
     if(current->getstartShadow() < bucket->getendTime() && bucket->getendTime() < current->getendShadow())
     {
       log->debug("The bucket has the last part in the initial part of the shadow.",6);
       log->debug("Creating a shadow from "+ftos(current->getstartShadow()) +" till "+ftos(bucket->getendTime()),6);
       SharedShadow* firstShadow = new SharedShadow(bucket->getstartTime()+1,current->getendShadow(),node);
       *firstShadow = *current; //the operator = has been overloaded
       firstShadow->setId(++this->lastShadowId);
       
       collisions[node]->insert(firstShadow);
       
       ShadowChecked.insert(pairBool(firstShadow->getId(),true));
       
       log->debug("Inserting the bucket to the current shadow.",6);
       current->setEndShadow(bucket->getendTime());
       current->insert(bucket);
       
       continue;
     }
     
     /* finally in the rare case that have exactly the same size or the bucket is fully included in the shadow .. we will just add the bucket to the shadow      */
     if(bucket->getstartTime() <= current->getstartShadow() && bucket->getendTime() >= current->getendShadow())
     {
       //we simply add the bucket 
       log->debug("The current shadow is totally included in the bucket interval. So we just add the bucket to the current shadow",6);
       current->insert(bucket);
       continue;
     }
    }  
    
    /* here we treat the situation when the the last part of the bucket does not share any shadow .. but it has an initial parts
       that shares.. we create a new shadow with the last part of the bucket.
       
       This would be the shadow C-D in the figure 1.              
    */
    
    
    if(lastShadow != NULL && lastShadow->getendShadow() < bucket->getendTime() && lastShadow->getendShadow() > bucket->getstartTime())
    {
       log->debug("The last shadow does not fully includes the current bucket, so we create a bucket that will include its last part",6);
       //We must create a shadow for the last part of the bucket .. coz no more shadows are present after this point
       log->debug("Creating a shadow from "+ftos(lastShadow->getendShadow()+1) +" till "+ftos(bucket->getendTime()),6);

       SharedShadow* last = new SharedShadow(lastShadow->getendShadow(),bucket->getendTime(),node);      
       last->setId(this->lastShadowId++);
       log->debug("Inserting the bucket to the last shadow created.",6);
       last->insert(bucket);
       
       ShadowChecked.insert(pairBool(last->getId(),true));
       
       collisions[node]->insert(last);
    }
    
    /*
      FIGURE 2:
    
      In the following picture one can see three buckets 3 buckets and 4 sharedShadow (a-b, b-c, c-d and d-e) for one node:
     
      The simpler situation is that the older shadow does not shares the time with the current bucket .. se the figure:
     
      a
      |___________b___c
      |__bucket_1_!___!__d____________________________e
      |___________!___|_|___________bucket 3_________|
      |           !___!
                     ^ bucket 2
                     
     Supose the current bucket is bucket 3. The last shadow is from b till c so we must create a new shadow and insert the currt bucket.. the easier situation .. 
     */
     
      if(lastShadow == NULL || (lastShadow->getendShadow() < bucket->getstartTime()))
      {
       //the current shadows does not share any interval of time with the current bucket 
       //and by construction of the iterator, the following shadows do not share also the i
       //no shadow so we create one shadwo and follow with the following bucket..
       log->debug("The last shadow does not overlap any part with the current bucket, so we create one and insert the bucket to this ",6);
       log->debug("Creating a shadow from "+ftos(bucket->getstartTime()) +" till "+ftos(bucket->getendTime()),6);

       SharedShadow* firstShadow = new SharedShadow(bucket->getstartTime(),bucket->getendTime(),node) ;
       firstShadow->setId(this->lastShadowId++);
       firstShadow->insert(bucket);
       
       collisions[node]->insert(firstShadow);
       ShadowChecked.insert(pairBool(firstShadow->getId(),true));
       
       continue;
      }
  }
  
  
  return collisions;
}

/**
 * Given the required of and the availables resources the function returns the penalty that the job would experiments if such situation would take place. 
 * @param availableMBW The available memory bandwith
 * @param availableNBW The available network bandwith
 * @param availableETH The available ethernet bandwith
 * @param memreq The required memory bandwith
 * @param netreq The required network bandwith
 * @param ethreq The required ethernet bandwith
 * @param shadow A reference to the shadow where such requirements are needed
 * @return The amount of penalized time 
 */
double VirtualAnalogicalRTable::computePenalty(double availableMBW,double availableNBW,double availableETH,
                                                 double memreq, double netreq, double ethreq, 
                                                 SharedShadow* shadow)
{
  
  //BE AWARE ! if changeing this definition, please update the same in the virtual analogical rtable 
  double BWmemPenalty = memreq/min(memreq,availableMBW) - 1;
  double BWnetPenalty = netreq/min(netreq,availableNBW) - 1;
  double BWethPenalty = ethreq/min(ethreq,availableETH) - 1;
  
  BWmemPenalty = (memreq == 0?  0 : BWmemPenalty);
  BWnetPenalty = (netreq == 0?  0 : BWnetPenalty);
  BWethPenalty = (ethreq == 0?  0 : BWethPenalty);
  
  log->debug("The penalty added by the BWMem availability is "+ftos(BWmemPenalty),6);
  log->debug("The penalty added by the BWnet availability is "+ftos(BWnetPenalty),6);
  log->debug("The penalty added by the BWeth availability is "+ftos(BWethPenalty),6);
  
  double time = shadow->getendShadow() - shadow->getstartShadow();
  
  double penalty_factor = (BWmemPenalty +BWnetPenalty+ BWethPenalty)*time;

  if(ceil(penalty_factor) < penalty_factor)
    penalty_factor = ceil(penalty_factor)+1;
  else
    penalty_factor = ceil(penalty_factor);

  log->debug("The penalty factor is "+ftos(penalty_factor),6);

  //we return the amount of time that the current
  return penalty_factor;
}

/**
 * Compute the penalties that a given allocation may imply in all the allocated jobs.
 * @param allocation The allocation that is required to be tested 
 * @return The metric with the performance that the current schedule would experiment if the  allocation would  be done 
 */
Metric* VirtualAnalogicalRTable::computeAllocationPenalties(AnalogicalJobAllocation* allocation)
{

  //before computting the penalities we clear the penalties associated to each bucket .. may be the approach will differ in the future 
  resetPenalties(allocation,NULL);
  
  //now we create the shadows that would be created if the current allocation would have been carried out
  vector<ShadowSetOrdered*>  shadows = ComputeShadows(allocation);
  
  //we compute the penalties that each shadows add to each job
  ComputeShadowPenalties(&shadows); 

  //now that we've checked and computed all the penalties that the jobs have for  the collision 
  //we check that there is no overlaping between jobs in the same processors in a given point of time 
  if(Overlapping(allocation))
  {
    Metric* result = new Metric();
    result->setStatisticUsed(CONSTANT);
    result->setNativeBool(true);
    
    allocation->setAllocationProblem(true);
    allocation->setAllocationReason(JOBS_COLLISION); 
    
    return result;
  }
  
  //we free the memory used by the computation 
  this->deleteShadows(&shadows);
  
  //in case that there is no overlapping we compute the metrics 
  Metric* performance = computePerformanceMetric(allocation);
  
  return performance; 
}

/**
 * Deletes the memory used by the vector of shadows 
 * @param shadows A reference to the vector to be released
 */
void VirtualAnalogicalRTable::deleteShadows(vector<ShadowSetOrdered*>* shadows)
{
    //we must free all the shadow stuff 
  for(vector<ShadowSetOrdered*>::iterator it = shadows->begin();it != shadows->end();++it)
  {
    ShadowSetOrdered* shadowsNode = *it;
    
    for(ShadowSetOrdered::iterator itsh = shadowsNode->begin();itsh != shadowsNode->end();++itsh)
    {   
      SharedShadow* shadow = *itsh;      
      delete shadow;      
    } 
    
    delete shadowsNode;
  }
}


/**
 * Computes the metric that the current schedule would achieve if the current allocation would be done
 * @param allocation The allocation to test 
 * @return The metric containing the performance
 */
Metric* VirtualAnalogicalRTable::computePerformanceMetric(AnalogicalJobAllocation* allocation)
{
  //we will compute the desired metric, we use the rtable rather than the job list due to the job list is not updated only the reservation table (remember that it is temporal)
  
  map<int, bool> JobChecked;
  vector<double> dvalue;
  int computed = 0;
  
  Metric* result = NULL;
  
  log->debug("There are "+itos(this->allBuckets->size())+" buckets in the table for check the metric",6);
  
  BucketSetOrdered::iterator it = this->allBuckets->begin();
  
  typedef pair<int,bool> pairBool;
  
  for(; it != this->allBuckets->end();++it)
  {
    AnalogicalBucket* bucket = *it;
    
    map<int,bool>::iterator isComputed = JobChecked.find(bucket->getjob()->getJobNumber());    
    
    if(isComputed == JobChecked.end())
    {
      //first time that the jobs is treated , so we just compute 
     switch(metricType)
     {
       case WAITTIME:
       {
         dvalue.push_back(bucket->getstartTime() - bucket->getjob()->getJobSimSubmitTime());
         break;
       }
       default: 
         assert(false); //shouldn't happend
      }
      
      JobChecked.insert(pairBool(bucket->getjob()->getJobNumber(),true));
    }
  }
  
  if(allocation != NULL)
  {
   log->debug("There are "+itos(allocation->allocations.size())+" in the allocation for check the metric",6);

  
   for(deque<AnalogicalBucket*>::iterator anit = allocation->allocations.begin();
       anit != allocation->allocations.end();anit++)
   {
    AnalogicalBucket* bucket = *anit;
    
    map<int,bool>::iterator isComputed = JobChecked.find(bucket->getjob()->getJobNumber());    
    
    if(isComputed == JobChecked.end())
    {
      //first time that the jobs is treated , so we just compute 
     switch(metricType)
     {
       case WAITTIME:
       {
         dvalue.push_back(bucket->getstartTime() - bucket->getjob()->getJobSimSubmitTime());
         break;
       }
       default: 
         assert(false); //shouldn't happend
      }
      
      JobChecked.insert(pairBool(bucket->getjob()->getJobNumber(),true));
    }   
   
   }
  }
  switch(statisticType)
  {
      case AVG:
      {
        Average* estimator = new Average();
        estimator->setValues(&dvalue); 
        result = estimator->computeValue();
        
        break;
      }
      default:
        assert(false);
        break;     
  }
  
  result->setType(metricType);
  
  return result;
}

/**
 * Given the buckets of the provided allocation checks that there is no overlapping between any of the already allocated buckets of the same processors. This is mainly done due to may happend that a given allocation, taking into account the penalties of all the jobs, overlaps to the already allocated buckets.
 * @param allocation The allocation to check 
 * @return A bool indicating if there is any overlapping.
 */
bool VirtualAnalogicalRTable::Overlapping(AnalogicalJobAllocation* allocation)
{
  //we've to check that there is no collosion for any of the buckets of the job 
  
  BucketSetOrdered allBucketsPlusAl;
  
  if(this->allBuckets->size() > 0)
  {
    allBucketsPlusAl = *this->allBuckets;
  }

  if(allocation != NULL)
    for(deque<AnalogicalBucket*>::iterator it = allocation->allocations.begin();it != allocation->allocations.end();++it)
    {
       allBucketsPlusAl.insert(*it);
    }
  
  
  for(BucketSetOrdered::iterator it = allBucketsPlusAl.begin();it != allBucketsPlusAl.end();++it)
  {
    AnalogicalBucket* bucket = *it;
    
    AnalogicalBucket* previousFreeBckt = this->findFreeBucketLowerBoundWithCpu(bucket->getcpu(),bucket);
    
    if(previousFreeBckt != NULL && previousFreeBckt->getpenalizedEndTime() > bucket->getstartTime())
    //there is an overlapping .. with the previous bucket
     return true;
  }
  
  return false;
  
}

/**
 * This function given a set of buckets (that should not be allocated to the reservation table) reduces its length according the provided job. This opperation is usually used when the policy needs to check how a given job would perform in the global schedule. The returned allocation should not be used to allocate the jobs, it is only temporal and would be used for check the performance of the allocation.
 * @param allocation The allocation to reduce 
 * @param job The job that is linked to the allocation 
 * @return A new allocation with the buckets of the original allocation but cut according to the job 
 */
AnalogicalJobAllocation* VirtualAnalogicalRTable::CutBuckets(AnalogicalJobAllocation* allocation,Job* job)
{
    
  
  /*this will be the real final allocation*/
  deque<AnalogicalBucket*> finalAllocation;
  
  for(deque<AnalogicalBucket*>::iterator it = allocation->allocations.begin();it != allocation->allocations.end();++it)
  {
    AnalogicalBucket* current = *it;
    
    assert(allocation->getstartTime() >= current->getstartTime() && 
            (allocation->getendTime() <= current->getendTime() || current->getendTime() == -1));
            
            
   AnalogicalBucket* newBucket = new AnalogicalBucket();
   
 
   newBucket->setCpu(current->getcpu());
   newBucket->setId(current->getId());
   newBucket->setNode(current->getnode());
   newBucket->setUsed(current->getused());  
   newBucket->setStartTime(allocation->getstartTime());
   newBucket->setEndTime(allocation->getendTime());
   
   //since the job has not been already assigned to the bucket .. we assign it to the temporal bucket.
   newBucket->setJob(job);
     
   finalAllocation.push_back(newBucket);
  }

  //we modify the proposed allocation from the allocation 
  //with the real allocation that has been computed right now 
  return new AnalogicalJobAllocation(finalAllocation,allocation->getstartTime(),allocation->getendTime());
}

/**
 * Reset the penalities for the provided allocation. 
 * @param allocation The allocation to be reset.
 */
void VirtualAnalogicalRTable::resetPenalties(AnalogicalJobAllocation* allocation,set<int>* OnlyNodes = NULL)
{ 
  /* we must find out if the current allocation buckets collates with any job -- using the assignament "=" operator */
  BucketSetOrdered allBucketsPlusA;
  
  if(this->allBuckets->size() > 0)
  {
    allBucketsPlusA = *this->allBuckets;
  }
  
  
  vector<ShadowSetOrdered*> collisions;
  
  log->debug("Resseting all the containers for all "+itos(this->architecture->getNumberNodes())+" nodes",6);
  
  //we put the buckets as we would have allocated them in case an allocation is provided
  if(allocation != NULL)
  {    
    for(deque<AnalogicalBucket*>::iterator it = allocation->allocations.begin();it != allocation->allocations.end();++it)
    {
      AnalogicalBucket* allocBucket = *it;
      allBucketsPlusA.insert(allocBucket);
    } 
  }
  /* we just have to check if the given allocation interfers with the rest of the buckets   
  */  
  for(BucketSetOrdered::iterator it = allBucketsPlusA.begin(); it != allBucketsPlusA.end();++it)
  {
    AnalogicalBucket* bucket = *it;
    int node = this->architecture->getNodeIdCpu(bucket->getcpu());
    
    if((OnlyNodes == NULL || OnlyNodes->find(node) != OnlyNodes->end()))
    {
      //in this case we resent the EndTime according to the job lenght
      //This value would be in case no penalties 
      if(bucket->getjob() != 0 )
        bucket->getjob()->setPenalizedRunTime(0);      

     bucket->setPenalizedTime(0);
      
    } 
  }
}



/**
 * This function evaluates the impact of allocating a given job in a given processors in the scheduling outcome contained in the virtual reservation table 
 * @param table the table where the allocation should be evaluated 
 * @param starttime When the allocation would start 
 * @param length The length of the current allocation 
 * @param job The job that will be allocated 
 * @param processor The processors where the job will be allocated 
 * @param maxPenalty Indicates the maximum penalty tolerated, if the allocation exceed this penalty the processors won't be reserved , otherwise a reservation is done by creating an allocation with the provided params for the job 
 * @param allocate Indicates if the allocation satisfies the criteria, the reservation has to be done in the table 
 * @return A virtual job indicating the penality that the job will experiment if it would be allocated in the specified circumstances. This virtual job is like the process representation for the job. Its id == -jobid - processor
 */
Job* VirtualAnalogicalRTable::EvaluateProcessorAllocation(VirtualAnalogicalRTable* table, double starttime, double length,Job* job,int processor,double maxPenalty,bool allocate,AnalogicalBucket* bucket)
{

  Job* virtualJob = new Job();
  *virtualJob = *job;    
  virtualJob->setJobNumber(-job->getJobNumber()-processor);
  virtualJob->setRequestedProcessors(1); 
  virtualJob->setNumberProcessors(1);
  table->insertTemporalProcess(virtualJob);
  
   /* Temporal allocation - we would evaluated the allocation as a single processors in the current reservation */
  AnalogicalJobAllocation* allocation =  new AnalogicalJobAllocation();
 
  allocation->setStartTime(starttime);
  allocation->setEndTime(starttime+length); 
    
  allocation->allocations.push_back(bucket);
  
  /* time to check what if */
  AnalogicalJobAllocation* allocationForCheckingRT = table->CutBuckets(allocation,virtualJob);
   
  //we compute the penality for this jobs according the realtime allocation 
  set<int> nodesToCheck;
  
  int node = this->architecture->getNodeIdCpu(processor);
  nodesToCheck.insert(node);
  
  table->resetPenalties(NULL,&nodesToCheck); 
  vector<ShadowSetOrdered*>  shadows = table->ComputeShadows(allocationForCheckingRT,NULL,&nodesToCheck);  
  //we compute the penalties that each shadows add to each job, and update the RTable
  table->ComputeShadowPenalties(&shadows);   
  table->deleteShadows(&shadows);
   
  freeAllocation(allocationForCheckingRT);
  //and set this value to the job 
  log->debug("The virtual process "+itos(processor) + "of the job " + " has a penalty of " +ftos((virtualJob->getRequestedTime() +  virtualJob->getPenalizedRunTime())/virtualJob->getRequestedTime())+ " and the maximum is "+ftos(maxPenalty),4);

  if(((virtualJob->getRequestedTime() +  virtualJob->getPenalizedRunTime())/virtualJob->getRequestedTime()<= maxPenalty || maxPenalty == -1) && allocate)
  {
    ((VirtualAnalogicalRTable*)table)->allocateJob(virtualJob,allocation);
    return virtualJob;  
  }
  
  //if not allocated we must free the associated allocation 
  delete allocation;

  table->freeTemporalProcess(virtualJob,false);   
  
  return NULL;
    
}

/**
 *  this function will return the allocation that tries to minimize the collisions between the allocated jobs. However, the allocation must satisify that any of the allocations has to exceed the provided  thresshold 
 * @param suitablebuckets A set of buckets that are suitable for the allocation, each analogical bucket is for one different processors 
 * @param length The duration required for the allocation 
 * @param numcpus The number of cpus required for the allocation 
 * @param initialShadow The point of time that the allocation is required 
 * @param job The job that will be allocated, its information will be used for compute what would occurs if it would be allocated 
 * @param thresshold the thresshold that indicates the maximum allowed penalty 
 * @return The allocation that matches the creiteria (if there is not allocation that matches the requirements it will be shown in the allocation problem). The returned allocation is a not real allocation. 
 */
AnalogicalJobAllocation* VirtualAnalogicalRTable::findLessThresholdConsumeMethodAllocation(BucketSetOrdered* suitablebuckets,double length,int numcpus,double initialShadow,Job* job,double thresshold)
{	
   
  list<AnalogicalBucket*> sharedShadow; 

  log->debug("Starting findLessThresholdConsumeMethodAllocation Allocation for the job"+itos(job->getJobNumber()),3);

  list<AnalogicalBucket*> lastValidSharedShadow; 
  double lastValidEndShadow = 0;
  double lastValidStartShadow = 0;

  double estimatedLastPenalty = -1;   

  list<Job*> sortSShadowPenalties; 
  
  double startShadow = initialShadow;
  double endShadow = initialShadow+length;
  int assignedcpus = 0;
     
  map<AnalogicalBucket*,Job*> assignedProcesses;
  map<Job*,AnalogicalBucket*> assignedBucket2Process;   
  
  
  //as the buckets may change over the time, we must copy the original buckets and search over the copy
  //but allocate accodring the original 

  BucketSetOrdered copySuitablebuckets;
  map<AnalogicalBucket*,AnalogicalBucket*> ApplicationCopyOriginal; 
 
  for(BucketSetOrdered::iterator i= suitablebuckets->begin(); i!=suitablebuckets->end(); ++i)
  {
    /* create the new bucket and copying it */
    AnalogicalBucket*  current = *i;
    AnalogicalBucket* copy = new AnalogicalBucket();
    *copy = *current;

    //we modify the bucket id for a new once, this is mainly for the strict order comparation when looking out for the orginal bucket when de allocation is found .. see the last part of the code. If it was not done, the wrong bucket would be found.

    copy->setId(-current->getId());

    assert(current->getstartTime() == copy->getstartTime());

    ApplicationCopyOriginal.insert(pair<AnalogicalBucket*,AnalogicalBucket*>(copy,current));
    copySuitablebuckets.insert(copy);

  }  

  VirtualAnalogicalRTable* temporalRT = this;
  
  AnalogicalJobAllocation* allocation = NULL;

  //this is current an optimization for the algorithm, before compute the penalty of a process in a given node
  //we will check if the node has been already checked , and if the penalty is bigger that the maxP computed (see below)
  //it's clear that we should take into account other issue such as start and end time of the allocation

  int numCpusChecked = 0;
	
  for(BucketSetOrdered::iterator i= copySuitablebuckets.begin(); i!=copySuitablebuckets.end(); ++i)
  {
    /*we have to compare with the first element */
    AnalogicalBucket*  current = *i;    
    Job* ProcessMaxP;
    list<Job*>::iterator maxP;    
    numCpusChecked++;


    int node = this->architecture->getNodeIdCpu(current->getcpu());    

    log->debug("Inspecting the node "+itos(node)+" - cpu "+itos(current->getcpu()),4);
    log->debug("findLessThresholdConsumeMethodAllocation: cheking the bucket CPU "+itos(current->getcpu())+" to the suitablebuckets list whom start time is "+ftos(current->getstartTime())+" and whom endtime is "+ftos(current->getendTime())+" and bucket id "+itos(current->getId()),6);
    
    /* for sanity */
     assert(assignedcpus >= 0);   
    
    /* Before do any stuff we will check is allocating the job to the current outcome would produce any collision */
    /* here we have to sent the original bucket rather the copy, if not , the memory pointers won't match on the allocation*/      
    AnalogicalBucket* theOriginalCurrent = ApplicationCopyOriginal[current];

    //the starttime to compute the penalty for the virtual allocations has to be the maximum of the bucket start time and the initial shadow used for the computation
    //this will allow to do not violate some checks that could trigger some assert false in the the EvaluatioProcessorAllocation, and will also allow to have a consistent view
    double vProcessStarttime = max(initialShadow,current->getstartTime());
    Job* virtualProcess = this->EvaluateProcessorAllocation(temporalRT,vProcessStarttime,length,job,current->getcpu(),thresshold,true,theOriginalCurrent);   

    //If the process is null means that the allocation of the process in the given cpu is higher that the thresshold
    if(virtualProcess == NULL)
    {
      log->debug("The penalty of this job is higher than the once specified in the thresshold "+ftos(thresshold),4);
      continue;
    }

    //setting the old penalized 
    virtualProcess->setOldPenalizedRunTime(virtualProcess->getPenalizedRunTime());

    
      
    if(virtualProcess != NULL)
    {
      /* the job processor has been allocated due to it has no penalty associated */
      log->debug("findLessThresholdConsumeMethodAllocation: The process "+itos(assignedcpus+1) +" the job "+itos(job->getJobNumber())+" has been allocated with the penalty "+ftos(virtualProcess->getPenalizedRunTime())+ " to the processor "+itos(current->getcpu())+" of the node"+itos(node)+", its process id is"+itos(virtualProcess->getJobNumber()),3);    
      assignedProcesses.insert(pair<AnalogicalBucket*,Job*>(current,virtualProcess));      
      assignedBucket2Process.insert(pair<Job*,AnalogicalBucket*>(virtualProcess,current));
      int nhnode = this->architecture->getNodeIdCpu(current->getcpu());
      
      //we insert the job to the set JobSetByPenalty 
      sortSShadowPenalties.push_back(virtualProcess);
    }
    else
    {
      log->debug("findLessThresholdConsumeMethodAllocation: The process "+itos(assignedcpus) +" the job "+itos(job->getJobNumber())+" has not been allocated to the processor, probably due it does not improves the overall penalty ",6 );    
      continue;
    }
    
    if(sharedShadow.size() != 0)
    {
      //we just have to check that the buckets of the vector shares the required window of time with the current 
      //bucket, be aware that by construction the buckets are ordered by they start time            
      for( list<AnalogicalBucket*>::iterator shbuckets = sharedShadow.begin();
           sharedShadow.end() != shbuckets;)
      {
        log->debug("findLessThresholdConsumeMethodAllocation: The size of the sharedShadow is "+itos(sharedShadow.size()),6);
      
        AnalogicalBucket* bucketShared = *shbuckets;
        
        
        //if the current bucket and the new processed bucket does not share the required period of time
        //we have to push popfront it .. otherwise.. by construction (ordered by startime) the rest of 
        //buckets shares the same period of time .. so we have finished
        
        log->debug("findLessThresholdConsumeMethodAllocation(: The difference is "+ftos(bucketShared->getendTime() - current->getstartTime()),6);
        log->debug("findLessThresholdConsumeMethodAllocation(: The difference with endShadow is "+ftos(bucketShared->getendTime() - startShadow),6);
        log->debug("findLessThresholdConsumeMethodAllocation: The startime for the already inserted bucket is "+ftos(bucketShared->getstartTime()),6);
        
        if(bucketShared->getendTime() - current->getstartTime() < length && 
           !(bucketShared->getendTime() == -1 && bucketShared->getendTime() == -1 )) //if both buckets are to infinity the share all the time..                      
        {
         //if the processes is removed form the allocation we must remove it from the assignedProcesses and unallocate from the virtual reservation table.  
          map<AnalogicalBucket*,Job*>::iterator procit = assignedProcesses.find(bucketShared);         
          assert(procit != assignedProcesses.end());
          Job* proc = procit->second;

          temporalRT->freeTemporalProcess(proc);
          
          assignedProcesses.erase(procit);
          
          //we also must remove it from the other hash 
          map<Job*,AnalogicalBucket*>::iterator  itBuckProc =  assignedBucket2Process.find(proc);
          assert(itBuckProc != assignedBucket2Process.end());
          assignedBucket2Process.erase(itBuckProc);

          //we must reomove it from the sortSShadowPenalties , we will use the find algorithm form the STL, however we won't provide a comparator, is it fine to find the bucket comparing the pointers values 
          list<Job*>::iterator itJobProc = find(sortSShadowPenalties.begin(),sortSShadowPenalties.end(),proc);
          sortSShadowPenalties.erase(itJobProc);


          log->debug("findLessThresholdConsumeMethodAllocation: UNselecting the bucket with id "+itos(bucketShared->getId()),6);
          //the shadow must be updated according to the fact that the initial bucket is deleted ..
          log->debug("findLessThresholdConsumeMethodAllocation: Removing the bucket with start time "+ftos(bucketShared->getstartTime()),6);
          sharedShadow.erase(shbuckets++);
          assignedcpus--;          
        }
        else
        {
          //the start shadow will be always the startime for the firts bucket in the shared shadow 
          if(startShadow < bucketShared->getstartTime())
            startShadow = bucketShared->getstartTime();
            
          endShadow = startShadow+length;  
                           
          ++shbuckets;
        }
      }
    }
      
    //initialize start the shadow according to this bucket .. the unique one at this moment
    //we must to take care that the bucket shadow is after the given limit
    if(startShadow < current->getstartTime())
    {
      startShadow = current->getstartTime();       
      endShadow = startShadow+length;

    }
    
    //this is only for debug 
    for(list<AnalogicalBucket*>::iterator debit = sharedShadow.begin();sharedShadow.end() != debit;++debit)
    {
      AnalogicalBucket* bucketShared = *debit;
      assert(bucketShared->getstartTime() <= startShadow);     
      assert(endShadow <= bucketShared->getendTime() || bucketShared->getendTime() == -1);
    }
    
    //we simple insert the bucket
    sharedShadow.push_back(current);
    log->debug("findLessThresholdConsumeMethodAllocation: Selecting the bucket with id "+itos(current->getId()),6);
    assignedcpus++;

    if(assignedcpus == numcpus)
    {
      break;
    }
  }
  
  
    
  allocation = new AnalogicalJobAllocation();
  
  list<AnalogicalBucket*>*   buckets = NULL;
    
   /*in the case that there are not enough buckets that satisties the given search criteria */
  if(assignedcpus == numcpus)
  {
    allocation->setStartTime(startShadow);
    allocation->setEndTime(endShadow);
    buckets = &sharedShadow;

    //we save again the maximum penalty for the job - but with out the last romved 
    Job* ProcessMaxP;
    list<Job*>::iterator maxP;   

    maxP = max_element(sortSShadowPenalties.begin(),
                        sortSShadowPenalties.end(), 
                        JobPenalty_lt_t());
    ProcessMaxP = *maxP; 
 
    job->setOldPenalizedRunTime(ProcessMaxP->getOldPenalizedRunTime());
  
  }else{

      //we've not found the appropiate allocation ..
      allocation->setAllocationProblem(true);
      allocation->setAllocationReason(NOT_ENOUGH_CPUS); 
  }
    

  //we free the resources that have been used for the computation 
  //we specify to free the processes used 
  //we rollback to the original scenario 
  temporalRT->freeProcessesAllocations();
   
  //In case there was an allocation problem the bucket stuff must 
  if(!allocation->getallocationProblem())
    for(list<AnalogicalBucket*>::iterator it = buckets->begin();it != buckets->end();++it)
    {
      AnalogicalBucket* copyBucket = (AnalogicalBucket*) *it;
      log->debug("findLessThresholdConsumeMethodAllocation: Selected the CPU "+itos(copyBucket->getcpu())+" from "+ftos(startShadow)+ " till "+ftos(endShadow),4);

      //we won't put the copyBucket bucket do to its a memory copy of the original, we have to find out again the original 
      //bucket and push it back to the allocations 

      AnalogicalBucket* theOriginalCurrent = temporalRT->findFirstBucketProcessor(copyBucket->getstartTime(), length, copyBucket->getcpu());

      log->debug("copy bucket startime "+ftos(copyBucket->getstartTime())+ " and bukcet id " +itos(copyBucket->getId())+ ", theOriginalCurrent startime "+ftos(theOriginalCurrent->getstartTime())+" and the bucket id "+itos(theOriginalCurrent->getId()),4 );

      assert(theOriginalCurrent->getjob() == NULL);
      assert(copyBucket->getstartTime() == theOriginalCurrent->getstartTime());
      assert(copyBucket->getendTime() == theOriginalCurrent->getendTime());

      allocation->allocations.push_back(theOriginalCurrent);
    }

  //finally we must free all the buckets that have been copied
  for(BucketSetOrdered::iterator i= copySuitablebuckets.begin(); i!=copySuitablebuckets.end(); ++i)
  {    
    AnalogicalBucket*  current = *i;    
    delete current;
  }
  
    
  return allocation;

}



/**
 *  this function will return the allocation that tries to minimize the collisions between the allocated jobs. Will return the N processors that would have less penalty in case the current job would be allocated.
 * @param suitablebuckets A set of buckets that are suitable for the allocation, each analogical bucket is for one different processors 
 * @param length The duration required for the allocation 
 * @param numcpus The number of cpus required for the allocation 
 * @param initialShadow The point of time that the allocation is required 
 * @param job The job that will be allocated, its information will be used for compute what would occurs if it would be allocated 
 * @return The allocation that matches the creiteria (if there is not allocation that matches the requirements it will be shown in the allocation problem). The returned allocation is a not real allocation. 
 */
AnalogicalJobAllocation* VirtualAnalogicalRTable::findLessConsumeMethodAllocation(BucketSetOrdered* suitablebuckets,double length,int numcpus,double initialShadow,Job* job)
{	
  list<AnalogicalBucket*> sharedShadow; 

  log->debug("Starting findLessConsumeMethod Allocation for the job"+itos(job->getJobNumber()),3);

  list<AnalogicalBucket*> lastValidSharedShadow; 
  double lastValidEndShadow = 0;
  double lastValidStartShadow = 0;
   
  list<Job*> sortSShadowPenalties; 
  
  double startShadow = initialShadow;
  double endShadow = initialShadow+length;
  int assignedcpus = 0;
  double estimatedLastPenalty = 0;
     
  map<AnalogicalBucket*,Job*> assignedProcesses;
  map<Job*,AnalogicalBucket*> assignedBucket2Process;   
  
  
  //as the buckets may change over the time, we must copy the original buckets and search over the copy
  //but allocate accodring the original 

  BucketSetOrdered copySuitablebuckets;
  map<AnalogicalBucket*,AnalogicalBucket*> ApplicationCopyOriginal; 
 
  for(BucketSetOrdered::iterator i= suitablebuckets->begin(); i!=suitablebuckets->end(); ++i)
  {
    /* create the new bucket and copying it */
    AnalogicalBucket*  current = *i;
    AnalogicalBucket* copy = new AnalogicalBucket();
    *copy = *current;

    //we modify the bucket id for a new once, this is mainly for the strict order comparation when looking out for the orginal bucket when de allocation is found .. see the last part of the code. If it was not done, the wrong bucket would be found.

    copy->setId(-current->getId());

    assert(current->getstartTime() == copy->getstartTime());

    ApplicationCopyOriginal.insert(pair<AnalogicalBucket*,AnalogicalBucket*>(copy,current));
    copySuitablebuckets.insert(copy);

  }  

  VirtualAnalogicalRTable* temporalRT = this;
  
  AnalogicalJobAllocation* allocation = NULL;

  //this is current an optimization for the algorithm, before compute the penalty of a process in a given node
  //we will check if the node has been already checked , and if the penalty is bigger that the maxP computed (see below)
  //it's clear that we should take into account other issue such as start and end time of the allocation

  int numCpusChecked = 0;
	
  for(BucketSetOrdered::iterator i= copySuitablebuckets.begin(); i!=copySuitablebuckets.end(); ++i)
  {
    /*we have to compare with the first element */
    AnalogicalBucket*  current = *i;    
    Job* ProcessMaxP;
    list<Job*>::iterator maxP;    
    numCpusChecked++;


    int node = this->architecture->getNodeIdCpu(current->getcpu());    

    log->debug("Inspecting the node "+itos(node)+" - cpu "+itos(current->getcpu()),4);
    log->debug("findLessConsumeMethodAllocation: cheking the bucket CPU "+itos(current->getcpu())+" to the suitablebuckets list whom start time is "+ftos(current->getstartTime())+" and whom endtime is "+ftos(current->getendTime())+" and bucket id "+itos(current->getId()),6);
    
    /* for sanity */
    assert(assignedcpus >= 0);   
    
    /* Before do any stuff we will check is allocating the job to the current outcome would produce any collision */
    /* here we have to sent the original bucket rather the copy, if not , the memory pointers won't match on the allocation*/      
    AnalogicalBucket* theOriginalCurrent = ApplicationCopyOriginal[current];

    Job* virtualProcess = this->EvaluateProcessorAllocation(temporalRT,current->getstartTime(),length,job,current->getcpu(),-1,true,theOriginalCurrent);    

    //setting the old penalized 
    virtualProcess->setOldPenalizedRunTime(virtualProcess->getPenalizedRunTime());

    
    AnalogicalBucket* toRemove = NULL; 
    
    if(assignedcpus == numcpus)
    {
     /*now its time to decide what to do ...
      two situations :
       1- The virtualProcess would has a lower penalty than the rest of the processes allocations for the current  allocation
          * We backup the last allocation    */

     //we compute the current penalty for those nodes where any of the allocated processes are running 
      maxP = max_element(sortSShadowPenalties.begin(),
                        sortSShadowPenalties.end(), 
                        JobPenalty_lt_t());
      
      for(list<Job*>::iterator itdeb = sortSShadowPenalties.begin(); 
         itdeb != sortSShadowPenalties.end() && this->log->getlevel() >= 3; ++itdeb)
      {
        Job* jobdeb = *itdeb;
        log->debug("Debugging stuff job "+itos(jobdeb->getJobNumber()) +" with penalty " +ftos(jobdeb->getOldPenalizedRunTime()),3);

      }
      
      ProcessMaxP = *maxP;      

      //in the case that the ProcessMaxP penalty is 0 means that the optimum allocation is found, so in this situation we return current allocation
      if(ProcessMaxP->getOldPenalizedRunTime() == 0)
      {
        //if the maximum penalty is 0 make no sense to find out more ...
        break; 
      }
       
      if(ProcessMaxP->getOldPenalizedRunTime() > virtualProcess->getPenalizedRunTime()) 
      {
       
        /* we save the last valid allocation parameters, perhaps the new allocation the new bucket that has less penality will produce an allocation with less allocated processors */
        lastValidSharedShadow = sharedShadow;
        lastValidStartShadow = startShadow;
        lastValidEndShadow = endShadow;
        
        
        /* we remove the process allocation with higer penalty of the the current sharedShadow  */
         //if the processes is removed form the allocation we must remove it from the assignedProcesses and unallocate from the virtual reservation table.            
        
        map<Job*,AnalogicalBucket*>::iterator  itBuckProc =  assignedBucket2Process.find(ProcessMaxP);
        assert(itBuckProc != assignedBucket2Process.end());
        AnalogicalBucket* theMaxBucket = itBuckProc->second;
        assignedBucket2Process.erase(itBuckProc);                          
        
        map<AnalogicalBucket*,Job*>::iterator procit = assignedProcesses.find(theMaxBucket);         
        assert(procit != assignedProcesses.end());
        assignedProcesses.erase(procit);
       
        //finally we must remove it from the set         
        list<Job*>::reverse_iterator itLast = sortSShadowPenalties.rbegin();
        Job* last = *itLast;	
        bool wasTheLast = last == ProcessMaxP;

        list<Job*>::iterator erased = sortSShadowPenalties.erase(maxP);        
        assert(wasTheLast || erased != sortSShadowPenalties.end());
        
        toRemove = theMaxBucket;
               
        temporalRT->freeTemporalProcess(ProcessMaxP);

  
        //we save again the maximum penalty for the job - but with out the last romved 
        //we have to consider what's up if there is only one job !
        if(sortSShadowPenalties.size()>0)
        {
          maxP = max_element(sortSShadowPenalties.begin(),
                          sortSShadowPenalties.end(), 
                          JobPenalty_lt_t());
          ProcessMaxP = *maxP; 
 
          estimatedLastPenalty = ProcessMaxP->getOldPenalizedRunTime();
        }
        else
          estimatedLastPenalty = virtualProcess->getPenalizedRunTime();

     }
       else
     {
       /*    2- The virtualProcess would have higer penalty than the rest of the processes allocations for the current  allocation, so we skip it and continou evaluationg the rest of buckets. However before  we must deallocate it from the virtual rtable */       
       temporalRT->freeTemporalProcess(virtualProcess);
       virtualProcess = NULL;

     }  
    }
      
    if(virtualProcess != NULL)
    {
      /* the job processor has been allocated due to it has no penalty associated */
      log->debug("findLessConsumeMethodAllocation: The process "+itos(assignedcpus+1) +" the job "+itos(job->getJobNumber())+" has been allocated with the penalty "+ftos(virtualProcess->getPenalizedRunTime())+ " to the processor "+itos(current->getcpu())+" of the node"+itos(node)+", its process id is"+itos(virtualProcess->getJobNumber()),3);    
      assignedProcesses.insert(pair<AnalogicalBucket*,Job*>(current,virtualProcess));      
      assignedBucket2Process.insert(pair<Job*,AnalogicalBucket*>(virtualProcess,current));
      int nhnode = this->architecture->getNodeIdCpu(current->getcpu());
      
      //we insert the job to the set JobSetByPenalty 
      sortSShadowPenalties.push_back(virtualProcess);
    }
    else
    {
      log->debug("findLessConsumeMethodAllocation: The process "+itos(assignedcpus) +" the job "+itos(job->getJobNumber())+" has not been allocated to the processor, probably due it does not improves the overall penalty ",6 );    
      continue;
    }
    
    if(sharedShadow.size() != 0)
    {
      //we just have to check that the buckets of the vector shares the required window of time with the current 
      //bucket, be aware that by construction the buckets are ordered by they start time            
      for( list<AnalogicalBucket*>::iterator shbuckets = sharedShadow.begin();
           sharedShadow.end() != shbuckets;)
      {
        log->debug("findLessConsumeMethodAllocation: The size of the sharedShadow is "+itos(sharedShadow.size()),6);
      
        AnalogicalBucket* bucketShared = *shbuckets;
        
        if(toRemove == bucketShared)
        {
          //the bucket has been touched for be removed, this may be due to it has a bigger penalty, so we must remove it from the list, we do this in this part of code for avoid unecessary computations 
          sharedShadow.erase(shbuckets++);
          assignedcpus--;
          continue;
        
        }
        
        //if the current bucket and the new processed bucket does not share the required period of time
        //we have to push popfront it .. otherwise.. by construction (ordered by startime) the rest of 
        //buckets shares the same period of time .. so we have finished
        
        log->debug("findLessConsumeMethodAllocation: The difference is "+ftos(bucketShared->getendTime() - current->getstartTime()),6);
        log->debug("findLessConsumeMethodAllocation: The difference with endShadow is "+ftos(bucketShared->getendTime() - startShadow),6);
        log->debug("findLessConsumeMethodAllocation: The startime for the already inserted bucket is "+ftos(bucketShared->getstartTime()),6);
        
        if(bucketShared->getendTime() - current->getstartTime() < length && 
           !(bucketShared->getendTime() == -1 && bucketShared->getendTime() == -1 )) //if both buckets are to infinity the share all the time..                      
        {
         //if the processes is removed form the allocation we must remove it from the assignedProcesses and unallocate from the virtual reservation table.  
          map<AnalogicalBucket*,Job*>::iterator procit = assignedProcesses.find(bucketShared);         
          assert(procit != assignedProcesses.end());
          Job* proc = procit->second;

          temporalRT->freeTemporalProcess(proc);
          
          assignedProcesses.erase(procit);
          
          //we also must remove it from the other hash 
          map<Job*,AnalogicalBucket*>::iterator  itBuckProc =  assignedBucket2Process.find(proc);
          assert(itBuckProc != assignedBucket2Process.end());
          assignedBucket2Process.erase(itBuckProc);

          //we must reomove it from the sortSShadowPenalties , we will use the find algorithm form the STL, however we won't provide a comparator, is it fine to find the bucket comparing the pointers values 
          list<Job*>::iterator itJobProc = find(sortSShadowPenalties.begin(),sortSShadowPenalties.end(),proc);
          sortSShadowPenalties.erase(itJobProc);

          log->debug("findLessConsumeMethodAllocation: UNselecting the bucket with id "+itos(bucketShared->getId()),6);
          //the shadow must be updated according to the fact that the initial bucket is deleted ..
          log->debug("findLessConsumeMethodAllocation: Removing the bucket with start time "+ftos(bucketShared->getstartTime()),6);
          sharedShadow.erase(shbuckets++);
          assignedcpus--;          
        }
        else
        {
          //the start shadow will be always the startime for the firts bucket in the shared shadow 
          if(startShadow < bucketShared->getstartTime())
            startShadow = bucketShared->getstartTime();
            
          endShadow = startShadow+length;  
                           
          ++shbuckets;
        }
      }
    }
      
    //initialize start the shadow according to this bucket .. the unique one at this moment
    //we must to take care that the bucket shadow is after the given limit
    if(startShadow < current->getstartTime())
    {
      startShadow = current->getstartTime();       
      endShadow = startShadow+length;

    }
    
    //this is only for debug 
    for(list<AnalogicalBucket*>::iterator debit = sharedShadow.begin();sharedShadow.end() != debit;++debit)
    {
      AnalogicalBucket* bucketShared = *debit;
      assert(bucketShared->getstartTime() <= startShadow);     
      assert(endShadow <= bucketShared->getendTime() || bucketShared->getendTime() == -1);
    }
    
    //we simple insert the bucket
    sharedShadow.push_back(current);
    log->debug("findLessConsumeMethodAllocation: Selecting the bucket with id "+itos(current->getId()),6);
    assignedcpus++;
  }
  
  
    
  allocation = new AnalogicalJobAllocation();
  
  list<AnalogicalBucket*>*   buckets = NULL;
    
   /*in the case that there are not enough buckets that satisties the given search criteria */
  if(assignedcpus == numcpus)
  {
    allocation->setStartTime(startShadow);
    allocation->setEndTime(endShadow);
    buckets = &sharedShadow;

    job->setOldPenalizedRunTime(estimatedLastPenalty);
  
  }else{
    if(lastValidSharedShadow.size() < numcpus)
    {
      //we've not found the appropiate allocation ..
      allocation->setAllocationProblem(true);
      allocation->setAllocationReason(NOT_ENOUGH_CPUS); 
    }

    //we save again the maximum penalty for the job - but with out the last romved 
    Job* ProcessMaxP;
    list<Job*>::iterator maxP;   

    maxP = max_element(sortSShadowPenalties.begin(),
                        sortSShadowPenalties.end(), 
                        JobPenalty_lt_t());
    ProcessMaxP = *maxP; 
 
    job->setOldPenalizedRunTime(ProcessMaxP->getOldPenalizedRunTime());

    allocation->setStartTime(lastValidStartShadow);
    allocation->setEndTime(lastValidEndShadow);
    buckets = &lastValidSharedShadow;  
  }
    

  //we free the resources that have been used for the computation 
  //we specify to free the processes used 
  //we rollback to the original scenario 
  temporalRT->freeProcessesAllocations();
   

  for(list<AnalogicalBucket*>::iterator it = buckets->begin();it != buckets->end();++it)
  {
    AnalogicalBucket* copyBucket = (AnalogicalBucket*) *it;
    log->debug("findLessConsumeMethodAllocation: Selected the CPU "+itos(copyBucket->getcpu())+" from "+ftos(startShadow)+ " till "+ftos(endShadow),4);

    //we won't put the copyBucket bucket do to its a memory copy of the original, we have to find out again the original 
    //bucket and push it back to the allocations 

    AnalogicalBucket* theOriginalCurrent = temporalRT->findFirstBucketProcessor(copyBucket->getstartTime(), length, copyBucket->getcpu());

    log->debug("copy bucket startime "+ftos(copyBucket->getstartTime())+ " and bukcet id " +itos(copyBucket->getId())+ ", theOriginalCurrent startime "+ftos(theOriginalCurrent->getstartTime())+" and the bucket id "+itos(theOriginalCurrent->getId()),4 );

    assert(theOriginalCurrent->getjob() == NULL);
    assert(copyBucket->getstartTime() == theOriginalCurrent->getstartTime());
    assert(copyBucket->getendTime() == theOriginalCurrent->getendTime());

    allocation->allocations.push_back(theOriginalCurrent);
  }    

  //finally we must free all the buckets that have been copied
  for(BucketSetOrdered::iterator i= copySuitablebuckets.begin(); i!=copySuitablebuckets.end(); ++i)
  {    
    AnalogicalBucket*  current = *i;    
    delete current;
  }
  
    
  return allocation;

}




/**
 * Returns the JobList
 * @return A reference containing the JobList
 */
map< int, Job * >* VirtualAnalogicalRTable::getJobList() const
{
  return JobList;
}

/**
 * Sets the JobList to the VirtualAnalogicalRTable
 * @param theValue The JobList
 */
void VirtualAnalogicalRTable::setJobList(map< int, Job * >* theValue)
{
  JobList = theValue;
}

/**
 * Returns the metricType
 * @return A metric_t indicating the metric optimized in the scheduling 
 */
metric_t VirtualAnalogicalRTable::getmetricType() const
{
  return metricType;
}

/**
 * Sets the metricType to the VirtualAnalogicalRTable
 * @param theValue The metricType
 */
void VirtualAnalogicalRTable::setMetricType(const metric_t& theValue)
{
  metricType = theValue;
}

/**
 * Returns the statisticType
 * @return A statistic_t indicating statistic for compute the metric optimized in the scheduling 
 */
statistic_t VirtualAnalogicalRTable::getstatisticType() const
{
  return statisticType;
}

/**
 * Sets the statisticType to the VirtualAnalogicalRTable
 * @param theValue The statisticType
 */
void VirtualAnalogicalRTable::setStatisticType(const statistic_t& theValue)
{
  statisticType = theValue;
}

/**
 * Returns the FreeBucketsOnJobCompletion
 * @return A bool containing the FreeBucketsOnJobCompletion
 */
bool VirtualAnalogicalRTable::getFreeBucketsOnJobCompletion() const
{
  return FreeBucketsOnJobCompletion;
}

/**
 * Sets the FreeBucketsOnJobCompletion to the VirtualAnalogicalRTable
 * @param theValue The FreeBucketsOnJobCompletion
 */
void VirtualAnalogicalRTable::setFreeBucketsOnJobCompletion(bool theValue)
{
  FreeBucketsOnJobCompletion = theValue;
}


}

