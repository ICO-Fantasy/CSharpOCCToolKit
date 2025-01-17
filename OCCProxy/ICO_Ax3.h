#pragma once
//值类型作为构造函数成员需要完整定义
#include <gp_Ax3.hxx>
#include "ICO_Ax1.h"
#include "ICO_Pnt.h"
#include "ICO_Dir.h"

namespace OCCTK {
namespace OCC {
namespace gp {
//前向声明
ref class Trsf;

public value struct Ax3 :System::ICloneable, System::Runtime::Serialization::ISerializable {
public:
    const static Ax3 Default = Ax3(::gp_Ax3());
public:
    Ax3(Pnt location, Dir zAxis);
    Ax3(Pnt location, Dir zAxis, Dir xAxis);
    Ax3(gp_Ax3 theAx3);
    Ax3(gp_Ax3* theAx3);
    gp_Ax3 GetOCC();
    virtual System::Object^ Clone();
    virtual System::String^ ToString() override;
    //! 隐式转换为 gp_Ax3
    static operator gp_Ax3 (Ax3 axis) { return axis.GetOCC(); }
public:
    // ISerializable 方法：用于序列化
    virtual void GetObjectData(System::Runtime::Serialization::SerializationInfo^ info, System::Runtime::Serialization::StreamingContext context);
    // 用于反序列化的构造函数
    Ax3(System::Runtime::Serialization::SerializationInfo^ info, System::Runtime::Serialization::StreamingContext context);
public:
    //void Transform(Trsf^ theT);
    Ax3 Transformed(Trsf^ theT);
public:
    property Pnt Location;
    property Dir XDir;
    property Dir YDir {Dir get(); }
    property Dir ZDir;
    property Ax1 XAxis {Ax1 get(); }
    property Ax1 YAxis {Ax1 get(); }
    property Ax1 ZAxis {Ax1 get(); }
};

}
}
}