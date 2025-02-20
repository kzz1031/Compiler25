<<<<<<< HEAD
# 环境配置

以下基于 Windows 的 Linux 子系统 WSL ，版本是 Ubuntu 20.04.6 LTS ，安装方法：[Windows10/11 三步安装wsl2 Ubuntu20.04（任意盘） - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/466001838) 。如需使用其他环境（Mac、Docker等）请自行配置（问GPT）。

gcc和g++（用于编译c程序。需要支持C++17标准的版本。以下演示10版本安装）

```bash
sudo apt update
sudo apt upgrade
sudo apt install gcc-10 g++-10
# 将安装 GCC 10 版本作为可选的 gcc 编译器，并设置它为默认
sudo update-alternatives --config gcc
```

Cmake（用于项目构建。需要3.20.0版本或以上。以下演示3.20.0版本安装）

```bash
# Cmake3.20.0 (Unbuntu22版本会自动安装最新版，但Unbuntu20版本需要手动安装)
mkdir -p ~/download
cd ~/download
sudo apt install libssl-dev -y
wget https://github.com/Kitware/CMake/releases/download/v3.20.0/cmake-3.20.0-linux-x86_64.tar.gz
tar -xzvf cmake-3.20.0-linux-x86_64.tar.gz
# ~/.bashrc中添加：export PATH=$PATH:~/download/cmake-3.20.0-linux-x86_64/bin
source ~/.bashrc
cmake --version
```

Ninja（用于快速运行CMake。默认版本即可）

```bash
sudo apt-get install ninja-build
```

flex和bison（用于编译器的词法分析和语法分析。需要2.6/3.8版本以上。以下演示2.6.4/3.8版本安装）

```bash
# 这里采用自动安装
sudo apt-get install flex bison
flex --version # flex 2.6.4
bison --version # bison (GNU Bison) 3.5.1
# 安装更高版本的bison
mkdir -p ~/download
cd ~/download
wget http://ftp.gnu.org/gnu/bison/bison-3.8.tar.gz
tar -xvzf bison-3.8.tar.gz
cd bison-3.8
sudo apt install m4 build-essential
./configure
make
sudo make install
bison --version # bison (GNU Bison) 3.8
```

clang-14和llvm-14（用于执行.ll文件，同时使用--opaque-pointer。需要14版本或以上。以下演示14版本安装）

```bash
# clang-14和llvm-14 (Unbuntu22版本会自动安装最新版，但Unbuntu20版本需要手动安装)
wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | sudo apt-key add -
sudo add-apt-repository "deb http://apt.llvm.org/focal/ llvm-toolchain-focal-14 main"
sudo apt-get update
sudo apt-get install clang-14
sudo apt-get install llvm-14
clang-14 -v
lli-14 --version
```

交叉编译器（用于将汇编语言转变为arm机器码。祖传的版本）

```bash
mkdir -p ~/download
cd ~/download
# 下载 gcc-arm-8.2-2018.11-x86_64-arm-linux-gnueabihf.tar.xz
wget https://armkeil.blob.core.windows.net/developer/Files/downloads/gnu-a/8.2-2018.11/gcc-arm-8.2-2018.11-x86_64-arm-linux-gnueabihf.tar.xz
tar xf gcc-arm-8.2-2018.11-x86_64-arm-linux-gnueabihf.tar.xz
# ~/.bashrc中添加：export PATH=$PATH:~/download/gcc-arm-8.2-2018.11-x86_64-arm-linux-gnueabihf/bin
source ~/.bashrc
arm-linux-gnueabihf-g++ -v
```

qemu模拟器（用于执行交叉编译出的arm机器码。祖传的版本）

```bash
mkdir -p ~/download
cd ~/download
# 下载 qemu-6.2.0.tar.xz
wget https://download.qemu.org/qemu-6.2.0.tar.xz
tar xf qemu-6.2.0.tar.xz
cd qemu-6.2.0
mkdir build
cd build
sudo apt-get install pkg-config libglib2.0-dev
../configure --target-list=arm-linux-user
ninja
sudo ninja install
qemu-arm --version
```

clang-format（代码格式化。默认版本即可）

```bash
sudo apt-get install clang-format
# 代码格式化的命令如下
find . -name "*.cc" -o -name "*.hh" | xargs clang-format -i
```

zip（用于打包作业。默认版本即可）

```bash
sudo apt-get install zip
```

