using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Windows.Media;
using TestWPF;
using Windows.UI.ViewManagement;
using OCCTK.Laser;


namespace TestWPF
{
    internal class WTopoDS_Shape
    {
    }

    internal class test
    {
        /// <summary>
        /// 工件数模
        /// </summary>
        //private WTopoDS_Shape InputWorkpiece { get; set; }
        #region 底板参数
        /// <summary>
        /// 拾取的底面数模
        /// </summary>
        //private WTopoDS_Shape InputFace { get; set; }
        /// <summary>
        /// 底板（原点）长
        /// </summary>
        public double theX { get; set; }
        /// <summary>
        /// 底板（原点）宽
        /// </summary>
        public double theY { get; set; }
        /// <summary>
        /// 底板（原点）高度，会用于竖版
        /// </summary>
        public double theZ { get; set; }
        /// <summary>
        /// 最大支撑高
        /// </summary>
        public float OffsetZ { get; set; }
        /// <summary>
        /// 底板横向偏移
        /// </summary>
        public double BasePlateOffsetX { get; set; }
        /// <summary>
        /// 底板纵向偏移
        /// </summary>
        public double BasePlateOffsetY { get; set; }
        /// <summary>
        /// 底板板厚
        /// </summary>
        public double BasePlateThickness { get; set; }
        /// <summary>
        /// 底板长度（X）= 包围盒长（不加BasePlateOffsetX * 2 +）
        /// </summary>
        public double BasePlateLengthX { get; set; }
        /// <summary>
        /// 底板宽度（Y）= 包围盒宽（不加BasePlateOffsetY * 2 +）
        /// </summary>
        public double BasePlateLengthY { get; set; }
        #endregion
        #region 竖板参数
        /// <summary>
        /// 竖板板厚
        /// </summary>
        public double VerticalPlateThickness { get; set; }
        /// <summary>
        /// 竖板横向初始偏移
        /// </summary>
        public double VerticalPlateInitialOffsetX { get; set; }
        /// <summary>
        /// 竖板横向偏移
        /// </summary>
        public double VerticalPlateOffsetX { get; set; }
        /// <summary>
        /// 竖板纵向初始偏移
        /// </summary>
        public double VerticalPlateInitialOffsetY { get; set; }
        /// <summary>
        /// 竖板纵向偏移
        /// </summary>
        public double VerticalPlateOffsetY { get; set; }
        /// <summary>
        /// 竖板连接高
        /// </summary>
        public double VerticalPlateConnectionHeight { get; set; }
        /// <summary>
        /// 竖板避让间隙
        /// </summary>
        public double VerticalPlateClearances { get; set; }
        /// <summary>
        /// 竖板切断距离
        /// </summary>
        public double VerticalPlateCuttingDistance { get; set; }
        #endregion
        /// <summary>
        /// 底板生成，并更新参数
        /// </summary>
        public void makeBasePlate()
        {
        }

    }
}
