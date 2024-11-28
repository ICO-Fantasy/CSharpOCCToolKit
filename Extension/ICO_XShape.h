#pragma once
#include "ICO_Color.h"
#include "..\OCCProxy\ICO_Trsf.h"

namespace OCCTK {
namespace Extension {
}
namespace OCC {
namespace AIS {
ref class AShape;
}
namespace gp {
ref class Trsf;
value struct Ax2;
}
namespace Topo {
ref class TShape;
ref class TEdge;
ref class TVertex;
ref class TFace;
}
}
}

namespace OCCTK {
namespace Extension {
//前向声明
value struct Color;

public ref class XShape {
public:

	XShape();
	XShape(bool isShape);
public:
	property bool IsShape;
	property bool IsAssembly {bool get() { return !IsShape; }};
	property System::String^ Name;
	property System::Nullable<Extension::Color> Color;
	property System::String^ Material;
	/// <summary>
	/// 透明度，范围(0-100)，默认值 -1
	/// </summary>
	property double Transparence;
	/// <summary>
	/// 父节点
	/// </summary>
	property XShape^ Parent;
	property System::Collections::Generic::List<XShape^>^ Children;
	property int Layer;
	property OCC::gp::Trsf^ Transform;
	property OCC::gp::Trsf^ Location {OCC::gp::Trsf^ get(); }
	property OCC::Topo::TShape^ TopoShape {OCC::Topo::TShape^ get() { return myTopo; } void set(OCC::Topo::TShape^ value); };
	property OCC::AIS::AShape^ AISShape {OCC::AIS::AShape^ get(); };
private:
	OCC::Topo::TShape^ myTopo;
	OCC::AIS::AShape^ myAIS;
};

}
}

