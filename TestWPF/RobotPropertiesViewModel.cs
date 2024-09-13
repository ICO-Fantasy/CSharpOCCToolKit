using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing.Drawing2D;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Navigation;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Messaging;
using MathNet.Numerics.LinearAlgebra;
using MathNet.Numerics.LinearAlgebra.Double;
using OCCTK.OCC.AIS;
using OCCTK.OCC.BRepPrimAPI;
using OCCTK.OCC.gp;
using OCCTK.OCC.TopoDS;
using TestWPF.Messages;
using TestWPF.Utils;

namespace TestWPF;

public partial class RobotPropertiesViewModel : ObservableObject
{
    [ObservableProperty]
    private RobotProperties? _model;

    #region 角度值

    [ObservableProperty]
    private double theta1;

    [ObservableProperty]
    private double theta1Min;

    [ObservableProperty]
    private double theta1Max;

    partial void OnTheta1Changed(double value) => UpdateToolCoordinates();

    [ObservableProperty]
    private double theta2;

    [ObservableProperty]
    private double theta2Min;

    [ObservableProperty]
    private double theta2Max;

    partial void OnTheta2Changed(double value) => UpdateToolCoordinates();

    [ObservableProperty]
    private double theta3;

    [ObservableProperty]
    private double theta3Min;

    [ObservableProperty]
    private double theta3Max;

    partial void OnTheta3Changed(double value) => UpdateToolCoordinates();

    [ObservableProperty]
    private double theta4;

    [ObservableProperty]
    private double theta4Min;

    [ObservableProperty]
    private double theta4Max;

    partial void OnTheta4Changed(double value) => UpdateToolCoordinates();

    [ObservableProperty]
    private double theta5;

    [ObservableProperty]
    private double theta5Min;

    [ObservableProperty]
    private double theta5Max;

    partial void OnTheta5Changed(double value) => UpdateToolCoordinates();

    [ObservableProperty]
    private double theta6;

    [ObservableProperty]
    private double theta6Min;

    [ObservableProperty]
    private double theta6Max;

    partial void OnTheta6Changed(double value) => UpdateToolCoordinates();

    #endregion

    #region 机器人模型

    private TShape? BaseShape;
    private AShape? BaseAIS;

    private TShape? Arm1Shape;
    private TShape? Arm2Shape;
    private TShape? Arm3Shape;
    private TShape? Arm4Shape;
    private TShape? Arm5Shape;
    private TShape? Arm6Shape;

    private AShape? Arm1AIS;
    private AShape? Arm2AIS;
    private AShape? Arm3AIS;
    private AShape? Arm4AIS;
    private AShape? Arm5AIS;
    private AShape? Arm6AIS;

    #endregion
    private Matrix<double>? toolCoordinates;

    #region 末端坐标系

    [ObservableProperty]
    private double xValue;

    [ObservableProperty]
    private double yValue;

    [ObservableProperty]
    private double zValue;

    [ObservableProperty]
    private double wValue;

    [ObservableProperty]
    private double pValue;

    [ObservableProperty]
    private double rValue;

    #endregion

    public RobotPropertiesViewModel()
    {
        // 订阅消息，接收 RobotProperties 的更新
        WeakReferenceMessenger.Default.Register<RobotChangedMessage>(
            this,
            (r, message) =>
            {
                Model = message.Value;
                // 处理 RobotProperties 更新后的逻辑
                SetDefault();
                UpdateToolCoordinates();
                DrawRobot();
            }
        );
    }

    private void SetDefault()
    {
        if (Model == null)
            return;

        Theta1 = Model.RobotData.DHParameters[0].Theta;
        Theta2 = Model.RobotData.DHParameters[1].Theta;
        Theta3 = Model.RobotData.DHParameters[2].Theta;
        Theta4 = Model.RobotData.DHParameters[3].Theta;
        Theta5 = Model.RobotData.DHParameters[4].Theta;
        Theta6 = Model.RobotData.DHParameters[5].Theta;

        Theta1Min = Model.RobotData.DHParameters[0].Min;
        Theta2Min = Model.RobotData.DHParameters[1].Min;
        Theta3Min = Model.RobotData.DHParameters[2].Min;
        Theta4Min = Model.RobotData.DHParameters[3].Min;
        Theta5Min = Model.RobotData.DHParameters[4].Min;
        Theta6Min = Model.RobotData.DHParameters[5].Min;

        Theta1Max = Model.RobotData.DHParameters[0].Max;
        Theta2Max = Model.RobotData.DHParameters[1].Max;
        Theta3Max = Model.RobotData.DHParameters[2].Max;
        Theta4Max = Model.RobotData.DHParameters[3].Max;
        Theta5Max = Model.RobotData.DHParameters[4].Max;
        Theta6Max = Model.RobotData.DHParameters[5].Max;
    }

    public void UpdateToolCoordinates()
    {
        if (Model == null)
            return;

        toolCoordinates = RobotDynamics.ForwardKinematics(
            [Theta1, Theta2, Theta3, Theta4, Theta5, Theta6],
            DenseMatrix.OfColumnArrays(Model.RobotData.BaseCoordinate),
            Model.RobotData,
            Matrix<double>.Build.DenseIdentity(4)
        );

        //提取XYZ
        XValue = toolCoordinates[0, 3];
        YValue = toolCoordinates[1, 3];
        ZValue = toolCoordinates[2, 3];

        // 提取旋转矩阵的值
        double r11 = toolCoordinates[0, 0];
        double r12 = toolCoordinates[0, 1];
        double r13 = toolCoordinates[0, 2];
        double r21 = toolCoordinates[1, 0];
        double r22 = toolCoordinates[1, 1];
        double r23 = toolCoordinates[1, 2];
        double r31 = toolCoordinates[2, 0];
        double r32 = toolCoordinates[2, 1];
        double r33 = toolCoordinates[2, 2];

        // 计算欧拉角 (yaw, pitch, roll)
        double yaw = Math.Atan2(r21, r11); // ψ
        double pitch = Math.Atan2(-r31, Math.Sqrt(r32 * r32 + r33 * r33)); // θ
        double roll = Math.Atan2(r32, r33); // φ
        WValue = yaw * (180.0 / Math.PI);
        PValue = pitch * (180.0 / Math.PI);
        RValue = roll * (180.0 / Math.PI);
    }

    #region 3D
    private void DrawRobot()
    {
        if (Model == null)
            return;
        Ax2 baseAx2 = new Ax2(Model.RobotData.BaseCoordinate);
        var m = new MakeBox(baseAx2, 10, 10, 1, 10, 10, 0);
        BaseShape = m.Shape();
        BaseAIS = new(BaseShape);
        App.Current.canva.Display(BaseAIS);
    }

    private void ReDrawRobot() { }
    #endregion
}
