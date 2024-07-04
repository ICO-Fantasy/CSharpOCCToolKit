#pragma once
#include "WAIS_Shape.h"
#include "WTopAbs_ShapeEnum.h"
#include <AIS_InteractiveObject.hxx>
#include <AIS_InteractiveContext.hxx>
#include "WColor.h"
//包装C++类到托管类
#include <NCollection_Haft.h> 
namespace OCCTK {
namespace OCC {
namespace AIS {


public ref class WAIS_InteractiveContext
{
public:
	WAIS_InteractiveContext(Handle(AIS_InteractiveContext) theAISContext);
	void EraseObjects(void);
	void SetSelectionMode(TopoAbs::WTopAbs_ShapeEnum theMode);
	void Select(void);
	void SelectAIS(WAIS_Shape^ theAIS);
	void MultipleSelect(void);
	void XORSelect(void);
	//void AreaSelect(int theX1, int theY1, int theX2, int theY2);
	//void MultipleAreaSelect(int theX1, int theY1, int theX2, int theY2);
	//void XORAreaSelect(int theX1, int theY1, int theX2, int theY2);
	void Display(Handle(AIS_InteractiveObject) theAISObject, bool theToUpdateViewer);
	void Display(WAIS_Shape^ theAIS, bool theToUpdateViewer);
	void EraseAll();
	void Erase(Handle(AIS_InteractiveObject) theAISObject, bool theToUpdateViewer);
	void Erase(WAIS_Shape^ theAIS, bool theToUpdateViewer);
#pragma region 设置AIS
	void SetColor(WAIS_Shape^ theAIS, Ex::WColor^ theColor, bool theToUpdateViewer);
	void UnsetColor(WAIS_Shape^ theAIS, bool theToUpdateViewer);
	void SetTransparency(WAIS_Shape^ theAIS, double theTransparency, bool theToUpdateViewer);
	void UnsetTransparency(WAIS_Shape^ theAIS, bool theToUpdateViewer);
#pragma endregion
	bool IsSelected(void);
protected:
	NCollection_Haft<Handle(AIS_InteractiveContext)> myAISContext;
};

}
}
}