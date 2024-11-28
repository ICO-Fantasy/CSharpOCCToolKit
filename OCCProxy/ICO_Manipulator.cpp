#include "ICO_Manipulator.h"
//loacl
#include "ICO_AIS_Shape.h"
#include "ICO_Trsf.h"
#include "ICO_Ax2.h"
#include "ICO_View.h"

using namespace OCCTK::OCC::V3d;
using namespace OCCTK::OCC::AIS;
using namespace OCCTK::OCC::gp;

namespace OCCTK {
namespace OCC {
namespace AIS {

Manipulator::Manipulator() :InteractiveObject() {
	NativeHandle = new AIS_Manipulator();
}

Manipulator::Manipulator(List<InteractiveObject^>^ theAISList) :InteractiveObject() {
	Handle(AIS_Manipulator) aManipulator = new AIS_Manipulator();
	AIS_ManipulatorObjectSequence* aSeq = new AIS_ManipulatorObjectSequence();
	for each (InteractiveObject ^ var in theAISList) {
		aSeq->Append((var->GetOCC()));
	}
	aManipulator->Attach(aSeq);

	NativeHandle = aManipulator;
}

bool Manipulator::HasActiveMode() {
	return myManipulator()->HasActiveMode();
}

ManipulatorMode Manipulator::ActiveMode() {
	return ManipulatorMode(myManipulator()->ActiveMode());
}

ManipulatorAxisIndex Manipulator::ActiveAxisIndex() {
	return ManipulatorAxisIndex(myManipulator()->ActiveAxisIndex());
}

bool Manipulator::IsAttached() {
	return myManipulator()->IsAttached();
}

Ax2 Manipulator::Position() {
	return Ax2(myManipulator()->Position());
}

InteractiveObject^ Manipulator::Object() {
	return gcnew InteractiveObject(myManipulator()->Object());
	//catch (const Standard_Failure e) {
	//	throw gcnew System::Exception(gcnew System::String(e.GetMessageString()));
	//}
}

void Manipulator::StartTransform(double theX, double theY, View^ theView) {
	myManipulator()->StartTransform(theX, theY, theView->GetOCC());
}

//void Manipulator::Transform(double theX, double theY, View^ theView) {
//	myManipulator()->Transform(theX, theY, theView->GetOCC());
//}

Trsf^ Manipulator::Transform(double theX, double theY, View^ theView) {
	Trsf^ T = gcnew Trsf(myManipulator()->Transform(theX, theY, theView->GetOCC()));
	theView->Redraw();
	return T;
}

void Manipulator::StopTransform(bool thetoApply) {
	myManipulator()->StopTransform(thetoApply);
}

void Manipulator::StopTransform() {
	myManipulator()->StopTransform();
}

void Manipulator::Attach(AShape^ theAIS) {
	myManipulator()->Attach(theAIS->GetOCC());
	// 让操作器自动激活
	myManipulator()->SetModeActivationOnDetection(true);
}

void Manipulator::Attach(AShape^ theAIS, bool adjustPosition, bool adjustSize, bool enableModes) {
	AIS_Manipulator::OptionsForAttach anOptions;
	anOptions.SetAdjustPosition(adjustPosition);
	anOptions.SetAdjustSize(!adjustSize);
	anOptions.SetEnableModes(enableModes);
	myManipulator()->Attach(theAIS->GetOCC(), anOptions);
	// 让操作器自动激活
	myManipulator()->SetModeActivationOnDetection(false);
}

void Manipulator::Attach(InteractiveObject^ theAISObject) {
	myManipulator()->Attach(theAISObject);
	// 让操作器自动激活
	myManipulator()->SetModeActivationOnDetection(true);
}

void Manipulator::Attach(InteractiveObject^ theAISObject, bool adjustPosition, bool adjustSize, bool enableModes) {
	AIS_Manipulator::OptionsForAttach anOptions;
	anOptions.SetAdjustPosition(adjustPosition);
	anOptions.SetAdjustSize(!adjustSize);
	anOptions.SetEnableModes(enableModes);
	myManipulator()->Attach(theAISObject, anOptions);
	// 让操作器自动激活
	myManipulator()->SetModeActivationOnDetection(true);
}

void Manipulator::Detach() {
	myManipulator()->Detach();
}

void Manipulator::DeactivateCurrentMode() {
	myManipulator()->DeactivateCurrentMode();
}

// 设置显示哪个部分,默认全部显示
void Manipulator::SetPart(ManipulatorMode^ theMode, bool isEnable) {
	if (*theMode == ManipulatorMode::None) { return; }
	myManipulator()->SetPart(AIS_ManipulatorMode((int)*theMode), isEnable);
}

//设置显示哪个轴的哪个部分
void Manipulator::SetPart(ManipulatorAxisIndex^ theAxisIndex, ManipulatorMode^ theMode, bool isEnable) {
	if (*theAxisIndex == ManipulatorAxisIndex::None) { return; }
	if (*theMode == ManipulatorMode::None) { return; }
	myManipulator()->SetPart((int)*theAxisIndex, AIS_ManipulatorMode((int)*theMode), isEnable);
}

// 开启操作器模式
void Manipulator::EnableMode(ManipulatorMode^ theMode) {
	myManipulator()->EnableMode(AIS_ManipulatorMode((int)*theMode));
}

}
}
}