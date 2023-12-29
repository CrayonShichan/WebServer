# Linux系统编程入门

## Linux开发环境搭建

### 1. 环境搭建

#### 1.1 Linux系统（虚拟机安装、云服务器）

安装 openssh-server

```bash
sudo apt update
sudo apt install openssh-server

service ssh start # 启动openssh-server
ps -e |grep sshd			#查看ssh服务是否启动  -e查看全部进程
service ssh status                      #查看ssh运行状态
```

安装net-tools

```shell
sudo apt install net-tools
```

安装编译调试工具

```shell
# 编译器
sudo apt install gcc g++
# 调试器
sudo apt install gdb
```

#### 1.2 XSHELL、SFTP

用于远程登陆服务器。

下载地址：https://www.xshell.com/zh/xshell-download/

#### 1.3 VSCode

安装远程登录服务器插件：**Remote Development**。选择 SSH Targets 配置连接。

**config：**

```shell
Host 主机名 # 随意取
	HostName 服务器IP地址
	User 用户名称
```

### 2. VScode 免密登录



#### 2.1 生成证书

**在本机生成SSH证书：**

```shell
ssh-keygen -t rsa
```

从而在指定目录下得到 `id_rsa、id_rsa.pub` 分别为私钥和公钥。

生成密钥之后，可以直接通过以下命令将密钥发送给虚拟机：

```bash
ssh-copy-id
```

如果失败，则继续下面的步骤，通过另一种方式配置。

**在 Linux 生成SSH证书：**

```shell
ssh-keygen -t rsa
```

从而在 **.ssh** 目录下得到 `id_rsa、id_rsa.pub` 分别为私钥和公钥。

#### 2.2 配置免密登陆

编辑虚拟机的 **authorized_keys** 配置文件

```shell
cd .ssh
vim authorized_keys
```

将本机的ssh公钥复制粘贴到其中，即可完成免密配置。

### 3. 使用技巧

**后台运行进程：**

```c
./test &
```

进程切换到后台运行：`./test &`，切换到后台后，当前终端可以使用其他命令，此时无法通过`CTRL C`终止

后台进程切换到前台：`fg`，切换后，可以通过`CTRL C`终止

## GCC

### 1. 什么是GCC

**GCC**（GNU Compiler Collection , GNU编译器套件）是由GNU开发的编程语言编译器。 GCC最初代表 "GNU C Compiler"，当时只支持C语言。 后来又扩展能够支持更多编程语言，包括 C++、Fortran 和 Java 等。 因此，GCC也被重新定义为“GNU Compiler Collection”。

GCC 可以使用命令行选项来控制编译器在翻译源代码时应该遵循那个 C 标准。 

``` c++
'-std=c99' // 启动GCC时，编译器支持C99标准
```

**安装命令：**`sudo apt install gcc g++` (版本 > 4.8.5)

**查看版本：** `gcc/g++ -v/--version`

**编译文件：**

```bash
gcc test.c -o app # 生成可执行文件app
gcc test.c   # 生成可执行文件a.out
```

选项-o用于指定要生成的目标文件，后面跟的就是结果目标名字。

**运行文件：**

```bash
./app
./a.out
```

### 2. GCC工作流程

![](D:/work/c++/Linux服务器开发/Linux系统编程入门/image/1.png)

GCC常用参数选项：

| gcc编译选项                              | 说明                                                         | 示例                           |
| ---------------------------------------- | ------------------------------------------------------------ | ------------------------------ |
| -E                                       | **预处理指定的源文件，不进行编译**，生成.i文件               | gcc -E test.c -o test.i        |
| -S                                       | **编译指定的源文件，但是不进行汇编**，生成.s文件             | gcc test.i -S -o test.s        |
| -c                                       | **编译、汇编但是不连接**，**生成.o文件**                     | gcc -c test1.c test2.c test3.c |
| -o [file1] [file2]  / [file2] -o [file1] | 将文件 file2 编译成可执行文件 file1                          |                                |
| -I Directory                             | 指定 include 包含的搜索目录                                  |                                |
| -g                                       | 在编译时，生成调试信息，该程序可以被调试器调试               |                                |
| -D                                       | 在程序编译时，指定一个宏                                     |                                |
| -w                                       | 不生成任何警告信息                                           |                                |
| -Wall                                    | 生成所有的警告                                               |                                |
| -On                                      | n的取值范围：0~3。编译器优化选项的四个级别，-O0表示没有优化，-O1为缺省值，-O3优化级别最高 |                                |
| -l                                       | 在程序编译的时候，指定使用库                                 |                                |
| -L                                       | 指定编译的时候，搜索的库的路径                               |                                |
| -fPIC/fpic                               | 生成与位置无关的代码                                         |                                |

### 3. GCC 和 G++ 的区别

* gcc 与 g++ 都可以编译 c 代码与 c++ 代码。但是：后缀为 .c 的，gcc 把它当做 C 程序，而 g++ 当做是 C++ 程序；后缀为 .cpp 的，两者都会认为是 C++ 程序。

* 编译阶段，g++ 会调用 gcc，对于 c++ 代码，两者是等价的，但是因为gcc命令不能自动和 C++ 程序使用的库联接，所以通常用g++来完成链接。

