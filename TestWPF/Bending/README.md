# TestWPF/Bending 设计说明

本文档说明 `TestWPF/Bending` 目录内的钣金折弯解析设计，重点覆盖：

- 从 STEP 模型到折弯特征的解析流程
- 关键数据结构及其关系
- 主要算法逻辑（板厚估计、折弯识别、树构建、展开参数计算）

## 1. 目录与职责

- `BendingTest.xaml` / `BendingTest.xaml.cs`
  - UI 交互、STEP 导入、3D 显示、节点选中与可视化调试。
- `BendingDataStruct.cs`
  - 几何基础数据结构：`Face`、`Edge`、`BendingDS`、`BendingType`。
- `BendingNode.cs`
  - 树节点结构与折弯展开计算：`NodeDS`、`RootNode`、`LeafNode`、`VBendNode`、`ClosedHemNode`。
- `BendingTree.cs`
  - 解析入口与主流程：拓扑解析、折弯识别、树构建、折弯合并。
- `Kparam.cs`
  - K 值表与中性层半径计算。

## 2. 核心数据设计

### 2.1 `Face`

封装拓扑面 `TFace`，并缓存算法需要的几何属性。

关键字段：

- `TopoFace` / `Shape`：原始 OCC 面与所属形体。
- `Index`：默认使用 `GetHashCode()` 作为身份标识。
- `Edges`：该面的边集合（`HashSet<Edge>`）。
- `AdjacentFaces`：通过共享边建立的邻面映射（`Dictionary<Edge, Face>`）。
- `Area`、`FaceCenter`、`Normal`。
- 圆柱专属属性：`CircleAixs`、`CircleRadius`、`CircleCenter`、`CircleAngle`、`CircleNormal`。

要点：

- 构造时立即提取面类型（`SurfaceType`）和边信息。
- 若是圆柱面，会采样 U 参数起点/中点/终点三点定圆，得到圆心与圆弧角，作为折弯识别输入。

### 2.2 `Edge`

封装拓扑边 `TEdge`。

关键字段：

- `Type`（直线/圆弧等）、`Points`（端点）、`Length`。
- 圆弧边属性：`CircleCenter`、`CircleRadius`。
- `Faces`：共享该边的所有面（后续由 `BendingTree` 回填）。

要点：

- `Index` 同样基于 `GetHashCode()`，后续会对拓扑等价边做索引归一。

### 2.3 `BendingDS`

表示“一个折弯特征”的几何集合。

组成：

- 两个同心圆柱面：`InnerFace` + `OuterFace`
- 若干扇形连接面：`LeftSectorFaces` + `RightSectorFaces`

关键字段：

- `Thickness = OuterRadius - InnerRadius`（四舍五入到 0.1）
- `Angle`（内圆弧角）
- `Axis`（折弯轴）
- `Normal`（弯曲法向）
- `Length`（取圆柱侧两条轴向直边中最长两条平均值）
- `Type`：目前仅区分 `VBend` 与 `ClosedHem`（依据 `Angle` 是否接近 π）。

### 2.4 节点模型（`NodeDS` / `LeafNode`）

- `RootNode`
  - 主平面与其平面组（不参与折弯的连通面集合）。
- `LeafNode`
  - 一道折弯及其后续子折弯。
  - 绑定 `BendingDS`、父节点 `Parent`、当前平面组 `FaceSet`。
  - 维护折弯展开状态 `Unfolded` 与当前累计变换 `Location`。

`LeafNode` 额外维护：

- 内/外圆柱上靠近父侧与子侧的边（`InnerFatherEdge` 等）
- 两侧 8 个关键点（内父、内子、外子、外父）
- `BendingAxis`、`UnfoldingDirection`、`BendingAngle`、`FlatLength`

派生类：

- `VBendNode`：V 弯展开长度按 K 因子计算。
- `ClosedHemNode`：压死边展开长度暂按 `1.2 * thickness`。

## 3. 从 STEP 到折弯特征：工作流

## 3.1 UI 触发

1. `InputWorkPiece_Button_Click`
   - 读取 STEP：`new STEPExchange(path).Shape().TopoShape`
   - 将包围盒最小角点平移到原点，得到 `InputWorkpiece`
2. `Unfold_Button_Click`
   - 调用 `CreateBendingTree()` -> `new BendingTree(InputWorkpiece)`

## 3.2 `BendingTree` 初始化主流程

构造函数中关键步骤：

1. `GetData()`：解析全模型拓扑并识别折弯
2. 主平面选择：默认规则 `Area`，取最大面积面作为 `RootNode.MainFace`
3. `GetFaceSet(mainface)`：获取根节点平面组
4. `CreateTree()`：按邻接关系广度优先建折弯树
5. `CombineBendings()`：处理压死边后继关系 + 折弯共轴合并

## 4. 关键算法逻辑

### 4.1 全部面边解析与邻接构建（`GetData` 前半）

1. 遍历所有 `FACE`，构建 `Face` 对象。
2. 收集所有 `Edge`，对拓扑相同边统一 `Index`。
3. 生成 `AllEdges` 去重集合。
4. 对每条边统计共享面 `Faces`。
5. 若一条边被多个面共享，则这些面两两互记为 `AdjacentFaces`。

结果：形成“面 - 边 - 邻面”拓扑图，为后续折弯识别和树构建提供基础。

### 4.2 板厚估计（`GetData` 中段）

策略：

- 对所有边长度（四舍五入到 0.01）分组计数。
- 取出现频次最高的前 8 个候选长度，逐一尝试作为板厚 `Thickness`。
- 每次候选都调用 `GetAllBendings()` 做折弯识别，若识别到至少一个折弯则接受该板厚。

