# 配置需求

项目为 CLR 类库 (.NET) 项目，需要对以下配置进行更改

* `配置属性`->`常规`->`C++语言标准`: ISO C++20 标准 (/std:c++20)
* `配置属性`->`常规`->`C 语言标准`: ISO C17 (2018) 标准 (/std:c17)
* `配置属性`->`高级`->`公共语言运行时支持`: .NET 运行时支持 (/clr:netcore)
* `配置属性`->`高级`->`.NET 目标 Windows 版本`: 10.0.10240.0
* `配置属性`->`高级`->`.NET 目标框架`: .NET 8.0
* `配置属性`->`C/C++`->`附加包含目录`: $(Eigen_Path);$(OCC_IncludeTKDebug);$(OCC_IncludePath);
* `配置属性`->`链接器`->`附加库目录`: $(OCC_LibTKDebug);
* `配置属性`->`链接器`->`输入`->`附加依赖项`: (所有项目需要的依赖库)(Debug 配置可以将全部的 lib 文件引入，Release 配置建议只引入必须的 lib)

## 用户宏的配置

用户需要手动配置用户宏的路径到本机安装的实际路径，修改方法为

* `属性管理器`->`CSharpOCCToolKit`->`Debug | x64`/`Release | x64`->`用户宏`: 将用户宏中的`值`对应的路径修改为本机的实际安装路径

### 一些非必须的更改

* `配置属性`->`常规`->`中间目录`: $(ShortProjectName)\obj\$(Platform)\$(Configuration)\
* `配置属性`->`C/C++`->`预编译头`: 不使用预编译头

# 编码规范

在编写头文件时尽量使用前向声明，避免 include 相互污染。

在任何的头文件中都不应该直接使用using namespace，任何情况下都不应该使用using namespace std。

针对OCC库中继承自Standard_Transient对象（OCC自定义的智能指针）的对象，在CLR代码中继承`OCCTK::OCC::BaseObject`进行声明以避免对象错误的析构。

# OCCView 的交互逻辑

| 模式        |       交互       | mouseButton | ModifierKeys |      CurrentAction3d      |     Cursor      |
| ----------- | :--------------: | :---------: | :----------: | :-----------------------: | :-------------: |
| Normal      |      无操作      |             |              |      Normal_Nothing       | Cursors.Default |
| Normal      |       单选       |    Left     |              |    Normal_SingleSelect    |  Cursors.Hand   |
| Normal      |     连续选择     |    Left     |   Control    |   Normal_MultipleSelect   |  Cursors.Hand   |
| Normal      |     异或选择     |    Left     |    Shift     |     Normal_XORSelect      |  "XOR.cur"   |
| Normal      |       框选       |    Left     |              |     Normal_AreaSelect     |  Cursors.Cross  |
| Normal      |     连续框选     |    Left     |   Control    | Normal_MultipleAreaSelect |  Cursors.Cross  |
| Normal      |     异或框选     |    Left     |    Shift     |   Normal_XORAreaSelect    |   "XOR.cur"   |
| Normal      |   框选区域放大   |    Right    |    Shift     |    Normal_AreaZooming     |   "Zoom.cur"    |
| Normal      |       旋转       |   Middle    |              |  Normal_DynamicRotation   | "Rotation.cur"  |
| Normal      |       平移       |   Middle    |   Control    |   Normal_DynamicPanning   | Cursors.SizeAll |
| Manipulator |      无操作      |             |              |    Manipulator_Nothing    | Cursors.Default |
| Manipulator | 单选并应用操作器 |    Left     |              | Manipulator_SingleSelect  |  Cursors.Hand   |
| Manipulator |    移动操作器    |    Left     |              |  Manipulator_Translation  |  Cursors.Cross  |
