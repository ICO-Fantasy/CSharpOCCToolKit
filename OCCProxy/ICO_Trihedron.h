#pragma once
#include "ICO_InteractiveObject.h"
#include <AIS_Trihedron.hxx>
#include <Standard_Handle.hxx>
//包装C++类到托管类
#include <NCollection_Haft.h> 
//Local
#include "ICO_AIS_Shape.h"
#include "ICO_Trsf.h"
#include "ICO_DatumParts.h"
#include "..\Extension\ICO_Color.h"

namespace OCCTK {
namespace OCC {
namespace AIS {

public ref class ATrihedron :public InteractiveObject {
private:
	Handle(AIS_Trihedron) myTrihedron() { return Handle(AIS_Trihedron)::DownCast(NativeHandle); }
public:
	ATrihedron(double axisSize);
	ATrihedron(AShape^ theAIS, double axisSize);
	ATrihedron(gp::Trsf^ theTrsf, double axisSize);
	Handle(AIS_Trihedron) GetOCC() { return myTrihedron(); };
public:
	void SetArrowWidth(DatumParts axis, double value);
	void SetColor(DatumParts axis, Extension::Color color);
	void SetDatumColor(DatumParts axis, Extension::Color color);
	void SetArrowColor(DatumParts axis, Extension::Color color);
	void SetTextColor(DatumParts axis, Extension::Color color);
	void SetAspect(int theX, int theY);
	property double ArrowLength {double get(); void set(double value); };
	property double ArrowWidth {double get(); void set(double value); };
	property bool DrawArrow {bool get(); void set(bool value); };
private:
	void SetDefaultAxis();
	bool MyDrawArrow = false;
	double MyArrowLength = 100.0;
	double MyArrowWidth = 2.5;
};

}
}
}

