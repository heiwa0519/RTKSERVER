/*
* 基于strsvr/str2str的实时流数据记录和转发模块，简单来说就是多个流程序集成到一个程序中
* 通过读取 .list 文件来快捷设置各项参数 通过一个程序 来实现数据的一键启动和停止多个strsvr数据流
*
* 计划功能：
* 增加事件日志功能，记录数据完整性情况
* CUI监视窗口，
* 具有交互性，
* 参考Ntrip Caster 2.0.27 的网页监视端，实现CUI程序的实时监控，对各个流的状况进行监控
*
* 增加一个读取输入参数的功能，能够在启动的时候根据输入的参数来修改输入ini文件的路径
*
*
* Caster有一个 casterwatch程序\脚本   可以记录程序的pid  方便后台程序的开启和关闭
*
*
*/


#include <QCoreApplication>
#include "recordmain.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


    //读取ini文件 获取list file 路径
    loadini(argc,argv);

    //读取基本参数，判断任务流个数  返回任务个数n  创建指针数组 宏定义支持的最大值
    taskInit();


    //根据任务个数来创建新的对象
    taskCreat();

    //利用循环创建对象，启动记录，并记录每个记录的svr的地址，利用结构体+链表？






    return a.exec();
}
