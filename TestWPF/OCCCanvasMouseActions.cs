using System;
using System.Collections.Generic;
using System.IO;
using System.Reflection;
using System.Windows.Forms;

using OCCTK.OCC.AIS;
using OCCTK.OCC.V3d;

namespace OCCViewForm;

#region Enum

public enum Action3d {
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

	// myManipulator
	Manipulator_Translation
}

#endregion

#region 接口和事件

public interface IAISSelectionHandler {
	void OnAISSelection( AShape theAIS );
}

public delegate void AISSelectionMadeHandler( AShape theAIS );

public interface IMouseMoveHandler {
	void OnMouseMove( int X, int Y );
}

public delegate void MouseMovedHandler( int X, int Y );

public interface IKeyDownHandler {
	void OnKeyDown( int X, int Y );
}

public delegate void KeyDownHandler( Keys keys );

public interface IKeyUpHandler {
	void OnKeyUp( int X, int Y );
}

public delegate void KeyUpHandler( Keys keys );

public interface ICameraChangedHandler {
	void OnCameraChanged( );
}

public delegate void CameraChangedHandler( );

#endregion

public partial class OCCCanvas {
	// 定义事件
	public event AISSelectionMadeHandler? OnAISSelectionEvent;
	public event MouseMovedHandler? OnMouseMovedEvent;
	public event KeyDownHandler? OnKeyDownEvent;
	public event KeyUpHandler? OnKeyUpEvent;
	public event CameraChangedHandler? OnCameraChangedEvent;

	private const string mouseMoveConfig = "interaction.json";

	//监控配置文件
	private FileSystemWatcher? _watcher;


	/// <summary>
	/// 注册鼠标事件
	/// </summary>
	private void RegisterMouseAction( ) {
		MouseDown += new System.Windows.Forms.MouseEventHandler(OnMouseDown);
		MouseUp += new System.Windows.Forms.MouseEventHandler(OnMouseUp);
		MouseMove += new System.Windows.Forms.MouseEventHandler(OnMouseMove);
		MouseWheel += new System.Windows.Forms.MouseEventHandler(OnMouseWheel);
	}

	/// <summary>
	/// 注册键盘事件
	/// </summary>
	private void RegisterKeyBoardAction( ) {
		KeyPreview = true;
		KeyDown += new System.Windows.Forms.KeyEventHandler(OnKeyDown);
		KeyUp += new System.Windows.Forms.KeyEventHandler(OnKeyUp);
	}

	#region 字段

	/// <summary>
	/// 交互动作对应的鼠标指针
	/// </summary>
	public Dictionary<Action3d, Cursor> CURSORS =
		new()
		{
            // myManipulator
            { Action3d.None, Cursors.Default },
			{ Action3d.SingleSelect, Cursors.Hand },
			{ Action3d.MultipleSelect, Cursors.Hand },
			{ Action3d.XORSelect, Cursors.Default },
			{ Action3d.AreaSelect, Cursors.Cross },
			{ Action3d.MultipleAreaSelect, Cursors.Cross },
			{ Action3d.XORAreaSelect, Cursors.Default },
			{ Action3d.AreaZooming, Cursors.SizeAll },
			{ Action3d.DynamicRotation, Cursors.NoMove2D },
			{ Action3d.DynamicPanning, Cursors.SizeAll },
			{ Action3d.Prohibition, Cursors.No },
            // myManipulator
            { Action3d.Manipulator_Translation, Cursors.SizeAll },
		};

	public bool isMousePointScroll = true; //true表示根据鼠标点缩放，false表示根据中心点缩放

	/// <summary>
	/// 选择框
	/// </summary>
	private readonly RubberBand myBubberBand;

	private bool _DetectedAIS;
	private int mouseDownX;
	private int mouseDownY;
	private int mouseCurrentX;
	private int mouseCurrentY;
	private int mouseUpX;
	private int mouseUpY;

	#endregion

	#region 属性

	/// <summary>
	/// 重设滚轮光标计时器
	/// </summary>
	private readonly System.Windows.Forms.Timer cursorResetTimer;

	/// <summary>
	/// 当前三维交互动作
	/// </summary>
	private Action3d _currentAction3d;

