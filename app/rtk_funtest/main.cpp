#include <QCoreApplication>
#include<QDebug>
#include "rtklib.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


    double dms[3];
    double deg;
    dms[0]=29;
    dms[1]=32;
    dms[2]=15.50486;

    deg=dms2deg(dms);


    qDebug()<<deg;



    return a.exec();
}
