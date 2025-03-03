#pragma once
#include <GCPnts_AbscissaPoint.hxx>

//ǰ������
namespace OCCTK::OCC::BRepAdaptor {
ref class Curve;
}

namespace OCCTK::OCC::GCPnts{

public ref class AbscissaPoint
{
public:
    // �������չ��캯��
    AbscissaPoint(GCPnts_AbscissaPoint theAP);
    AbscissaPoint(BRepAdaptor::Curve^ curve, double abscissa,double u0);
    AbscissaPoint(BRepAdaptor::Curve^ curve, double abscissa, double u0, double ui,double tol);
    GCPnts_AbscissaPoint GetOCC() { return *myAP; }
    //! ��ʽת��Ϊ GCPnts_AbscissaPoint
    static operator GCPnts_AbscissaPoint (AbscissaPoint^ ap) { return ap->GetOCC(); }
public:
    double static Length(BRepAdaptor::Curve^ curve);
    double Parameter();
    bool IsDone();
private:
    GCPnts_AbscissaPoint* myAP;
protected:
    // ������������������й���Դ
    !AbscissaPoint() {
        delete myAP;
        myAP = nullptr;
    }
    // �ս�����finalizer��������������ʱ������
    ~AbscissaPoint() {
        // ��������������������й���Դ
        this->!AbscissaPoint();
    }
};

}
