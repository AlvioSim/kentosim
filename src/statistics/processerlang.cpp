#include <statistics/processerlang.h>
#include <math.h>

namespace Statistics {


/*******************************************************************
	Metodes ProcessErlang
*******************************************************************/
//@mfunc Creates a new Erlang.
//@parm double | beta0 | Erlang's parameter.
//@parm int | m0 | Number of stages.
//@parm int | stream | Random number stream.
ProcessErlang::ProcessErlang(double beta0,int m0)
{
	beta=beta0;
	m=m0;
	SetLastNumber(zrng[lastStream]);
}

ProcessErlang::ProcessErlang(double beta0,int m0, int stream)
{
  beta=beta0;
  m=m0;
  SetLastNumber(zrng[stream]);
}

//@mfunc Returns the number of stages.
//@rdesc Return the number of stages.
unsigned int ProcessErlang::GetStage()
{
  return m;
}

//@mfunc Sets the number of stages.
//@rdesc Returns a flag.
//@flag 0  | Operation was succesfull.
//@flag -1 | Number of stages less or equal than zero.
//@parm int | m0 | Number of stages.
int ProcessErlang::SetStage(int m0)
{
  if (m0 <= 0) return -1;
  else m=m0;
  return 0;
}

//@mfunc Returns beta parameter.
//@rdesc Returns beta parameter.
double ProcessErlang::GetBeta()
{
	return beta;
}

//@mfunc Sets beta parameter.
//@rdesc Returns a flag.
//@flag 0  | Operation was succesfull.
//@flag -1 | Beta less or equal than zero.
//@parm double | beta0 | Erlang's parameter.
int ProcessErlang::SetBeta(double beta0)
{
  if (beta0 <= 0) return -1;
  else beta=beta0;
  return 0;
}

//@mfunc Returns an Erlang random number.
//@rdesc Returns an Erlang random number.
UDTIME ProcessErlang::DeltaProx()
{
  double ra=1;
  int i;
  for(i=1;i<=m;i++) ra *= rando(); // Num aleatori [0,1]

  ra = ra < epsilon? epsilon : ra;        // Num aleat ]0,1]

  UDTIME tmp=(UDTIME) (-log(ra)*beta / m);               // Law pag. 486
  
  if (tmp<0) tmp=DeltaProx();
	
	return tmp;
}


}
