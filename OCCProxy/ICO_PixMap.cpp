#include "ICO_PixMap.h"
#include <Image_PixMap.hxx>

namespace OCCTK {
namespace OCC {
namespace Image {

void OCCTK::OCC::Image::PixMap::FlipY(PixMap^ map)
{
    Image_PixMap::FlipY(map->GetRef());
}

}
}
}