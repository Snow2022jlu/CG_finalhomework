# Minkowski Sum Visualizer

一个基于 Qt 的交互式可视化工具，支持：
- 凸多边形 + 凸多边形 的闵可夫斯基和计算
- 非凸多边形 + 凸多边形 的三角剖分 + 分别求和 + 合并（含空洞处理）
- 鼠标点击输入顶点，回车完成多边形

## 文件结构

| 文件名                | 功能说明                                 |
| --------------------- | ---------------------------------------- |
| `main.cpp`            | 程序入口                                 |
| `mainwindow.h/cpp`    | 主窗口 UI，按钮绑定                      |
| `drawingcanvas.h/cpp` | 绘图画布，处理用户输入与绘制             |
| `utils.h/cpp`         | 几何操作工具：凸包、三角剖分、布尔合并等 |

## 操作说明

1. 点击「输入多边形」，鼠标左键点击添加顶点，回车完成一个多边形（需要两个多边形）
2. 点击「凸 + 凸」计算两个凸多边形的 Minkowski 和
3. 点击「凸 + 非凸」进入三步操作流程：
   - 三角剖分：将非凸多边形分解为多个三角形
   - 分别求和：每个三角形与凸多边形分别求 Minkowski 和
   - 合并：用二路归并 + 空洞识别合并所有结果
4. 空洞将以无填充边框 + 红色顶点展示

## 编译方式

使用 Qt Creator 打开 `.pro` 工程文件构建



## 效果展示（Screenshots）

### 凸 + 凸

![image-20250608142253549](C:\Users\zhy\AppData\Roaming\Typora\typora-user-images\image-20250608142253549.png)

### 非凸 + 凸（三角剖分）

![image-20250608142504256](C:\Users\zhy\AppData\Roaming\Typora\typora-user-images\image-20250608142504256.png)

---

### 非凸 + 凸（分别求和）

![image-20250608142651515](C:\Users\zhy\AppData\Roaming\Typora\typora-user-images\image-20250608142651515.png)

### 非凸 + 凸（合并（有空洞））

![image-20250608142719802](C:\Users\zhy\AppData\Roaming\Typora\typora-user-images\image-20250608142719802.png)

