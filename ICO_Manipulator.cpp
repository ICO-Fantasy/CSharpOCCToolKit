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
	_pManipulator() = new AIS_Manipulator();
	// 保存原始句柄
	nativeHandle() = _pManipulator();
}

Manipulator::Manipulator(Manipulator^ theManipulator) {
	_pManipulator() = theManipulator->_pManipulator();
	// 保存原始句柄
	nativeHandle() = _pManipulator();
}

Manipulator::Manipulator(Handle(AIS_Manipulator)theManipulator) {
	_pManipulator() = theManipulator;
	// 保存原始句柄
	nativeHandle() = _pManipulator();
}

Manipulator::Manipulator(List<InteractiveObject^>^ theAISList) {
	AIS_Manipulator* aManipulator = new AIS_Manipulator();
	AIS_ManipulatorObjectSequence* aSeq = new AIS_ManipulatorObjectSequence();
	for each (Shape ^ var in theAISList) {
		aSeq->Append((var->GetOCC()));
	}
	aManipulator->Attach(aSeq);
	_pManipulator() = aManipulator;
	// 保存原始句柄
	nativeHandle() = _pManipulator();
}

Handle(AIS_Manipulator) Manipulator::GetOCC() {
	return _pManipulator();
}

bool Manipulator::HasActiveMode() {
	return _pManipulator()->HasActiveMode();
}

ManipulatorMode Manipulator::ActiveMode() {
	auto theMode = _pManipulator()->ActiveMode();
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
	return ManipulatorAxisIndex(_pManipulator()->ActiveAxisIndex());
}

bool Manipulator::IsAttached() {
	return _pManipulator()->IsAttached();
}

Ax2^ Manipulator::Position() {
	return gcnew Ax2(_pManipulator()->Position());
}

void Manipulator::StartTransform(double theX, double theY, View^ theView) {
	_pManipulator()->StartTransform(theX, theY, theView->getOCC());
}

void Manipulator::Transform(double theX, double theY, View^ theView) {
	_pManipulator()->Transform(theX, theY, theView->getOCC());
}

void Manipulator::StopTransform(bool thetoApply) {
	_pManipulator()->StopTransform(thetoApply);
}

void Manipulator::StopTransform() {
	_pManipulator()->StopTransform();
}

void Manipulator::Attach(Shape^ theAIS) {
	_pManipulator()->Attach(theAIS->GetOCC());
	// 让操作器自动激活
	_pManipulator()->SetModeActivationOnDetection(true);
}

void Manipulator::Attach(Shape^ theAIS, bool adjustPosition, bool adjustSize, bool enableModes) {
	AIS_Manipulator::OptionsForAttach anOptions;
	anOptions.SetAdjustPosition(adjustPosition);
	anOptions.SetAdjustSize(!adjustSize);
	anOptions.SetEnableModes(enableModes);
	_pManipulator()->Attach(theAIS->GetOCC(), anOptions);
	// 让操作器自动激活
	_pManipulator()->SetModeActivationOnDetection(true);
}

void Manipulator::Detach() {
	_pManipulator()->Detach();
}

void Manipulator::DeactivateCurrentMode() {
	_pManipulator()->DeactivateCurrentMode();
}

// 设置显示哪个部分,默认全部显示
void Manipulator::SetPart(ManipulatorMode^ theMode, bool isEnable) {
	_pManipulator()->SetPart(AIS_ManipulatorMode((int)*theMode), isEnable);
}
//设置显示哪个轴的哪个部分，0=x、1=y、2=z
void Manipulator::SetPart(int theAxisIndex, ManipulatorMode^ theMode, bool isEnable) {
	_pManipulator()->SetPart(theAxisIndex, AIS_ManipulatorMode((int)*theMode), isEnable);
}

// 开启操作器模式
void Manipulator::EnableMode(ManipulatorMode^ theMode) {
	_pManipulator()->EnableMode(AIS_ManipulatorMode((int)*theMode));
}

Handle(Standard_Transient) Manipulator::GetStd() {
	return nativeHandle();
}

}
}
}