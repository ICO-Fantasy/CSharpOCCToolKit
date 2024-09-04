using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using CommunityToolkit.Mvvm.ComponentModel;

namespace OCCFramework;

public class OCCCanvasViewModel : ObservableObject
{
    private OCCCanvasModel _model;

    public OCCCanvasViewModel()
    {
        _model = new OCCCanvasModel();
        // 初始化和绑定逻辑
    }

    // 绑定到View的属性和命令
}
