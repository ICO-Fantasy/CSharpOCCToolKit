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


//public class RobotProperties
//{
//    private RobotData? _RobotData;
//    public RobotData? RobotData
//    {
//        get { return _RobotData; }
//        private set
//        {
//            _RobotData = value;
//            SaveJson();
//        }
//    }

//    private readonly string _jsonFilePath;
//    private FileSystemWatcher? _fileWatcher;

//    public RobotProperties(string jsonFile)
//    {
//        _jsonFilePath = jsonFile ?? throw new Exception("json file is null");

//        // 加载 JSON 文件并初始化数据
//        LoadJson();

//        // 初始化文件监控器
//        SetupFileWatcher();
//    }

//    private void LoadJson()
//    {
//        if (!File.Exists(_jsonFilePath))
//        {
//            throw new FileNotFoundException("JSON file not found.");
//        }

//        string jsonContent = File.ReadAllText(_jsonFilePath);

//        // 反序列化 JSON
//        var options = new JsonSerializerOptions { PropertyNameCaseInsensitive = true };
//        RobotData = JsonSerializer.Deserialize<RobotData>(jsonContent, options);
//        if (RobotData == null)
//        {
//            throw new Exception("null JSON data");
//        }
//        if (RobotData.DHParameters.Count != 6)
//        {
//            throw new Exception("不支持的机器人类型");
//        }
//    }

//    public void SaveJson()
//    {
//        string jsonContent = JsonSerializer.Serialize(
//            RobotData,
//            new JsonSerializerOptions { WriteIndented = true }
//        );
//        File.WriteAllText(_jsonFilePath, jsonContent);
//    }

//    private void SetupFileWatcher()
//    {
//        _fileWatcher = new FileSystemWatcher(
//            Path.GetDirectoryName(_jsonFilePath) ?? throw new Exception("")
//        )
//        {
//            Filter = Path.GetFileName(_jsonFilePath),
//            NotifyFilter = NotifyFilters.LastWrite
//        };

//        _fileWatcher.Changed += (sender, args) =>
//        {
//            // 当文件发生变化时，重新加载 JSON
//            Task.Delay(500).Wait(); // 延迟，确保文件写入完成
//            LoadJson();
//        };

//        _fileWatcher.EnableRaisingEvents = true;
//    }
//}
