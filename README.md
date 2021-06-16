# BugBullet
### 简介

数字媒体实训，根据PhysX物理引擎制作一个第一人称游戏，渲染引擎使用原生OpenGL。

### 目录结构

共有五个子模块：Main、Render、Data、Utils、Test。对应文件夹存放代码文件。

其他：

1. Common：
2. compiler：项目文件及资源文件存放位置
3. Graphics：OpenGL-Shader所需的头文件及库(glew,glfw,assimp,soil2)（x64版本）
4. lib：存放子模块生成的lib

### 模块功能

- Main：游戏主逻辑

- Render：负责游戏渲染（读入obj模型并渲染）
- Data：储存游戏中的自定义数据
- Utils：工具模块，包含各种工具类（如转换函数，输出等）
- Test：测试模块