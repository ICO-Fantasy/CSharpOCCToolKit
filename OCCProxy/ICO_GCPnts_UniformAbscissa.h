#pragma once
#include <GCPnts_UniformAbscissa.hxx>

//前向声明
namespace OCCTK::OCC::BRepAdaptor {
ref class Curve;
}
namespace OCCTK::OCC::GCPnts {

public ref class UniformAbscissa
{
public:
    UniformAbscissa(BRepAdaptor::Curve^ curve, double abscissa,double tol);
public:
    double Parameter(int index);
    double Abscissa();
    int NbPoints();
    bool IsDone();
private:
    GCPnts_UniformAbscissa* myUA;
protected:
    // 析构函数用于清理非托管资源
    !UniformAbscissa() {
        delete myUA;
        myUA = nullptr;
    }
    // 终结器（finalizer）用于垃圾回收时的清理
    ~UniformAbscissa() {
        // 调用析构函数来清理非托管资源
        this->!UniformAbscissa();
    }
};

}