原因：

- 钣金模型中，板厚对应的短边长度通常高频出现。

### 4.3 折弯识别（`GetAllBendings`）

识别目标：找到一个折弯的“圆柱 - 扇形 - 圆柱”闭环。

核心判据：

- 两圆柱面同心（圆心距离 < `LINEAR_TOL`）
- 半径不同且半径差（取 0.01 精度）等于当前 `Thickness`
- 两圆柱有公共邻接面集合作为扇形连接面 `sectorFaces`

识别方式：

- 以圆柱面为起点，通过多层邻接遍历寻找闭合拓扑模式。
- 匹配成功即构造 `BendingDS(cylin1, cylin2, sectorFaces)`。
- 同时从临时面池移除已使用面，降低重复识别。

### 4.4 主平面组提取（`GetFaceSet`）

- 从某主平面出发递归扩张邻面。
- 遇到属于任意 `BendingDS.BendingFaces` 的面时停止，不跨越折弯区。
- 得到“同一刚性平面块”的连通面集合。

### 4.5 折弯树构建（`CreateNodeBFS`）

- 采用 BFS，队列元素为 `(当前主面, 当前节点)`。
- 从当前主面的邻面中查找与某个 `BendingDS.CylinderFaces` 相邻的候选。
- 跨过折弯区到另一侧找到新主面 `adj2`。
- 对 `adj2` 求 `faceSet`，构造 `VBendNode` 或 `ClosedHemNode`。
- 新节点入队继续扩展。

结果：

- 形成从根平面出发、沿折弯传播的层级树。

### 4.6 `LeafNode` 的几何求解

`LeafNode` 构造时会做两件核心事：

1. **辅助几何配对（`CalculateAuxiliaryInfo`）**

- 在内/外圆柱面上分别定位父侧边与子侧边。
- 通过扇形左右面匹配，确定两侧 8 个关键点。
- 支持“主平面在内圆柱侧”与“主平面在外圆柱侧”两种情况。

1. **方向与轴修正**

- 初始折弯轴来自 `BendingDS.Axis`。
- 通过“将子面法向按折弯角旋转后与父面法向比较”判断轴向是否需要反转。
- 展开方向由父面法向与折弯线方向叉乘得到，再按弯曲法向一致性修正。

### 4.7 展开参数计算

#### `VBendNode`

- `KRadius = innerRadius + k * thickness`
- `BendingAngle = π - innerArcAngle`
- `FlatLength = KRadius * innerArcAngle`
- 展开变换：
  - 平移 `T = translation(UnfoldingDirection * FlatLength)`
  - 旋转 `R = rotation(BendingAxis.Reversed, innerArcAngle)`
  - `UnfoldTransform = T * R`

#### `ClosedHemNode`

- `BendingAngle = π`
- `FlatLength = 1.2 * thickness`（当前经验值）
- 变换构造方式与 V 弯一致。

### 4.8 折弯合并（`CombineBendings`）

分两步：

1. **死边相关处理**

- 先 `FoldAllBendings()` 回到折叠态。
- 将死边后的子树节点收集为 `NodesAfterHem`，并从死边节点切开子关系。
- 通过轴位置接近 + 方向平行建立 `HemNode` 对应。

1. **常规共轴合并**

- `UnfoldAllBendings()` 后，在展开态比较轴线。
- 若两节点展开后轴线 `IsCoaxial`，则合并：
  - `otherNode` 挂入 `node.CombinedNodes`
  - 从原父节点子列表移除 `otherNode`

## 5. UI 层如何消费解析结果

- `TreeView` 展示 `RootNode + AllLeafNodes`。
- 选中节点时调用 `Show(...)`：
  - 显示平面组、折弯面、法向、轴向、标签等调试几何。
- 点击 3D 面时：
  - 用 `BendingFaceMap` 定位所属 `LeafNode`
  - 切换 `Unfolded` 状态并重绘该节点及其子节点。

## 6. 设计假设与当前边界

当前实现明确依赖以下假设：

- 只支持 `VBend` 与 `ClosedHem` 两类。
- 折弯可由“两同心圆柱 + 扇形连接面”模式识别。
- 板厚可由高频边长统计推断。
- 主平面默认取最大面积面。

已在代码中可见的待改进点：

- 多处 `todo` 标注（例如扇形面点位父子逻辑“逻辑反了”）。
- `GetAllBendings` 与 `CalculateAuxiliaryInfo` 依赖复杂邻接与点匹配，异常路径较多。
- `ClosedHem` 展开长度仍是经验常数（`1.2*t`）。
- 部分容差硬编码（`1e-4/1e-2`），不同模型精度下可能需要参数化。

## 7. 快速调用链（入口到结果）

1. `BendingTest.InputWorkPiece_Button_Click`：读 STEP、平移到原点。
2. `BendingTest.Unfold_Button_Click`：创建 `BendingTree`。
3. `BendingTree.GetData`：建拓扑、估板厚、识别 `AllBendings`。
4. `BendingTree.CreateTree`：构建 `RootNode` + `LeafNode` 层级。
5. `BendingTree.CombineBendings`：死边处理 + 共轴合并。
6. UI 基于节点 `Show()` / `Unfolded` 状态做可视化与交互。

---

如果要继续增强鲁棒性，优先建议从三处入手：

- 将容差与板厚候选策略参数化。
- 给 `CalculateAuxiliaryInfo` 增加可诊断中间结果输出。
- 为典型 STEP 用例补最小回归测试（V 弯、压死边、多折连续件）。
