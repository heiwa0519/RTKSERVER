#include "navimain.h"


//option file path ---------------------------------------------------
static char ConfPath[MAXSTR]={""};
static char DbOptPath[MAXSTR]={""};


static opt_t iniopts[]={
    {"conf-path",       2,  (void *)ConfPath,         ""     },
    {"dbopt-path",      2,  (void *)DbOptPath,        ""     },

    {"",0,NULL,""} /* terminator */
};


//SQL option --------------------------------------------------------
static char sqlConnType[MAXSTR];
static char sqlHostName[MAXSTR];
static int sqlPort;
static char sqlDatebaseName[MAXSTR];
static char sqlUserName[MAXSTR];
static char sqlPassword[MAXSTR];
static char sqlTableName[MAXSTR];
static char sqlNet[MAXSTR];
static char sqlBase[MAXSTR];
static char sqlRover[MAXSTR];
static char sqlTableformat[MAXSTR];
static int sqlSwapIntv;
static int sqlSwapTime;
static int sqlHeart;
static double rover_llh[3],rover_xyz[3];

static opt_t dbopts[]={
    {"sql-dbconntype",  2,  (void *)sqlConnType,      ""     },
    {"sql-hostname",    2,  (void *)sqlHostName,      ""     },
    {"sql-port",        0,  (void *)&sqlPort,         ""     },
    {"sql-dbname",      2,  (void *)sqlDatebaseName,  ""     },
    {"sql-username",    2,  (void *)sqlUserName,      ""     },
    {"sql-password",    2,  (void *)sqlPassword,      ""     },
    {"sql-net",         2,  (void *)sqlNet,           ""     },
    {"sql-base",        2,  (void *)sqlBase,          ""     },
    {"sql-rover",       2,  (void *)sqlRover,         ""     },
    {"sql-tableformat", 2,  (void *)sqlTableformat,   ""     },
    {"sql-swapIntv",    0,  (void *)&sqlSwapIntv,     ""     },
    {"sql-swapTime",    0,  (void *)&sqlSwapTime,     ""     },
    {"sql-heart",       0,  (void *)&sqlHeart,        ""     },
    {"rover-lat",       1,  (void *)&rover_llh[0],     ""    },
    {"rover-lon",       1,  (void *)&rover_llh[1],     ""    },
    {"rover-hight",     1,  (void *)&rover_llh[2],     ""    },
    {"rover-ecefx",     1,  (void *)&rover_xyz[0],     ""    },
    {"rover-ecefy",     1,  (void *)&rover_xyz[1],     ""    },
    {"rover-ecefz",     1,  (void *)&rover_xyz[2],     ""    },

    {"",0,NULL,""} /* terminator */
};



// receiver options table ---------------------------------------------------
static int Strtype[8]={                  /* stream types */
    STR_NONE,STR_NONE,STR_NONE,STR_NONE,STR_NONE,STR_NONE,STR_NONE,STR_NONE
};
static char Strpath[8][MAXSTR]={""};    /* stream paths */
static int Strfmt[8]={                   /* stream formats */
    STRFMT_RTCM3,STRFMT_RTCM3,STRFMT_SP3,SOLF_LLH,SOLF_NMEA,0,0,0
};
static int Svrcycle     =10;            /* server cycle (ms) */
static int Timeout      =10000;         /* timeout time (ms) */
static int Reconnect    =10000;         /* reconnect interval (ms) */
static int Nmeacycle    =5000;          /* nmea request cycle (ms) */
static int Fswapmargin  =30;            /* file swap marign (s) */
static int Buffsize     =32768;         /* input buffer size (bytes) */
static int Navmsgsel    =0;             /* navigation mesaage select */
static int Nmeareq      =0;             /* nmea request type (0:off,1:lat/lon,2:single) */
static double Nmeapos[2] ={0,0};         /* nmea position (lat/lon) (deg) */
static char Proxyaddr[MAXSTR]="";       /* proxy address */


static char Strpath_defult[MAXSTR]={""};
static char Proxyaddr_defult[MAXSTR]={""};





//static int timetype     =0;             /* time format (0:gpst,1:utc,2:jst,3:tow) */
//static int soltype      =0;             /* sol format (0:dms,1:deg,2:xyz,3:enu,4:pyl) */
//static int solflag      =2;             /* sol flag (1:std+2:age/ratio/ns) */
//static int strtype[]={                  /* stream types */
//    STR_SERIAL,STR_NONE,STR_NONE,STR_NONE,STR_NONE,STR_NONE,STR_NONE,STR_NONE
//};
//static char strpath[8][MAXSTR]={"","","","","","","",""}; /* stream paths */
//static int strfmt[]={                   /* stream formats */
//    STRFMT_UBX,STRFMT_RTCM3,STRFMT_SP3,SOLF_LLH,SOLF_NMEA
//};
//static int svrcycle     =10;            /* server cycle (ms) */
//static int timeout      =10000;         /* timeout time (ms) */
//static int reconnect    =10000;         /* reconnect interval (ms) */
//static int nmeacycle    =5000;          /* nmea request cycle (ms) */
//static int buffsize     =32768;         /* input buffer size (bytes) */
//static int navmsgsel    =0;             /* navigation mesaage select */
//static char proxyaddr[256]="";          /* http/ntrip proxy */
//static int nmeareq      =0;             /* nmea request type (0:off,1:lat/lon,2:single) */
//static double nmeapos[] ={0,0,0};       /* nmea position (lat/lon/height) (deg,m) */
//static char rcvcmds[3][MAXSTR]={""};    /* receiver commands files */
//static char startcmd[MAXSTR]="";        /* start command */
//static char stopcmd [MAXSTR]="";        /* stop command */
//static int modflgr[256] ={0};           /* modified flags of receiver options */
//static int modflgs[256] ={0};           /* modified flags of system options */
//static int moniport     =0;             /* monitor port */
//static int keepalive    =0;             /* keep alive flag */
//static int fswapmargin  =30;            /* file swap margin (s) */
//static char sta_name[256]="";           /* station name */





