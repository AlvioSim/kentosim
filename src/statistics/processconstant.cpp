#include <statistics/processconstant.h>

namespace Statistics {


/*******************************************************************
	Metodes ProcessConstant
*******************************************************************/
//@mfunc Creates a new constant distribution.
//@parm double | k0 | Constant value.
ProcessConstant::ProcessConstant(double k0)
{
  k = k0;
}

//@mfunc Returns the constant value.
//@rdesc Returns the constant value.
double ProcessConstant::GetConstant()
{
  return k;
}

//@mfunc Sets the constant value.
//@rdesc Always return zero.
//@parm double | k0 | Constant value.
int ProcessConstant::SetConstant(double k0)
{
	k=k0;
 return 0;
}

//@mfunc Returns a constant random number.
//@rdesc Returns a constant random number.
UDTIME ProcessConstant::DeltaProx()
{
  UDTIME tmp=(UDTIME)k;

  if (tmp<0) tmp=k;//TODO Ara for�o que retorni el valor k, no t� sentit si �s una constant que faci un deltaprox d'ell mateix o potser si ???  DeltaProx();

	return tmp;
}

}
