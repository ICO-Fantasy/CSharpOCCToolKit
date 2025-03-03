using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Linq;
using System.Threading.Tasks;

using OCCTK.OCC.gp;

using TestWPF.Robotics;

namespace TestWPF.Laser.Positioner;

public class PositionerFunctions {
	/// <summary>
	/// 将方向按角度分度
	/// </summary>
	/// <param name="originDir">原始方向</param>
	/// <param name="internalAngle">分度角度(Degree)</param>
	/// <param name="boundary">最大偏离范围（正/负）</param>
	/// <returns></returns>
	public static AngleMatrix<Knot> AngularDivision(
		Ax2 originAx2,
		FanucRobotConfig robotConfig,
		double internalAngle,
		double boundary
	) {
		AngleMatrix<Knot> result = new();

		List<(double angleX, double angleY)> diffAngles = [];

		// 计算正向角度组合
		double angleXPlus = 0;
		while( angleXPlus <= boundary ) {
			double angleYPlus = 0;
			while( angleYPlus <= boundary ) {
				diffAngles.Add((angleXPlus, angleYPlus));
				angleYPlus += internalAngle;
			}
			angleXPlus += internalAngle;
		}
		// 计算负向角度组合
		double angleXMinus = 0;
		while( angleXMinus >= -boundary ) {
			double angleYMinus = 0;
			while( angleYMinus >= -boundary ) {
				diffAngles.Add((angleXMinus, angleYMinus));
				angleYMinus -= internalAngle;
			}
			angleXMinus -= internalAngle;
		}

		// 并行处理角度组合
		object lockObj = new object();
		Parallel.ForEach(
			diffAngles,
			angles => {
				double angleX = angles.angleX;
				double angleY = angles.angleY;

				Trsf rx = Trsf.Default;
				Trsf ry = Trsf.Default;
				rx = new(new Quat(originAx2.XAxis, angleX));
				ry = new(new Quat(originAx2.YAxis, angleY));

				Dir newDir = originAx2.ZDir.Transformed(rx).Transformed(ry);
				double angleWithOrigin = newDir.Angle(originAx2.ZDir);

				lock( lockObj ) // 确保线程安全地更新结果
				{
					result[angleX, angleY] = new(
						originAx2,
						robotConfig,
						angleX,
						angleY,
						angleWithOrigin,
						newDir
					);
				}
			}
		);

		return result;
	}

	/// <summary>
	/// 根据两个姿态之间的角度差，构建层间距离矩阵
	/// </summary>
	/// <param name="fromLayer"></param>
	/// <param name="toLayer"></param>
	public static void GetAdjacencyMatrixWithDir(
		AngleMatrix<Knot> fromLayer,
		AngleMatrix<Knot> toLayer
	) {
		foreach( var fromKnot in fromLayer ) {
			foreach( var toKnot in toLayer ) {
				//判断是否有二轴解
				//todo 二轴解算法
				bool twoAxisDone = false;
				if( !twoAxisDone ) {
					//无解则角度为无穷大
					fromKnot.AdjacencyMatrix[toKnot] = double.PositiveInfinity;
					continue;
				}
				//判断是否有六轴解
				//todo 6R逆解算法
				bool sixAxisDone = false;
				if( !sixAxisDone ) {
					//无解则角度为无穷大
					fromKnot.AdjacencyMatrix[toKnot] = double.PositiveInfinity;
					continue;
				}
				//计算最终角度
				double angleDistance = fromKnot.Direction.Angle(toKnot.Direction);
				fromKnot.AdjacencyMatrix[toKnot] = angleDistance;
			}
		}
	}

