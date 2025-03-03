#include "ICO_Manipulator.h"
//loacl
#include "ICO_AIS_Shape.h"
#include "ICO_Trsf.h"
#include "ICO_Ax2.h"
#include "ICO_View.h"
#include <V3d_View.hxx>

using namespace OCCTK::OCC::V3d;
using namespace OCCTK::OCC::AIS;
using namespace OCCTK::OCC::gp;

namespace OCCTK {
namespace OCC {
namespace AIS {

/// <summary>
/// 构造空的操作器
/// </summary>
Manipulator::Manipulator() :InteractiveObject() {
    NativeHandle = new AIS_Manipulator();
}

/// <summary>
/// 对列表中的AIS对象创建并应用同一个操作器
/// </summary>
/// <param name="theAISList"></param>
Manipulator::Manipulator(List<InteractiveObject^>^ theAISList) :InteractiveObject() {
    Handle(AIS_Manipulator) aManipulator = new AIS_Manipulator();
    Handle(AIS_ManipulatorObjectSequence) aSeq = new AIS_ManipulatorObjectSequence();
    for each (InteractiveObject ^ var in theAISList) {
        aSeq->Append((var));
    }
    aManipulator->Attach(aSeq);

    NativeHandle = aManipulator;
}

/// <summary>
/// 当前操作器是否激活
/// </summary>
/// <returns></returns>
bool Manipulator::HasActiveMode() {
    return myManipulator()->HasActiveMode();
}

/// <summary>
/// 当前操作器激活的模式
/// </summary>
/// <returns></returns>
ManipulatorMode Manipulator::ActiveMode() {
    return ManipulatorMode(myManipulator()->ActiveMode());
}

/// <summary>
/// 当前操作器激活的轴
/// </summary>
/// <returns></returns>
ManipulatorAxisIndex Manipulator::ActiveAxisIndex() {
    return ManipulatorAxisIndex(myManipulator()->ActiveAxisIndex());
}

/// <summary>
/// 操作器是否已附加
/// </summary>
/// <returns></returns>
bool Manipulator::IsAttached() {
    return myManipulator()->IsAttached();
}

/// <summary>
/// 
/// </summary>
/// <returns>操作器的位置坐标系</returns>
Ax2 Manipulator::Position() {
    return Ax2(myManipulator()->Position());
}

/// <summary>
/// </summary>
/// <returns> 附加的第一个AIS对象</returns>
InteractiveObject^ Manipulator::Object() {
    return gcnew InteractiveObject(myManipulator()->Object());
}

/// <summary>
/// 记录鼠标点击位置
/// </summary>
/// <param name="theX"></param>
/// <param name="theY"></param>
/// <param name="theView"></param>
void Manipulator::StartTransform(double theX, double theY, View^ theView) {
    myManipulator()->StartTransform(theX, theY, theView);
}

/// <summary>
/// 执行变换操作
/// </summary>
/// <param name="theX"></param>
/// <param name="theY"></param>
/// <param name="theView"></param>
/// <returns>从开始点到当前点发生的变换</returns>
Trsf Manipulator::Transform(double theX, double theY, View^ theView) {
    Trsf t = Trsf(myManipulator()->Transform(theX, theY, theView));
    theView->Redraw();
    return t;
}

/// <summary>
/// 停止变换操作
/// </summary>
/// <param name="thetoApply"></param>
void Manipulator::StopTransform(bool thetoApply) {
    myManipulator()->StopTransform(thetoApply);
}

void Manipulator::StopTransform() {
    myManipulator()->StopTransform();
}

/// <summary>
/// 将AIS对象附加到操作器
/// </summary>
/// <param name="theAIS"></param>
void Manipulator::Attach(AShape^ theAIS) {
    myManipulator()->Attach(theAIS);
    // 让操作器自动激活
    myManipulator()->SetModeActivationOnDetection(true);
}

/// <summary>
/// 将AIS对象附加到操作器
/// </summary>
/// <param name="theAIS"></param>
/// <param name="adjustPosition">自动调整操作器原点</param>
/// <param name="adjustSize">自动调整操作器尺寸（相对AIS对象）</param>
/// <param name="enableModes"></param>
void Manipulator::Attach(AShape^ theAIS, bool adjustPosition, bool adjustSize, bool enableModes) {
    AIS_Manipulator::OptionsForAttach anOptions;
    anOptions.SetAdjustPosition(adjustPosition);
    anOptions.SetAdjustSize(!adjustSize);//有bug
    anOptions.SetEnableModes(enableModes);
    myManipulator()->Attach(theAIS->GetOCC(), anOptions);
    // 让操作器自动激活
    myManipulator()->SetModeActivationOnDetection(true);
}

/// <summary>
/// 将AIS对象附加到操作器
/// </summary>
void Manipulator::Attach(InteractiveObject^ theAISObject) {
    myManipulator()->Attach(theAISObject);
    // 让操作器自动激活
    myManipulator()->SetModeActivationOnDetection(true);
}

/// <summary>
/// 将AIS对象附加到操作器
/// </summary>
/// <param name="theAIS"></param>
/// <param name="adjustPosition">自动调整操作器原点</param>
/// <param name="adjustSize">自动调整操作器尺寸（相对AIS对象）</param>
/// <param name="enableModes"></param>
void Manipulator::Attach(InteractiveObject^ theAISObject, bool adjustPosition, bool adjustSize, bool enableModes) {
    AIS_Manipulator::OptionsForAttach anOptions;
    anOptions.SetAdjustPosition(adjustPosition);
    anOptions.SetAdjustSize(!adjustSize);//有bug
    anOptions.SetEnableModes(enableModes);
    myManipulator()->Attach(theAISObject, anOptions);
    // 让操作器自动激活
    myManipulator()->SetModeActivationOnDetection(true);
}

/// <summary>
/// 解除AIS对象绑定
/// </summary>
void Manipulator::Detach() {
    myManipulator()->Detach();
}

/// <summary>
/// 设置检测到轴时自动激活模式
/// </summary>
/// <param name="isEnable"></param>
void Manipulator::SetModeActivationOnDetection(bool isEnable)
{
    myManipulator()->SetModeActivationOnDetection(isEnable);
}

/// <summary>
/// 是否自动激活模式
/// </summary>
/// <returns></returns>
bool Manipulator::IsModeActivationOnDetection()
{
    return myManipulator()->IsModeActivationOnDetection();
}

/// <summary>
/// 关闭当前所有模式
/// </summary>
void Manipulator::DeactivateCurrentMode() {
    myManipulator()->DeactivateCurrentMode();
}

/// <summary>
/// 设置显示哪个部分,默认全部显示
/// </summary>
/// <param name="theMode"></param>
/// <param name="isEnable"></param>
void Manipulator::SetPart(ManipulatorMode theMode, bool isEnable) {
    if (theMode == ManipulatorMode::None) { return; }
    myManipulator()->SetPart(AIS_ManipulatorMode((int)theMode), isEnable);
}

/// <summary>
/// 设置显示哪个轴的哪个部分
/// </summary>
/// <param name="theAxisIndex"></param>
/// <param name="theMode"></param>
/// <param name="isEnable"></param>
void Manipulator::SetPart(ManipulatorAxisIndex theAxisIndex, ManipulatorMode theMode, bool isEnable) {
    if (theAxisIndex == ManipulatorAxisIndex::None) { return; }
    if (theMode == ManipulatorMode::None) { return; }
    myManipulator()->SetPart((int)theAxisIndex, AIS_ManipulatorMode(theMode), isEnable);
}

/// <summary>
/// 开启操作器模式
/// </summary>
/// <param name="theMode"></param>
void Manipulator::EnableMode(ManipulatorMode theMode) {
    myManipulator()->EnableMode(AIS_ManipulatorMode(theMode));
}

}
}
}