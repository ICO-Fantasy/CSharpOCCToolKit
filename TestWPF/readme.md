﻿# 配置需求

项目为 CLR 类库 (.NET) 项目，需要对以下配置进行更改

* `配置属性`->`常规`->`C++语言标准`: ISO C++20 标准 (/std:c++20)
* `配置属性`->`常规`->`C 语言标准`: ISO C17 (2018) 标准 (/std:c17)
* `配置属性`->`高级`->`公共语言运行时支持`: .NET 运行时支持 (/clr:netcore)
* `配置属性`->`高级`->`.NET 目标 Windows 版本`: 10.0.10240.0
* `配置属性`->`高级`->`.NET 目标框架`: .NET 6.0
* `配置属性`->`C/C++`->`附加包含目录`: $(OCC_IncludeTKDebug);\$(OCC_IncludePath);
* `配置属性`->`链接器`->`附加库目录`: $(OCC_LibTKDebug);
* `配置属性`->`链接器`->`输入`->`附加依赖项`: (所有项目需要的依赖库)(Debug 配置可以将全部的 lib 文件引入，Release 配置建议只引入必须的 lib)

### 一些非必须的更改

* `配置属性`->`常规`->`中间目录`: $(ShortProjectName)\obj\$(Platform)\$(Configuration)\
* `配置属性`->`C/C++`->`预编译头`: 不使用预编译头

# 源码错误

在 OCC 源码中包含 HashCode 函数，其与 `.NET Core` 中的方法 `System::HashCode` 存在符号混淆，因此需要改写 OCC 源码中存在混淆的 `HashCode` 为 `::HashCode`

# 生成后事件

OCC 自身需要引用TKxxx.dll，以及第三方dll，包括：freetype、tcl、FreeImage
