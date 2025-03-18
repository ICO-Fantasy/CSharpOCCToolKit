using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

using log4net;

using OCCTK.Extension;
using OCCTK.OCC.AIS;
using OCCTK.OCC.BRep;
using OCCTK.OCC.BRepAlgoAPI;
using OCCTK.OCC.BRepBuilderAPI;
using OCCTK.OCC.Geom;
using OCCTK.OCC.GeomAPI;
using OCCTK.OCC.gp;
using OCCTK.OCC.Topo;

using TestWPF.Geometry.Tools;

namespace TestWPF.Laser.SimpleClamp;

public partial class Workpiece {
	private static TShape MoveToOrigin( TShape shape ) {
		BoundingBox Bnd = new(shape);
		return new Transform(shape, new Trsf(new Vec(Bnd.GetAABB( ).CornerMin( ), new Pnt( ))));
	}

	private double transparency = 0.8;
	public double Transparency {
		get => transparency;
		set {
			if( value < 0.0 || value > 1.0 )
				return; //应该抛出异常
			transparency = value;
		}
	}

	public void Show( InteractiveContext context, bool update ) {
		if( context.IsDisplayed(_AIS) ) {
			context.Redisplay(_AIS, false);
		}
		context.Display(_AIS, false);
		context.SetColor(_AIS, new(125, 125, 125), false);
		context.SetTransparency(_AIS, transparency, false);
		context.SetSelectionMode(_AIS, SelectionMode.None);
		if( update ) {
			context.UpdateCurrentViewer( );
		}
	}

	public void Remove( bool update ) {
		_AIS.RemoveSelf(update);
	}
}

public partial class BasePlate {
	private double transparency = 0.8;
	public double Transparency {
		get => transparency;
		set {
			if( value < 0.0 || value > 1.0 )
				return; //应该抛出异常
			transparency = value;
		}
	}

	public void Show( InteractiveContext context, bool update ) {
		if( context.IsDisplayed(AIS) ) {
			context.Redisplay(AIS, false);
		}
		context.Display(AIS, false);
		context.SetColor(AIS, new(125, 125, 125), false);
		context.SetTransparency(AIS, transparency, false);
		context.SetSelectionMode(AIS, SelectionMode.None);
		if( update ) {
			context.UpdateCurrentViewer( );
		}
	}

	public void Remove( bool update ) {
		_AIS?.RemoveSelf(update);
	}
}

public partial class PlatePose {
	public void Show( InteractiveContext context, Workpiece workpiece ) {
		_AIS ??= new(
			new MakeFace(
				new Pln(Location, Direction),
				workpiece.BndBox.XMin,
				workpiece.BndBox.XMax,
				workpiece.BndBox.YMin,
				workpiece.BndBox.YMax
			)
		);
		if( context.IsDisplayed(_AIS) ) {
			context.Redisplay(_AIS, false);
		}
		context.Display(_AIS, false);
		context.SetColor(_AIS, new(0, 255, 97), false);
		context.SetTransparency(_AIS, 0.8, false);
		context.SetSelectionMode(_AIS, SelectionMode.None);
		context.UpdateCurrentViewer( );
	}

	public void Remove( bool update ) {
		_AIS?.RemoveSelf(update);
	}

	//// 定义隐式转换运算符，将 PlatePose 隐式转换为 TShape
	//public static implicit operator AShape(PlatePose platePose)
	//{
	//    return platePose.AIS;
	//}
}

public partial class MyEdge {
	public double Length {
		get { return Math.Sqrt(Math.Pow(Start.X - End.X, 2) + Math.Pow(Start.Y - End.Y, 2)); }
	}
}

public partial class VerticalPiece {
	private static readonly ILog log = LogManager.GetLogger(typeof(App));

