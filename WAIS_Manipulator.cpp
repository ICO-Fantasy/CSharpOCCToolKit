#pragma once
#include "WAIS_Manipulator.h"
#include "WAIS_Shape.h"
#include <AIS_InteractiveObject.hxx>
#include <AIS_Manipulator.hxx>
#include <AIS_Shape.hxx>
#include <BRepPrimAPI_MakeBox.hxx>

namespace OCCTK {
namespace OCC {
namespace AIS
{

WAIS_Manipulator::WAIS_Manipulator()
{
	_pManipulator() = new AIS_Manipulator();
	// 保存原始句柄
	nativeHandle() = _pManipulator();
}

WAIS_Manipulator::WAIS_Manipulator(WAIS_Manipulator^ theManipulator)
{
	_pManipulator() = theManipulator->_pManipulator();
	// 保存原始句柄
	nativeHandle() = _pManipulator();
}

WAIS_Manipulator::WAIS_Manipulator(Handle(AIS_Manipulator)theManipulator)
{
	_pManipulator() = theManipulator;
	// 保存原始句柄
	nativeHandle() = _pManipulator();
}

WAIS_Manipulator::WAIS_Manipulator(List<WAIS_InteractiveObject^>^ theAISList)
{
	AIS_Manipulator* aManipulator = new AIS_Manipulator();
	AIS_ManipulatorObjectSequence* aSeq = new AIS_ManipulatorObjectSequence();
	for each (WAIS_Shape ^ var in theAISList)
	{
		aSeq->Append((var->GetOCC()));
	}
	aManipulator->Attach(aSeq);
	_pManipulator() = aManipulator;
	// 保存原始句柄
	nativeHandle() = _pManipulator();
}

Handle(AIS_Manipulator) WAIS_Manipulator::GetOCC()
{
	return _pManipulator();
}

bool WAIS_Manipulator::HasActiveMode()
{
	return _pManipulator()->HasActiveMode();
}

bool WAIS_Manipulator::IsAttached()
{
	return _pManipulator()->IsAttached();
}

void WAIS_Manipulator::StartTransform(double theX, double theY, WV3d_View^ theView)
{
	_pManipulator()->StartTransform(theX, theY, theView->getOCC());
}

void WAIS_Manipulator::Transform(double theX, double theY, WV3d_View^ theView)
{
	_pManipulator()->Transform(theX, theY, theView->getOCC());
}

void WAIS_Manipulator::StopTransform(bool thetoApply)
{
	_pManipulator()->StopTransform(thetoApply);
}

void WAIS_Manipulator::StopTransform()
{
	_pManipulator()->StopTransform();
}

void WAIS_Manipulator::Attach(WAIS_Shape^ theAIS)
{
	_pManipulator()->Attach(theAIS->GetOCC());
	// 让操作器自动激活
	_pManipulator()->SetModeActivationOnDetection(true);
}

void WAIS_Manipulator::Attach(WAIS_Shape^ theAIS, bool adjustPosition, bool adjustSize, bool enableModes)
{
	AIS_Manipulator::OptionsForAttach anOptions;
	anOptions.SetAdjustPosition(adjustPosition);
	anOptions.SetAdjustSize(adjustSize);
	anOptions.SetEnableModes(enableModes);
	_pManipulator()->Attach(theAIS->GetOCC(), anOptions);
	// 让操作器自动激活
	_pManipulator()->SetModeActivationOnDetection(true);
}

void WAIS_Manipulator::Detach()
{
	_pManipulator()->Detach();
}

void WAIS_Manipulator::DeactivateCurrentMode()
{
	_pManipulator()->DeactivateCurrentMode();
}

// 设置显示哪个部分,默认全部显示
void WAIS_Manipulator::SetPart(WAIS_ManipulatorMode^ theMode, bool isEnable)
{
	_pManipulator()->SetPart(AIS_ManipulatorMode((int)*theMode), isEnable);
}

// 开启操作器模式
void WAIS_Manipulator::EnableMode(WAIS_ManipulatorMode^ theMode)
{
	_pManipulator()->EnableMode(AIS_ManipulatorMode((int)*theMode));
}

Handle(Standard_Transient) WAIS_Manipulator::GetStd()
{
	return nativeHandle();
}

}
}
}