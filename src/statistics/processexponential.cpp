#include <statistics/processexponential.h>
#include <math.h>

namespace Statistics {


/////////////////////////////////////////////////////
//Metodes ProcessExponential

//@mfunc   //@cmember Creates a new exponential distribution.
//@parm double | mu0    | Time beetwen arrivals.
//@parm int | stream | Random number stream.
ProcessExponential::ProcessExponential(double mu0,  int stream)
{
  mu = mu0;
  // per a evitar zeros en el long de DeltaProx()
  SetLastNumber(zrng[stream]);
}

ProcessExponential::ProcessExponential(double mu0)
{
  mu = mu0;
  // per a evitar zeros en el long de DeltaProx()
  SetLastNumber(zrng[lastStream]);
}

//@mfunc Returns the time beetwen arrivals.
//@rdesc Returns the time beetwen arrivals.
double ProcessExponential::GetMu()
{
  return mu;
}

//@mfunc Sets the time beetwen arrivals.
//@rdesc Returns a flag.
//@flag 0  | Operation was succesfull.
//@flag -1 | Mu less or equal tahan zero.
//@parm double | mu0 | Time beetwen arrivals.
int ProcessExponential::SetMu(double mu0)
{
 if (mu0 <= 0) return -1;
 else mu=mu0;
 return 0;
}

//@mfunc Returns an exponential random number.
//@rdesc Returns an exponential random number.
UDTIME ProcessExponential::DeltaProx()
{
  double ra=rando();
  
  ra = ra < epsilon? epsilon: ra;  // Num aleat ]0,1]

 // ra = ra < epsilonExpon ? epsilonExpon : ra;	// Num aleatori ]0,1]
  UDTIME tmp=(UDTIME)(-log(ra) * (mu));      	// Law pag. 486
  if (tmp<0) tmp=DeltaProx();

	return tmp;
}


}
