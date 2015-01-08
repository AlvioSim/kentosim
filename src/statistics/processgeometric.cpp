#include <statistics/processgeometric.h>

#include <math.h>

namespace Statistics {


/*******************************************************************
	Metodes ProcessGeometric
*******************************************************************/
//@mfunc Creates a new Geometric instance.
//@parm double | p0 | Probability of succes in each prove.
//@parm int | stream | Random number stream.
ProcessGeometric::ProcessGeometric (double p0,  int stream)       
{
  p=log(1-p0);  
  SetLastNumber(zrng[stream]);
}

ProcessGeometric::ProcessGeometric (double p0)       
{
  p=log(1-p0);  
}

//@mfunc Returns succes probability.
//@rdesc Returns succes probability.
double ProcessGeometric::GetProbability()
{
	return p;
}

//@mfunc Sets succes probability.
//@rdesc Returns a flag.
//@flag 0  | Operation was succesfull.
//@flag -1 | Probability isn't beetwen 0 and 1.
//@parm double | p0 | Succes probability.
int ProcessGeometric::SetProbability(double p0)
{
	if ((p0<=0)||(p0>=1)) return -1;
	else p=p0;
	return 0;
}

//@mfunc Returns a geometric random number.
//@rdesc Returns a geometric random number.
UDTIME ProcessGeometric::DeltaProx()
{
  double ra=rando();
   UDTIME tmp=(UDTIME)(floor(log(ra)/p)+1); 
   if (tmp<0) tmp=DeltaProx();

	return tmp;
} 



}