#define TIMOPT  "0:gpst,1:utc,2:jst,3:tow"
#define CONOPT  "0:dms,1:deg,2:xyz,3:enu,4:pyl"
#define FLGOPT  "0:off,1:std+2:age/ratio/ns"
#define ISTOPT  "0:off,1:serial,2:file,3:tcpsvr,4:tcpcli,6:ntripcli,7:ftp,8:http"
#define OSTOPT  "0:off,1:serial,2:file,3:tcpsvr,4:tcpcli,5:ntripsvr,9:ntrcaster"
#define FMTOPT  "0:rtcm2,1:rtcm3,2:oem4,3:oem3,4:ubx,5:ss2,6:hemis,7:skytraq,8:javad,9:nvs,10:binex,11:rt17,12:spt,13:rnx,14:sp3,15:clk,16:sbas,17:nmea"
#define NMEOPT  "0:off,1:latlon,2:single"
#define SOLOPT  "0:llh,1:xyz,2:enu,3:nmea"
#define MSGOPT  "0:all,1:rover,2:base,3:corr"



static opt_t rcvopts[]={
    {"inpstr1-type",    3,  (void *)&Strtype[0],         ISTOPT },
    {"inpstr2-type",    3,  (void *)&Strtype[1],         ISTOPT },
    {"inpstr3-type",    3,  (void *)&Strtype[2],         ISTOPT },
    {"inpstr1-path",    2,  (void *)Strpath [0],         ""     },
    {"inpstr2-path",    2,  (void *)Strpath [1],         ""     },
    {"inpstr3-path",    2,  (void *)Strpath [2],         ""     },
    {"inpstr1-format",  3,  (void *)&Strfmt [0],         FMTOPT },
    {"inpstr2-format",  3,  (void *)&Strfmt [1],         FMTOPT },
    {"inpstr3-format",  3,  (void *)&Strfmt [2],         FMTOPT },
    {"inpstr2-nmeareq", 3,  (void *)&Nmeareq,            NMEOPT },
    {"inpstr2-nmealat", 1,  (void *)&Nmeapos[0],         "deg"  },
    {"inpstr2-nmealon", 1,  (void *)&Nmeapos[1],         "deg"  },
    {"outstr1-type",    3,  (void *)&Strtype[3],         OSTOPT },
    {"outstr2-type",    3,  (void *)&Strtype[4],         OSTOPT },
    {"outstr1-path",    2,  (void *)Strpath [3],         ""     },
    {"outstr2-path",    2,  (void *)Strpath [4],         ""     },
    {"outstr1-format",  3,  (void *)&Strfmt [3],         SOLOPT },
    {"outstr2-format",  3,  (void *)&Strfmt [4],         SOLOPT },
    {"logstr1-type",    3,  (void *)&Strtype[5],         OSTOPT },
    {"logstr2-type",    3,  (void *)&Strtype[6],         OSTOPT },
    {"logstr3-type",    3,  (void *)&Strtype[7],         OSTOPT },
    {"logstr1-path",    2,  (void *)Strpath [5],         ""     },
    {"logstr2-path",    2,  (void *)Strpath [6],         ""     },
    {"logstr3-path",    2,  (void *)Strpath [7],         ""     },

    {"misc-svrcycle",   0,  (void *)&Svrcycle,           "ms"   },
    {"misc-timeout",    0,  (void *)&Timeout,            "ms"   },
    {"misc-reconnect",  0,  (void *)&Reconnect,          "ms"   },
    {"misc-nmeacycle",  0,  (void *)&Nmeacycle,          "ms"   },
    {"misc-buffsize",   0,  (void *)&Buffsize,           "bytes"},
    {"misc-navmsgsel",  3,  (void *)&Navmsgsel,          MSGOPT },
    {"misc-proxyaddr",  2,  (void *)Proxyaddr,           ""     },
    {"misc-fswapmargin",0,  (void *)&Fswapmargin,        "s"    },

    {"",0,NULL,""}
};

NaviMain::NaviMain()
{
    //ui->setupUi(this);


}

NaviMain::~NaviMain()
{

}

void NaviMain::init()
{
    //connect(&Timer,SIGNAL(timeout()),this,SLOT(outPutSol()));



    qDebug()<<"Init Navimain server start";

   connect(&Timer,&QTimer::timeout,this,&NaviMain::outPutSol);

    Timer.setInterval(66);
    Timer.setSingleShot(false);
    Timer.start();
}

void NaviMain::loadini()
{

#ifdef WIN32
    loadopts("D:/RTKSERVER/conf/server/option.ini",iniopts);
#else
    loadopts("option.ini",iniopts);
#endif


    qDebug()<<"conf  file path:"<<ConfPath;
    qDebug()<<"dbopt file path:"<<DbOptPath;
}


void NaviMain::svrinit()
{
    confPath=ConfPath;
    char *optfile=ConfPath;


    //初始化 流路径
    qDebug()<<"navi server init";

    //定义opt
    prcopt=prcopt_default;
    solopt=solopt_default;
    filopt={};


     qDebug()<<"load conf opt";

    //初始化 opt全局变量
    resetsysopts();
    resetrcvopts();

    //将conf文件读入到 sysopts（prcopt_ solopt_ filopt_ 等）和rcvopts
    loadopts(optfile,sysopts);
    loadopts(optfile,rcvopts);

    //将prcopt_ solopt_ filopt_等赋值给prcopt,solopt,filopt
    getsysopts(&prcopt,&solopt,&filopt);
    getrcvopts();


    //qDebug()<<"conf load finished";

    //rtksvr_t结构体初始化
    rtksvrinit(svr);
    strinit(moni);

    //rtksvrstart函数所需的所有参数读取到类内成员中，并完成navi部分所需的类内成员的初始化
    //读取完成，重置opt全局变量
    resetsysopts();
    resetrcvopts();


}


void NaviMain::svrstart()
{


    sprintf(solopt.prog ,"%s ver.%s %s",PROGNAME,VER_RTKLIB,PATCH_LEVEL);
    sprintf(filopt.trace,"%s.trace",PROGNAME);


    if (!prcopt.navsys) {
        prcopt.navsys=SYS_GPS|SYS_GLO;
    }

    qDebug()<<"svrstart";
    rtksvrstart(svr,svrcycle,buffsize,strtype,paths,strfmt,navmsgsel,
                         cmds,cmds_periodic,ropts,nmeacycle,nmeareq,nmeapos,&prcopt,
                         &solopt,moni,errmsg);

    //服务启动后才启动计时器是不是更好一些？

}




