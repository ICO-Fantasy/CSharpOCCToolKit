#include "ICO_Splitter.h"
#include<TopTools_ListOfShape.hxx>
#include<TopoDS.hxx>
//local
#include"ICO_Topo_Shape.h"
#include"ICO_Topo_Edge.h"

using namespace OCCTK::OCC::Topo;
using namespace System::Collections::Generic;

namespace OCCTK {
namespace OCC {
namespace BRepAlgoAPI {

Splitter::Splitter() {
	mySplitter = new BRepAlgoAPI_Splitter();
}

/// <summary>
/// 设置被分割对象
/// </summary>
/// <param name="shapeList"></param>
void Splitter::SetArguments(List<TShape^>^ shapeList) {
	TopTools_ListOfShape SList;
	for each (auto shape in shapeList) {
		SList.Append(shape->GetOCC());
	}
	mySplitter->SetArguments(SList);
}

/// <summary>
/// 设置被分割对象
/// </summary>
/// <param name="shapeList"></param>
void Splitter::SetArguments(Topo::TShape^ singleShape) {
	TopTools_ListOfShape SList;
	SList.Append(singleShape->GetOCC());
	mySplitter->SetArguments(SList);
}

/// <summary>
/// 设置分割工具Shape
/// </summary>
/// <param name="shapeList"></param>
void Splitter::SetTools(List<TShape^>^ shapeList) {
	TopTools_ListOfShape SList;
	for each (auto shape in shapeList) {
		SList.Append(shape->GetOCC());
	}
	mySplitter->SetTools(SList);
}

/// <summary>
/// 设置分割工具Shape
/// </summary>
/// <param name="shapeList"></param>
void Splitter::SetTools(Topo::TShape^ singleTool) {
	TopTools_ListOfShape SList;
	SList.Append(singleTool->GetOCC());
	mySplitter->SetTools(SList);
}

void Splitter::SetFuzzyValue(double fuzz) {
	mySplitter->SetFuzzyValue(fuzz);
}

void Splitter::Build() {
	mySplitter->Build();
}

/// <summary>
/// 获取分割线
/// </summary>
/// <returns></returns>
List<TEdge^>^ Splitter::SectionEdges() {
	List<TEdge^>^ edges = gcnew List<TEdge^>();
	TopTools_ListOfShape Sedges = mySplitter->SectionEdges();
	TopTools_ListIteratorOfListOfShape it(Sedges);
	for (; it.More(); it.Next()) {
		edges->Add(gcnew TEdge(TopoDS::Edge(it.Value())));
	}
	return edges;
}

}
}
}