	/// <summary>
	/// 当前交互动作
	/// </summary>
	public Action3d CurrentMovingAction3d {
		get => _currentAction3d;
		set {
			//左键需要额外的逻辑
			if( value == Action3d.LeftNone ) {
				if( _DetectedAIS ) {
					// 操作器执行移动
					if( myManipulator.HasActiveMode( ) ) {
						_manipulatorMode ??= myManipulator.ActiveMode( );
						_manipulatorAxis ??= myManipulator.ActiveAxisIndex( );
						//维持运动模式不变
						if(
							myManipulator.ActiveMode( ) == _manipulatorMode
							&& myManipulator.ActiveAxisIndex( ) == _manipulatorAxis
						) {
							//移动
							_currentAction3d = Action3d.Manipulator_Translation;
						}
					}
					//不执行动作
					else if( AISContext.IsSelected( ) ) {
						_currentAction3d = Action3d.Prohibition;
					}
					_currentAction3d = Action3d.AreaSelect;
				} else {
					_currentAction3d = Action3d.AreaSelect;
				}
			} else {
				_currentAction3d = value;
			}
			//设置鼠标样式
			if( CURSORS.TryGetValue(_currentAction3d, out Cursor? cursor) ) {
				Cursor = cursor;
			}

			// 使用反射调用对应的方法
			string methodName = _currentAction3d.ToString();
			MethodInfo? methodInfo = GetType()
				.GetMethod(methodName, BindingFlags.NonPublic | BindingFlags.Instance);
			if( methodInfo != null ) {
				methodInfo.Invoke(this, null); // 调用方法
			} else {
				// 尝试获取对应的属性
				PropertyInfo? propertyInfo = GetType()
					.GetProperty(methodName, BindingFlags.NonPublic | BindingFlags.Instance);
				if( propertyInfo == null ) {
					return;
				}
				if( propertyInfo.GetValue(this) is not Action action ) {
					return;
				}
				// 调用委托
				action.Invoke( );
			}
		}
	}

	#endregion

	/// <summary>
	/// 计时器触发后将光标恢复为默认光标
	/// </summary>
	/// <param name="sender"></param>
	/// <param name="e"></param>
	private void CursorResetTimer_Tick( object? sender, EventArgs e ) {
		Cursor = System.Windows.Forms.Cursors.Default;
		// 停止计时器
		cursorResetTimer.Stop( );
	}

	#region 重写虚方法

	#region 鼠标

	private void OnMouseDown( object? sender, MouseEventArgs e ) {
		//! 键盘响应一定要获得焦点
		this.Focus( );
		// 获取鼠标按下的按钮和修改键
		//mouseButtons mouseButton = e.Button;
		//Keys modifiers = ModifierKeys;
		// 记录鼠标按下的位置
		mouseDownX = e.X;
		mouseDownY = e.Y;
		//默认为点击事件
		CurrentMovingAction3d = Action3d.SingleSelect;
		//AISContext.InitSelected( );
		//! 记录旋转开始点
		View?.StartRotation(mouseDownX, mouseDownY);
		//! 记录平移开始点
		View?.StartPan( );
		//! 记录变换开始点
		if( myManipulator.HasActiveMode( ) ) {
			log.Debug($"OnMouseMove ActiveMode: {myManipulator.ActiveMode( )}");
			myManipulator.StartTransform(mouseDownX, mouseDownY, View);
		}
	}