* 编译可以用 gcc/g++，而链接可以用 `g++` 或者 `gcc -lstdc++` 。因为 gcc 命令不能自动和 C++ 程序使用的库联接，所以通常使用 g++ 来完成联接。但在编译阶段，g++ 会自动调用 gcc ，二者等价。

## 静态库/动态库的创建和使用

### 1. 库

* 库文件是计算机上的一类文件，可以简单的把库文件看成一种代码仓库，他提供使用者一些可以直接拿来用的变量、函数或类。

* 库是一种特殊的程序，只是库不能单独运行。
* 库有两种，静态库和动态库（共享库）。

**静态库和动态库的区别：**

`静态库在程序的链接阶段被复制到了程序当中；动态库在链接阶段没有复制到程序中，而是程序在运行时由系统动态加载到内存中供程序调用。`

**库的好处：**

1. 代码保密
2. 方便部署和分发。

### 2. 工作原理

**静态库：**GCC进行链接时，会把静态库中代码打包到可执行程序中

**动态库：**GCC 进行链接时，动态库的代码不会被打包到可执行程序中。程序启动后，动态库会被动态加载到内存中，通过 **ldd** (list dynamic dependencies) 命令检查动态库依赖关系

```bash
ldd main # main 为可执行文件
```

**定位动态库（共享库）文件：**

当系统加载可执行代码时，能够知道其所依赖的库的名字，但是还需要知道绝对路径。此时就需要系统的**动态载入器（ld-linux-x86-64.so.2）**来获取其该绝对路径。对于 elf 格式的可执行程序，是由 **ld-linux.so** 来完成的，它先后搜索 elf 文件的 **DT_RPATH段 ---->  环境变量 LD_LIBRARY_PATH ---> / etc/ld.so.cache 文件列表 ---> /lib/，/usr/lib** 目录，找到库文件后将其载入内存。

### 3. 静态库的制作

**命名规则：**

* Linux：`libxxx.a`
  * lib：前缀（固定）
  * xxx：库的名字，自己决定
  * .a：后缀（固定）
* Windows：`libxxx.lib`

**静态库的制作：**

1. 通过 gcc 获得 .o 文件

   ```bash
   gcc -c xxx1.c xxx2.c
   ```

2. 将 .o 文件打包。使用 ar 工具 （archive）

   ```bash
   ar rcs libxxx.a xxx1.o xxx2.o
   ```

   r — 将文件插入备存文件中

   c — 建立备存文件

   r — 索引

### 4. 静态库的使用

