#include "ViewCube.h"
#include <Standard_Handle.hxx>
#include <AIS_ViewCube.hxx>
#include <Prs3d_DatumAspect.hxx>
#include <Resource_Unicode.hxx>
#include "convertChinese.h"
namespace OCCTK::Visualization {
ViewCube::ViewCube(float axisSize)
{
	int R, G, B;
	_ptransient = new Handle_Standard_Transient(new AIS_ViewCube());//创建基类对象智能指针
	myViewCube = (Handle(AIS_ViewCube)*)_ptransient;//强制转换基类对象，并创建指向Handle(AIS_ViewCube)的指针
	Handle(AIS_ViewCube) aViewCube = (*myViewCube);//指针解引用
	// 设置坐标轴半径
	aViewCube->SetAxesRadius(axisSize);
	aViewCube->SetAxesConeRadius(axisSize * 1.5);
	aViewCube->SetAxesSphereRadius(axisSize * 1.5);
	//修改坐标轴文字颜色
	Handle(Prs3d_Drawer) aDrawer = aViewCube->Attributes();
	aDrawer->SetDatumAspect(new Prs3d_DatumAspect());  // 动态设置
	Handle(Prs3d_DatumAspect) aDatumAsp = aDrawer->DatumAspect();  // 获取 Prs3d_DatumAspect 对象指针
	aDatumAsp->TextAspect(Prs3d_DP_XAxis)->SetColor(Quantity_NOC_RED2);
	aDatumAsp->TextAspect(Prs3d_DP_YAxis)->SetColor(Quantity_NOC_GREEN2);
	aDatumAsp->TextAspect(Prs3d_DP_ZAxis)->SetColor(Quantity_NOC_BLUE2);
	aDatumAsp->TextAspect(Prs3d_DP_XAxis)->SetHeight(axisSize * 5);
	aDatumAsp->TextAspect(Prs3d_DP_YAxis)->SetHeight(axisSize * 5);
	aDatumAsp->TextAspect(Prs3d_DP_ZAxis)->SetHeight(axisSize * 5);
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
	aViewCube->SetBoxSideLabel(V3d_Xneg, ConvertChineseToUnicode("左"));
	aViewCube->SetBoxSideLabel(V3d_Xpos, ConvertChineseToUnicode("右"));
	aViewCube->SetBoxSideLabel(V3d_Yneg, ConvertChineseToUnicode("前"));
	aViewCube->SetBoxSideLabel(V3d_Ypos, ConvertChineseToUnicode("后"));
	aViewCube->SetBoxSideLabel(V3d_Zpos, ConvertChineseToUnicode("上"));
	aViewCube->SetBoxSideLabel(V3d_Zneg, ConvertChineseToUnicode("下"));
	aViewCube->SetFont("MSYHL");
	aViewCube->SetFontHeight(50);
	R = 241;
	G = 217;
	B = 37;
	aViewCube->SetTextColor(Quantity_Color(R / 255, G / 255, B / 255, Quantity_TOC_RGB));
	// 设置立方体面颜色
	R = 0;
	G = 194;
	B = 168;
	aViewCube->SetBoxColor(Quantity_Color(R / 255, G / 255, B / 255, Quantity_TOC_RGB));
	// 设置半透明立方体
	aViewCube->SetTransparency(0.6);
	// 设置立方体面到边的距离
	aViewCube->SetBoxFacetExtension(14);
	// 设置轴标签
	// aViewCube->SetAxesLabels("x", "y", "Z")
	// 设置盒子圆角
	// aViewCube->SetRoundRadius(0.1)
	// 设置立方体面到边间空隙的可选择范围
	// aViewCube->SetBoxEdgeGap(0)
}
Handle(AIS_ViewCube)* ViewCube::GetOCC()
{
	return myViewCube;
}
}
