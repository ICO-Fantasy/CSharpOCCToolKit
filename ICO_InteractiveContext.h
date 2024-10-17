#pragma once
#include <AIS_InteractiveContext.hxx>
#include<Standard_Handle.hxx>
//local
#include "ICO_BaseObject.h"

namespace OCCTK {
namespace Extension {
value struct Color;
}
namespace OCC {
namespace AIS {
ref class InteractiveObject;
ref class AShape;
enum class DisplayMode;
enum class SelectionMode;
}
namespace V3d {
ref class Viewer;
ref class View;
}
}
}


namespace OCCTK {
namespace OCC {
namespace AIS {

public ref class InteractiveContext :BaseObject {
private:
	Handle(AIS_InteractiveContext) myAISContext() { return Handle(AIS_InteractiveContext)::DownCast(NativeHandle); }
public:
	InteractiveContext(V3d::Viewer^ theViewer);
	InteractiveContext(const Handle(AIS_InteractiveContext)& theAISContext) :BaseObject(theAISContext) {}
	Handle(AIS_InteractiveContext) GetOCC();
	System::IntPtr GetIntPtr();

	void SetDefault();
	void SetDefaultHighlightStyle();

#pragma region 渲染
	void UpdateCurrentViewer();
#pragma endregion

#pragma region 选择

	void MoveTo(int theX, int theY, V3d::View^ theView);
	void SetDisplayMode(DisplayMode theMode);
	void SetDefaultSelectionMode(SelectionMode theMode);
	void SetSelectionMode(InteractiveObject^ theAISObject, SelectionMode theMode);
	void Select(void);
	void SelectAIS(InteractiveObject^ theAISObject, bool update);
	void MultipleSelect(void);
	void XORSelect(void);
	void AreaSelect(int minX, int minY, int maxX, int maxY, V3d::View^ theView);
	void MultipleAreaSelect(int minX, int minY, int maxX, int maxY, V3d::View^ theView);
	void XORAreaSelect(int minX, int minY, int maxX, int maxY, V3d::View^ theView);

	void InitSelected();
	bool MoreSelected();
	bool NextSelected();
	InteractiveObject^ SelectedInteractive();
	AShape^ SelectedAIS();

	void InitDetected();
	bool MoreDetected();
	void NextDetected();

#pragma endregion

#pragma region 显示

	void Display(Handle(AIS_InteractiveObject) theAISObject, bool theToUpdateViewer);
	void Display(InteractiveObject^ theAISObject, bool theToUpdateViewer);
	void Redisplay(InteractiveObject^ theAISObject, bool theToUpdateViewer);

	void EraseAll(bool theToUpdateViewer);
	void Erase(InteractiveObject^ theAISObject, bool theToUpdateViewer);
	void Erase(Handle(AIS_InteractiveObject) theAISObject, bool theToUpdateViewer);
	void EraseSelected(void);

	void RemoveAll(bool theToUpdateViewer);
	void Remove(Handle(AIS_InteractiveObject) theAISObject, bool theToUpdateViewer);
	void Remove(InteractiveObject^ theAISObject, bool theToUpdateViewer);

#pragma endregion

#pragma region 设置AIS

	void SetColor(InteractiveObject^ theAISObject, Extension::Color theColor, bool theToUpdateViewer);
	void UnsetColor(InteractiveObject^ theAISObject, bool theToUpdateViewer);
	void SetTransparency(InteractiveObject^ theAISObject, double theTransparency, bool theToUpdateViewer);
	void UnsetTransparency(InteractiveObject^ theAISObject, bool theToUpdateViewer);
	void SetWidth(InteractiveObject^ theAISObject, double width, bool theToUpdateViewer);
	void UnsetWidth(InteractiveObject^ theAISObject, bool theToUpdateViewer);
	void SetZLayer(InteractiveObject^ theAISObject, int theZLayerID);

#pragma endregion

	bool IsSelected(void);
	bool IsDisplayed(InteractiveObject^ theAISObject);
};

}
}
}