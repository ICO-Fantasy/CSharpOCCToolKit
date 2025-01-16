#pragma once
#include <Image_VideoRecorder.hxx>
//local
#include "ICO_BaseObject.h"

namespace OCCTK {
namespace OCC {
namespace Image {

//Ç°ÏòÉùÃ÷
ref class VideoParams;
ref class PixMap;

public ref class VideoRecorder :BaseObject {
private:
    Handle(Image_VideoRecorder) myRecorder() { return Handle(Image_VideoRecorder)::DownCast(NativeHandle); };
public:
    bool Open(System::String^ fileName, VideoParams^ params);
    PixMap^ ChangeFrame();
    bool PushFrame();
    void Close();
    int FrameCount();
};

}
}
}