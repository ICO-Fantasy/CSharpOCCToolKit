using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls.Primitives;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using MathNet.Numerics;
using MathNet.Spatial.Units;
using OCCTK.Extension;
using OCCTK.OCC.AIS;
using OCCTK.OCC.BRepPrimAPI;
using OCCTK.OCC.gp;
using OCCTK.OCC.Topo;
using OCCTK.Utils;
using OCCViewForm;
using TestWPF.Robotics;
using View = OCCTK.OCC.V3d.View;

namespace TestWPF.ViewModel;

public partial class RobotWindowsViewModel : ObservableObject
{
    public RobotWindowsViewModel() { }

    private static ThreeDimensionContext ThreeDContext =>
        App.Current.ContextManager.MainContext ?? throw new Exception("请先创建OCC画布上下文");

    private OCCCanvas occCanvas => ThreeDContext.ViewList[0];
    private InteractiveContext AISContext => ThreeDContext.AISContext;

    CameraOrientation currentViewStaute;
    CameraOrientation CurrentViewStaute
    {
        get => currentViewStaute;
        set
        {
            currentViewStaute = value;
            Debug.WriteLine(
                $"Scale: {value.Scale} | Eye: {value.Eye} | Prj: {value.Projection} | Aspect: {value.Aspect} | HightPoint: {value.HightPoint} | ScreenCenter: {value.ScreenCenter} | Size: {value.Size} | ViewPoint: {value.ViewPoint} "
            );
        }
    }

    [RelayCommand]
    private void FrontView()
    {
        CurrentViewStaute = occCanvas.CurrentViewStaute;
        Debug.WriteLine(
            $"旧: Scale: {occCanvas.CurrentViewStaute.Scale.Round(1)} | Eye: {occCanvas.CurrentViewStaute.Eye} | Prj: {occCanvas.CurrentViewStaute.Projection} | Aspect: {occCanvas.CurrentViewStaute.Aspect} | HightPoint: {occCanvas.CurrentViewStaute.HightPoint} | ScreenCenter: {occCanvas.CurrentViewStaute.ScreenCenter} | Size: {occCanvas.CurrentViewStaute.Size} | ViewPoint: {occCanvas.CurrentViewStaute.ViewPoint} "
        );
        occCanvas.SetViewOrientation(OCCTK.OCC.V3d.ViewOrientation.Front);
        Debug.WriteLine(
            $"新: Scale: {occCanvas.CurrentViewStaute.Scale.Round(1)} | Eye: {occCanvas.CurrentViewStaute.Eye} | Prj: {occCanvas.CurrentViewStaute.Projection} | Aspect: {occCanvas.CurrentViewStaute.Aspect} | HightPoint: {occCanvas.CurrentViewStaute.HightPoint} | ScreenCenter: {occCanvas.CurrentViewStaute.ScreenCenter} | Size: {occCanvas.CurrentViewStaute.Size} | ViewPoint: {occCanvas.CurrentViewStaute.ViewPoint} "
        );
    }

    [RelayCommand]
    private void BackView()
    {
        CurrentViewStaute = occCanvas.CurrentViewStaute;
        occCanvas.SetViewOrientation(OCCTK.OCC.V3d.ViewOrientation.Back);
    }

    [RelayCommand]
    private void TopView()
    {
        CurrentViewStaute = occCanvas.CurrentViewStaute;
        occCanvas.SetViewOrientation(OCCTK.OCC.V3d.ViewOrientation.Top);
    }

    [RelayCommand]
    private void BottomView()
    {
        CurrentViewStaute = occCanvas.CurrentViewStaute;
        occCanvas.SetViewOrientation(OCCTK.OCC.V3d.ViewOrientation.Bottom);
    }

    [RelayCommand]
    private void LeftView()
    {
        CurrentViewStaute = occCanvas.CurrentViewStaute;
        occCanvas.SetViewOrientation(OCCTK.OCC.V3d.ViewOrientation.Left);
    }

