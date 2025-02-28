using System;
using System.Collections.Generic;

using OCCTK.OCC.gp;

namespace TestWPF.Utils;

public class WorkSpaceTree { }

public enum DOF {
	None = -1,
	X,
	Y,
	Z,
	W,
	P,
	R
}

public enum WorkSpaceType {
	Robot,

	/// <summary>
	/// 外部设备
	/// </summary>
	ExternalMachine,

	/// <summary>
	/// 变位机
	/// </summary>
	Positioner,

	/// <summary>
	/// 导轨
	/// </summary>
	Slider
}

public class Node {
	/// <summary>
	/// 节点名称
	/// </summary>
	public string Name { get; set; }

	/// <summary>
	/// 节点类型
	/// </summary>
	public WorkSpaceType Type { get; set; }

	public Node Parent { get; set; }
	public List<Node> Children { get; set; }
	//public Trsf Transfrom {
	//	get {
	//		return Parent.Transfrom.Multiplied(offset).Multiplied(DOFTransfrom).Multiplied(pose);
	//	}
	//}

	#region 偏置
	/// <summary>
	/// 节点原点偏置
	/// </summary>
	public Trsf offset = new();
	#endregion

	#region 位姿
	/// <summary>
	/// 节点位姿
	/// </summary>
	private Trsf pose = new();
	#endregion

	#region 运动
	/// <summary>
	/// 初始值
	/// </summary>
	public double InitialValue { get; set; }

	/// <summary>
	/// 当前值（用于计算动画）
	/// </summary>
	public double CurrentValue => InitialValue + DeltaValue;

	private double deltaValue = 0.0;
	public double DeltaValue {
		get { return deltaValue; }
		set {
			if( InitialValue + value < Max || InitialValue + value > Min ) {
				throw new ArgumentException("设置值超过范围");
			}
			deltaValue = value;
		}
	}

	/// <summary>
	/// 最大行程
	/// </summary>
	public double Max { get; set; }

	/// <summary>
	/// 最小行程
	/// </summary>
	public double Min { get; set; }

	/// <summary>
	/// 自由度
	/// </summary>
	public DOF DOF { get; set; }
	//public Trsf DOFTransfrom {
	//	get {
	//		Trsf T = new();
	//		if( DOF == DOF.None ) { }
	//		if( DOF == DOF.X ) {
	//			T.SetTranslation(new(CurrentValue, 0, 0));
	//		}
	//		if( DOF == DOF.Y ) {
	//			T.SetTranslation(new(0, CurrentValue, 0));
	//		}
	//		if( DOF == DOF.Z ) {
	//			T.SetTranslation(new(0, 0, CurrentValue));
	//		}
	//		if( DOF == DOF.W ) {
	//			T.SetRotation(new(new Pnt( ), new Dir(1, 0, 0)), CurrentValue);
	//		}
	//		if( DOF == DOF.W ) {
	//			T.SetRotation(new(new Pnt( ), new Dir(0, 1, 0)), CurrentValue);
	//		}
	//		if( DOF == DOF.W ) {
	//			T.SetRotation(new(new Pnt( ), new Dir(0, 0, 1)), CurrentValue);
	//		}
	//		return T;
	//	}
	//}

	#endregion
}
