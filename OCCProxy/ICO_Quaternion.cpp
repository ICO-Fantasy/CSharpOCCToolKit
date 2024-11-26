#include "ICO_Quaternion.h"
#include <gp_EulerSequence.hxx>
#include <gp_Quaternion.hxx>
#include <gp_Vec.hxx>
//local
#include "ICO_EulerSequence.h"
#include "ICO_Ax1.h"
#include "ICO_Dir.h"
#include "ICO_Vec.h"

using namespace System;

namespace OCCTK {
namespace OCC {
namespace gp {

Quat::Quat() {
	X = 0.0;
	Y = 0.0;
	Z = 0.0;
	W = 1.0;
}

Quat::Quat(double theX, double theY, double theZ, double theW) {
	X = theX;
	Y = theY;
	Z = theZ;
	W = theW;
}

Quat::Quat(double alpha, double beta, double gamma, EulerSequence sequence) {
	gp_Quaternion q = gp_Quaternion();
	q.SetEulerAngles(gp_EulerSequence(sequence), alpha, beta, gamma);
	X = q.X();
	Y = q.Y();
	Z = q.Z();
	W = q.W();
}

Quat::Quat(Ax1^ axis, double angle) {
	gp_Quaternion q = gp_Quaternion();
	q.SetVectorAndAngle(axis->Direction->ToVec(1), angle);
	X = q.X();
	Y = q.Y();
	Z = q.Z();
	W = q.W();
}

Quat::Quat(Vec vec, double angle) {
	gp_Quaternion q = gp_Quaternion();
	q.SetVectorAndAngle(vec, angle);
	X = q.X();
	Y = q.Y();
	Z = q.Z();
	W = q.W();
}

Quat::Quat(gp_Quaternion theQuat) {
	X = theQuat.X();
	Y = theQuat.Y();
	Z = theQuat.Z();
	W = theQuat.W();
}

Quat::Quat(gp_Quaternion* theQuat) {
	X = theQuat->X();
	Y = theQuat->Y();
	Z = theQuat->Z();
	W = theQuat->W();
}

gp_Quaternion Quat::GetOCC() {
	return gp_Quaternion(X, Y, Z, W);
}

System::Object^ Quat::Clone() {
	return gcnew Quat(X, Y, Z, W);
}

System::String^ Quat::ToString() {
	return "(" + X.ToString("F3") + "," + Y.ToString("F3") + "," + Z.ToString("F3") + "," + W.ToString("F3") + ")";
}

System::Tuple<double, double, double>^ Quat::ToEuler(EulerSequence sequence) {
	double a, b, c;
	GetOCC().GetEulerAngles(gp_EulerSequence(sequence), a, b, c);
	return gcnew System::Tuple<double, double, double> {a, b, c};
}

}
}
}