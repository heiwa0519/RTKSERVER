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