	/// <summary>
	/// 根据输入的轨迹计算最优轨迹
	/// </summary>
	/// <param name="trajectory"></param>
	/// <param name="internalAngle"></param>
	/// <param name="boundary"></param>
	/// <returns></returns>
	public static List<Knot> CalculateBestTrajectory(
		List<(Ax2 pose, FanucRobotConfig config)> trajectory,
		double internalAngle,
		double boundary
	) {
		List<Knot> result = [];
		List<AngleMatrix<Knot>> layers = [];
		//按照传入的轨迹顺序构建轨迹层
		foreach( var t in trajectory ) {
			layers.Add(AngularDivision(t.pose, t.config, internalAngle, boundary));
		}

		#region 粗的分度
		//todo 先用粗的分度进行尝试
		//List<AngleMatrix<(double, Dir)>> coarseLayers = [];
		//foreach (var layer in layers)
		//{
		//    AngleMatrix<(double, Dir)> coarseAngleMatrix = new();
		//    if (layer.Dx.Count <= 10 || layer.Dy.Count <= 10)
		//    {
		//        coarseLayers.Add(layer);
		//        continue;
		//    }
		//    for (int i = 0; i < layer.Dx.Count; i += 4)
		//    {
		//        for (int j = 0; j < layer.Dy.Count; j += 4)
		//        {
		//            coarseAngleMatrix[layer.Dx[i], layer.Dy[j]] = layer[layer.Dx[i], layer.Dy[j]];
		//        }
		//    }
		//    coarseLayers.Add(coarseAngleMatrix);
		//}
		#endregion

		//获取每个节点的距离邻接矩阵
		for( int i = 0; i < layers.Count - 1; i++ ) {
			GetAdjacencyMatrixWithDir(layers[i], layers[i + 1]);
		}
		//获取所有路径，并记录路径长度（总的角度该变量）
		OrderedDictionary odTrajectory = new();

		var bestTrajectory = GetBsetTrajectory(layers.First());

		return result;
	}

	public static List<Knot> GetBsetTrajectory( AngleMatrix<Knot> layer ) {
		List<List<Knot>> allTrajectory = new List<List<Knot>>();
		foreach( var kont in layer ) {
			allTrajectory.AddRange(GetTrajectoryDFS(kont));
		}
		List<Knot>? bsetTrajectory = null;
		double minAngle = double.PositiveInfinity;
		foreach( var tra in allTrajectory ) {
			double totalAngle = CalculateTrajectoryToltalAngularOffset(tra);
			if( totalAngle < minAngle ) {
				minAngle = totalAngle;
				bsetTrajectory = tra;
			}
		}
		if( bsetTrajectory != null ) {
			return bsetTrajectory;
		} else {
			throw new Exception("未找到可行轨迹");
		}
	}

	public static double CalculateTrajectoryToltalAngularOffset( List<Knot> trajectory ) {
		double totalAngle = 0.0;
		for( int i = 0; i < trajectory.Count - 1; i++ ) {
			totalAngle += trajectory[i].AdjacencyMatrix[trajectory[i + 1]];
		}
		return totalAngle;
	}

	/// <summary>
	/// 深度遍历找到每一条轨迹
	/// </summary>
	/// <param name="knot"></param>
	/// <returns></returns>
	private static List<List<Knot>> GetTrajectoryDFS( Knot knot ) {
		List<List<Knot>> allTrajectory = new List<List<Knot>>();

		void NextSearch( Knot currentKnot, List<Knot> currentPath ) {
			// 将当前节点加入路径
			currentPath.Add(currentKnot);

			// 如果当前节点没有邻接节点，将此路径加入结果
			if( currentKnot.AdjacencyMatrix.Count == 0 ) {
				allTrajectory.Add(new List<Knot>(currentPath));
				return;
			}
			// 遍历当前节点的邻接节点
			foreach( var knotAndAngle in currentKnot.AdjacencyMatrix ) {
				// 跳过无穷大距离的节点
				if( knotAndAngle.Value == double.PositiveInfinity ) {
					continue;
				}

				// 创建一个新的路径副本，避免修改原路径
				var newPath = new List<Knot>(currentPath);

				// 递归搜索
				NextSearch(knotAndAngle.Key, newPath);
			}
		}

		// 初始调用
		NextSearch(knot, new List<Knot>( ));

		return allTrajectory;
	}
}
