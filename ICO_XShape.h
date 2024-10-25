#pragma once
#include "ICO_Trsf.h"
namespace OCCTK {
namespace Extension {
value struct Color;
}
namespace OCC {
namespace gp {
ref class Trsf;
}
namespace Topo {
ref class TShape;
}
namespace AIS {
ref class AShape;
}
}
}

namespace OCCTK {
namespace Extension {

public ref class XShapeNode {
public:
	XShapeNode() {
		Name = nullptr;
		Parent = nullptr;
		Children = gcnew System::Collections::Generic::List<XShapeNode^>();
		Layer = -1;
		Transform = nullptr;
		Transparence = 0.0;
		TopoShape = nullptr;
		AISShape = nullptr;
		Color = nullptr;
	}
public:
	property System::String^ Name;
	property XShapeNode^ Parent;
	property System::Collections::Generic::List<XShapeNode^>^ Children;
	property int Layer;
	property OCC::gp::Trsf^ Transform;
	property OCC::gp::Trsf^ Location {OCC::gp::Trsf^ get() {
		if (Parent != nullptr) {
			return  Parent->Location->Multiplied(Transform);
		}
		else {
			return Transform;
		}
	}}
#pragma region 末端节点属性
	property OCC::Topo::TShape^ TopoShape;
	property OCC::AIS::AShape^ AISShape;
	property Extension::Color^ Color;
	property double Transparence;
#pragma endregion
};

public ref class XShape {
public:
	XShape() {
		Name = nullptr;
		Color = nullptr;
		Transparence = 0.0;
		Location = nullptr;
		Nodes = gcnew System::Collections::Generic::List<XShapeNode^>();
		TopoShape = nullptr;
		AISShape = nullptr;
	}
public:
	property System::String^ Name;
	property Color^ Color;
	property double Transparence;
	property OCC::gp::Trsf^ Location;
	property System::Collections::Generic::List<XShapeNode^>^ Nodes;
	/// <summary>
	/// 当只有一个TopoShape的时候，储存在TopoShape中，此时Nodes中应该为空
	/// </summary>
	property OCC::Topo::TShape^ TopoShape;
	property OCC::AIS::AShape^ AISShape;
};

}
}

