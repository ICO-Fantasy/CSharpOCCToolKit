#include "ICO_XCAFDoc.h"
#include <BinXCAFDrivers.hxx>
#include <BRep_Builder.hxx>
#include <Quantity_Color.hxx>
#include <Standard_Handle.hxx>
#include <STEPCAFControl_Reader.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDataStd_Name.hxx>
#include <TDF_Label.hxx>
#include <TDF_LabelSequence.hxx>
#include <TDF_Tool.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Shape.hxx>
#include <XCAFApp_Application.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_ColorType.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_LayerTool.hxx>
#include <XCAFDoc_MaterialTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <TopLoc_Location.hxx>
//local
#include "ICO_StringExchange.h"
#include "..\OCCProxy\ICO_AIS_Shape.h"
#include "..\OCCProxy\ICO_Topo_Shape.h"
#include "..\OCCProxy\ICO_Trsf.h"
#include "..\Extension\ICO_Color.h"
#include "..\Extension\ICO_XShape.h"
#include "..\DataExchange\ICO_StringExchange.h"

using namespace OCCTK::OCC::gp;
using namespace OCCTK::Extension;
using namespace OCCTK::OCC::AIS;
using namespace OCCTK::OCC::Topo;

static System::String^ GetLabelName(TDF_Label label) {
	//TCollection_AsciiString name;
	//TDF_Tool::Entry(label, name);
	Handle(TDataStd_Name) stdName;
	label.FindAttribute(TDataStd_Name::GetID(), stdName);
	System::String^ labelName;
	if (!stdName->IsValid()) {
		labelName = "Invalid Name";
	}
	else {
		labelName = OCCTK::DataExchange::WcharToString(stdName->Get().ToWideString());
	}
	return labelName;
}

static XShape^ GetShapeInfo(const TDF_Label& label, const Handle(TDocStd_Document)& doc) {
	XShape^ result = gcnew XShape(true);
	Handle(XCAFDoc_ShapeTool) shapeTool = XCAFDoc_DocumentTool::ShapeTool(doc->Main());
	Handle(XCAFDoc_ColorTool) colorTool = XCAFDoc_DocumentTool::ColorTool(doc->Main());
	Handle(XCAFDoc_MaterialTool) matTool = XCAFDoc_DocumentTool::MaterialTool(doc->Main());
	Handle(XCAFDoc_LayerTool) layerTool = XCAFDoc_DocumentTool::LayerTool(doc->Main());
	TopoDS_Shape shape = shapeTool->GetShape(label);
	result->TopoShape = gcnew TShape(shape);
	//! 名称
	System::String^ name = GetLabelName(label);
	result->Name = name;
	//! 颜色
	//如果使用了SHUO设置颜色
	Quantity_Color c;
	bool colorSeted = false;
	if (colorTool->GetInstanceColor(shape, XCAFDoc_ColorType::XCAFDoc_ColorGen, c)) { colorSeted = true; }
	else if (colorTool->GetInstanceColor(shape, XCAFDoc_ColorType::XCAFDoc_ColorSurf, c)) { colorSeted = true; }
	else if (colorTool->GetInstanceColor(shape, XCAFDoc_ColorType::XCAFDoc_ColorCurv, c)) { colorSeted = true; }
	//尝试直接获取颜色
	if (!colorSeted) {
		if (colorTool->GetColor(label, XCAFDoc_ColorType::XCAFDoc_ColorGen, c)) { colorSeted = true; }
		else if (colorTool->GetColor(label, XCAFDoc_ColorType::XCAFDoc_ColorSurf, c)) { colorSeted = true; }
		else if (colorTool->GetColor(label, XCAFDoc_ColorType::XCAFDoc_ColorCurv, c)) { colorSeted = true; }
	}
	if (colorSeted) {
		//System::Diagnostics::Debug::WriteLine("RGB: " + gcnew Color(c));
		result->Color = Color(c);
	}
	else {
		//System::Diagnostics::Debug::WriteLine("读取到的颜色为空");
	}
	//! 材料
	Handle(TCollection_HAsciiString) materialName;
	Handle(TCollection_HAsciiString) materialDescription;
	double materialDensity;
	Handle(TCollection_HAsciiString) materialDensName;
	Handle(TCollection_HAsciiString) materialDensValType;
	if (matTool->GetMaterial(label, materialName, materialDescription, materialDensity, materialDensName, materialDensValType)) {
		System::Diagnostics::Debug::WriteLine("材料属性" +
			gcnew System::String(materialName->ToCString()) +
			gcnew System::String(materialDescription->ToCString()) +
			materialDensity +
			gcnew System::String(materialDensName->ToCString()) +
			gcnew System::String(materialDensValType->ToCString())
		);
		result->Material = gcnew System::String(materialName->ToCString());
	}
	return result;
}

static XShape^ GetReference(const TDF_Label& label, const Handle(TDocStd_Document)& doc) {
	return GetShapeInfo(label, doc);
}