Vscode插件（按需）

- C/C++ Extension Pack：C/C++开发全家桶
  - 注意 `.vscode/c_cpp_properties.json`文件的编写，有助于识别头文件避免报错。HW1示例

    ```json
    {
        "configurations": [
            {
                "name": "Win32",
                "includePath": [
                    "${workspaceFolder}/build/lib/frontend",
                    "${workspaceFolder}/include/ast",
                    "${workspaceFolder}/include/frontend",
                    "${workspaceFolder}/vendor/tinyxml2"
                ],
                "defines": [
                    "_DEBUG",
                    "UNICODE",
                    "_UNICODE"
                ],
                "windowsSdkVersion": "10.0.17763.0",
                "compilerPath": "D:/Download/VS2022/2017/VC/Tools/MSVC/14.16.27023/bin/Hostx64/x64/cl.exe",
                "cStandard": "c17",
                "cppStandard": "c++17",
                "intelliSenseMode": "windows-msvc-x64"
            }
        ],
        "version": 4
    }
    ```
- CMake：cmake高亮
- Yash：flex/bison高亮
- LLVM Syntax Highlighting：llvm高亮
- Git Graph：git开发可视化
- ...（自己在编码过程中搜索更多方便的插件）

# 项目介绍

本课程始于Tiger编译器（将Tiger编译为x86，使用C+Makefile），每年的课程都有较大的变动：

- 开课-2022：用**FDMJ**（FuDan Mini Java）替代Tiger，用**ARM**替代x86。即将FDMJ编译为ARM
- 2023：新增**LLVM**。即将FDMJ编译为LLVM和ARM
- 2024：用**CMake**重构项目
- 2025（今年）：用**C++**重构项目（lex/yacc→flex/bison；visitor pattern；data structure……）

文档及代码的更新使用git管理，gitee托管（避免翻墙）：https://gitee.com/fudanCompiler/FudanCompilerH2025 【私密仓库，需要联系老师拉入】

我们的编译器仿照gcc三段式的设计

![gcc](img/gcc.png)

- Frontend（前端）：词法分析、语法分析、语义分析、生成中间代码
- Optimizer（优化器）：中间代码优化
- Backend（后端）：生成机器码

我们的项目结构模仿llvm-project（非常著名的一个编译项目）

```tree
.
├── build CMake命令构建产生的文件夹
│   └── ...
├── docs 实验文档、阅读材料、示例代码等
│   └── ...
├── include 头文件
│   └── ...
├── lib 源文件
│   └── ...(与include文件夹对应的文件结构)
│── test FDMJ测试文件
│   └── ...
│── tools 最终用户的应用程序，即main函数文件
│   └── ...
├── vendor 外部链接库
│   └── ...
├── CMakeLists.txt CMake构建文件
└── Makefile 构建、测试、清理命令
```

作业内容与评分：请按照上述项目结构完成编码，并记录实验过程。每次作业满分为100分

- 我们会发布样例测试文件（见test文件夹），都通过将获得30的分数；
- 隐藏测试文件都通过将获得60的分数；
- 实验过程记录满足以下要求则可获得10的分数：
  - 按开发过程的顺序描述用到的参考材料，包括虎书、网站等，并附上描述与链接；
  - 展示关键技术的实现，简要描述即可；
  - 本地的git提交记录，展示开发过程；
  - 测试通过截图。

作业打包与提交：

- 首先，将你的实验过程记录转换为pdf文件，命名为 `report.pdf`，放在 `docs`文件夹下
- 然后，进入作业主文件夹（与 `Makefile`同级），输入命令 `make handin`，然后输入你的 `学号-姓名`，则会在 `docs`文件夹下生成需要提交的 `zip`文件
- 最后，将你的zip文件提交至eLearning相应的作业区

# 背景知识

## gcc和g++

