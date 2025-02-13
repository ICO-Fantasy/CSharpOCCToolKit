#pragma once
//在构造函数中使用的值对象需要直接引入
#include "ICO_Pnt.h"

//前向声明
class gp_Trsf;

namespace OCCTK {
namespace OCC {
namespace gp {
//前向声明
value struct Ax1;
value struct Pnt;
value struct Ax2;
value struct Quat;
value struct Vec;
value struct WPR;
value struct XYZ;

public ref class Trsf :System::ICloneable {
public:
    Trsf();
    Trsf(Trsf^ theT);
    Trsf(gp_Trsf theT);
    Trsf(gp_Trsf* theT);
    Trsf(array<double, 2>^ matrix);
    Trsf(array<array<double>^>^ matrix);
    Trsf(Vec translation, WPR rotation);
    Trsf(Ax2 fromAx2, Ax2 toAx2);
    Trsf(Ax2 toAx2);
    Trsf(Pnt fromPoint, Pnt toPoint);
    gp_Trsf GetOCC();
    virtual System::Object^ Clone();
    virtual System::String^ ToString() override;
    //! 隐式转换为 gp_Trsf
    static operator gp_Trsf (Trsf^ t) { return t->GetOCC(); }
public:
    void SetTranslation(Pnt fromPoint, Pnt toPoint);
    void SetTranslation(Vec vec);
    void SetTranslationPart(Vec vec);
    void SetRotationPart(Quat quat);
    void SetRotation(Quat quat);
    void SetRotation(Ax1 axis, double angle);
    //void PreMultiply(Trsf^ leftTrsf);
    Trsf^ Multiplied(Trsf^ rightTrsf);
    Trsf^ Inverted();
public:
    property XYZ Translation {XYZ get(); };
    property Quat Rotation {Quat get(); };
#pragma region 重载操作符
    static Trsf^ operator * (Trsf^ Left, Trsf^ Right);
    static Trsf^ operator - (Trsf^ Left);
#pragma endregion
private:
    gp_Trsf* myTrsf;
protected:
    // 析构函数用于清理非托管资源
    !Trsf() {
        delete myTrsf;
        myTrsf = nullptr;
    }

    // 终结器（finalizer）用于垃圾回收时的清理
    ~Trsf() {
        // 调用析构函数来清理非托管资源
        this->!Trsf();
    }
};

}
}
}
