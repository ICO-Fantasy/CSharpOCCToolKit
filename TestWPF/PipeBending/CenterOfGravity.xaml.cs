﻿using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Forms.Integration;
using System.Windows.Media;
using Microsoft.Win32;
using OCCTK.IO;
using OCCTK.OCC.AIS;
using OCCTK.OCC.Topo;
using OCCViewForm;
using TestWPF.Utils;
using Brushes = System.Windows.Media.Brushes;
using Color = OCCTK.Extension.Color;

namespace TestWPF.PipeBending;

/// <summary>
/// CenterOfGravity.xaml 的交互逻辑
/// </summary>
public partial class CenterOfGravity : Window
{
    public CenterOfGravity()
    {
        InitializeComponent();
        // 创建 Windows Forms 控件和 WindowsFormsHost
        WindowsFormsHost aHost = new WindowsFormsHost();
        OCCCanvas = new OCCCanvas();
        OCCCanvas.ShowOriginTrihedron = true;
        aHost.Child = OCCCanvas;
        Canvas_Grid.Children.Add(aHost);
        OCCCanvas.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
        OCCCanvas.Show();
        OCCCanvas.ShowGraduatedTrihedron = false;
        Pipe = new(OCCCanvas.AISContext);
    }

    private readonly OCCCanvas OCCCanvas;

    #region Pipe
    private Pipe Pipe { get; set; }

    private void PopulateTabGrids(List<XYZR> xyzrList, List<YBCR> ybcrList)
    {
        // 清空现有行，保留第一行的标题行
        PipeXYZR_Grid.RowDefinitions.Clear();
        PipeYBCR_Grid.RowDefinitions.Clear();
        PipeXYZR_Grid.Children.Clear();
        PipeYBCR_Grid.Children.Clear();

        // 添加标题行
        PipeXYZR_Grid.RowDefinitions.Add(new RowDefinition { Height = new GridLength(30) });
        PipeYBCR_Grid.RowDefinitions.Add(new RowDefinition { Height = new GridLength(30) });
        AddHeaderLabels(PipeXYZR_Grid, ["X", "Y", "Z", "R"]);
        AddHeaderLabels(PipeYBCR_Grid, ["Y", "B", "C", "R"]);

        // 填充 XYZR 数据
        for (int i = 0; i < xyzrList.Count; i++)
        {
            var xyzr = xyzrList[i];
            PipeXYZR_Grid.RowDefinitions.Add(new RowDefinition { Height = new GridLength(30) });

            AddTextBoxToGrid(PipeXYZR_Grid, i + 1, 0, Math.Round(xyzr.X, 2).ToString());
            AddTextBoxToGrid(PipeXYZR_Grid, i + 1, 2, Math.Round(xyzr.Y, 2).ToString());
            AddTextBoxToGrid(PipeXYZR_Grid, i + 1, 4, Math.Round(xyzr.Z, 2).ToString());
            AddTextBoxToGrid(PipeXYZR_Grid, i + 1, 6, Math.Round(xyzr.R, 2).ToString());
        }

        // 填充 YBCR 数据
        for (int i = 0; i < ybcrList.Count; i++)
        {
            var ybcr = ybcrList[i];
            PipeYBCR_Grid.RowDefinitions.Add(new RowDefinition { Height = new GridLength(30) });

            AddTextBoxToGrid(PipeYBCR_Grid, i + 1, 0, Math.Round(ybcr.Y, 2).ToString());
            AddTextBoxToGrid(PipeYBCR_Grid, i + 1, 2, ybcr.B.ToDegrees(1).ToString());
            AddTextBoxToGrid(PipeYBCR_Grid, i + 1, 4, ybcr.C.ToDegrees(1).ToString());
            AddTextBoxToGrid(PipeYBCR_Grid, i + 1, 6, Math.Round(ybcr.R, 2).ToString());
        }
    }

    private void AddHeaderLabels(Grid grid, string[] headers)
    {
        for (int i = 0; i < headers.Length; i++)
        {
            var label = new Label
            {
                Content = headers[i],
                HorizontalAlignment = HorizontalAlignment.Stretch,
                HorizontalContentAlignment = HorizontalAlignment.Center,
            };
            Grid.SetRow(label, 0);
            Grid.SetColumn(label, i * 2);
            grid.Children.Add(label);
            if (i == 0)
                continue;
            var spliter = new GridSplitter
            {
                Background = Brushes.LightGray,
                HorizontalAlignment = HorizontalAlignment.Stretch
            };
            Grid.SetRow(spliter, 0);
            Grid.SetRowSpan(spliter, 99);
            Grid.SetColumn(spliter, i * 2 - 1);
            grid.Children.Add(spliter);
        }
    }

    private void AddTextBoxToGrid(Grid grid, int row, int column, string text)
    {
        var textBox = new TextBox
        {
            Text = text,
            Background = Brushes.LightYellow,
            Height = 30,
            HorizontalAlignment = HorizontalAlignment.Stretch
        };
        Grid.SetRow(textBox, row);
        Grid.SetColumn(textBox, column);
        grid.Children.Add(textBox);
    }