	public void Show( InteractiveContext context, bool update ) {
		if( context.IsDisplayed(AISPiece) ) {
			context.Redisplay(AISPiece, false);
		}
		context.Display(AISPiece, false);
		context.SetColor(AISPiece, new(0, 255, 0), false);
		if( context.IsDisplayed(_AISEdge) ) {
			context.Redisplay(_AISEdge, false);
		}
		context.Display(_AISEdge, false);
		context.SetColor(_AISEdge, new(255, 0, 0), false);
		context.SetWidth(_AISEdge, 5, false);
		if( update ) {
			context.UpdateCurrentViewer( );
		}
	}

	public void ShowEdge( InteractiveContext context, bool update ) {
		if( context.IsDisplayed(_AISEdge) ) {
			context.Redisplay(_AISEdge, false);
		}
		context.Display(_AISEdge, false);
		context.SetColor(_AISEdge, new(255, 0, 0), false);
		context.SetWidth(_AISEdge, 5, false);
		context.SetSelectionMode(_AISEdge, SelectionMode.None);
		if( update ) {
			context.UpdateCurrentViewer( );
		}
	}

	public void Remove( bool update ) {
		AISPiece.RemoveSelf(update);
		_AISEdge.RemoveSelf(update);
	}
}

public partial class VerticalPlate {
	/// <summary>
	/// 环结构
	/// </summary>
	public struct Ring {
		public Ring( ) { }

		public Pnt Start = new();
		public Pnt End = new();
		public List<MyEdge> Edges = [];

		// 定义隐式转换运算符
		public static implicit operator List<MyEdge>( Ring platePose ) {
			return platePose.Edges;
		}

		public readonly void Add( MyEdge edge ) => Edges.Add(edge);

		public readonly void Remove( MyEdge edge ) => Edges.Remove(edge);

		public readonly void RemoveAt( int i ) => Edges.RemoveAt(i);
	}

	private static readonly ILog log = LogManager.GetLogger(typeof(App));

	//横截面切出的环
	private List<Ring> rings = [];

	//原始的底部线段
	private readonly List<MyEdge> buttomEdges = [ ];

	//修剪线段的两端
	private readonly List<MyEdge> trimedEdges = [ ];

	//移除过短的线段
	private readonly List<MyEdge> filteredEdges = [ ];

	//切断过长的线段
	private readonly List<MyEdge> cuttedEdges = [ ];

	public List<Ring> DebugRings => rings;
	public List<MyEdge> DebugButtomEdges => buttomEdges;
	public List<MyEdge> DebugTrimedEdges => trimedEdges;
	public List<MyEdge> DebugFilteredEdges => filteredEdges;
	public List<MyEdge> DebugCuttedEdges => cuttedEdges;

