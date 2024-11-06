#include "ICO_ViewCube.h"
#include <AIS_ViewCube.hxx>
#include <Prs3d_DatumAspect.hxx>
#include <Standard_Handle.hxx>
//local
#include "ICO_StringExchange.h"

namespace OCCTK {
namespace OCC {
namespace AIS {

ViewCube::ViewCube() :InteractiveObject() {
	NativeHandle = new AIS_ViewCube();
}

/// <summary>
/// 创建视图立方
/// </summary>
/// <param name="axesRadius">轴半径</param>
ViewCube::ViewCube(double axesRadius) :ViewCube() {
	myAxisSize = axesRadius;
	SetDefault();
}

void ViewCube::SetDefault() {
	int R, G, B;
	myViewCube()->SetSize(myAxisSize * 30.0);
	// 设置坐标轴半径
	myViewCube()->SetAxesRadius(myAxisSize);
	myViewCube()->SetAxesConeRadius(myAxisSize * 1.5);
	myViewCube()->SetAxesSphereRadius(myAxisSize * 1.5);
	//修改坐标轴文字颜色
	Handle(Prs3d_Drawer) aDrawer = myViewCube()->Attributes();
	aDrawer->SetDatumAspect(new Prs3d_DatumAspect());  // 动态设置
	Handle(Prs3d_DatumAspect) aDatumAsp = aDrawer->DatumAspect();  // 获取 Prs3d_DatumAspect 对象指针
	aDatumAsp->TextAspect(Prs3d_DP_XAxis)->SetColor(Quantity_NOC_RED2);
	aDatumAsp->TextAspect(Prs3d_DP_YAxis)->SetColor(Quantity_NOC_GREEN2);
	aDatumAsp->TextAspect(Prs3d_DP_ZAxis)->SetColor(Quantity_NOC_BLUE2);
	aDatumAsp->TextAspect(Prs3d_DP_XAxis)->SetHeight(myAxisSize * 5);
	aDatumAsp->TextAspect(Prs3d_DP_YAxis)->SetHeight(myAxisSize * 5);
	aDatumAsp->TextAspect(Prs3d_DP_ZAxis)->SetHeight(myAxisSize * 5);
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
	myViewCube()->SetBoxSideLabel(V3d_Xneg, DataExchange::ConvertChineseToUnicode("左"));
	myViewCube()->SetBoxSideLabel(V3d_Xpos, DataExchange::ConvertChineseToUnicode("右"));
	myViewCube()->SetBoxSideLabel(V3d_Yneg, DataExchange::ConvertChineseToUnicode("前"));
	myViewCube()->SetBoxSideLabel(V3d_Ypos, DataExchange::ConvertChineseToUnicode("后"));
	myViewCube()->SetBoxSideLabel(V3d_Zpos, DataExchange::ConvertChineseToUnicode("上"));
	myViewCube()->SetBoxSideLabel(V3d_Zneg, DataExchange::ConvertChineseToUnicode("下"));
	myViewCube()->SetFont("MSYHL");
	myViewCube()->SetFontHeight(myAxisSize * 15.0);
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

}
}
}