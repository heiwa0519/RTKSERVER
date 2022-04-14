/* -----------------------------------------------------------------
 *
 *
 *
 *
 *
 *
 *
 *
 * ----------------------------------------------------------------*/
#include<stdio.h>


#include"rtklib.h"
#include"server_mult.h"
#include"svrsettings.h"


/*  --------------------------------------------*/


/* write solution header to output stream ------------------------------------*/
static void writesolhead(stream_t *stream, const solopt_t *solopt)
{
    uint8_t buff[1024];
    int n;

    n=outsolheads(buff,solopt);
    strwrite(stream,buff,n);
}




/* 主循环函数 ------------------------------------------------------*/
extern int svrMain()
{

    //一个线程需要的结构体
    rtksvr_t *svr=new rtksvr_t;
    stream_t *moni=new stream_t;

    //创建更改参数需要的结构体并初始化
    svrio_t svrio=svrio_default;


    //初始化svr结构体和moni
    svrInit(svr,moni);
    printf("1\n");

    //载入必要的参数
    optLoad(&svrio);
    printf("2\n");

    //其他参数更改【未实现】
    //optChange(svr);

    //打开监视流【未实现】
    moniOpen(moni,moniport);

    //启动线程
    svrStart(svr,&svrio);
    printf("4\n");

    return 0;
}

extern int moniOpen(stream_t *moni,int moniport)
{

    if(moniport)
    {
        return 0;
    }

//    char path[64];

//    trace(3,"openmomi: port=%d\n",port);

//    sprintf(path,":%d",moniport);
//    if (!stropen(moni,STR_TCPSVR,STR_MODE_RW,path)) return 0;
//    strsettimeout(moni,timeout,reconnect);


    return 1;



}



/* svr初始化函数 ---------------------------------------------------*/
extern int svrInit(rtksvr_t *svr, stream_t *moni)
{
    //初始化svr、moni
    rtksvrinit(svr);
    strinit(moni);

    gtime_t time0={0};
    int i,j;


    if (svr->state) {
        //sprintf(errmsg,"server already started");
        return 0;
    }

    strinitcom();

    //可修改的默认参数
    svr->cycle=svrcycle>1?svrcycle:1;;
    svr->nmeacycle=nmeacycle>1000?nmeacycle:1000;
    svr->nmeareq=nmeareq;
    for (i=0;i<3;i++) svr->nmeapos[i]=nmeapos[i];
    svr->buffsize=buffsize>4096?buffsize:4096;
    for (i=0;i<3;i++) svr->format[i]=strfmt[i];

    svr->navsel=navsel;

    //需要初始化的参数

    svr->nsbs=0;      /* number of solution buffer */
    svr->nsol=0;      /* number of solution buffer */
    svr->prcout=0;    /* missing observation data count */



    rtkfree(&svr->rtk);
    rtkinit(&svr->rtk,&svr_prcopt_defult);


    if (svr_prcopt_defult.initrst) { /* init averaging pos by restart */
        svr->nave=0;
        for (i=0;i<3;i++) svr->rb_ave[i]=0.0;
    }



    for (i=0;i<3;i++) { /* input/log streams */
        svr->nb[i]=svr->npb[i]=0;
        if (!(svr->buff[i]=(uint8_t *)malloc(buffsize))||
            !(svr->pbuf[i]=(uint8_t *)malloc(buffsize))) {
            tracet(1,"rtksvrstart: malloc error\n");
            //sprintf(errmsg,"rtk server malloc error");
            return 0;
        }
        for (j=0;j<10;j++) svr->nmsg[i][j]=0;
        for (j=0;j<MAXOBSBUF;j++) svr->obs[i][j].n=0;
        strcpy(svr->cmds_periodic[i],!cmds_periodic[i]?"":cmds_periodic[i]);

        /* initialize receiver raw and rtcm control */
        init_raw(svr->raw+i,strfmt[i]);
        init_rtcm(svr->rtcm+i);

        /* set receiver and rtcm option */
        strcpy(svr->raw [i].opt,rcvopts[i]);
        strcpy(svr->rtcm[i].opt,rcvopts[i]);

        /* connect dgps corrections */
        svr->rtcm[i].dgps=svr->nav.dgps;
    }



    for (i=0;i<2;i++) { /* output peek buffer */
        if (!(svr->sbuf[i]=(uint8_t *)malloc(buffsize))) {
            tracet(1,"rtksvrstart: malloc error\n");
            //sprintf(errmsg,"rtk server malloc error");
            return 0;
        }
    }

    /* set solution options */
    for (i=0;i<2;i++) {
        svr->solopt[i]=svr_solopt_default;
    }
    /* set base station position */
    if (svr_prcopt_defult.refpos!=POSOPT_SINGLE) {
        for (i=0;i<6;i++) {
            svr->rtk.rb[i]=i<3?svr_prcopt_defult.rb[i]:0.0;
        }
    }
    /* update navigation data */
    for (i=0;i<MAXSAT*4 ;i++) svr->nav.eph [i].ttr=time0;
    for (i=0;i<NSATGLO*2;i++) svr->nav.geph[i].tof=time0;
    for (i=0;i<NSATSBS*2;i++) svr->nav.seph[i].tof=time0;

    /* set monitor stream */
    svr->moni=moni;


    return 1;
}