	#region 得到下端线
	/// <summary>
	/// 从横截面得到边,并分离得到闭合的环
	/// </summary>
	public void GetRings( ) {
		#region 得到截面上所有边
		Splitter splitter = new();
		splitter.SetArguments(Workpiece);
		splitter.SetTools(Pose);
		splitter.Build( );
		List<MyEdge> allEdges = [];
		foreach( TEdge? edge in splitter.SectionEdges( ) ) {
			allEdges.Add(new MyEdge(edge, Pose));
		}
		//首先按照order排序，加快分离速度
		allEdges.Sort(( edge1, edge2 ) => edge1.Order.CompareTo(edge2.Order));

		//分离环
		/// <summary>
		/// 从所有边线中构造环
		/// </summary>
		/// <returns></returns>
		static List<Ring> SplitRing( List<MyEdge> edges, double TOL = 5e-2 ) {
			List<Ring> rings = [];
			while( edges.Count > 0 ) {
				Ring aRing = new();
				aRing.Start = edges[0].Start;
				aRing.End = edges[0].End;
				aRing.Add(edges[0]);
				edges.RemoveAt(0); // 移除已处理的边

				while( edges.Count > 0 ) // 如果还有未处理的边
				{
					bool foundRing = false; // 标记是否找到环的一部分
					for( int i = edges.Count - 1; i >= 0; --i ) {
						MyEdge edge = edges[i];

						// 找到匹配的边后处理
						if( aRing.Start.Distance(edge.Start) < TOL ) {
							//log.Debug($"分离Ring-点的距离:{aRing.Start.Distance(edge.Start)}");
							//Debug.WriteLine($"分离Ring-点的距离:{aRing.Start.Distance(edge.Start)}");
							aRing.Add(edge);
							aRing.Start = edge.End; // 更新起点
							edges.RemoveAt(i); // 移除已处理的边
							foundRing = true;
						} else if( aRing.Start.Distance(edge.End) < TOL ) {
							//log.Debug($"分离Ring-点的距离:{aRing.Start.Distance(edge.End)}");
							//Debug.WriteLine($"分离Ring-点的距离:{aRing.Start.Distance(edge.End)}");
							aRing.Add(edge);
							aRing.Start = edge.Start; // 更新起点
							edges.RemoveAt(i);
							foundRing = true;
						} else if( aRing.End.Distance(edge.Start) < TOL ) {
							//log.Debug($"分离Ring-点的距离:{aRing.End.Distance(edge.Start)}");
							//Debug.WriteLine($"分离Ring-点的距离:{aRing.End.Distance(edge.Start)}");
							aRing.Add(edge);
							aRing.End = edge.End; // 更新终点
							edges.RemoveAt(i);
							foundRing = true;
						} else if( aRing.End.Distance(edge.End) < TOL ) {
							//log.Debug($"分离Ring-点的距离:{aRing.End.Distance(edge.End)}");
							//Debug.WriteLine($"分离Ring-点的距离:{aRing.End.Distance(edge.End)}");
							aRing.Add(edge);
							aRing.End = edge.Start; // 更新终点
							edges.RemoveAt(i);
							foundRing = true;
						}

						// 如果找到匹配边，继续外层循环
						if( foundRing ) {
							i = edges.Count; // 重新遍历未处理的边
							foundRing = false; // 重置foundRing标记，寻找下一个匹配边
						}
					}

					// 如果无法找到新的匹配边，则结束环查找
					if( !foundRing ) {
						break;
					}
				}

				rings.Add(aRing); // 将找到的环加入rings列表
			}

			return rings;
		}
		rings = SplitRing(allEdges);
		#endregion
	}

