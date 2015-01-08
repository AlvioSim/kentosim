#ifndef SIMULATORPROCESSUNIFORM_H
#define SIMULATORPROCESSUNIFORM_H

#include <statistics/process.h>

namespace Statistics {

/**
	@author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/
class ProcessUniform : public Process
{
  //@cmember Lower edge.
  double a;
  //@cmember Upper edge.
  double b;
public:
  //@cmember Creates a new uniform distribution.
  ProcessUniform(){}
  //@cmember Creates a new uniform distribution.
  ProcessUniform(double a0, double b0, int stream);
  ProcessUniform(double a0, double b0);
  //@cmember Return the lower edge.
  double GetLowerLimit();
  //@cmember Return the upper edge.
  double GetUpperLimit();
  //@cmember Sets the limits.
  int SetLimits(double inf, double sup);
  //@cmember Sets the lower edge.
  int SetLowerLimit(double a0);
  //@cmember Sets the upper edge.
  int SetUpperLimit(double b0);
  //@cmember Returns a random number.
  virtual UDTIME DeltaProx();
};

}

#endif
