﻿#pragma once
#include <AIS_Manipulator.hxx>
//local
#include "ICO_InteractiveObject.h"
#include "ICO_ManipulatorAxisIndex.h"
#include "ICO_ManipulatorMode.h"


namespace OCCTK {
namespace OCC {
namespace gp {
value struct Pnt;
value struct Ax2;
value struct Trsf;
}
namespace Topo {
ref class TShape;
}
namespace V3d {
ref class View;
}
namespace AIS {


public ref class Manipulator : public InteractiveObject {
private:
    Handle(AIS_Manipulator) myManipulator() { return Handle(AIS_Manipulator)::DownCast(NativeHandle); }
public:
    Manipulator();
    Manipulator(const Handle(AIS_Manipulator)& theManipulator) :InteractiveObject(theManipulator) {};
    Manipulator(List<InteractiveObject^>^ theAISList);
    Handle(AIS_Manipulator) GetOCC() { return myManipulator(); };
public:
    bool HasActiveMode();
    ManipulatorMode ActiveMode();
    ManipulatorAxisIndex ActiveAxisIndex();
    bool IsAttached();
    gp::Ax2 Position();
    InteractiveObject^ Object();

#pragma region 变换

    void StartTransform(double theX, double theY, V3d::View^ theView);
    //void Transform(double theX, double theY, View^ theView);
    gp::Trsf Transform(double theX, double theY, V3d::View^ theView);
    void StopTransform();
    void StopTransform(bool thetoApply);

#pragma endregion

#pragma region 附加

    void Attach(AShape^ theAIS);
    void Attach(AShape^ theAIS, bool adjustPosition, bool adjustSize, bool enableModes);
    void Attach(InteractiveObject^ theAISObject);
    void Attach(InteractiveObject^ theAISObject, bool adjustPosition, bool adjustSize, bool enableModes);
    void Detach();

#pragma endregion

    void SetModeActivationOnDetection(bool isEnable);
    bool IsModeActivationOnDetection();

    void DeactivateCurrentMode();
    void SetPart(ManipulatorMode theMode, bool isEnable);
    void SetPart(ManipulatorAxisIndex theAxisIndex, ManipulatorMode theMode, bool isEnable);
    void EnableMode(ManipulatorMode theMode);
};

}
}
}