void NaviMain::svrstop()
{
    rtksvrstop(svr, cmds);
}

void NaviMain::outputsol()
{

    sol_t *outsol;

    rtksvrlock(svr);

    //如果时间到达零点且是正常时间（不是1970年）那么创建新表，数据改写
    double soltime[6]={0};
//        //gpst2utc(sol->time);
//        time2epoch(gpst2utc(outsol->time),soltime);
    gtime_t filetime;
    time(&filetime.time);
    filetime.sec=0.0;
    time2epoch(filetime,soltime);


    if(int(soltime[0])!=0 && int(soltime[3])==0 &&int(soltime[4])==0 && int(soltime[5])==0)
     {
         if(creatsqlsec!=svr->solbuf->time.time)
         {
             creatsqlsec=svr->solbuf->time.time;
             reppath(unrepname,tableformat,svr->solbuf->time,"","");
             creatTable(net,base,rover,tableformat);
         }
     }


    //利用nsol 和sol 进行数据输出

    for(int i=0;i<svr->nsol;i++)
    {
        outsol=svr->solbuf+i;



        double soltime[6]={0};
        //gpst2utc(sol->time);
        time2epoch(gpst2utc(outsol->time),soltime);

        char Time[128];
        sprintf(Time,"%d-%02d-%02d %02d:%02d:%02d",int(soltime[0]),int(soltime[1]),int(soltime[2]),int(soltime[3]),int(soltime[4]),int(soltime[5]));





        //一个历元sol的输出

        //sol_to_debug(outsol,nsol);

        //sol_to_sql(outsol,postable);

        //sql_out_rr(sol,table_name,num);
        sql_out_ecef(outsol,postable,Time);
        //sql_out_pos(sol,table_name,num);

        qDebug()<<num<<svr->nsol<<outsol->time.time<<"state"<<outsol->stat<<outsol->ns;

        sql_out_sat(svr,sattable,Time);

        num++;


    }

    svr->nsol=0;//solbuff计数清零  清零之后会影响程序本身的解算结果写出函数吗？

//    sql_out_snr(rtksvr_t *svr,char* tablename);




//    if(tosqlsec!=svr->rtk.sol.time.time)
//    {
//        tosqlsec=svr->rtk.sol.time.time;
//        sql_out_sky(&svr->rtk,skytable);
//        sql_out_obs(svr->obs[0],svr->obs[1],obstable);
//        //sql_out_obs(svr->obs[1],obstable,1);
//    }





    rtksvrunlock(svr);


}

void NaviMain::sol_to_debug(sol_t *sol,int nsol)
{

        qDebug()<<"eph"<<nsol<<"time:"<<sol->time.time<<"sec:"<<sol->time.sec
                <<"state:"<<sol->stat
                <<"    x:"<<sol->rr[0]
                <<"    y:"<<sol->rr[1]
                <<"    z:"<<sol->rr[2]
                <<"ratio:"<<sol->ratio;


}

void NaviMain::getrcvopts()
{

    //输入流
    for(int i=0;i<8;i++)
    {
        strtype[i]=Strtype[i];
        strncpy(strpath[i],Strpath[i],MAXSTR);
        strfmt[i]=Strfmt[i];
    }

    //nmea
    nmeareq=Nmeareq;
    nmeapos[0]=Nmeapos[0];
    nmeapos[1]=Nmeapos[1];
    nmeapos[2]=0;

    //
    svrcycle=Svrcycle;
    timeout=Timeout;
    reconnect=Reconnect;
    nmeacycle=Nmeacycle;
    buffsize=Buffsize;
    navmsgsel=Navmsgsel;
    strncpy(proxyaddr,Proxyaddr,MAXSTR);
    fswapmargin=Fswapmargin;


}

void NaviMain::resetrcvopts()
{

    for(int i=0;i<8;i++)
    {
        Strtype[i]=STR_NONE;
        strncpy(Strpath[i],Strpath_defult,MAXCHAR);
        Strfmt[i]=STRFMT_RTCM3;
    }

    Svrcycle     =10;            /* server cycle (ms) */
    Timeout      =10000;         /* timeout time (ms) */
    Reconnect    =10000;         /* reconnect interval (ms) */
    Nmeacycle    =5000;          /* nmea request cycle (ms) */
    Fswapmargin  =30;            /* file swap marign (s) */
    Buffsize     =32768;         /* input buffer size (bytes) */
    Navmsgsel    =0;             /* navigation mesaage select */
    Nmeareq      =0;             /* nmea request type (0:off,1:lat/lon,2:single) */
    Nmeapos[0] =0;/* nmea position (lat/lon) (deg) */
    Nmeapos[1] =0;
    strncpy(Proxyaddr,Proxyaddr_defult,MAXCHAR); /* proxy address */
}

void NaviMain::outPutSol()
{
    outputsol();
}

static int abortf=0;

int showmsg(char *format,...)
{

    va_list arg;
    QString str;
    static QString buff2;
    char buff[1024],*p;

    va_start(arg,format);
    vsprintf(buff,format,arg);
    va_end(arg);

    qDebug()<<buff;

    return abortf;
}

int debug_svr(rtksvr_t svr)
{

    system("pause");

    return 0;

}


