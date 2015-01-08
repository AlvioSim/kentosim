#ifndef STATISTICSPROCESSWEIBULL_H
#define STATISTICSPROCESSWEIBULL_H

#include <statistics/process.h>

namespace Statistics {

/**
	@author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/
//@class Generates a Weibull distribution.
//@base public | Process.
class ProcessWeibull : public Process
{
  //@cmember Alfa parameter.
  double alfa;
  //@cmember Beta parameter.
  double beta;
public:
  //@cmember Creates a new Weibull distribution.
  ProcessWeibull(){}
  //@cmember Creates a new Weibull distribution.
  ProcessWeibull(double alfa0,double beta0, int stream);
  ProcessWeibull(double alfa0,double beta0);  
  //@cmember Returns alfa parameter.
  double GetAlfa();
  //@cmember Returns beta parameter.
  double GetBeta();
  //@cmember Sets alfa parameter.
  int SetAlfa(double alfa0);
  //@cmember Sets beta parameter.
  int SetBeta(double beta0);
  //@cmember Returns a random number.
  virtual UDTIME DeltaProx();
};
}

#endif
