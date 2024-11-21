#include "ICO_RubberBand.h"
#include<Aspect_TypeOfLine.hxx>
//local
#include "..\Extension\ICO_Color.h"

namespace OCCTK {
namespace OCC {
namespace AIS {

RubberBand::RubberBand() : InteractiveObject() {
	NativeHandle = new AIS_RubberBand();
}

void RubberBand::SetRectangle(int minX, int minY, int maxX, int maxY) {
	if (myRubber().IsNull())return;
	myRubber()->SetRectangle(minX, minY, maxX, maxY);
}

void RubberBand::SetLineType(Aspect::Line theType) {
	if (myRubber().IsNull())return;
	myRubber()->SetLineType(Aspect_TypeOfLine((int)theType));
}

void RubberBand::SetFillColor(Color theColor) {
	if (myRubber().IsNull())return;
	myRubber()->SetFillColor(theColor.GetOCC());
}

void RubberBand::SetFillTransparency(double transparency) {
	if (myRubber().IsNull())return;
	myRubber()->SetFillTransparency(transparency);
}

void RubberBand::SetLineColor(Color theColor) {
	if (myRubber().IsNull())return;
	myRubber()->SetLineColor(theColor.GetOCC());
}

void RubberBand::SetFilling(bool isFilling) {
	if (myRubber().IsNull())return;
	myRubber()->SetFilling(isFilling);
}

}
}
}