int NaviMain::SQLInit()
{
    //初始化 流路径
    dboptPath=DbOptPath;
    char *dboptfile=DbOptPath;

    //初始化 对象内的opt 其实没必要


    //初始化全局变量
    sqlConnType[0]='\0';
    sqlHostName[0]='\0';
    sqlPort=0;
    sqlDatebaseName[0]='\0';
    sqlUserName[0]='\0';
    sqlPassword[0]='\0';
    sqlTableName[0]='\0';
    sqlNet[0]='\0';
    sqlBase[0]='\0';
    sqlRover[0]='\0';
    sqlTableformat[0]='\0';
    sqlSwapIntv=0;
    sqlSwapTime=0;
    sqlHeart=0;


    //将dbopt文件读入到dbopts
    loadopts(dboptfile,dbopts);

    //将dbopt传入对象内的变量
    dbconnType=sqlConnType;
    hostName=sqlHostName;
    port=sqlPort;
    dbName=sqlDatebaseName;
    userName=sqlUserName;
    password=sqlPassword;

    roverllh[0]=rover_llh[0]*D2R;
    roverllh[1]=rover_llh[1]*D2R;
    roverllh[2]=rover_llh[2];
    roverxyz[0]=rover_xyz[0];
    roverxyz[1]=rover_xyz[1];
    roverxyz[2]=rover_xyz[2];
    //pos2ecef(roverllh,roverxyz);


    strncpy(net,sqlNet,MAXCHAR);
    strncpy(base,sqlBase,MAXCHAR);
    strncpy(rover,sqlRover,MAXCHAR);


    //tablename=sqlTableName;

    //根据时间来修改talbename 实现动态建立表名字


    strncpy(unrepname,sqlTableformat,MAXCHAR);
    //tablename应当进行修改  以实现动态建立表明的功能 但当前还未修改
    //利用rtkcmn的  reppath函数
    gtime_t filetime;
    time(&filetime.time);
    filetime.sec=0.0;

    reppath(unrepname,tableformat,filetime,"","");


        qDebug()<<"optfile path  :"<<dboptPath;
        qDebug()<<"Connect Type  :"<<dbconnType;
        qDebug()<<"SQL HostName  :"<<hostName;
        qDebug()<<"SQL Port      :"<<port;
        qDebug()<<"DatebaseName  :"<<dbName;
        qDebug()<<"userName      :"<<userName;
        qDebug()<<"password      :"<<password;
        qDebug()<<"Net-base-rover:"<<net<<"-"<<base<<"-"<<rover;
        qDebug()<<"creat time    :"<<tableformat;



    //将dbopts的参数传入db  打开连接
    db =QSqlDatabase::addDatabase(dbconnType);
    //初始化连接参数
    db.setHostName(hostName);
    db.setPort(port);
    db.setDatabaseName(dbName);
    db.setUserName(userName);
    db.setPassword(password);
    bool ok =db.open();
    if(ok)
    {
        qDebug()<<"SQL connect success";
    }
    static QSqlQuery Query(db);
    query=Query;

    //创建表

    creatTable(net,base,rover,tableformat);


    //重置dbopt全局变量
    sqlConnType[0]='\0';
    sqlHostName[0]='\0';
    sqlPort=0;
    sqlDatebaseName[0]='\0';
    sqlUserName[0]='\0';
    sqlPassword[0]='\0';
    sqlTableName[0]='\0';


    sqlNet[0]='\0';
    sqlBase[0]='\0';
    sqlRover[0]='\0';
    sqlTableformat[0]='\0';
    sqlSwapIntv=0;
    sqlSwapTime=0;
    sqlHeart=0;






    return 0;
}

int NaviMain::creatTable(char* netname,char* basename,char* rovername,char*tableformat)
{


    sprintf(postable,"bak_%s_%s_pos_1s_%s",basename,rovername,tableformat);
    sprintf(sattable,"bak_%s_%s_sat_1s_%s",basename,rovername,tableformat);
//    sprintf(skytable,"%s_%s_%s_sky_%s",netname,basename,rovername,tableformat);
//    sprintf(obstable,"bak_%s_%s_obs_1s_%s",basename,rovername,tableformat);
    //sprintf(baseobstable,"%s_%s_OBS_%s",netname,basename,tableformat);
//    sprintf(eventtable,"%s_eventRecord",netname);



    sql_creat_ecef(postable);
    sql_creat_sat(sattable);
//    sql_creat_sky(skytable);
//    sql_creat_obs(obstable);


}

int NaviMain::SQLconnect_close()
{

    db.close();

    qDebug()<<"connect is closed";

    return 0;
}

int NaviMain::sol_to_sql(rtksvr_t *svr, char* table_name)
{


    return 0;
}

int NaviMain::sql_creat_ecef(char *tablename)
{
    //如果表不存在，则创建。
    QString creat="create table if not exists ";
    creat.append(tablename);//表名

    //定义主键、列名、列类型
    creat.append("("
                 "id                   bigint not null auto_increment comment 'ID',"
                 "time                 datetime comment '历元时间',"
                 "ecef_x               double comment 'ECEF_X',"
                 "ecef_y               double comment 'ECEF_Y',"
                 "ecef_z               double comment 'ECEF_Z',"
                 "q                    int comment '解的状态',"
                 "ns                   int comment '卫星数',"
                 "sdx                  float comment 'X方向标准差',"
                 "sdy                  float comment 'Y方向标准差',"
                 "sdz                  float comment 'Z方向标准差',"
                 "e                    double comment '东方向相对位移',"
                 "n                    double comment '北方向相对位移',"
                 "u                    double comment '天顶方向相对位移',"
                 "age                  float comment '差分年龄',"
                 "ratio                float comment 'ratio',"
                 "del_flag             char comment '删除标记',"
                 "creat_time           datetime comment '创建时间',"
                 "creat_by             varchar(64) comment '创建者',"
                 "update_time          datetime comment '更新时间',"
                 "update_by            varchar(64) comment '更新者',"
                 "remarks              varchar(255) comment '备注',"
                 "primary key (id)"
              ");");
    //"(time DATETIME primary key,X_ECEF double,Y_ECEF double,Z_ECEF double,Q int,ns int,sd_x float,sd_y float,sd_z float,sd_xy float,sd_yz float,sd_zx float,age float,ratio float)"

    qDebug()<<creat;

    query.exec(creat);

    return 0;
}

