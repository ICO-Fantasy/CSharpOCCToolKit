using System;

using OCCTK.OCC.gp;

namespace TestWPF.Geometry.Tools;

public class BasicGeometryTools {
	private BasicGeometryTools( ) { }

	#region 求交点
	/// <summary>
	/// 计算两直线交点
	/// </summary>
	/// <param name="p1"></param>
	/// <param name="p2"></param>
	/// <param name="p3"></param>
	/// <param name="p4"></param>
	/// <param name="TOL"></param>
	/// <param name="isSegment"></param>
	/// <returns></returns>
	public static Pnt? GetIntersectionPoint(
		Pnt2D p1,
		Pnt2D p2,
		Pnt2D p3,
		Pnt2D p4,
		double TOL = 1e-2,
		bool isSegment = true
	) {
		//检测四个点位于同一平面上

		if( isSegment ) { }
		double x1 = p1.X;
		double y1 = p1.Y;
		double x2 = p2.X;
		double y2 = p2.Y;
		double x3 = p3.X;
		double y3 = p3.Y;
		double x4 = p4.X;
		double y4 = p4.Y;
		if( isSegment ) {
			return _GetSegmentIntersectionPoint(x1, y1, x2, y2, x3, y3, x4, y4, TOL);
		} else {
			//todo
			throw new Exception("算法未完成");
		}
	}

	/// <summary>
	/// 计算两直线交点
	/// </summary>
	/// <param name="x1"></param>
	/// <param name="y1"></param>
	/// <param name="x2"></param>
	/// <param name="y2"></param>
	/// <param name="x3"></param>
	/// <param name="y3"></param>
	/// <param name="x4"></param>
	/// <param name="y4"></param>
	/// <param name="TOL"></param>
	/// <param name="isSegment"></param>
	/// <returns></returns>
	/// <exception cref="Exception"></exception>
	public static Pnt? GetSegmentIntersectionPoint(
		double x1,
		double y1,
		double x2,
		double y2,
		double x3,
		double y3,
		double x4,
		double y4,
		double TOL = 1e-2,
		bool isSegment = true
	) {
		if( isSegment ) {
			return _GetSegmentIntersectionPoint(x1, y1, x2, y2, x3, y3, x4, y4, TOL);
		} else {
			//todo
			throw new Exception("算法未完成");
		}
	}

	private static Pnt? _GetSegmentIntersectionPoint(
		double x1,
		double y1,
		double x2,
		double y2,
		double x3,
		double y3,
		double x4,
		double y4,
		double TOL = 1e-2
	) {
		return null;
	}
	#endregion

	/// <summary>
	/// 两直线是否共线
	/// </summary>
	/// <param name="p1"></param>
	/// <param name="p2"></param>
	/// <param name="p3"></param>
	/// <param name="p4"></param>
	/// <param name="TOL"></param>
	/// <returns></returns>
	public static bool TwoSegmentsIsCollinear( Pnt p1, Pnt p2, Pnt p3, Pnt p4, double TOL ) {
		Vec v1 = new(p1, p2);
		Vec v2 = new(p3, p4);

		if( !v1.IsParallel(v2, TOL) ) {
			return false;
		}
		if( p1.Distance(p3) > TOL ) {
			Vec v3 = v1.Normalized().Multiplied(new Vec(p1, p3).Length);
			Trsf t3 = new();
			t3 = new(v3);
			Pnt newp3 = p1.Transformed(t3);
			//尝试正向
			if( newp3.Distance(p3) > TOL ) {
				//尝试反向
				t3 = new(v3.Reversed( ));
				newp3 = p1.Transformed(t3);
				if( newp3.Distance(p3) > TOL ) {
					return false;
				}
			}
		}
		if( p1.Distance(p4) > TOL ) {
			Vec v4 = v1.Normalized().Multiplied(new Vec(p1, p4).Length);
			Trsf t4 = new(v4);
			Pnt newp4 = p1.Transformed(t4);
			//尝试正向
			if( Math.Abs(newp4.Distance(p4)) > TOL ) {
				//尝试反向
				t4 = new(v4.Reversed( ));
				newp4 = p1.Transformed(t4);
				if( Math.Abs(newp4.Distance(p4)) > TOL ) {
					return false;
				}
			}
		}
		return true;
	}

