#ifndef STATISTICSPROCESSGEOMETRIC_H
#define STATISTICSPROCESSGEOMETRIC_H

#include <statistics/process.h>

namespace Statistics {

/**
	@author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

//@class Generates a Geometric distribution.
//@base public | Process.
class ProcessGeometric : public Process
{
  //@cmember Probability of succes in each prove.
  double p;
public:
  //@cmember Creates a new geometric distribution.
  ProcessGeometric(){}
  //@cmember Creates a new geometric distribution.
  ProcessGeometric(double p0, int stream);
  ProcessGeometric(double p0);

  //@cmember Returns succes probability.
  double GetProbability();
  //@cmember Sets succes probability.
  int SetProbability(double p0);
  //@cmember Returns a random number.
  virtual UDTIME DeltaProx();
};

}

#endif
