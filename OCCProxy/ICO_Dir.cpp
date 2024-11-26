﻿#include <gp_Dir.hxx>
#include <cmath>
//Local
#include "ICO_Dir.h"
#include "ICO_Pnt.h"
#include "ICO_Vec.h"
#include "ICO_Trsf.h"

using namespace System;

namespace OCCTK {
namespace OCC {
namespace gp {

Dir::Dir(double theX, double theY, double theZ) {
	if (std::abs(theX) < 1e-6 && std::abs(theY) < 1e-6 && std::abs(theZ) < 1e-6) {
		throw gcnew System::ArgumentException("不能创建零向量");
	}
	X = theX;
	Y = theY;
	Z = theZ;
	Normalize();
}

Dir::Dir(Vec theDir) {
	X = theDir.X;
	Y = theDir.Y;
	Z = theDir.Z;
	Normalize();
}

Dir::Dir(Pnt fromPoint, Pnt toPoint) {
	if (fromPoint.Distance(toPoint) < 1e-6) {
		throw gcnew System::ArgumentException("不能创建零向量");
	}
	Pnt p = toPoint - fromPoint;
	X = p.X;
	Y = p.Y;
	Z = p.Z;
	Normalize();
}

Dir::Dir(gp_Dir theDir) {
	X = theDir.X();
	Y = theDir.Y();
	Z = theDir.Z();
	Normalize();
}

Dir::Dir(gp_Dir* theDir) {
	X = theDir->X();
	Y = theDir->Y();
	Z = theDir->Z();
	Normalize();
}

bool Dir::IsParallel(Dir otherDir, double theAngularTolerance) {
	return GetOCC().IsParallel(otherDir, theAngularTolerance);
}

/// <summary>
/// 获取 [0,π] 范围内的角度
/// </summary>
/// <param name="otherDir"></param>
/// <returns></returns>
double Dir::Angle(Dir otherDir) {
	return GetOCC().Angle(otherDir);
}

/// <summary>
/// 获取 [-π,π] 范围内的角度，正负取决于参考方向
/// </summary>
/// <param name="otherDir"></param>
/// <param name="refDir"></param>
/// <returns></returns>
double Dir::AngleWithRef(Dir otherDir, Dir refDir) {
	return GetOCC().AngleWithRef(otherDir, refDir);
}

void Dir::Cross(Dir other) {
	X = (this->Y * other.Z) - (this->Z * other.Y);
	Y = (this->Z * other.X) - (this->X * other.Z);
	Z = (this->X * other.Y) - (this->Y * other.X);
	Normalize();
}

Dir Dir::Crossed(Dir other) {
	double crossX = (this->Y * other.Z) - (this->Z * other.Y);
	double crossY = (this->Z * other.X) - (this->X * other.Z);
	double crossZ = (this->X * other.Y) - (this->Y * other.X);
	return Dir(crossX, crossY, crossZ);
}

double Dir::Dot(Dir other) {
	return GetOCC().Dot(other);
}

void Dir::Reverse() {
	X = -X;
	Y = -Y;
	Z = -Z;
}

Dir Dir::Reversed() {
	double newX = -X;
	double newY = -Y;
	double newZ = -Z;
	return Dir(newX, newY, newZ);
}

void Dir::Transform(Trsf^ T) {
	gp_Dir newD = GetOCC().Transformed(T->GetOCC());
	X = newD.X();
	Y = newD.Y();
	Z = newD.Z();
}

Dir Dir::Transformed(Trsf^ T) {
	gp_Dir newD = GetOCC().Transformed(T->GetOCC());
	return Dir(newD);
}

Vec Dir::ToVec(double factor) {
	return Vec(X * factor, Y * factor, Z * factor);
}

Eigen::Vector3d Dir::ToVector3d() {
	return Eigen::Vector3d(X, Y, Z);
}

void Dir::Normalize() {
	double m = std::sqrt(X * X + Y * Y + Z * Z);
	X = X / m;
	Y = Y / m;
	Z = Z / m;
}

gp_Dir Dir::GetOCC() {
	return gp_Dir(X, Y, Z);
}

Object^ Dir::Clone() {
	return Dir(X, Y, Z);
}

System::String^ Dir::ToString() {
	return X.ToString("F3") + ", " + Y.ToString("F3") + ", " + Z.ToString("F3");
}

}
}
}