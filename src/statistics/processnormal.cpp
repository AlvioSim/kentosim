#include <statistics/processnormal.h>

#include <math.h>

namespace Statistics {

/*******************************************************************
	Metodes ProcessNormal
*******************************************************************/
//@mfunc Creates a new Normal instance.
//@parm double | mu0 | Average of normal distribution.
//@parm double | sigma0 | Stdev of normal distribution.
//@parm int | stream | Random number stream.
ProcessNormal::ProcessNormal(double mu0,double sigma0,  int stream)
{
/*  mu=mu0;
  sigma=sigma0; */
	SetMu(mu0);
	SetSigma(sigma0);
	SetLastNumber(zrng[stream]);
}

ProcessNormal::ProcessNormal(double mu0,double sigma0)
{
/*  mu=mu0;
  sigma=sigma0; */
	SetMu(mu0);
	SetSigma(sigma0);
    SetLastNumber(zrng[lastStream]);
}

//@mfunc Returns distribution's average.
//@rdesc Returns distribution's average.
double ProcessNormal::GetMu()
{
	return mu;
}

//@mfunc Returns distribution's stdev.
//@rdesc Returns distribution's stdev.
double ProcessNormal::GetSigma()
{
	return sigma;
}

//@mfunc Sets the average of the distribution.
//@rdesc Returns a flag.
//@flag 0  | Operation was succesfull.
//@flag -1 | Average less than zero.
//@parm double | mu0 | Average of normal distribution.
int ProcessNormal::SetMu(double mu0)
{
 if (mu0 < 0) return -1;
 else mu=mu0;
 return 0;
}

//@mfunc Sets the stdev of the distribution.
//@rdesc Returns a flag.
//@flag 0  | Operation was succesfull.
//@flag -1 | Sigma less or equal than zero.
//@parm double | sigma0 | Stdev of normal distribution.
int ProcessNormal::SetSigma(double sigma0)
{
	if (sigma0 <= 0)  return -1;
	else sigma=sigma0;
	return 0;
} 

//@mfunc Returns a normal random number.
//@rdesc Returns a normal random number.
UDTIME ProcessNormal::DeltaProx()
{
  double w;
  double ra1,ra2,y,x1;

	do
    {
     ra1 = rando();   // Num aleatori [0,1]
     ra2 = rando();   // Num aleatori [0,1]

     ra1=(2.0*ra1)-1.0;
     ra2=(2.0*ra2)-1.0;
     w=pow(ra1,2)+pow(ra2,2);
    }
	while (w>1.0  || w < 1E-30);


  w = w < epsilon ? epsilon : w;  // Num aleat ]0,1[
  y=sqrt((-2.0)*(log(w))/w);
  x1=ra1*y;
  UDTIME tmp=(UDTIME)(mu+sigma*x1); //o x2	// Law pag. 491
  
  return tmp;

}


}
