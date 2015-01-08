#include <scheduling/analogicalresertaviontable.h>
#include <utils/architectureconfiguration.h>
#include <utils/utilities.h>

#include <scheduling/schedulingpolicy.h>
#include <scheduling/simulation.h>


namespace Simulator {

/**
 * The default constructor for the class
 */
AnalogicalResertavionTable::AnalogicalResertavionTable()
{
}

/**
 * The default destructor for the class 
 */
AnalogicalResertavionTable::~AnalogicalResertavionTable()
{
  
  //we free the buckets 
  int check = this->globalView.size() - this->buckets.size()-  this->allocations.size();
  
  for(vector<BucketSetOrdered*>::iterator it = this->globalView.begin(); it != this->globalView.end();++it)
  {
    BucketSetOrdered* setBuckets= *it;
    delete setBuckets;
  
  }
  
  //we free all the free buckets 
  for(vector<BucketSetOrdered*>::iterator it = this->buckets.begin(); it != this->buckets.end();++it)
  {
    BucketSetOrdered* setBuckets= *it;
    
    for(BucketSetOrdered::iterator deb = setBuckets->begin();deb !=   setBuckets->end();++deb)
    { 
      AnalogicalBucket* debBuck = *deb;
      delete debBuck;
    }
    
    delete setBuckets;        
  }
  
  //we free the allocations 
  for(vector<BucketSetOrdered*>::iterator it = this->allocations.begin(); it != this->allocations.end();++it)
  {
    BucketSetOrdered* setBuckets= *it;
    
    for(BucketSetOrdered::iterator deb = setBuckets->begin();deb !=   setBuckets->end();++deb)
    { 
      AnalogicalBucket* debBuck = *deb;
      delete debBuck;
    }
    
    delete setBuckets;      
  }
  
  //now we free all the job allocations if present 
  for(map<Job*,JobAllocation*>::iterator it = JobAllocationsMapping.begin(); it != JobAllocationsMapping.end();++it)
  {
    JobAllocation* allocation = it->second;
    delete allocation;
  }
  
}

/**
 *  This is the constructor for the class. 
 *  BE AWARE ! IF SOMETHING CHANGES CHECK THAT THE operator= OF THE VIRTUAL RESERVATION TALBE IS NOT BEING AFECTED 
 *  MAINLY REFERENCES 
 *
 * @param configuration A reference to the configuration concerning the architecture
 * @param log A reference to the logging engine 
 * @param globaltime The globaltime of the simulation
 */
AnalogicalResertavionTable::AnalogicalResertavionTable(ArchitectureConfiguration* configuration,Log* log, double globaltime) : ReservationTable(configuration,log,globaltime) 
{

  
  /* first we get the architecture .. to go through all the processors */

  ArchitectureConfiguration* architecture = this->getarchitecture(); 
  this->lastBucketId = 0;

  /* this value should never be null and there always have to be cpus*/
  assert(architecture != 0 && architecture->getNumberCpus() > 0);

  /* creating the sets  */
  for(int i = 0; i <  architecture->getNumberCpus();i++)
  {    
   /* the free buckets */
    BucketSetOrdered* freeBuckets = new BucketSetOrdered();
    AnalogicalBucket* free = new AnalogicalBucket(globaltime,-1);
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
  for(int i = 0; i < configuration->getNumberCpus();i++)
  {
    this->CPUMapping.push_back(0);
  }  
}

/**
 * 
 * @param bucketSet A reference to the BucketSetOrdered with a set of buckets.
 * @param current A reference to the reference bucket 
 * @return Returns the last bucket (in the ordered set) that has a lower startime that the reference bucket 
 */
AnalogicalBucket* AnalogicalResertavionTable::findLowerBound(BucketSetOrdered* bucketSet,AnalogicalBucket* current)
{
  BucketSetOrdered::iterator itfreeb = bucketSet->begin();
     
  AnalogicalBucket* freeLowerBucket = NULL;
  
  for(;itfreeb != bucketSet->end();++itfreeb)
    if(((AnalogicalBucket*)*itfreeb)->getstartTime() < current->getstartTime())
      freeLowerBucket = (AnalogicalBucket*)*itfreeb;
    else
      break;
      
  return freeLowerBucket;    
  
  
}

/**
 * 
 * @param bucketSet A reference to the BucketSetOrdered with a set of buckets.
 * @param current A reference to the reference bucket 
 * @return Returns the first bucket (in the ordered set) that has the startime bigger thant the reference bucket 
 */
AnalogicalBucket* AnalogicalResertavionTable::findUpperBound(BucketSetOrdered* bucketSet,AnalogicalBucket* current)
{
  //may be the the usage of lower bound seems rarely here but
  //looking at the STL reference one can find the follow definition 
  //iterator lower_bound(const key_type& k) const -->	Finds the first element whose key is not less than k.
  BucketSetOrdered::iterator itfreeb = bucketSet->lower_bound(current); 
      
  //however we would like to return the following one !! not the same current bucket
  //so we iterate ..
  AnalogicalBucket * found = 0;
  
  for(;itfreeb != bucketSet->end();++itfreeb)
  {
    found = *itfreeb;
  
    if(found != current)
      break;
  }
  
  return (found != current ? found : 0 );
}

/**
 * Inherited from reservation table 
 * @see the reservation table class 
 * BE AWARE !!! IF A BUG IS  IN THIS METHOD PLEASE FIX THE SAME BUG AT THE CUTBUCKETS METHOD OF THE 
 * VIRTUAL RESERVATION TABLE !!!
 * @param job The job to be allocated to the reservation table 
 * @param genericallocation The job allocation that has to be allocated 
 * @return True is the job has been correctly allocated 
 */
bool  AnalogicalResertavionTable::allocateJob(Job* job,JobAllocation* genericallocation)
{
  

  /*it's time to convert it to analogic reservation*/
  AnalogicalJobAllocation* allocation = (AnalogicalJobAllocation*) genericallocation;

  /* it's time to allocate the job to the reservation table !*/    
  deque<AnalogicalBucket*>::iterator it = allocation->allocations.begin();  
  
  /*this will be the real final allocation*/
  deque<AnalogicalBucket*> finalAllocation;
  
  for(;it != allocation->allocations.end();++it)
  {
    AnalogicalBucket* current = *it;
    int cpuCurrent = current->getcpu();
    
    /* first we have to check weather this bucket has to be all used ..
       or it must be splitted in two or more parts , the worst case would be
       ___________________________________________________________________________________
      |_____b1_not used _________|_____used __b2_________________|_ not used_____b3_______|
      ^-- current->startTime     ^-- allocation.startTime        ^--allocation.endtime    ^-- current->endTime
        
      in this case (A) b1 must be added to the free buckets (may be join to the previous free bucket)
      (B) b2 must be insert in the allocations.  
      (C) b3 must be added to the free buckets (may be joint to the next free bucket)
      
      BE AWARE THAT: the current bucket is shared with the globalview and the buckets table !
      
      First we chekc if the condition (A) occurs:
    */
        
    /*for sanity */
    assert(allocation->getstartTime() >= current->getstartTime() && 
            (allocation->getendTime() <= current->getendTime() || current->getendTime() == -1));
    
    /* DEBUG*/
    this->dumpBucketSet(allocations[current->getcpu()]);
    this->dumpBucketSet(globalView[current->getcpu()]);
    this->dumpBucketSet(buckets[current->getcpu()]);
    this->checkBucketSet(globalView[current->getcpu()]);
     
    /* before do any stuff with the buckets we will find the upper and lower buckets */ 
    AnalogicalBucket* nextFreeBckt = this->findUpperBound(buckets[current->getcpu()],current);
    AnalogicalBucket* previousFreeBckt = this->findLowerBound(buckets[current->getcpu()],current);
            
    if(current->getstartTime() < allocation->getstartTime())
    {
      /*we must add the already free period of time to the list of free buckets ())*/
      double newBucketStartTime =  current->getstartTime();
      double newBucketEndTime = allocation->getstartTime()-1;
      
      /* for sanity */
      assert(newBucketStartTime <= newBucketEndTime);  
      
      if(previousFreeBckt != 0 &&  //not null
         previousFreeBckt->getendTime() > newBucketStartTime /*can be joint*/)
      {
        //we increment the previousFreeBckt -- 
        previousFreeBckt->setEndTime(newBucketEndTime);
      }
      else
      {
      	//we add a new bucket
      	AnalogicalBucket* newBucket = new AnalogicalBucket(newBucketStartTime,newBucketEndTime);
      	newBucket->setUsed(false);
      	newBucket->setId(++this->lastBucketId);
      	newBucket->setCpu(current->getcpu());      	
      	
      	//updating the free buckets structure and the globalview
      	buckets[current->getcpu()]->insert(newBucket);
      	globalView[current->getcpu()]->insert(newBucket);
      }
     }
      
     /* Second first the if the condiftion (C) occures ) - remember that this can be the infinity bucket */ 
     if(current->getendTime() > allocation->getendTime() || current->getendTime() == -1 )
     {
       /*we must add the already free period of time to the list of free buckets ())*/
      double newBucketStartTime =  allocation->getendTime()+1;
      double newBucketEndTime =  current->getendTime();
      
      /* for sanity */
      assert(newBucketStartTime <= newBucketEndTime || newBucketEndTime == -1);
      
      /*we have to find out if this time can be added to the previous free bucket */                 
      if(current->getendTime() == -1)        
        //this bucket is the last one 
        nextFreeBckt = current;
      
      
      if(nextFreeBckt != 0 &&  //not null
         newBucketEndTime > nextFreeBckt->getstartTime() || 
         newBucketEndTime == -1 /*can be joint*/)
      {
        //we increment the previousFreeBckt --         
        this->updateStartTimeFreeBucket(nextFreeBckt,newBucketStartTime);
      }
      else
      {
        //we add a new bucket ..
      	AnalogicalBucket* newBucket = new AnalogicalBucket(newBucketStartTime,newBucketEndTime);
      	newBucket->setUsed(false);
      	newBucket->setId(++this->lastBucketId);
      	newBucket->setCpu(current->getcpu());
      	
      	//updating the free buckets structure and the globalview
      	buckets[current->getcpu()]->insert(newBucket);
      	globalView[current->getcpu()]->insert(newBucket);
      }
     }        
     
     /* now it's time to carry create the allocated bucket !*/
    
     //we add a new bucket ..
     AnalogicalBucket* newBucket = new AnalogicalBucket(allocation->getstartTime(),
                                                        allocation->getendTime());    
     
     newBucket->setUsed(true);
     newBucket->setCpu(current->getcpu());
     newBucket->setId(++this->lastBucketId);
     newBucket->setJob(job);
     
     //updating the free buckets structure and the globalview
     //first we remove the current bucket from the allocations and globalview

     BucketSetOrdered::iterator old = buckets[current->getcpu()]->find(current);
     AnalogicalBucket* oldbucket = *old;
     
     //in the case that the old one was to till -1 (means infinity)
     //we do not have to erase it, due to it's the same as the current !
     if(oldbucket->getendTime() != -1)
     {   
       /*for sanity , we must remove the proper bucket */
       assert(oldbucket->getId() == current->getId());     
       buckets[current->getcpu()]->erase(old);
     
       /*for sanity , we must remove the proper bucket */
       old = globalView[current->getcpu()]->find(current);
       oldbucket = *old;
     
       /*for sanity , we must remove the proper bucket */
       assert(oldbucket->getId() == current->getId());     
       globalView[current->getcpu()]->erase(old);
     
       //finally we free the memory used for this old bucket
       delete oldbucket;
     }
     
     
     //we insert the bucket to the allocations and globalview          
     allocations[newBucket->getcpu()]->insert(newBucket);
     globalView[newBucket->getcpu()]->insert(newBucket);
      
     
    /* DEBUG*/
    this->dumpBucketSet(allocations[cpuCurrent]);
    this->dumpBucketSet(globalView[cpuCurrent]);
    this->checkBucketSet(globalView[cpuCurrent]);

     
     finalAllocation.push_back(newBucket);
  }

  //we modify the proposed allocation from the allocation 
  //with the real allocation that has been computed right now 
  allocation->setAllocations(finalAllocation);
  
  //for sanity 
  map<Job*,JobAllocation*>::iterator itcheck = this->JobAllocationsMapping.find(job);
  assert(itcheck == this->JobAllocationsMapping.end());
  
  //adding the mapping for the job allocation .. then we will be able to kill or finish it   
  this->JobAllocationsMapping.insert(pairJobAlloc(job,allocation));
  
  return true;

}

/**
 * function created for debugging, dumps the current buckets in a set of buckets
 * @param setBuckets The set of buckets to be dump 
 */
void AnalogicalResertavionTable::dumpBucketSet(BucketSetOrdered* setBuckets)
{                    
  if(log->getlevel() >= 6)
  {
    log->debug("The size of the buckets for the cpu is "+itos(setBuckets->size()),6);  
    for(BucketSetOrdered::iterator deb = setBuckets->begin();deb !=   setBuckets->end();++deb)
    { 
      AnalogicalBucket* debBuck = *deb;
      log->debug("Bucked id "+itos(debBuck->getId())+ " statTime "+ftos(debBuck->getstartTime())+" endTime "+ftos(debBuck->getendTime())  ,6); 
    }
  }
}



/**
 * for asserting that everything is fine in the reservationtable. it basically checks that no free buckets are continous (due to in this case the buckets should be joint) and that two continous buckets are really continous (what means that the endtime for the previous+1 is the same as the startime for the next)
 * @param setBuckets  The set of buckets to check.
*/

void AnalogicalResertavionTable::checkBucketSet(BucketSetOrdered* setBuckets)
{                    
  if(log->getlevel() >= 2)
  {
    bool lastUsed = false; 
    double lastEndTime = -1;
    AnalogicalBucket* last = NULL;
    
    for(BucketSetOrdered::iterator deb = setBuckets->begin();deb !=   setBuckets->end();++deb)
    { 
      AnalogicalBucket* debBuck = *deb;
      assert(last == NULL || (!(debBuck->getused() == false && lastUsed == false) && lastEndTime+1==debBuck->getstartTime()));
      lastUsed = debBuck->getused();
      lastEndTime = debBuck->getendTime();
      last = debBuck;
    }
  }
}


/**
 * this functions allows to modify safely the startime of a bucket that has been already inserted in a BucketSetOrdered 
 * (see the note of the header file concerning the strict order of the function)
 * @param bucket The bucket to whom the startime has to be modified 
 * @param startTime The new startime
 */
void AnalogicalResertavionTable::updateStartTimeFreeBucket(AnalogicalBucket* bucket,double startTime)
{
  bool error = buckets[bucket->getcpu()]->erase(bucket);
  assert(error);
  error = globalView[bucket->getcpu()]->erase(bucket);
  assert(error);
  
  bucket->setStartTime(startTime);
  
  error = (buckets[bucket->getcpu()]->insert(bucket)).second;
  assert(error);
  error = (globalView[bucket->getcpu()]->insert(bucket)).second;
  assert(error);
}

/**
 * This function reduces the runtime of a given job that has been allocated in the reservation table, this is used in situations where a given job was suppoed to run X and the scheduler has realized that it has run X-alpha, and due to the scheduler algorithm the reservationtable has to be updated
 * @param job The job whom allocation has to be updated 
 * @param length The lenght that has to be reduced 
 * @return True if no problems have raised 
 */

bool AnalogicalResertavionTable::reduceRuntime(Job* job,double length)
{
   map<Job*,JobAllocation*>::iterator allocIter = this->JobAllocationsMapping.find(job);
   AnalogicalJobAllocation* allocation = (AnalogicalJobAllocation*) allocIter->second;
   
   assert(allocation->getendTime()-length > 0);
   allocation->setEndTime(allocation->getendTime()-length);
   
   assert(allocation != 0);
   assert(length > 0);
      
   for(deque<AnalogicalBucket*>::iterator it = allocation->allocations.begin();
       it != allocation->allocations.end();++it)
   {
     AnalogicalBucket* current = *it;
     
     
     /* for sanity*/
     assert(current != 0);
     assert(current->getjob()->getJobNumber() == job->getJobNumber());
     assert(current->getendTime()-current->getstartTime()-length > 0); //the job allocation can not be reduced till zero
       
     /* we find the upper bucket to check if it can be joint to the slot of time that its is not used
        or we must to create a new free bucket and add it to the freeBuckets 
     */
     AnalogicalBucket* freeUpperBucket = this->findUpperBound(this->buckets[current->getcpu()],current);
     
     
     if(current->getendTime()+1 == freeUpperBucket->getstartTime())
       //the freed slot of time can be joined to the next free bucket 
       freeUpperBucket->setStartTime(freeUpperBucket->getstartTime()-length);
     else
     {
        //the slot can not be joined ot the next free bucket .. so we must create a new one 
        //and add it to the free buckets 
        AnalogicalBucket* newBucket = new AnalogicalBucket(current->getendTime()-length+1,current->getendTime());
      	newBucket->setUsed(false);
      	newBucket->setId(++this->lastBucketId);
      	newBucket->setCpu(current->getcpu());      	
      	
      	//updating the free buckets structure and the globalview
      	buckets[current->getcpu()]->insert(newBucket);
      	globalView[current->getcpu()]->insert(newBucket);

     }
     
     current->setEndTime(current->getendTime()-length);
     
   }
   

   return true;

}
/* 
   
   
*/

/**
 * This function extends the runtime of an allocation of a given with the specified length
 * in this case a precondition is that there are no used buckets after the current bucket  this function must be called 
 * @param job The job to whom the job runtime must be extended 
 * @param length The lenght of the extension 
 * @return True if the extension has not raised any problem 
*/

bool AnalogicalResertavionTable::extendRuntime(Job* job,double length)
{
   //we get the allocation and free it from the JobAllocationMapping 
   map<Job*,JobAllocation*>::iterator allocIter = this->JobAllocationsMapping.find(job);
   AnalogicalJobAllocation* allocation = (AnalogicalJobAllocation*) allocIter->second;
   
   assert(allocation != 0);
   
   
   for(deque<AnalogicalBucket*>::iterator it = allocation->allocations.begin();
       it != allocation->allocations.end();++it)
   {
     AnalogicalBucket* current = *it;
     
     
     /* for sanity*/
     assert(current != 0);
     assert(current->getjob()->getJobNumber() == job->getJobNumber());
       
     AnalogicalBucket* freeUpperBucket = this->findUpperBound(this->buckets[current->getcpu()],current);
     
     assert(freeUpperBucket->getstartTime()-1 == current->getendTime() &&
        (freeUpperBucket->getendTime() > freeUpperBucket->getstartTime()+length || freeUpperBucket->getendTime() == -1)
     );
     
     freeUpperBucket->setStartTime(freeUpperBucket->getstartTime()+length);
     current->setEndTime(current->getendTime()+length);
     
   }
   
   //now its time to update the allocation 
   allocation->setEndTime(allocation->getendTime()+length); 
   
   return true;
}

/**
 * this function deallocates a given job, that has been allocated to the reservation table. The allocation can be for a job that is running or for a job that is still queued in the queue waitting for its startime.
 * @param job The job to deallocate 
 * @return True if the job has succesfully deallocated
*/

bool AnalogicalResertavionTable::deAllocateJob(Job* job)
{
   map<Job*,JobAllocation*>::iterator allocIter = this->JobAllocationsMapping.find(job);
   assert(allocIter != this->JobAllocationsMapping.end());
   AnalogicalJobAllocation* allocation = (AnalogicalJobAllocation*) allocIter->second;
   this->JobAllocationsMapping.erase(allocIter);
   
   assert(allocation != 0);
   
   /*
     the steps to do are the following:
     1- Free the buckets used by this job in each cpu in the allocations buckets 
     2- Free the same buckets from the global view
     3- Add the buckets to the free area and update the global view               
   */
 
   for(deque<AnalogicalBucket*>::iterator it = allocation->allocations.begin();
       it != allocation->allocations.end();++it)
   {
     AnalogicalBucket* current = *it;
     
     
     /* for sanity*/
     assert(current != 0);
     assert(current->getjob()->getJobNumber() == job->getJobNumber());
     
     /* We have to find out if there are buckets that are contigous to it 
       and we will fussion both, of we just have to add a free bucket. In the global view rtable may see
       ___________________________________________________________________________________
      |_____b1_not used _________|_____current __b2______________|___ used______b3_______|   (A)
      or 
      ___________________________________________________________________________________
      |_____b1_not used _________|_____current __b2______________|__not_ used______b3____|  (B)
      or
      ___________________________________________________________________________________
      |_____b1_ used ___________|_____current __b2______________|__not_ used____b3_______| (C)
      or
      ___________________________________________________________________________________
      |_____b1__ used __________|_____current __b2_______________|___ used______b3_______| (D)
       
     */
     /* DEBUG*/
     int cpu = current->getcpu();
     this->dumpBucketSet(allocations[current->getcpu()]);     
     this->dumpBucketSet(globalView[current->getcpu()]);
     this->dumpBucketSet(buckets[current->getcpu()]);
     this->checkBucketSet(globalView[current->getcpu()]);

          
     AnalogicalBucket* freeUpperBucket = this->findUpperBound(this->buckets[current->getcpu()],current);
     
     /* the lower bucket may not exists.. since in the first job allocation the running buckets
        only would have an upper bucket
     */
     assert(freeUpperBucket != 0);
     
     AnalogicalBucket* freeLowerBucket = this->findLowerBound(this->buckets[current->getcpu()],current);
     
     bool fussion = false; /*if true we just will delete the current bucket*/     
          
     if(freeLowerBucket != 0)
     {
       /* We just have to check if we are in (A) - but only checking the left side */  
       if(freeLowerBucket->getendTime()+1 >= current->getstartTime() )
       {
          /* remeber that the set ordered by the startTime so by construction 
             the current bucket must be joint tho the freeLowerBucket
             
             we must join the bucket extending it - b1 and b2 are fussioned
          */
          
          freeLowerBucket->setEndTime(current->getendTime());
          fussion = true; 
       }             
       
       /* now we chek if the situation (B) occurs */
       if(current->getendTime()+1>=freeUpperBucket->getstartTime() && fussion)
       {
         /*extending the lower bucket and deleting b3           
         */
         freeLowerBucket->setEndTime(freeUpperBucket->getendTime());
         
         /*time to delete upperbucket and current from the globalview , buckets and allocation */
         this->buckets[current->getcpu()]->erase(freeUpperBucket);
         this->globalView[current->getcpu()]->erase(freeUpperBucket);
         
         this->allocations[current->getcpu()]->erase(current);
         this->globalView[current->getcpu()]->erase(current);
         
         delete current;
         delete freeUpperBucket;
         /*situation (B) fixes .. we can return*/
         this->checkBucketSet(globalView[cpu]);
         continue;
       }
     }
     
     /*now we check if we have to (C)*/
     if(current->getendTime()+1>=freeUpperBucket->getstartTime())
     {
       /*just extent the upper bucket*/
       this->updateStartTimeFreeBucket(freeUpperBucket,current->getstartTime());
       fussion = true;
     }
     
     if(fussion)
     {
       /*at this point we know that the situation (C) or (A) has veen detected .. so we just have
       to delete the current bucket */
       this->allocations[current->getcpu()]->erase(current);
       this->globalView[current->getcpu()]->erase(current);
       
       delete current;
     }
     else
     {
        /* there are no continous free buckets .. so we just have to move this bucket to free */
        this->allocations[current->getcpu()]->erase(current);        
        
        /*reset the bucket to free and insert it to the free bucket list*/
        int usedcpu = current->getcpu();
        current->deallocateJob();
        
        /*reinsert*/
        assert(usedcpu	 >= 0);
        this->buckets[usedcpu]->insert(current);

     }
     
         /* DEBUG*/
    this->dumpBucketSet(allocations[cpu]);
    this->dumpBucketSet(globalView[cpu]);
    this->dumpBucketSet(buckets[cpu]);
    this->checkBucketSet(globalView[cpu]);

   }      
   
   //we free the allocation , if other classes are using it they should copy in its own space
   delete allocation;   
   
}

/**
 * If a job has been killed this function may be invoqued. Actually it is not used but defined.
 * @param job The job that has been killed 
 * @return True if no problems hava rised
 */
bool AnalogicalResertavionTable::killJob(Job* job)
{
  /* maybe with deallocate is enough?*/
}




//RUTGERS
/**
 *  Extended version of FIRST-FIT for Datacenters (towards autonomic resource provisioning)
 * @param suitablebuckets A set of buckets that are suitable for the allocation, each analogical bucket is for one different processors 
 * @param length The duration required for the allocation 
 * @param numcpus The number of cpus required for the allocation 
 * @param initialShadow The point of time that the allocation is required 
 * @return The allocation that matches the creiteria (if there is not allocation that matches the requirements it will be shown in the allocation problem). The returned allocation is a not real allocation. 
 */
AnalogicalJobAllocation* AnalogicalResertavionTable::findDcFirstVmAllocation(BucketSetOrdered* suitablebuckets,double length,int numcpus,double initialShadow, int type, SchedulingPolicy* sched)//PowerConsumption* power)
{		  
  list<AnalogicalBucket*> sharedShadow; 
  double startShadow = initialShadow;
  double endShadow = initialShadow+length;
  int assignedcpus = 0;
  AnalogicalJobAllocation* allocation;
	
  int reconfigure = 0;


/*
MareNostrum* mn = ((MNConfiguration*) this->architecture)->getmarenostrum();

int kk1 = mn->getTotalNodes();
int kk2 = mn->getTotalBlades();
int kk3 = mn->getTotalCPUS();

Node *node1 = mn->getNodeWithGlobalId(1);
Node *node2 = mn->getNodeWithGlobalId(6);
*/

BucketSetOrdered sameTypeBuckets;

MareNostrum* mn = ((MNConfiguration*) this->architecture)->getmarenostrum();

  for(BucketSetOrdered::iterator i= suitablebuckets->begin(); i!=suitablebuckets->end(); ++i)
  {
    /*we have to compare with the first element */
    AnalogicalBucket*  current = *i;

    int cpu = current->getcpu();
    Node *node = mn->getNodeWithGlobalId(mn->getNodeIdCpu(cpu));

    // Create a list with the buckets belonging to nodes of the requested type
    if(node->getNetworkBW() == type){
	sameTypeBuckets.insert(current);
    }
    //Create a list with buckets belonging to nodes with the requested memory config
    //Create a list with buckets belonging to nodes of the requested type and with the requested memory config
  }


  if(sameTypeBuckets.size()>=numcpus){

  	// FIRST APPROACH: follow regular FCFS approach with the buckets in nodes of same type/class

     for(BucketSetOrdered::iterator i=sameTypeBuckets.begin(); i!=sameTypeBuckets.end(); ++i)
  	{
    	//we have to compare with the first element 
    	AnalogicalBucket*  current = *i;

    	log->debug("findDcFirstVmAllocation: cheking the bucket CPU "+itos(current->getcpu())+" to the suitablebuckets list whom start time is "+ftos(current->getstartTime())+" and whom endtime is "+ftos(current->getendTime())+" and bucket id "+itos(current->getId()),6);
    
   	 // for sanity 
  	  assert(assignedcpus >= 0);
    
    	if(assignedcpus == numcpus)
     	 //the required allocation has been found ..
      	break;
        
    
    	if(sharedShadow.size() != 0)
   	 {
     	 //we just have to check that the buckets of the vector shares the required window of time with the current 
      	//bucket, be aware that by construction the buckets are ordered by they start time            
      	for( list<AnalogicalBucket*>::iterator shbuckets = sharedShadow.begin();
       	    sharedShadow.end() != shbuckets;)
     	 {
      	  log->debug("The size of the sharedShadow is "+itos(sharedShadow.size()),6);
      
     	   AnalogicalBucket* bucketShared = *shbuckets;
      	  //if the current bucket and the new processed bucket does not share the required period of time
      	  //we have to push popfront it .. otherwise.. by construction (ordered by startime) the rest of 
       	 //buckets shares the same period of time .. so we have finished
        
       	 log->debug("The difference is "+ftos(bucketShared->getendTime() - current->getstartTime()),6);
      	  log->debug("The difference with endShadow is "+ftos(bucketShared->getendTime() - startShadow),6);
       	 log->debug("The startime for the already inserted bucket is "+ftos(bucketShared->getstartTime()),6);
        
       	 if(bucketShared->getendTime() - current->getstartTime() < length && 
       	    !(bucketShared->getendTime() == -1 && bucketShared->getendTime() == -1 )) //if both buckets are to infinity the share all the time..                      
       	 {
        	  //the shadow must be updated according to the fact that the initial bucket is deleted ..
       	   log->debug("Removing the bucket with start time "+ftos(bucketShared->getstartTime()),6);
       	   sharedShadow.erase(shbuckets++);
       	   assignedcpus--;
          
       	   log->debug("FindFastMethodAllocation: UNselecting the bucket with id "+itos(bucketShared->getId()),6);
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
      }//if (sharedShadow.size() != 0)
      
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
      log->debug("FindFastMethodAllocation: Selecting the bucket with id "+itos(current->getId()),6);
      assignedcpus++;

    }

  } //sametypeBuckets>=numcpus
  else{ //sameTypeBuckets<numcpus
	/////////////////// RECONFIGURATION!!!!!!!!!!!!!

	reconfigure = 1;

	cout<<"RECONFIGURATION WILL BE!!"<<endl;

	// follow regular FCFS approach and then reconfigure selected nodes!

     for(BucketSetOrdered::iterator i=suitablebuckets->begin(); i!=suitablebuckets->end(); ++i)
  	{
    	//we have to compare with the first element 
    	AnalogicalBucket*  current = *i;

    	log->debug("findDcFirstVmAllocation: cheking the bucket CPU "+itos(current->getcpu())+" to the suitablebuckets list whom start time is "+ftos(current->getstartTime())+" and whom endtime is "+ftos(current->getendTime())+" and bucket id "+itos(current->getId()),6);
    
   	 // for sanity 
  	  assert(assignedcpus >= 0);
    
    	if(assignedcpus == numcpus)
     	 //the required allocation has been found ..
      	break;
        
    
    	if(sharedShadow.size() != 0)
   	 {
     	 //we just have to check that the buckets of the vector shares the required window of time with the current 
      	//bucket, be aware that by construction the buckets are ordered by they start time            
      	for( list<AnalogicalBucket*>::iterator shbuckets = sharedShadow.begin();
       	    sharedShadow.end() != shbuckets;)
     	 {
      	  log->debug("The size of the sharedShadow is "+itos(sharedShadow.size()),6);
      
     	   AnalogicalBucket* bucketShared = *shbuckets;
      	  //if the current bucket and the new processed bucket does not share the required period of time
      	  //we have to push popfront it .. otherwise.. by construction (ordered by startime) the rest of 
       	 //buckets shares the same period of time .. so we have finished
        
       	 log->debug("The difference is "+ftos(bucketShared->getendTime() - current->getstartTime()),6);
      	  log->debug("The difference with endShadow is "+ftos(bucketShared->getendTime() - startShadow),6);
       	 log->debug("The startime for the already inserted bucket is "+ftos(bucketShared->getstartTime()),6);
        
       	 if(bucketShared->getendTime() - current->getstartTime() < length && 
       	    !(bucketShared->getendTime() == -1 && bucketShared->getendTime() == -1 )) //if both buckets are to infinity the share all the time..                      
       	 {
        	  //the shadow must be updated according to the fact that the initial bucket is deleted ..
       	   log->debug("Removing the bucket with start time "+ftos(bucketShared->getstartTime()),6);
       	   sharedShadow.erase(shbuckets++);
       	   assignedcpus--;
          
       	   log->debug("FindFastMethodAllocation: UNselecting the bucket with id "+itos(bucketShared->getId()),6);
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
      }//if (sharedShadow.size() != 0)
      
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
      log->debug("FindFastMethodAllocation: Selecting the bucket with id "+itos(current->getId()),6);
      assignedcpus++;

    }
  }





  allocation = new AnalogicalJobAllocation();

  allocation->setStartTime(startShadow);
  allocation->setEndTime(endShadow);
  
  
  for(list<AnalogicalBucket*>::iterator it = sharedShadow.begin();it != sharedShadow.end();++it)
  {
    AnalogicalBucket* logbuck = (AnalogicalBucket*) *it;
    log->debug("FindFastMethodAllocation: Selected the CPU "+itos(logbuck->getcpu())+" from "+ftos(startShadow)+ " till "+ftos(endShadow),4);
    allocation->allocations.push_back(logbuck);

	if(reconfigure){

    		int cpu = logbuck->getcpu();
    		Node *node = mn->getNodeWithGlobalId(mn->getNodeIdCpu(cpu));
		int previousType = node->getNetworkBW();

		//node->setNetworkBW(type);

		/////// INSERT STRUCTURE EN power-> etc...

		int theNode = mn->getNodeIdCpu(cpu);


		if(previousType!=type){
			mn->reconfigureByNode(theNode ,type);

			((MNConfiguration*) this->architecture)->setMarenostrum(mn);

			sched->power->confs[theNode].push_back(pairConfs(initialShadow, type));
			//sched->power->confs.push_back(tmp);

			cout<<"Reconfiguring NODE:"<<theNode<<" because CPU:"<<cpu<<" at time:"<<(long)initialShadow<<" from type:"<<previousType<<" to type:"<<type<<endl;
		}
	}

  }

  //in the case that there are not enough buckets that satisties the given search criteria 
  if(assignedcpus < numcpus)
  {
    allocation->setAllocationProblem(true);
    allocation->setAllocationReason(NOT_ENOUGH_CPUS); 
  }
 

  return allocation;
  }


/**
 *  this function returns the first set of buckets that share a periodof time and whom lenght is the once required 
 *  something important is that the forech bucket only the first bucket that satisfies the lenght is returned
 * @param suitablebuckets A set of buckets that are suitable for the allocation, each analogical bucket is for one different processors 
 * @param length The duration required for the allocation 
 * @param numcpus The number of cpus required for the allocation 
 * @param initialShadow The point of time that the allocation is required 
 * @return The allocation that matches the creiteria (if there is not allocation that matches the requirements it will be shown in the allocation problem). The returned allocation is a not real allocation. 
 */
AnalogicalJobAllocation* AnalogicalResertavionTable::findFastMethodAllocation(BucketSetOrdered* suitablebuckets,double length,int numcpus,double initialShadow)
{		  
  list<AnalogicalBucket*> sharedShadow; 
  double startShadow = initialShadow;
  double endShadow = initialShadow+length;
  int assignedcpus = 0;
  
  AnalogicalJobAllocation* allocation;
	
  for(BucketSetOrdered::iterator i= suitablebuckets->begin(); i!=suitablebuckets->end(); ++i)
  {
    /*we have to compare with the first element */
    AnalogicalBucket*  current = *i;    
    
    log->debug("FindFastMethodAllocation: cheking the bucket CPU "+itos(current->getcpu())+" to the suitablebuckets list whom start time is "+ftos(current->getstartTime())+" and whom endtime is "+ftos(current->getendTime())+" and bucket id "+itos(current->getId()),6);
    
    /* for sanity */
    assert(assignedcpus >= 0);
    
    if(assignedcpus == numcpus)
      //the required allocation has been found ..
      break;
        
    
    if(sharedShadow.size() != 0)
    {
      //we just have to check that the buckets of the vector shares the required window of time with the current 
      //bucket, be aware that by construction the buckets are ordered by they start time            
      for( list<AnalogicalBucket*>::iterator shbuckets = sharedShadow.begin();
           sharedShadow.end() != shbuckets;)
      {
        log->debug("The size of the sharedShadow is "+itos(sharedShadow.size()),6);
      
        AnalogicalBucket* bucketShared = *shbuckets;
        //if the current bucket and the new processed bucket does not share the required period of time
        //we have to push popfront it .. otherwise.. by construction (ordered by startime) the rest of 
        //buckets shares the same period of time .. so we have finished
        
        log->debug("The difference is "+ftos(bucketShared->getendTime() - current->getstartTime()),6);
        log->debug("The difference with endShadow is "+ftos(bucketShared->getendTime() - startShadow),6);
        log->debug("The startime for the already inserted bucket is "+ftos(bucketShared->getstartTime()),6);
        
        if(bucketShared->getendTime() - current->getstartTime() < length && 
           !(bucketShared->getendTime() == -1 && bucketShared->getendTime() == -1 )) //if both buckets are to infinity the share all the time..                      
        {
          //the shadow must be updated according to the fact that the initial bucket is deleted ..
          log->debug("Removing the bucket with start time "+ftos(bucketShared->getstartTime()),6);
          sharedShadow.erase(shbuckets++);
          assignedcpus--;
          
          log->debug("FindFastMethodAllocation: UNselecting the bucket with id "+itos(bucketShared->getId()),6);
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
    log->debug("FindFastMethodAllocation: Selecting the bucket with id "+itos(current->getId()),6);
    assignedcpus++;
  }
  
  allocation = new AnalogicalJobAllocation();
  allocation->setStartTime(startShadow);
  allocation->setEndTime(endShadow);
  
  
  for(list<AnalogicalBucket*>::iterator it = sharedShadow.begin();it != sharedShadow.end();++it)
  {
    AnalogicalBucket* logbuck = (AnalogicalBucket*) *it;
    log->debug("FindFastMethodAllocation: Selected the CPU "+itos(logbuck->getcpu())+" from "+ftos(startShadow)+ " till "+ftos(endShadow),4);
    allocation->allocations.push_back(logbuck);
  }

  
  
  
  /*in the case that there are not enough buckets that satisties the given search criteria */
  if(assignedcpus < numcpus)
  {
    allocation->setAllocationProblem(true);
    allocation->setAllocationReason(NOT_ENOUGH_CPUS); 
  }
  
  return allocation;
  }
  

/**
 * Function that indicates if two buckets share in the time equal or more time than the indicated 
 * @param bucket1 The first bucket 
 * @param bucket2 The second bucket 
 * @param length The amount of time that they should share 
 * @return True if both buckets share the amount of time provided.
 */
bool AnalogicalResertavionTable::shareEnoughTime(AnalogicalBucket* bucket1, AnalogicalBucket* bucket2,double length)
{
    AnalogicalBucket* first = NULL;
    AnalogicalBucket* second = NULL;
    
    if(bucket1->getstartTime() < bucket2->getstartTime())
    {
      first = bucket1;
      second = bucket2;    
    }
    else
    {
      first = bucket2;
      second = bucket1;
    }
   
   /* in the case that the first is to inifity we simply return true */
   if(first->getendTime() == -1)
     return true;
   
   /* now that we have set the order for the buckets we just have to check if they share the time */
   if(first->getendTime() < second->getstartTime())
     return false; /* do no share any interval */
     
   if(second->getendTime() == -1 && second->getstartTime()-first->getendTime() >= length)  
     return true;
     
   if(first->getendTime() > second->getendTime())
     return second->getendTime()-second->getstartTime() >= length; /* the second bucket is fully included in the first */
     
   return first->getendTime()-second->getstartTime() >= length;
     
}

/**
 * This function will return the numcpus first buckets that starts earlier in the time 
 * @param suitablebuckets The set of buckets to explore
 * @param length The amount of time required to share all the buckets 
 * @param numcpus The number of buckets that are required 
 * @param initialShadow From which startime the algorithm has to explore
 * @return  The allocation that matches the creiteria (if there is not allocation that matches the requirements it will be shown in the allocation problem). The returned allocation is a not real allocation. 
 */
AnalogicalJobAllocation* AnalogicalResertavionTable::findFastConsecutiveMethodAllocation(BucketSetOrdered* suitablebuckets,double length,int numcpus,double initialShadow)
{		  
  deque<AnalogicalBucket*> sharedShadow; 
  double startShadow = initialShadow;
  double endShadow = startShadow+length;
  int assignedcpus = 0;
  
  AnalogicalJobAllocation* allocation = NULL;
  AnalogicalJobAllocation* earlierAllocation = NULL;	
	
  BucketSetCPUOrdered suitableByCPUS;
  
  //First we allocate the current buckets to the bucket ordered by CPUS
  for(BucketSetOrdered::iterator i= suitablebuckets->begin(); i!=suitablebuckets->end(); ++i)
  {    
    AnalogicalBucket*  current = *i;
     
    suitableByCPUS.insert(current);
  
  }
  
  /* we try to find out the buckets ordered by the time */
	
  for(BucketSetCPUOrdered::iterator i= suitableByCPUS.begin(); i!=suitableByCPUS.end(); ++i)
  {
    /*we have to compare with the first element */
    AnalogicalBucket*  current = *i;    
    
    log->debug("findFastConsecutiveMethodAllocation: cheking the bucket CPU "+itos(current->getcpu())+" to the suitablebuckets list whom start time is "+ftos(current->getstartTime())+" and whom endtime is "+ftos(current->getendTime())+" and bucket id "+itos(current->getId()),6);           
    
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
      
      //we implement this as a hashmap due to we can detect two times that a given bucket has to 
      //be deleted from the current bucket.
      map<AnalogicalBucket*,bool> toDelete;
      
      for(deque<AnalogicalBucket*>::iterator shbuckets = sharedShadow.begin();sharedShadow.end() != shbuckets;++shbuckets)
      {
        AnalogicalBucket* bucketShared = *shbuckets;
        //if the current bucket and the new processed bucket does not share the required period of time
        //we have to push popfront it .. otherwise.. by construction (ordered by startime) the rest of 
        //buckets shares the same period of time .. so we have finished
           
        
        if(!shareEnoughTime(current,bucketShared,length)) //if both buckets are to infinity the share all the time..                      
        {          
          //we must delete all the previous buckets .. since the allocation can't be continous ,,
          for(deque<AnalogicalBucket*>::iterator deleteIt = sharedShadow.begin();*deleteIt != bucketShared;++deleteIt)
          {
            //it can't be deleted here due to we would be modifying the set structure here and would cause crash in some situations , due to the outher loop would be using the old structure
            //however we have to check if the current bucket has not been already unselected 
            if(toDelete.count(*deleteIt) == 0)
            {
              toDelete.insert(pairBucketBool((AnalogicalBucket*)*deleteIt,true));
              assignedcpus--;
            }
          }  
          //we delete the bucket 
          toDelete.insert(pairBucketBool(bucketShared,true));
          
          assignedcpus--;
          
          log->debug("FindFastMethodAllocation: UNselecting the bucket with id "+itos(bucketShared->getId()),6);
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
      
      /* now we have to te de delete the buckets that have been unselected */
      for(map<AnalogicalBucket*,bool>::iterator del = toDelete.begin(); del != toDelete.end();++del)
      {
         deque<AnalogicalBucket*>::iterator it = find(sharedShadow.begin(),sharedShadow.end(),del->first);
         if(it != sharedShadow.end())
           sharedShadow.erase(it);
         else
           assert(false);
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
      if(allocation == NULL)
      {
        allocation = new AnalogicalJobAllocation(sharedShadow,startShadow,endShadow);
      }
      else if(allocation->getstartTime() > startShadow)
      {
        //the current allocation is better than the previous computed before
        delete allocation;
        allocation = new AnalogicalJobAllocation(sharedShadow,startShadow,endShadow);
      }
      
      //we just pop front the first element.. this will allow to find allocations with more granularity
      sharedShadow.pop_front();
      assignedcpus--;
      
    } 
    
  }
    
    //in case there are not buckets for the requested cpu we must create an allocation with the error
  if(allocation == NULL)
  {
    log->debug("There are not enough cpus in the suitable buckets for the job ",4);
    allocation = new AnalogicalJobAllocation();
    allocation->setAllocationProblem(true);
    allocation->setAllocationReason(NOT_ENOUGH_CPUS); 
  }
  else if(log->getlevel() >= 4)
   for(deque<AnalogicalBucket*>::iterator it = allocation->allocations.begin();it != allocation->allocations.end();++it)
   {
     AnalogicalBucket* logbuck = (AnalogicalBucket*) *it;     
     log->debug("findFastConsecutiveMethodAllocation: Selected the CPU "+itos(logbuck->getcpu())+" from "+ftos(allocation->getstartTime())+ " till "+ftos(allocation->getendTime()),4);
   }  
  
  return allocation;
}


/**
 * This function, for each cpu, returns a pointer to the first available bucket whom lenght is bigger than the provided and that starts after the date time 
 * 
 * @param time The start time from when the job has to start to explore
 * @param length The minimum lenght required for the selected buckets
 * @return The set of buckets that mathces the criteria
 */
BucketSetOrdered AnalogicalResertavionTable::findFirstBucketCpus(double time, double length)
{ 
  int minimum_st_time = 0;
  int total_buckets = 0;

  BucketSetOrdered suitablebuckets;


  /* first we get the architecture .. to go through all the processors */

  ArchitectureConfiguration* architecture =  this->getarchitecture(); 

  /* this value should never be null and there always have to be cpus*/
  assert(architecture != 0 && architecture->getNumberCpus() > 0);

  
  //this value indicates the number of consecutive buckets that 
  //matches in a given period of time, this period of time must be bigger than lenght
  int consecutives = 0; 
  
  for(int i = 0; i < architecture->getNumberCpus();i++)
  {    
    AnalogicalBucket* current = findFirstBucketProcessor(time,length,i);
   
    suitablebuckets.insert(current);	
    log->debug("findFirstBucketCpus: Adding the CPU "+itos(current->getcpu())+" to the suitablebuckets list whom start time is "+ftos(current->getstartTime())+" and whom endtime is "+ftos(current->getendTime())+" and bucket id "+itos(current->getId()),6); 
  }
    
  return suitablebuckets;

}

/**
 * This function  returns a pointer to the first available bucket whom lenght is bigger than the provided and that starts after the date time in the specified processor 
 * 
 * @param time The start time from when the job has to start to explore
 * @param length The minimum lenght required for the selected buckets
 * @param processor The processors where the bucket has to be found 
 * @return The set of buckets that mathces the criteria
 */
AnalogicalBucket* AnalogicalResertavionTable::findFirstBucketProcessor(double time, double length, int processor)
{


  /* starting the search based on the o time, coz may be the more suitable bucket starts at the past 
    for example if we are in the time 3 we may find the bucket 150 -1 that is the firts bucket that matches the lower_bound propierty, but we could used the bucket 0..120
    
    TO IMPROVE : use the function find of the algorithm
  */ 

  /*we find the first bucket that satisfies the conditions 
     AWARE - See the definition of lower_bound in the STL (Finds the first element whose key is not less than k.)
  */
  BucketSetOrdered::iterator cpuBuckets; 	       
    
  log->debug("There are "+ itos(buckets[processor]->size()) + " in the buckets for the cpu "+itos(processor) + " there are "+itos(buckets.size()) + " total macro buckets",6);
    
  if(buckets[processor]->size() == 1)
  //There are no buckets in the upper bound .. 
  {
    cpuBuckets = buckets[processor]->begin();
    AnalogicalBucket* current = *cpuBuckets;
    return current;
    log->debug("findFirstBucketCpus: Adding the CPU "+itos(current->getcpu())+" to the suitablebuckets list whom start time is "+ftos(current->getstartTime())+" and whom endtime is "+ftos(current->getendTime())+" and bucket id "+itos(current->getId()),6);   
  }
  else
  {
    cpuBuckets = buckets[processor]->begin();
            
    /*Can happend that the only suitable bucket is the last one 
        when this occurs the time > any of the start times .. and cpuBuckets will be end(),
        at this time we must point to the last ..
    */
    if(cpuBuckets==buckets[processor]->end())
    {
      //using a reverse iterator we get the last element of the set
      BucketSetOrdered::reverse_iterator cpuBucketsReverse =buckets[processor]->rbegin();
      //For sanity
      assert(cpuBucketsReverse !=buckets[processor]->rend());
      AnalogicalBucket* current = *cpuBucketsReverse;
      return current;
      log->debug("findFirstBucketCpus: Adding the CPU "+itos(current->getcpu())+" to the suitablebuckets list whom start time is "+ftos(current->getstartTime())+" and whom endtime is "+ftos(current->getendTime())+" and bucket id "+itos(current->getId()),6);      
    }
  }
    //Only debug stuff
  if(log->getlevel() >= 6)
    for(BucketSetOrdered::iterator deb = this->buckets[processor]->begin();deb !=   this->buckets[processor]->end();++deb)
    {
      AnalogicalBucket* debBuck = *deb;
      log->debug("Bucked id "+itos(debBuck->getId())+ " statTime "+ftos(debBuck->getstartTime())+" endTime "+ftos(debBuck->getendTime())  ,6);
    } 
    
  for(; cpuBuckets!=buckets[processor]->end(); ++cpuBuckets)
  {
    AnalogicalBucket* current = *cpuBuckets;
      
   //if endtime == -1 means that this is the last bucket of the cpu , the once that reperesnts the infinite 
    if(current->getendTime() == -1 || 
      (current->getendTime()-current->getstartTime() > length && current->getstartTime() >= time) || /* the bucket is just after the required allocation*/
      (current->getendTime()-time > length && current->getstartTime() < time) /* the bucket start before the current bucket, but its lenght its enough*/
        )
    {
      return current;
       break; 
    }
  }    
}

/**
 * Returns the number of processors that are currently used by running jobs.
 * @return The number of used processors
 */
int AnalogicalResertavionTable::getNumberCPUSUsed()
{
   int used = 0;
   
   for(map<Job*,JobAllocation*>::iterator allocIter = this->JobAllocationsMapping.begin();
       allocIter != this->JobAllocationsMapping.end();
       ++allocIter 
    )
   {
    AnalogicalJobAllocation* allocation = (AnalogicalJobAllocation*) allocIter->second;
    Job* allocatedJob = allocIter->first;
    
    if(allocatedJob->getJobSimStatus() == RUNNING)
    {
     used+=allocatedJob->getNumberProcessors();
    }
  }
  
  return used;

}


/**
 * Deletes all the buckets used by a given allocation, it is an auxiliar function.
 * @param allocationForCheckingReq  The allocation to free.
 */
void AnalogicalResertavionTable::freeAllocation(AnalogicalJobAllocation* allocationForCheckingReq)
{
 /* we must free all the used memory that won't be used later */
 for(deque< AnalogicalBucket * >::iterator iter = allocationForCheckingReq->allocations.begin();
      iter != allocationForCheckingReq->allocations.end(); ++iter)  
 {
    AnalogicalBucket* bucket = *iter;
    delete bucket;
   
 }
   
 delete allocationForCheckingReq;
}


/**
 * Returns the las id assigned to the last bucket created to the table
 * @return A integer containing the last id assigned. 
 */
int AnalogicalResertavionTable::getlastBucketId() const
{
  return lastBucketId;
}


/**
 * Sets the id that will be asigned to the next bucket created. 
 * @param theValue The next id 
 */
void AnalogicalResertavionTable::setLastBucketId(const int& theValue)
{
  lastBucketId = theValue;
}


/**
 * Returns the reference to the buckets 
 * @return A reference to the buckets 
 */
vector<BucketSetOrdered*>* AnalogicalResertavionTable::getbuckets() 
{
  return &this->buckets;
}

/**
 * Returns the reference to the allocations 
 * @return A reference to the allocations 
 */
vector<BucketSetOrdered*>* AnalogicalResertavionTable::getallocations() 
{
  return &this->allocations;
}


}
