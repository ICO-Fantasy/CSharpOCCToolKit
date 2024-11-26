#include "ICO_TextLabel.h"
#include <AIS_TextLabel.hxx>
//local
#include "..\Extension\ICO_Color.h"
#include "..\DataExchange\ICO_StringExchange.h"
#include "ICO_Pnt.h"

using namespace OCCTK::OCC::Topo;
using namespace OCCTK::OCC::gp;
using namespace OCCTK::DataExchange;

namespace OCCTK {
namespace OCC {
namespace AIS {
TextLabel::TextLabel() {
	NativeHandle = new AIS_TextLabel();
}
TextLabel::TextLabel(const AIS_TextLabel& label) {
	NativeHandle = new AIS_TextLabel(label);
}

TextLabel::TextLabel(System::String^ str) {
	NativeHandle = new AIS_TextLabel();
	myShape()->SetText(ConvertChineseToUnicode(str));
}

void TextLabel::SetFont(System::String^ str) {
	myShape()->SetFont(ToAsciiString(str).ToCString());
}

void TextLabel::SetText(System::String^ str) {
	myShape()->SetText(ToAsciiString(str));
}

void TextLabel::SetHeight(double hight) {
	myShape()->SetHeight(hight);
}

void TextLabel::SetColor(int R, int G, int B) {
	myShape()->SetColor((gcnew Extension::Color(R, G, B))->GetOCC());
}

void TextLabel::SetPosition(Pnt position) {
	myShape()->SetPosition(position);
}

void TextLabel::SetZoomable(bool zoomable) {
	myShape()->SetZoomable(zoomable);
}

void TextLabel::SetColor(Extension::Color^ color) {
	myShape()->SetColor(color->GetOCC());
}

void TextLabel::SetTransparency(double theFactor) {
	myShape()->SetTransparency(theFactor);
}

}
}
}