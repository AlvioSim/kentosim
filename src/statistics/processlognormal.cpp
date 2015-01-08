#include <statistics/processlognormal.h>
#include <math.h>

namespace Statistics {


/*******************************************************************
	Metodes ProcessLogNormal
*******************************************************************/
//@mfunc Creates a new Lognormal instance.
//@parm double | mu0 | Average of lognormal distribution.
//@parm double | sigma0 | Stdev of lognormal distribution.
//@parm int | stream | Llavor del n�mero aleatori.
ProcessLogNormal::ProcessLogNormal(double mu0,double sigma0,  int stream) :
	   ProcessNormal(mu0, sigma0)
{
  //La mu i la sigma son heredades de la classe pare.
  SetLastNumber(zrng[stream]);
}

ProcessLogNormal::ProcessLogNormal(double mu0,double sigma0) :
	   ProcessNormal(mu0, sigma0)
{
  //La mu i la sigma son heredades de la classe pare.
  SetLastNumber(zrng[lastStream]);
}

//@mfunc Returns a lognormal random number.
//@rdesc Returns a lognormal random number.
UDTIME ProcessLogNormal::DeltaProx()
{
    UDTIME tmp=(UDTIME) exp(ProcessNormal::DeltaProx()); // Law pag. 492
	if (tmp<0) tmp=DeltaProx();
	return tmp;
}


}
