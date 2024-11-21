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

using namespace OCCTK::Extension;
using namespace OCCTK::OCC::Topo;
using namespace OCCTK::OCC::AIS;

namespace OCCTK {
namespace IO {

OBJExchange::OBJExchange(System::String^ filePath) {
	//Handle(TDocStd_Document) anXdeDoc;
	Handle(TDocStd_Document) doc;
	Handle(XCAFApp_Application) anApp = XCAFApp_Application::GetApplication();
	anApp->NewDocument("MDTV-XCAF", doc);
	Handle(XCAFDoc_ShapeTool) shapeTool = XCAFDoc_DocumentTool::ShapeTool(doc->Main());
	Handle(XCAFDoc_ColorTool) colorTool = XCAFDoc_DocumentTool::ColorTool(doc->Main());
	Handle(XCAFDoc_MaterialTool) matTool = XCAFDoc_DocumentTool::MaterialTool(doc->Main());
	Handle(XCAFDoc_LayerTool) layerTool = XCAFDoc_DocumentTool::LayerTool(doc->Main());

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

	TDF_LabelSequence labels;
	TopoDS_Shape shape = shapeTool->GetShape(doc->Main());
	if (!shape.IsNull()) {
		mySingleShape->TopoShape = gcnew TShape(shape);
		return;
	}
	//todo
	shapeTool->GetFreeShapes(labels);
	//+ 单个根节点
	System::Diagnostics::Debug::WriteLine((labels.Size()));
	if (labels.Size() == 1) {
		TDF_Label label = labels.Value(1);
		TDF_LabelSequence subLabels;
		shapeTool->GetComponents(label, subLabels);
		//! 单个形状
		if (subLabels.Size() == 0) {
			mySingleShape = gcnew XShape();
			mySingleShape->TopoShape = gcnew TShape(shapeTool->GetShape(label));
			mySingleShape->AISShape = gcnew AShape(shapeTool->GetShape(label));
			Quantity_Color c;
			colorTool->GetColor(label, c);
			mySingleShape->Color = gcnew Color(c);
			Handle(TDataStd_Name) name;
			label.FindAttribute(TDataStd_Name::GetID(), name);
			mySingleShape->Name = DataExchange::WcharToString(name->Get().ToWideString());
			return;
		}
	}

}

Extension::XShape^ OBJExchange::SingleShape() {
	if (mySingleShape == nullptr) {
		throw gcnew System::Exception("SingleShape为空");
	}
	return mySingleShape;
}

}
}