	private void OnMouseMove( object? sender, MouseEventArgs e ) {
		// 获取鼠标按下的按钮和修改键
		MouseButtons mouseButtons = MouseButtons;
		Keys modifiers = ModifierKeys;
		mouseCurrentX = e.X;
		mouseCurrentY = e.Y;
		//log.Debug($"Before ActiveMode: {myManipulator.ActiveMode( )}");
		//AISContext.InitDetected( );
		//触发事件（OCC的Y值零点与WPF相反）
		OnMouseMovedEvent?.Invoke(e.X, Height - e.Y);
		if( mouseButtons == MouseButtons.Left ) {
			log.Debug($"OnMouseMove ActiveMode: {myManipulator.ActiveMode( )}");
			if( myManipulator.HasActiveMode( ) ) {
				myManipulator.Transform(mouseCurrentX, mouseCurrentY, View);
				CurrentMovingAction3d = Action3d.Manipulator_Translation;
			}
			if( AISContext.MoreDetected( ) ) {
				DrawRectangle( );
				CurrentMovingAction3d = Action3d.AreaSelect;
			}
		} else if( mouseButtons == MouseButtons.Right ) {

			if( AISContext.MoreDetected( ) ) {
				DrawRectangle( );
				CurrentMovingAction3d = Action3d.AreaZooming;
			}
		} else if( mouseButtons == MouseButtons.Middle ) {
			if( modifiers == Keys.None ) {
				View.Rotation(mouseCurrentX, mouseCurrentY);
				CurrentMovingAction3d = Action3d.DynamicRotation;
			}
			if( modifiers == Keys.Control ) {
				View.Pan(mouseCurrentX - mouseDownX, -( mouseCurrentY - mouseDownY ));
				CurrentMovingAction3d = Action3d.DynamicPanning;
			}
		}

		//将鼠标位置发送给OCC交互上下文管理器，用于获取该位置的所选对象
		//! 单选等操作均需要基于该位置进行
		//! moveto的坐标是基于pix的，不需要做转换
		AISContext.MoveTo(mouseCurrentX, mouseCurrentY, View);
		View.Redraw( );
	}

	private void OnMouseUp( object? sender, MouseEventArgs e ) {
		// 获取鼠标按下的按钮和修改键
		MouseButtons mouseButtons = MouseButtons;
		Keys modifiers = ModifierKeys;
		mouseUpX = e.X;
		mouseUpY = e.Y;
		bool HasCheckbox( ) {
			return Math.Abs(mouseUpX - mouseDownX) > ZEROWIDTHMIN
				&& Math.Abs(mouseUpY - mouseDownY) > ZEROWIDTHMIN;
		}
		//switch在多分支情况下性能更好
		switch( CurrentMovingAction3d ) {
			case Action3d.SingleSelect:
				// 单选
				AISContext.Select( );
				if( AISContext.IsSelected( ) ) {
					// 触发事件，调用所有订阅的方法
					OnAISSelectionEvent?.Invoke(AISContext.SelectedAIS( ));
				}
				break;
			case Action3d.MultipleSelect:
				AISContext.XORSelect( );
				break;
			case Action3d.AreaSelect:
				if( HasCheckbox( ) ) {
					// 移动了,结束单次框选
					//! 框选是基于pix，不需要做处理
					AISContext.AreaSelect(
						Math.Min(mouseDownX, mouseUpX),
						Math.Min(mouseDownY, mouseUpY),
						Math.Max(mouseDownX, mouseUpX),
						Math.Max(mouseDownY, mouseUpY),
						View
					);
				} else {
					// 单选
					AISContext.Select( );
				}
				break;
			case Action3d.MultipleAreaSelect:
				if( HasCheckbox( ) ) {
					// 移动了 结束连续框选
					AISContext.MultipleAreaSelect(
						Math.Min(mouseDownX, mouseUpX),
						Math.Min(mouseDownY, mouseUpY),
						Math.Max(mouseDownX, mouseUpX),
						Math.Max(mouseDownY, mouseUpY),
						View
					);
				} else {
					//连续选择，只添加
					AISContext.XORSelect( );
				}
				break;
			case Action3d.AreaZooming:
				// 结束区域缩放
				AISContext.Erase(myBubberBand, true);
				if( HasCheckbox( ) ) {
					View?.WindowFitAll(mouseDownX, mouseDownY, mouseCurrentX, mouseCurrentY);
				}
				OnCameraChangedEvent?.Invoke( );
				break;
			case Action3d.DynamicRotation:
				OnCameraChangedEvent?.Invoke( );
				break;
			case Action3d.DynamicPanning:
				OnCameraChangedEvent?.Invoke( );
				break;
			case Action3d.Manipulator_Translation:
				myManipulator.StopTransform( );
				break;
			case Action3d.LeftNone:
				break;
			case Action3d.None:
				break;
			case Action3d.XORSelect:
				//todo
				break;
			case Action3d.XORAreaSelect:
				//todo
				break;
			case Action3d.Prohibition:
				break;
			default:
				break;
		}

		// 擦除选择框
		if( AISContext.IsDisplayed(myBubberBand) ) {
			AISContext.Erase(myBubberBand, true);
		}
		_currentAction3d = Action3d.None;
		_manipulatorMode = null;
		_manipulatorAxis = null;

		// 恢复光标
		Cursor = Cursors.Default;
		View.Redraw( );
	}

