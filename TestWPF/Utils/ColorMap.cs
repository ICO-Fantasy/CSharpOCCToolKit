using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Color = OCCTK.Extension.Color;

namespace OCCTK.Extension;

public class ColorMap
{
    public static readonly Color Red = new(255, 0, 0);
    public static readonly Color Green = new(0, 255, 0);
    public static readonly Color Blue = new(0, 0, 255);
    public static readonly Color Gray = new(125, 125, 125);
    public static readonly Color Black = new(0, 0, 0);
    public static readonly List<Color> Colors =
    [
        new(255, 0, 0),
        new(255, 255, 0),
        new(0, 255, 0),
        new(0, 255, 255),
        new(255, 165, 0),
        new(0, 0, 255),
        new(128, 0, 128),
        new(0, 0, 0),
    ];
}