static XShape^ GetSubShape(const TDF_Label& label, const Handle(TDocStd_Document)& doc, gp_Trsf parentTrsf) {
	Handle(XCAFDoc_ShapeTool) shapeTool = XCAFDoc_DocumentTool::ShapeTool(doc->Main());
	Handle(XCAFDoc_ColorTool) colorTool = XCAFDoc_DocumentTool::ColorTool(doc->Main());
	Handle(XCAFDoc_MaterialTool) matTool = XCAFDoc_DocumentTool::MaterialTool(doc->Main());
	Handle(XCAFDoc_LayerTool) layerTool = XCAFDoc_DocumentTool::LayerTool(doc->Main());

	System::String^ labelName = GetLabelName(label);
	//System::Diagnostics::Debug::WriteLine("===================================== " + labelName + " =====================================");
	//TDF_LabelSequence labels_Users;
	//shapeTool->GetUsers(label, labels_Users);
	//System::Diagnostics::Debug::WriteLine(labelName + " 被 " + labels_Users.Length() + " 个标签引用");

	if (shapeTool->IsAssembly(label)) {
		System::Diagnostics::Debug::WriteLine(labelName + " 是装配体");
		XShape^ shape = gcnew XShape();
		shape->Name = labelName;
		TDF_LabelSequence labels_Components;
		shapeTool->GetComponents(label, labels_Components);
		TopLoc_Location loc = shapeTool->GetLocation(label);
		gp_Trsf theT = loc.Transformation();
		theT.PreMultiply(parentTrsf.Inverted());
		Trsf curentT =Trsf(theT);
		shape->Transform = curentT;
		System::Diagnostics::Debug::WriteLine(labelName + " 有 " + labels_Components.Length() + " 个部件");
		for (size_t i = 1; i < labels_Components.Length() + 1; i++) {
			TDF_Label ref = labels_Components.Value(i);
			XShape^ subShape = GetSubShape(ref, doc, theT);
			subShape->Parent = shape;
			shape->Children->Add(subShape);
		}
		return shape;
	}
	else if (shapeTool->IsReference(label)) {
		//System::Diagnostics::Debug::WriteLine(labelName + " 是引用");
		TDF_Label label_ReferredShape;
		shapeTool->GetReferredShape(label, label_ReferredShape);
		//System::Diagnostics::Debug::WriteLine(labelName + " 引用了 " + GetLabelName(label_ReferredShape));
		TopLoc_Location loc = shapeTool->GetLocation(label);
		gp_Trsf theT = loc.Transformation();
		theT.PreMultiply(parentTrsf.Inverted());
		XShape^ shape = GetReference(label_ReferredShape, doc);
		shape->Transform = Trsf(theT);
		return shape;
	}
	else if (shapeTool->IsSimpleShape(label)) {
		System::Diagnostics::Debug::WriteLine(labelName + " 是单个形状");
		return GetShapeInfo(label, doc);
	}
	throw gcnew System::Exception("不受支持的子节点类型");

	//if (shapeTool->IsShape(label)) {
	//	System::Diagnostics::Debug::WriteLine(labelName + " 是形状");
	//}
	//if (shapeTool->IsCompound(label)) {
	//	System::Diagnostics::Debug::WriteLine(labelName + " 是组合体");
	//}
	//if (shapeTool->IsComponent(label)) {
	//	System::Diagnostics::Debug::WriteLine(labelName + " 是部件");
	//}
	//if (shapeTool->IsSimpleShape(label)) {
	//	System::Diagnostics::Debug::WriteLine(labelName + " 是简单形状");
	//}


}

