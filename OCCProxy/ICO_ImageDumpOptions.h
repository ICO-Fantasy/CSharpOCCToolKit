#pragma once
#include <V3d_ImageDumpOptions.hxx>

namespace OCCTK {
namespace OCC {
namespace V3d {

public value struct ImageDumpOptions
{
public:
    const static ImageDumpOptions Default = ImageDumpOptions(::V3d_ImageDumpOptions());
public:
    ImageDumpOptions(V3d_ImageDumpOptions options);
    ImageDumpOptions(V3d_ImageDumpOptions* options);
    V3d_ImageDumpOptions  GetOCC();
    //! 隐式转换为 Image_VideoParams 
    static operator V3d_ImageDumpOptions  (ImageDumpOptions  d) { return d.GetOCC(); }
public:
    /// <summary>
    /// 图像宽度，用于分配图像。如果为 0，则表示图像已分配。
    /// </summary>
    property int Width;

    /// <summary>
    /// 图像高度，用于分配图像。如果为 0，则表示图像已分配。
    /// </summary>
    property int Height;

    /// <summary>
    /// 指定需要转储的缓冲区（颜色/深度）。0 (Graphic3d_BT_RGB),1(Graphic3d_BT_RGBA)。
    /// </summary>
    property int BufferType;

    /// <summary>
    /// 转储立体摄像头的选项。默认值为 V3d_SDO_MONO（中点单目投影）。
    /// </summary>
    property int StereoOptions;

    /// <summary>
    /// 限制用于平铺转储的视图维度。如果为 0，则表示根据硬件能力自动平铺。
    /// </summary>
    property int TileSize;

    /// <summary>
    /// 是否覆盖活动视图的宽高比，使用图像转储中定义的宽度/高度。默认值为 TRUE。
    /// </summary>
    property bool ToAdjustAspect;

};

}
}
}