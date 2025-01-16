#include "ICO_ImageDumpOptions.h"
#include <V3d_ImageDumpOptions.hxx>
#include <Graphic3d_BufferType.hxx>

namespace OCCTK {
namespace OCC {
namespace V3d {
ImageDumpOptions::ImageDumpOptions(V3d_ImageDumpOptions options)
{
    Width = options.Width;
    Height = options.Height;
    BufferType = options.BufferType;
    StereoOptions = options.StereoOptions;
    TileSize = options.TileSize;
    ToAdjustAspect = options.ToAdjustAspect;
}
ImageDumpOptions::ImageDumpOptions(V3d_ImageDumpOptions* options)
{
    Width = options->Width;
    Height = options->Height;
    BufferType = options->BufferType;
    StereoOptions = options->StereoOptions;
    TileSize = options->TileSize;
    ToAdjustAspect = options->ToAdjustAspect;
}
V3d_ImageDumpOptions ImageDumpOptions::GetOCC()
{
    V3d_ImageDumpOptions options;
    options.Width = Width;
    options.Height = Height;
    options.BufferType = Graphic3d_BufferType(BufferType);
    options.StereoOptions = V3d_StereoDumpOptions(StereoOptions);
    options.TileSize = TileSize;
    options.ToAdjustAspect = ToAdjustAspect;
    return options;
}
}
}
}