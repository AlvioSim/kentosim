#ifndef SIMULATORRESERVATIONTABLE_H
#define SIMULATORRESERVATIONTABLE_H

#include <utils/architectureconfiguration.h>
#include <scheduling/job.h>
#include <utils/log.h>
#include <scheduling/joballocation.h>

#include <vector>
#include <map>

using namespace std; 
using std::vector;
using std::map;

namespace Simulator {

/**
* @author Francesc Guim,C6-E201,93 401 16 50,
*/

/**
* This class contains information about how the jobs are mapped into the architecture this should be used by the scheduling policy for allocating jobs.
*/
class ReservationTable{
public:

  
  /* Constructors and destructors*/
  ReservationTable();
  ReservationTable(ArchitectureConfiguration* theValue,Log* log,double globaltime);  
  virtual ~ReservationTable();
  
  /* Set and gets methods*/
  void setArchitecture(ArchitectureConfiguration* theValue);
  ArchitectureConfiguration* getarchitecture() const;	
  
  /* Auxiliar methods and main */
  //
  //
  /**
   * Allocate a job the job to the architecture to a given set of processors at a given time be aware that no collision must exist in this allocation ! this must be controlled by the scheduler.
   * @param job The job to be allocated 
   * @param allocation The allocation 
   * @return True if no error has raised, false otherwise
   */
  virtual bool allocateJob(Job* job, JobAllocation* allocation) = 0; 
  
  /**
   * deallocate a job the job to the architecture
   * @param job The job to be deallocated    
   * @return True if no error has raised, false otherwise
   */
  virtual bool deAllocateJob(Job* job) = 0; //DeAllocate a job the job to the architecture based on the criteria  
  /**
   * Advises to the reservation table that a given job has been killed 
   * @param job A reference to the killed job 
   * @return True if no error has raised, false otherwise
   */
  virtual bool killJob(Job* job) = 0;
  /**
   * Returns the number of processors currently used in the reservation table at the current time 
   * @return An integer containing the number of cpus
   */
  virtual int  getNumberCPUSUsed() = 0;
  
  /* returns the previous done allocation for the given*/
  JobAllocation* getJobAllocation(Job* job);
  void setGlobaltime(double theValue);
  double getglobaltime() const;
  void setInitialGlobalTime(double theValue);
  double getinitialGlobalTime() const;
  void setLog(Log* theValue);
  Log* getlog() const;
	
	
	
  
protected:
  ArchitectureConfiguration* architecture; /**< Contains the definition of the architecture where the jobs will be mapped */
  int freeNodes; /**< just for checking */
  Log* log; /**< A reference to the logging engine */
  double globaltime; /**<  contains the current global time */
  double initialGlobalTime; /**< contains the initial time that the reservation table starts */
  map<Job*,JobAllocation*> JobAllocationsMapping; /**< mapping the buckets that are associated to the job -  the hash map key corresponds to the id of the joband the content corresponds to the allocation for the job */
  
  
};

}

#endif