> [编译器 cc、gcc、g++、CC 的区别 - 52php - 博客园 (cnblogs.com)](https://www.cnblogs.com/52php/p/5681725.html)

- gcc 是GNU Compiler Collection，原名为Gun C语言编译器，因为它原本只能处理C语言，但gcc很快地扩展，包含很多编译器（C、C++、Objective-C、Ada、Fortran、Java），可以说gcc是GNU编译器集合。
- g++ 是C++编译器。并不是说gcc只能编译C代码，g++只能编译C++代码，而是两者都可以，只是实现细节上有差别（若有兴趣请见链接）。
- cc 是 Unix系统的 C Compiler，一个是古老的 C 编译器。而 Linux 下 cc 一般是一个符号连接，指向 gcc；可以通过 $ ls -l /usr/bin/cc 来简单察看，该变量是 make 程序的内建变量，默认指向 gcc 。 cc 符号链接和变量存在的意义在于源码的移植性，可以方便的用 gcc 来编译老的用cc编译的Unix软件，甚至连 makefile 都不用改，而且也便于 Linux 程序在 Unix下 编译。
- CC 则一般是 makefile 里面的一个名字标签，即宏定义，表示采用的是什么编译器（如：CC = gcc）。

## clang和llvm

gcc ≈ clang（前端） + llvm（优化器+后端）

> [【编译原理】GCC/Clang/LLVM的区别与联系 - 掘金 (juejin.cn)](https://juejin.cn/post/6946088617617915918)
>
> [深入浅出让你理解什么是LLVM - 简书 (jianshu.com)](https://www.jianshu.com/p/1367dad95445)

![llvm_ir](img/llvm_ir.png)

gcc的前端和后端没分得太开，前端后端耦合在了一起，所以gcc为了支持一门新的语言，或者为了支持一个新的目标平台，就变得特别困难。LLVM (Low Level Virtual Machine) 的出现正是为了解决编译器代码重用的问题，不同的前端后端使用统一的中间代码LLVM IR (LLVM Intermediate Representation)

- 如果需要支持一种新的编程语言，那么只需要实现一个新的前端
- 如果需要支持一种新的硬件设备，那么只需要实现一个新的后端
- 优化阶段是一个通用的阶段，它针对的是统一的LLVM IR，不论是支持新的编程语言，还是支持新的硬件设备，都不需要对优化阶段做修改

## Makefile和CMake

演进：bash脚本 → Makefile → Makefile+CMake

> Makefile：[Makefile教程（绝对经典，所有问题看这一篇足够了）-CSDN博客](https://blog.csdn.net/weixin_38391755/article/details/80380786)
>
> CMake基础：[CMake 良心教程，教你从入门到入魂 - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/500002865)
>
> CMake进阶：[Introduction - 《CMake菜谱（CMake Cookbook中文版）》 - 书栈网 · BookStack](https://www.bookstack.cn/read/CMake-Cookbook/README.md)

![gcc_makefile_cmake](img/gcc_makefile_cmake.png)

- bash脚本：执行.c文件前，首先要把源文件编译成.o中间代码文件（Object File），然后再把大量的.o文件链接成.exe可执行文件。这个过程通过若干gcc命令来完成。一个简单的做法是编写.sh文件，一般来说需要构建 `build.sh`、运行 `run.sh`、清理 `clean.sh`等脚本。
- Makefile
  - 起到汇总脚本的效果，可以看作.sh文件的多合一。
  - 其另一个用处是简化构建过程。当需要编译的东西很多时，需要说明先编译什么，后编译什么，这个过程称为构建。Makefile能自动找到依赖关系，我们就无需按顺序写命令编译文件；并且，Makefile还会根据文件更新的时间以及依赖关系，避免重新编译无需再编译的文件。
- CMake：能进一步简化构建过程，但无法替代Makefile作为.sh文件的多合一，因此通常的做法是Makefile单单将构建过程建委托给CMake。此外，Makefile的构建会将中间文件和代码文件混在一起，而CMake能分离它们。

本项目使用Makefile调用CMake进行构建，测试、清理等命令都集成在Makefile中。

## Visitor Pattern

> [秒懂设计模式之访问者模式（Visitor Pattern） - 知乎](https://zhuanlan.zhihu.com/p/380161731)

Visitor Pattern（访问者模式） 的主要目的是将一些作用于某一数据结构的操作与数据结构本身分离，使得可以在不修改数据结构的前提下，新增不同的操作（开闭原则：一个类应该对扩展开放，对修改封闭）。

该模式有助于不断遍历编译器的抽象语法树（AST） ，进行不同的操作。如果用C的话，每次遍历AST需要写一堆函数递归访问AST的结点【例子：xml2ast】；C++的话，相当于每次遍历都用一个类（Visitor）将相应的递归函数打包起来【例子：ast2xml】。

以下是HW1相关的使用PlantUML生成的UML：

=======
# 环境配置

以下基于 Windows 的 Linux 子系统 WSL ，版本是 Ubuntu 20.04.6 LTS ，安装方法：[Windows10/11 三步安装wsl2 Ubuntu20.04（任意盘） - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/466001838) 。如需使用其他环境（Mac、Docker等）请自行配置（问GPT），满足依赖的版本要求即可。

gcc和g++（用于编译c程序。需要支持C++17标准的版本。以下演示10版本安装）

```bash
sudo apt update
sudo apt upgrade
sudo apt install gcc-10 g++-10
# 将安装 GCC 10 版本作为可选的 gcc 编译器，并设置它为默认
sudo update-alternatives --config gcc
```

Cmake（用于项目构建。需要3.20.0版本或以上。以下演示3.20.0版本安装）

```bash
# Cmake3.20.0 (Unbuntu22版本会自动安装最新版，但Unbuntu20版本需要手动安装)
mkdir -p ~/download
cd ~/download
sudo apt install libssl-dev -y
wget https://github.com/Kitware/CMake/releases/download/v3.20.0/cmake-3.20.0-linux-x86_64.tar.gz
tar -xzvf cmake-3.20.0-linux-x86_64.tar.gz
# ~/.bashrc中添加：export PATH=$PATH:~/download/cmake-3.20.0-linux-x86_64/bin
source ~/.bashrc
cmake --version
```

Ninja（用于快速运行CMake。默认版本即可）

```bash
sudo apt-get install ninja-build
```

flex和bison（用于编译器的词法分析和语法分析。需要2.6/3.8版本以上。以下演示2.6.4/3.8版本安装）

```bash
# 这里采用自动安装
sudo apt-get install flex bison
flex --version # flex 2.6.4
bison --version # bison (GNU Bison) 3.5.1
# 安装更高版本的bison
mkdir -p ~/download
cd ~/download
wget http://ftp.gnu.org/gnu/bison/bison-3.8.tar.gz
tar -xvzf bison-3.8.tar.gz
cd bison-3.8
sudo apt install m4 build-essential
./configure
make
sudo make install
bison --version # bison (GNU Bison) 3.8
```

clang-14和llvm-14（用于执行.ll文件，同时使用--opaque-pointer。需要14版本或以上。以下演示14版本安装）

```bash
# clang-14和llvm-14 (Unbuntu22版本会自动安装最新版，但Unbuntu20版本需要手动安装)
wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | sudo apt-key add -
sudo add-apt-repository "deb http://apt.llvm.org/focal/ llvm-toolchain-focal-14 main"
sudo apt-get update
sudo apt-get install clang-14
sudo apt-get install llvm-14
clang-14 -v
lli-14 --version
```

交叉编译器（用于将汇编语言转变为arm机器码。祖传的版本）

```bash
mkdir -p ~/download
cd ~/download
# 下载 gcc-arm-8.2-2018.11-x86_64-arm-linux-gnueabihf.tar.xz
wget https://armkeil.blob.core.windows.net/developer/Files/downloads/gnu-a/8.2-2018.11/gcc-arm-8.2-2018.11-x86_64-arm-linux-gnueabihf.tar.xz
tar xf gcc-arm-8.2-2018.11-x86_64-arm-linux-gnueabihf.tar.xz
# ~/.bashrc中添加：export PATH=$PATH:~/download/gcc-arm-8.2-2018.11-x86_64-arm-linux-gnueabihf/bin
source ~/.bashrc
arm-linux-gnueabihf-g++ -v
```

qemu模拟器（用于执行交叉编译出的arm机器码。祖传的版本）

```bash
mkdir -p ~/download
cd ~/download
# 下载 qemu-6.2.0.tar.xz
wget https://download.qemu.org/qemu-6.2.0.tar.xz
tar xf qemu-6.2.0.tar.xz
cd qemu-6.2.0
mkdir build
cd build
sudo apt-get install pkg-config libglib2.0-dev
../configure --target-list=arm-linux-user
ninja
sudo ninja install
qemu-arm --version
```

clang-format（代码格式化。默认版本即可）

```bash
sudo apt-get install clang-format
# 代码格式化的命令如下
find . -name "*.cc" -o -name "*.hh" | xargs clang-format -i
```

zip（用于打包作业。默认版本即可）

```bash
sudo apt-get install zip
```

Vscode插件（按需）

- C/C++ Extension Pack：C/C++开发全家桶
  - 注意 `.vscode/c_cpp_properties.json`文件的编写，有助于识别头文件避免报错。HW1示例

    ```json
    {
        "configurations": [
            {
                "name": "Win32",
                "includePath": [
                    "${workspaceFolder}/build/lib/frontend",
                    "${workspaceFolder}/include/ast",
                    "${workspaceFolder}/include/frontend",
                    "${workspaceFolder}/vendor/tinyxml2"
                ],
                "defines": [
                    "_DEBUG",
                    "UNICODE",
                    "_UNICODE"
                ],
                "windowsSdkVersion": "10.0.17763.0",
                "compilerPath": "D:/Download/VS2022/2017/VC/Tools/MSVC/14.16.27023/bin/Hostx64/x64/cl.exe",
                "cStandard": "c17",
                "cppStandard": "c++17",
                "intelliSenseMode": "windows-msvc-x64"
            }
        ],
        "version": 4
    }
    ```
- CMake：cmake高亮
- Yash：flex/bison高亮
- LLVM Syntax Highlighting：llvm高亮
- Git Graph：git开发可视化
- ...（自己在编码过程中搜索更多方便的插件）

# 项目介绍

本课程始于Tiger编译器（将Tiger编译为x86，使用C+Makefile），每年的课程都有较大的变动：

- 开课-2022：用**FDMJ**（FuDan Mini Java）替代Tiger，用**ARM**替代x86。即将FDMJ编译为ARM
- 2023：新增**LLVM**。即将FDMJ编译为LLVM和ARM
- 2024：用**CMake**重构项目
- 2025（今年）：用**C++**重构项目（lex/yacc→flex/bison；visitor pattern；data structure……）

文档及代码的更新使用git管理，gitee托管（避免翻墙）：https://gitee.com/fudanCompiler/FudanCompilerH2025 【私密仓库，需要联系老师拉入】

我们的编译器仿照gcc三段式的设计

![gcc](img/gcc.png)

- Frontend（前端）：词法分析、语法分析、语义分析、生成中间代码
- Optimizer（优化器）：中间代码优化
- Backend（后端）：生成机器码

我们的项目结构模仿llvm-project（非常著名的一个编译项目）

```tree
.
├── build CMake命令构建产生的文件夹
│   └── ...
├── docs 实验文档、阅读材料、示例代码等
│   └── ...
├── include 头文件
│   └── ...
├── lib 源文件
│   └── ...(与include文件夹对应的文件结构)
│── test FDMJ测试文件
│   └── ...
│── tools 最终用户的应用程序，即main函数文件
│   └── ...
├── vendor 外部链接库
│   └── ...
├── CMakeLists.txt CMake构建文件
└── Makefile 构建、测试、清理命令
```

作业内容与评分：请按照上述项目结构完成编码，并记录实验过程。每次作业满分为100分

- 我们会发布样例测试文件（见test文件夹），都通过将获得30的分数；
- 隐藏测试文件都通过将获得60的分数；
- 实验过程记录满足以下要求则可获得10的分数：
  - 按开发过程的顺序描述用到的参考材料，包括虎书、网站等，并附上描述与链接；
  - 展示关键技术的实现，简要描述即可；
  - 本地的git提交记录（截图vscode里的git graph即可），展示开发过程；
  - 测试通过截图。

作业打包与提交：

- 首先，将你的实验过程记录转换为pdf文件，命名为 `report.pdf`，放在 `docs`文件夹下
- 然后，进入作业主文件夹（与 `Makefile`同级），输入命令 `make handin`，然后输入你的 `学号-姓名`，则会在 `docs`文件夹下生成需要提交的 `zip`文件
- 最后，将你的zip文件提交至eLearning相应的作业区

# 背景知识

## gcc和g++

> [编译器 cc、gcc、g++、CC 的区别 - 52php - 博客园 (cnblogs.com)](https://www.cnblogs.com/52php/p/5681725.html)

- gcc 是GNU Compiler Collection，原名为Gun C语言编译器，因为它原本只能处理C语言，但gcc很快地扩展，包含很多编译器（C、C++、Objective-C、Ada、Fortran、Java），可以说gcc是GNU编译器集合。
- g++ 是C++编译器。并不是说gcc只能编译C代码，g++只能编译C++代码，而是两者都可以，只是实现细节上有差别（若有兴趣请见链接）。
- cc 是 Unix系统的 C Compiler，一个是古老的 C 编译器。而 Linux 下 cc 一般是一个符号连接，指向 gcc；可以通过 $ ls -l /usr/bin/cc 来简单察看，该变量是 make 程序的内建变量，默认指向 gcc 。 cc 符号链接和变量存在的意义在于源码的移植性，可以方便的用 gcc 来编译老的用cc编译的Unix软件，甚至连 makefile 都不用改，而且也便于 Linux 程序在 Unix下 编译。
- CC 则一般是 makefile 里面的一个名字标签，即宏定义，表示采用的是什么编译器（如：CC = gcc）。

## clang和llvm

gcc ≈ clang（前端） + llvm（优化器+后端）

> [【编译原理】GCC/Clang/LLVM的区别与联系 - 掘金 (juejin.cn)](https://juejin.cn/post/6946088617617915918)
>
> [深入浅出让你理解什么是LLVM - 简书 (jianshu.com)](https://www.jianshu.com/p/1367dad95445)

![llvm_ir](img/llvm_ir.png)

gcc的前端和后端没分得太开，前端后端耦合在了一起，所以gcc为了支持一门新的语言，或者为了支持一个新的目标平台，就变得特别困难。LLVM (Low Level Virtual Machine) 的出现正是为了解决编译器代码重用的问题，不同的前端后端使用统一的中间代码LLVM IR (LLVM Intermediate Representation)

- 如果需要支持一种新的编程语言，那么只需要实现一个新的前端
- 如果需要支持一种新的硬件设备，那么只需要实现一个新的后端
- 优化阶段是一个通用的阶段，它针对的是统一的LLVM IR，不论是支持新的编程语言，还是支持新的硬件设备，都不需要对优化阶段做修改

## Makefile和CMake

演进：bash脚本 → Makefile → Makefile+CMake

> Makefile：[Makefile教程（绝对经典，所有问题看这一篇足够了）-CSDN博客](https://blog.csdn.net/weixin_38391755/article/details/80380786)
>
> CMake基础：[CMake 良心教程，教你从入门到入魂 - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/500002865)
>
> CMake进阶：[Introduction - 《CMake菜谱（CMake Cookbook中文版）》 - 书栈网 · BookStack](https://www.bookstack.cn/read/CMake-Cookbook/README.md)

![gcc_makefile_cmake](img/gcc_makefile_cmake.png)

- bash脚本：执行.c文件前，首先要把源文件编译成.o中间代码文件（Object File），然后再把大量的.o文件链接成.exe可执行文件。这个过程通过若干gcc命令来完成。一个简单的做法是编写.sh文件，一般来说需要构建 `build.sh`、运行 `run.sh`、清理 `clean.sh`等脚本。
- Makefile
  - 起到汇总脚本的效果，可以看作.sh文件的多合一。
  - 其另一个用处是简化构建过程。当需要编译的东西很多时，需要说明先编译什么，后编译什么，这个过程称为构建。Makefile能自动找到依赖关系，我们就无需按顺序写命令编译文件；并且，Makefile还会根据文件更新的时间以及依赖关系，避免重新编译无需再编译的文件。
- CMake：能进一步简化构建过程，但无法替代Makefile作为.sh文件的多合一，因此通常的做法是Makefile单单将构建过程建委托给CMake。此外，Makefile的构建会将中间文件和代码文件混在一起，而CMake能分离它们。

本项目使用Makefile调用CMake进行构建，测试、清理等命令都集成在Makefile中。

## Visitor Pattern

> [秒懂设计模式之访问者模式（Visitor Pattern） - 知乎](https://zhuanlan.zhihu.com/p/380161731)

Visitor Pattern（访问者模式） 的主要目的是将一些作用于某一数据结构的操作与数据结构本身分离，使得可以在不修改数据结构的前提下，新增不同的操作（开闭原则：一个类应该对扩展开放，对修改封闭）。

该模式有助于不断遍历编译器的抽象语法树（AST） ，进行不同的操作。如果用C的话，每次遍历AST需要写一堆函数递归访问AST的结点【例子：xml2ast】；C++的话，相当于每次遍历都用一个类（Visitor）将相应的递归函数打包起来【例子：ast2xml】。

以下是HW1相关的使用PlantUML生成的UML：

>>>>>>> 84ebd885f67b30753fb454f9366f73be42506149
![ast_uml](img/ast_uml.png)