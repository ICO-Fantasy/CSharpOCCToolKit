#pragma once
#include<Graphic3d_Structure.hxx>
#include<gp_Pnt.hxx>
#include<gp_Dir.hxx>
#include<gp_Vec.hxx>
#include<Prs3d_Arrow.hxx>
//包装C++类到托管类
#include <NCollection_Haft.h> 
//Local
#include "..\OCCProxy\ICO_Vec.h"
#include "..\OCCProxy\ICO_Pnt.h"
#include "..\OCCProxy\ICO_Dir.h"

namespace OCCTK {
namespace Extension {

public ref class Arrow {
public:
	Arrow() {
		Prs3d_Arrow::Draw(
			myStructure().CurrentGroup(),
			myStart->GetOCC(),
			gp_Dir(myVec->GetOCC()),
			20 * M_PI / 180,
			myVec->GetOCC().Magnitude()
		);
	}
	//		myStructure.Display()
	//	# it would be more coherent if a AIS_InteractiveObject
	//	# would be returned
	//	if update:
	//self.Repaint()
	//	return myStructure
private:
	OCC::gp::Pnt^ myStart;
	OCC::gp::Vec^ myVec;
	NCollection_Haft<Graphic3d_Structure> myStructure;
};

}
}