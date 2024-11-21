using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls.Primitives;
using CommunityToolkit.Mvvm.ComponentModel;
using MathNet.Spatial.Units;
using OCCTK.Extension;
using OCCTK.OCC.AIS;
using OCCTK.OCC.BRepPrimAPI;
using OCCTK.OCC.gp;
using OCCTK.OCC.Topo;
using OCCTK.Utils;
using TestWPF.Robotics;

namespace TestWPF.ViewModel;

public partial class RobotWindowsViewModel : ObservableObject
{
    public RobotWindowsViewModel() { }

    private static ThreeDimensionContext ThreeDContext =>
        App.Current.ContextManager.MainContext ?? throw new Exception("请先创建OCC画布上下文");

    private InteractiveContext AISContext => ThreeDContext.AISContext;
}
