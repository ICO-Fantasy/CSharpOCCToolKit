using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using CommunityToolkit.Mvvm.Messaging.Messages;

namespace TestWPF.Messages;

public class RobotChangedMessage : ValueChangedMessage<RobotProperties>
{
    public RobotChangedMessage(RobotProperties value)
        : base(value) { }
}
