using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using CommunityToolkit.Mvvm.ComponentModel;

namespace TestWPF;
[ObservableObject]
public partial class TestMainWindowViewModel {
	public TestMainWindowViewModel( ) {
		HSize = 5;
		VSize = 10;
		H = 0;
		V = 0;
	}
	[ObservableProperty]
	private int hSize;
	[ObservableProperty]
	private int vSize;
	[ObservableProperty]
	private int h;
	[ObservableProperty]
	private int v;
}