int NaviMain::sql_creat_sky(char *tablename)
{
    QString creat="create table if not exists ";
    creat.append(tablename);//表名

    //定义主键、列名、列类型
    //creat.append("(time DATETIME primary key,num_G int,num_C int,num_R int,num_E int,G_S_AzEL TEXT,G_S_AzEL TEXT,C_S_AzEL TEXT,R_S_AzEL TEXT,E_S_AzEL TEXT)");
    creat.append("(time DATETIME primary key,num_G int,num_C int,num_R int,num_E int,G_AzEL varchar(255),C_AzEL varchar(255),R_AzEL varchar(255),E_AzEL varchar(255))");//时间 记录的卫星数  卫星编码_状态_方位角高度角 ("AABCCCCCDDDD ")
    //creat.append("(time DATETIME primary key,X_ECEF double,Y_ECEF double,Z_ECEF double,Q int,ns int,sd_x float,sd_y float,sd_z float,sd_xy float,sd_yz float,sd_zx float,age float,ratio float)");

    //"(time DATETIME primary key,X_ECEF double,Y_ECEF double,Z_ECEF double,Q int,ns int,sd_x float,sd_y float,sd_z float,sd_xy float,sd_yz float,sd_zx float,age float,ratio float)"

    qDebug()<<creat;

    query.exec(creat);

    return 0;
}

int NaviMain::sql_creat_obs(char *tablename)
{
    QString creat="create table if not exists ";
    creat.append(tablename);//表名

    //定义主键、列名、列类型
    creat.append("(time DATETIME primary key,rovernum int,basenum int,RGnum int,RCnum int,RRnum int,REnum int,BGnum int,BCnum int,BRnum int,BEnum int,RG_SNR varchar(512),RC_SNR varchar(512),RR_SNR varchar(512),RE_SNR varchar(512),BG_SNR varchar(512),BC_SNR varchar(512),BR_SNR varchar(512),BE_SNR varchar(512))");//时间 测站卫星数量 基站卫星数量 测站卫星数据，基站卫星数据（卫星编码 S1 S2 "AAABBBB CCCC ")
    //(time DATETIME primary key,rovernum int,basenum int,R_SNR varchar(1024),B_SNR varchar(1024))
    //"(time DATETIME primary key,X_ECEF double,Y_ECEF double,Z_ECEF double,Q int,ns int,sd_x float,sd_y float,sd_z float,sd_xy float,sd_yz float,sd_zx float,age float,ratio float)"

    qDebug()<<creat;

    query.exec(creat);

    return 0;
}

int NaviMain::sql_creat_sat(char *tablename)
{
    //如果表不存在，则创建。
    QString creat="create table if not exists ";
    creat.append(tablename);//表名

    //定义主键、列名、列类型
    creat.append("("
                    "id                   bigint not null auto_increment comment 'ID',"
                    "time                 datetime comment '时间',"
                 "rover_num           int comment '测站可见卫星数',"
                 "base_num             int comment '基站可见卫星数',"
                 "rg_num               int comment '测站可见GPS卫星数量',"
                 "rc_num               int comment '测站可见北斗卫星数量',"
                 "rr_num               int comment '测站可见GLONASS数量',"
                 "re_num               int comment '测站可见Galileo卫星数量',"
                 "bg_num               int comment '基站可见GPS卫星数量',"
                 "bc_num               int comment '基站可见北斗卫星数量',"
                 "br_num               int comment '基站可见GLONASS数量',"
                 "be_num               int comment '基站可见Galileo卫星数量',"
                 "rg_sat               text comment '测站_GPS_info',"
                 "rc_sat               text comment '测站_BDS_info',"
                 "rr_sat               text comment '测站_GLONASS_info',"
                 "re_sat               text comment '测站_Galileo_info',"
                 "bg_sat               text comment '基站_GPS_info',"
                 "bc_sat               text comment '基站_BDS_info',"
                 "br_sat               text comment '基站_GLONASS_info',"
                 "be_sat               text comment '基站_Galileo_info',"
                 "creat_time           datetime comment '创建时间',"
                 "creat_by             varchar(64) comment '创建者',"
                 "update_time          datetime comment '更新时间',"
                 "update_by            varchar(64) comment '更新者',"
                 "remarks              varchar(255) comment '备注',"
                 "primary key (id)"
              ");");
    //"(time DATETIME primary key,X_ECEF double,Y_ECEF double,Z_ECEF double,Q int,ns int,sd_x float,sd_y float,sd_z float,sd_xy float,sd_yz float,sd_zx float,age float,ratio float)"

    qDebug()<<creat;

    query.exec(creat);

    return 0;
}

int NaviMain::sql_out_rr(sol_t *sol, char* tablename ,int num)
{
    QString sqlout="insert into ";
    sqlout.append(tablename);
    sqlout.append(" (num,gpss,sec,state,x,y,z,ratio) values(?,?,?,?,?,?,?,?)");

    int time =sol->time.time;

    query.prepare(sqlout);
    query.bindValue(0,num);
    query.bindValue(1,time);
    query.bindValue(2,sol->time.sec);
    query.bindValue(3,sol->stat);
    query.bindValue(4,sol->rr[0]);
    query.bindValue(5,sol->rr[1]);
    query.bindValue(6,sol->rr[2]);
    query.bindValue(7,sol->ratio);

    query.exec();

    query.clear();

    return 0;
}

int NaviMain::sql_out_ecef(sol_t *sol, char *tablename,char* Time)
{

    QString sqlout="insert into ";
    sqlout.append(tablename);



    double roverenu[3]={0};
    double rovervector[3]={sol->rr[0]-roverxyz[0],sol->rr[1]-roverxyz[1],sol->rr[2]-roverxyz[2]};

    ecef2enu(roverllh,rovervector,roverenu);



    //float std[3]={sqrt(sol->qr[0]),sqrt(sol->qr[1]),(sol->qr[2])};
    //写入数据库的数据最好提前定义格式并处理好，而不是在写入的时候进行处理，避免格式错误（如float开方得到的结果可能导致结果变成double型？，与数据库类型不匹配导致数据无法正常写入）


    sqlout.append(" (time,ecef_x,ecef_y,ecef_z,q,ns,sdx,sdy,sdz,e,n,u,age,ratio) values(?,?,?,?,?,?,?,?,?,?,?,?,?,?)");

    //int time =sol->time.time;

    query.prepare(sqlout);
    query.bindValue(0,Time);
    query.bindValue(1,sol->rr[0]);
    query.bindValue(2,sol->rr[1]);
    query.bindValue(3,sol->rr[2]);
    query.bindValue(4,sol->stat);
    query.bindValue(5,sol->ns);
    query.bindValue(6,sol->qr[0]);
    query.bindValue(7,sol->qr[1]);
    query.bindValue(8,sol->qr[2]);
    query.bindValue(9,roverenu[0]);
    query.bindValue(10,roverenu[1]);
    query.bindValue(11,roverenu[2]);
    query.bindValue(12,sol->age);
    query.bindValue(13,sol->ratio);


    //qDebug()<<sqlout;

    query.exec();

    query.clear();

    return 0;
}

