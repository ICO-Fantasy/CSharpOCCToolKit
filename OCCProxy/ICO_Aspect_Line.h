#pragma once
#include<Aspect_TypeOfLine.hxx>
namespace OCCTK {
namespace OCC {
namespace Aspect {

/// <summary>
/// 线的类型
/// </summary>
public enum class Line {
	Aspect_TOL_EMPTY = Aspect_TypeOfLine::Aspect_TOL_EMPTY,
	Aspect_TOL_SOLID = Aspect_TypeOfLine::Aspect_TOL_SOLID,
	Aspect_TOL_DASH = Aspect_TypeOfLine::Aspect_TOL_DASH,
	Aspect_TOL_DOT = Aspect_TypeOfLine::Aspect_TOL_DOT,
	Aspect_TOL_DOTDASH = Aspect_TypeOfLine::Aspect_TOL_DOTDASH,
	Aspect_TOL_USERDEFINED = Aspect_TypeOfLine::Aspect_TOL_USERDEFINED,

	Hidden = Aspect_TOL_EMPTY, //!< hidden
	Solid = Aspect_TOL_SOLID, //!< continuous
	Dash = Aspect_TOL_DASH, //!< dashed 2.0,1.0 (MM)
	Dot = Aspect_TOL_DOT, //!< dotted 0.2,0.5 (MM)
	DotDash = Aspect_TOL_DOTDASH, //!< mixed  10.0,1.0,2.0,1.0 (MM)
	Userdefined = Aspect_TOL_USERDEFINED, //!< defined by Users
};

}
}
}