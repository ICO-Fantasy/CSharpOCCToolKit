#pragma once
#include"ICO_ShapeEnum.h"

class TopExp_Explorer;
namespace OCCTK {
namespace OCC {
namespace Topo {
ref class TShape;
}
}
}

namespace OCCTK {
namespace OCC {
namespace TopExp {
public ref class Explorer : System::Collections::Generic::IEnumerable<Topo::TShape^> {
public:
	Explorer();
	Explorer(Topo::TShape^ shape, TopoAbs::ShapeEnum toFind);
	Explorer(Topo::TShape^ shape, TopoAbs::ShapeEnum toFind, TopoAbs::ShapeEnum toAvoid);
	bool More();
	void Next();
	Topo::TShape^ Current();
private:
	TopExp_Explorer* myExplorer;
private:
	// 析构函数用于清理非托管资源
	!Explorer() {
		if (myExplorer != 0) {
			delete myExplorer;
			myExplorer = 0;
		}
	}

	// 终结器（finalizer）用于垃圾回收时的清理
	~Explorer() {
		// 调用析构函数来清理非托管资源
		this->!Explorer();
	}
	// 实现 IEnumerator 嵌套类
private:
	ref class Enumerator : System::Collections::Generic::IEnumerator<Topo::TShape^> {
	private:
		Explorer^ explorer;
		bool started;

	public:
		Enumerator(Explorer^ explorerInstance) : explorer(explorerInstance), started(false) {}

		virtual bool MoveNext() {
			if (!started) {
				started = true;
				return explorer->More();
			}
			explorer->Next();
			return explorer->More();
		}

		virtual void Reset() {
			throw gcnew System::NotSupportedException("Reset is not supported.");
		}

		property Topo::TShape^ Current {
			virtual Topo::TShape^ get() {
				return explorer->Current();
			}
		}

		property Object^ System_Collections_IEnumerator_Current {
			virtual Object^ get() = System::Collections::IEnumerator::Current::get{
				return Current;
			}
		}

		virtual ~Enumerator() {}
	};

public:
	// 实现 IEnumerable 接口的 GetEnumerator 方法
	virtual  System::Collections::Generic::IEnumerator<Topo::TShape^>^ GetEnumerator() {
		return gcnew Enumerator(this);
	}

	// 也要实现非泛型接口的 GetEnumerator 方法
	virtual System::Collections::IEnumerator^ System_Collections_IEnumerable_GetEnumerator() = System::Collections::IEnumerable::GetEnumerator{
		return GetEnumerator();
	}
};

}
}
}