extern int optLoad(svrio_t *svrio)
{
    //rover
    svrio->type[0]=STR_TCPCLI;
    svrio->format[0]=STRFMT_RTCM3;
    char *path_0={"101.34.228.202:10002"};
    strcpy(svrio->paths[0],path_0);


    //base
    svrio->type[1]=STR_TCPCLI;
    svrio->format[1]=STRFMT_RTCM3;
    char *path_1={"101.34.228.202:10002"};
    strcpy(svrio->paths[1],path_0);


    //base坐标
    svrio->refpos=0;   /* base position for relative mode */
                             /* (0:pos in prcopt,  1:average of single pos, */
                             /*  2:read from file, 3:rinex header, 4:rtcm pos) */
    svrio->rb[0]= 0.0;
    svrio->rb[1]= 0.0;
    svrio->rb[2]= 0.0;


    //corr
//    svrio->type[2]=STR_TCPCLI;
//    svrio->format[2]=STRFMT_RTCM3;
//    char *path_2={"101.34.228.202:10002"};
//    strcpy(svrio->paths[2],path_0);


}


/* 创建svr线程 ---------------------------------------------------*/
extern int svrStart(rtksvr_t *svr,svrio_t *svrio)
{
    gtime_t time;
    int i,k,rw;

    /* open input streams */
    for (i=0;i<8;i++) {
        rw=i<3?STR_MODE_R:STR_MODE_W;
        if (svrio->type[i]!=STR_FILE) rw|=STR_MODE_W;
        if (!stropen(svr->stream+i,svrio->type[i],rw,svrio->paths[i])) {
            //sprintf(errmsg,"str%d open error path=%s",i+1,svrio->paths[i]);
            for (i--;i>=0;i--) strclose(svr->stream+i);
            return 0;
        }
        /* set initial time for rtcm and raw */
        if (i<3) {
            time=utc2gpst(timeget());
            svr->raw [i].time=svrio->type[i]==STR_FILE?strgettime(svr->stream+i):time;
            svr->rtcm[i].time=svrio->type[i]==STR_FILE?strgettime(svr->stream+i):time;
        }
    }
    /* sync input streams */
    strsync(svr->stream,svr->stream+1);
    strsync(svr->stream,svr->stream+2);

    /* write start commands to input streams */
    for (i=0;i<3;i++) {
        if (!cmds[i]) continue;
        strwrite(svr->stream+i,(uint8_t *)"",0); /* for connect */
        sleepms(100);
        strsendcmd(svr->stream+i,cmds[i]);
    }
    /* write solution header to solution streams */
    for (i=3;i<5;i++) {
        writesolhead(svr->stream+i,svr->solopt+i-3);
    }
    /* create rtk server thread */
    if (svrThreadCreat(svr)) {

        //sprintf(errmsg,"thread create error\n");
            printf("0\n");
        return 0;
    }

    printf("3\n");

    return 1;


}