    [RelayCommand]
    private void RightView()
    {
        CurrentViewStaute = occCanvas.CurrentViewStaute;
        Debug.WriteLine(
            $"旧: Scale: {occCanvas.CurrentViewStaute.Scale.Round(1)} | Eye: {occCanvas.CurrentViewStaute.Eye} | Prj: {occCanvas.CurrentViewStaute.Projection} | Aspect: {occCanvas.CurrentViewStaute.Aspect} | HightPoint: {occCanvas.CurrentViewStaute.HightPoint} | ScreenCenter: {occCanvas.CurrentViewStaute.ScreenCenter} | Size: {occCanvas.CurrentViewStaute.Size} | ViewPoint: {occCanvas.CurrentViewStaute.ViewPoint} "
        );
        occCanvas.SetViewOrientation(OCCTK.OCC.V3d.ViewOrientation.Right);
        Debug.WriteLine(
            $"新: Scale: {occCanvas.CurrentViewStaute.Scale.Round(1)} | Eye: {occCanvas.CurrentViewStaute.Eye} | Prj: {occCanvas.CurrentViewStaute.Projection} | Aspect: {occCanvas.CurrentViewStaute.Aspect} | HightPoint: {occCanvas.CurrentViewStaute.HightPoint} | ScreenCenter: {occCanvas.CurrentViewStaute.ScreenCenter} | Size: {occCanvas.CurrentViewStaute.Size} | ViewPoint: {occCanvas.CurrentViewStaute.ViewPoint} "
        );
    }

    [RelayCommand]
    private void AxoView()
    {
        CurrentViewStaute = occCanvas.CurrentViewStaute;
        Debug.WriteLine(
            $"旧: Scale: {occCanvas.CurrentViewStaute.Scale.Round(1)} | Eye: {occCanvas.CurrentViewStaute.Eye} | Prj: {occCanvas.CurrentViewStaute.Projection} | Aspect: {occCanvas.CurrentViewStaute.Aspect} | HightPoint: {occCanvas.CurrentViewStaute.HightPoint} | ScreenCenter: {occCanvas.CurrentViewStaute.ScreenCenter} | Size: {occCanvas.CurrentViewStaute.Size} | ViewPoint: {occCanvas.CurrentViewStaute.ViewPoint} "
        );
        occCanvas.SetViewOrientation(OCCTK.OCC.V3d.ViewOrientation.Axo);
        Debug.WriteLine(
            $"新: Scale: {occCanvas.CurrentViewStaute.Scale.Round(1)} | Eye: {occCanvas.CurrentViewStaute.Eye} | Prj: {occCanvas.CurrentViewStaute.Projection} | Aspect: {occCanvas.CurrentViewStaute.Aspect} | HightPoint: {occCanvas.CurrentViewStaute.HightPoint} | ScreenCenter: {occCanvas.CurrentViewStaute.ScreenCenter} | Size: {occCanvas.CurrentViewStaute.Size} | ViewPoint: {occCanvas.CurrentViewStaute.ViewPoint} "
        );
    }

    [RelayCommand]
    private void ResetView()
    {
        Debug.WriteLine(
            $"重设旧: Scale: {occCanvas.CurrentViewStaute.Scale.Round(1)} | Eye: {occCanvas.CurrentViewStaute.Eye} | Prj: {occCanvas.CurrentViewStaute.Projection} | Aspect: {occCanvas.CurrentViewStaute.Aspect} | HightPoint: {occCanvas.CurrentViewStaute.HightPoint} | ScreenCenter: {occCanvas.CurrentViewStaute.ScreenCenter} | Size: {occCanvas.CurrentViewStaute.Size} | ViewPoint: {occCanvas.CurrentViewStaute.ViewPoint} "
        );
        occCanvas.View.SetViewOrientation(CurrentViewStaute, true);
        Debug.WriteLine(
            $"重设新: Scale: {occCanvas.CurrentViewStaute.Scale.Round(1)} | Eye: {occCanvas.CurrentViewStaute.Eye} | Prj: {occCanvas.CurrentViewStaute.Projection} | Aspect: {occCanvas.CurrentViewStaute.Aspect} | HightPoint: {occCanvas.CurrentViewStaute.HightPoint} | ScreenCenter: {occCanvas.CurrentViewStaute.ScreenCenter} | Size: {occCanvas.CurrentViewStaute.Size} | ViewPoint: {occCanvas.CurrentViewStaute.ViewPoint} "
        );
    }
}
