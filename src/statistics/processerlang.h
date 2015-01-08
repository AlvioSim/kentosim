#ifndef STATISTICSPROCESSERLANG_H
#define STATISTICSPROCESSERLANG_H

#include <statistics/process.h>

namespace Statistics {

/**
	@author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

//@class Generates an Erlang distribution.
//@base public | Process.
class ProcessErlang : public Process
{
  //@cmember Erlang distribution parameter.
  double beta;
  //@cmember Stage number of Erlang distribution.
  int m;
public:
  //@cmember Creates a new Erlang distribution.
  ProcessErlang(){}
  //@cmember Creates a new Erlang distribution.
  ProcessErlang(double beta0,int m0);
  ProcessErlang(double beta0,int m0, int stream);
  //@cmember Return the number of stages.
  unsigned int GetStage();
  //@cmember Returns beta parameter.
  double GetBeta();
  //@cmember Sets the number of stages.
  int SetStage(int m0);
  //@cmember Sets beta parameter.
  int SetBeta(double beta0);
  //@cmember Returns a random number.
  virtual UDTIME DeltaProx();
};
}

#endif
