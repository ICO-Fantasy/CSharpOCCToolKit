#include "ICO_VideoRecorder.h"
//local
#include "ICO_VideoParams.h"
#include "ICO_PixMap.h"
#include "..\DataExchange\ICO_StringExchange.h"

using namespace OCCTK::DataExchange;
namespace OCCTK {
namespace OCC {
namespace Image {

bool VideoRecorder::Open(System::String^ fileName, VideoParams^ params)
{
    return myRecorder()->Open(ToCString(fileName), params->GetRef());
}

PixMap^ VideoRecorder::ChangeFrame()
{
    return gcnew PixMap(myRecorder()->ChangeFrame());
}

bool VideoRecorder::PushFrame()
{
    return myRecorder()->PushFrame();
}

void VideoRecorder::Close()
{
    myRecorder()->Close();
}

int VideoRecorder::FrameCount()
{
    return myRecorder()->FrameCount();
}

}
}
}