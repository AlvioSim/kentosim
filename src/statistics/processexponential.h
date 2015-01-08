#ifndef STATISTICSPROCESSEXPONENTIAL_H
#define STATISTICSPROCESSEXPONENTIAL_H

#include <statistics/process.h>

namespace Statistics {

/**
	@author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/
//@class Generates an exponential distribution.
//@base public | Process.
class ProcessExponential : public Process
{
  //@cmember Time beetwen arrivals.
  double mu;
public:
  //@cmember Creates a new exponential distribution.
  ProcessExponential(){}
  //@cmember Creates a new exponential distribution.
  ProcessExponential(double mu0, int stream);
  ProcessExponential(double mu0);
  //@cmember Returns the time beetwen arrivals.
  double GetMu();
  //@cmember Sets the time beetwen arrivals.
  int SetMu(double mu0);
  //@cmember Returns a random number.
  virtual UDTIME DeltaProx();
};

}

#endif