[参考链接：](https://blog.csdn.net/u013632755/article/details/108301510)  https://blog.csdn.net/u013632755/article/details/108301510

了使用静态库 mymath，我们在相同文件夹下，创建一个 main.c 文件:

```bash
– add.c # 加法实现
– add.h # 加法头文件
– sub.c # 减法实现
– sub.h # 减法头文件
– libmymath.a # 静态库文件
– main.c # 入口主程序
```

使用#include预处理指令包含了 sub.h 和 add.h 头文件，来使用我们制作的 mymath 静态库。

如果直接使用：

```shell
gcc main.c -o main 
```

肯定会出错，因为编译器不知道 add.h 和 sub.h 是哪里来的，也不知道add和sub函数的定义在哪里。所以我们需要使用`-l`选项，让编译器链接到我们制作的静态库:

```bash
gcc main.c -o main -lmymath 
```

执行上面的命令发现还是会报错，是`因为编译器默认查找的/usr/lib;/usr/local/lib系统文件夹中的库。而非我们的制作的静态库存放地址，`除非将你制作的 `libmymath.a` 放到上面的lib文件夹中，那么执行上面的命令就不会有问题。如果不在上述文件夹中，则需要使用`-L`来指定附加库文件搜索路径。

此处我的libmymath.a在当前目录，所以执行:

```bash
gcc main.c -o main -lmymath -L./  
```

> 注意：gcc的`-l`选项后面跟的库名，只需要指定除去lib和.a后缀的中间的实际命名就可以了，编译器会自动按照`lib<库名>.a`规则来搜索库文件。
> 如果预处理指令#include包含的头文件不在当前目录下，则需要在gcc的时候，通过`-I`选项来指定头文件附加搜索路径。
>
> ```bash
> – add.c # 加法实现
> – sub.c # 减法实现
> – include # 文件夹
> 	– add.h # 加法头文件
> 	– sub.h # 减法头文件
> – libmymath.a # 静态库文件
> – main.c # 入口主程序
> # 执行：
> gcc main.c -o main -I ./include/ -lmymath -L./ 
> ```

静态库如果被删除，不会影响可执行文件的执行。因为静态库的代码被复制到了可执行文件中，作为了可执行文件的一部分，这也是静态库和动态库的部分区别所在。

### 5. 动态库的制作

**命名规则：**

* Linux：`libxxx.so`
  * lib：前缀（固定）
  * xxx：库的名字，自己决定
  * .a：后缀（固定）
* Windows：`libxxx.dll`

**动态库的制作：**

1. 通过 gcc 获得 .o 文件，得到和位置无关的代码

   ```bash
   gcc -c -fpic/fPIC xxx1.c xxx2.c
   ```

2. gcc 得到动态库

   ```shell
   gcc -shared xxx1.o xxx2.o -o libxxx.so
   ```

### 6. 动态库的使用

使用原理基本与静态库的使用一致。

当直接运行时，会出现如下错误：

```bash
./dymain: error while loading shared libraries: libdymath.so: cannot open shared object file: No such file or directory
# dymain 为使用了动态库的可执行文件
# dymath 为自己创建的动态库
```

使用 ldd 查看动态库依赖关系时发现：

```bash
libdymath.so => not found
```

**解决动态库加载失败：**

1. 配置环境变量

**1.1 在终端中配置自己动态库的环境变量（临时配置）：**

```shell
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/user/Linux/lesson02/lib
# $LD_LIBRARY_PATH 获取之前的环境变量，每个路径之间以 : 隔开
# /home/user/Linux/lesson02/lib 动态库所在目录

# 查看环境变量：
echo $LD_LIBRARY_PATH
```

使用 ldd 查看动态库依赖关系时发现：

```bash
libdymath.so (0x00007f2bd5bdf000)
```

但此方法为在终端中临时配置，关闭终端后就会失效。

**1.2 用户级配置（永久）：**

home/user 目录下存在  `.bashrc` 文件，只需要将上面配置的环境变量加入其中。 

```bash
vim .bashrc

# 添加
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/user/Linux/lesson02/lib
```

为了使配置的环境变量生效，还需运行如下面命令：

```bash
. .bashrc / source .bashrc # source 与 . 等价
```

**1.3  系统级配置（永久）：**

在 /etc/profile 中添加环境变量

```bash
sudo vim /etc/profile

# 添加
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/user/Linux/lesson02/lib
```

为了使配置的环境变量生效，还需运行如下面命令：

```bash
. .bashrc / source .bashrc # source 与 . 等价
```

2. 配置  /etc/ld.so.cache 文件列表

 `/etc/ld.so.cache` 为二进制文件，不能直接配置，需要通过 **/etc/ld.so.conf** 文件间接配置。

```bash
sudo vim  /etc/ld.so.conf

# 添加
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/user/Linux/lesson02/lib
```

更新 `/etc/ld.so.cache`:

```bash
sudo ldconfig
```

3. 直接将动态库放到 `/lib` 或 `/usr/lib` 目录下 （不推荐）

可能会误替换已经存在的同名的系统文件。

### 7. 静态库和动态库的对比

1. 程序编译成可执行程序的过程

![](D:/work/c++/Linux服务器开发/Linux系统编程入门/image/2.png)

2. 静态库的制作过程

![](D:/work/c++/Linux服务器开发/Linux系统编程入门/image/3.png)

3. 动态库的制作过程

![](D:/work/c++/Linux服务器开发/Linux系统编程入门/image/4.png)

4. 静态库的优缺点

* 优点：
  * 静态库被打包到应用程序中加载速度快
  * 发布程序无需提供静态库，移植方便

* 缺点：
  * 消耗系统资源，浪费内存
  * 更新、部署、发布麻烦

5. 动态库的优缺点

* 优点：
  * 可以实现进程间资源共享（共享库）
  * 更新、部署、发布简单
  * 可以控制何时加载动态库

* 缺点：
  * 加载速度比静态库慢
  * 发布程序时需要提供依赖的动态库

## GDB 调试

### 1、 什么是GDB

GNU 软件系统社区提供的调试工具，同 GCC 配套组成了一套完整的开发环 境，GDB 是 Linux 和许多类 Unix 系统中的标准开发环境。

**功能：**

1.  启动程序，可以按照自定义的要求随心所欲的运行程序 
2.  可让被调试的程序在所指定的调置的断点处停住（断点可以是条件表达式） 
3.  当程序被停住时，可以检查此时程序中所发生的事 
4.  可以改变程序，将一个 BUG 产生的影响修正从而测试其他 BUG

### 2. 准备工作

* 在为调试而编译时，我们会关掉编译器的优化选项（`-O`）， 并打开调 试选项（`-g`）。另外，`-Wall`在尽量不影响程序行为的情况下选项打开所有 warning，也可以发现许多问题，避免一些不必要的 BUG。

```shell
gcc -g -Wall program.c -o program
```

* `-g` 选项的作用是在可执行文件中加入源代码的信息，比如可执行文件中第几条机 器指令对应源代码的第几行，但并不是把整个源文件嵌入到可执行文件中，所以在调 试时必须保证 gdb 能找到源文件。

### 3. GDB命令 --- 启动、退出、查看代码

**启动和退出：**

```shell
gdb 可执行程序
quit
```

**给程序设置参数/获取设置参数**

```shell
set args 10 20
show args
```

**查看当前文件代码：**

```shell
list/l （从默认位置显示）
list/l 行号 （从指定的行显示）
list/l 函数名（从指定的函数显示）
```

**查看非当前文件代码：**

```shell
list/l 文件名:行号
list/l 文件名:函数名
```

**设置显示行数**

```shell
show list/listsize
set list/listsize 行数
```

**GDB 使用帮助：**

```shell
help
```

### 4. GDB命令--断点操作

**设置断点：**

```c++
b/break 行号
b/break 函数名
b/break 文件名：行号
b/break 文件名：函数
```

**查看断点：**

```shell
i/info b/break
```

**删除断点：**

```shell
d/del/delte 断点编号
```

**设置断点无效：**

```shell
dis/disable 断点编号
```

**设置断点生效：**

```shell
ena/enable 断点编号
```

**设置条件断点：**

```shell
b/break 10 if i == 5
```

### 5. GDB命令--调试命令

**运行GDB程序：**

```shell
start（程序停在第一行）
run（遇到断点才停）
```

**继续运行，直到下一个断点停：**

```shell
c/continue
```

**向下执行一行代码（不会进入函数体）：**

```shell
n/next
```

**变量操作：**

```shell
p/print 变量名（打印变量值）
ptype 变量名（打印变量类型）
```

**向下单步调试（遇到函数体进入函数体）：**

```
s/step
finish（跳出函数体）
```

**自动变量操作：**

```
display 变量名（自动打印指定变量的值）
i/info display
undisplay 编号
```

**其他操作：**

```
set var 变量名=变量值 （循环中用的较多）
until （跳出循环）
```

## Makefile



### 1. 什么是Makefile

一个工程中的源文件不计其数，按其类型、功能、模块分别放在若干个目录中。

**Makefile**文件定义了一系列的规则来指定哪些文件需要先编译，哪些文件需要后编译，哪些文件需要重新编译，甚至于进行更加复杂的功能操作，因为 Makefile 文件就像一个 shell 脚本一样，也可以执行操作系统的命令。

Makefile 带来的好处就是 **“自动化编译**”，一旦写好，只需要一个 make 命令，整个工程完全自动编译，极大提高了软件开发的效率。`make 是一个命令工具，是一个解释Makefile 文件中指定的命令工具`。

### 2. Makefile 文件命名和规则

**文件命名：**`makefile` 或 `Makefile`

**Makefile 规则：**

* 一个 Makefile 文件中可以有一个或多个规则。

  > `目标...：依赖...`
  > 	    `命令（Shell命令）`
  > 		 `...`

  * 目标：最终要生成的文件（伪目标除外）
  * 依赖：生成目标所需要的文件或是目标
  * 命令：通过执行命令对依赖操作生成目标（命令前必须 Tab 缩进）

* `Makefile 中的其他规则一般是为第一条规则服务的。如果下面的其他规则跟第一条规则没有关联，则默认不会执行`

### 3. 工作原理

* 命令在执行之前，需要先检查规则中的依赖是否存在

  * 如果存在，执行命令

    ```shell
    app: add.c sub.c main.c
    	gcc sub.c add.c main.c -o app
    ```

  * 如果不存在，向下检查其它的规则，检查有没有一个规则是用来生成这个依赖的，如果找到了，则执行该规则中的命令

    ```shell
    app: add.o sub.o main.o
    	gcc sub.o add.o main.o -o app
    sub.o: sub.c
    	gcc -c sub.c -o sub.o
    add.o: add.c
    	gcc -c add.c -o add.o
    main.o: main.c
    	gcc -c main.c -o main.o
    ```

    

* 检测更新，在执行规则的命令时，会比较目标和依赖文件的时间

  * 如果依赖的时间比目标的时间晚，则需要重新生成目标
  * 如果依赖的时间比目标的时间早，目标不需要更新，对应规则中的命令不需要被执行。

### 4. 变量

**自定义变量：**

​		变量名 = 变量值   var = hello

**预定义变量：**

​		AR：归档维护程序的名称，默认值为 ar

​		CC：c 编译器的名称，默认值为 cc

​		CXX：C++ 编译器的名称，默认值为 g++

​		$@：目标的完整名称

​		$< ：第一个依赖文件的名称

​		$^：所有的依赖文件

**获取变量的值：**

​		$(变量名)

示例：

```shell
app: main.c a.c b.c
	gcc main.c a.c b.c -o app
	
app: main.c a.c b.c
	$(CC) $^ -o $@
```

```shell
# 定义变量
src = sub.o add.o main.o
target = app

$(target): $(src)
	$(CC) $(src) -o $(target)
sub.o: sub.c
	gcc -c sub.c -o sub.o
add.o: add.c
	gcc -c add.c -o add.o
main.o: main.c
	gcc -c main.c -o main.o

```

### 5. 通配符

```shell
%.o: %.c
```

​		`		-%: 通配符，匹配同一个字符串`

​		`		-两个%匹配的是同一个字符串`

```
%.o: %.c
	gcc -c $< -o $@
```

```shell
# 定义变量
src = sub.o add.o main.o
target = app

$(target): $(src)
	$(CC) $(src) -o $(target)
	
%.o: %.c
	$(CC) -c $< -o $@
```

### 6. 函数

* $(wildcard PATTERN ... )

  * 功能：获取指定目录下指定类型的文件列表
  * 参数：PATTERN 指的是某个或多个目录下的对应的某种类型的文件，如果有多个目录。一般使用空格间隔
  * 返回：得到的若干个文件的文件列表，文件名之间使用空格隔开

  示例：

  ```c++
  $(wildcard *.c ./sub/*.c)
  返回值格式：a.c b.c c.c d.c e.c f.c
  ```

* $(patsubst <pattern>, <replacement>, <text>)

  * 功能：查找 <text> 中的单词（单词以”空格“、”Tab“  或 "回车"、”换行“ 分隔）是否符合模式 <pattern>，如果匹配的话，则以 <replacement> 替换
  * <pattern> 可以包括通配符 ‘%’，表示任意长度的字串。如果 <replacement> 中包含 ‘%’，那么，<replacement>中的这个 '%' 将是<pattern> 中的那个 % 所代表的字串。（可以用 '\\'来转义，以 '\\'来表示真实含义的'%'字符）
  * 返回：函数返回被替换过后的字符串

  示例：

  ```
  $(patsubst %.c, %.o, x.c bar.c)
  返回值格式：x.o bar.o
  ```

  ```shell
  # 定义变量
  src = $(wildcard ./*.c)
  objs=$(patsubst %.c, %.o, $(src))
  target=app
  $(target): $(objs)
  	$(CC) $(objs) -o $(target)
  
  %.o: %.c
  	$(CC) -c $< -o $@
  
  # 伪目标，不会生成目标文件	
  .PHONY:clean
  clean:
  	rm $(objs) -f
  ```

  

## 文件IO

### 1. 标准C库IO函数

> 在`Linux`中使用`man 2 API名`查看**Linux系统API**，`man 3 API名`查看**标准C库API**

![](D:/work/c++/Linux服务器开发/Linux系统编程入门/image/5.png)

### 2. 标准C库IO和Linux系统IO的关系

![](D:/work/c++/Linux服务器开发/Linux系统编程入门/image/6.png)

### 3. 虚拟地址空间

![](D:/work/c++/Linux服务器开发/Linux系统编程入门/image/7.png)

- 虚拟地址空间是为了解决内存加载问题
  - 问题1：假设实际内存为`4G`，此时共有`1G`、`2G`、`2G`三个程序，如果直接加载，那么第三个程序由于内存不足而无法执行
  - 问题2：当问题1的`1G`程序执行完后，释放内存，第三个程序可以执行，但此时内存空间不连续
- 对于32位机器来说，大小约为$2^{32}$，即`4G`左右，对于64位机器来说，，大小约为$2^{48}$，即`256T`左右
- 通过`CPU中的MMU(内存管理单元)`将虚拟内存地址映射到物理内存地址上

### 4. 文件描述符

![](D:/work/c++/Linux服务器开发/Linux系统编程入门/image/8.png)

- 文件描述符表是一个**数组**，为了一个进程能够同时操作多个文件
- 文件描述符表默认大小：1024

### 5. Linux系统IO函数

#### 5.1 open + close

> open 包含在<fcntl.h>头文件中
>
> flags 定义了某些宏在 <sys/types.h>  <sys/stat.h>
>
> c 中没有函数重载，同名函数运用了可变参数

- `int open(const char *pathname, int flags);`，使用`man 2 open`查看帮助
  - 参数
    - `pathname`：要打开的文件路径
    - `flags`：对文件的操作权限设置还有其他的设置(`O_RDONLY,` `O_WRONLY,` `O_RDWR` 这三个设置是互斥的，代表只读，只写，可读可写)
  - 返回值：返回一个新的文件描述符，如果调用失败，返回-1，并设置`errno`，`errno`属于Linux系统函数库里面的一个全局变量，记录的是最近的错误号

```c
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include<unistd.h>

int main() {
    int fd = open("a.txt", O_RDONLY);

    if(fd == -1) {
        perror("open"); // 在<stdio.h> 头文件中
    }

    // 关闭
    close(fd);
    return 0;
}
```

`int open(const char *pathname, int flags, mode_t mode);`

- 参数

  - `pathname`：要创建的文件的路径
  - flags：对文件的操作权限和其他的设置
    - 必选项：`O_RDONLY`, `O_WRONLY`, `O_RDWR` 这三个之间是互斥的
    - 可选项：`O_CREAT` 文件不存在，创建新文件
    - `flags`参数是一个int类型的数据，占4个字节，32位，每一位就是一个标志位，所以用 `|` 可以保证能够实现多个操作
  - `mode`：八进制的数，表示创建出的新的文件的操作权限，比如：0775

  `创建的文件的最终的权限： mode & ~umask。umask的作用就是抹去某些权限`

```c
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include<unistd.h>
int main() {

    // 创建一个新文件
    int fd = open("create.txt", O_RDWR | O_CREAT, 0777);
    /*
         最终的权限是：mode & ~umask
            0777   ->   111111111
        &   0775   ->   111111101
        ----------------------------
                        111111101
        按位与：0和任何数都为0
    */
    if(fd == -1)
        perror("open");

    close(fd);
    return 0;
}
```

#### 5.2 read + write

- `ssize_t read(int fd, void *buf, size_t count);`
  - 参数
    - `fd`：文件描述符，open得到的，通过这个文件描述符操作某个文件
    - `buf`：需要读取数据存放的地方，数组的地址（传出参数）
    - `count`：指定的数组的大小
  - 返回值
    - 成功
      - `> 0`: 返回实际的读取到的字节数
      - `= 0`：文件已经读取完了
    - 失败：-1
- `ssize_t write(int fd, const void *buf, size_t count);`
  - 参数
    - `fd`：文件描述符，open得到的，通过这个文件描述符操作某个文件
    - `buf`：要往磁盘写入的数据
    - `count`：要写的数据的实际的大小
  - 返回值
    - 成功：实际写入的字节数
    - 失败：返回-1，并设置`errno`

```c
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include<unistd.h>
#define BUFFSIZE 1024
int main() {
    // 通过open打开english.txt
    int srcfd = open("english.txt", O_RDONLY);
    if(srcfd == -1) {
        perror("openFile");
        return -1;
    }
    // 创建一个新的文件（拷贝文件）
    int desfd = open("cpy.txt", O_WRONLY | O_CREAT, 0664);
    if(desfd == -1) {
        perror("createFile");
        return -1;
    }
    // 频繁的读写操作
    char buf[BUFFSIZE] = {0};
    int len  = 0;
    while((len = read(srcfd, buf, BUFFSIZE)) > 0) {
        write(desfd, buf, len);
    }
     // 关闭文件
    close(srcfd);
    close(desfd);

   return 0;
}
```

#### 5.3 lseek

`off_t lseek(int fd, off_t offset, int whence);`

* 参数

  * \- fd 文件描述符 opne得到的，通过这个文件描述符操作某一个文件
  * \- offset 偏移量
  * \- whence    

  ```
  SEEK_SET 文件的开头
  SEEK_CUR 文件指针当前的位置
  SEEK_END 文件结尾
  ```

* 返回值：文件指针的位置

**作用：**

1. 移动文件指针到头文件

```c
lseek(fd, 0, SEEK_SET);
```

2. 获取当前文件指针的位置

​    lseek(fd, 0, SEEK_CUAR)

```c
lseek(fd, 0, SEEK_CUAR)
```

3. 获取文件长度

```c
lseek(fd, 0, SEEK_END)
```

4. 拓展文件的长度，当前文件按10b，110b,增加100个字节（从逻辑上移动指针的位置）

```c
lseek(fd, 100, SEEK_END)
```

```c
#include <sys/types.h>
#include<sys/stat.h>
#include<stdio.h>
#include<fcntl.h>
#include <unistd.h>

int main() {
    int fd = open("hello.txt", O_RDWR);
    if(fd == -1)
    {
        perror("open");
        return -1;
    }

    // 拓展文件的长度
    int ret = lseek(fd, 100, SEEK_END);
    if(ret == -1) {
        perror("lseek");
        return -1;
    }
    write(fd, "h", 1);
    close(fd);
    return 0;
}
```

#### 5.4 stat + lstat

**stat 结构体：**

```c
struct stat {
    dev_t st_dev; // 文件的设备编号
    ino_t st_ino; // 节点
    mode_t st_mode; // 文件的类型和存取的权限
    nlink_t st_nlink; // 连到该文件的硬连接数目
    uid_t st_uid; // 用户ID
    gid_t st_gid; // 组ID
    dev_t st_rdev; // 设备文件的设备编号
    off_t st_size; // 文件字节数(文件大小)
    blksize_t st_blksize; // 块大小
    blkcnt_t st_blocks; // 块数
    time_t st_atime; // 最后一次访问时间
    time_t st_mtime; // 最后一次修改时间
    time_t st_ctime; // 最后一次改变时间(指属性)
};
```

**st_mode:**

![](D:/work/c++/Linux服务器开发/Linux系统编程入门/image/9.png)****



`int stat(const char *pathname, struct stat *statbuf)`

**作用：**`获取文件相关的一些信息`

*   参数：
    * pathname 操作的文件的路径
    * statbuf 结构体变量，`传出参数`， 获取保存到的文件信息

*   返回：成功 -0，失败 -1 设置errno



```c
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<stdio.h>
int main() {
    struct stat buf;
    int ret = stat("a.txt", &buf);
    if(ret == -1) {
        perror("stat");
        return -1;
    }
    printf("%ld\n",  buf.st_size);

    return 0;
}
```

`int lstat(const char *pathname, struct stat *statbuf);`

  **作用：**获取软链接指向的文件的信息

**模拟实现 ls -l**

```c
// 模拟实现 ls -l 指令
// -rw-rw-r-- 1 user user 11 Nov 20 22:10 a.txt

#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<stdio.h>
#include<pwd.h>
#include<grp.h>
#include<time.h>
#include <string.h>

int main(int argc, char * argv[]) {
    // 判断输入的参数是否正确
    if(argc < 2) {
        printf("%s filename\n", argv[0]);
        return -1;
    }
    struct stat st;
    int ret = stat(argv[1], &st);
    if(ret == -1) {
        perror("stat");
        return;
    }
    // 获取文件类型和文件权限
    char perms[11] = {0};
    switch(st.st_mode & __S_IFMT) {
        case __S_IFLNK:
            perms[0] = 'l';
            break;
        case __S_IFDIR:
            perms[0] = 'd';
            break;
        case __S_IFREG:
            perms[0] = '-';
            break;
        case __S_IFBLK:
            perms[0] = 'b';
            break;
        case __S_IFCHR:
            perms[0] = 'c';
            break;
        case __S_IFSOCK:
            perms[0] = 's';
            break;
        case __S_IFIFO:
            perms[0] = 't';
            break;
        default:
            perms[0] = '?';
            break;
    }
    // 文件的访问权限
    // 文件所有者
    perms[1] = (st.st_mode & S_IRUSR) ? 'r' : '-';
    perms[2] = (st.st_mode & S_IWUSR) ? 'w' : '-';
    perms[3] = (st.st_mode & S_IXUSR) ? 'x' : '-';

    // 文件所在组
    perms[4] = (st.st_mode & S_IRGRP) ? 'r' : '-';
    perms[5] = (st.st_mode & S_IWGRP) ? 'w' : '-';
    perms[6] = (st.st_mode & S_IXGRP) ? 'x' : '-';

    // 其他组
    perms[7] = (st.st_mode & S_IROTH) ? 'r' : '-';
    perms[8] = (st.st_mode & S_IWOTH) ? 'w' : '-';
    perms[9] = (st.st_mode & S_IXOTH) ? 'x' : '-';

    // 硬链接数
    int linkNum = st.st_nlink;

    // 文件所有者
    char *fileUser = getpwuid(st.st_uid)->pw_name;

    // 文件所在组
    char *fileGrp = getgrgid(st.st_gid)->gr_name;

    // 文件大小
    long int fileSize = st.st_size;

    // 获取修改的时间
    char *time = ctime(&st.st_mtime);

    char *mtime[512] = {0};
    strncpy(mtime, time, strlen(time) - 1);

    char buf[1024];
    sprintf(buf, "%s %d %s %s %ld %s %s", perms, linkNum, fileUser, fileGrp, fileSize, mtime, argv[1]);
    printf("%s\n", buf);
    
    return 0;
}
```

### 6. 文件属性操作函数

#### 6.1 access

`int access(const char *pathname, int mode);`

  **作用：**判断某个文件是否有某个权限，或者判断文件是否存在

* 参数:
  * pathname 判断文件的路径
  * mode:
    *  R_OK 判断是否有读权限
    *  W_OK 判断是否有写权限
    *  X_OK 判断是否有执行权限
    *  F_OK 判断是否有存在
* 返回值：0 成功  -1 失败

```c
#include<unistd.h>
#include<stdio.h>

int main() {
    int ret = access("a.txt", F_OK);
    if(ret == -1) {
        perror("access");
    }

    printf("文件存在 !!!");
    return 0;
}
```

#### 6.2 chmod + chown

`int chmod(const char *pathname, mode_t mode);`

  **作用：**用来修改文件的权限

* 参数：
  * pathname: 需要修改的文件路径
  * mode 需要修改的权限值， 八进制数

* 返回值 0 -1

```c
#include<sys/stat.h>
#include<stdio.h>

int main() {
    int ret = chmod("a.txt", 0775);
    if(ret == -1) {
        perror("chmod");
        return -1;
    }
    return 0;
}
```

`int chown(const char *path, uid_t owner, gid_t group);`

**作用：**修改文件所有者

- 可使用`vim /etc/passwd`查看有哪些用户
- 可使用`vim /etc/group`查看有哪些组

#### 6.3 truncate

`int truncate(const char *path, off_t length);`

  **作用：**缩减或者扩展文件的尺寸至指定的大小

*   参数：
    * path 需要修改文件的路径
    * length 需要最终文件变成的大小

### 7. 目录操作函数

#### 7.1 mkdir + rename

`int mkdir(const char *pathname, mode_t mode);`

**作用：**创建一个目录

*   参数：
    * pathname 创建的目录的路径
    * mode 权限

*   返回值: 0 -1

```c
int ret = mkdir("aaa", 0777) 
```

`int rename(const char *oldpath, const char *newpath);`

```c
int ret = rename("aaa", "bbb");
```

#### 7.2 chdir + getcwd

`int chdir(const char *path);`

**作用：**修改进程的工作目录

比如在/home/user 启动了一个可执行程序a.out 进程的工作目录 /home/user

`char *getcwd(char *buf, size_t size)`;

 `作用：`获取当前的工作目录

*   参数：
    * buf 存储路径，指向一个数组（传出参数）
    * size 数组的大小

*   返回值：返回指向内存的一块地址，这个数据就是第一个参数

```c
#include <unistd.h>
#include<stdio.h>
int main() {
    // 获取当前的工作目录
    char buf[128];
    getcwd(buf, sizeof(buf));
    printf("当前的工作目录是：%s\n", buf);

    // 修改工作目录
    int ret = chdir("/home/user/Linux/lesson09");
    if(ret == -1) {
        perror("chdir");
        return -1;
    }

    getcwd(buf, sizeof(buf));
    printf("当前的工作目录是：%s\n", buf);
    return 0;
}
```

### 8. 目录遍历函数

#### 8.1 dirent结构体 和 d_type

**dirent结构体:**

```c
struct dirent
{
    // 此目录进入点的inode
    ino_t d_ino; 
    // 目录文件开头至此目录进入点的位移
    off_t d_off; 
    // d_name 的长度, 不包含NULL字符
    unsigned short int d_reclen; 
    // d_name 所指的文件类型
    unsigned char d_type; 
    // 文件名
    char d_name[256];
};
```

**d_type:**

![](D:/work/c++/Linux服务器开发/Linux系统编程入门/image/10.png)

#### 8.2opendir + readdir

`DIR *opendir(const char *name);`

* 参数：
  * name 需要打开的目录的名称

* 返回值
  * DIR * 类型，目录流
  * 错误返回NULL

`struct dirent *readdir(DIR *dirp);`

 **作用：**读取目录中的数据

* 参数  dirp是opendir的返回的结果

* 返回值：
  * struct dirent * 代表读取到的文件信息
  * 读取到末尾或者失败了 返回NULL

```c
#include<dirent.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<stdio.h>
#include<string.h>

int getFileNum(const char *);
// 读取某个目录下普通文件的个数
int main(int argc, char * argv[]) {
    if(argc < 2) {
        printf("%s path\n", argv[0]);
        return -1;
    }
    int num = getFileNum(argv[1]);
    printf("普通文件的个数为：%d\n", num);

    return 0;
}

// 用于获取目录下所有普通文件的个数
int getFileNum(const char *path) {
    int total = 0;
    // 打开目录
    DIR  *dir = opendir(path);
    if(dir == NULL) {
        perror("opendir");
        exit(0);
    }
    struct dirent *ptr = NULL;
    while((ptr = readdir(dir)) != NULL) {
        // 获取名称
        char *dname = ptr->d_name;
        // 忽略掉 . 和 ..
        if(strcmp(dname, ".") == 0 || strcmp(dname, "..") == 0) {
            continue;
        }
        // 判断是否是普通文件还是目录
        if(ptr->d_type == DT_DIR) {
            // 目录 需要继续读取该目录
            char newpath[256];
            sprintf(newpath, "%s/%s", path, ptr->d_name);
            total += getFileNum(newpath);
        } else if(ptr->d_type == DT_REG) {
            // 普通文件
            ++ total;
        }
    }
    // 关闭目录
    closedir(dir);
    return total;
}
```

### 9. 其他

#### 9.1 dup + dup2

`int dup(int oldfd);`

**作用：**复制一个新的文件描述符，指向同一个文件

```c
#include<unistd.h>
#include<stdio.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<string.h>
int main() {
    int fd = open("a.txt", O_RDWR | O_CREAT | O_APPEND, 0664);
    int fd1 = dup(fd);
    if(fd1 == -1) {
        perror("dup");
        return -1;
    }

    printf("fd: %d, fd1: %d\n", fd, fd1);

    close(fd);
    char *str = " dergw";
    write(fd1, str, strlen(str));
    close(fd1);
    return 0;
}
```

`int dup2(int oldfd, int newfd);`

**作用：**重定向文件描述符

* 参数：
  * oldfd 指向 a.txt newfd 指向 b.txt

>   调用函数成功后，newfd和b.txt做close,newfd指向了a.txt
>
>   oldfd必须是一个有效的文件描述符

```c
int main () {
    int fd = open("1.txt", O_RDWR | O_CREAT, 0664);
    if(fd == -1) {
        perror("open");
        return -1;
    }

    int fd1 = open("2.txt", O_RDWR | O_CREAT, 0664);
    if(fd1 == -1) {
        perror("open");
        return -1;
    }

    printf("fd: %d fd1: %d \n", fd, fd1);

    int fd2 = dup2(fd, fd1);
    if(fd2 == - 1) {
        perror("dup2");
        return -1;
    }
    char * str = "hekl";
    int len = write(fd1, str, strlen(str));
    printf("fd: %d fd1: %d fd2: %d\n", fd, fd1, fd2);
    close("1.txt");
    return 0;
}
```

#### 9.2 fcntl

`int fcntl(int fd, int cmd, ... / arg /);`

  **作用：**复制文件描述符和设置/获取文件的状态标志

* 参数：

  * fd 文件描述符

  * cmd 对文件描述符如何操作

    * F_DUPFD 复制文件描述符  复制的是第一个参数fd，得到一个新的文件描述符

    * F_GETFL 获取指定文件描述符的文件状态flag

      ​		获取的flag和open函数传递的flag是一个东西

    * F_SETFL 设置指定文件描述符的文件状态flag

      ​        必选项 O_RDONLY O_WRONLY O_RDER 不可以被修改

    * O_APPEND 表示追加数据

    * NONBLOCK 设置成非阻塞

  `阻塞和非阻塞：描述的是函数调用的行为。`

```c
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include<string.h>
int main() {
    // 复制文件描述符
    // int fd = open("1.txt", O_RDONLY);
    //int ret = fcntl(fd, F_DUPFD);
    
    // 修改或获取文件状态flag
    int fd = open("1.txt", O_RDWR);
    if(fd == - 1) {
        perror("open");
        return -1;
    }
    // 获取文件描述符状态flag
    int flag = fcntl(fd, F_GETFL);
    fcntl(fd, F_SETFL, O_APPEND | flag);

    char *str = "defwe";
    write(fd, str, strlen(str));

    close(fd);
    return 0;

}
```