	#region 三点定圆
	public static (Pnt CircleCenter, double Radius, double Angle) ThreePointFixedCircle(
		Pnt p1,
		Pnt p2,
		Pnt p3
	) {
		// 检查点是否共线
		Vec localX = new(p1, p2); // x轴
		Vec localY = new(p3, p2); // y轴
		Vec localZ;
		try {
			localZ = localX.Crossed(localY); // z轴
		} catch( Exception ) {
			throw new ArgumentException("三点共线，无法确定圆");
		}

		Trsf localT = new(new Ax2(p2, new(localZ), new(localX)));

		// 转换到局部坐标系
		Pnt localP1 = p1.Transformed(localT);
		Pnt localP2 = p2.Transformed(localT);
		Pnt localP3 = p3.Transformed(localT);
		//Debug.WriteLine($"{localP1} | {localP2} | {localP3}");
		// 使用2D算法计算圆心
		var (center2D, radius) = CalculateCircleIn2D(localP1, localP2, localP3);

		// 将2D圆心转换回3D坐标系
		Pnt circleCenter = ConvertBackTo3D(center2D, localT);
		double angle = CalculateArcAngle(circleCenter, p1, p2, p3);
		return (circleCenter, radius, angle);
	}

	private static (Pnt2D center, double radius) CalculateCircleIn2D( Pnt p1, Pnt p2, Pnt p3 ) {
		// 计算圆心和半径（在2D中，z = 0）
		double a = p1.X * p1.X + p1.Y * p1.Y;
		double b = p2.X * p2.X + p2.Y * p2.Y;
		double c = p3.X * p3.X + p3.Y * p3.Y;

		double d = 2 * (p1.X * (p2.Y - p3.Y) + p2.X * (p3.Y - p1.Y) + p3.X * (p1.Y - p2.Y));

		double centerX = ((a * (p2.Y - p3.Y) + b * (p3.Y - p1.Y) + c * (p1.Y - p2.Y)) / d);
		double centerY = ((a * (p3.X - p2.X) + b * (p1.X - p3.X) + c * (p2.X - p1.X)) / d);

		double radius = Math.Sqrt(
			(centerX - p1.X) * (centerX - p1.X) + (centerY - p1.Y) * (centerY - p1.Y)
		);

		return (new(centerX, centerY), radius);
	}

	private static Pnt ConvertBackTo3D( Pnt2D center2D, Trsf T ) {
		// 将局部坐标系中的圆心转换回原始3D坐标系
		return new Pnt(center2D.X, center2D.Y, 0).Transformed(T.Inverted( ));
	}

	// 计算三点间的圆弧角度
	private static double CalculateArcAngle( Pnt circleCenter, Pnt p1, Pnt p2, Pnt p3 ) {
		// 将 p1, p2, p3 相对于圆心的向量
		Vec v1 = new Vec(p1, circleCenter).Normalized();
		Vec v2 = new Vec(p2, circleCenter).Normalized();
		Vec v3 = new Vec(p3, circleCenter).Normalized();

		// 计算 v1 和 v2 之间的夹角
		double angle1 = Math.Acos(v1.Dot(v2));

		// 计算 v2 和 v3 之间的夹角
		double angle2 = Math.Acos(v2.Dot(v3));

		//// 使用叉积来确定方向
		//var cross12 = v1.CrossProduct(v2);
		//var cross23 = v2.CrossProduct(v3);

		//// 如果叉积为负值，表示顺时针旋转，需要从 2π 中减去角度
		//if (cross12.Z < 0)
		//{
		//    angle1 = 2 * Math.PI - angle1;
		//}
		//if (cross23.Z < 0)
		//{
		//    angle2 = 2 * Math.PI - angle2;
		//}

		// 将两部分角度相加，确保返回的总角度在 0 到 2π 之间
		double totalAngle = angle1 + angle2;

		// 如果总角度超过 2π，则对 2π 取模
		while( totalAngle > 2 * Math.PI ) {
			totalAngle -= 2 * Math.PI;
		}
		while( totalAngle < 0 ) {
			totalAngle += 2 * Math.PI;
		}

		return totalAngle;
	}

	#endregion
}
