#pragma once
#include <Quantity_Color.hxx>
#include <Quantity_TypeOfColor.hxx>

namespace OCCTK {
namespace Extension {

public value struct Color {
public:
    Color(int r, int g, int b);
    Color(System::String^ hex);
    Color(Quantity_Color Qcolor);

    Quantity_Color GetOCC() {
        return Quantity_Color((double)R / 255.0, (double)G / 255.0, (double)B / 255.0, Quantity_TOC_RGB);
    };
    //! 隐式转换为 Quantity_Color
    static operator Quantity_Color (Color p) { return p.GetOCC(); }
    virtual System::String^ ToString() override {
        return "(" + R + ", " + G + ", " + B + ")";
    };
public:
    // 定义一些常用的颜色
    static property Color Red {Color get() { return Color(255, 0, 0); }}
    static property Color Green {Color get() { return Color(0, 255, 0); }}
    static property Color Blue {Color get() { return Color(0, 0, 255); }}
    static property Color Black {Color get() { return Color(0, 0, 0); }}
    static property Color White {Color get() { return Color(255, 255, 255); }}
    static property Color Gray {Color get() { return Color(125, 125, 125); }}
    static property Color Purple {Color get() { return Color(128, 0, 128); }}
    static property Color XAxis {Color get() { return Blue; }}
    static property Color YAxis {Color get() { return Green; }}
    static property Color ZAxis {Color get() { return Red; }}

public:
    property int R;
    property int G;
    property int B;
};

}
}


