#pragma once
#include <V3d_TypeOfOrientation.hxx>
namespace OCCTK {
namespace OCC {
namespace V3d {

public enum class ViewOrientation {
	V3d_Xpos = V3d_TypeOfOrientation::V3d_Xpos, //!< (+Y+Z) view
	V3d_Ypos = V3d_TypeOfOrientation::V3d_Ypos, //!< (-X+Z) view
	V3d_Zpos = V3d_TypeOfOrientation::V3d_Zpos, //!< (+X+Y) view
	V3d_Xneg = V3d_TypeOfOrientation::V3d_Xneg, //!< (-Y+Z) view
	V3d_Yneg = V3d_TypeOfOrientation::V3d_Yneg, //!< (+X+Z) view
	V3d_Zneg = V3d_TypeOfOrientation::V3d_Zneg, //!< (+X-Y) view

	V3d_XposYpos = V3d_TypeOfOrientation::V3d_XposYpos,
	V3d_XposZpos = V3d_TypeOfOrientation::V3d_XposZpos,
	V3d_YposZpos = V3d_TypeOfOrientation::V3d_YposZpos,
	V3d_XnegYneg = V3d_TypeOfOrientation::V3d_XnegYneg,
	V3d_XnegYpos = V3d_TypeOfOrientation::V3d_XnegYpos,
	V3d_XnegZneg = V3d_TypeOfOrientation::V3d_XnegZneg,
	V3d_XnegZpos = V3d_TypeOfOrientation::V3d_XnegZpos,
	V3d_YnegZneg = V3d_TypeOfOrientation::V3d_YnegZneg,
	V3d_YnegZpos = V3d_TypeOfOrientation::V3d_YnegZpos,
	V3d_XposYneg = V3d_TypeOfOrientation::V3d_XposYneg,
	V3d_XposZneg = V3d_TypeOfOrientation::V3d_XposZneg,
	V3d_YposZneg = V3d_TypeOfOrientation::V3d_YposZneg,
	V3d_XposYposZpos = V3d_TypeOfOrientation::V3d_XposYposZpos,
	V3d_XposYnegZpos = V3d_TypeOfOrientation::V3d_XposYnegZpos,
	V3d_XposYposZneg = V3d_TypeOfOrientation::V3d_XposYposZneg,
	V3d_XnegYposZpos = V3d_TypeOfOrientation::V3d_XnegYposZpos,
	V3d_XposYnegZneg = V3d_TypeOfOrientation::V3d_XposYnegZneg,
	V3d_XnegYposZneg = V3d_TypeOfOrientation::V3d_XnegYposZneg,
	V3d_XnegYnegZpos = V3d_TypeOfOrientation::V3d_XnegYnegZpos,
	V3d_XnegYnegZneg = V3d_TypeOfOrientation::V3d_XnegYnegZneg,

	// +Z-up +Y-forward convention
	V3d_TypeOfOrientation_Zup_AxoLeft = V3d_XnegYnegZpos,  //!< +Z-up +Y-forward Left +Front+Top
	V3d_TypeOfOrientation_Zup_AxoRight = V3d_XposYnegZpos,  //!< +Z-up +Y-forward Right+Front+Top
	V3d_TypeOfOrientation_Zup_Front = V3d_Yneg,          //!< +Z-up +Y-forward Front  (+X+Z) view
	V3d_TypeOfOrientation_Zup_Back = V3d_Ypos,          //!< +Z-up +Y-forward Back   (-X+Z) view
	V3d_TypeOfOrientation_Zup_Top = V3d_Zpos,          //!< +Z-up +Y-forward Top    (+X+Y) view
	V3d_TypeOfOrientation_Zup_Bottom = V3d_Zneg,          //!< +Z-up +Y-forward Bottom (+X-Y) view
	V3d_TypeOfOrientation_Zup_Left = V3d_Xneg,          //!< +Z-up +Y-forward Left   (-Y+Z) view
	V3d_TypeOfOrientation_Zup_Right = V3d_Xpos,          //!< +Z-up +Y-forward Right  (+Y+Z) view

	// +Y-up -Z-forward convention
	V3d_TypeOfOrientation_Yup_AxoLeft = V3d_XnegYposZpos,  //!< +Y-up -Z-forward Left +Front+Top
	V3d_TypeOfOrientation_Yup_AxoRight = V3d_XposYposZpos,  //!< +Y-up -Z-forward Right+Front+Top
	V3d_TypeOfOrientation_Yup_Front = V3d_Zpos,          //!< +Y-up -Z-forward Front  (+X+Y) view
	V3d_TypeOfOrientation_Yup_Back = V3d_Zneg,          //!< +Y-up -Z-forward Back   (-X+Y) view
	V3d_TypeOfOrientation_Yup_Top = V3d_Ypos,          //!< +Y-up -Z-forward Top    (+X-Z) view
	V3d_TypeOfOrientation_Yup_Bottom = V3d_Yneg,          //!< +Y-up -Z-forward Bottom (-X-Z) view
	V3d_TypeOfOrientation_Yup_Left = V3d_Xpos,          //!< +Y-up -Z-forward Left   (-Z+Y) view
	V3d_TypeOfOrientation_Yup_Right = V3d_Xneg,          //!< +Y-up -Z-forward Right  (+Z+Y) view
	//等轴测
	Axo = V3d_XposYnegZpos,
	//六面体
	Front = V3d_Yneg,
	Back = V3d_Ypos,
	Top = V3d_Zpos,
	Bottom = V3d_Zneg,
	Left = V3d_Xneg,
	Right = V3d_Xpos,
};

}
}
}