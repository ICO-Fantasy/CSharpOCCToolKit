﻿#pragma once
#include <AIS_InteractiveContext.hxx>
#include <AIS_InteractiveObject.hxx>
//包装C++类到托管类
#include <NCollection_Haft.h> 
//Local
#include "ICO_AIS_Shape.h"
#include "ICO_Color.h"
#include "ICO_ShapeEnum.h"
#include "ICO_InteractiveObject.h"

namespace OCCTK {
namespace OCC {
namespace AIS {


public ref class InteractiveContext {
public:
	InteractiveContext(Handle(AIS_InteractiveContext) theAISContext);
	void EraseObjects(void);

#pragma region 选择

	void SetSelectionMode(TopoAbs::ShapeEnum theMode);
	void Select(void);
	void SelectAIS(AShape^ theAIS, bool update);
	void MultipleSelect(void);
	void XORSelect(void);

	void InitSelected();
	bool MoreSelected();
	bool NextSelected();
	InteractiveObject^ SelectedInteractive();
	AShape^ SelectedAIS();
	//void AreaSelect(int theX1, int theY1, int theX2, int theY2);
	//void MultipleAreaSelect(int theX1, int theY1, int theX2, int theY2);
	//void XORAreaSelect(int theX1, int theY1, int theX2, int theY2);

	void InitDetected();
	bool MoreDetected();
	void NextDetected();

#pragma endregion

#pragma region 显示

	void Display(Handle(AIS_InteractiveObject) theAISObject, bool theToUpdateViewer);
	void Display(InteractiveObject^ theAISObject, bool theToUpdateViewer);
	void Display(AShape^ theAIS, bool theToUpdateViewer);

	void EraseAll(bool theToUpdateViewer);
	void Erase(InteractiveObject^ theAIS, bool theToUpdateViewer);
	void Erase(Handle(AIS_InteractiveObject) theAISObject, bool theToUpdateViewer);
	void Erase(AShape^ theAIS, bool theToUpdateViewer);

	void RemoveAll(bool theToUpdateViewer);
	void Remove(Handle(AIS_InteractiveObject) theAISObject, bool theToUpdateViewer);
	void Remove(InteractiveObject^ theAISObject, bool theToUpdateViewer);
	void Remove(AShape^ theAIS, bool theToUpdateViewer);

#pragma endregion

#pragma region 设置AIS

	void SetColor(AShape^ theAIS, Extension::Color^ theColor, bool theToUpdateViewer);
	void UnsetColor(AShape^ theAIS, bool theToUpdateViewer);
	void SetTransparency(AShape^ theAIS, double theTransparency, bool theToUpdateViewer);
	void UnsetTransparency(AShape^ theAIS, bool theToUpdateViewer);
	void SetZLayer(AShape^ theAIS, int theZLayerID);

#pragma endregion

	bool IsSelected(void);

	Handle(AIS_InteractiveContext) GetOCC();
	System::IntPtr GetIntPtr();
protected:
	NCollection_Haft<Handle(AIS_InteractiveContext)> myAISContext;
};

}
}
}