	private void OnMouseWheel( object? sender, MouseEventArgs e ) {
		Keys modifiers = ModifierKeys;
		// 处理鼠标滚轮事件
		int delta = e.Delta; // 获取鼠标滚轮滚动的增量
		int zoomDistance = 10;
		mouseCurrentX = e.X;
		mouseCurrentY = e.Y;
		int endX = e.X;
		int endY = e.Y;

		double zoomFactor;
		// 根据需要执行相应操作

		if( delta > 0 ) {
			// 向上滚动
			Cursor = Cursors.PanNorth;
			zoomFactor = 2.0;
			endX += zoomDistance;
			endY += zoomDistance;
			if( modifiers == Keys.Shift ) {
				// Shift键被按下时进行不同的缩放操作
				zoomFactor = 1.1;
				endX += (int)( zoomDistance * 0.1 );
				endY += (int)( zoomDistance * 0.1 );
			}
		} else {
			// 向下滚动
			Cursor = Cursors.PanSouth;
			zoomFactor = 0.5;
			endX -= zoomDistance;
			endY -= zoomDistance;
			if( modifiers == Keys.Shift ) {
				// Shift键被按下时进行不同的缩放操作
				zoomFactor = 0.9;
				endX -= (int)( zoomDistance * 0.1 );
				endY -= (int)( zoomDistance * 0.1 );
			}
		}
		if( isMousePointScroll ) {
			View?.StartZoomAtPoint(
				devicePixelRatioX * mouseCurrentX,
				devicePixelRatioY * mouseCurrentY
			);
			View?.ZoomAtPoint(mouseCurrentX, mouseCurrentY, endX, endY);
			OnCameraChangedEvent?.Invoke( );
		} else {
			View?.SetZoom(zoomFactor, true);
			OnCameraChangedEvent?.Invoke( );
		}
		// 启动计时器
		cursorResetTimer.Start( );
	}

	#endregion

	/// <summary>
	/// 键盘事件
	/// </summary>
	/// <param name="sender"></param>
	/// <param name="e"></param>
	private void OnKeyDown( object? sender, KeyEventArgs e ) {
		OnKeyDownEvent?.Invoke(e.KeyCode);
		if( e.KeyCode == Keys.F ) {
			FitAll( );
			OnCameraChangedEvent?.Invoke( );
		}
	}

	private void OnKeyUp( object? sender, KeyEventArgs e ) {
		OnKeyUpEvent?.Invoke(e.KeyCode);
	}

	#endregion

	#region 对象交互

	#region 选择框

	/// <summary>
	/// 绘制选择框
	/// </summary>
	/// <param name="draw"></param>
	private void DrawRectangle( ) {
		myBubberBand.SetRectangle(
			Math.Min(mouseDownX, mouseCurrentX),
			Math.Min(Height - mouseDownY, Height - mouseCurrentY),
			Math.Max(mouseDownX, mouseCurrentX),
			Math.Max(Height - mouseDownY, Height - mouseCurrentY)
		);
		if( AISContext.IsDisplayed(myBubberBand) ) {
			AISContext.Redisplay(myBubberBand, true);
		} else {
			//! 不能即时刷新，否则上一次设置的框会闪动
			AISContext.Display(myBubberBand, false);
		}
	}

	#endregion

	#region 操作器

	private Manipulator myManipulator;

	private ManipulatorMode? _manipulatorMode;
	private ManipulatorAxisIndex? _manipulatorAxis;

	public void Attach( InteractiveObject ais ) {
		myManipulator.Attach(ais);
	}
	public void Detach( ) {
		myManipulator.Detach( );
	}

	#endregion

	#endregion
}
