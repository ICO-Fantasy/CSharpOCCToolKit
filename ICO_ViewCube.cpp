﻿#include "ICO_ViewCube.h"
#include <AIS_ViewCube.hxx>
#include <Prs3d_DatumAspect.hxx>
#include <Standard_Handle.hxx>
//字符编码转换
#include "ConvertChinese.h"

namespace OCCTK {
namespace OCC {
namespace AIS {

ViewCube::ViewCube(float axesRadius) {
	int R, G, B;
	myViewCube() = new AIS_ViewCube();
	nativeHandle() = myViewCube();
	// 设置坐标轴半径
	myViewCube()->SetAxesRadius(axesRadius);
	myViewCube()->SetAxesConeRadius(axesRadius * 1.5);
	myViewCube()->SetAxesSphereRadius(axesRadius * 1.5);
	//修改坐标轴文字颜色
	Handle(Prs3d_Drawer) aDrawer = myViewCube()->Attributes();
	aDrawer->SetDatumAspect(new Prs3d_DatumAspect());  // 动态设置
	Handle(Prs3d_DatumAspect) aDatumAsp = aDrawer->DatumAspect();  // 获取 Prs3d_DatumAspect 对象指针
	aDatumAsp->TextAspect(Prs3d_DP_XAxis)->SetColor(Quantity_NOC_RED2);
	aDatumAsp->TextAspect(Prs3d_DP_YAxis)->SetColor(Quantity_NOC_GREEN2);
	aDatumAsp->TextAspect(Prs3d_DP_ZAxis)->SetColor(Quantity_NOC_BLUE2);
	aDatumAsp->TextAspect(Prs3d_DP_XAxis)->SetHeight(axesRadius * 5);
	aDatumAsp->TextAspect(Prs3d_DP_YAxis)->SetHeight(axesRadius * 5);
	aDatumAsp->TextAspect(Prs3d_DP_ZAxis)->SetHeight(axesRadius * 5);
	// 修改坐标轴颜色
	aDatumAsp->ShadingAspect(Prs3d_DP_XAxis)->SetColor(Quantity_NOC_RED2);
	aDatumAsp->ShadingAspect(Prs3d_DP_YAxis)->SetColor(Quantity_NOC_GREEN2);
	aDatumAsp->ShadingAspect(Prs3d_DP_ZAxis)->SetColor(Quantity_NOC_BLUE2);
	//设置描边和描边颜色
	aDrawer->SetFaceBoundaryDraw(true);
	aDrawer->SetFaceBoundaryAspect(new Prs3d_LineAspect(new Graphic3d_AspectLine3d()));
	R = 228;
	G = 144;
	B = 255;
	aDrawer->FaceBoundaryAspect()->SetColor(Quantity_Color(R / 255, G / 255, B / 255, Quantity_TOC_RGB));
	//设置立方体标签
	myViewCube()->SetBoxSideLabel(V3d_Xneg, Tool::ConvertChineseToUnicode("左"));
	myViewCube()->SetBoxSideLabel(V3d_Xpos, Tool::ConvertChineseToUnicode("右"));
	myViewCube()->SetBoxSideLabel(V3d_Yneg, Tool::ConvertChineseToUnicode("前"));
	myViewCube()->SetBoxSideLabel(V3d_Ypos, Tool::ConvertChineseToUnicode("后"));
	myViewCube()->SetBoxSideLabel(V3d_Zpos, Tool::ConvertChineseToUnicode("上"));
	myViewCube()->SetBoxSideLabel(V3d_Zneg, Tool::ConvertChineseToUnicode("下"));
	myViewCube()->SetFont("MSYHL");
	myViewCube()->SetFontHeight(50);
	R = 241;
	G = 217;
	B = 37;
	myViewCube()->SetTextColor(Quantity_Color(R / 255, G / 255, B / 255, Quantity_TOC_RGB));
	// 设置立方体面颜色
	R = 0;
	G = 194;
	B = 168;
	myViewCube()->SetBoxColor(Quantity_Color(R / 255, G / 255, B / 255, Quantity_TOC_RGB));
	// 设置半透明立方体
	myViewCube()->SetTransparency(0.6);
	// 设置立方体面到边的距离
	myViewCube()->SetBoxFacetExtension(14);
	// 设置轴标签
	// myViewCube()->SetAxesLabels("x", "y", "Z")
	// 设置盒子圆角
	// myViewCube()->SetRoundRadius(0.1)
	// 设置立方体面到边间空隙的可选择范围
	// myViewCube()->SetBoxEdgeGap(0)
}

Handle(AIS_ViewCube) ViewCube::GetOCC() {
	return myViewCube();

}

Handle(Standard_Transient) ViewCube::GetStd() {
	return myViewCube();
}

}
}
}