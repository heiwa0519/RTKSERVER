#include "recordmain.h"





#define ISTOPT  "0:off,1:serial,2:file,3:tcpsvr,4:tcpcli,6:ntripcli,7:ftp,8:http"
#define OSTOPT  "0:off,1:serial,2:file,3:tcpsvr,4:tcpcli,5:ntripsvr,9:ntrcaster"

//static int n;   //线程数，实际运行的strsvr任务数量



static char TaskPath[MAXCHAR];//任务文件的路径


static int Opt[8];

static int Reconnect    =10000;         /* reconnect interval (ms) */
static int Timeout      =10000;         /* timeout time (ms) */
static int Periodrate   =1000;          /* period of rate (ms) */
static int Buffsize     =32768;         /* input buffer size (bytes) */
static int Svrcycle     =10;            /* server cycle (ms) */
static int NMEAcycle    =0;             /* NMEA Cycle (ms)*/
static int Fswapmargin  =30;            /* file swap marign (s) */
static int Relayback    =0;             /* relay back of output stream (0:no) */

static char Logdirectory[MAXCHAR];      /* log directory                                 ::T    */
static char Fimenameformat[MAXCHAR];    /* filename format ( \0.25\0.5\1\2\3\6\12\24) H  ::S=24 */
static int  Swipintv=24;                /* Swip Tntv */
static int  Timetig=1;                  /* Time Tag  (0:off,1:on) */

static char InputID[MAXCHAR];
static int  Inputtype;
static char Inputpath[MAXCHAR];


static opt_t iniopts[]={
    {"task-path",       2,  (void *)TaskPath,         ""     },
};

static opt_t Taskopts[]={
    {"opt1-0",       0,  (void *)&Timeout,         ""     },
    {"opt1-1",       0,  (void *)&Reconnect,         ""     },
    {"opt1-2",       0,  (void *)&Periodrate,         ""     },
    {"opt1-3",       0,  (void *)&Buffsize,         ""     },
    {"opt1-4",       0,  (void *)&Svrcycle,         ""     },
    {"opt1-5",       0,  (void *)&NMEAcycle,         ""     },
    {"opt1-6",       0,  (void *)&Fswapmargin,         ""     },
    {"opt1-7",       0,  (void *)&Relayback,         ""     },
    {"opt2-1",       2,  (void *)Logdirectory,         ""     },
    {"opt2-2",       2,  (void *)Fimenameformat,         ""     },
    {"opt2-3",       0,  (void *)&Swipintv,         ""     },
    {"opt2-4",       0,  (void *)&Timetig,         ""     },

    {"",0,NULL,""}
};

static opt_t Tasklist[]={
    {"input-ID",       2,  (void *)InputID,         ""     },
    {"input-type",     3,  (void *)&Inputtype,         ISTOPT     },
    {"input-path",     2,  (void *)Inputpath,         ""     },

    {"",0,NULL,""}
};







static tasklists list;

int loadini(int argc, char *argv[])
{


    strncpy(TaskPath,"G:/RTKSERVER/next/rtk_record/record.list",MAXCHAR);

//    if(argc)
//    {
//        loadopts(*argv,iniopts);
//    }
//    else
//    {
//        #ifdef WIN32
//        loadopts("G:/RTKSERVER/next/rtk_record/record.list",iniopts);
//        #else
//        loadopts("option.ini",iniopts);
//        #endif
//    }

    //loadopts(TaskPath,Taskopts);
    //loadopts(TaskPath,Tasklist);


//新建一个读取函数，能够读取多个任务的路径？

//    qDebug()<<"conf  file path:"<<ConfPath;
    //    qDebug()<<"dbopt file path:"<<DbOptPath;

    return 0;
}

int taskInit()
{

    //读取基本参数设置  超时重连时间等

    //读取文件存放路径和格式 可添加%b%r来实现替换为站点名

    //设置单个文件记录时长

    //是否开启时间戳
    loadopts(TaskPath,Taskopts);



    Opt[0]=Timeout;
    Opt[1]=Reconnect;
    Opt[2]=Periodrate;
    Opt[3]=Buffsize;
    Opt[4]=Svrcycle;
    Opt[5]=NMEAcycle;
    Opt[6]=Fswapmargin;
    Opt[7]=Relayback;

    //是否使用64bit


    //loadtasks(TaskPath,);
    //读取流的站点名、类型、地址，并判断流个数，将各个流设置存入结构体

    //将结构体内的站点%b名先进行替换（因为strsvr里面的替换并不替换站点名）


    //关闭文件 fclose


//    strcpy(rpath,path);

//    if (!strstr(rpath,"%")) return 0;
//    if (*rov ) stat|=repstr(rpath,"%r",rov );
//    if (*base) stat|=repstr(rpath,"%b",base);


    return 0;
}


int taskCreat()
{
    FILE *fp;
    char buff[2048];

    //打开文件
    if (!(fp=fopen(TaskPath,"r"))) {
        trace(1,"loadopts: options file open error (%s)\n",TaskPath);
        return 0;
    }
    //读取到 opt-end 的下一行  获取其指针
    do
    {

        fgets(buff,sizeof(buff),fp);

    }
    while (!strstr(buff,"opt-end"));

    //将读取完的参数读取参数

    while(!strstr(buff,"streams-end"))
    {

        //新建一个对象
        RecordMain w;


        //读取参数strsvr所需的各项参数，补全参数，返回strsvr结构体的地址
        w.svrinit();

        //程序启动
        w.svrstart();


    }




    return 0;
}







