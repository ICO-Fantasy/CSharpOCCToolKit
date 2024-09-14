#pragma once
#include <Quantity_Color.hxx>

namespace OCCTK {
namespace Extension {

public ref class Color {
public:
	Color() { myColor = new Quantity_Color(Quantity_NOC_GRAY); }
	Color(int R, int G, int B) {
		myColor = new Quantity_Color((double)R / 255.0, (double)G / 255.0, (double)B / 255.0, Quantity_TOC_RGB);
	};

	Quantity_Color GetOCC() {
		return *myColor;
	};

	property Quantity_Color XAxis {Quantity_Color get() { return Quantity_Color(Quantity_NOC_RED); }}
	property Quantity_Color YAxis {Quantity_Color get() { return Quantity_Color(Quantity_NOC_GREEN); }}
	property Quantity_Color ZAxis {Quantity_Color get() { return Quantity_Color(Quantity_NOC_BLUE); }}
private:
	Quantity_Color* myColor;
};

}
}


