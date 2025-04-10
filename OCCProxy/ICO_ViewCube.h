#pragma once
#include <AIS_ViewCube.hxx>
//Local
#include "ICO_InteractiveObject.h"
#include <V3d_TypeOfOrientation.hxx>

namespace OCCTK {
namespace Extension {
value struct Color;
}
}

namespace OCCTK {
namespace OCC {
namespace AIS {
enum class DatumParts;

//与V3d_TypeOfOrientation保持一致
/// <summary>
/// 视图立方表面方向
/// </summary>
public enum class BoxSide {
    Front= V3d_Yneg,
	Back = V3d_Ypos,
	Left = V3d_Xneg,
	Right = V3d_Xpos,
	Top	= V3d_Zpos,
	Bottom = V3d_Zneg
};

public ref class ViewCube :public InteractiveObject {
private:
	Handle(AIS_ViewCube) myViewCube() { return Handle(AIS_ViewCube)::DownCast(NativeHandle); }
public:
	ViewCube();
	ViewCube(double size);
	Handle(AIS_ViewCube) GetOCC() { return myViewCube(); };
public:
	void SetSize(double size);
	void SetAxesRadius(double radius);
	void SetAxesConeRadius(double radius);
	void SetAxesSphereRadius(double radius);
	void SetAxesTextColor(DatumParts axisPart, Extension::Color color);
	void SetAxesTextHeight(DatumParts axisPart,double height);
	void SetAxesColor(DatumParts axisPart, Extension::Color color);
	void SetAxesLabels(System::String^ x, System::String^ y, System::String^ z);
	void SetFaceBoundaryDraw(bool isEnabled);
	void SetFaceBoundaryColor(Extension::Color color);
	void SetBoxSideLabel(BoxSide boxSide,System::String^ label);
	void SetBoxSideFont(System::String^ fontFamilyName);
	void SetBoxSideTextHeight(double height);
	void SetBoxSideTextColor(Extension::Color color);
	void SetBoxSideFaceColor(Extension::Color color);
	void SetTransparency(double transparency);
	void SetBoxFacetExtension(double transparency);
	void SetRoundRadius(double roundRadius);
	void SetBoxEdgeGap(double roundRadius);
	void SetICODefault();
	void SetEqualScaling(double axisSize);
};

}
}
}

