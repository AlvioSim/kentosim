#include <scheduling/gridjob.h>

namespace Simulator {

/***************************
  Constructor and destructors
***************************/
/**
 * The default constructor for the class
 */
GridJob::GridJob() : Job()
{
}

/**
 * The default destructor for the class 
 */
GridJob::~GridJob()
{
}

/**
 * Returns the ClockSpeedRequirement
 * @return An Requirement object containing the ClockSpeedRequirement 
 */
JobRequirement* GridJob::getClockSpeedRequirement() 
{
	return &ClockSpeedRequirement;
}

/**
 * Sets the ClockSpeedRequirement
 * @param theValue The ClockSpeedRequirement
 */
void GridJob::setClockSpeedRequirement ( const JobRequirement& theValue )
{
	ClockSpeedRequirement = theValue;
}

/**
 * Returns the DiskRequirement
 * @return An Requirement object containing the DiskRequirement 
 */
JobRequirement* GridJob::getDiskRequirement() 
{
	return &DiskRequirement;
}

/**
 * Sets the setDiskRequirement
 * @param theValue The setDiskRequirement
 */
void GridJob::setDiskRequirement ( const JobRequirement& theValue )
{
	DiskRequirement = theValue;
}

/**
 * Returns the MemoryRequirement
 * @return An Requirement object containing the MemoryRequirement 
 */
JobRequirement* GridJob::getMemoryRequirement() 
{
	return &MemoryRequirement;
}

/**
 * Sets the setMemoryRequirement
 * @param theValue The setMemoryRequirement
 */
void GridJob::setMemoryRequirement ( const JobRequirement& theValue )
{
	MemoryRequirement = theValue;
}

/**
 * Returns the MemoryRequirement
 * @return An Requirement object containing the MemoryRequirement 
 */
JobRequirement* GridJob::getNumberProcessorsRequirement() 
{
	return &NumberProcessorsRequirement;
}

/**
 * Sets the NumberProcessorsRequirement
 * @param theValue The NumberProcessorsRequirement
 */
void GridJob::setNumberProcessorsRequirement ( const JobRequirement& theValue )
{
	NumberProcessorsRequirement = theValue;
}

/**
 * Returns the OSNameRequirement
 * @return An Requirement object containing the OSNameRequirement 
 */
JobRequirement* GridJob::getOSNameRequirement() 
{
	return &OSNameRequirement;
}

/**
 * Sets the OSNameRequirement
 * @param theValue The OSNameRequirement
 */
void GridJob::setOSNameRequirement ( const JobRequirement& theValue )
{
	OSNameRequirement = theValue;
}

/**
 * Returns the VendorRequirement
 * @return An Requirement object containing the VendorRequirement 
 */
JobRequirement* GridJob::getVendorRequirement() 
{
	return &VendorRequirement;
}

/**
 * Sets the VendorRequirement
 * @param theValue The VendorRequirement
 */
void GridJob::setVendorRequirement ( const JobRequirement& theValue )
{
	VendorRequirement = theValue;
}

/**
 * Returns the center where the job was initially submitted
 * @return An string containing the center
 */
string Simulator::GridJob::getSubmittedCenter() const
{
	return SubmittedCenter;
}


/**
 * Sets the center where the job was initially submitted 
 * @param theValue The center 
 */
void Simulator::GridJob::setSubmittedCenter ( const string& theValue )
{
	SubmittedCenter = theValue;
}

/**
 * Returns the forwardedTimes
 * @return An integer containing the number of times that the job has been forwarded among the different virtual organitzations 
 */
double Simulator::GridJob::getforwardedTimes() const
{
	return forwardedTimes;
}

/**
 * Sets the number of times that the job has been forwarded 
 * @param theValue The number of times
 */
void Simulator::GridJob::setForwardedTimes ( double theValue )
{
	forwardedTimes = theValue;
}


/**
 * Returns the center where the job was initially submitted
 * @return An string containing the center
 */
string Simulator::GridJob::getVOSubmitted() const
{
	return VOSubmitted;
}

/**
 * Sets the center where the job was initially submitted 
 * @param theValue The center 
 */
void Simulator::GridJob::setVOSubmitted ( const string& theValue )
{
	VOSubmitted = theValue;
}


}
