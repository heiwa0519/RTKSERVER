#ifndef NAVIMAIN_H
#define NAVIMAIN_H
#include <QObject>
//#include <QWidget>
#include <QTimer>
#include <QDebug>
#include "rtklib.h"
//#include <QLabel>
#include <QString>
#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <string.h>

#define PRGNAME     "KORO_RTK"            /* program name */
#define TRACEFILE   "koro_rtk_%Y%m%d%h%M.trace" // debug trace file
#define STATFILE    "koro_rtk_%Y%m%d%h%M.stat"  // solution status file
#define MAXSTR      1024                /* max length of a stream */
#define PROGNAME    "navi"              /* program name */
#define MAXFILE     16                  /* max number of input files */
#define MAXCHAR     0x7f

class NaviMain:public QObject
{
public:
    NaviMain();
        ~NaviMain();

    //定时器  周期性输出用
    QTimer Timer;


    int num=1;

    //两个path只是为了记录路径  在输入设置时候不起作用
    QString confPath;
    QString dboptPath;

    QString dbconnType;
    QString hostName;
    int port;
    QString dbName;
    QString userName;
    QString password;


    //char tablename[MAXCHAR];  //tablename是写入数据库表的依据  动态的建立表名通过修改该变量达到目的
    char net[MAXCHAR];
    char rover[MAXCHAR];
    char base[MAXCHAR];
    char unrepname[MAXCHAR];
    char tableformat[MAXCHAR];
    int swapintv;
    int swaptime;
    int heart;

    double roverllh[3];//rad  流动站的坐标，计算流动站解算结果ENU用
    double roverxyz[3];//重大问题！ llh转为xyz后计算的ENU U方向差别很大，暂时把xyz和llh都设置为输入参数，后续再测试为啥llh和xyz互转值不一样（与rtklib区别）

    char postable[MAXCHAR];
    char skytable[MAXCHAR];
    char obstable[MAXCHAR];
    char eventtable[MAXCHAR];


    int creatsqlsec=-1;//创建到数据库时的gtime秒数
    int tosqlsec=-1;//输出到数据库时的gtime秒数，如果是这个秒数的第一次输出，则正常输出，如果不是就不输出

    QSqlDatabase db=*new QSqlDatabase;
    QSqlQuery query;

    //navi解算部分全局变量（类内）
    rtksvr_t *svr=new rtksvr_t;
    stream_t *moni=new stream_t;

    prcopt_t prcopt;
    solopt_t solopt;
    filopt_t filopt;
    //rcvopt_t rcvopt;


    int strtype[8]={                  /* stream types */
        STR_NONE,STR_NONE,STR_NONE,STR_NONE,STR_NONE,STR_NONE,STR_NONE,STR_NONE
    };
    char strpath[8][MAXSTR]={""};    /* stream paths */
    int strfmt[8]={                   /* stream formats */
        STRFMT_RTCM3,STRFMT_RTCM3,STRFMT_SP3,SOLF_LLH,SOLF_NMEA,0,0,0
    };
    int svrcycle     =10;            /* server cycle (ms) */
    int timeout      =10000;         /* timeout time (ms) */
    int reconnect    =10000;         /* reconnect interval (ms) */
    int nmeacycle    =5000;          /* nmea request cycle (ms) */
    int fswapmargin  =30;            /* file swap marign (s) */
    int buffsize     =32768;         /* input buffer size (bytes) */
    int navmsgsel    =0;             /* navigation mesaage select */
    int nmeareq      =0;             /* nmea request type (0:off,1:lat/lon,2:single) */
    double nmeapos[3] ={0,0};         /* nmea position (lat/lon) (deg) */
    char proxyaddr[MAXSTR]="";       /* proxy address */

    char *paths[8]={strpath[0],strpath[1],strpath[2],strpath[3],
                    strpath[4],strpath[5],strpath[6],strpath[7]};

    char*cmds[3]={0,0,0};
    char*cmds_periodic[3]= {0,0,0};
    char*ropts[3]={"","",""};

    char errmsg[20148];


    void init();//

    void loadini();
    void svrinit();//参数初始化 全局变量，load到class内

    void getrcvopts();//将全局变量的rcvopt部分读取到类内成员
    void resetrcvopts();//将全局变量重置为默认值。

    void svrstart();//svr服务启动
    void svrstop();//svr服务关闭


//————————————数据库相关函数——————————————————————————————————
    int SQLInit();//数据库初始化
    int SQLconnect_close();//关闭数据库

    int creatTable(char* netname,char* basename,char* rovername,char*tableformat);//创建数据表，并定义表的主键、列名、列类型等
    //创建数据表的函数
    int sql_creat_rr(char* tablename);
    int sql_creat_ecef(char* tablename);
    int sql_creat_pos(char* tablename);
    int sql_creat_sky(char* tablename);
    int sql_creat_obs(char* tablename);
    int sql_creat_net(char* tablename);

    void outputsol();//sol结果输出
    int sol_to_sql(sol_t *sol, char* table_name);
    void sol_to_debug(sol_t *sol,int nsol);//sol结果输出到debug

    //更新数据表的函数
    int sql_update_station();
    int sql_update_baseline();
    int sql_update_net();

    //sol输出到表的具体函数
    //输出sol_t.rr到sql
    int sql_out_rr(sol_t *sol,char* tablename,int num);
    int sql_out_ecef(sol_t *sol,char* tablename,int num);
    int sql_out_pos(sol_t *sol,char* tablename,int num);
    int sql_out_obs(obs_t *obs_R,obs_t *obs_B,char* tablename);
    int sql_out_sky(rtk_t *rtk,char* tablename);

//————————————数据库相关函数——————————————————————————————————


    void OpenMoniPort(int port);

public slots:
        void outPutSol();

};

#endif // NAVIMAIN_H
