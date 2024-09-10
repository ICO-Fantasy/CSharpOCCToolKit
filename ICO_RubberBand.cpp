#include "ICO_RubberBand.h"
#include<Aspect_TypeOfLine.hxx>
namespace OCCTK {
namespace OCC {
namespace AIS {

RubberBand::RubberBand() : InteractiveObject(Handle(AIS_RubberBand)()) {
	// 初始化 myRubber
	myRubber() = new AIS_RubberBand();

	// 确保使用正确的 Handle
	InteractiveObject::myAISObject() = myRubber();
	BaseObject::myHandle() = myRubber();
}

Handle(AIS_RubberBand) RubberBand::GetOCC() {
	return myRubber();
}

void RubberBand::SetRectangle(int minX, int minY, int maxX, int maxY) {
	if (myRubber().IsNull())return;
	myRubber()->SetRectangle(minX, minY, maxX, maxY);
}

void RubberBand::SetLineType(Aspect::Line theType) {
	if (myRubber().IsNull())return;
	myRubber()->SetLineType(Aspect_TypeOfLine((int)theType));
}

void RubberBand::SetFillColor(Color^ theColor) {
	if (myRubber().IsNull())return;
	myRubber()->SetFillColor(theColor->GetOCC());
}

void RubberBand::SetFillTransparency(double transparency) {
	if (myRubber().IsNull())return;
	myRubber()->SetFillTransparency(transparency);
}

void RubberBand::SetLineColor(Color^ theColor) {
	if (myRubber().IsNull())return;
	myRubber()->SetLineColor(theColor->GetOCC());
}

void RubberBand::SetFilling(bool isFilling) {
	if (myRubber().IsNull())return;
	myRubber()->SetFilling(isFilling);
}

}
}
}