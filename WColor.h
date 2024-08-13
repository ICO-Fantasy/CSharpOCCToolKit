#pragma once
#include <Quantity_Color.hxx>
//包装C++类到托管类
#include <NCollection_Haft.h> 
#include <functional>
namespace OCCTK {
namespace Ex {

public ref class WColor {
public:
	WColor() { myColor() = Quantity_Color(Quantity_NOC_GRAY); }
	WColor(int R, int G, int B) {
		Quantity_Color theColor((double)R / 255.0, (double)G / 255.0, (double)B / 255.0, Quantity_TOC_RGB);
		myColor() = theColor;
	};

	Quantity_Color GetOCC() {
		return myColor();
	};

	property Quantity_Color XAxis {Quantity_Color get() { return Quantity_Color(Quantity_NOC_RED); }}
	property Quantity_Color YAxis {Quantity_Color get() { return Quantity_Color(Quantity_NOC_GREEN); }}
	property Quantity_Color ZAxis {Quantity_Color get() { return Quantity_Color(Quantity_NOC_BLUE); }}
private:
	NCollection_Haft<Quantity_Color> myColor;
};

}
}


