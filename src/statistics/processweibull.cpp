#include <statistics/processweibull.h>

#include <math.h>

namespace Statistics {


/*******************************************************************
	Metodes ProcessWeibull
*******************************************************************/
//@mfunc Creates a new Weibull instance.
//@parm double | alfa0 |  Weibull's parameter.
//@parm double | beta0 |  Weibull's parameter.
//@parm int | stream | Random number stream.
ProcessWeibull::ProcessWeibull (double alfa0,double beta0,  int stream)
{
  alfa=alfa0;
  beta=beta0;
  SetLastNumber(zrng[stream]);
}
ProcessWeibull::ProcessWeibull (double alfa0,double beta0)
{
  alfa=alfa0;
  beta=beta0;
  SetLastNumber(zrng[lastStream]);
}

//@mfunc Returns alfa Weibull parameter.
//@rdesc Returns alfa Weibull parameter.
double ProcessWeibull::GetAlfa()
{
	return alfa;
}

//@mfunc Returns beta Weibull parameter.
//@rdesc Returns beta Weibull parameter.
double ProcessWeibull::GetBeta()
{
	return beta;
}

//@mfunc Sets the alfa Weibull parameter.
//@rdesc Returns a flag.
//@flag 0  | Operation was succesfull.
//@flag -1 | Alfa less or equal than zero.
//@parm double | alfa0 | alfa Weibull parameter.
int ProcessWeibull::SetAlfa(double alfa0)
{
	if (alfa0 <= 0) return -1;
	else alfa=alfa0;
	return 0;
}

//@mfunc Sets the beta Weibull parameter.
//@rdesc Returns a flag.
//@flag 0  | Operation was succesfull.
//@flag -1 | Beta less or equal than zero.
//@parm double | beta0 | beta Weibull parameter.
int ProcessWeibull::SetBeta(double beta0)
{
	if (beta0 <= 0) return -1;
	else beta=beta0;
	return 0;
}

//@mfunc Returns a weibull random number.
//@rdesc Returns a weibull random number.
UDTIME ProcessWeibull::DeltaProx()
{
  double ra;

  ra = rando();   // Num aleatori [0,1]
  ra = ra < epsilon ? epsilon : ra;  // Num aleat ]0,1]
  UDTIME tmp=(UDTIME) (beta * pow (-log(ra),(1/alfa)) ) ;  // Law pag. 490
  if (tmp<0) tmp=DeltaProx();
  return tmp;
}


}
