#pragma once
#include <Quantity_Color.hxx>
#include <Quantity_TypeOfColor.hxx>

namespace OCCTK {
namespace Extension {

public value struct Color {
public:
	Color(int r, int g, int b) {
		R = r;
		G = g;
		B = b;
	};
	Color(Quantity_Color Qcolor) {
		R = (int)(Qcolor.Red() * 255.0);
		G = (int)(Qcolor.Green() * 255.0);
		B = (int)(Qcolor.Blue() * 255.0);
	}
	Quantity_Color GetOCC() {
		return Quantity_Color((double)R / 255.0, (double)G / 255.0, (double)B / 255.0, Quantity_TOC_RGB);
	};
	virtual System::String^ ToString() override {
		return "(" + R + ", " + G + ", " + B + ")";
	};
public:
	// 定义一些常用的颜色
	static Color Red() {
		return Color(255, 0, 0);
	}
	static Color Green() {
		return Color(0, 255, 0);
	}
	static Color Blue() {
		return Color(0, 0, 255);
	}
	static Color Black() {
		return Color(0, 0, 0);
	}
	static Color White() {
		return Color(255, 255, 255);
	}
	static Color Gray() {
		return Color(125, 125, 125);
	}

	static Color XAxis() {
		return Blue();
	}
	static Color YAxis() {
		return Green();
	}
	static Color ZAxis() {
		return Red();
	}

public:
	property int R;
	property int G;
	property int B;
};

}
}


