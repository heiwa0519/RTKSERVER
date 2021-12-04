#ifndef RECORDMAIN_H
#define RECORDMAIN_H
#include<QString>
#include<QDebug>
#include"rtklib.h"



#define MAXSTR        7    // number of streams
#define MAXTHREAD     30   //最大支持的strsvr线程数量


struct taskaddr
{
    char ID[128];
    int type=0;
    char path[MAXCHAR];
    taskaddr *next;
};

struct tasklists
{
    int n;
    taskaddr *tasks;
};




int loadini(int argc, char *argv[]);

int taskInit();

int taskCreat();

int resetopt();

int loadtasks(char *file,FILE fp);




class RecordMain: public QObject
{
public:
    RecordMain();
     ~RecordMain();

    strsvr_t *strsvr =new strsvr_t;

    int strs[MAXSTR]={0};//输入输出流
    //int opts[8]={0},n;//  设置选项

    char *paths[MAXSTR];//stream paths
    char *logs[MAXSTR];//log paths
    char str1[MAXSTR][1024],str2[MAXSTR][1024];//流路径  log路径

    strconv_t *conv[MAXSTR-1]={0};//stream converter

    char *cmds[MAXSTR]={0};//start/stop commands (NULL: no cmd)
    char *cmds_periodic[MAXSTR]={0};//periodic commands (NULL: no cmd)



     double AntPos[3],AntOff[3];



//     int itype[8]={
//		STR_SERIAL,STR_TCPCLI,STR_TCPSVR,STR_NTRIPCLI,STR_UDPSVR,STR_FILE,
//		STR_FTP,STR_HTTP
//	};
//	int otype[8]={
//		STR_NONE,STR_SERIAL,STR_TCPCLI,STR_TCPSVR,STR_NTRIPSVR,STR_NTRIPCAS,
//		STR_UDPCLI,STR_FILE
//	};


    //char *paths[MAXSTR],*logs[MAXSTR],*cmds[MAXSTR]={0},*cmds_periodic[MAXSTR]={0};
    //流路径  log路径   命令   周期命令
    char filepath[1024],buff[1024],*p;
    const char *ant[3]={"","",""},*rcv[3]={"","",""};
    FILE *fp;



    int svrinit();
    int svrstart();


private:




};

#endif // RECORDMAIN_H
