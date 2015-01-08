#ifndef STATISTICSPROCESSLOGNORMAL_H
#define STATISTICSPROCESSLOGNORMAL_H

#include <statistics/processnormal.h>

namespace Statistics {

/**
	@author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/
//@class Generates a Lognormal distribution.
//@base public | ProcessNormal.
class ProcessLogNormal : public ProcessNormal
{
public:
  //@cmember Creates a new lognormal distribution.
  ProcessLogNormal(){}
  //@cmember Creates a new lognormal distribution.
  ProcessLogNormal(double mu0,double sigma0, int stream);
  ProcessLogNormal(double mu0,double sigma0);

  //@cmember Returns a random number.
  virtual UDTIME DeltaProx();
};

}

#endif
