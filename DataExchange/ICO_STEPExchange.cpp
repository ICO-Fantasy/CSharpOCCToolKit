#pragma once
#include "ICO_STEPExchange.h"
#include <IFSelect_ReturnStatus.hxx>
#include <STEPCAFControl_Reader.hxx>//不能同时使用STEPControl_Reader
#include <STEPControl_Writer.hxx>
#include <TDocStd_Document.hxx>
#include <TopoDS_Shape.hxx>
#include <STEPControl_StepModelType.hxx>
//local
#include "ICO_StringExchange.h"
#include "..\OCCProxy\ICO_Topo_Shape.h"
#include "..\Extension\ICO_XShape.h"
#include"ICO_XCAFDoc.h"

using namespace OCCTK::Extension;
using namespace OCCTK::DataExchange;
using namespace OCCTK::OCC::Topo;
using namespace OCCTK::OCC::gp;
using namespace System;

namespace OCCTK {
namespace IO {

/// <summary>
/// 储存单个的TShape
/// </summary>
/// <param name="theShape"></param>
STEPExchange::STEPExchange(TShape^ theShape) {
	myShape->TopoShape = theShape;
}

STEPExchange::STEPExchange(String^ filePath) {
	ReadFile(filePath);
}

/// <summary>
/// 读取文件并储存结果为XShape
/// </summary>
/// <param name="filePath"></param>
void STEPExchange::ReadFile(String^ filePath) {
	Handle(TDocStd_Document) doc = new TDocStd_Document("doc");

	TCollection_AsciiString cPath = DataExchange::ToAsciiString(filePath);
	STEPCAFControl_Reader reader;
	reader.SetColorMode(true);//颜色
	reader.SetLayerMode(true);//图层
	reader.SetNameMode(true);//名称
	reader.SetMatMode(true);//材料
	//reader.SetGDTMode(true);//几何尺寸和公差
	//reader.SetPropsMode(true);//物理属性
	//reader.SetSHUOMode(true);//Shape Usage Occurrence 文件关联
	//reader.SetViewMode(true);//视图信息
	IFSelect_ReturnStatus status = reader.ReadFile(cPath.ToCString());
	if (status == IFSelect_RetDone) {
		reader.Transfer(doc);
		XCAFDocExchange^ docResolver = gcnew XCAFDocExchange(doc);
		myShape = docResolver->Shape();
	}
	else if (status == IFSelect_RetVoid) { throw gcnew System::Exception("读取完成，内容为空"); }
	else if (status == IFSelect_RetError) { throw gcnew System::Exception("输入有错误，读取失败"); }
	else if (status == IFSelect_RetFail) { throw gcnew System::Exception("读取完成，执行失败"); }
	else if (status == IFSelect_RetStop) { throw gcnew System::Exception("操作中断"); }
	else { throw gcnew System::Exception("读取失败，未知错误"); }

}

Extension::XShape^ STEPExchange::Shape() {
	return myShape;
}

//todo 需要支持组合体
/// <summary>
/// 单个的形状保存到文件
/// </summary>
/// <param name="filePath"></param>
/// <returns></returns>
bool STEPExchange::SaveSingleShape(String^ filePath) {
	TopoDS_Shape theoccShape = myShape->TopoShape->GetOCC();
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
