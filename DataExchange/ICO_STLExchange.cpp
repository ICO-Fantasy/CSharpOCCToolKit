#include "ICO_STLExchange.h"
#include <RWStl.hxx>
#include <TopoDS_Face.hxx>
#include <BRep_Builder.hxx>
#include <Poly_Triangulation.hxx>
#include <TCollection_AsciiString.hxx>
//local
#include "..\DataExchange\ICO_StringExchange.h"
#include "..\OCCProxy\ICO_Poly_Triangulation.h"
#include "..\OCCProxy\ICO_Topo_Face.h"

using namespace OCCTK::DataExchange;
using namespace OCCTK::OCC::Poly;
using namespace OCCTK::OCC::Topo;

namespace OCCTK {
namespace IO {

STLExchange::STLExchange(System::String^ filePath) {
	TCollection_AsciiString cPath = ToAsciiString(filePath);
	Handle(Poly_Triangulation) result = RWStl::ReadFile(cPath);
	if (result.IsNull()) {
		throw gcnew System::Exception("读取失败");
	}
	myPT = gcnew PTriangulation(result);
}

TShape^ STLExchange::Shape() {
	TopoDS_Face face;
	BRep_Builder builder;
	builder.MakeFace(face, myPT->GetOCC());
	return gcnew TFace(face);
}

OCC::Poly::PTriangulation^ STLExchange::PolyTriangulation() {
	return myPT;
}

}
}