    private void UpdatePipeProperties()
    {
        PipeName_Label.Content = $"管件名称：{Pipe.Name}";
        PipeOuterDiameter_Label.Content = $"管外径：{Math.Round(Pipe.Diameter, 1)}";
        PipeThickness_Label.Content = $"管壁厚：{Math.Round(Pipe.Thickness, 1)}";
        PopulateTabGrids(Pipe.XYZR, Pipe.YBCR);
    }
    #endregion
    #region 简化调用
    private Action<InteractiveObject, bool> Display => OCCCanvas.Display;
    private Action<InteractiveObject, Color, bool> SetColor => OCCCanvas.AISContext.SetColor;
    private Action<InteractiveObject, double, bool> SetTransparency =>
        OCCCanvas.AISContext.SetTransparency;
    private Action<InteractiveObject, bool> Erase => OCCCanvas.Erase;
    private Action<bool> EraseAll => OCCCanvas.EraseAll;
    private Action Update => OCCCanvas.Update;
    private Action FitAll => OCCCanvas.FitAll;
    #endregion

    #region Button
    private void InputSTEP_Button_Click(object sender, RoutedEventArgs e)
    {
        MessageBox.Show(
            "Demo不支持导入解析STEP，仅用于验证结果\n请在导入STEP之前手动输入管件\n 验证管件的坐标系需要满足以下条件：\n    1. 起点在原点\n    2. 第二点在X轴正方向\n    3. 第三点在XOY平面上（Y为正，Z为0）",
            "提示",
            MessageBoxButton.OK,
            MessageBoxImage.Warning
        );
        //return;
        // 创建文件选择对话框
        OpenFileDialog openFileDialog = new OpenFileDialog
        {
            Filter = "STEP Files (*.STEP;*.step;*.stp)|*.STEP;*.step;*.stp|All Files (*.*)|*.*", // 设置文件过滤器
            InitialDirectory = System.IO.Path.Combine(
                AppDomain.CurrentDomain.BaseDirectory,
                "D:\\Projects\\FANUC折弯机器人\\折弯离线编程软件参考资料"
            ) // 设置初始目录为指定的路径
            //InitialDirectory = System.IO.Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "mods") // 设置初始目录为指定的路径
        };

        // 如果用户选择了文件并点击了“打开”按钮
        if (openFileDialog.ShowDialog() == true)
        {
            string selectedFilePath = openFileDialog.FileName; // 获取选择的文件路径
            //更新Label
            PipeName_Label.Content = $"管件名称：{System.IO.Path.GetFileName(selectedFilePath)}";
            ;
            TShape inputWorkpiece = new STEPExchange(selectedFilePath); // 使用选择的文件路径
            //包围盒左下角点作为原点
            //BoundingBox bnd = new(inputWorkpiece);
            //var AABB = bnd.GetAABB();
            //Trsf toOrigin = new();
            //toOrigin.SetTranslation(AABB.CornerMin(), new());
            //InputWorkpiece = new Transform(inputWorkpiece, toOrigin);
            //EraseAll(false);
            Pipe.originSTEPShape = inputWorkpiece;
            //UpdatePipeProperties();
            AShape aShape = new(inputWorkpiece);
            Display(aShape, false);
            SetTransparency(aShape, 0.8, false);
            Update();
            FitAll();
        }
    }

    private void AddXYZR_Button_Click(object sender, RoutedEventArgs e)
    {
        // 获取当前按钮的父级 Grid
        var parentGrid = FindParentGrid((Button)sender);
        if (parentGrid == null)
            return;

        // 获取当前行数，并计算下一行索引
        int nextRow = parentGrid.RowDefinitions.Count;

        // 添加新行定义
        parentGrid.RowDefinitions.Add(new RowDefinition { Height = new GridLength(30) });

        // 创建新的 TextBox 并放置到新行中
        for (int i = 0; i < 8; i += 2)
        {
            var newTextBox = new TextBox { Background = Brushes.LightYellow, Height = 30 };
            Grid.SetColumn(newTextBox, i);
            Grid.SetRow(newTextBox, nextRow);
            parentGrid.Children.Add(newTextBox);
        }

        // 添加新的按钮到新行中
        var newAddButton = new Button
        {
            Content = "➕",
            Height = 30,
            IsTabStop = false
        };
        newAddButton.Click += AddXYZR_Button_Click; // 重新绑定事件
        Grid.SetColumn(newAddButton, 8);
        Grid.SetRow(newAddButton, nextRow);
        parentGrid.Children.Add(newAddButton);

        var newDeleteButton = new Button
        {
            Content = "➖",
            Height = 30,
            IsTabStop = false
        };
        newDeleteButton.Click += DeleteXYZR_Button_Click; // 绑定删除事件
        Grid.SetColumn(newDeleteButton, 9);
        Grid.SetRow(newDeleteButton, nextRow);
        parentGrid.Children.Add(newDeleteButton);
    }

