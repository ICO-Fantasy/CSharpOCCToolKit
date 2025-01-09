using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace TestWPF;
/// <summary>
/// testMainWindow.xaml 的交互逻辑
/// </summary>
public partial class TestMainWindow:Window {
	public TestMainWindow( ) {
		InitializeComponent( );
		Model = new TestMainWindowViewModel( );
		DataContext = Model;
	}
	private TestMainWindowViewModel Model { get; set; }

	private void Button_Click( object sender, RoutedEventArgs e ) {
		// 清空之前的内容
		canvas.Children.Clear( );
		canvas.RowDefinitions.Clear( );
		canvas.ColumnDefinitions.Clear( );

		for( int h = 0; h < Model.HSize; h++ ) {
			// 添加列定义
			canvas.ColumnDefinitions.Add(new ColumnDefinition { Width = new GridLength(1, GridUnitType.Star) });
		}

		for( int v = 0; v < Model.VSize; v++ ) {
			// 添加行定义
			canvas.RowDefinitions.Add(new RowDefinition { Height = new GridLength(1, GridUnitType.Star) });
		}

		for( int h = 0; h < Model.HSize; h++ ) {
			for( int v = 0; v < Model.VSize; v++ ) {
				// 创建绿色背景的Button
				var button = new Button      {
					Background = Brushes.Green,
					HorizontalAlignment = HorizontalAlignment.Stretch,
					VerticalAlignment = VerticalAlignment.Stretch,
					Margin=new Thickness(1,1,1,1)
				};
				int currenth=h;
				int currentv=v;
				button.Click += ( sender, e ) => {
					Model.H = currenth;
					Model.V = currentv;
					// 重置所有按钮颜色
					foreach( var child in canvas.Children ) {
						if( child is Button btn ) {
							btn.Background = Brushes.Green;
						}
					}
					// 当前按钮变红
					button.Background = Brushes.Red;
				};
				// 设置 Label 在 Grid 中的位置
				Grid.SetColumn(button, h);
				Grid.SetRow(button, v);
				canvas.Children.Add(button);
			}
		}
	}

}
