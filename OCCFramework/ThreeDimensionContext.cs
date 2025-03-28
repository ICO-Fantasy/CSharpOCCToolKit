﻿using OCCTK.Extension;
using OCCTK.OCC.AIS;
using OCCTK.OCC.gp;
using OCCTK.OCC.V3d;

using Color = OCCTK.Extension.Color;
using SelectionMode = OCCTK.OCC.AIS.SelectionMode;

namespace OCCFramework;

/// <summary>
/// 三维显示上下文
/// </summary>
public class ThreeDimensionContext {
	public ThreeDimensionContext( InteractiveContext context ) {
		AISContext = context;
		Viewer = context.Viewer;
		SetDefault( );
	}

	public readonly InteractiveContext AISContext;
	public readonly Viewer Viewer;

	/// <summary>
	/// 视图列表
	/// </summary>
	public List<OCCCanvas> ViewList { get; set; } = [ ];

	public void SetDefault( ) {
		//设置选择模式
		CurrentSelectionMode = SelectionMode.Shape;
		//设置默认灯光
		Viewer.SetICOLight( );
		//设置交互默认值
		AISContext.SetDefault( );
		//显示视图立方
		ShowViewCube = true;
		//显示原点坐标轴
		ShowOriginTrihedron = false;
		//显示视图坐标轴
		ShowViewTrihedron = false;
	}

	#region 视图立方

	protected ViewCube? viewCube;

	public void DisplayViewCube( bool showViewCube ) {
		viewCube ??= new(3.0);
		if( showViewCube ) {
			Display(viewCube, false);
		} else {
			if( AISContext.IsDisplayed(viewCube) ) {
				Erase(viewCube, false);
			}
		}
	}

	bool _ShowViewCube;
	public bool ShowViewCube {
		get { return _ShowViewCube; }
		set {
			_ShowViewCube = value;
			DisplayViewCube(value);
		}
	}

	#endregion

	#region 视图坐标系

	protected ATrihedron? viewTrihedron;

	public void DisplayViewTrihedron( bool showViewTrihedron ) {
		viewTrihedron ??= new(100);
		viewTrihedron.SetAspect(10, 10);
		if( showViewTrihedron ) {
			Display(viewTrihedron, false);
		} else {
			if( AISContext.IsDisplayed(viewTrihedron) ) {
				Erase(viewTrihedron, false);
			}
		}
	}

	private bool _ShowViewTrihedron;
	public bool ShowViewTrihedron {
		get { return _ShowViewTrihedron; }
		set {
			_ShowViewTrihedron = value;
			DisplayViewTrihedron(value);
		}
	}

	#endregion

	#region 原点坐标系

	protected ATrihedron? originTrihedron;

	public void DisplayOriginTrihedron( bool showOriginTrihedron ) {
		originTrihedron ??= new(100);
		if( showOriginTrihedron ) {
			Display(originTrihedron, false);
			AISContext.SetSelectionMode(originTrihedron, OCCTK.OCC.AIS.SelectionMode.None);
		} else {
			Erase(originTrihedron, false);
		}
	}

	private bool _ShowOriginTrihedron;
	public bool ShowOriginTrihedron {
		get { return _ShowOriginTrihedron; }
		set {
			_ShowOriginTrihedron = value;
			DisplayOriginTrihedron(value);
		}
	}

	#endregion

	#region 显示

	public CanvasDisplayMode _currentDisplayMode;

	/// <summary>
	/// 当前显示模式
	/// </summary>
	public CanvasDisplayMode CurrentDisplayMode {
		get => _currentDisplayMode;
		set {
			if( AISContext != null ) {
				if( value == CanvasDisplayMode.Wireframe ) {
					AISContext.SetDisplayMode(DisplayMode.WireFrame);
					_currentDisplayMode = CanvasDisplayMode.Wireframe;
				}
				if( value == CanvasDisplayMode.Shading ) {
					AISContext.SetDisplayMode(DisplayMode.Shaded);
					_currentDisplayMode = CanvasDisplayMode.Shading;
				}
				if( value == CanvasDisplayMode.HLR ) {
					_currentDisplayMode |= CanvasDisplayMode.HLR;
				}
			}
		}
	}

