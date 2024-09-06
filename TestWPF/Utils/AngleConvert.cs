using System;
using System.Globalization;
using System.Windows.Data;

namespace TestWPF.Utils;

public class RadianToDegreeConverter : IValueConverter
{
    public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
    {
        if (value is double radians)
        {
            // 将弧度转换为角度，并保留一位小数
            double degrees = radians * (180.0 / Math.PI);
            return Math.Round(degrees, 1); // 保留一位小数
        }
        return value;
    }

    public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
    {
        if (value is double degrees)
        {
            // 将角度转换回弧度
            return degrees * (Math.PI / 180.0);
        }
        return value;
    }
}
