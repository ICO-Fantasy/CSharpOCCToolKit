#pragma once
#include "..\OCCProxy\ICO_ShapeEnum.h"
#include "..\OCCProxy\ICO_Topo_Shape.h"
#include <stack>
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS_Iterator.hxx>


// 比较shape的类型
// 总是返回 True 如果第一个是 SHAPE
#define SAMETYPE(x,y) ((x) == (y))
#define AVOID(x,y) (((x) == TopAbs_SHAPE) ? false : (x) == (y))
#define LESSCOMPLEX(x,y) ((x) > (y))

class TopExp_Explorer;
class TopoDS_Shape;
class TopoDS_Iterator;
namespace OCCTK {
namespace OCC {
namespace Topo {
ref class TShape;
}
}
}

namespace OCCTK::Extension {

public ref class Explorer : System::Collections::Generic::IEnumerable<OCC::Topo::TShape^>
{
private:
	OCC::Topo::TShape^ _Shape;
	OCC::TopoAbs::ShapeEnum _ToFind;
	OCC::TopoAbs::ShapeEnum _ToAvoid;
	bool _CumOrientation;
	bool _CumLocation;

public:
	Explorer(OCC::Topo::TShape^ S, OCC::TopoAbs::ShapeEnum ToFind, OCC::TopoAbs::ShapeEnum ToAvoid, bool cumOrientation, bool cumLocation)
		: _Shape(S)
		, _ToFind(ToFind)
		, _ToAvoid(ToAvoid)
		, _CumOrientation(cumOrientation)
		, _CumLocation(cumLocation)
	{
	}

	/// <summary>
	/// 获取迭代器
	/// </summary>
	/// <returns></returns>
	virtual System::Collections::Generic::IEnumerator<OCC::Topo::TShape^>^ GetEnumerator() sealed = System::Collections::Generic::IEnumerable<OCC::Topo::TShape^>::GetEnumerator
	{
		return gcnew TopoDS_ShapeEnumerator(this);
	}
	/// <summary>
	/// 获取起始的迭代器
	/// </summary>
	/// <returns></returns>
	virtual System::Collections::IEnumerator^ EnumerableGetEnumerator() sealed = System::Collections::IEnumerable::GetEnumerator
	{
		return GetEnumerator();
	}
private:
/// <summary>
/// 迭代器定义
/// </summary>
	ref struct TopoDS_ShapeEnumerator : public System::Collections::Generic::IEnumerator<OCC::Topo::TShape^> {
	private:
		// 原始类型
		// 迭代器堆栈
		std::stack<::TopoDS_Iterator*>* _iteratorStack;
        // 当前的形状
		const ::TopoDS_Shape* _Shape;
        // 寻找的类型
		::TopAbs_ShapeEnum _ToFind;
        // 避免的类型
		::TopAbs_ShapeEnum _ToAvoid;
		//方向是否一致
		bool _CumOrientation;
		//位置是否一致
		bool _CumLocation;
		// 是否初始化
		bool _Initialized;

	internal:
		TopoDS_ShapeEnumerator(const Explorer^ explorer)
			: _iteratorStack(nullptr)
			, _Initialized(false)
		{
			_Shape = &explorer->_Shape->GetOCC();
			_ToFind = (::TopAbs_ShapeEnum)explorer->_ToFind;
			_ToAvoid = (::TopAbs_ShapeEnum)explorer->_ToAvoid;
			_CumOrientation = explorer->_CumOrientation;
			_CumLocation = explorer->_CumLocation;
		}

	public:
		property OCC::Topo::TShape^ Current {
			virtual OCC::Topo::TShape^ get() {
				return gcnew OCC::Topo::TShape(_iteratorStack->top()->Value());
			}
		};

		property Object^ CurrentBase
		{
			virtual Object^ get() sealed = System::Collections::IEnumerator::Current::get
			{
				return Current;
			}
		};

		virtual bool MoveNext()
		{
			if (!_Initialized)
				return _Init();

			if (_iteratorStack->empty())
				return false;

			_iteratorStack->top()->Next();

			return _FindNext();
		}

		bool _FindNext()
		{
			if (_iteratorStack->empty())
				return false;

			if (!_iteratorStack->top()->More())
			{
				delete _iteratorStack->top();
				_iteratorStack->pop();
				return MoveNext();
			}

			while (_iteratorStack->top()->More())
			{
				auto shape = _iteratorStack->top()->Value();
				auto shapeType = shape.ShapeType();
				if (shapeType == _ToFind)
					return true;

				if (LESSCOMPLEX(_ToFind, shapeType) && !AVOID(_ToAvoid, shapeType))
				{
					_iteratorStack->push(new ::TopoDS_Iterator(shape, _CumOrientation, _CumLocation));
					return _FindNext();
				}
			}

			return false;
		}

		virtual void Reset()
		{
			throw gcnew System::NotImplementedException();
		}

		virtual ~TopoDS_ShapeEnumerator()
		{
			while (!_iteratorStack->empty())
			{
				delete _iteratorStack->top();
				_iteratorStack->pop();
			}
			delete _iteratorStack;
		}

	private:
		bool _Init()
		{
			_iteratorStack = new std::stack<::TopoDS_Iterator*>();
			if (_Shape == nullptr)
				return false;
			if (_ToFind == TopAbs_SHAPE)
				return false;
			auto shapeType = _Shape->ShapeType();

			if (LESSCOMPLEX(shapeType, _ToFind))
			{
				//输入的类型没有要找的类型复杂，没有找到
				return false;
			}
			else if (SAMETYPE(shapeType, _ToFind))
			{
				// 输入的类型和要找的类型一致，没有找到
				return false;
			}

			// Init first level
			_Initialized = true;
			_iteratorStack->push(new ::TopoDS_Iterator(*_Shape, _CumOrientation, _CumLocation));
			return _FindNext();
		}
	};

};

}
