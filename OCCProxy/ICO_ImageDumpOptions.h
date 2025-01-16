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
    //! ��ʽת��Ϊ Image_VideoParams 
    static operator V3d_ImageDumpOptions  (ImageDumpOptions  d) { return d.GetOCC(); }
public:
    /// <summary>
    /// ͼ���ȣ����ڷ���ͼ�����Ϊ 0�����ʾͼ���ѷ��䡣
    /// </summary>
    property int Width;

    /// <summary>
    /// ͼ��߶ȣ����ڷ���ͼ�����Ϊ 0�����ʾͼ���ѷ��䡣
    /// </summary>
    property int Height;

    /// <summary>
    /// ָ����Ҫת���Ļ���������ɫ/��ȣ���0 (Graphic3d_BT_RGB),1(Graphic3d_BT_RGBA)��
    /// </summary>
    property int BufferType;

    /// <summary>
    /// ת����������ͷ��ѡ�Ĭ��ֵΪ V3d_SDO_MONO���е㵥ĿͶӰ����
    /// </summary>
    property int StereoOptions;

    /// <summary>
    /// ��������ƽ��ת������ͼά�ȡ����Ϊ 0�����ʾ����Ӳ�������Զ�ƽ�̡�
    /// </summary>
    property int TileSize;

    /// <summary>
    /// �Ƿ񸲸ǻ��ͼ�Ŀ�߱ȣ�ʹ��ͼ��ת���ж���Ŀ��/�߶ȡ�Ĭ��ֵΪ TRUE��
    /// </summary>
    property bool ToAdjustAspect;

};

}
}
}