	/// <summary>
	/// 从环中获取下端线
	/// </summary>
	/// <param name="TOL">拼接精度</param>
	/// <exception cref="Exception">上下端线分离失败</exception>
	public void GetBottomEdges( double TOL = 5e-2 ) {
		//并行迭代
		Parallel.ForEach(
			rings,
			originRing => {
				//理论上 Ring 不应该少于4条边
				if( originRing.Edges.Count < 4 ) {
					//todo 错误处理
					throw new Exception("环小于四条边");
				}
				//如果工件无厚度，则提取的环为线段
				if( originRing.Start.Distance(originRing.End) > 5e-2 ) {
					//todo 错误处理
					throw new Exception("环不闭合");
				}

				Ring ring = new();
				foreach( MyEdge originEdge in originRing.Edges ) {
					ring.Add(originEdge);
				}
				Ring firstWire = new();
				//从Order最大值开始（反向循环从尾端开始）
				MyEdge? startEdge = ring.Edges.MaxBy(edge => edge.Order) ?? throw new Exception("输入的环为空");
				firstWire.Add(startEdge);
				firstWire.Start = startEdge.Start;
				firstWire.End = startEdge.End;
				ring.Remove(startEdge);
				//在第一条线两端生长
				bool findAdj = false;
				for( int i = ring.Edges.Count - 1; i >= 0; --i ) {
					MyEdge edge = ring.Edges[i];
					if( firstWire.Start.Distance(edge.End) < TOL ) {
						firstWire.Start = edge.Start;
						findAdj = true;
					}
					if( firstWire.End.Distance(edge.Start) < TOL ) {
						firstWire.End = edge.End;
						findAdj = true;
					}
					if( findAdj ) {
						//添加并删除
						firstWire.Add(edge);
						ring.RemoveAt(i);
						//重新找
						i = ring.Edges.Count;
						findAdj = false;
					}
				}
				//剩余的线作为第二段
				List<MyEdge> secondWire = ring.Edges;
				//去掉与Z平行的边
				Vec Zaxis = new(0, 0, 1);
				firstWire.Edges.RemoveAll(edge =>
					new Vec(edge.Start, edge.End).IsParallel(Zaxis, Math.PI * 2 / 180)
				);
				secondWire.RemoveAll(edge =>
					new Vec(edge.Start, edge.End).IsParallel(Zaxis, Math.PI * 2 / 180)
				);
				//比较谁更低，加入buttomEdges
				if( firstWire.Edges.Count == 0 || secondWire.Count == 0 ) {
					throw new Exception("下端线分离失败");
				}
				double firstWireAvgZ = firstWire.Edges.Average(edge => edge.Start.Z);
				double secondWireAvgZ = secondWire.Average(edge => edge.Start.Z);
				if( firstWireAvgZ < secondWireAvgZ ) {
					foreach( var e in firstWire.Edges ) {
						buttomEdges.Add(e);
					}
				} else {
					foreach( var e in secondWire ) {
						buttomEdges.Add(e);
					}
				}
				//最终检查
				for( int i = ring.Edges.Count - 1; i >= 0; --i ) {
					//todo
					break;
					MyEdge edge1 = buttomEdges[i];
					bool overlap = false;
					for( int j = ring.Edges.Count - 1; j >= 0; --j ) {
						if( i == j ) {
							continue;
						}
						MyEdge edge2 = buttomEdges[j];
						if(
							(
								edge2.OrderStart - TOL < edge1.OrderStart
								&& edge1.OrderStart < edge2.OrderEnd + TOL
							)
							|| (
								edge2.OrderStart - TOL < edge1.OrderEnd
								&& edge1.OrderEnd < edge2.OrderEnd + TOL
							)
						) {
							//与其它线存在重叠
							overlap = true;
						}
						if( overlap ) {
							//todo 将存在重叠的点，投影到另一个线上，判断谁更低
							//删除高的线
							//重新开始判断
							i = ring.Edges.Count - 1;
						}
					}
				}
			}
		);
	}
	#endregion

	#region 处理线段

	//切连接槽
	private bool sloted = false;
	private readonly List<Pnt> slotPnts = [ ];

	//烙印数字
	private readonly bool branded = false;

	//有辅助板
	private readonly bool withSupport = false;

	/// <summary>
	/// 修剪线段两端
	/// </summary>
	public void TrimEdgeEnds( ) {
		List<MyEdge> originEdges = [];
		//修剪前检查，长度需要大于2*clearances
		foreach( var edge in buttomEdges ) {
			if( edge.Length < Clearances * 2 ) {
				//log.Info($"不修剪长度为:{Math.Round(edge.Length, 3)}的边，并删除");
				continue;
			}
			originEdges.Add(edge);
		}
		//修剪线段
		MyEdge TrimEdgeEnd( MyEdge edge ) {
			Pnt p1 = (Pnt)edge.Start.Clone();
			Pnt p2 = (Pnt)edge.End.Clone();
			double ratio = Clearances / edge.Length;
			if( ratio > 1e-2 ) {
				//计算投影点
				p1.X = ( p1.X + ( p2.X - p1.X ) * ratio );
				p1.Y = ( p1.Y + ( p2.Y - p1.Y ) * ratio );

				p2.X = ( p2.X + ( p1.X - p2.X ) * ratio );
				p2.Y = ( p2.Y + ( p1.Y - p2.Y ) * ratio );
				try {
					return new(BrepGeomtryTools.TrimCurve(edge, p1, p2), edge.Pose);
				} catch( Exception ) {
					//todo 暂不处理，直接返回原边
					log.Warn($"修剪失败 长度:{edge.Length}");
				}
			}
			log.Warn($"修剪失败 修剪比率:{ratio}");
			return edge;
		}

		Parallel.ForEach(
			originEdges,
			edge => {
				trimedEdges.Add(TrimEdgeEnd(edge));
			}
		);
	}

