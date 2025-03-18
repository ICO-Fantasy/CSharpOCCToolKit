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

namespace OCCTK {
namespace Extension {

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

	virtual System::Collections::Generic::IEnumerator<OCC::Topo::TShape^>^ GetEnumerator() sealed = System::Collections::Generic::IEnumerable<OCC::Topo::TShape^>::GetEnumerator
	{
		return gcnew TopoDS_ShapeEnumerator(this);
	}

		virtual System::Collections::IEnumerator^ EnumerableGetEnumerator() sealed = System::Collections::IEnumerable::GetEnumerator
	{
		return GetEnumerator();
	}

private:
	ref struct TopoDS_ShapeEnumerator : public System::Collections::Generic::IEnumerator<OCC::Topo::TShape^> {
	private:
		// native members
		std::stack<::TopoDS_Iterator*>* _iteratorStack;
		const ::TopoDS_Shape* _Shape;
		::TopAbs_ShapeEnum _ToFind;
		::TopAbs_ShapeEnum _ToAvoid;
		bool _CumOrientation;
		bool _CumLocation;
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
				::TopoDS_Shape* _result = new ::TopoDS_Shape(_iteratorStack->top()->Value());
				return gcnew OCC::Topo::TShape(*_result);
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
			// No more elements
			if (_iteratorStack->empty())
				return false;

			if (!_iteratorStack->top()->More())
			{
				// Current iterator is empty
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
				// the first Shape is less complex, nothing to find
				return false;
			}
			else if (SAMETYPE(shapeType, _ToFind))
			{
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
}
