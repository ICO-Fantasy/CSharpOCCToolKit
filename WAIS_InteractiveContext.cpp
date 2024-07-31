#include <AIS_Manipulator.hxx>
#include "WAIS_InteractiveContext.h"
#include "WTopAbs_ShapeEnum.h"

namespace OCCTK {
namespace OCC {
namespace AIS {

WAIS_InteractiveContext::WAIS_InteractiveContext(Handle(AIS_InteractiveContext) theAISContext)
{
	myAISContext() = theAISContext;
}
// 删除选中的物体（首先需要选中物体）
void WAIS_InteractiveContext::EraseObjects(void) {
	if (myAISContext().IsNull()) return;

	myAISContext()->EraseSelected(Standard_False);
	myAISContext()->ClearSelected(Standard_True);
	myAISContext()->UpdateCurrentViewer();
}

// 单选
void WAIS_InteractiveContext::SetSelectionMode(TopoAbs::WTopAbs_ShapeEnum theMode) {
	if (myAISContext().IsNull()) return;

	myAISContext()->Deactivate();
	myAISContext()->Activate(theMode.GetOCC(), true);
	myAISContext()->UpdateSelected(true);
}

void WAIS_InteractiveContext::Select(void) {
	if (myAISContext().IsNull()) return;
	myAISContext()->SelectDetected(AIS_SelectionScheme_Replace); // 将检测到的对象替换当前选择
	myAISContext()->UpdateCurrentViewer();
}

// 选中输入的对象
void WAIS_InteractiveContext::SelectAIS(WAIS_Shape^ theAIS) {
	if (myAISContext().IsNull()) return;
	myAISContext()->ClearSelected(Standard_True); // 清除当前选中的对象
	myAISContext()->AddOrRemoveSelected(theAIS->GetOCC(), true);// 选中AIS
	myAISContext()->UpdateCurrentViewer();
}

// 多选
void WAIS_InteractiveContext::MultipleSelect(void) {
	if (myAISContext().IsNull()) return;
	myAISContext()->SelectDetected(AIS_SelectionScheme_Add); //将检测到的对象添加到当前选择
	myAISContext()->UpdateCurrentViewer();
}

// 异或多选
void WAIS_InteractiveContext::XORSelect(void) {
	if (myAISContext().IsNull()) return;
	myAISContext()->SelectDetected(AIS_SelectionScheme_XOR);//对检测到的对象执行异或，再次点击可以取消选择
	myAISContext()->UpdateCurrentViewer();
}

void WAIS_InteractiveContext::InitSelected()
{
	if (myAISContext().IsNull()) return;
	myAISContext()->InitSelected();

}

bool WAIS_InteractiveContext::MoreSelected()
{
	if (myAISContext().IsNull()) return false;
	return myAISContext()->MoreSelected();

}

WAIS_InteractiveObject^ WAIS_InteractiveContext::SelectedInteractive()
{
	if (myAISContext().IsNull()) return nullptr;
	return gcnew WAIS_InteractiveObject(myAISContext()->SelectedInteractive());
}

// 显示
// AIS_InteractiveContext 内部的对象是在无序映射中定义的，因此它们的绘制顺序默认是未定义的（Z 深度缓冲区用于正确渲染），
// 但可以通过显示优先级和分配图层来改变（参见Graphic3d_ZLayerId / PrsMgr_PresentableObject::ZLayer()）。
void WAIS_InteractiveContext::Display(Handle(AIS_InteractiveObject) theAISObject, bool theToUpdateViewer) {
	if (myAISContext().IsNull()) return;
	myAISContext()->Display(theAISObject, theToUpdateViewer);
}

void WAIS_InteractiveContext::Display(WAIS_InteractiveObject^ theAISObject, bool theToUpdateViewer) {
	if (myAISContext().IsNull()) return;
	myAISContext()->Display(theAISObject->GetOCC(), theToUpdateViewer);
}

void WAIS_InteractiveContext::Display(WAIS_Shape^ theAIS, bool theToUpdateViewer) {
	if (myAISContext().IsNull()) return;
	myAISContext()->Display(theAIS->GetOCC(), theToUpdateViewer);
}

// 擦除全部(保留缓存)
void WAIS_InteractiveContext::EraseAll() {
	if (myAISContext().IsNull()) return;
	myAISContext()->EraseAll(true);
}

// 擦除(保留缓存)
void WAIS_InteractiveContext::Erase(WAIS_InteractiveObject^ theAISObject, bool theToUpdateViewer) {
	if (myAISContext().IsNull()) return;
	myAISContext()->Erase(theAISObject->GetOCC(), theToUpdateViewer);
}

void WAIS_InteractiveContext::Erase(Handle(AIS_InteractiveObject) theAISObject, bool theToUpdateViewer) {
	if (myAISContext().IsNull()) return;
	myAISContext()->Erase(theAISObject, theToUpdateViewer);
}

void WAIS_InteractiveContext::Erase(WAIS_Shape^ theAIS, bool theToUpdateViewer) {
	if (myAISContext().IsNull()) return;
	myAISContext()->Erase(theAIS->GetOCC(), theToUpdateViewer);
}

// 移除全部(包括缓存)
void WAIS_InteractiveContext::RemoveAll(bool theToUpdateViewer)
{
	if (myAISContext().IsNull()) return;
	myAISContext()->RemoveAll(theToUpdateViewer);
}

// 移除(包括缓存)
void WAIS_InteractiveContext::Remove(Handle(AIS_InteractiveObject) theAISObject, bool theToUpdateViewer)
{
	if (myAISContext().IsNull()) return;
	myAISContext()->Remove(theAISObject, theToUpdateViewer);
}

void WAIS_InteractiveContext::Remove(WAIS_InteractiveObject^ theAISObject, bool theToUpdateViewer)
{
	if (myAISContext().IsNull()) return;
	myAISContext()->Remove(theAISObject->GetOCC(), theToUpdateViewer);
}

void WAIS_InteractiveContext::Remove(WAIS_Shape^ theAIS, bool theToUpdateViewer)
{
	if (myAISContext().IsNull()) return;
	myAISContext()->Remove(theAIS->GetOCC(), theToUpdateViewer);
}

#pragma region 设置AIS

// 颜色
void WAIS_InteractiveContext::SetColor(WAIS_Shape^ theAIS, Ex::WColor^ theColor, bool theToUpdateViewer)
{
	if (myAISContext().IsNull()) return;
	myAISContext()->SetColor(theAIS->GetOCC(), theColor->GetOCC(), theToUpdateViewer);
}

void WAIS_InteractiveContext::UnsetColor(WAIS_Shape^ theAIS, bool theToUpdateViewer)
{
	if (myAISContext().IsNull()) return;
	myAISContext()->UnsetColor(theAIS->GetOCC(), theToUpdateViewer);
}
// 透明度
void WAIS_InteractiveContext::SetTransparency(WAIS_Shape^ theAIS, double theTransparency, bool theToUpdateViewer)
{
	if (myAISContext().IsNull()) return;
	myAISContext()->SetTransparency(theAIS->GetOCC(), theTransparency, theToUpdateViewer);
}

void WAIS_InteractiveContext::UnsetTransparency(WAIS_Shape^ theAIS, bool theToUpdateViewer)
{
	if (myAISContext().IsNull()) return;
	myAISContext()->UnsetTransparency(theAIS->GetOCC(), theToUpdateViewer);
}

void WAIS_InteractiveContext::SetZLayer(WAIS_Shape^ theAIS, int theZLayerID)
{
	if (myAISContext().IsNull()) return;
	myAISContext()->SetZLayer(theAIS->GetOCC(), theZLayerID);
}


#pragma endregion

bool WAIS_InteractiveContext::IsSelected(void)
{
	if (myAISContext().IsNull()) return false;
	myAISContext()->InitSelected();
	return myAISContext()->MoreSelected();
}

}
}
}