	public OCCTK.OCC.AIS.SelectionMode _currentSelectionMode;

	/// <summary>
	/// 当前选择模式
	/// </summary>
	public OCCTK.OCC.AIS.SelectionMode CurrentSelectionMode {
		get { return _currentSelectionMode; }
		set {
			if( AISContext != null ) {
				AISContext.SetDefaultSelectionMode(value);
				_currentSelectionMode = value;
			}
		}
	}

	public void Display( InteractiveObject theAIS, bool Toupdate = true ) {
		AISContext.Display(theAIS, false);
		//默认颜色为灰色
		AISContext.SetColor(theAIS, new Color(125, 125, 125), Toupdate);
	}

	public void Display( XShape theAIS, bool Toupdate = true ) {
		Trsf t = new Trsf();
		if( theAIS.IsShape ) {
			AISContext.Display(theAIS.AISShape, false);
			if( theAIS.Color.HasValue ) {
				AISContext.SetColor(theAIS.AISShape, (Color)theAIS.Color, Toupdate);
			} else {
				//默认颜色为灰色
				AISContext.SetColor(theAIS.AISShape, new Color(125, 125, 125), Toupdate);
			}
		}
		if( theAIS.IsAssembly ) {
			t = new Trsf( );
			this.Display(theAIS, false, null);
		}
	}

	public void Display( XShape theAIS, bool Toupdate, Color? color = null ) {
		//! 颜色
		Color? currentColor = null;
		//子节点的颜色优先于父节点的颜色
		if( theAIS.Color.HasValue ) {
			currentColor = (Color)theAIS.Color;
		} else if( color != null ) {
			currentColor = (Color)color;
		}
		//! 位置
		theAIS.AISShape.SetLocalTransformation(theAIS.Location);
		//! 材质
		//! 展示
		AISContext.Display(theAIS.AISShape, false);
		AISContext.SetColor(theAIS.AISShape, currentColor ?? new Color(125, 125, 125), Toupdate);
		foreach( var child in theAIS.Children ) {
			this.Display(child, false, currentColor);
		}
	}

	public void Redisplay( InteractiveObject theAIS, bool Toupdate = true ) {
		AISContext.Redisplay(theAIS, false);
	}

	public void EraseSelected( ) {
		AISContext.EraseSelected( );
	}

	public void Erase( InteractiveObject theAIS, bool Toupdate ) {
		AISContext.Erase(theAIS, Toupdate);
	}

	public void EraseAll( bool update ) {
		AISContext.EraseAll(false);
		DisplayViewCube(false);
		DisplayOriginTrihedron(false);
		DisplayViewTrihedron(false);
		if( update ) {
			AISContext.UpdateCurrentViewer( );
		}
	}

	public void Remove( InteractiveObject theAIS, bool Toupdate ) {
		AISContext.Remove(theAIS, Toupdate);
	}

	public void RemoveAll( bool update ) {
		AISContext.RemoveAll(false);
		DisplayViewCube(false);
		DisplayOriginTrihedron(false);
		DisplayViewTrihedron(false);
		if( update ) {
			AISContext.UpdateCurrentViewer( );
		}
	}

	public void Update( ) {
		foreach( var view in ViewList ) {
			view.Update( );
		}
		AISContext.UpdateCurrentViewer( );
	}

	#endregion

	#region 对象交互

	/// <summary>
	/// 设置选择模式
	/// </summary>
	/// <param name="theMode"></param>
	public void SetDefaultSelectionMode( OCCTK.OCC.AIS.SelectionMode theMode ) {
		AISContext.SetDefaultSelectionMode(theMode);
	}

	/// <summary>
	/// 获取选中的AIS对象
	/// </summary>
	/// <returns></returns>
	public AShape GetSelectedShape( ) {
		return AISContext.SelectedAIS( );
	}

	#endregion
}
