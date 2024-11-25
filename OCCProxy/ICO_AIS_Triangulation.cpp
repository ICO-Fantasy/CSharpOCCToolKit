#include "ICO_AIS_Triangulation.h"
#include <AIS_Triangulation.hxx>
//local
#include "ICO_Poly_Triangulation.h"
#include "..\Extension\ICO_Color.h"

using namespace OCCTK::Extension;

inline static Handle(TColStd_HArray1OfInteger) RGBToIntArray(int r, int g, int b) {
	Handle(TColStd_HArray1OfInteger) colorArray = new TColStd_HArray1OfInteger(1, 1);

	// 确保输入值在 0-255 范围内
	r = (r < 0) ? 0 : (r > 255) ? 255 : r;
	g = (g < 0) ? 0 : (g > 255) ? 255 : g;
	b = (b < 0) ? 0 : (b > 255) ? 255 : b;

	// 将 RGB 转换为整数并存储到数组中
	colorArray->SetValue(1, (b << 16) | (g << 8) | r); // 将RGB值转化为一个32位整数

	return colorArray;
}

namespace OCCTK {
namespace OCC {
namespace AIS {

ATriangulation::ATriangulation(Poly::PTriangulation^ triangulation) {
	NativeHandle = new AIS_Triangulation(triangulation->GetOCC());
}

void ATriangulation::SetColor(Extension::Color^ color) {
	myPT()->SetColors(RGBToIntArray(color->R, color->G, color->B));
}

}
}
}