int NaviMain::sql_out_pos(sol_t *sol, char *tablename, int num)
{



    double soltime[6]={0};
    //gpst2utc(sol->time);
    time2epoch(gpst2utc(sol->time),soltime);

    char Time[128];
    sprintf(Time,"%d-%02d-%02d %02d:%02d:%02d",int(soltime[0]),int(soltime[1]),int(soltime[2]),int(soltime[3]),int(soltime[4]),int(soltime[5]));


    double roverenu[3]={0};
    double rovervector[3]={sol->rr[0]-roverxyz[0],sol->rr[1]-roverxyz[1],sol->rr[2]-roverxyz[2]};

    ecef2enu(roverllh,rovervector,roverenu);



    float std[3]={sqrt(sol->qr[0]),sqrt(sol->qr[1]),(sol->qr[2])};
    //写入数据库的数据最好提前定义格式并处理好，而不是在写入的时候进行处理，避免格式错误（如float开方得到的结果可能导致结果变成double型？，与数据库类型不匹配导致数据无法正常写入）



    QString sqlout;
    sqlout.asprintf("insert into %s (time,ecef_x,ecef_y,ecef_z,q,ns,sdx,sdy,sdz,e,n,u,age,ratio)"
                    " values(%s,%lf,%lf,%lf,%d,%d,%f,%f,%f,%lf,%lf,%lf,%f,%f)",
                    tablename,Time,sol->rr[0],sol->rr[1],sol->rr[2],sol->stat,sol->ns,std[0],std[1],std[2],roverenu[0],roverenu[1],roverenu[2],sol->age,sol->ratio);


    sqlout=QString("insert into %1 (time,ecef_x,ecef_y,ecef_z,q,ns,sdx,sdy,sdz,e,n,u,age,ratio) values(?,%2,%3,%4,%5,%6,%7,%8,%9,%10,%11,%12,%13,%14)")
            .arg(tablename)
            //.arg("2022-04-15 14:25:35")
            .arg(sol->rr[0])
            .arg(sol->rr[1])
            .arg(sol->rr[2])
            .arg(sol->stat)
            .arg(sol->ns)
            .arg(std[0])
            .arg(std[1])
            .arg(std[2])
            .arg(roverenu[0])
            .arg(roverenu[1])
            .arg(roverenu[2])
            .arg(sol->age)
            .arg(sol->ratio);



//    sqlout.append(" (time,ecef_x,ecef_y,ecef_z,q,ns,sdx,sdy,sdz,e,n,u,age,ratio) values(?,?,?,?,?,?,?,?,?,?,?,?,?,?)");

    //int time =sol->time.time;

    qDebug()<<sqlout;

    query.prepare(sqlout);
    query.bindValue(0,Time);
//    query.bindValue(1,sol->rr[0]);
//    query.bindValue(2,sol->rr[1]);
//    query.bindValue(3,sol->rr[2]);
//    query.bindValue(4,sol->stat);
//    query.bindValue(5,sol->ns);
//    query.bindValue(6,std[0]);
//    query.bindValue(7,std[1]);
//    query.bindValue(8,std[2]);
//    query.bindValue(9,roverenu[0]);
//    query.bindValue(10,roverenu[1]);
//    query.bindValue(11,roverenu[2]);
//    query.bindValue(12,sol->age);
//    query.bindValue(13,sol->ratio);

    query.exec();

    query.clear();

    return 0;
}

int NaviMain::sql_out_obs(obs_t *obs_R,obs_t *obs_B,char* tablename)
{

    obs_t* obs[2]={obs_R,obs_B};
    //creat.append("(time DATETIME primary key,rovernum int,basenum int,R_SNR varchar(1024),B_SNR varchar(1024))");//时间 测站卫星数量 基站卫星数量 测站卫星数据，基站卫星数据（卫星编码 S1 S2 "AAABBBB CCCC ")
    //time DATETIME primary key,rovernum int,basenum int,RGnum int,RCnum int,RRnum int,REnum int,BGnum int,BCnum int,BRnum int,BEnum int,
    //RG_SNR varchar(255),RC_SNR varchar(255),RR_SNR varchar(255),RE_SNR varchar(255),BG_SNR varchar(255),BC_SNR varchar(255),BR_SNR varchar(255),BE_SNR varchar(255))

    double soltime[6]={0};
    //gpst2utc(sol->time);
    time2epoch(gpst2utc(obs[0]->data->time),soltime);

    char Time[128];
    sprintf(Time,"%d-%02d-%02d %02d:%02d:%02d",int(soltime[0]),int(soltime[1]),int(soltime[2]),int(soltime[3]),int(soltime[4]),int(soltime[5]));


    //SYS_GPS     0x01   0-31
    //SYS_GLO     0x04   32-58
    //SYS_GAL     0x08   59-94
    //SYS_CMP     0x20   105-167

//    int satenum[4][2]={{0,32},{105,168},{31,59},{59,95}};//GCRE

    int satnum[2]={0,0};//测站 基站 obs卫星数
    int sysnum[8]={0};
    char data[8][1024]={{'\0'},{'\0'},{'\0'},{'\0'},{'\0'},{'\0'},{'\0'},{'\0'}};

    for(int j=0;j<2;j++)
    {
        satnum[j]=obs[j]->n;
        for(int i=0;i<obs[j]->n;i++)
        {
            int sat=obs[j]->data[i].sat;//卫星编号
            int prn;
            double snr1=obs[j]->data[i].SNR[0]*0.001;
            double snr2=obs[j]->data[i].SNR[1]*0.001;
            int code1=obs[j]->data[i].code[0];
            int code2=obs[j]->data[i].code[1];


            //sprintf(onedate,"%03d%02d%4.1lf%02d%4.1lf ",prn,code1,snr1,code2,snr2);
            int x = 0;
            if(satsys(sat,&prn)==SYS_GPS)
            {
                x=0;
            }
            else if(satsys(sat,&prn)==SYS_CMP)
            {
                x=1;
            }
            else if(satsys(sat,&prn)==SYS_GLO)
            {
                x=2;
            }
            else if(satsys(sat,&prn)==SYS_GAL)
            {
                x=3;
            }

            char onedate[40];
            sprintf(onedate,"%02d_%02d_%4.1lf/%02d_%4.1lf\n",prn,code1,snr1,code2,snr2);
            strcat(data[j*4+x],onedate);
            sysnum[j*4+x]++;
            //strcat(data[j],onedate);



        }

    }


    QString sqlout="insert into ";
    sqlout.append(tablename);

    sqlout.append(" (time,rovernum,basenum,RGnum,RCnum,RRnum,REnum,BGnum,BCnum,BRnum,BEnum,RG_SNR,RC_SNR,RR_SNR,RE_SNR,BG_SNR,BC_SNR,BR_SNR,BE_SNR) values(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)");

    //int time =sol->time.time;

    query.prepare(sqlout);
    query.bindValue(0,Time);
    query.bindValue(1,satnum[0]);
    query.bindValue(2,satnum[1]);
    for(int i=0;i<8;i++)
    {
       query.bindValue(3+i,sysnum[i]);
       query.bindValue(11+i,data[i]);
    }
//    query.bindValue(3,sysnum[0]);
//    query.bindValue(3,data[0]);
//    query.bindValue(4,data[1]);
//    query.bindValue(5,data[0]);
//    query.bindValue(6,data[1]);
//    query.bindValue(7,data[2]);
//    query.bindValue(8,data[3]);
//qDebug("rover%s\n",data[0]);

    query.exec();

    query.clear();

    //qDebug()<<sqlout;
//qDebug("base%s\n",data[1]);







    return 0;
}

