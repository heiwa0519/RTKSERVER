# RTKSERVER #

----------

一个变形监测系统后端软件，基于RTKlib开发，实现GNSS数据流接收、解算、并将解算结果以指定形式存放至数据库。
整个监测系统由后端、数据库和前端组成。


本程序利用Qt编写、调试和生成可执行程序，为满足课题实际需要，可在Windows和Linux下运行


- 在Windows下利用 `Qt 6.2.1 MinGW 64-bit` 版本编译器构建


- 在linux下利用 `Qt 6.1.2 GCC 64-bit` 版本编译器构建


- 工程的构建参照了[emlid/RTKLIB](https://github.com/emlid/RTKLIB/tree/reach)的工程构建方法，并将rtklib核心代码部分替换为[RTKLIB 2.4.3 b34](https://github.com/tomojitakasu/RTKLIB/tree/rtklib_2.4.3) 版本



based on RTKlib 2.4.3 b34 , use Qt 6.2.1 MinGW 64-bit in Windows and Qt 6.1.2 GCC 64bit in Linux.


----------

# 已实现功能 #


##  1. RTKSERVER ##
本项目的主要工作，逐步实现各个功能并完善，基于程序rtknavi和rtkrcv改写，主要实现接收GNSS实时流并实时解算，将结果和一些其他指定信息写入数据库，为监测系统前端展示提供数据支持。


### 功能实现 ###
- 单个线程的RTK解算，并输出结果到数据库


### 存在问题 ###
- 没有数据库重连机制，为解决超时断开的问题

## 2. str2str_mult ##
一个CUI数据记录软件，基于str2str改写，相较于RTKLIB提供的CUI程序str2str，可以实现多个数据流的同步记录。

### 功能实现 ###
- 多个数据流同时记录并存储数据，理论上str2str能做的都能做，并且一个程序能执行多个线程


### 存在问题 ###

- 程序时间过于简单，不能针对某一流进行操作
- 存放文件的时候不能根据需求自动创建文件夹，智能在已有的文件夹下建立新文件

----------


# 下一步开发计划 #

## 1.server_mult  ##

- 实现server的多线程解算和数据库输出
- 优化参数的输入路径
- 重新设计逻辑结构，提升扩展性


## 2.rtk_analysis  ##
- 数据分析软件


## 3.rtk_tools  ##

- GNSS数据处理小工具

## 4.server_GUI  ##

- SERVER程序的图形化界面接口
