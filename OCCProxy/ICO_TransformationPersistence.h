//#pragma once
//#include <Graphic3d_TransformPers.hxx>
//#include <Graphic3d_TransModeFlags.hxx>
//#include<Standard_Handle.hxx>
////local
//#include "ICO_BaseObject.h"
//#include "ICO_Pnt.h"
//
//namespace OCCTK {
//namespace OCC {
//namespace gp {
//namespace Aspect {
//enum class TypeOfTriedronPosition;
//}
//value struct Pnt;
//}
//namespace Graphic3d {
//
//enum class TransModeFlags;
//
///// <summary>
///// 持久化变换
///// </summary>
//public ref class TransformationPersistence :BaseObject {
//private:
//	Handle(Graphic3d_TransformPers) myTP() { return Handle(Graphic3d_TransformPers)::DownCast(NativeHandle); }
//public:
//	TransformationPersistence(TransModeFlags theMode) {
//		NativeHandle = new Graphic3d_TransformPers(Graphic3d_TransModeFlags(theMode));
//	}
//	TransformationPersistence(TransModeFlags theMode, gp::Pnt point) {
//		NativeHandle = new Graphic3d_TransformPers(Graphic3d_TransModeFlags(theMode), point);
//	}
//	TransformationPersistence(TransModeFlags theMode, TypeOfTriedronPosition theCorner, int x, int y) {
//		NativeHandle = new Graphic3d_TransformPers(Graphic3d_TransModeFlags(theMode), Aspect_TypeOfTriedronPosition(theCorner), Graphic3d_Vec2i(x, y));
//	}
//};
//
//}
//}
//}
//
