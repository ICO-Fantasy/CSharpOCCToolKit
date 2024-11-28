using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace OCCFramework;

[Flags]
public enum CanvasDisplayMode
{
    Wireframe = 0,
    Shading = 1,
    HLR = 2,
    NoHLR = 3
}
