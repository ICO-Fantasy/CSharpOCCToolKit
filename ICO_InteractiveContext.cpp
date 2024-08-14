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

// 单选
void InteractiveContext::SetSelectionMode(TopoAbs::ShapeEnum theMode) {
	if (myAISContext().IsNull()) return;

	myAISContext()->Deactivate();
	myAISContext()->Activate(theMode.GetOCC(), true);
	myAISContext()->UpdateSelected(true);
}

void InteractiveContext::Select(void) {
	if (myAISContext().IsNull()) return;
	myAISContext()->SelectDetected(AIS_SelectionScheme_Replace); // 将检测到的对象替换当前选择
	myAISContext()->UpdateCurrentViewer();
}

// 选中输入的对象
void InteractiveContext::SelectAIS(Shape^ theAIS) {
	if (myAISContext().IsNull()) return;
	myAISContext()->ClearSelected(Standard_True); // 清除当前选中的对象
	myAISContext()->AddOrRemoveSelected(theAIS->GetOCC(), true);// 选中AIS
	myAISContext()->UpdateCurrentViewer();
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

InteractiveObject^ InteractiveContext::SelectedInteractive() {
	if (myAISContext().IsNull()) return nullptr;
	return gcnew InteractiveObject(myAISContext()->SelectedInteractive());
}

Shape^ InteractiveContext::SelectedAIS() {
	if (myAISContext().IsNull()) return nullptr;
	Handle(AIS_Shape) anAISShape;
	if (myAISContext()->SelectedInteractive()->IsKind(STANDARD_TYPE(AIS_Shape))) {
		anAISShape = Handle(AIS_Shape)::DownCast(myAISContext()->SelectedInteractive());
		AIS::Shape^ result = gcnew AIS::Shape(*anAISShape);
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

void InteractiveContext::Display(Shape^ theAIS, bool theToUpdateViewer) {
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

void InteractiveContext::Erase(Shape^ theAIS, bool theToUpdateViewer) {
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

void InteractiveContext::Remove(Shape^ theAIS, bool theToUpdateViewer) {
	if (myAISContext().IsNull()) return;
	myAISContext()->Remove(theAIS->GetOCC(), theToUpdateViewer);
}

#pragma region 设置AIS

// 颜色
void InteractiveContext::SetColor(Shape^ theAIS, Extension::Color^ theColor, bool theToUpdateViewer) {
	if (myAISContext().IsNull()) return;
	myAISContext()->SetColor(theAIS->GetOCC(), theColor->GetOCC(), theToUpdateViewer);
}

void InteractiveContext::UnsetColor(Shape^ theAIS, bool theToUpdateViewer) {
	if (myAISContext().IsNull()) return;
	myAISContext()->UnsetColor(theAIS->GetOCC(), theToUpdateViewer);
}
// 透明度
void InteractiveContext::SetTransparency(Shape^ theAIS, double theTransparency, bool theToUpdateViewer) {
	if (myAISContext().IsNull()) return;
	myAISContext()->SetTransparency(theAIS->GetOCC(), theTransparency, theToUpdateViewer);
}

void InteractiveContext::UnsetTransparency(Shape^ theAIS, bool theToUpdateViewer) {
	if (myAISContext().IsNull()) return;
	myAISContext()->UnsetTransparency(theAIS->GetOCC(), theToUpdateViewer);
}

void InteractiveContext::SetZLayer(Shape^ theAIS, int theZLayerID) {
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

}
}
}