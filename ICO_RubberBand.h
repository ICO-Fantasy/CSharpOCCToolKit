﻿#pragma once
#include<AIS_RubberBand.hxx>
//包装C++类到托管类
#include <NCollection_Haft.h> 
//Local
#include "ICO_InteractiveObject.h"
#include "ICO_Color.h"
#include "ICO_Aspect_Line.h"
using namespace OCCTK::Extension;

namespace OCCTK {
namespace OCC {
namespace AIS {

public ref class RubberBand :public InteractiveObject {
public:
	RubberBand();
	Handle(AIS_RubberBand) GetOCC();
	Handle(Standard_Transient) GetStd() override;

	void SetRectangle(int minX, int minY, int maxX, int maxY);
	void SetLineType(Aspect::Line theType);
	void SetFillColor(Color^ theColor);
	void SetFillTransparency(double transparency);
	void SetLineColor(Color^ theColor);
	void SetFilling(bool isFilling);
private:
	NCollection_Haft<Handle(AIS_RubberBand)> myRubber;
};

}
}
}