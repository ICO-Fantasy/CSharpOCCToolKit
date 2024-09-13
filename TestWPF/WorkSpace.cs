using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Messaging;
using TestWPF.Messages;

namespace TestWPF;

public partial class WorkSpace : ObservableObject
{
    [ObservableProperty]
    private RobotProperties? robot;

    partial void OnRobotChanged(RobotProperties? value)
    {
        // 每当 Robot 属性发生变化时，发送消息
        if (value != null)
        {
            SendRobotChangedMessage();
        }
    }

    private void SendRobotChangedMessage()
    {
        WeakReferenceMessenger.Default.Send(new RobotChangedMessage(Robot));
    }

    public WorkSpace()
    {
        Robot = new("D:\\ICO\\CSharpOCCToolKit\\TestWPF\\Configs\\testrobot.json");
        //// 发送初始的 Robot 对象
        //SendRobotChangedMessage();
    }
}
