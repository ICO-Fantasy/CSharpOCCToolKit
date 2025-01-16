#include "ICO_VideoParams.h"
#include <Image_VideoRecorder.hxx>
//local
#include "../DataExchange/ICO_StringExchange.h"

using namespace OCCTK::DataExchange;

namespace OCCTK {
namespace OCC {
namespace Image {

VideoParams::VideoParams()
{
    myVideoParams = new Image_VideoParams();
}

VideoParams::VideoParams(Image_VideoParams options)
{
    myVideoParams = &options;
}

VideoParams::VideoParams(Image_VideoParams* options)
{
    myVideoParams = options;
    Width = options->Width;
    Height = options->Height;
    FpsNum = options->FpsNum;
    FpsDen = options->FpsDen;
    Format = gcnew System::String(options->Format.ToCString());
    PixelFormat = gcnew System::String(options->PixelFormat.ToCString());
    VideoCodec = gcnew System::String(options->VideoCodec.ToCString());
}

Image_VideoParams& VideoParams::GetRef()
{
    return *myVideoParams;
}

System::String^ VideoParams::Format::get()
{
    return gcnew System::String(myVideoParams->Format.ToCString());
}

void VideoParams::Format::set(System::String^ value)
{
    myVideoParams->Format = ToAsciiString(value);
}

System::String^ VideoParams::VideoCodec::get()
{
    return gcnew System::String(myVideoParams->VideoCodec.ToCString());
}

void VideoParams::VideoCodec::set(System::String^ value)
{
    myVideoParams->VideoCodec = ToAsciiString(value);
}

System::String^ VideoParams::PixelFormat::get()
{
    return gcnew System::String(myVideoParams->PixelFormat.ToCString());
}

void VideoParams::PixelFormat::set(System::String^ value)
{
    myVideoParams->PixelFormat = ToAsciiString(value);
}

int VideoParams::Width::get()
{
    return myVideoParams->Width;
}

void VideoParams::Width::set(int value)
{
    myVideoParams->Width = value;
}

int VideoParams::Height::get()
{
    return myVideoParams->Height;
}

void VideoParams::Height::set(int value)
{
    myVideoParams->Height = value;
}

int VideoParams::FpsNum::get()
{
    return myVideoParams->FpsNum;
}

void VideoParams::FpsNum::set(int value)
{
    myVideoParams->FpsNum = value;
}

int VideoParams::FpsDen::get()
{
    return myVideoParams->FpsDen;
}

void VideoParams::FpsDen::set(int value)
{
    myVideoParams->FpsDen = value;
}


}
}
}