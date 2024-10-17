using System;
using System.Collections.Generic;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using MathNet.Numerics;

namespace TestWPF.Bending;

public class Kparam
{
    private Kparam()
    {
        kTable = new();
        //使用默认的 kTable
        DataColumn Rt = new();
        Rt.DataType = typeof(double);
        Rt.ColumnName = "R/t";
        DataColumn K = new();
        K.DataType = typeof(double);
        K.ColumnName = "K";
        kTable.Columns.Add();
        foreach (var d in defaultData)
        {
            var row = kTable.NewRow();
            row["R/t"] = d.Item1;
            row["K"] = d.Item2;
            kTable.Rows.Add(row);
        }
    }

    private List<(double, double)> defaultData =
    [
        (3, 0.194),
        (0.3125, 0.199),
        (0.32, 0.201),
        (0.333, 0.206),
        (0.343, 0.209),
        (0.357, 0.213),
        (0.375, 0.219),
        (0.4, 0.226),
        (0.417, 0.23),
        (0.429, 0.233),
        (0.444, 0.237),
        (0.48, 0.2452),
        (0.5, 0.25),
        (0.533, 0.257),
        (0.555, 0.261),
        (0.571, 0.264),
        (0.6, 0.27),
        (0.6253, 0.274),
        (0.667, 0.281),
        (0.7, 0.286),
        (0.714, 0.288),
        (0.75, 0.2943),
        (0.8, 0.301),
        (0.833, 0.305),
        (0.857, 0.308),
        (0.889, 0.312),
        (1, 0.325),
        (1.111, 0.336),
        (1.143, 0.34),
        (1.2, 0.345),
        (1.25, 0.349),
        (1.333, 0.356),
        (1.4, 0.362),
        (1.429, 0.364),
        (1.5, 0.369),
        (1.6, 0.376),
        (1.667, 0.38),
        (1.714, 0.384),
        (1.778, 0.387),
        (1.875, 0.393),
        (2, 0.4),
        (2.083, 0.405),
        (2.143, 0.408),
        (2.222, 0.412),
        (2.286, 0.415),
        (2.4, 0.42),
        (2.5, 0.424),
        (2.667, 0.431),
        (2.857, 0.439),
        (3, 0.444),
        (3.125, 0.449),
        (3.2, 0.451),
        (3.333, 0.456),
        (3.429, 0.459),
        (3.5, 0.461),
        (3.571, 0.463),
        (3.75, 0.469),
        (4, 0.476),
        (4.167, 0.48),
        (4.286, 0.483),
        (4.375, 0.485),
        (4.444, 0.487),
        (4.5, 0.488),
        (4.8, 0.495),
        (5, 0.5),
    ];

    public DataTable kTable { get; set; }

    /// <summary>
    /// 获取中性层半径
    /// </summary>
    /// <param name="innerRadius"></param>
    /// <param name="thickness"></param>
    /// <returns></returns>
    double GetKRadius(double innerRadius, double thickness)
    {
        // 计算 R/t
        double rt = innerRadius / thickness;

        // 找到 R/t 列和 k 列
        var rtColumn = kTable.AsEnumerable().Select(row => Convert.ToDouble(row["R/t"])).ToArray();
        var kColumn = kTable.AsEnumerable().Select(row => Convert.ToDouble(row["k"])).ToArray();

        // 计算 k 系数
        double k;
        if (rt <= rtColumn.Max())
        {
            if (rtColumn.Contains(rt))
            {
                k = kColumn[Array.IndexOf(rtColumn, rt)];
            }
            else
            {
                // 线性插值
                var interpolator = Interpolate.Linear(rtColumn, kColumn);
                k = interpolator.Interpolate(rt);

                // 如果插值结果为 NaN，将 k 设置为 0.5
                if (double.IsNaN(k))
                {
                    k = 0.5;
                }
            }
        }
        else
        {
            // 如果超出最大值，取最大 R/t 对应的 k 值
            int maxIndex = Array.IndexOf(rtColumn, rtColumn.Max());
            k = kColumn[maxIndex];
        }

        // 计算中性层半径
        double kRadius = innerRadius + k * thickness;
        return kRadius;
    }
}
