#include "ICO_MakeWire.h"
#include <Standard_Failure.hxx>
#include <BRepBuilderAPI_WireError.hxx>
//Local
#include "ICO_Topo_Shape.h"
#include "ICO_Topo_Edge.h"
#include "ICO_Topo_Wire.h"
#include "ICO_Exception.h"

using namespace OCCTK::OCC::Topo;
using namespace OCCTK::OCC::gp;
using namespace System::Collections::Generic;

namespace OCCTK {
namespace OCC {
namespace BRepBuilderAPI {

MakeWire::MakeWire() {
	myMaker = new BRepBuilderAPI_MakeWire();
}

MakeWire::MakeWire(TEdge^ edge) {
	myMaker = new BRepBuilderAPI_MakeWire(edge->GetOCC());
}

MakeWire::MakeWire(List<TEdge^>^ edges) {
	myMaker = new BRepBuilderAPI_MakeWire();
	for each (TEdge ^ edge in edges) {
		myMaker->Add(edge->GetOCC());
	}
}

/// <summary>
/// 向构造器添加边，边线必须首尾相连
/// </summary>
/// <param name="edge"></param>
void MakeWire::Add(TEdge^ edge) {
	try {
		myMaker->Add(edge->GetOCC());
	}
	CATCH_AND_THROW_OCC_EXCEPTIONS
}

void MakeWire::Add(TWire^ wire) {
	try {
		myMaker->Add(wire->GetOCC());
	}
	CATCH_AND_THROW_OCC_EXCEPTIONS
}

void MakeWire::Add(List<TEdge^>^ edges) {
	try {
		for each (TEdge ^ edge in edges) {
			myMaker->Add(edge->GetOCC());
		}
	}
	CATCH_AND_THROW_OCC_EXCEPTIONS
}

TShape^ MakeWire::Shape() {
	try {
		return gcnew TShape(myMaker->Shape());
	}
	CATCH_AND_THROW_OCC_EXCEPTIONS
}

TWire^ MakeWire::Wire() {
	try {
		return gcnew TWire(myMaker->Wire());
	}
	CATCH_AND_THROW_OCC_EXCEPTIONS
}

bool MakeWire::Error() {
	BRepBuilderAPI_WireError state = myMaker->Error();
	if (state == BRepBuilderAPI_WireDone) {
		return true;
	}
	if (state == BRepBuilderAPI_EmptyWire) {
		throw gcnew System::Exception(gcnew System::String("Empty Wire"));
	}
	if (state == BRepBuilderAPI_DisconnectedWire) {
		throw gcnew System::Exception(gcnew System::String("Disconnected Wire"));
	}
	if (state == BRepBuilderAPI_NonManifoldWire) {
		throw gcnew System::Exception(gcnew System::String("Non Manifold Wire"));
		//! 线框不符合流形要求(Non Manifold Wire)的常见原因
		//边与超过两个面共享： 在线框拓扑中，每条边只能属于两个面（最多），如果某条边被超过两个面共享，就会导致“非流形”错误。这种结构在一些几何建模操作中是无效的。
		//不连续的线框： 线框中的边可能不连续，导致无法形成封闭的面或体。
		//不闭合的线框： 当尝试构建封闭的几何结构时，线框可能没有完全闭合，这导致拓扑上不完整。例如，某个线段的起点和终点没有正确连接到其他部分。
		//顶点重复使用： 某些顶点可能被多个独立的边或面共享，但没有遵守流形要求（例如，一个顶点同时被多个不相关的面连接）。
		//! 如何修复 Non Manifold Wire 错误
		//检查线框的拓扑完整性： 使用几何检查工具（如 Open CASCADE 的 ShapeFix_Wire）来验证线框是否符合拓扑要求，并自动修复一些常见的拓扑问题。
		//确保线框闭合： 检查所有边的连接情况，确保线框是一个闭合的结构，起点和终点正确连接。
		//限制边的共享： 检查是否有某些边被超过两个面共享，必要时将这些边拆分，确保每条边最多只被两个面共享。
		//使用几何修复工具： Open CASCADE 提供了一些修复工具，可以用于自动检查和修复流形问题，例如 ShapeFix_Shape, ShapeFix_Wire 等工具类，可以用来修复线框或几何形状的拓扑结构。
	}
	return false;
}


bool MakeWire::IsDone::get()
{
	return myMaker->IsDone();
}

}
}
}