	/// <summary>
	/// 移除过短的线
	/// </summary>
	public void RemoveShortEdge( ) {
		foreach( MyEdge edge in trimedEdges ) {
			if( edge.Length < MinSupportLen ) {
				log.Info($"移除长度为:{Math.Round(edge.Length, 3)}的边");
				continue;
			}
			filteredEdges.Add(edge);
		}
	}

	/// <summary>
	/// 分割过长线段
	/// </summary>
	public void CutLongEdges( ) {
		foreach( MyEdge edge in trimedEdges ) {
			//如果两个参数值任意为 0，则不处理
			if(
				Math.Abs(MinSupportLen) < 1e-2
				|| Math.Abs(CutDistance) < 1e-2
				|| Math.Abs(MinSupportLen + CutDistance) < 1e-2
			) {
				cuttedEdges.Add(edge);
				continue;
			}
			//跳过不需要切割的边
			if( edge.Length < MinSupportLen * 2 + CutDistance ) {
				cuttedEdges.Add(edge);
				continue;
			}
			// 以 (MinSupportLen + CutDistance) 作为分割依据，调整实际的 supLen, cutLen 值
			// 如果小于它，同时增加 supLen, cutLen
			double remainder = (edge.Length - MinSupportLen) % (MinSupportLen + CutDistance);
			int quotient = (int)
				Math.Truncate((edge.Length - MinSupportLen) / (MinSupportLen + CutDistance));
			//实际的支撑长度
			double supLen =
				MinSupportLen
				+ remainder / (quotient + 1) * ((double)(quotient + 1) / (quotient * 2 + 1));
			//实际的切割长度
			double cutLen =
				CutDistance + remainder / quotient * ((double)quotient / (quotient * 2 + 1));
			void CutLongEdge( MyEdge edge, double supLen, double cutLen ) {
				// 如果最小支撑长度不足，则不做切割(理论上在前面步骤已经处理了过短边)
				if( supLen <= 1 ) {
					return;
				}

				// 获取底层曲线
				double first = 0.0;
				double last = 0.0;
				Curve aCurve = Tool.Curve(edge, ref first, ref last);

				Pnt startP = (Pnt)edge.Start.Clone();
				Pnt endP = (Pnt)edge.End.Clone();
				// 减少投影误差，取线的中点为Z值
				double zValue = BrepGeomtryTools.GetEdgeMidlePoint(edge).Z;
				startP.Z = zValue;
				endP.Z = zValue;
				double param1;
				double param2;

				double currentNum = 0.0;
				while( currentNum * ( supLen + cutLen ) < edge.Length ) {
					Trsf toStart = new();
					toStart = new(
						new Vec(startP, endP)
							.Normalized( )
							.Multiplied(currentNum * ( supLen + cutLen ))
					);
					Pnt start = startP.Transformed(toStart);

					Trsf toEnd = new();
					toEnd = new(new Vec(startP, endP).Normalized( ).Multiplied(supLen));
					Pnt end = start.Transformed(toEnd);

					//投影点到曲线上，并获取投影点处的参数
					ProjectPointOnCurve ppc1S = new(start, aCurve);
					param1 = ppc1S.LowerDistanceParameter( );
					ProjectPointOnCurve ppc2S = new(end, aCurve);
					param2 = ppc2S.LowerDistanceParameter( );

					cuttedEdges.Add(new(new MakeEdge(aCurve, param1, param2), Pose));
					currentNum += 1.0;
				}
			}
			CutLongEdge(edge, supLen, cutLen);
		}
	}

	public void CalculateSlot( List<VerticalPlate> otherPlate ) {
		if( Pieces.Count == 0 ) {
			throw new Exception("竖板片为空");
		}

		slotPnts.Add(new( ));
		sloted = true;
	}

	#endregion

	#region Display

