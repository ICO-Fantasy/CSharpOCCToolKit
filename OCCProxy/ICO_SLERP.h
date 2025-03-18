#pragma once

class gp_QuaternionSLerp;
namespace OCCTK {
namespace OCC {
namespace gp {
//ǰ������
value struct Quat;

public ref class SlerpCalculator
{
public:
    SlerpCalculator(Quat start, Quat end);
public:
    Quat Interpolate(double percentage);
private:
    gp_QuaternionSLerp* mySlerpCalculator;
protected:
    // ������������������й���Դ
    !SlerpCalculator() {
        delete mySlerpCalculator;
        mySlerpCalculator = nullptr;
    }

    // �ս�����finalizer��������������ʱ������
    ~SlerpCalculator() {
        // ��������������������й���Դ
        this->!SlerpCalculator();
    }
};

}
}
}
