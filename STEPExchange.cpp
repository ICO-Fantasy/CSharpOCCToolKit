#pragma once
#include "STEPExchange.h"
#include <IFSelect_ReturnStatus.hxx>
#include <STEPControl_Reader.hxx>
#include "StringExchange.h"
#include <STEPControl_Writer.hxx>

namespace OCCTK {
namespace IO {

STEPExchange::STEPExchange(TShape^ theShape) {
	myShape = theShape;
}

STEPExchange::STEPExchange(String^ filePath) {
	this->ReadFile(filePath);
}

void STEPExchange::ReadFile(String^ filePath) {
	TCollection_AsciiString cPath = DataExchange::ToAsciiString(filePath);
	TopoDS_Shape InputWorkpiece;
#pragma region readStep

	//生成一个step模型类
	STEPControl_Reader reader;
	//加载一个文件并且返回一个状态枚举值
	IFSelect_ReturnStatus status = reader.ReadFile(cPath.ToCString());
	if (status == IFSelect_RetDone) {
		reader.TransferRoot(1);
		InputWorkpiece = reader.Shape(1);
	}

#pragma endregion
	myShape = gcnew TShape(InputWorkpiece);
}

TShape^ STEPExchange::Shape() {
	return myShape;
}

bool STEPExchange::SaveFile(String^ filePath) {
	TopoDS_Shape theoccShape = myShape->GetOCC();
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
