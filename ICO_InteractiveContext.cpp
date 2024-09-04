#include <AIS_Manipulator.hxx>
#include <StdSelect_BRepOwner.hxx>
#include "ICO_InteractiveContext.h"
#include "ICO_ShapeEnum.h"

namespace OCCTK {
namespace OCC {
namespace AIS {

InteractiveContext::InteractiveContext(Handle(AIS_InteractiveContext) theAISContext) {
	myAISContext() = theAISContext;
}

InteractiveContext::InteractiveContext(V3d::Viewer^ theViewer) {
	myAISContext() = new AIS_InteractiveContext(theViewer->GetOCC());
}

void InteractiveContext::SetDefault() {
	// 设置自动高亮
	myAISContext()->SetAutomaticHilight(true);
	// 不显示面中间ISO线
	myAISContext()->SetIsoNumber(0);
	// 设置自动选择
	myAISContext()->SetAutoActivateSelection(true);
	// 绘制黑色轮廓边缘
	myAISContext()->DefaultDrawer()->SetFaceBoundaryDraw(true);
	//设置显示实体
	this->SetDisplayMode(DisplayMode::Shaded);
	//设置默认选中后的高亮模式
	this->SetDefaultHighlightStyle();
}

void InteractiveContext::SetDefaultHighlightStyle() {
	//整个对象被选中
	Handle(Prs3d_Drawer) all_selected = myAISContext()->HighlightStyle(Prs3d_TypeOfHighlight_Selected);
	all_selected->SetMethod(Aspect_TOHM_COLOR);
	all_selected->SetColor(Quantity_NOC_LIGHTSEAGREEN);
	all_selected->SetDisplayMode(1);
	all_selected->SetTransparency(0.2f);
	//对象的一部分被选择
	Handle(Prs3d_Drawer) part_selected = myAISContext()->HighlightStyle(Prs3d_TypeOfHighlight_LocalSelected);
	part_selected->SetMethod(Aspect_TOHM_COLOR);
	part_selected->SetColor(Quantity_NOC_LIGHTSEAGREEN);
	part_selected->SetDisplayMode(1);
	part_selected->SetTransparency(0.2f);
}

void InteractiveContext::UpdateCurrentViewer() {
	if (myAISContext().IsNull()) return;
	myAISContext()->UpdateCurrentViewer();
}

void InteractiveContext::MoveTo(int theX, int theY, V3d::View^ theView) {
	if (myAISContext().IsNull()) return;
	myAISContext()->MoveTo(theX, theY, theView->GetOCC(), true);
}

//设置显示模式
void InteractiveContext::SetDisplayMode(DisplayMode theMode) {
	if (myAISContext().IsNull()) return;
	AIS_DisplayMode aCurrentMode;
	if (myAISContext()->NbSelected() == 0) {
		myAISContext()->SetDisplayMode((int)theMode, false);
	}
	else {
		for (myAISContext()->InitSelected(); myAISContext()->MoreSelected(); myAISContext()->NextSelected()) {
			myAISContext()->SetDisplayMode(myAISContext()->SelectedInteractive(), (int)theMode, false);
		}
	}
}

//设置选择模式
void InteractiveContext::SetSelectionMode(SelectionMode theMode) {
	if (myAISContext().IsNull()) return;
	myAISContext()->Deactivate();
	myAISContext()->Activate((int)theMode, true);
	myAISContext()->UpdateSelected(true);
}

// 单选
void InteractiveContext::Select(void) {
	if (myAISContext().IsNull()) return;
	myAISContext()->SelectDetected(AIS_SelectionScheme_Replace); // 将检测到的对象替换当前选择
	myAISContext()->UpdateCurrentViewer();
}

// 选中输入的对象
void InteractiveContext::SelectAIS(InteractiveObject^ theAISObject, bool update) {
	if (myAISContext().IsNull()) return;
	myAISContext()->ClearSelected(Standard_True); // 清除当前选中的对象
	myAISContext()->AddOrRemoveSelected(theAISObject->GetOCC(), update);// 选中AIS
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

void InteractiveContext::AreaSelect(int minX, int minY, int maxX, int maxY, V3d::View^ theView) {
	if (myAISContext().IsNull()) return;
	if (theView->GetOCC().IsNull()) return;
	myAISContext()->SelectRectangle(Graphic3d_Vec2i(minX, minY),
		Graphic3d_Vec2i(maxX, maxY),
		theView->GetOCC());
}

void InteractiveContext::MultipleAreaSelect(int minX, int minY, int maxX, int maxY, V3d::View^ theView) {
	if (myAISContext().IsNull()) return;
	if (theView->GetOCC().IsNull()) return;
	myAISContext()->SelectRectangle(Graphic3d_Vec2i(minX, minY),
		Graphic3d_Vec2i(maxX, maxY),
		theView->GetOCC(),
		AIS_SelectionScheme_Add);
	myAISContext()->UpdateCurrentViewer();
}

void InteractiveContext::XORAreaSelect(int minX, int minY, int maxX, int maxY, V3d::View^ theView) {
	if (myAISContext().IsNull()) return;
	if (theView->GetOCC().IsNull()) return;
	myAISContext()->SelectRectangle(Graphic3d_Vec2i(minX, minY),
		Graphic3d_Vec2i(maxX, maxY),
		theView->GetOCC(),
		AIS_SelectionScheme_XOR);
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
	myAISContext()->InitSelected();
	if (!myAISContext()->SelectedInteractive().IsNull()) {
		return gcnew InteractiveObject(myAISContext()->SelectedInteractive());
	}
	return nullptr;
}

AShape^ InteractiveContext::SelectedAIS() {
	if (myAISContext().IsNull()) return nullptr;
	myAISContext()->InitSelected();
	auto owner = myAISContext()->SelectedOwner();
	if (!owner.IsNull()) {
		const Handle(StdSelect_BRepOwner) aBRepOwner = Handle(StdSelect_BRepOwner)::DownCast(owner);
		if (aBRepOwner->HasShape()) {
			auto anShape = aBRepOwner->Shape();
			Handle(AIS_Shape) refAIS = new AIS_Shape(anShape);
			AIS::AShape^ result = gcnew AIS::AShape(refAIS);
			return result;
		}
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

//! 显示
// AIS_InteractiveContext 内部的对象是在无序映射中定义的，因此它们的绘制顺序默认是未定义的（Z 深度缓冲区用于正确渲染），
// 但可以通过显示优先级和分配图层来改变（参见Graphic3d_ZLayerId / PrsMgr_PresentableObject::ZLayer()）。
void InteractiveContext::Display(Handle(AIS_InteractiveObject) theAISObject, bool theToUpdateViewer) {
	if (myAISContext().IsNull()) return;
	try {
		myAISContext()->Display(theAISObject, theToUpdateViewer);
	}
	catch (Standard_Failure e) {
		throw gcnew System::Exception(gcnew System::String(e.GetMessageString()));
	}
}

void InteractiveContext::Display(InteractiveObject^ theAISObject, bool theToUpdateViewer) {
	if (myAISContext().IsNull()) return;
	try {
		myAISContext()->Display(theAISObject->GetOCC(), theToUpdateViewer);
	}
	catch (Standard_Failure e) {
		throw gcnew System::Exception(gcnew System::String(e.GetMessageString()));
	}
}

void InteractiveContext::Redisplay(InteractiveObject^ theAISObject, bool theToUpdateViewer) {
	if (myAISContext().IsNull()) return;
	myAISContext()->Redisplay(theAISObject->GetOCC(), theToUpdateViewer);
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

// 删除选中的物体（首先需要选中物体）
void InteractiveContext::EraseSelected(void) {
	if (myAISContext().IsNull()) return;

	myAISContext()->EraseSelected(Standard_False);
	myAISContext()->ClearSelected(Standard_True);
	myAISContext()->UpdateCurrentViewer();
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

#pragma region 设置AIS

// 颜色
void InteractiveContext::SetColor(InteractiveObject^ theAIS, Extension::Color^ theColor, bool theToUpdateViewer) {
	if (myAISContext().IsNull()) return;
	myAISContext()->SetColor(theAIS->GetOCC(), theColor->GetOCC(), theToUpdateViewer);
}

void InteractiveContext::UnsetColor(InteractiveObject^ theAIS, bool theToUpdateViewer) {
	if (myAISContext().IsNull()) return;
	myAISContext()->UnsetColor(theAIS->GetOCC(), theToUpdateViewer);
}
// 透明度
void InteractiveContext::SetTransparency(InteractiveObject^ theAIS, double theTransparency, bool theToUpdateViewer) {
	if (myAISContext().IsNull()) return;
	myAISContext()->SetTransparency(theAIS->GetOCC(), theTransparency, theToUpdateViewer);
}

void InteractiveContext::UnsetTransparency(InteractiveObject^ theAIS, bool theToUpdateViewer) {
	if (myAISContext().IsNull()) return;
	myAISContext()->UnsetTransparency(theAIS->GetOCC(), theToUpdateViewer);
}

void InteractiveContext::SetZLayer(InteractiveObject^ theAIS, int theZLayerID) {
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

//是否正在显示
bool InteractiveContext::IsDisplayed(InteractiveObject^ theAISObject) {
	if (myAISContext().IsNull()) return false;
	return myAISContext()->IsDisplayed(theAISObject->GetOCC());
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