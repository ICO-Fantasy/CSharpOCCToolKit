#include "ICO_AABB.h"
//local
#include "ICO_Pnt.h"

using namespace OCCTK::OCC::gp;

namespace OCCTK {
namespace OCC {
namespace Bnd {

AABB::AABB() {
	myBox = new Bnd_Box();
}

AABB::AABB(Bnd_Box* box) {
	myBox = box;
}

/// <summary>
/// 返回左下角点 + Gap
/// </summary>
/// <returns></returns>
Pnt^ AABB::CornerMin() {
	return gcnew Pnt(myBox->CornerMin());
}

/// <summary>
/// 返回右下角点 + Gap
/// </summary>
/// <returns></returns>
Pnt^ AABB::CornerMax() {
	return gcnew Pnt(myBox->CornerMax());
}

/// <summary>
/// 返回 xMin, yMin, zMin, xMax, yMax, zMax
/// </summary>
/// <returns></returns>
array<double>^ AABB::Get() {
	double xMin, yMin, zMin, xMax, yMax, zMax;
	try {
		myBox->Get(xMin, yMin, zMin, xMax, yMax, zMax);
	}
	catch (const Standard_ConstructionError& e) {
		System::Text::StringBuilder^ sb = gcnew System::Text::StringBuilder("");
		sb->Append(e.GetMessageString());
		sb->Append("空盒子");
		throw gcnew System::Exception(sb->ToString());
	}
	return gcnew array<double>(6) { xMin, yMin, zMin, xMax, yMax, zMax };
}


}
}
}
