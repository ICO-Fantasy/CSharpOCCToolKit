#include "ICO_GCPnts_UniformAbscissa.h"
#include <GCPnts_UniformAbscissa.hxx>
//local
#include "ICO_BRepAdaptor_Curve.h"
#include "ICO_Exception.h"

namespace OCCTK::OCC::GCPnts {

/// <summary>
/// �����߽��о��Ȳ���(�����ֵ)
/// </summary>
/// <param name="curve">����</param>
/// <param name="abscissa">���ֵ</param>
/// <param name="tol">�ݲ�</param>
UniformAbscissa::UniformAbscissa(BRepAdaptor::Curve^ curve, double abscissa, double tol)
{
    myUA=new GCPnts_UniformAbscissa(curve, abscissa, tol);
}

double UniformAbscissa::Parameter(int index)
{
	TRY_OCC(return myUA->Parameter(index);)
}

/// <summary>
/// ���ֵ
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