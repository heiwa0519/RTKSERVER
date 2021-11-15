/*
 * 程序基于Qt6  实现GNSS数据处理的便捷小工具
 * Qt多线程试验
 *
 * 计划实现的功能：
 *  （1）坐标转换
 *      >ITRF与WGS84 CGCS2000之间的坐标转换
 *      >llh转ECEF转ENU
 *
 *  （2）时间转换
 *      >UTC\GPST\年纪日\儒略日
 *
 *  （3）格式转换，读取文件格式并修改为特定格式
 *      >多个文件的比较（对准时间然后进行简单的运算）+结合matlab
 *
 *
 *  （4）数据完整性分析
 *      >根据rtklib的pos文件，检查数据是否连续（中间是否有哪一秒数据丢失了）
 *
 *
 *  （5）数据质量分析模块
 *      >基于matlab？
 *
 *
 *
 *
 *
 *
*/
#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}