    private void DeleteXYZR_Button_Click(object sender, RoutedEventArgs e)
    {
        // 获取当前按钮的父级 Grid
        var parentGrid = FindParentGrid((Button)sender);
        if (parentGrid == null)
            return;

        // 获取按钮所在的行索引
        int rowToDelete = Grid.GetRow((Button)sender);

        // 移除当前行的所有控件
        var childrenToRemove = parentGrid
            .Children.Cast<UIElement>()
            .Where(child => Grid.GetRow(child) == rowToDelete)
            .ToList();

        foreach (var child in childrenToRemove)
        {
            parentGrid.Children.Remove(child);
        }

        // 移除行定义
        parentGrid.RowDefinitions.RemoveAt(rowToDelete);

        // 更新所有控件的行索引以保持一致性
        foreach (var child in parentGrid.Children.Cast<UIElement>())
        {
            int currentRow = Grid.GetRow(child);
            if (currentRow > rowToDelete)
            {
                Grid.SetRow(child, currentRow - 1);
            }
        }
    }

    // Helper 方法：递归查找按钮的父级 Grid
    private static Grid? FindParentGrid(UIElement? element)
    {
        while (element != null)
        {
            if (element is Grid grid)
            {
                return grid;
            }
            element = VisualTreeHelper.GetParent(element) as UIElement;
        }
        return null;
    }

    private void InputPipeInfo_Button_Click(object sender, RoutedEventArgs e)
    {
        OCCCanvas.EraseAll(false);
        //添加名称
        Pipe.Name = InputPipeInfo_Name.Text ?? "pipe";
        //添加管径
        if (double.TryParse(InputPipeInfo_Diameter.Text, out double d))
        {
            Pipe.Diameter = d;
        }
        else
        {
            MessageBox.Show("输入的管径有误", "提示", MessageBoxButton.OK, MessageBoxImage.Warning);
            return;
        }
        //添加管厚度
        if (double.TryParse(InputPipeInfo_Diameter.Text, out double t))
        {
            Pipe.Thickness = t;
        }
        else
        {
            MessageBox.Show("输入的厚度有误", "提示", MessageBoxButton.OK, MessageBoxImage.Warning);
            return;
        }
        #region 添加XYZR、YBCR
        if (InputPipeInfo_TabControl.SelectedItem is TabItem selectedTab)
        {
            if (selectedTab.Header.ToString() == "XYZR")
            {
                List<XYZR> newXYZ = [];
                foreach (var row in GetGridRows(PipeInfo_XYZR))
                {
                    double x = double.TryParse(((TextBox)row[0]).Text, out double parsedX)
                        ? parsedX
                        : 0.0;
                    double y = double.TryParse(((TextBox)row[1]).Text, out double parsedY)
                        ? parsedY
                        : 0.0;
                    double z = double.TryParse(((TextBox)row[2]).Text, out double parsedZ)
                        ? parsedZ
                        : 0.0;
                    double r = double.TryParse(((TextBox)row[3]).Text, out double parsedR)
                        ? parsedR
                        : 0.0;

                    newXYZ.Add(
                        new XYZR
                        {
                            X = x,
                            Y = y,
                            Z = z,
                            R = r
                        }
                    );
                }
                Pipe.XYZR = newXYZ;
            }
            else if (selectedTab.Header.ToString() == "YBCR")
            {
                List<YBCR> newYBC = []; // 清空当前列表
                foreach (var row in GetGridRows(PipeInfo_YBCR))
                {
                    double y = double.TryParse(((TextBox)row[0]).Text, out double parsedY)
                        ? parsedY
                        : 0.0;
                    double b = double.TryParse(((TextBox)row[1]).Text, out double parsedB)
                        ? parsedB
                        : 0.0;
                    double c = double.TryParse(((TextBox)row[2]).Text, out double parsedC)
                        ? parsedC
                        : 0.0;
                    double r = double.TryParse(((TextBox)row[3]).Text, out double parsedR)
                        ? parsedR
                        : 0.0;

                    newYBC.Add(
                        new YBCR
                        {
                            Y = y,
                            B = b.ToRadians(),
                            C = c.ToRadians(),
                            R = r
                        }
                    );
                }
                Pipe.YBCR = newYBC;
            }
        #endregion
            Pipe.makeWireFromYBC(); //debug
            // 调用更新方法
            UpdatePipeProperties();
        }
    }

    // 获取Grid中每一行的控件集合
    private List<List<UIElement>> GetGridRows(Grid grid)
    {
        List<List<UIElement>> rows = new();

        for (int i = 1; i < grid.RowDefinitions.Count; i++) // 跳过标题行
        {
            List<UIElement> row = grid
                .Children.Cast<UIElement>()
                .Where(child => Grid.GetRow(child) == i)
                .OrderBy(child => Grid.GetColumn(child))
                .ToList();

            if (row.Count > 0)
            {
                rows.Add(row);
            }
        }

        return rows;
    }

    private void InputPipeValues_Button_Click(object sender, RoutedEventArgs e)
    {
        InputPipeValues_Button.Visibility = Visibility.Collapsed;
        InputPipeValues_ScrollViewer.Visibility = Visibility.Visible;
        InputPipeInfo_Button.Visibility = Visibility.Visible;
    }

    #endregion
}