#include <statistics/processtriangular.h>

#include <math.h>

namespace Statistics {


/*******************************************************************
	Metodes ProcessTriangular
*******************************************************************/
//@mfunc Creates a new Triangular instance.
//@parm double | a0 | Left trianle edge.
//@parm double | b0 | Right trianle edge.
//@parm double | c0 | Height triangle edge.
//@parm int | stream | Random number stream.
ProcessTriangular::ProcessTriangular(double a0, double b0, double c0, int stream)
{
 a=a0;
 b=b0;
 c=c0;
 SetLastNumber(zrng[stream]);
}
ProcessTriangular::ProcessTriangular(double a0, double b0, double c0)
{
 a=a0;
 b=b0;
 c=c0;
}


//@mfunc Returns left triangle edge.
//@rdesc Returns left triangle edge.
double ProcessTriangular::GetLowerLimit()
{
	return a;
}

//@mfunc Returns right triangle edge.
//@rdesc Returns right triangle edge.
double ProcessTriangular::GetUpperLimit()
{
	return b;
}

//@mfunc Returns height triangle edge.
//@rdesc Returns height triangle edge.
double ProcessTriangular::GetHigherLimit()
{
	return c;
}


int ProcessTriangular::SetLimits(double lower, double upper, double higher)
{

	if ((lower >= higher) || (lower >= upper)) 
		return -1;
	else if ((upper <= higher) || (upper <= lower))
		return -1;
	else if ((higher >= upper) || (higher <= lower))
		return -1;
	else
	{
		a=lower;
		b=upper;
		c=higher;

		return 0;
	}
}

//@mfunc Sets the left triangle edge.
//@rdesc Returns a flag.
//@flag 0  | Operation was succesfull.
//@flag -1 | Left edge greater than the other.
//@parm double | a0 | Left triangle edge.
int ProcessTriangular::SetLowerLimit (double a0)
{
	if ((a0 >= c) || (a0 >= b)) return -1;
	else a=a0;
	return 0;
}

//@mfunc Sets the right triangle edge.
//@rdesc Returns a flag.
//@flag 0  | Operation was succesfull.
//@flag -1 | Right edge lower than the other.
//@parm double | b0 | Right triangle edge.
int ProcessTriangular::SetUpperLimit (double b0)
{
	if ((b0 <= c) || (b0 <= a)) return -1;
	else b=b0;
	return 0;
}

//@mfunc Sets the height triangle edge.
//@rdesc Returns a flag.
//@flag 0  | Operation was succesfull.
//@flag -1 | High isn't beetwen edges.
//@parm double | c0 | Height triangle edge.
int ProcessTriangular::SetHigherLimit (double c0)
{
	if ((c0 >= b) || (c0 <= a)) return -1;
	else c=c0;
	return 0;
}
  
//@mfunc Returns a triangular random number.
//@rdesc Returns a triangular random number.
UDTIME ProcessTriangular::DeltaProx()
{
 double ra, aux;

 // ERROR: c1 debe ser (c-a)/(b-a)
/*
 if ((ra = (double)(rando())) <= c)	   // Num aleatori [0,1]
    aux = (sqrt(c*ra));
 else
    aux = (1-sqrt((1-c)*(1-ra)));
 
 UDTIME tmp=(UDTIME) (a+ (b-a)*aux);	// Law pag. 494
*/
 double c1=(c-a)/(b-a);

 if ((ra = (double)(rando())) <= c1)	   // Num aleatori [0,1]
    aux = (sqrt(c1*ra));
 else
    aux = (1-sqrt((1-c1)*(1-ra)));
 
 UDTIME tmp=(UDTIME) (a+ (b-a)*aux);	// Law pag. 494

 if (tmp<0) tmp=DeltaProx();
 
 return tmp;

}


}
