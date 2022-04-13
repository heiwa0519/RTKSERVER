/*
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
*/

#include <QCoreApplication>
#include"server_mult.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //进入主函数
    svrMain();

    return a.exec();
}
