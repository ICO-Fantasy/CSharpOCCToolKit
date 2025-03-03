#include "ICO_GCPnts_UniformAbscissa.h"
#include <GCPnts_UniformAbscissa.hxx>
//local
#include "ICO_BRepAdaptor_Curve.h"
#include "ICO_Exception.h"

namespace OCCTK::OCC::GCPnts {

/// <summary>
/// 对曲线进行均匀采样(按间隔值)
/// </summary>
/// <param name="curve">曲线</param>
/// <param name="abscissa">间隔值</param>
/// <param name="tol">容差</param>
UniformAbscissa::UniformAbscissa(BRepAdaptor::Curve^ curve, double abscissa, double tol)
{
    myUA=new GCPnts_UniformAbscissa(curve, abscissa, tol);
}

double UniformAbscissa::Parameter(int index)
{
	TRY_OCC(return myUA->Parameter(index);)
}

/// <summary>
/// 间隔值
/// </summary>
/// <returns></returns>
double UniformAbscissa::Abscissa()
{
	TRY_OCC(return myUA->Abscissa();)
}

int UniformAbscissa::NbPoints()
{
	TRY_OCC(return myUA->NbPoints();)
}

bool UniformAbscissa::IsDone()
{
	TRY_OCC(return myUA->IsDone();)
}

}