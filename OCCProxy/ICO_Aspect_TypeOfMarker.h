#pragma once
#include<Aspect_TypeOfMarker.hxx>
namespace OCCTK {
namespace OCC {
namespace Aspect {

/// <summary>
/// 点的标记类型
/// </summary>
public enum class TypeOfMarker {
	Aspect_TOM_EMPTY = Aspect_TypeOfMarker::Aspect_TOM_EMPTY,  //!< hidden
	Aspect_TOM_POINT = Aspect_TypeOfMarker::Aspect_TOM_POINT,  //!< point   .
	Aspect_TOM_PLUS = Aspect_TypeOfMarker::Aspect_TOM_PLUS,        //!< plus    +
	Aspect_TOM_STAR = Aspect_TypeOfMarker::Aspect_TOM_STAR,        //!< star    *
	Aspect_TOM_X = Aspect_TypeOfMarker::Aspect_TOM_X,           //!< cross   x
	Aspect_TOM_O = Aspect_TypeOfMarker::Aspect_TOM_O,           //!< circle  O
	Aspect_TOM_O_POINT = Aspect_TypeOfMarker::Aspect_TOM_O_POINT,     //!< a point in a circle
	Aspect_TOM_O_PLUS = Aspect_TypeOfMarker::Aspect_TOM_O_PLUS,      //!< a plus  in a circle
	Aspect_TOM_O_STAR = Aspect_TypeOfMarker::Aspect_TOM_O_STAR,      //!< a star  in a circle
	Aspect_TOM_O_X = Aspect_TypeOfMarker::Aspect_TOM_O_X,         //!< a cross in a circle
	Aspect_TOM_RING1 = Aspect_TypeOfMarker::Aspect_TOM_RING1,       //!< a large  ring
	Aspect_TOM_RING2 = Aspect_TypeOfMarker::Aspect_TOM_RING2,       //!< a medium ring
	Aspect_TOM_RING3 = Aspect_TypeOfMarker::Aspect_TOM_RING3,       //!< a small  ring
	Aspect_TOM_BALL = Aspect_TypeOfMarker::Aspect_TOM_BALL,        //!< a ball with 1 color and different saturations
	Aspect_TOM_USERDEFINED = Aspect_TypeOfMarker::Aspect_TOM_USERDEFINED,//!< defined by Users (custom image)

	Point = Aspect_TOM_POINT,
	Plus = Aspect_TOM_PLUS,
	Cross = Aspect_TOM_X,
};

}
}
}