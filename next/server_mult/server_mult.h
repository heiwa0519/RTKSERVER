#ifndef SERVER_MULT_H
#define SERVER_MULT_H



#include<stdio.h>
#include"rtklib.h"


/* 宏定义 -----------------------------------------------------------------*/

#define MAXSTR      1024                /* max length of a stream */




/* 结构体 -----------------------------------------------------------------*/

struct svrio_t{           /* svr input/sol/log stream type */
    int type[8];           /* svr input(0-2)/sol(3-4)/log(5-7) stream type */
    int format[8];         /* svr input(0-2) / sol(3-4) / log(5-7) stream formats */
                           /*   ( STRFMT_??? / SOLF_??? / 0, 0 ,0  )   */

    char paths[8][MAXSTR]; /*svr input(0-2) / sol(3-4) / log(5-7) stream path*/
    int refpos;            /* base position for relative mode */
                           /* (0:pos in prcopt,  1:average of single pos, */
                           /*  2:read from file, 3:rinex header, 4:rtcm pos) */
    double rb[3];          /* base position */

};






/* 全局变量 ---------------------------------------------------------------*/

extern const prcopt_t svr_prcopt_defult;

extern const solopt_t svr_solopt_default;

extern const int strtype[8],strfmt[8],svrcycle,
                 timeout,reconnect,nmeacycle,
                 fswapmargin,buffsize,navmsgsel,
                 nmeareq;

extern const double nmeapos[3];

extern char strpath[8][MAXSTR],
            proxyaddr[MAXSTR],
            *paths[8],
            *cmds[3],
            *cmds_periodic[3],
            *ropts[3],
            errmsg[20148];



/* 函数 ------------------------------------------------------------------*/

//主循环函数
extern int svrMain();

//任务输入函数



//任务初始化与线程创建函数
extern int svrInit(rtksvr_t *svr,stream_t *moni);

extern int optInit(svrio_t *svrio);

extern int optLoad(svrio_t *svr);

extern int moniOpen(stream_t *moni,int moniport);

extern int svrStart(rtksvr_t *svr,svrio_t *svrio);

//输出函数







#endif // SERVER_MULT_H

