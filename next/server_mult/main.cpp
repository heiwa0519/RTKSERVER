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


#define SVR_DEFAULT_SETTING_PATH  "D:/RTKSERVER/conf/server_mult/default.ini"



int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //进入主函数
    svrMain();



    return a.exec();
}
