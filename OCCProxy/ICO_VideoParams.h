#pragma once
#include <Image_VideoRecorder.hxx>

namespace OCCTK {
namespace OCC {
namespace Image {

/// <summary>
/// 储存视频参数
/// </summary>
public ref class VideoParams
{
public:
    VideoParams();
    VideoParams(Image_VideoParams  options);
    VideoParams(Image_VideoParams* options);
public:
    Image_VideoParams& GetRef();
    //! 隐式转换为 Image_VideoParams 
    static operator Image_VideoParams& (VideoParams  vp) { return vp.GetRef(); }
public:
    property System::String^ Format {System::String^ get(); void set(System::String^ value); }
    property System::String^ VideoCodec {System::String^ get(); void set(System::String^ value); }
    property System::String^ PixelFormat {System::String^ get(); void set(System::String^ value); }
    property int Width {int get(); void set(int value); }
    property int Height {int get(); void set(int value); }
    /// <summary>
    /// 帧数
    /// </summary>
    property int FpsNum {int get(); void set(int value); }
    /// <summary>
    /// 时间间隔
    /// </summary>
    property int FpsDen {int get(); void set(int value); }
private:
    Image_VideoParams* myVideoParams;
private:
    // 析构函数用于清理非托管资源
    !VideoParams() {
        if (myVideoParams != 0) {
            delete myVideoParams;
            myVideoParams = 0;
        }
    }

    // 终结器（finalizer）用于垃圾回收时的清理
    ~VideoParams() {
        // 调用析构函数来清理非托管资源
        this->!VideoParams();
    }
};

}
}
}