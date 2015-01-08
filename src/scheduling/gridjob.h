#ifndef SIMULATORGRIDJOB_H
#define SIMULATORGRIDJOB_H

#include <scheduling/job.h>
#include <scheduling/jobrequirement.h>

namespace Simulator {

/**
	@author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/
/**
*   This class mainly extends the Job Basic Information for deal with jobs that belong to a grid environment.
*/

class GridJob : public Job {
public:
  GridJob();
  ~GridJob();

  void setOSNameRequirement ( const JobRequirement& theValue );
  JobRequirement* getOSNameRequirement() ;
  void setNumberProcessorsRequirement ( const JobRequirement& theValue );
  JobRequirement* getNumberProcessorsRequirement() ;
  void setMemoryRequirement ( const JobRequirement& theValue );
  JobRequirement* getMemoryRequirement() ;
  void setDiskRequirement ( const JobRequirement& theValue );
  JobRequirement* getDiskRequirement() ;
  void setClockSpeedRequirement ( const JobRequirement& theValue );
  JobRequirement* getClockSpeedRequirement() ;
  void setVendorRequirement ( const JobRequirement& theValue );
  JobRequirement* getVendorRequirement() ;
  void setSubmittedCenter ( const string& theValue );
  string getSubmittedCenter() const;
  void setForwardedTimes ( double theValue );
  double getforwardedTimes() const;	
  void setVOSubmitted ( const string& theValue );
  string getVOSubmitted() const;
	

private:
  JobRequirement MemoryRequirement; /**< Memory requirement for the grid  job  */
  JobRequirement VendorRequirement; /**< Vendor requirement for the grid  job  */
  JobRequirement ClockSpeedRequirement; /**< Clock Speed requirement for the grid  job  */
  JobRequirement OSNameRequirement; /**< OS requirement for the grid  job  */
  JobRequirement NumberProcessorsRequirement; /**< Number of processors requirement for the grid  job  */
  JobRequirement DiskRequirement; /**< Disk requirement for the grid  job  */

  string SubmittedCenter; /**< The center where the job was initially submitted*/;
  string VOSubmitted; /** The string containing the initial VO where it was submitted */
  double forwardedTimes; /**<  The number of times that the job has been forwarded */

};

}

#endif
