﻿#include "WAIS_InteractiveContext.h"
#include "WTopAbs_ShapeEnum.h"

namespace OCCTK {
namespace OCC {
namespace AIS {

WAIS_InteractiveContext::WAIS_InteractiveContext(Handle(AIS_InteractiveContext) theAISContext)
{
	myAISContext() = theAISContext;
}
/// <summary>
/// 删除选中的物体（首先需要选中物体）
/// </summary>
/// <param name=""></param>
void WAIS_InteractiveContext::EraseObjects(void) {
	if (myAISContext().IsNull())
	{
		return;
	}

	myAISContext()->EraseSelected(Standard_False);
	myAISContext()->ClearSelected(Standard_True);
	myAISContext()->UpdateCurrentViewer();
}

/// <summary>
/// 单选
/// </summary>
/// <param name=""></param>
void WAIS_InteractiveContext::SetSelectionMode(TopoAbs::WTopAbs_ShapeEnum theMode) {
	if (myAISContext().IsNull())
	{
		return;
	}

	myAISContext()->Deactivate();
	myAISContext()->Activate(theMode.GetOCC(), true);
	myAISContext()->UpdateSelected(true);
}

void WAIS_InteractiveContext::Select(void) {
	if (myAISContext().IsNull())
	{
		return;
	}
	myAISContext()->SelectDetected(AIS_SelectionScheme_Replace); // 将检测到的对象替换当前选择
	myAISContext()->UpdateCurrentViewer();
}

/// <summary>
/// 选中输入的对象
/// </summary>
void WAIS_InteractiveContext::SelectAIS(WAIS_Shape^ theAIS) {
	if (myAISContext().IsNull())
	{
		return;
	}
	myAISContext()->ClearSelected(Standard_True); // 清除当前选中的对象
	myAISContext()->AddOrRemoveSelected(theAIS->GetOCC(), true);// 选中AIS
	myAISContext()->UpdateCurrentViewer();
}

/// <summary>
/// 多选
/// </summary>
/// <param name=""></param>
void WAIS_InteractiveContext::MultipleSelect(void) {
	if (myAISContext().IsNull())
	{
		return;
	}
	myAISContext()->SelectDetected(AIS_SelectionScheme_Add); //将检测到的对象添加到当前选择
	myAISContext()->UpdateCurrentViewer();
}

/// <summary>
/// 异或多选
/// </summary>
/// <param name=""></param>
void WAIS_InteractiveContext::XORSelect(void) {
	if (myAISContext().IsNull())
	{
		return;
	}
	myAISContext()->SelectDetected(AIS_SelectionScheme_XOR);//对检测到的对象执行异或，再次点击可以取消选择
	myAISContext()->UpdateCurrentViewer();
}

void WAIS_InteractiveContext::Display(Handle(AIS_InteractiveObject) theAISObject, bool theToUpdateViewer) {
	if (myAISContext().IsNull())
	{
		return;
	}
	myAISContext()->Display(theAISObject, theToUpdateViewer);
}

void WAIS_InteractiveContext::Display(WAIS_Shape^ theAIS, bool theToUpdateViewer) {
	if (myAISContext().IsNull())
	{
		return;
	}
	myAISContext()->Display(theAIS->GetOCC(), theToUpdateViewer);
}

void WAIS_InteractiveContext::EraseAll() {
	if (myAISContext().IsNull())
	{
		return;
	}
	myAISContext()->EraseAll(true);
}

void WAIS_InteractiveContext::Erase(Handle(AIS_InteractiveObject) theAISObject, bool theToUpdateViewer) {
	if (myAISContext().IsNull())
	{
		return;
	}
	myAISContext()->Erase(theAISObject, theToUpdateViewer);
}

void WAIS_InteractiveContext::Erase(WAIS_Shape^ theAIS, bool theToUpdateViewer) {
	if (myAISContext().IsNull())
	{
		return;
	}
	myAISContext()->Erase(theAIS->GetOCC(), theToUpdateViewer);
}
#pragma region 设置AIS
void WAIS_InteractiveContext::SetColor(WAIS_Shape^ theAIS, Ex::WColor^ theColor, bool theToUpdateViewer)
{
	if (myAISContext().IsNull())
	{
		return;
	}
	myAISContext()->SetColor(theAIS->GetOCC(), theColor->GetOCC(), theToUpdateViewer);
}

void WAIS_InteractiveContext::UnsetColor(WAIS_Shape^ theAIS, bool theToUpdateViewer)
{
	if (myAISContext().IsNull())
	{
		return;
	}
	myAISContext()->UnsetColor(theAIS->GetOCC(), theToUpdateViewer);
}

void WAIS_InteractiveContext::SetTransparency(WAIS_Shape^ theAIS, double theTransparency, bool theToUpdateViewer)
{
	if (myAISContext().IsNull())
	{
		return;
	}
	myAISContext()->SetTransparency(theAIS->GetOCC(), theTransparency, theToUpdateViewer);
}

void WAIS_InteractiveContext::UnsetTransparency(WAIS_Shape^ theAIS, bool theToUpdateViewer)
{
	if (myAISContext().IsNull())
	{
		return;
	}
	myAISContext()->UnsetTransparency(theAIS->GetOCC(), theToUpdateViewer);
}


#pragma endregion

bool WAIS_InteractiveContext::IsSelected(void)
{
	if (myAISContext().IsNull())
	{
		return false;
	}
	myAISContext()->InitSelected();
	return myAISContext()->MoreSelected();
}

}
}
}