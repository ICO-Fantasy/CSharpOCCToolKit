#pragma once
#include <AIS_InteractiveObject.hxx>
#include <AIS_TextLabel.hxx>
//Local
#include "ICO_InteractiveObject.h"

class TopoDS_Shape;
namespace OCCTK {
namespace Extension {
value struct Color;
}
namespace OCC {
namespace gp {
ref class Trsf;
ref class Ax2;
value struct Pnt;
}
namespace Topo {
ref class TShape;
}
}
}

namespace OCCTK {
namespace OCC {
namespace AIS {

public ref class TextLabel :public InteractiveObject {
private:
	Handle(AIS_TextLabel) myShape() { return Handle(AIS_TextLabel)::DownCast(NativeHandle); }
public:
	TextLabel();
	TextLabel(const Handle(AIS_TextLabel) label) : InteractiveObject(label) {};
	TextLabel(const AIS_TextLabel& aShape);
	TextLabel(System::String^ str);
public:
	Handle(AIS_TextLabel) GetOCC() { return myShape(); };
public:
	void SetFont(System::String^ str);
	void SetText(System::String^ str);
	void SetHeight(double hight);
	void SetColor(int R, int G, int B);
	void SetPosition(gp::Pnt position);
	void SetZoomable(bool zoomable);
	void SetColor(Extension::Color^ color);
	void SetTransparency(double theFactor);
};

}
}
}


