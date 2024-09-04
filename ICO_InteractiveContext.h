#pragma once
#include <AIS_InteractiveContext.hxx>
#include <AIS_InteractiveObject.hxx>
//包装C++类到托管类
#include <NCollection_Haft.h> 
//Local
#include "ICO_AIS_Shape.h"
#include "ICO_Color.h"
#include "ICO_View.h"
#include "ICO_ShapeEnum.h"
#include "ICO_SelectionMode.h"
#include "ICO_InteractiveObject.h"
#include "ICO_Viewer.h"
#include "ICO_DisplayMode.h"

namespace OCCTK {
namespace OCC {
namespace AIS {


public ref class InteractiveContext {
public:
	InteractiveContext(Handle(AIS_InteractiveContext) theAISContext);
	InteractiveContext(V3d::Viewer^ theViewer);

	void SetDefault();
	void SetDefaultHighlightStyle();

#pragma region 渲染
	void UpdateCurrentViewer();
#pragma endregion

#pragma region 选择

	void MoveTo(int theX, int theY, V3d::View^ theView);
	void SetDisplayMode(DisplayMode theMode);
	void SetSelectionMode(SelectionMode theMode);
	void Select(void);
	void SelectAIS(InteractiveObject^ theAISObject, bool update);
	void MultipleSelect(void);
	void XORSelect(void);
	void AreaSelect(int theX1, int theY1, int theX2, int theY2, V3d::View^ theView);
	void MultipleAreaSelect(int theX1, int theY1, int theX2, int theY2, V3d::View^ theView);
	void XORAreaSelect(int theX1, int theY1, int theX2, int theY2, V3d::View^ theView);

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

	void SetColor(InteractiveObject^ theAISObject, Extension::Color^ theColor, bool theToUpdateViewer);
	void UnsetColor(InteractiveObject^ theAISObject, bool theToUpdateViewer);
	void SetTransparency(InteractiveObject^ theAISObject, double theTransparency, bool theToUpdateViewer);
	void UnsetTransparency(InteractiveObject^ theAISObject, bool theToUpdateViewer);
	void SetZLayer(InteractiveObject^ theAISObject, int theZLayerID);

#pragma endregion

	bool IsSelected(void);
	bool IsDisplayed(InteractiveObject^ theAISObject);

	Handle(AIS_InteractiveContext) GetOCC();
	System::IntPtr GetIntPtr();
protected:
	NCollection_Haft<Handle(AIS_InteractiveContext)> myAISContext;
};

}
}
}