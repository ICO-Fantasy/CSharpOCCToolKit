#include "ICO_Color.h"
#include <Quantity_Color.hxx>
#include "../DataExchange/ICO_StringExchange.h"
#include "../OCCProxy/ICO_Exception.h"

using namespace OCCTK::DataExchange;

namespace OCCTK {
namespace Extension {

Color::Color(int r, int g, int b) {
    R = r;
    G = g;
    B = b;
};

Color::Color(Quantity_Color Qcolor) {
    R = (int)(Qcolor.Red() * 255.0);
    G = (int)(Qcolor.Green() * 255.0);
    B = (int)(Qcolor.Blue() * 255.0);
}

Color::Color(System::String^ hex) {
    if (hex == nullptr) {
        throw gcnew System::ArgumentNullException("输入的值为空，hex");
    }

    try {
        Quantity_Color Qcolor;
        Qcolor.ColorFromHex(ToCString(hex), Qcolor);
        R = (int)(Qcolor.Red() * 255.0);
        G = (int)(Qcolor.Green() * 255.0);
        B = (int)(Qcolor.Blue() * 255.0);
    }
    CATCH_AND_THROW_OCC_EXCEPTIONS
}

}
}