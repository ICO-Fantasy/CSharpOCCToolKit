using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace OCCFramework;

public enum Action3d
{
    // Confusions
    LeftNone = -1,

    // normal
    None,
    SingleSelect,
    MultipleSelect,
    XORSelect,
    AreaSelect,
    MultipleAreaSelect,
    XORAreaSelect,
    AreaZooming,
    DynamicRotation,
    DynamicPanning,
    Prohibition,

    // Manipulator
    Manipulator_Translation
}
