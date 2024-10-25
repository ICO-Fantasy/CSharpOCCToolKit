using System;
using System.Collections.Generic;
using System.Configuration;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Color = OCCTK.Extension.Color;

namespace OCCTK.Extension;

public class ColorMap
{
    public static readonly Color Red = new(255, 0, 0);
    public static readonly Color Orange = new(255, 165, 0);
    public static readonly Color Yellow = new(225, 255, 0);
    public static readonly Color Green = new(0, 255, 0);
    public static readonly Color Cyan = new(0, 127, 255);
    public static readonly Color Blue = new(0, 0, 255);
    public static readonly Color Purple = new(139, 0, 255);
    public static readonly Color Black = new(0, 0, 0);
    public static readonly Color White = new(255, 255, 255);
    public static readonly Color Gray = new(125, 125, 125);
    public static readonly List<Color> Colors =
    [
        Red,
        Orange,
        Green,
        Cyan,
        Yellow,
        Purple,
        Blue,
        Black,
        White,
        Gray,
    ];
}
