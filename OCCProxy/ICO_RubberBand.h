#pragma once
#include<AIS_RubberBand.hxx>
//Local
#include "ICO_InteractiveObject.h"
#include "..\Extension\ICO_Color.h"
#include "ICO_Aspect_Line.h"

using namespace OCCTK::Extension;

namespace OCCTK {
namespace OCC {
namespace AIS {

public ref class RubberBand :public InteractiveObject {
private:
	Handle(AIS_RubberBand) myRubber() { return Handle(AIS_RubberBand)::DownCast(NativeHandle); }
public:
	RubberBand();
	Handle(AIS_RubberBand) GetOCC() { return myRubber(); };

	void SetRectangle(int minX, int minY, int maxX, int maxY);
	void SetLineType(Aspect::Line theType);
	void SetFillColor(Color theColor);
	void SetFillTransparency(double transparency);
	void SetLineColor(Color theColor);
	void SetFilling(bool isFilling);
};

}
}
}