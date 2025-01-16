#pragma once
#include <Image_PixMap.hxx>
//local
#include "ICO_BaseObject.h"

namespace OCCTK {
namespace OCC {
namespace Image {

public ref class PixMap {
public:
    PixMap(Image_PixMap& pixMap) { myPixMapIntPtr = &pixMap; }
    Image_PixMap& GetRef() {
        if (myPixMapIntPtr == nullptr) {
            throw gcnew System::InvalidOperationException("Image_PixMap is null.");
        }
        return *myPixMapIntPtr; // 解引用 Handle 并返回引用
    };
public:
    static void FlipY(PixMap^ map);
private:
    Image_PixMap* myPixMapIntPtr;
};

}
}
}