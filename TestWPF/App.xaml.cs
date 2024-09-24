using System;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Runtime.InteropServices;
using System.Threading;
using System.Windows;
using System.Windows.Controls;
using log4net;
using log4net.Config;
using Microsoft.VisualBasic.Logging;
using OCCTK.OCC.AIS;
using OCCViewForm;
using TestWPF;
using Application = System.Windows.Application;

namespace TestWPF;

public class Singleton<T>
    where T : class, new()
{
    // 私有静态变量用于保存单例实例
    private static T? _instance;
    private static readonly object _lock = new();

    // 私有构造函数，防止外部实例化
    protected Singleton() { }

    // 公共静态属性，用于访问单例实例
    public static T Instance
    {
        get
        {
            // 确保线程安全
            lock (_lock)
            {
                _instance ??= new T();
                return _instance;
            }
        }
    }
}

/// <summary>
/// Execute logic for App.xaml
/// </summary>
public partial class App : Application
{
    private static readonly ILog log = LogManager.GetLogger(typeof(App));
    public static new App Current => (App)Application.Current;

    //public WorkSpace workSpace;
    public readonly OCCCanvas canva = Singleton<OCCCanvas>.Instance;

    [STAThread]
    protected override void OnStartup(StartupEventArgs e)
    {
        // 加载 log4net 配置文件
        var logRepository = LogManager.GetRepository(System.Reflection.Assembly.GetEntryAssembly());
        XmlConfigurator.Configure(logRepository, new FileInfo("log4net.config"));

        log.Info($"程序启动");

        AppContext.SetSwitch("Switch.System.Windows.Input.Stylus.EnablePointerSupport", true);

        CultureInfo culture = new CultureInfo("zh-cn");
        Thread.CurrentThread.CurrentCulture = culture;
        Thread.CurrentThread.CurrentUICulture = culture;
        CultureInfo.DefaultThreadCurrentCulture = culture;
        CultureInfo.DefaultThreadCurrentUICulture = culture;

        CreateInstanceMutexes();

        // Start main window
        //MainWindow = new RobotWindows();
        //MainWindow = new CanvasTest();
        MainWindow = new SimpleClamp();
        MainWindow.Show();

        ShutdownMode = ShutdownMode.OnMainWindowClose;
        base.OnStartup(e);
    }

    /*
* Creates the two mutexes checked for by the installer/uninstaller to see if
* the program is still running.
* One of the mutexes is created in the global name space (which makes it
* possible to access the mutex across user sessions in Windows XP); the other
* is created in the session name space(because versions of Windows NT prior
* to 4.0 TSE don't have a global name space and don't support the 'Global\'
* prefix).
*/
    void CreateInstanceMutexes()
    {
        //const string mutexName = "MacadInstanceRunning";
        ///* By default on Windows NT, created mutexes are accessible only by the user
        // * running the process. We need our mutexes to be accessible to all users, so
        // * that the mutex detection can work across user sessions in Windows XP. To
        // * do this we use a security descriptor with a null DACL.
        // */
        //IntPtr ptrSecurityDescriptor = IntPtr.Zero;
        //try
        //{
        //    var securityDescriptor = new Win32Api.SECURITY_DESCRIPTOR();
        //    Win32Api.InitializeSecurityDescriptor(
        //        out securityDescriptor,
        //        Win32Api.SECURITY_DESCRIPTOR_REVISION
        //    );
        //    Win32Api.SetSecurityDescriptorDacl(ref securityDescriptor, true, IntPtr.Zero, false);
        //    ptrSecurityDescriptor = Marshal.AllocCoTaskMem(Marshal.SizeOf(securityDescriptor));
        //    Marshal.StructureToPtr(securityDescriptor, ptrSecurityDescriptor, false);

        //    var securityAttributes = new Win32Api.SECURITY_ATTRIBUTES();
        //    securityAttributes.nLength = Marshal.SizeOf(securityAttributes);
        //    securityAttributes.lpSecurityDescriptor = ptrSecurityDescriptor;
        //    securityAttributes.bInheritHandle = false;

        //    if (
        //        Win32Api.CreateMutex(ref securityAttributes, false, mutexName) == IntPtr.Zero
        //        || Win32Api.CreateMutex(ref securityAttributes, false, @"Global\" + mutexName)
        //            == IntPtr.Zero
        //    )
        //    {
        //        var lastError = Win32Error.GetLastError();
        //        // if we get the ERROR_ALREADY_EXISTS value, there is
        //        // already another instance of this application running.
        //        // That is ok and no error.
        //        if (lastError != Win32Error.ERROR_ALREADY_EXISTS)
        //        {
        //            Console.WriteLine(
        //                $"InstanceMutex creation failed: {Marshal.GetLastWin32Error()}"
        //            );
        //        }
        //    }
        //}
        //catch (Exception e)
        //{
        //    Console.WriteLine(e);
        //}

        //if (ptrSecurityDescriptor != IntPtr.Zero)
        //    Marshal.FreeCoTaskMem(ptrSecurityDescriptor);
    }
}
