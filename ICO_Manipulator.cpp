#pragma once
#include "ICO_Manipulator.h"
#include <AIS_InteractiveObject.hxx>
#include <AIS_Manipulator.hxx>
#include <AIS_Shape.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
//Local
#include "ICO_AIS_Shape.h"

namespace OCCTK {
namespace OCC {
namespace AIS {

Manipulator::Manipulator() {
	myManipulator() = new AIS_Manipulator();

	// 保存原始句柄
	myAISObject() = myManipulator();
	nativeHandle() = myManipulator();
}

Manipulator::Manipulator(Manipulator^ theManipulator) {
	myManipulator() = theManipulator->GetOCC();

	// 保存原始句柄
	myAISObject() = myManipulator();
	nativeHandle() = myManipulator();
}

Manipulator::Manipulator(Handle(AIS_Manipulator)theManipulator) {
	myManipulator() = theManipulator;

	// 保存原始句柄
	myAISObject() = myManipulator();
	nativeHandle() = myManipulator();
}

Manipulator::Manipulator(List<InteractiveObject^>^ theAISList) {
	Handle(AIS_Manipulator) aManipulator = new AIS_Manipulator();
	AIS_ManipulatorObjectSequence* aSeq = new AIS_ManipulatorObjectSequence();
	for each (AShape ^ var in theAISList) {
		aSeq->Append((var->GetOCC()));
	}
	aManipulator->Attach(aSeq);

	myManipulator() = aManipulator;
	// 保存原始句柄
	myAISObject() = myManipulator();
	nativeHandle() = myManipulator();
}

Handle(AIS_Manipulator) Manipulator::GetOCC() {
	return myManipulator();
}

Handle(Standard_Transient) Manipulator::GetStd() {
	return nativeHandle();
}

bool Manipulator::HasActiveMode() {
	return myManipulator()->HasActiveMode();
}

ManipulatorMode Manipulator::ActiveMode() {
	auto theMode = myManipulator()->ActiveMode();
	ManipulatorMode output;
	switch (theMode) {
	case AIS_MM_None:
		output = ManipulatorMode::None;
		break;
	case AIS_MM_Translation:
		output = ManipulatorMode::Translation;
		break;
	case AIS_MM_Rotation:
		output = ManipulatorMode::Rotation;
		break;
	case AIS_MM_Scaling:
		output = ManipulatorMode::Scaling;
		break;
	case AIS_MM_TranslationPlane:
		output = ManipulatorMode::TranslationPlane;
		break;
	default:
		output = ManipulatorMode::None;
		break;
	}
	return output;
}

ManipulatorAxisIndex Manipulator::ActiveAxisIndex() {
	return ManipulatorAxisIndex(myManipulator()->ActiveAxisIndex());
}

bool Manipulator::IsAttached() {
	return myManipulator()->IsAttached();
}

Ax2^ Manipulator::Position() {
	return gcnew Ax2(myManipulator()->Position());
}

void Manipulator::StartTransform(double theX, double theY, View^ theView) {
	myManipulator()->StartTransform(theX, theY, theView->GetOCC());
}
//
//void Manipulator::Transform(double theX, double theY, View^ theView) {
//	myManipulator()->Transform(theX, theY, theView->GetOCC());
//}

Trsf^ Manipulator::Transform(double theX, double theY, View^ theView) {
	return gcnew Trsf(myManipulator()->Transform(theX, theY, theView->GetOCC()));
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
	myManipulator()->SetModeActivationOnDetection(true);
}

void Manipulator::Attach(InteractiveObject^ theAISObject) {
	myManipulator()->Attach(theAISObject->GetOCC());
	// 让操作器自动激活
	myManipulator()->SetModeActivationOnDetection(true);
}

void Manipulator::Attach(InteractiveObject^ theAISObject, bool adjustPosition, bool adjustSize, bool enableModes) {
	AIS_Manipulator::OptionsForAttach anOptions;
	anOptions.SetAdjustPosition(adjustPosition);
	anOptions.SetAdjustSize(!adjustSize);
	anOptions.SetEnableModes(enableModes);
	myManipulator()->Attach(theAISObject->GetOCC(), anOptions);
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

//设置显示哪个轴的哪个部分，0=x、1=y、2=z
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