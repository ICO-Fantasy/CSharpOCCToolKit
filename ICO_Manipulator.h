#pragma once
#include <AIS_Manipulator.hxx>
#include <Standard_Transient.hxx>
//包装C++类到托管类
#include <NCollection_Haft.h> 
#include "ICO_InteractiveObject.h"
#include "ICO_AIS_Shape.h"
#include "ICO_View.h"
#include "ICO_Ax2.h"

using namespace System::Collections::Generic;
using namespace OCCTK::OCC::V3d;
using namespace OCCTK::OCC::gp;

namespace OCCTK {
namespace OCC {
namespace AIS {

public enum class ManipulatorMode {
	None = AIS_ManipulatorMode::AIS_MM_None,
	Translation = AIS_ManipulatorMode::AIS_MM_Translation,
	Rotation = AIS_ManipulatorMode::AIS_MM_Rotation,
	Scaling = AIS_ManipulatorMode::AIS_MM_Scaling,
	TranslationPlane = AIS_ManipulatorMode::AIS_MM_TranslationPlane,
};
public enum class ManipulatorAxisIndex {
	X = 0,
	Y = 1,
	Z = 2,
	None = 99,
};

public ref class Manipulator : public InteractiveObject {
public:
	Manipulator();
	Manipulator(Manipulator^ theManipulator);
	Manipulator(Handle(AIS_Manipulator) theManipulator);
	Manipulator(List<InteractiveObject^>^ theAISList);
	Handle(AIS_Manipulator) GetOCC();
	virtual Handle(Standard_Transient) GetStd() override;

	bool HasActiveMode();
	ManipulatorMode ActiveMode();
	ManipulatorAxisIndex ActiveAxisIndex();
	bool IsAttached();
	Ax2^ Position();
#pragma region 变换

	void StartTransform(double theX, double theY, View^ theView);
	//void Transform(double theX, double theY, View^ theView);
	Trsf^ Transform(double theX, double theY, View^ theView);
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

	void DeactivateCurrentMode();
	void SetPart(ManipulatorMode^ theMode, bool isEnable);
	void SetPart(ManipulatorAxisIndex^ theAxisIndex, ManipulatorMode^ theMode, bool isEnable);
	void EnableMode(ManipulatorMode^ theMode);
private:
	NCollection_Haft<Handle(AIS_Manipulator)> myManipulator;
};

}
}
}
