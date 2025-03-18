#include "ICO_Quaternion.h"
#include <gp_EulerSequence.hxx>
#include <gp_Mat.hxx>
#include <gp_Quaternion.hxx>
#include <gp_Vec.hxx>
//local
#include "ICO_EulerSequence.h"
#include "ICO_Ax1.h"
#include "ICO_Dir.h"
#include "ICO_Vec.h"
#include "../Extension/ICO_WPR.h"

using namespace System;

namespace OCCTK {
namespace OCC {
namespace gp {

Quat::Quat(double theX, double theY, double theZ, double theW) {
    x = theX;
    y = theY;
    z = theZ;
    w = theW;
}

Quat::Quat(double alpha, double beta, double gamma, EulerSequence sequence) {
    gp_Quaternion q = gp_Quaternion();
    q.SetEulerAngles(gp_EulerSequence(sequence), alpha, beta, gamma);
    Init(q);
}

Quat::Quat(SO3Matrix matrix) {
    double a = matrix.Item1.Item1;
    gp_Mat mat(
        matrix.Item1.Item1, matrix.Item1.Item2, matrix.Item1.Item3,
        matrix.Item2.Item1, matrix.Item2.Item2, matrix.Item2.Item3,
        matrix.Item3.Item1, matrix.Item3.Item2, matrix.Item3.Item3
    );
    gp_Quaternion q = gp_Quaternion(mat);
    Init(q);
}

Quat::Quat(Ax1 axis, double angle) {
    gp_Quaternion q = gp_Quaternion();
    q.SetVectorAndAngle(axis.Direction.ToVec(1), angle);
    Init(q);
}

Quat::Quat(Vec vec, double angle) {
    gp_Quaternion q = gp_Quaternion();
    q.SetVectorAndAngle(vec, angle);
    Init(q);
}

Quat::Quat(gp_Quaternion theQuat) {
    Init(theQuat);
}

Quat::Quat(Vec fromVec, Vec toVec) {
    gp_Quaternion q = gp_Quaternion();
    q.SetRotation(fromVec, toVec);
    Init(q);
}

gp_Quaternion Quat::GetOCC() {
    //! default构造的Quaternion是(0,0,0,0)需要处理
    if (X == 0 && Y == 0 && Z == 0 && W == 0)
    {
        w = 1;
    }
    return gp_Quaternion(X, Y, Z, W);
}

System::Object^ Quat::Clone() {
    return Quat(X, Y, Z, W);
}

System::String^ Quat::ToString() {
    return "(" + X.ToString("F3") + "," + Y.ToString("F3") + "," + Z.ToString("F3") + "," + W.ToString("F3") + ")";
}

Quat::operator WPR(Quat q)
{
    double a, b, c;
    a = b = c = 0;
    q.GetOCC().GetEulerAngles(gp_EulerSequence::gp_Extrinsic_XYZ, a, b, c);
    return WPR(a / M_PI * 180, b / M_PI * 180, c / M_PI * 180);
}

System::ValueTuple<double, double, double> Quat::ToEuler(EulerSequence sequence) {
    double a, b, c;
    a = b = c = 0;
    GetOCC().GetEulerAngles(gp_EulerSequence(sequence), a, b, c);
    return System::ValueTuple<double, double, double> {a, b, c};
}


SO3Matrix Quat::GetMatrix() {
    gp_Mat mat = GetOCC().GetMatrix();
    return SO3Matrix(
        System::ValueTuple<double, double, double>(
            mat.Value(1, 1), mat.Value(2, 1), mat.Value(3, 1)),
        System::ValueTuple<double, double, double>(
            mat.Value(1, 2), mat.Value(2, 2), mat.Value(3, 2)),
        System::ValueTuple<double, double, double>(
            mat.Value(1, 3), mat.Value(2, 3), mat.Value(3, 3))
    );
}

void Quat::Init(gp_Quaternion q) {
    x = q.X();
    y = q.Y();
    z = q.Z();
    w = q.W();
}

}
}
}