int NaviMain::sql_out_sky(rtk_t *rtk, char *tablename)
{
    //定义主键、列名、列类型
    //creat.append("(time DATETIME primary key,num_G int,num_C int,num_R int,num_E int,G_S_AzEL TEXT,G_S_AzEL TEXT,C_S_AzEL TEXT,R_S_AzEL TEXT,E_S_AzEL TEXT)");//时间 记录的卫星数  卫星编码_状态_方位角高度角 ("AABCCCCCDDDD ")
    double soltime[6]={0};
    //gpst2utc(sol->time);
    time2epoch(gpst2utc(rtk->sol.time),soltime);

    char Time[128];
    sprintf(Time,"%d-%02d-%02d %02d:%02d:%02d",int(soltime[0]),int(soltime[1]),int(soltime[2]),int(soltime[3]),int(soltime[4]),int(soltime[5]));

    ssat_t* ssat=rtk->ssat;

    //SYS_GPS     0x01   0-31       1-32
    //SYS_GLO     0x04   32-58      33-60
    //SYS_GAL     0x08   59-94      61-95
    //SYS_CMP     0x20   105-167    106-167

    int satenum[4][2]={{0,32},{105,168},{32,59},{59,95}};//GCRE
    int count[4]={0},prn[4]={0};

    char Gdata[1024]={"\0"},Cdata[1024]={"\0"},Rdata[1024]={"\0"},Edata[1024]={"\0"};
    char *data[4]={Gdata,Cdata,Rdata,Edata};

    for(int j=0;j<4;j++)
    {
        for(int i=satenum[j][0];i<satenum[j][1];i++)
        {

            prn[j]++;
            if(ssat[i].vs)
            //if(1)
            {
                count[j]++;

                char onedate[20];
                //onedate[0]='1';
                sprintf(onedate,"%02d_%05.1lf/%04.1lf\n",prn[j],ssat[i].azel[0]*R2D,ssat[i].azel[1]*R2D);

                strcat(data[j],onedate);

                 //qDebug()<<i<<ssat[i].sys<<" "<<ssat[i].azel[0]<<" "<<ssat[i].azel[1]<<" "<<ssat[i].vs;
             }
         }
        //qDebug()<<count[j];
        //qDebug("%s",data[j]);
        //printf("%s\n",data[j]);
    }

//    for(int i=0;i<MAXSAT;i++)
//    {
//        if(ssat[i].vs)
//        {
//            count[0]++;
//        }
//    }
//    qDebug()<<count[0];


    QString sqlout="insert into ";
    sqlout.append(tablename);

    sqlout.append(" (time,num_G,num_C,num_R,num_E,G_AzEL,C_AzEL,R_AzEL,E_AzEL) values(?,?,?,?,?,?,?,?,?)");

    //int time =sol->time.time;

    query.prepare(sqlout);
    query.bindValue(0,Time);
    query.bindValue(1,count[0]);
    query.bindValue(2,count[1]);
    query.bindValue(3,count[2]);
    query.bindValue(4,count[3]);
    query.bindValue(5,data[0]);
    query.bindValue(6,data[1]);
    query.bindValue(7,data[2]);
    query.bindValue(8,data[3]);

//    query.bindValue(9,sol->qr[3]);
//    query.bindValue(10,sol->qr[4]);
//    query.bindValue(11,sol->qr[5]);
//    query.bindValue(12,sol->age);
//    query.bindValue(13,sol->ratio);

    query.exec();

    query.clear();

    return 0;


}

int build_info(int prn,int vs,double* AzEl,double *SNR,char* text)
{

    char onedate[26];
    //onedate[0]='1';
    sprintf(onedate,"%02d_%d_%05.1lf_%04.1lf_%4.1lf_%4.1lf ",prn,vs,AzEl[0]*R2D,AzEl[1]*R2D,SNR[0],SNR[1]);

    strcat(text,onedate);




}




