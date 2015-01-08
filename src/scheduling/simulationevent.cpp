#include <scheduling/simulationevent.h>

namespace Simulator {

/**
 * The default constructor for the job event 
 * @param type The event type 
 * @param job The job associated to the event 
 * @param id The id for event 
 * @param time The time associated to the event 
 */
SimulationEvent::SimulationEvent(event_t type,Job* job,int id,double time)
{
  this->type = type;
  this->job = job;
  this->id = id;
  this->time = time;
}

/**
 * The default destructor for the class 
 */
SimulationEvent::~SimulationEvent()
{
}

/**
 * Returns the id for the simulationevent
 * @return A integer containing the id
 */
int SimulationEvent::getid() const
{
  return id;
}


void SimulationEvent::setId(const int& theValue)
{
  id = theValue;
}

void SimulationEvent::setTime(const double& theValue)
{
  this->time = theValue;
}

/**
 * Returns the time for the event 
 * @return A double containing the Time for the event 
 */

double SimulationEvent::getTime() const
{
  return this->time;
}


/**
 * Returns the event type 
 * @return A event_t containing the type of the event 
 */
event_t SimulationEvent::gettype() const
{
  return type;
}


/**
 * Sets the type of he current event 
 * @param theValue The type of the event 
 */
void SimulationEvent::setType(const event_t& theValue)
{
  type = theValue;
}

/**
 * Returns the job related to the event 
 * @return A reference to the job 
 */
Job* SimulationEvent::getjob() const
{
  return job;
}


/**
 * Sets the job related to the event 
 * @param theValue A reference to the event 
 */
void SimulationEvent::setJob(Job* theValue)
{
  job = theValue;
}

}
