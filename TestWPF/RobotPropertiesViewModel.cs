using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing.Drawing2D;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using CommunityToolkit.Mvvm.ComponentModel;
using MathNet.Numerics.LinearAlgebra;
using MathNet.Numerics.LinearAlgebra.Double;
using TestWPF.Utils;

namespace TestWPF;

public partial class RobotPropertiesViewModel : ObservableObject
{
    [ObservableProperty]
    private RobotProperties _model;

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

    private Matrix<double> toolCoordinates;

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
        _model = new RobotProperties("D:\\ICO\\CSharpOCCToolKit\\TestWPF\\Configs\\testrobot.json");

        Theta1 = _model.RobotData.DHParameters[0].Theta;
        Theta2 = _model.RobotData.DHParameters[1].Theta;
        Theta3 = _model.RobotData.DHParameters[2].Theta;
        Theta4 = _model.RobotData.DHParameters[3].Theta;
        Theta5 = _model.RobotData.DHParameters[4].Theta;
        Theta6 = _model.RobotData.DHParameters[5].Theta;

        Theta1Min = _model.RobotData.DHParameters[0].Min;
        Theta2Min = _model.RobotData.DHParameters[1].Min;
        Theta3Min = _model.RobotData.DHParameters[2].Min;
        Theta4Min = _model.RobotData.DHParameters[3].Min;
        Theta5Min = _model.RobotData.DHParameters[4].Min;
        Theta6Min = _model.RobotData.DHParameters[5].Min;

        Theta1Max = _model.RobotData.DHParameters[0].Max;
        Theta2Max = _model.RobotData.DHParameters[1].Max;
        Theta3Max = _model.RobotData.DHParameters[2].Max;
        Theta4Max = _model.RobotData.DHParameters[3].Max;
        Theta5Max = _model.RobotData.DHParameters[4].Max;
        Theta6Max = _model.RobotData.DHParameters[5].Max;

        UpdateToolCoordinates();
    }

    public void UpdateToolCoordinates()
    {
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
}