int NaviMain::sql_out_sat(rtksvr_t *svr, char *tablename,char* Time)
{


    int satnum[2]={0,0};//基站和测站卫星数量计数
    int sysnum[8]={0};




    char text[8][2048]={"\0"};

    //svr->rtk.ssat[i]与 svr.obs[i].data+i;


    //读取 基站i=1 和 流动站i=2 的OBS数据
    for(int i=0;i<2;i++)
    {

        for(int j=0;j<svr->obs[i]->n;j++)
        {
            satnum[i]=svr->obs[i]->n;

            obsd_t* data=svr->obs[i]->data+j;
            ssat_t* ssat=svr->rtk.ssat;

            //if(ssat[data->sat-1].vs)//判断卫星是否可用
            //{
                //

                int vs=0;

                double SNR[2]={0.0};
                double AzEl[2]={0.0};

                int sys=0;
                int prn=0;


                vs=ssat[data->sat-1].vs;

                AzEl[0]=ssat[data->sat-1].azel[0];
                AzEl[1]=ssat[data->sat-1].azel[1];
                SNR[0]=data->SNR[0]*0.001;
                SNR[1]=data->SNR[1]*0.001;

                sys=satsys(data->sat,&prn);

                switch (sys) {
                case SYS_GPS:sysnum[i*4+0]++;build_info(prn,vs,AzEl,SNR,text[i*4+0]);break;
                case SYS_CMP:sysnum[i*4+1]++;build_info(prn,vs,AzEl,SNR,text[i*4+1]);break;
                case SYS_GLO:sysnum[i*4+2]++;build_info(prn,vs,AzEl,SNR,text[i*4+2]);break;
                case SYS_GAL:sysnum[i*4+3]++;build_info(prn,vs,AzEl,SNR,text[i*4+3]);break;
                default: break;
               // }
            }
        }
    }


    QString sqlout="insert into ";
    sqlout.append(tablename);

    sqlout.append(" (time,rover_num,base_num,rg_num,rc_num,rr_num,re_num,bg_num,bc_num,br_num,be_num,rg_sat,rc_sat,rr_sat,re_sat,bg_sat,bc_sat,br_sat,be_sat) values(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)");
    //sqlout.append(" (time,rover_num,base_num,rg_num,rc_num,rr_num,re_num,bg_num,bc_num,br_num,be_num) values(?,?,?,?,?,?,?,?,?,?,?)");

    //int time =sol->time.time;

    query.prepare(sqlout);
    query.bindValue(0,Time);
    query.bindValue(1,satnum[0]);
    query.bindValue(2,satnum[1]);
    query.bindValue(3,sysnum[0]);
    query.bindValue(4,sysnum[1]);
    query.bindValue(5,sysnum[2]);
    query.bindValue(6,sysnum[3]);
    query.bindValue(7,sysnum[4]);
    query.bindValue(8,sysnum[5]);
    query.bindValue(9,sysnum[6]);
    query.bindValue(10,sysnum[7]);
    query.bindValue(11,text[0]);
    query.bindValue(12,text[1]);
    query.bindValue(13,text[2]);
    query.bindValue(14,text[3]);
    query.bindValue(15,text[4]);
    query.bindValue(16,text[5]);
    query.bindValue(17,text[6]);
    query.bindValue(18,text[7]);

    qDebug()<<Time<<satnum[0]<<satnum[1]<<sysnum[0]<<sysnum[1]<<sysnum[2]<<sysnum[3]<<sysnum[4]<<sysnum[5]<<sysnum[6]<<sysnum[7];
    qDebug()<<text[0];
            qDebug()<<text[1];
            qDebug()<<text[2];
            qDebug()<<text[3];
            qDebug()<<text[4];
            qDebug()<<text[5];
            qDebug()<<text[6];
            qDebug()<<text[7];

    query.exec();

    query.clear();

    return 0;
    //用OBS数据匹配卫星高度角与方位角，

}




// open monitor port --------------------------------------------------------
//void  NaviMain::OpenMoniPort(int port)
//{
//    QString s;
//    int i;
//    char path[64];

//    if (port<=0) return;

//    trace(3,"OpenMoniPort: port=%d\n",port);



//        sprintf(path,":%d",port+i);

//        if (stropen(&moni,STR_TCPSVR,STR_MODE_RW,path)) {
//            strsettimeout(&moni,10000,10000);
//            if (i>0) setWindowTitle(QString(tr("%1 ver.%2 (%3)")).arg(PRGNAME).arg(VER_RTKLIB).arg(i+1));

//            return;
//        }


//}
//static void *sendkeepalive(void *arg)
//{
//    trace(3,"sendkeepalive: start\n");

//    while (keepalive) {
//        strwrite(&moni,(uint8_t *)"\r",1);
//        sleepms(INTKEEPALIVE);
//    }
//    trace(3,"sendkeepalive: stop\n");
//    return NULL;
//}


//int NaviMain::OpenMoniPort(int port)
//{
//    pthread_t thread;
//    char path[64];

//    trace(3,"openmomi: port=%d\n",port);

//    sprintf(path,":%d",port);
//    if (!stropen(moni,STR_TCPSVR,STR_MODE_RW,path)) return 0;
//    strsettimeout(moni,10000,10000);
//    keepalive=1;
//    pthread_create(&thread,NULL,sendkeepalive,NULL);
//    return 1;
//}


// open monitor port --------------------------------------------------------
//void NaviMain::OpenMoniPort(int port)
//{
//    QString s;
//    int i;
//    char path[64];

//    if (port<=0) return;

//    trace(3,"OpenMoniPort: port=%d\n",port);

//    for (i=0;i<=MAXPORTOFF;i++) {

//        sprintf(path,":%d",port+i);

//        if (stropen(&monistr,STR_TCPSVR,STR_MODE_RW,path)) {
//            strsettimeout(&monistr,TimeoutTime,ReconTime);
//            if (i>0) setWindowTitle(QString(tr("%1 ver.%2 (%3)")).arg(PRGNAME).arg(VER_RTKLIB).arg(i+1));
//            OpenPort=MoniPort+i;
//            return;
//        }
//    }
//    QMessageBox::critical(this,tr("Error"),QString(tr("monitor port %1-%2 open error")).arg(port).arg(port+MAXPORTOFF));
//    OpenPort=0;
//}

