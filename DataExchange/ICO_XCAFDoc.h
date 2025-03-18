#pragma once
#include <TDocStd_Document.hxx>
//包装C++类到托管类
#include <NCollection_Haft.h> 
namespace OCCTK {
namespace Extension {
ref class XShape;
}
}

namespace OCCTK {
namespace Extension {

public ref class XCAFDocExchanger {
public:
    XCAFDocExchanger(const Handle(TDocStd_Document)& doc);
    //static void test(System::String^ filePath);
    XShape^ Shape();
private:
    NCollection_Haft<Handle(TDocStd_Document)> myDoc;
    XShape^ myShape;
};

}
}

