#pragma once
#include "../OCCProxy/ICO_Pnt.h"
#include "../OCCProxy/ICO_Vec.h"
#include "../OCCProxy/ICO_Dir.h"

namespace OCCTK {
namespace Extension {

public ref class CameraOrientation {
public:
	property double Scale;
	property double Aspect;
	property OCC::gp::Pnt^ Eye;
	property OCC::gp::Dir^ ViewDirection;
	property OCC::gp::Pnt^ ViewPoint;
	property OCC::gp::Pnt^ HightPoint;
	property OCC::gp::Vec^ Projection;
	property System::ValueTuple<int, int> ScreenCenter;
	property System::ValueTuple<double, double> Size;
};

}
}