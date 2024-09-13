﻿#pragma once
#include <AIS_InteractiveObject.hxx>
#include <AIS_Shape.hxx>
#include <Standard_Transient.hxx>
#include <TopoDS_Shape.hxx>
//包装C++类到托管类
#include <NCollection_Haft.h> 
//Local
#include "ICO_TopoDS_Shape.h"
#include "ICO_InteractiveObject.h"
#include "ICO_Trsf.h"

using namespace OCCTK::OCC::TopoDS;
using namespace System;

namespace OCCTK {
namespace OCC {
namespace AIS {

public ref class AShape :public InteractiveObject {
public:
	AShape(const Handle(AIS_InteractiveObject) aInteractive);
	AShape(const Handle(AIS_Shape) aAISShape);
	AShape(const AIS_Shape& aAISShape);
	AShape(const TopoDS_Shape& aShape);
	AShape(System::IntPtr aShapePtr);
	AShape(TopoDS::TShape^ aShape);
public:
	Handle(AIS_Shape) GetOCC();
	System::IntPtr GetIntPtr();
public:
	TopoDS::TShape^ Shape();
	void SetColor(int R, int G, int B);
	void SetTransparency(double theFactor);
	bool HasInteractiveContext();
	Trsf^ LocalTransformation();
	void RemoveSelf();
	virtual bool Equals(System::Object^ obj) override;

private:
	NCollection_Haft<Handle(AIS_Shape)> myShape;
	//protected:
	//	// 析构函数用于清理非托管资源
	//	!AShape() {
	//		delete myShape;
	//	}
	//
	//	// 终结器（finalizer）用于垃圾回收时的清理
	//	~AShape() {
	//		// 调用析构函数来清理非托管资源
	//		this->!AShape();
	//	}
};

}
}
}


