#ifndef STATISTICSPROCESSPOISSON_H
#define STATISTICSPROCESSPOISSON_H

#include <statistics/process.h>

namespace Statistics {

/**
	@author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/
//@class Generates a Poission distribution.
//@base public | Process.
class ProcessPoisson : public Process
{
  //@cmember Frequency of entities arrivals.
  double lambda;
public:
  //@cmember Creates a new poission distribution.
  ProcessPoisson(){}
  //@cmember Creates a new poission distribution.
  ProcessPoisson(double lambda0, int stream);
  ProcessPoisson(double lambda0);
  //@cmember Returns the frequency of enities arrivals.
  double GetMu();
  //@cmember Sets the frequency of enities arrivals.
  int SetMu(double lambda0);
  //@cmember Returns a random number.
  virtual UDTIME DeltaProx();
};

}

#endif
