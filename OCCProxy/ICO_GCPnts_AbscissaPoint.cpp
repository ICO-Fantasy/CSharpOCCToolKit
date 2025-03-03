#include "ICO_GCPnts_AbscissaPoint.h"
#include <GCPnts_AbscissaPoint.hxx>
//local
#include "ICO_BRepAdaptor_Curve.h"
#include "ICO_Exception.h"

using namespace OCCTK::OCC::BRepAdaptor;

namespace OCCTK::OCC::GCPnts {

AbscissaPoint::AbscissaPoint(GCPnts_AbscissaPoint theAP)
{
   myAP= &theAP;
}

AbscissaPoint::AbscissaPoint(Curve^ curve, double abscissa, double u0)
{
    myAP = new GCPnts_AbscissaPoint(curve, abscissa, u0);
}

AbscissaPoint::AbscissaPoint(BRepAdaptor::Curve^ curve, double abscissa, double u0, double ui, double tol)
{
    myAP=new GCPnts_AbscissaPoint(curve, abscissa, u0, ui, tol);
}

/// <summary>
/// 获取曲线长度
/// </summary>
/// <param name="curve"></param>
/// <returns></returns>
double AbscissaPoint::Length(BRepAdaptor::Curve^ curve)
{
    return  GCPnts_AbscissaPoint::Length(curve);
}

/// <summary>
/// 返回此算法的点解曲线上的参数。如果计算不成功或没有完成，则抛出异常 StdFail_NotDone。
/// </summary>
/// <returns></returns>
double AbscissaPoint::Parameter()
{
	try	{
		return myAP->Parameter();
	}
	CATCH_AND_THROW_OCC_EXCEPTIONS
}

bool AbscissaPoint::IsDone()
{
	try {
		return myAP->IsDone();
	}
	CATCH_AND_THROW_OCC_EXCEPTIONS
}

}
