using System.Collections.Generic;

using CommunityToolkit.Mvvm.Messaging.Messages;

namespace TestWPF.Message
{
    class LabelMessage : ValueChangedMessage<KeyValuePair<string, string?>>
    {
        public LabelMessage(KeyValuePair<string, string?> value) : base(value)
        {

        }
    }

    ///// <summary>
    ///// 竖板横向偏移
    ///// </summary>
    //public double OffsetX { get; set; }
    ///// <summary>
    ///// 竖板纵向初始偏移
    ///// </summary>
    //public double InitialOffsetY { get; set; }
    ///// <summary>
    ///// 竖板纵向偏移
    ///// </summary>
    //public double OffsetY { get; set; }
    ///// <summary>
    ///// 竖板连接高
    ///// </summary>
    //public double ConnectionHeight { get; set; }
    ///// <summary>
    ///// 竖板避让间隙
    ///// </summary>
    //public double Clearances { get; set; }
    ///// <summary>
    ///// 竖板最小支撑长度
    ///// </summary>
    //public double MinSupportingLen { get; set; }
    ///// <summary>
    ///// 竖板切断距离
    ///// </summary>
    //public double CuttingDistance { get; set; }
}
