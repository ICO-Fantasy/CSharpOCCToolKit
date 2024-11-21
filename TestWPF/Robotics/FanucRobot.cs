using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TestWPF.Robotics;

public class FanucRobot : SixArmRobot
{
    public FanucRobot(string name)
        : base(name, RobotType.Fanuc) { }
}
