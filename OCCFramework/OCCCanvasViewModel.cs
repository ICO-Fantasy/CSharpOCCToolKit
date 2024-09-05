using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms.Integration;
using System.Windows.Interop;
using CommunityToolkit.Mvvm.ComponentModel;
using OCCTK.OCC.AIS;
using OCCTK.OCC.V3d;
using OCCTK.Visualization;

namespace OCCFramework;

public class OCCCanvasViewModel : ObservableObject
{
    private CSharpViewer _model;
    public Viewer _viewer;
    public InteractiveContext _context;
    public List<OCCTK.OCC.V3d.View> _views;

    public OCCCanvasViewModel()
    {
        _model = new CSharpViewer();
        // 初始化和绑定逻辑
        _model.InitViewer();
        _viewer = _model.GetViewer();
        _context = _model.GetContext();
        _views = new();

        _viewer.SetDefaultLight();
        _context.SetDefault();
    }

    public OCCTK.OCC.V3d.View CreateView(IntPtr theIntPtr)
    {
        return _model.CreateView(theIntPtr);
    }

    // 绑定到View的属性和命令
}
