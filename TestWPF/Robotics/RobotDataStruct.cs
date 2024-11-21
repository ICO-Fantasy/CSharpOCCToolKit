using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TestWPF.Robotics;

public enum RobotType
{
    Fanuc,
    Custom
}

public class RobotDataStruct
{
    public RobotDataStruct(string name, RobotType type)
    {
        Name = name;
        Type = type;
    }

    public required string Name { get; set; } = "";
    public required RobotType Type { get; set; } = RobotType.Custom;

    public required List<DHParameter> DHParameters { get; set; }

    public int Count => DHParameters.Count;
}

public class SixArmRobot : RobotDataStruct
{
    public SixArmRobot(string name, RobotType type)
        : base(name, type) { }
}
