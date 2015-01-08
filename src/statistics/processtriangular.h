#ifndef STATISTICSPROCESSTRIANGULAR_H
#define STATISTICSPROCESSTRIANGULAR_H

#include <statistics/process.h>

namespace Statistics {

/**
	@author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/
//@class Generates a Triangular distribution.
//@base public | Process.
class ProcessTriangular : public Process
{
  //@cmember Left triangle edge.
  double a;
  //@cmember Right triangle edge.
  double b;
  //@cmember Height triangle edge.
  double c;
public:
  //@cmember Creates a new triangular distribution.
  ProcessTriangular(){}
  //@cmember Creates a new triangular distribution.
  ProcessTriangular(double a0, double b0, double c0, int stream);
  ProcessTriangular(double a0, double b0, double c0);

  //@cmember Returns left triangle edge.
  double GetLowerLimit();
  //@cmember Returns right triangle edge.
  double GetUpperLimit();
  //@cmember Returns height triangle edge.
  double GetHigherLimit();
  
  //@cmember Sets The limits
  int SetLimits (double lower, double upper, double higher);
  //@cmember Sets left triangle edge.
  int SetLowerLimit (double a0);
  //@cmember Sets right triangle edge.
  int SetUpperLimit (double b0);
  //@cmember Sets height triangle edge.
  int SetHigherLimit (double c0);
  //@cmember Returns a random number.
  virtual UDTIME DeltaProx();
};

}

#endif
