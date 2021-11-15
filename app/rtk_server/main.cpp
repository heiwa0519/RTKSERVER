/*
 * 基于RTKlib 2.4.3 b.34 Navi\rtkrcv模块的实时RTK解算程序
 * 可以理解为navi去掉了图形化模块，并可以将解算结果输出至数据库
 * 参考了navi（win）和rtkrcv（linux）的代码，利用qt跨平台的特性实现一份代码多平台运行。
 * 暂时没有对rtklib的源码部分进行修改，通过调用rtklib库的方式实现当前的功能
 *
 * 当前实现功能：
 * 基于RTKnavi内核（rtksvrstart()函数）的RTK解算
 * 通过调用计时器来读取rtk解算的结果并写入MySQL数据库（固定的表结构）
 * 通过读取conf文件和dbopt文件来设置解算参数和写入数据库的设置
 *
 * 后续功能完善：
 *
 *
 *
*/



#include <QCoreApplication>
#include "navimain.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


    //新建一个navi对象
    NaviMain w;

    //连接信号和槽，初始化timer
    w.init();

    //开始程序 构建函数 读取ini文件 获取OPT file 路径
    w.loadini();

    //读conf dbopt  将设置读取到全局变量

    //初始化rtknavi所需的各项参数 读取conf
    w.svrinit();
    //初始化MySQL连接,读取参数到class内   读取dbopt
    w.Navi_Init();
    //解算开始
    w.svrstart();

    //结果的实时输出函数
    //w.outputsol();


    qDebug()<<"emmmm问题很大";
    return a.exec();

}
