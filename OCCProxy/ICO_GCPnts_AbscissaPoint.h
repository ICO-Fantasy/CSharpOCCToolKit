#pragma once
#include <GCPnts_AbscissaPoint.hxx>

//前向声明
namespace OCCTK::OCC::BRepAdaptor {
ref class Curve;
}

namespace OCCTK::OCC::GCPnts{

public ref class AbscissaPoint
{
public:
    // 不包含空构造函数
    AbscissaPoint(GCPnts_AbscissaPoint theAP);
    AbscissaPoint(BRepAdaptor::Curve^ curve, double abscissa,double u0);
    AbscissaPoint(BRepAdaptor::Curve^ curve, double abscissa, double u0, double ui,double tol);
    GCPnts_AbscissaPoint GetOCC() { return *myAP; }
    //! 隐式转换为 GCPnts_AbscissaPoint
    static operator GCPnts_AbscissaPoint (AbscissaPoint^ ap) { return ap->GetOCC(); }
public:
    double static Length(BRepAdaptor::Curve^ curve);
    double Parameter();
    bool IsDone();
private:
    GCPnts_AbscissaPoint* myAP;
protected:
    // 析构函数用于清理非托管资源
    !AbscissaPoint() {
        delete myAP;
        myAP = nullptr;
    }
    // 终结器（finalizer）用于垃圾回收时的清理
    ~AbscissaPoint() {
        // 调用析构函数来清理非托管资源
        this->!AbscissaPoint();
    }
};

}
