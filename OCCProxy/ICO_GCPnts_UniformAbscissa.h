#pragma once
#include <GCPnts_UniformAbscissa.hxx>

//ǰ������
namespace OCCTK::OCC::BRepAdaptor {
ref class Curve;
}
namespace OCCTK::OCC::GCPnts {

public ref class UniformAbscissa
{
public:
    UniformAbscissa(BRepAdaptor::Curve^ curve, double abscissa, double tol);
    UniformAbscissa(BRepAdaptor::Curve^ curve, int numberPoints, double tol);
public:
    double Parameter(int index);
    double Abscissa();
    int NbPoints();
    bool IsDone();
private:
    GCPnts_UniformAbscissa* myUA;
protected:
    // ������������������й���Դ
    !UniformAbscissa() {
        delete myUA;
        myUA = nullptr;
    }
    // �ս�����finalizer��������������ʱ������
    ~UniformAbscissa() {
        // ��������������������й���Դ
        this->!UniformAbscissa();
    }
};

}
