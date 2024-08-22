#include <AIS_Manipulator.hxx>
#include "ICO_InteractiveContext.h"
#include "ICO_ShapeEnum.h"

namespace OCCTK {
namespace OCC {
namespace AIS {

InteractiveContext::InteractiveContext(Handle(AIS_InteractiveContext) theAISContext) {
	myAISContext() = theAISContext;
}

// 删除选中的物体（首先需要选中物体）
void InteractiveContext::EraseObjects(void) {
	if (myAISContext().IsNull()) return;

	myAISContext()->EraseSelected(Standard_False);
	myAISContext()->ClearSelected(Standard_True);
	myAISContext()->UpdateCurrentViewer();
}

void InteractiveContext::SetSelectionMode(TopoAbs::ShapeEnum theMode) {
	if (myAISContext().IsNull()) return;

	myAISContext()->Deactivate();
	myAISContext()->Activate(theMode.GetOCC(), true);
	myAISContext()->UpdateSelected(true);
}

// 单选
void InteractiveContext::Select(void) {
	if (myAISContext().IsNull()) return;
	myAISContext()->SelectDetected(AIS_SelectionScheme_Replace); // 将检测到的对象替换当前选择
	myAISContext()->UpdateCurrentViewer();
}

// 选中输入的对象
void InteractiveContext::SelectAIS(AShape^ theAIS, bool update) {
	if (myAISContext().IsNull()) return;
	myAISContext()->ClearSelected(Standard_True); // 清除当前选中的对象
	myAISContext()->AddOrRemoveSelected(theAIS->GetOCC(), update);// 选中AIS
}

// 多选
void InteractiveContext::MultipleSelect(void) {
	if (myAISContext().IsNull()) return;
	myAISContext()->SelectDetected(AIS_SelectionScheme_Add); //将检测到的对象添加到当前选择
	myAISContext()->UpdateCurrentViewer();
}

// 异或多选
void InteractiveContext::XORSelect(void) {
	if (myAISContext().IsNull()) return;
	myAISContext()->SelectDetected(AIS_SelectionScheme_XOR);//对检测到的对象执行异或，再次点击可以取消选择
	myAISContext()->UpdateCurrentViewer();
}

void InteractiveContext::InitSelected() {
	if (myAISContext().IsNull()) return;
	myAISContext()->InitSelected();

}

bool InteractiveContext::MoreSelected() {
	if (myAISContext().IsNull()) return false;
	return myAISContext()->MoreSelected();
}

bool InteractiveContext::NextSelected() {
	if (myAISContext().IsNull()) return false;
	myAISContext()->NextSelected();
	return true;
}

InteractiveObject^ InteractiveContext::SelectedInteractive() {
	if (myAISContext().IsNull()) return nullptr;
	if (!myAISContext()->SelectedInteractive().IsNull()) {
		return gcnew InteractiveObject(myAISContext()->SelectedInteractive());
	}
	return nullptr;
}

AShape^ InteractiveContext::SelectedAIS() {
	if (myAISContext().IsNull()) return nullptr;
	Handle(AIS_Shape) anAISShape;
	auto selectedObj = myAISContext()->SelectedInteractive();
	if (!selectedObj.IsNull() && selectedObj->IsKind(STANDARD_TYPE(AIS_Shape))) {
		anAISShape = Handle(AIS_Shape)::DownCast(selectedObj);
		AIS::AShape^ result = gcnew AIS::AShape(*anAISShape);
		return result;
	}
	return nullptr;
}

void InteractiveContext::InitDetected() {
	if (myAISContext().IsNull()) return;
	myAISContext()->InitDetected();
}

bool InteractiveContext::MoreDetected() {
	if (myAISContext().IsNull()) return false;
	return myAISContext()->MoreDetected();
}

void InteractiveContext::NextDetected() {
	if (myAISContext().IsNull()) return;
	myAISContext()->NextDetected();
}

// 显示
// AIS_InteractiveContext 内部的对象是在无序映射中定义的，因此它们的绘制顺序默认是未定义的（Z 深度缓冲区用于正确渲染），
// 但可以通过显示优先级和分配图层来改变（参见Graphic3d_ZLayerId / PrsMgr_PresentableObject::ZLayer()）。
void InteractiveContext::Display(Handle(AIS_InteractiveObject) theAISObject, bool theToUpdateViewer) {
	if (myAISContext().IsNull()) return;
	myAISContext()->Display(theAISObject, theToUpdateViewer);
}

void InteractiveContext::Display(InteractiveObject^ theAISObject, bool theToUpdateViewer) {
	if (myAISContext().IsNull()) return;
	myAISContext()->Display(theAISObject->GetOCC(), theToUpdateViewer);
}

void InteractiveContext::Display(AShape^ theAIS, bool theToUpdateViewer) {
	if (myAISContext().IsNull()) return;
	myAISContext()->Display(theAIS->GetOCC(), theToUpdateViewer);
}

// 擦除全部(保留缓存)
void InteractiveContext::EraseAll(bool theToUpdateViewer) {
	if (myAISContext().IsNull()) return;
	myAISContext()->EraseAll(theToUpdateViewer);
}

// 擦除(保留缓存)
void InteractiveContext::Erase(InteractiveObject^ theAISObject, bool theToUpdateViewer) {
	if (myAISContext().IsNull()) return;
	myAISContext()->Erase(theAISObject->GetOCC(), theToUpdateViewer);
}

void InteractiveContext::Erase(Handle(AIS_InteractiveObject) theAISObject, bool theToUpdateViewer) {
	if (myAISContext().IsNull()) return;
	myAISContext()->Erase(theAISObject, theToUpdateViewer);
}

void InteractiveContext::Erase(AShape^ theAIS, bool theToUpdateViewer) {
	if (myAISContext().IsNull()) return;
	myAISContext()->Erase(theAIS->GetOCC(), theToUpdateViewer);
}

// 移除全部(包括缓存)
void InteractiveContext::RemoveAll(bool theToUpdateViewer) {
	if (myAISContext().IsNull()) return;
	myAISContext()->RemoveAll(theToUpdateViewer);
}

// 移除(包括缓存)
void InteractiveContext::Remove(Handle(AIS_InteractiveObject) theAISObject, bool theToUpdateViewer) {
	if (myAISContext().IsNull()) return;
	myAISContext()->Remove(theAISObject, theToUpdateViewer);
}

void InteractiveContext::Remove(InteractiveObject^ theAISObject, bool theToUpdateViewer) {
	if (myAISContext().IsNull()) return;
	myAISContext()->Remove(theAISObject->GetOCC(), theToUpdateViewer);
}

void InteractiveContext::Remove(AShape^ theAIS, bool theToUpdateViewer) {
	if (myAISContext().IsNull()) return;
	myAISContext()->Remove(theAIS->GetOCC(), theToUpdateViewer);
}

#pragma region 设置AIS

// 颜色
void InteractiveContext::SetColor(AShape^ theAIS, Extension::Color^ theColor, bool theToUpdateViewer) {
	if (myAISContext().IsNull()) return;
	myAISContext()->SetColor(theAIS->GetOCC(), theColor->GetOCC(), theToUpdateViewer);
}

void InteractiveContext::UnsetColor(AShape^ theAIS, bool theToUpdateViewer) {
	if (myAISContext().IsNull()) return;
	myAISContext()->UnsetColor(theAIS->GetOCC(), theToUpdateViewer);
}
// 透明度
void InteractiveContext::SetTransparency(AShape^ theAIS, double theTransparency, bool theToUpdateViewer) {
	if (myAISContext().IsNull()) return;
	myAISContext()->SetTransparency(theAIS->GetOCC(), theTransparency, theToUpdateViewer);
}

void InteractiveContext::UnsetTransparency(AShape^ theAIS, bool theToUpdateViewer) {
	if (myAISContext().IsNull()) return;
	myAISContext()->UnsetTransparency(theAIS->GetOCC(), theToUpdateViewer);
}

void InteractiveContext::SetZLayer(AShape^ theAIS, int theZLayerID) {
	if (myAISContext().IsNull()) return;
	myAISContext()->SetZLayer(theAIS->GetOCC(), theZLayerID);
}


#pragma endregion

//是否选中
bool InteractiveContext::IsSelected(void) {
	if (myAISContext().IsNull()) return false;
	myAISContext()->InitSelected();
	return myAISContext()->MoreSelected();
}

Handle(AIS_InteractiveContext) InteractiveContext::GetOCC() {
	return myAISContext();
}

System::IntPtr InteractiveContext::GetIntPtr() {
	return System::IntPtr(&*myAISContext());
}

}
}
}