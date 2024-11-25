#include "ICO_OBJExchange.h"
#include <BinXCAFDrivers.hxx>
#include <RWObj_CafReader.hxx>
#include <TDF_Label.hxx>
#include <TDF_LabelSequence.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_LayerTool.hxx>
#include <XCAFDoc_MaterialTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <UnitsMethods.hxx>
#include <TDataStd_Name.hxx>
#include <TCollection_AsciiString.hxx>
#include <XCAFApp_Application.hxx>
//local
#include "ICO_StringExchange.h"
#include "..\Extension\ICO_XShape.h"
#include "..\Extension\ICO_Color.h"
#include "..\OCCProxy\ICO_Topo_Shape.h"
#include "..\OCCProxy\ICO_AIS_Shape.h"
#include "ICO_XCAFDoc.h"

using namespace OCCTK::DataExchange;
using namespace OCCTK::Extension;
using namespace OCCTK::OCC::Topo;
using namespace OCCTK::OCC::AIS;

namespace OCCTK {
namespace IO {

OBJExchange::OBJExchange(System::String^ filePath) {
	//Handle(TDocStd_Document) anXdeDoc;
	Handle(TDocStd_Document) doc;
	Handle(XCAFApp_Application) anApp = XCAFApp_Application::GetApplication();
	anApp->NewDocument("OBJ-XCAF", doc);

	Handle(RWObj_CafReader) aReader = new RWObj_CafReader();
	aReader->SetSinglePrecision(true);
	Standard_Real m_Unit = UnitsMethods::GetCasCadeLengthUnit();
	aReader->SetSystemLengthUnit(m_Unit * 0.001);
	aReader->SetSystemCoordinateSystem(RWMesh_CoordinateSystem_Zup);
	aReader->SetFileLengthUnit(0.001);
	aReader->SetFileCoordinateSystem(RWMesh_CoordinateSystem_Zup);
	aReader->SetDocument(doc);
	TCollection_AsciiString cPath = DataExchange::ToAsciiString(filePath);
	bool status = aReader->Perform(cPath, Message_ProgressRange());
	if (!status) {
		throw gcnew System::Exception("读取失败");
	}
	XCAFDocExchange docParser(doc);
	myShape = docParser.Shape();
}

Extension::XShape^ OBJExchange::Shape() {
	return myShape;
}

}
}