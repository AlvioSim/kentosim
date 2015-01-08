#ifndef SIMULATORSIMULATIONEVENT_H
#define SIMULATORSIMULATIONEVENT_H

#include <scheduling/job.h>



namespace Simulator {

/** Enum that contains all the event types that can be triggered in the simulation */
enum event_t {
    EVENT_ILLEGAL=0,
    EVENT_TERMINATION,		 
    EVENT_ADJUST_PREDICTION,// prediction too short (event may be turned off/on)
    EVENT_ARRIVAL,
    EVENT_START,
    EVENT_ABNORMAL_TERMINATION,
    EVENT_NOSPECIFIC, //using for finding specific events
    EVENT_COLLECT_STATISTICS,
    EVENT_OTHER,
};

/**
* @author Francesc Guim,C6-E201,93 401 16 50, 
*/

/**
* This class implements a simulation event. 
* @see The class simulation
*/
class SimulationEvent{
public:
  SimulationEvent(event_t type,Job* job,int id,double time);
  ~SimulationEvent();
  void setId(const int& theValue);
  int getid() const;
  void setTime(const double& theValue);
  double getTime() const;	
  void setType(const event_t& theValue);
  event_t gettype() const;
  void setJob(Job* theValue);
  Job* getjob() const;
	

private:
  
  Job* job;  /**< to which job the event is associated.. */
  double time; /**< the time associated to event .. when it must be triggered..*/
  int id; /** the id for the event */
  event_t type; /**< which kind of event .*/

};

}

#endif