namespace OCCTK {
namespace Extension {

XCAFDocExchange::XCAFDocExchange(const Handle(TDocStd_Document)& doc) {
	myDoc() = doc;

	//开始解析
	Handle(XCAFDoc_ShapeTool) shapeTool = XCAFDoc_DocumentTool::ShapeTool(doc->Main());
	TDF_LabelSequence labels_FreeShapes;
	shapeTool->GetFreeShapes(labels_FreeShapes);
	System::Diagnostics::Debug::WriteLine("根节点有 " + labels_FreeShapes.Length() + " 个自由形状");
	if (labels_FreeShapes.Length() == 1) {
		for (size_t i = 1; i < labels_FreeShapes.Length() + 1; i++) {
			myShape = GetSubShape(labels_FreeShapes.Value(i), doc, gp_Trsf());
		}
	}
	else if (labels_FreeShapes.Length() > 1) {
		myShape = gcnew XShape();
		for (size_t i = 1; i < labels_FreeShapes.Length() + 1; i++) {
			myShape->Children->Add(GetSubShape(labels_FreeShapes.Value(i), doc, gp_Trsf()));
		}
	}
	else {
		throw gcnew System::Exception("文件中没有形状");
	}
}

XShape^ XCAFDocExchange::Shape() {
	return myShape;
}
//void XCAFDocExchange::test(System::String^ filePath) {
//	//处理输入路径编码格式
//	TCollection_AsciiString cPath = DataExchange::ToAsciiString(filePath);
//	//创建app
//	Handle(TDocStd_Application) app = new TDocStd_Application();
//	//定义文档格式
//	BinXCAFDrivers::DefineFormat(app);
//	Handle(TDocStd_Document) doc;
//	app->NewDocument("BinXCAF", doc);
//	Handle(XCAFDoc_ShapeTool) shapeTool = XCAFDoc_DocumentTool::ShapeTool(doc->Main());
//	Handle(XCAFDoc_ColorTool) colorTool = XCAFDoc_DocumentTool::ColorTool(doc->Main());
//	Handle(XCAFDoc_MaterialTool) matTool = XCAFDoc_DocumentTool::MaterialTool(doc->Main());
//	Handle(XCAFDoc_LayerTool) layerTool = XCAFDoc_DocumentTool::LayerTool(doc->Main());
//	//创建STEP文件读取器
//	STEPCAFControl_Reader reader;
//	reader.SetColorMode(true);//颜色
//	reader.SetLayerMode(true);//图层
//	reader.SetNameMode(true);//名称
//	//reader.SetMatMode(true);//材料
//	//reader.SetGDTMode(true);//几何尺寸和公差
//	//reader.SetPropsMode(true);//物理属性
//	//reader.SetSHUOMode(true);//Shape Usage Occurrence 文件关联
//	//reader.SetViewMode(true);//视图信息
//	IFSelect_ReturnStatus status = reader.ReadFile(cPath.ToCString());
//	if (status == IFSelect_RetDone) {
//		reader.Transfer(doc);
//	}
//	else {
//		throw gcnew System::Exception("读取失败");
//	}
//
//	//开始解析
//	TDF_LabelSequence labels_FreeShapes;
//	shapeTool->GetFreeShapes(labels_FreeShapes);
//	System::Diagnostics::Debug::WriteLine("根节点有 " + labels_FreeShapes.Length() + " 个自由形状");
//	for (size_t i = 1; i < labels_FreeShapes.Length() + 1; i++) {
//		GetSubShape(labels_FreeShapes.Value(i), doc);
//	}
//
//
//
//	//Handle(TDataStd_Name) name;
//	//label.FindAttribute(TDataStd_Name::GetID(), name);
//	//node->Name = DataExchange::WcharToString(name->Get().ToWideString());
//	//node->Transform = Trsf(shapeTool->GetLocation(label));
//	//if (shapeTool->IsAssembly(label)) {
//	//	TDF_LabelSequence sequence;
//	//	shapeTool->GetComponents(label, sequence);
//	//	for (size_t j = 1; j < sequence.Size(); ++j) {
//	//		XShapeNode^ childnode = LableToNode(sequence.Value(j), shapeTool, colorTool, matTool, layerTool);
//	//		node->Children->Add(childnode);
//	//	}
//	//}
//	//else if (shapeTool->IsShape(label)) {
//	//	TopoDS_Shape shape = shapeTool->GetShape(label);
//	//	node->TopoShape = gcnew TShape(shape);
//	//	node->AISShape = gcnew AShape(shape);
//	//	//如果使用了SHUO设置颜色
//	//	Quantity_Color c;
//	//	bool colorSeted = false;
//	//	if (colorTool->GetInstanceColor(shape, XCAFDoc_ColorType::XCAFDoc_ColorGen, c)) { colorSeted = true; }
//	//	else if (colorTool->GetInstanceColor(shape, XCAFDoc_ColorType::XCAFDoc_ColorSurf, c)) { colorSeted = true; }
//	//	else if (colorTool->GetInstanceColor(shape, XCAFDoc_ColorType::XCAFDoc_ColorCurv, c)) { colorSeted = true; }
//	//	if (!colorSeted) {
//	//		if (colorTool->GetColor(label, XCAFDoc_ColorType::XCAFDoc_ColorGen, c)) { colorSeted = true; }
//	//		else if (colorTool->GetColor(label, XCAFDoc_ColorType::XCAFDoc_ColorSurf, c)) { colorSeted = true; }
//	//		else if (colorTool->GetColor(label, XCAFDoc_ColorType::XCAFDoc_ColorCurv, c)) { colorSeted = true; }
//	//	}
//	//	if (!colorSeted) {
//	//		node->Color = Color::Gray();
//	//		System::Diagnostics::Debug::WriteLine("读取到的颜色为空");//todo 可以不需要
//	//	}
//	//	else {
//	//		node->Color = gcnew Color(c);
//	//	}
//	//}
//}

//XShape^ XCAFDocExchange::Shape() {
//	BRep_Builder myBuilder;
//	TopoDS_Compound cpd;
//	myBuilder.MakeCompound(cpd);
//	for (size_t i = 0; i < Shapes.size(); i++) {
//		myBuilder.Add(cpd, Shapes[i]->Shape);
//	}
//	XShape^ result = gcnew XShape();
//	return result;
//}
}
}