RecordMain::RecordMain()
{

}

RecordMain::~RecordMain()
{

}

int RecordMain::svrinit()
{

    //将对象内的参数初始化，包括流设置和各个数据流参数设置

    //将读取到的参数写入对象

    //初始化流参数，名称类型地址等


    //这个函数实现所有函数的准备工作
    //strsvrstart(strsvr,opts,strs,paths,logs,conv,cmds,cmds_periodic,AntPos);





//    *          int    *strs     I   stream types (STR_???)
//    *              strs[0]= input stream
//    *              strs[1]= output stream 1
//    *              strs[2]= output stream 2
//    *              strs[3]= output stream 3
//    *              ...




//    *          char   **paths   I   stream paths
//    *              paths[0]= input stream
//    *              paths[1]= output stream 1
//    *              paths[2]= output stream 2
//    *              paths[3]= output stream 3
//    *              ...



//    *          char   **logs    I   log paths
//    *              logs[0]= input log path
//    *              logs[1]= output stream 1 return log path
//    *              logs[2]= output stream 2 retrun log path
//    *              logs[3]= output stream 2 retrun log path
//    *              ...



//    *          strconv_t **conv I   stream converter
//    *              conv[0]= output stream 1 converter
//    *              conv[1]= output stream 2 converter
//    *              conv[2]= output stream 3 converter
//    *              ...



//    *          char   **cmds    I   start/stop commands (NULL: no cmd)
//    *              cmds[0]= input stream command
//    *              cmds[1]= output stream 1 command
//    *              cmds[2]= output stream 2 command
//    *              cmds[3]= output stream 3 command
//    *              ...



//    *          char   **cmds_periodic I periodic commands (NULL: no cmd)
//    *              cmds[0]= input stream command
//    *              cmds[1]= output stream 1 command
//    *              cmds[2]= output stream 2 command
//    *              cmds[3]= output stream 3 command
//    *              ...




//    *          double *nmeapos  I   nmea request position (ecef) (m) (NULL: no)


    //log opts

    //strs

    //paths

    //conv

    //cmds cmds_periodic

    //antpos

     return 0;
}

int RecordMain::svrstart()
{

    strsvrstart(strsvr,Opt,strs,paths,logs,conv,cmds,cmds_periodic,AntPos);

     return 0;
}







int resetopt()
{
    Reconnect    =10000;         /* reconnect interval (ms) */
    Timeout      =10000;         /* timeout time (ms) */
    Periodrate   =1000;          /* period of rate (ms) */
    Buffsize     =32768;         /* input buffer size (bytes) */
    Svrcycle     =10;            /* server cycle (ms) */
    NMEAcycle    =0;             /* NMEA Cycle (ms)*/
    Fswapmargin  =30;            /* file swap marign (s) */
    Relayback    =0;             /* relay back of output stream (0:no) */

    Logdirectory[0]='\0';      /* log directory                                 ::T    */
    Fimenameformat[0]='\0';    /* filename format ( \0.25\0.5\1\2\3\6\12\24) H  ::S=24 */
    Swipintv=24;                /* Swip Tntv */
    Timetig=1;                  /* Time Tag  (0:off,1:on) */
}


static void chop(char *str)
{
    char *p;
    if ((p=strchr(str,'#'))) *p='\0'; /* comment */
    for (p=str+strlen(str)-1;p>=str&&!isgraph((int)*p);p--) *p='\0';
}

int loadtasks(char *file, tasklists *list)
{

    char buff[2048],*p;
    opt_t *opt;
    //建立taskaddr指针
    taskaddr addr,*addrp;
    addr.next=nullptr;
    addrp=&addr;

    list->tasks=&addr;

    //打开文件
    FILE *fp;
    if (!(fp=fopen(file,"r"))) {
        trace(1,"loadopts: options file open error (%s)\n",file);
        return 0;
    }

    //读取到 opt-end 的下一行  获取其指针
    do
    {

        fgets(buff,sizeof(buff),fp);

    }
    while (!strstr(buff,"opt-end"));

    while(fgets(buff,sizeof(buff),fp)){

        if(strstr(buff,"input-num"))

        if(strstr(buff,"input-ID"))
        {


            for(int i=0;i<3;i++){
                chop(buff);
                if ((opt=searchopt(buff,Tasklist))) continue;
                if (!str2opt(opt,p)) {
                    printf("invalid option value %s (%s:%d)\n",buff,file,list->n);
                    continue;
                }
            }
            strncpy(addrp->ID,InputID,MAXSTR);
            addrp->type=Inputtype;
            strncpy(addrp->path,Inputpath,MAXSTR);


            list->n++;

            taskaddr addrnext;
            addrnext.next=nullptr;
            addrp->next=&addrnext;
            addrp=&addrnext;

        }
        else if(strstr(buff,"streams-end"))
        {
            addrp->next=nullptr;
            break;
        }
    }


    //循环函数  如果读取到 stream-end 则停止循环

    //新建一个taskaddr

    //如果是ID  存入ID

    //如果是type  存入type

    //如果是path  存入 path



    return 0;
}
