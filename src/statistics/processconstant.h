#ifndef STATISTICSPROCESSCONSTANT_H
#define STATISTICSPROCESSCONSTANT_H

#include <statistics/process.h>

namespace Statistics {

/**
	@author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/
//@class Generates a constant distribution.
//@base public | Process.
class ProcessConstant : public Process
{
  //@cmember Constant value.
  double k;
  public:
  //@cmember Creates a new constant distribution.
  ProcessConstant(){}
  //@cmember Creates a new constant distribution.
  ProcessConstant(double k0);
  //@cmember Returns the constant value.
  double GetConstant();
  //@cmember Sets the constant value.
  int SetConstant(double k0);
  //@cmember Returns a random number.
  virtual UDTIME DeltaProx();
};

}

#endif
