using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TestWPF.Utils;

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
