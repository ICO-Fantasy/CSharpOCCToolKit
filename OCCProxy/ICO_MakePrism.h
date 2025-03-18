#pragma once

namespace OCCTK {
namespace OCC {
namespace gp {
value struct Pnt;
value struct Ax2;
value struct Vec;
}
namespace Topo {
ref class TShape;
}
}
}
class BRepPrimAPI_MakePrism;
namespace OCCTK {
namespace OCC {
namespace BRepPrimAPI {

public ref class MakePrism {
public:
    MakePrism(Topo::TShape^ shape, gp::Vec vec);
    MakePrism(Topo::TShape^ shape, gp::Vec vec, bool copy);
    MakePrism(Topo::TShape^ shape, gp::Vec vec, bool copy , bool canonize);
    Topo::TShape^ Shape();
    //! ��ʽת��Ϊ TShape
    static operator Topo::TShape ^ (MakePrism^ mp) {
        return mp->Shape();  // ��ʽ���� Shape ����
    }
private:
    BRepPrimAPI_MakePrism* myMaker;
protected:
    // ������������������й���Դ
    !MakePrism() {
        delete myMaker;
        myMaker = nullptr;
    }

    // �ս�����finalizer��������������ʱ������
    ~MakePrism() {
        // ��������������������й���Դ
        this->!MakePrism();
    }
};

}
}
}