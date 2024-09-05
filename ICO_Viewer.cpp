#include "ICO_Viewer.h"
#include<V3d_DirectionalLight.hxx>
#include<V3d_AmbientLight.hxx>
namespace OCCTK {
namespace OCC {
namespace V3d {

Viewer::Viewer(Handle(OpenGl_GraphicDriver) theGraphicDriver) {
	myViewer() = new V3d_Viewer(theGraphicDriver);
}

Viewer::Viewer(Handle(V3d_Viewer) theViewer) {
	myViewer() = theViewer;
}

Handle(V3d_Viewer) Viewer::GetOCC() {
	return myViewer();
}

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

void Viewer::ActivateGrid(Aspect::GridType type, Aspect::GridDrawMode drawMode) {
	if (myViewer().IsNull()) return;
	myViewer()->ActivateGrid(Aspect_GridType(type), Aspect_GridDrawMode(drawMode));
}

void Viewer::DeactivateGrid() {
	if (myViewer().IsNull()) return;
	myViewer()->DeactivateGrid();
}

}
}
}