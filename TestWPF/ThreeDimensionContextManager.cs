using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;

using OCCTK.OCC.AIS;
using OCCTK.OCC.OpenGL;
using OCCTK.OCC.V3d;
using OCCTK.Utils;

using OCCViewForm;

namespace TestWPF;

/// <summary>
/// Viewer管理器作为全局单例，负责绘制引擎和上下文的创建和管理
/// </summary>
public class ThreeDimensionContextManager {
	public ThreeDimensionContextManager( ) {
		Contexts = [ ];
	}

	public List<ThreeDimensionContext> Contexts { get; set; }

	/// <summary>
	/// 默认的画布，列表第一项
	/// </summary>
	public ThreeDimensionContext? MainContext => Contexts.FirstOrDefault( );

	public ThreeDimensionContext CreateContext( ) {
		//创建图形引擎
		GraphicDriver graphicDriver = new();
		//创建视图对象
		Viewer viewer = new(graphicDriver);
		//创建AIS上下文管理器
		InteractiveContext anAISContext = new(viewer);
		//创建三维画布上下文对象
		ThreeDimensionContext context = new(anAISContext);
		Contexts.Add(context);
		return context;
	}
}
