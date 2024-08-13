#pragma once
#include "WAIS_Shape.h"
#include <AIS_InteractiveContext.hxx>
#include <AIS_InteractiveObject.hxx>
// 包装C++类到托管类
#include <NCollection_Haft.h> 
// local
#include "WColor.h"
#include "WTopAbs_ShapeEnum.h"
#include "WAIS_InteractiveObject.h"
namespace OCCTK {
namespace OCC {
namespace AIS {


public ref class WAIS_InteractiveContext {
public:
	WAIS_InteractiveContext(Handle(AIS_InteractiveContext) theAISContext);
	void EraseObjects(void);
#pragma region 选择
	void SetSelectionMode(TopoAbs::WTopAbs_ShapeEnum theMode);
	void Select(void);
	void SelectAIS(WAIS_Shape^ theAIS);
	void MultipleSelect(void);
	void XORSelect(void);
	void InitSelected();
	bool MoreSelected();
	WAIS_InteractiveObject^ SelectedInteractive();
	WAIS_Shape^ SelectedAIS();
	//void AreaSelect(int theX1, int theY1, int theX2, int theY2);
	//void MultipleAreaSelect(int theX1, int theY1, int theX2, int theY2);
	//void XORAreaSelect(int theX1, int theY1, int theX2, int theY2);
#pragma endregion
	void Display(Handle(AIS_InteractiveObject) theAISObject, bool theToUpdateViewer);
	void Display(WAIS_InteractiveObject^ theAISObject, bool theToUpdateViewer);
	void Display(WAIS_Shape^ theAIS, bool theToUpdateViewer);

	void EraseAll(bool theToUpdateViewer);
	void Erase(WAIS_InteractiveObject^ theAIS, bool theToUpdateViewer);
	void Erase(Handle(AIS_InteractiveObject) theAISObject, bool theToUpdateViewer);
	void Erase(WAIS_Shape^ theAIS, bool theToUpdateViewer);

	void RemoveAll(bool theToUpdateViewer);
	void Remove(Handle(AIS_InteractiveObject) theAISObject, bool theToUpdateViewer);
	void Remove(WAIS_InteractiveObject^ theAISObject, bool theToUpdateViewer);
	void Remove(WAIS_Shape^ theAIS, bool theToUpdateViewer);
#pragma region 设置AIS
	void SetColor(WAIS_Shape^ theAIS, Ex::WColor^ theColor, bool theToUpdateViewer);
	void UnsetColor(WAIS_Shape^ theAIS, bool theToUpdateViewer);
	void SetTransparency(WAIS_Shape^ theAIS, double theTransparency, bool theToUpdateViewer);
	void UnsetTransparency(WAIS_Shape^ theAIS, bool theToUpdateViewer);
	void SetZLayer(WAIS_Shape^ theAIS, int theZLayerID);
#pragma endregion
	bool IsSelected(void);
protected:
	NCollection_Haft<Handle(AIS_InteractiveContext)> myAISContext;
};

}
}
}