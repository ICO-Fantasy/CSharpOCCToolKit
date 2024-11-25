#include "ICO_InteractiveContext.h"
#include <AIS_Manipulator.hxx>
#include <AIS_TexturedShape.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_InteractiveContext.hxx>
#include <StdSelect_BRepOwner.hxx>
//Local
#include "ICO_ShapeEnum.h"
#include "ICO_AIS_Shape.h"
#include "ICO_View.h"
#include "ICO_SelectionMode.h"
#include "ICO_InteractiveObject.h"
#include "ICO_Viewer.h"
#include "ICO_DisplayMode.h"
#include "..\Extension\ICO_Color.h"
#include "ICO_Exception.h"

using namespace OCCTK::OCC::V3d;

namespace OCCTK {
namespace OCC {
namespace AIS {

/// <summary>
/// 交互式可视化对象（AIS）上下文管理器，用于管理AIS对象的状态
/// </summary>
/// <remark>
/// 每一个AIS对象均定义了颜色、透明度、材质（AIS_TexturedShape）、显示模式、选择模式，不应该独立设置AIS对象的这些属性，而应该通过Context统一设置和管理
/// </remark>
/// <param name="theViewer"></param>
InteractiveContext::InteractiveContext(V3d::Viewer^ theViewer) :BaseObject() {
	NativeHandle = new AIS_InteractiveContext(theViewer->GetOCC());
}

Handle(AIS_InteractiveContext) InteractiveContext::GetOCC() {
	return myAISContext();
}

/// <summary>
/// 将C++指针包装为 System::IntPtr 传出
/// </summary>
/// <returns></returns>
System::IntPtr InteractiveContext::GetIntPtr() {
	return System::IntPtr(&*myAISContext());
}

/// <summary>
/// 采用默认设置
/// </summary>
/// <remark>
/// 自动高亮，不显示面中间ISO线，自动选择，绘制黑色轮廓边缘，显示实体，自定义选中高亮
/// </remark>
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

/// <summary>
/// 设置默认选中后的高亮模式
/// </summary>
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

V3d::Viewer^ InteractiveContext::Viewer::get() {
	return gcnew V3d::Viewer(myAISContext()->CurrentViewer());
}

/// <summary>
/// 更新画面
/// </summary>
void InteractiveContext::UpdateCurrentViewer() {
	if (myAISContext().IsNull()) return;
	myAISContext()->UpdateCurrentViewer();
}

/// <summary>
/// 移动到
/// </summary>
/// <remark>
/// 用于后续选中、平移、旋转功能的定位
/// </remark>
/// <param name="theX"></param>
/// <param name="theY"></param>
/// <param name="theView"></param>
void InteractiveContext::MoveTo(int theX, int theY, V3d::View^ theView) {
	if (myAISContext().IsNull()) return;
	myAISContext()->MoveTo(theX, theY, theView->GetOCC(), true);
}

/// <summary>
/// 设置默认的显示模式（线框/实体）
/// </summary>
/// <param name="theMode"></param>
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

/// <summary>
/// 设置默认的选择模式
/// </summary>
/// <remark>
/// 点、边线、线、面、壳体、实体
/// </remark>
/// <param name="theMode"></param>
void InteractiveContext::SetDefaultSelectionMode(SelectionMode theMode) {
	if (myAISContext().IsNull()) return;
	myAISContext()->Deactivate();
	if ((int)theMode != -1) {
		myAISContext()->Activate((int)theMode, true);
	}
	myAISContext()->UpdateSelected(true);
}

/// <summary>
/// 设置输入对象的选择模式
/// </summary>
/// <param name="theAISObject"></param>
/// <param name="theMode"></param>
void InteractiveContext::SetSelectionMode(InteractiveObject^ theAISObject, SelectionMode theMode) {
	if (myAISContext().IsNull()) return;
	myAISContext()->Deactivate(theAISObject->GetOCC());
	if ((int)theMode != -1) {
		myAISContext()->Activate(theAISObject->GetOCC(), (int)theMode, true);
	}
	myAISContext()->UpdateSelected(true);
}

/// <summary>
/// 选择
/// </summary>
/// <param name=""></param>
void InteractiveContext::Select(void) {
	if (myAISContext().IsNull()) return;
	myAISContext()->SelectDetected(AIS_SelectionScheme_Replace); // 将检测到的对象替换当前选择
	myAISContext()->UpdateCurrentViewer();
}

/// <summary>
/// 选中输入的对象
/// </summary>
/// <param name="theAISObject"></param>
/// <param name="update"></param>
void InteractiveContext::SelectAIS(InteractiveObject^ theAISObject, bool update) {
	if (myAISContext().IsNull()) return;
	myAISContext()->ClearSelected(Standard_True); // 清除当前选中的对象
	myAISContext()->AddOrRemoveSelected(theAISObject->GetOCC(), update);// 选中AIS
}

/// <summary>
/// 多选
/// </summary>
/// <param name=""></param>
void InteractiveContext::MultipleSelect(void) {
	if (myAISContext().IsNull()) return;
	myAISContext()->SelectDetected(AIS_SelectionScheme_Add); //将检测到的对象添加到当前选择
	myAISContext()->UpdateCurrentViewer();
}

/// <summary>
/// 异或多选
/// </summary>
/// <param name=""></param>
void InteractiveContext::XORSelect(void) {
	if (myAISContext().IsNull()) return;
	myAISContext()->SelectDetected(AIS_SelectionScheme_XOR);//对检测到的对象执行异或，再次点击可以取消选择
	myAISContext()->UpdateCurrentViewer();
}

/// <summary>
/// 区域选择
/// </summary>
/// <param name="minX"></param>
/// <param name="minY"></param>
/// <param name="maxX"></param>
/// <param name="maxY"></param>
/// <param name="theView"></param>
void InteractiveContext::AreaSelect(int minX, int minY, int maxX, int maxY, V3d::View^ theView) {
	if (myAISContext().IsNull()) return;
	if (theView->GetOCC().IsNull()) return;
	myAISContext()->SelectRectangle(Graphic3d_Vec2i(minX, minY),
		Graphic3d_Vec2i(maxX, maxY),
		theView->GetOCC());
}

/// <summary>
/// 区域多选
/// </summary>
/// <param name="minX"></param>
/// <param name="minY"></param>
/// <param name="maxX"></param>
/// <param name="maxY"></param>
/// <param name="theView"></param>
void InteractiveContext::MultipleAreaSelect(int minX, int minY, int maxX, int maxY, V3d::View^ theView) {
	if (myAISContext().IsNull()) return;
	if (theView->GetOCC().IsNull()) return;
	myAISContext()->SelectRectangle(Graphic3d_Vec2i(minX, minY),
		Graphic3d_Vec2i(maxX, maxY),
		theView->GetOCC(),
		AIS_SelectionScheme_Add);
	myAISContext()->UpdateCurrentViewer();
}

/// <summary>
/// 区域异或多选
/// </summary>
/// <param name="minX"></param>
/// <param name="minY"></param>
/// <param name="maxX"></param>
/// <param name="maxY"></param>
/// <param name="theView"></param>
void InteractiveContext::XORAreaSelect(int minX, int minY, int maxX, int maxY, V3d::View^ theView) {
	if (myAISContext().IsNull()) return;
	if (theView->GetOCC().IsNull()) return;
	myAISContext()->SelectRectangle(Graphic3d_Vec2i(minX, minY),
		Graphic3d_Vec2i(maxX, maxY),
		theView->GetOCC(),
		AIS_SelectionScheme_XOR);
	myAISContext()->UpdateCurrentViewer();
}

/// <summary>
/// 初始化选择
/// </summary>
void InteractiveContext::InitSelected() {
	if (myAISContext().IsNull()) return;
	myAISContext()->InitSelected();
}

/// <summary>
/// 是否有更多选中
/// </summary>
/// <returns></returns>
bool InteractiveContext::MoreSelected() {
	if (myAISContext().IsNull()) return false;
	return myAISContext()->MoreSelected();
}

/// <summary>
/// 下一个选中
/// </summary>
/// <returns></returns>
bool InteractiveContext::NextSelected() {
	if (myAISContext().IsNull()) return false;
	myAISContext()->NextSelected();
	return true;
}

/// <summary>
/// 返回选中的对象(InteractiveObject)
/// </summary>
/// <returns></returns>
InteractiveObject^ InteractiveContext::SelectedInteractive() {
	if (myAISContext().IsNull()) return nullptr;
	myAISContext()->InitSelected();
	if (!myAISContext()->SelectedInteractive().IsNull()) {
		return gcnew InteractiveObject(myAISContext()->SelectedInteractive());
	}
	return nullptr;
}

/// <summary>
/// 返回选中的对象（AShape）
/// </summary>
/// <returns></returns>
AShape^ InteractiveContext::SelectedAIS() {
	if (myAISContext().IsNull()) return nullptr;
	myAISContext()->InitSelected();
	if (!myAISContext()->HasSelectedShape()) {
		return nullptr;
	}
	auto owner = myAISContext()->SelectedOwner();
	if (!owner.IsNull()) {
		const Handle(StdSelect_BRepOwner) aBRepOwner = Handle(StdSelect_BRepOwner)::DownCast(owner);
		if (aBRepOwner->HasShape()) {
			return gcnew AIS::AShape(aBRepOwner->Shape());
		}
	}
	return nullptr;
}

/// <summary>
/// 初始化选择检测
/// </summary>
void InteractiveContext::InitDetected() {
	if (myAISContext().IsNull()) return;
	myAISContext()->InitDetected();
}

/// <summary>
/// 更多选中对象
/// </summary>
/// <returns></returns>
bool InteractiveContext::MoreDetected() {
	if (myAISContext().IsNull()) return false;
	return myAISContext()->MoreDetected();
}

/// <summary>
/// 下一个检测到的对象
/// </summary>
void InteractiveContext::NextDetected() {
	if (myAISContext().IsNull()) return;
	myAISContext()->NextDetected();
}

/// <summary>
/// 显示
/// </summary>
/// <remarks> 
/// AIS_InteractiveContext 内部的对象是在无序映射中定义的，因此它们的绘制顺序默认是未定义的（Z 深度缓冲区用于正确渲染），
/// 但可以通过显示优先级和分配图层来改变（参见Graphic3d_ZLayerId / PrsMgr_PresentableObject::ZLayer()）。
/// </remarks>
/// <param name=""></param>
/// <param name="theToUpdateViewer"></param>
void InteractiveContext::Display(Handle(AIS_InteractiveObject) theAISObject, bool theToUpdateViewer) {
	if (myAISContext().IsNull()) return;
	try {
		myAISContext()->Display(theAISObject, theToUpdateViewer);
	}
	CATCH_AND_THROW_OCC_EXCEPTIONS
}

/// <summary>
/// 显示
/// </summary>
/// <param name="theAISObject"></param>
/// <param name="theToUpdateViewer"></param>
void InteractiveContext::Display(InteractiveObject^ theAISObject, bool theToUpdateViewer) {
	if (myAISContext().IsNull()) return;
	try {
		myAISContext()->Display(theAISObject->GetOCC(), theToUpdateViewer);
	}
	CATCH_AND_THROW_OCC_EXCEPTIONS
}

/// <summary>
/// 重绘
/// </summary>
/// <param name="theAISObject"></param>
/// <param name="theToUpdateViewer"></param>
void InteractiveContext::Redisplay(InteractiveObject^ theAISObject, bool theToUpdateViewer) {
	if (myAISContext().IsNull()) return;
	try {
		myAISContext()->Redisplay(theAISObject->GetOCC(), theToUpdateViewer);
	}
	CATCH_AND_THROW_OCC_EXCEPTIONS
}

/// <summary>
/// 擦除全部(保留缓存)
/// </summary>
/// <param name="theToUpdateViewer"></param>
void InteractiveContext::EraseAll(bool theToUpdateViewer) {
	if (myAISContext().IsNull()) return;
	myAISContext()->EraseAll(theToUpdateViewer);
}

/// <summary>
/// 擦除(保留缓存)
/// </summary>
/// <param name="theAISObject"></param>
/// <param name="theToUpdateViewer"></param>
void InteractiveContext::Erase(InteractiveObject^ theAISObject, bool theToUpdateViewer) {
	if (myAISContext().IsNull()) return;
	try {
		myAISContext()->Erase(theAISObject->GetOCC(), theToUpdateViewer);
	}
	CATCH_AND_THROW_OCC_EXCEPTIONS
}

/// <summary>
/// 擦除(保留缓存)
/// </summary>
/// <param name=""></param>
/// <param name="theToUpdateViewer"></param>
void InteractiveContext::Erase(Handle(AIS_InteractiveObject) theAISObject, bool theToUpdateViewer) {
	if (myAISContext().IsNull()) return;
	try {
		myAISContext()->Erase(theAISObject, theToUpdateViewer);
	}
	CATCH_AND_THROW_OCC_EXCEPTIONS
}

/// <summary>
/// 删除选中的物体（首先需要选中物体）
/// </summary>
/// <param name=""></param>
void InteractiveContext::EraseSelected(void) {
	if (myAISContext().IsNull()) return;
	myAISContext()->EraseSelected(Standard_False);
	myAISContext()->ClearSelected(Standard_True);
	myAISContext()->UpdateCurrentViewer();
}

/// <summary>
/// 移除全部(删除缓存)
/// </summary>
/// <param name="theToUpdateViewer"></param>
void InteractiveContext::RemoveAll(bool theToUpdateViewer) {
	if (myAISContext().IsNull()) return;
	myAISContext()->RemoveAll(theToUpdateViewer);
}

/// <summary>
/// 移除(删除缓存)
/// </summary>
/// <param name=""></param>
/// <param name="theToUpdateViewer"></param>
void InteractiveContext::Remove(Handle(AIS_InteractiveObject) theAISObject, bool theToUpdateViewer) {
	if (myAISContext().IsNull()) return;
	myAISContext()->Remove(theAISObject, theToUpdateViewer);
}

/// <summary>
/// 移除(删除缓存)
/// </summary>
/// <param name="theAISObject"></param>
/// <param name="theToUpdateViewer"></param>
void InteractiveContext::Remove(InteractiveObject^ theAISObject, bool theToUpdateViewer) {
	if (myAISContext().IsNull()) return;
	myAISContext()->Remove(theAISObject->GetOCC(), theToUpdateViewer);
}

#pragma region 设置AIS

/// <summary>
/// 设置颜色
/// </summary>
/// <param name="theAIS"></param>
/// <param name="theColor"></param>
/// <param name="theToUpdateViewer"></param>
void InteractiveContext::SetColor(InteractiveObject^ theAISObject, Extension::Color theColor, bool theToUpdateViewer) {
	if (myAISContext().IsNull()) return;
	myAISContext()->SetColor(theAISObject->GetOCC(), theColor.GetOCC(), theToUpdateViewer);
}

/// <summary>
/// 取消颜色设置
/// </summary>
/// <param name="theAIS"></param>
/// <param name="theToUpdateViewer"></param>
void InteractiveContext::UnsetColor(InteractiveObject^ theAISObject, bool theToUpdateViewer) {
	if (myAISContext().IsNull()) return;
	myAISContext()->UnsetColor(theAISObject->GetOCC(), theToUpdateViewer);
}

/// <summary>
/// 设置透明度
/// </summary>
/// <param name="theAIS"></param>
/// <param name="theTransparency"></param>
/// <param name="theToUpdateViewer"></param>
void InteractiveContext::SetTransparency(InteractiveObject^ theAISObject, double theTransparency, bool theToUpdateViewer) {
	if (myAISContext().IsNull()) return;
	myAISContext()->SetTransparency(theAISObject->GetOCC(), theTransparency, theToUpdateViewer);
}

/// <summary>
/// 取消透明度设置
/// </summary>
/// <param name="theAIS"></param>
/// <param name="theToUpdateViewer"></param>
void InteractiveContext::UnsetTransparency(InteractiveObject^ theAISObject, bool theToUpdateViewer) {
	if (myAISContext().IsNull()) return;
	myAISContext()->UnsetTransparency(theAISObject->GetOCC(), theToUpdateViewer);
}

/// <summary>
/// 设置线宽
/// </summary>
/// <param name="theAISObject"></param>
/// <param name="width"></param>
/// <param name="theToUpdateViewer"></param>
void InteractiveContext::SetWidth(InteractiveObject^ theAISObject, double width, bool theToUpdateViewer) {
	if (myAISContext().IsNull()) return;
	myAISContext()->SetWidth(theAISObject->GetOCC(), width, theToUpdateViewer);
}

/// <summary>
/// 取消线宽设置
/// </summary>
/// <param name="theAISObject"></param>
/// <param name="theToUpdateViewer"></param>
void InteractiveContext::UnsetWidth(InteractiveObject^ theAISObject, bool theToUpdateViewer) {
	if (myAISContext().IsNull()) return;
	myAISContext()->UnsetWidth(theAISObject->GetOCC(), theToUpdateViewer);
}

/// <summary>
/// 设置Z层数
/// </summary>
/// <param name="theAIS"></param>
/// <param name="theZLayerID"></param>
void InteractiveContext::SetZLayer(InteractiveObject^ theAIS, int theZLayerID) {
	if (myAISContext().IsNull()) return;
	myAISContext()->SetZLayer(theAIS->GetOCC(), theZLayerID);
}

#pragma endregion

/// <summary>
/// 是否选中
/// </summary>
/// <remark>
/// 等价于调用 InitSelected 后，再调用 MoreSelected 
/// </remark>
/// <param name=""></param>
/// <returns></returns>
bool InteractiveContext::IsSelected(void) {
	if (myAISContext().IsNull()) return false;
	myAISContext()->InitSelected();
	return myAISContext()->MoreSelected();
}

/// <summary>
/// 输入对象是否正在显示
/// </summary>
/// <param name="theAISObject"></param>
/// <returns></returns>
bool InteractiveContext::IsDisplayed(InteractiveObject^ theAISObject) {
	if (myAISContext().IsNull()) return false;
	return myAISContext()->IsDisplayed(theAISObject->GetOCC());
}

}
}
}