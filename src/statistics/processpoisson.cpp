#include <statistics/processpoisson.h>
#include <math.h>

namespace Statistics {


/*******************************************************************
	Metodes ProcessPoisson
*******************************************************************/
//@mfunc Creates a new Poission distribution.
//@parm double | lambda0 | Frequency beetwen arrivals.
//@parm int | stream | Random nuber stream.
ProcessPoisson::ProcessPoisson(double lambda0,  int stream)
{
  lambda = lambda0;
  // per a evitar zeros en el log de DeltaProx()
  SetLastNumber(zrng[stream]);
}

ProcessPoisson::ProcessPoisson(double lambda0)
{
  lambda = lambda0;
  // per a evitar zeros en el log de DeltaProx()
  SetLastNumber(zrng[lastStream]);
}

//@mfunc Returns the frequency of enities arrivals.
//@rdesc Returns the frequency of enities arrivals.
double ProcessPoisson::GetMu()
{
	return lambda;
}

//@mfunc Sets the frequency of enities arrivals.
//@rdesc Returns a flag.
//@flag 0  | Operation was succesfull.
//@flag -1 | Lambda less or equal tahan zero.
//@parm double | lambda0 | frequency of enities arrivals.
int ProcessPoisson::SetMu(double lambda0)
{
	if (lambda0 <= 0) return -1;
	else lambda=lambda0;
	return 0;
}

//@mfunc Returns a poisson random number.
//@rdesc Returns a poisson random number.
UDTIME ProcessPoisson::DeltaProx()
{
  double ra=rando();
  ra = ra < epsilon? epsilon : ra;  // Num aleat ]0,1]
  UDTIME tmp=(UDTIME)(-log(ra) / (lambda));           // Law pag. 507
  if (tmp<0) tmp=DeltaProx();
  return tmp;
}

}
