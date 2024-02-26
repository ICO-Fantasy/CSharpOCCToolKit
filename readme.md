# 配置需求

项目为 CLR 类库 (.NET) 项目，需要对以下配置进行更改

* `配置属性`->`常规`->`C++语言标准`: ISO C++20 标准 (/std:c++20)
* `配置属性`->`常规`->`C 语言标准`: ISO C17 (2018) 标准 (/std:c17)
* `配置属性`->`高级`->`公共语言运行时支持`: .NET 运行时支持(/clr:netcore)
* `配置属性`->`高级`->`.NET 目标 Windows 版本`: 10.0.10240.0
* `配置属性`->`高级`->`.NET 目标框架`: .NET 6.0
* `配置属性`->`C/C++`->`附加包含目录`: $(OCC_IncludeTKDebug);\$(OCC_IncludePath);
* `配置属性`->`链接器`->`附加库目录`: $(OCC_LibTKDebug);
* `配置属性`->`链接器`->`输入`->`附加依赖项`: (所有项目需要的依赖库)(Debug配置可以将全部的lib文件引入，Release配置建议只引入必须的lib)

### 一些非必须的更改

* `配置属性`->`常规`->`中间目录`: $(ShortProjectName)\obj\$(Platform)\$(Configuration)\
* `配置属性`->`C/C++`->`预编译头`: 不使用预编译头

# 源码错误

在OCC源码中包含HashCode函数，其与.NET Core中的方法System::HashCode存在符号混淆，因此需要改写OCC源码中。
