#pragma once
#include <Image_VideoRecorder.hxx>

namespace OCCTK {
namespace OCC {
namespace Image {

/// <summary>
/// ������Ƶ����
/// </summary>
public ref class VideoParams
{
public:
    VideoParams();
    VideoParams(Image_VideoParams  options);
    VideoParams(Image_VideoParams* options);
public:
    Image_VideoParams& GetRef();
    //! ��ʽת��Ϊ Image_VideoParams 
    static operator Image_VideoParams& (VideoParams  vp) { return vp.GetRef(); }
public:
    property System::String^ Format {System::String^ get(); void set(System::String^ value); }
    property System::String^ VideoCodec {System::String^ get(); void set(System::String^ value); }
    property System::String^ PixelFormat {System::String^ get(); void set(System::String^ value); }
    property int Width {int get(); void set(int value); }
    property int Height {int get(); void set(int value); }
    /// <summary>
    /// ֡��
    /// </summary>
    property int FpsNum {int get(); void set(int value); }
    /// <summary>
    /// ʱ����
    /// </summary>
    property int FpsDen {int get(); void set(int value); }
private:
    Image_VideoParams* myVideoParams;
private:
    // ������������������й���Դ
    !VideoParams() {
        if (myVideoParams != 0) {
            delete myVideoParams;
            myVideoParams = 0;
        }
    }

    // �ս�����finalizer��������������ʱ������
    ~VideoParams() {
        // ��������������������й���Դ
        this->!VideoParams();
    }
};

}
}
}