using System;
using System.Collections.Generic;

using CommunityToolkit.Mvvm.ComponentModel;

using OCCTK.OCC.gp;

using TestWPF.Robotics;

namespace TestWPF.Model;

[Serializable]
public class RobotAxis:DHParameter {
	RobotAxis( )
		: base(0, 0, 0, 0, 0) { }

	Trsf Transform {
		get => Kinematics.DHParaToTransfrom(this, 0.0).ToTrsf( );
	}
}

/// <summary>
/// 刚体运动学对象数据模型
/// </summary>
[ObservableObject]
public partial class RigidBodyDynamicsModel {
	public RigidBodyDynamicsModel( Trsf baseLoc ) {
		BaseLocation = baseLoc;
		JointTransfroms = new( );
	}

	/// <summary>
	/// 基准坐标
	/// </summary>
	[ObservableProperty]
	private Trsf baseLocation;

	/// <summary>
	/// 关节变换集合
	/// </summary>
	[ObservableProperty]
	private List<Trsf> jointTransfroms;

	/// <summary>
	/// 多个末端坐标
	/// </summary>
	public List<Trsf> EndLocations {
		get {
			var endLocations = new List<Trsf>();
			foreach( var joint in JointTransfroms ) {
				endLocations.Add(BaseLocation * joint);
			}
			return endLocations;
		}
	}
}
