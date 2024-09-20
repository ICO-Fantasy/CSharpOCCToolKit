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
protected:
	// 析构函数用于清理非托管资源
	!Color() {
		if (myColor != 0) {
			delete myColor;
			myColor = 0;
		}
	}

	// 终结器（finalizer）用于垃圾回收时的清理
	~Color() {
		// 调用析构函数来清理非托管资源
		this->!Color();
	}
};

}
}


