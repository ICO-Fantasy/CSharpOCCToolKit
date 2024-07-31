#pragma once
//包装C++类到托管类
#include "WAIS_InteractiveObject.h"
#include "WAIS_Shape.h"
#include "WV3d_View.h"
#include <AIS_Manipulator.hxx>
#include <Standard_Transient.hxx>
//包装C++类到托管类
#include <NCollection_Haft.h> 

using namespace System::Collections::Generic;
using namespace OCCTK::OCC::V3d;

namespace OCCTK {
namespace OCC {
namespace AIS
{

public enum class WAIS_ManipulatorMode {
	None = AIS_ManipulatorMode::AIS_MM_None,
	Translation = AIS_ManipulatorMode::AIS_MM_Translation,
	Rotation = AIS_ManipulatorMode::AIS_MM_Rotation,
	Scaling = AIS_ManipulatorMode::AIS_MM_Scaling,
	TranslationPlane = AIS_ManipulatorMode::AIS_MM_TranslationPlane,
};

public ref class WAIS_Manipulator : public WAIS_InteractiveObject
{
public:
	WAIS_Manipulator();
	WAIS_Manipulator(WAIS_Manipulator^ theManipulator);
	WAIS_Manipulator(Handle(AIS_Manipulator) theManipulator);
	WAIS_Manipulator(List<WAIS_InteractiveObject^>^ theAISList);
	Handle(AIS_Manipulator) GetOCC();
	bool HasActiveMode();
	bool IsAttached();
	void StartTransform(double theX, double theY, WV3d_View^ theView);
	void Transform(double theX, double theY, WV3d_View^ theView);
	void StopTransform();
	void StopTransform(bool thetoApply);
	void Attach(WAIS_Shape^ theAIS);
	void Attach(WAIS_Shape^ theAIS, bool adjustPosition, bool adjustSize, bool enableModes);
	void Detach();
	void DeactivateCurrentMode();
	void SetPart(WAIS_ManipulatorMode^ theMode, bool isEnable);
	void EnableMode(WAIS_ManipulatorMode^ theMode);
	virtual Handle(Standard_Transient) GetStd() override;
private:
	NCollection_Haft<Handle(AIS_Manipulator)> _pManipulator;
};

}
}
}
