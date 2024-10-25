#pragma once
#include "ICO_STEPExchange.h"
#include <IFSelect_ReturnStatus.hxx>
#include <STEPCAFControl_Reader.hxx>//不能同时使用STEPControl_Reader
#include <STEPControl_Writer.hxx>
#include <TDataStd_Name.hxx>
#include <TDF_Label.hxx>
#include <TDF_LabelSequence.hxx>
#include <TDocStd_Document.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_LayerTool.hxx>
#include <XCAFDoc_MaterialTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
//local
#include "ICO_StringExchange.h"
#include "ICO_Topo_Shape.h"
#include "ICO_AIS_Shape.h"
#include "ICO_XShape.h"
#include "ICO_Trsf.h"
#include "ICO_Color.h"

using namespace OCCTK::Extension;
using namespace OCCTK::OCC::Topo;
using namespace OCCTK::OCC::gp;
using namespace OCCTK::OCC::AIS;
using namespace System;

namespace OCCTK {
namespace IO {

/// <summary>
/// 储存单个的TShape
/// </summary>
/// <param name="theShape"></param>
STEPExchange::STEPExchange(TShape^ theShape) {
	mySingleShape = gcnew XShape();
	mySingleShape->TopoShape = theShape;
}

STEPExchange::STEPExchange(String^ filePath) {
	this->ReadFile(filePath);
}

/// <summary>
/// 读取组合体的辅助函数
/// </summary>
/// <param name="label"></param>
/// <param name=""></param>
/// <param name=""></param>
/// <param name=""></param>
/// <param name=""></param>
/// <returns></returns>
static XShapeNode^ LableToNode(
	TDF_Label label,
	const Handle(XCAFDoc_ShapeTool)& shapeTool,
	const Handle(XCAFDoc_ColorTool)& colorTool,
	const Handle(XCAFDoc_MaterialTool)& matTool,
	const Handle(XCAFDoc_LayerTool)& layerTool
) {
	XShapeNode^ node = gcnew XShapeNode();
	Handle(TDataStd_Name) name;
	label.FindAttribute(TDataStd_Name::GetID(), name);
	node->Name = DataExchange::WcharToString(name->Get().ToWideString());
	node->Transform = gcnew Trsf(shapeTool->GetLocation(label));
	if (shapeTool->IsAssembly(label)) {
		TDF_LabelSequence sequence;
		shapeTool->GetComponents(label, sequence);
		for (size_t j = 1; j < sequence.Size(); ++j) {
			XShapeNode^ childnode = LableToNode(sequence.Value(j), shapeTool, colorTool, matTool, layerTool);
			node->Children->Add(childnode);
		}
	}
	else if (shapeTool->IsShape(label)) {
		TopoDS_Shape shape = shapeTool->GetShape(label);
		node->TopoShape = gcnew TShape(shape);
		node->AISShape = gcnew AShape(shape);
		//如果使用了SHUO设置颜色
		Quantity_Color c;
		bool colorSeted = false;
		if (colorTool->GetInstanceColor(shape, XCAFDoc_ColorType::XCAFDoc_ColorGen, c)) { colorSeted = true; }
		else if (colorTool->GetInstanceColor(shape, XCAFDoc_ColorType::XCAFDoc_ColorSurf, c)) { colorSeted = true; }
		else if (colorTool->GetInstanceColor(shape, XCAFDoc_ColorType::XCAFDoc_ColorCurv, c)) { colorSeted = true; }
		if (!colorSeted) {
			if (colorTool->GetColor(label, XCAFDoc_ColorType::XCAFDoc_ColorGen, c)) { colorSeted = true; }
			else if (colorTool->GetColor(label, XCAFDoc_ColorType::XCAFDoc_ColorSurf, c)) { colorSeted = true; }
			else if (colorTool->GetColor(label, XCAFDoc_ColorType::XCAFDoc_ColorCurv, c)) { colorSeted = true; }
		}
		if (!colorSeted) {
			node->Color = Color::Gray();
			System::Diagnostics::Debug::WriteLine("读取到的颜色为空");//todo 可以不需要
		}
		else {
			node->Color = gcnew Color(c);
		}
	}
	return node;
}

void STEPExchange::ReadFile(String^ filePath) {
#pragma region 读取文件
	Handle(TDocStd_Document) doc = new TDocStd_Document("doc");
	Handle(XCAFDoc_ShapeTool) shapeTool = XCAFDoc_DocumentTool::ShapeTool(doc->Main());
	Handle(XCAFDoc_ColorTool) colorTool = XCAFDoc_DocumentTool::ColorTool(doc->Main());
	Handle(XCAFDoc_MaterialTool) matTool = XCAFDoc_DocumentTool::MaterialTool(doc->Main());
	Handle(XCAFDoc_LayerTool) layerTool = XCAFDoc_DocumentTool::LayerTool(doc->Main());

	TCollection_AsciiString cPath = DataExchange::ToAsciiString(filePath);
	STEPCAFControl_Reader reader;
	reader.SetColorMode(true);//颜色
	reader.SetLayerMode(true);//图层
	reader.SetNameMode(true);//名称
	//reader.SetMatMode(true);//材料
	//reader.SetGDTMode(true);//几何尺寸和公差
	//reader.SetPropsMode(true);//物理属性
	//reader.SetSHUOMode(true);//Shape Usage Occurrence 文件关联
	//reader.SetViewMode(true);//视图信息
	IFSelect_ReturnStatus status = reader.ReadFile(cPath.ToCString());
	if (status == IFSelect_RetDone) {
		reader.Transfer(doc);
	}
	else if (status == IFSelect_RetVoid) { throw gcnew System::Exception("读取完成，内容为空"); }
	else if (status == IFSelect_RetError) { throw gcnew System::Exception("输入有错误，读取失败"); }
	else if (status == IFSelect_RetFail) { throw gcnew System::Exception("读取完成，执行失败"); }
	else if (status == IFSelect_RetStop) { throw gcnew System::Exception("操作中断"); }
	else { throw gcnew System::Exception("未知错误"); }

#pragma endregion
#pragma region 解析文件
	TDF_LabelSequence labels;
	shapeTool->GetFreeShapes(labels);
	//+ 单个根节点
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
		//! 组合体
		myAssemblyShape = gcnew XShape();
		//根据subLabels构造节点
		for (size_t i = 1; i <= subLabels.Size(); ++i) {
			myAssemblyShape->Nodes->Add(LableToNode(subLabels.Value(i), shapeTool, colorTool, matTool, layerTool));
		}
	}
	else {
		//! 组合体
		myAssemblyShape = gcnew XShape();
		//根据labels构造节点
		for (size_t i = 1; i <= labels.Size(); ++i) {
			myAssemblyShape->Nodes->Add(LableToNode(labels.Value(i), shapeTool, colorTool, matTool, layerTool));
		}
	}

#pragma endregion
}

//void STEPExchange::ReadFile(String^ filePath) {
//	TCollection_AsciiString cPath = DataExchange::ToAsciiString(filePath);
//	//生成一个step模型类
//	STEPControl_Reader reader;
//	//加载一个文件并且返回一个状态枚举值
//	IFSelect_ReturnStatus status = reader.ReadFile(cPath.ToCString());
//
//	TopoDS_Shape InputWorkpiece;
//	if (status == IFSelect_RetDone) {
//		reader.TransferRoot(1);
//		InputWorkpiece = reader.Shape(1);
//	}
//	myShape = gcnew TShape(InputWorkpiece);
//}

XShape^ STEPExchange::SingleShape() {
	return mySingleShape;
}

Extension::XShape^ STEPExchange::AssemblyShape() {
	return myAssemblyShape;
}

bool STEPExchange::SaveFile(String^ filePath) {
	//todo 需要更改为储存XShape
	TopoDS_Shape theoccShape = mySingleShape->TopoShape->GetOCC();
	TCollection_AsciiString cPath = DataExchange::ToAsciiString(filePath);

	//初始化写入对象
	STEPControl_Writer aWriter;

	aWriter.Transfer(theoccShape, STEPControl_AsIs);
	IFSelect_ReturnStatus status = aWriter.Write(cPath.ToCString());

	if (status == IFSelect_RetDone) {
		return true;
	}
	return false;
}

}
}
