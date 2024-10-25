#include "ICO_Viewer.h"
#include<V3d_DirectionalLight.hxx>
#include<V3d_AmbientLight.hxx>

namespace OCCTK {
namespace OCC {
namespace V3d {

/// <summary>
/// 从窗口句柄创建画面并绑定
/// </summary>
/// <param name=""></param>
Viewer::Viewer(Handle(OpenGl_GraphicDriver) theGraphicDriver) :BaseObject() {
	NativeHandle = new V3d_Viewer(theGraphicDriver);
}

/// <summary>
/// 设置默认光源
/// </summary>
void Viewer::SetDefaultLight() {
	if (myViewer().IsNull()) return;
	Handle(V3d_DirectionalLight) aDirLight = new V3d_DirectionalLight(V3d_XposYnegZpos, Quantity_NOC_WHITE);
	aDirLight->SetName("HeadLight");
	aDirLight->SetHeadlight(true);
	Handle(V3d_AmbientLight) anAmbLight = new V3d_AmbientLight(Quantity_NOC_WHITE);
	anAmbLight->SetName("AmbLight");
	//打开光源
	myViewer()->SetLightOn(aDirLight);
	myViewer()->SetLightOn(anAmbLight);
}

/// <summary>
/// 设置自定义光源
/// </summary>
void Viewer::SetICOLight() {
	if (myViewer().IsNull()) return;
	Handle(V3d_AmbientLight) anAmbLight = new V3d_AmbientLight(Quantity_NOC_WHITE);
	anAmbLight->SetName("AmbLight");
	anAmbLight->SetIntensity(1.0);
	//打开光源
	myViewer()->SetLightOn(anAmbLight);
}

/// <summary>
/// 创建画面
/// </summary>
/// <returns></returns>
V3d::View^ Viewer::CreateView() {
	return gcnew V3d::View(myViewer()->CreateView());
}

/// <summary>
/// 启用Grid
/// </summary>
/// <param name="type"></param>
/// <param name="drawMode"></param>
void Viewer::ActivateGrid(Aspect::GridType type, Aspect::GridDrawMode drawMode) {
	if (myViewer().IsNull()) return;
	myViewer()->ActivateGrid(Aspect_GridType(type), Aspect_GridDrawMode(drawMode));
}

/// <summary>
/// 禁用Grid
/// </summary>
void Viewer::DeactivateGrid() {
	if (myViewer().IsNull()) return;
	myViewer()->DeactivateGrid();
}

}
}
}