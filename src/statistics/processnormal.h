#ifndef STATISTICSPROCESSNORMAL_H
#define STATISTICSPROCESSNORMAL_H

#include <statistics/process.h>

namespace Statistics {

/**
	@author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/
//@class Generates a Normal distribution.
//@base public | Process.
class ProcessNormal : public Process
{
  //@cmember Distribution's Average.
  double mu;
  //@cmember Distribution's Stdev.
  double sigma;
public:
  //@cmember Creates a new Normal distribution.
	ProcessNormal(){ 
		mu=123;   
	}
  //@cmember Creates a new Normal distribution.
  ProcessNormal(double mu0,double sigma0, int stream);
  ProcessNormal(double mu0,double sigma0);
  //@cmember Returns distribution's average.
  double GetMu();
  //@cmember Returns distribution's stdev.
  double GetSigma();
  //@cmember Sets distribution's average.
  int SetMu(double mu0);
  //@cmember Sets distribution's stdev.
  int SetSigma(double sigma0);
  //@cmember Returns a random number.
  virtual UDTIME DeltaProx();
};

}

#endif