	//直接连接
	public AShape? SuturedShape;

	//切连接槽
	public AShape? SlotedShape;

	//烙印数字
	public AShape? BrandedShape;

	//有辅助板
	public AShape? SupportPlate;

	public void ShowPlate( InteractiveContext context, bool update ) {
		this.Remove(false);
		TEdge leftWire;
		List<TEdge> topWire = [];
		TEdge rightWire;
		List<TEdge> bottomWire = [];

		#region 处理通用部分

		//左、右、上均一致
		//! 左
		VerticalPiece first = Pieces.First();
		Pnt leftBottom = first.Edge.Start.SetZ(this.BasePlate.Z);
		leftWire = new MakeEdge(leftBottom, first.Edge.Start);
		//! 右
		VerticalPiece last = Pieces.Last();
		Pnt rightBottom = last.Edge.End.SetZ(this.BasePlate.Z);
		rightWire = new MakeEdge(last.Edge.End, rightBottom);
		//! 上
		var tempPieces = Pieces.ToList();
		//处理首
		topWire.Add(first);
		//记录最后一个点，用于下一段构建
		Pnt tempPnt = first.Edge.End.SetZ(first.Edge.End.Z - AvoidanceHeight);
		topWire.Add(new MakeEdge(first.Edge.End, tempPnt));
		//去掉首尾，其它的处理
		for( var i = 1; i < tempPieces.Count - 1; i++ ) {
			topWire.Add(
				new MakeEdge(
					tempPnt,
					tempPieces[i].Edge.Start.SetZ(tempPieces[i].Edge.Start.Z - AvoidanceHeight)
				)
			);
			topWire.Add(
				new MakeEdge(
					tempPieces[i].Edge.Start.SetZ(tempPieces[i].Edge.Start.Z - AvoidanceHeight),
					tempPieces[i].Edge.Start
				)
			);
			topWire.Add(tempPieces[i].Edge);
			tempPnt = tempPieces[i].Edge.End.SetZ(tempPieces[i].Edge.End.Z - AvoidanceHeight);
			topWire.Add(new MakeEdge(tempPieces[i].Edge.End, tempPnt));
		}
		//处理尾
		topWire.Add(
			new MakeEdge(tempPnt, last.Edge.Start.SetZ(last.Edge.Start.Z - AvoidanceHeight))
		);
		topWire.Add(
			new MakeEdge(last.Edge.Start.SetZ(last.Edge.Start.Z - AvoidanceHeight), last.Edge.Start)
		);
		topWire.Add(last);

		//! 构造不封闭的线
		MakeWire maker = new();
		maker.Add(leftWire);
		foreach( var topEdge in topWire ) {
			maker.Add(topEdge);
		}
		maker.Add(rightWire);

		#endregion

		//需要开连接槽
		if( sloted ) {
			//不烙印数字
			if( !branded ) {
				return;
			} else
			  //需要烙印数字
			  {
				//todo
				return;
			}
		} else
		  //直接连接
		  {
			//直接封闭
			maker.Add(new MakeEdge(rightBottom, leftBottom));
			SuturedShape = new(new MakeFace(maker));
			//显示用于构造的线
			foreach( var piece in Pieces ) {
				piece.ShowEdge(context, false);
			}
			//显示连接后的板
			context.Display(SuturedShape, true);
			context.SetColor(SuturedShape, new(0, 255, 0), true);
		}
		//需要绘制辅助板
		if( withSupport ) {
			return;
		}
	}

	public void ShowPieces( InteractiveContext context, bool update ) { }

	public void Remove( bool update ) {
		SuturedShape?.RemoveSelf(false);
		SlotedShape?.RemoveSelf(false);
		BrandedShape?.RemoveSelf(false);
		SupportPlate?.RemoveSelf(false);
		foreach( var piece in Pieces ) {
			piece.Remove(false);
		}
		if( update ) {
			Pieces.First( ).AISPiece.GetContext( ).UpdateCurrentViewer( );
		}
	}
	#endregion
}
