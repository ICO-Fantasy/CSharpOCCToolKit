using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;
using CommunityToolkit.Mvvm.ComponentModel;
using MathNet.Numerics.LinearAlgebra;
using MathNet.Numerics.LinearAlgebra.Double;

namespace TestWPF;

public class DHParameter
{
    public double Alpha { get; set; }
    public double A { get; set; }
    public double Theta { get; set; }
    public double Offset { get; set; }
    public double D { get; set; }
    public double Min { get; set; }
    public double Max { get; set; }
}

public class RobotData
{
    public required string Name { get; set; }
    public required string Type { get; set; }

    public required Dictionary<int, DHParameter> DHParameters { get; set; }

    public required double[][] BaseCoordinate { get; set; }
}

public class RobotProperties
{
    private RobotData? _RobotData;
    public RobotData? RobotData
    {
        get { return _RobotData; }
        private set
        {
            _RobotData = value;
            SaveJson();
        }
    }

    private readonly string _jsonFilePath;
    private FileSystemWatcher? _fileWatcher;

    public RobotProperties(string jsonFile)
    {
        _jsonFilePath = jsonFile ?? throw new Exception("json file is null");

        // 加载 JSON 文件并初始化数据
        LoadJson();

        // 初始化文件监控器
        SetupFileWatcher();
    }

    private void LoadJson()
    {
        if (!File.Exists(_jsonFilePath))
        {
            throw new FileNotFoundException("JSON file not found.");
        }

        string jsonContent = File.ReadAllText(_jsonFilePath);

        // 反序列化 JSON
        var options = new JsonSerializerOptions { PropertyNameCaseInsensitive = true };
        RobotData = JsonSerializer.Deserialize<RobotData>(jsonContent, options);
        if (RobotData == null)
        {
            throw new Exception("null JSON data");
        }
        if (RobotData.DHParameters.Count != 6)
        {
            throw new Exception("不支持的机器人类型");
        }
    }

    public void SaveJson()
    {
        string jsonContent = JsonSerializer.Serialize(
            RobotData,
            new JsonSerializerOptions { WriteIndented = true }
        );
        File.WriteAllText(_jsonFilePath, jsonContent);
    }

    private void SetupFileWatcher()
    {
        _fileWatcher = new FileSystemWatcher(
            Path.GetDirectoryName(_jsonFilePath) ?? throw new Exception("")
        )
        {
            Filter = Path.GetFileName(_jsonFilePath),
            NotifyFilter = NotifyFilters.LastWrite
        };

        _fileWatcher.Changed += (sender, args) =>
        {
            // 当文件发生变化时，重新加载 JSON
            Task.Delay(500).Wait(); // 延迟，确保文件写入完成
            LoadJson();
        };

        _fileWatcher.EnableRaisingEvents = true;
    }
}

public class RobotTransform
{
    public static Matrix<double> DHtoSO3(double theta, DHParameter theDH)
    {
        // 使用 DH 参数计算变换矩阵
        double _theta = theta + theDH.Offset; // 计算 θ
        double alpha = theDH.Alpha; // α
        double d = theDH.D; // d
        double a = theDH.A; // a

        // 计算正弦和余弦值
        double s_t = Math.Sin(_theta);
        double c_t = Math.Cos(_theta);
        double s_a = Math.Sin(alpha);
        double c_a = Math.Cos(alpha);

        // 创建 4x4 变换矩阵
        Matrix<double> SO3 = DenseMatrix.OfArray(
            new double[,]
            {
                { c_t, (-s_t * c_a), (s_t * s_a), (a * c_t), },
                { s_t, (c_t * c_a), (-c_t * s_a), (a * s_t), },
                { 0, s_a, c_a, d, },
                { 0, 0, 0, 1 }
            }
        );

        return SO3;
    }
}
