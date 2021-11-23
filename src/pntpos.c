/*------------------------------------------------------------------------------
* pntpos.c : standard positioning
*
*          Copyright (C) 2007-2020 by T.TAKASU, All rights reserved.
*
* version : $Revision:$ $Date:$
* history : 2010/07/28 1.0  moved from rtkcmn.c
*                           changed api:
*                               pntpos()
*                           deleted api:
*                               pntvel()
*           2011/01/12 1.1  add option to include unhealthy satellite
*                           reject duplicated observation data
*                           changed api: ionocorr()
*           2011/11/08 1.2  enable snr mask for single-mode (rtklib_2.4.1_p3)
*           2012/12/25 1.3  add variable snr mask
*           2014/05/26 1.4  support galileo and beidou
*           2015/03/19 1.5  fix bug on ionosphere correction for GLO and BDS
*           2018/10/10 1.6  support api change of satexclude()
*           2020/11/30 1.7  support NavIC/IRNSS in pntpos()
*                           no support IONOOPT_LEX option in ioncorr()
*                           improve handling of TGD correction for each system
*                           use E1-E5b for Galileo dual-freq iono-correction
*                           use API sat2freq() to get carrier frequency
*                           add output of velocity estimation error in estvel()
*-----------------------------------------------------------------------------*/
#include "rtklib.h"

/* constants/macros ----------------------------------------------------------*/

#define SQR(x)      ((x)*(x))

#if 0 /* enable GPS-QZS time offset estimation */
#define NX          (4+5)       /* # of estimated parameters */
#else
#define NX          (4+4)       /* # of estimated parameters */
#endif
#define MAXITR      10          /* max number of iteration for point pos */
#define ERR_ION     5.0         /* ionospheric delay Std (m) */
#define ERR_TROP    3.0         /* tropspheric delay Std (m) */
#define ERR_SAAS    0.3         /* Saastamoinen model error Std (m) */
#define ERR_BRDCI   0.5         /* broadcast ionosphere model error factor */
#define ERR_CBIAS   0.3         /* code bias error Std (m) */
#define REL_HUMI    0.7         /* relative humidity for Saastamoinen model */
#define MIN_EL      (5.0*D2R)   /* min elevation for measurement error (rad) */

/* pseudorange measurement error variance ------------------------------------*/
static double varerr(const prcopt_t *opt, double el, int sys)
{
    double fact,varr;
    fact=sys==SYS_GLO?EFACT_GLO:(sys==SYS_SBS?EFACT_SBS:EFACT_GPS);
    if (el<MIN_EL) el=MIN_EL;
    varr=SQR(opt->err[0])*(SQR(opt->err[1])+SQR(opt->err[2])/sin(el));
    //prcopt_default;  err[5]={100.0,0.003,0.003,0.0,1.0}  第一个值是默认的100，后四个值可以在conf文件修改，具体可参考说明书（测量值引起的std 高度角引起的std 基线长度引起的std 和  多普勒测量的std
    if (opt->ionoopt==IONOOPT_IFLC) varr*=SQR(3.0); /* iono-free */
    return SQR(fact)*varr;
}
/* get group delay parameter (m) ---------------------------------------------*/
static double gettgd(int sat, const nav_t *nav, int type)
{
    int i,sys=satsys(sat,NULL);
    
    if (sys==SYS_GLO) {
        for (i=0;i<nav->ng;i++) {
            if (nav->geph[i].sat==sat) break;
        }
        return (i>=nav->ng)?0.0:-nav->geph[i].dtaun*CLIGHT;
    }
    else {
        for (i=0;i<nav->n;i++) {
            if (nav->eph[i].sat==sat) break;
        }
        return (i>=nav->n)?0.0:nav->eph[i].tgd[type]*CLIGHT;
    }
}
/* test SNR mask -------------------------------------------------------------*/
static int snrmask(const obsd_t *obs, const double *azel, const prcopt_t *opt)
{
    if (testsnr(0,0,azel[1],obs->SNR[0]*SNR_UNIT,&opt->snrmask)) {
        return 0;
    }
    if (opt->ionoopt==IONOOPT_IFLC) {
        if (testsnr(0,1,azel[1],obs->SNR[1]*SNR_UNIT,&opt->snrmask)) return 0;
    }
    return 1;
}
/* psendorange with code bias correction -------------------------------------*/
static double prange(const obsd_t *obs, const nav_t *nav, const prcopt_t *opt,
                     double *var)
{
    double P1,P2,gamma,b1,b2;//双频观测值、
    int sat,sys;
    
    sat=obs->sat;
    sys=satsys(sat,NULL);
    P1=obs->P[0];
    P2=obs->P[1];
    *var=0.0;
    
    if (P1==0.0||(opt->ionoopt==IONOOPT_IFLC&&P2==0.0)) return 0.0;//如果频段一没有数据，或设置是IONOOPT_IFLC模式但是频段二没有数据，则返回0.0  表示这颗卫星的数据舍弃
    
    /* P1-C1,P2-C2 DCB correction */ //如果是GPS或者GLONASS 用频间DCB来改正（应该是基于DCB文件或RTCM增强信息）
    if (sys==SYS_GPS||sys==SYS_GLO) {
        if (obs->code[0]==CODE_L1C) P1+=nav->cbias[sat-1][1]; /* C1->P1 */
        if (obs->code[1]==CODE_L2C) P2+=nav->cbias[sat-1][2]; /* C2->P2 */
    }
    if (opt->ionoopt==IONOOPT_IFLC) { /* dual-frequency */
        
        if (sys==SYS_GPS||sys==SYS_QZS) { /* L1-L2,G1-G2 */
            gamma=SQR(FREQ1/FREQ2);
            return (P2-gamma*P1)/(1.0-gamma);
        }
        else if (sys==SYS_GLO) { /* G1-G2 */
            gamma=SQR(FREQ1_GLO/FREQ2_GLO);
            return (P2-gamma*P1)/(1.0-gamma);
        }
        else if (sys==SYS_GAL) { /* E1-E5b */
            gamma=SQR(FREQ1/FREQ7);
            if (getseleph(SYS_GAL)) { /* F/NAV */
                P2-=gettgd(sat,nav,0)-gettgd(sat,nav,1); /* BGD_E5aE5b */
            }
            return (P2-gamma*P1)/(1.0-gamma);
        }
        else if (sys==SYS_CMP) { /* B1-B2 */
            gamma=SQR(((obs->code[0]==CODE_L2I)?FREQ1_CMP:FREQ1)/FREQ2_CMP);
            if      (obs->code[0]==CODE_L2I) b1=gettgd(sat,nav,0); /* TGD_B1I */
            else if (obs->code[0]==CODE_L1P) b1=gettgd(sat,nav,2); /* TGD_B1Cp */
            else b1=gettgd(sat,nav,2)+gettgd(sat,nav,4); /* TGD_B1Cp+ISC_B1Cd */
            b2=gettgd(sat,nav,1); /* TGD_B2I/B2bI (m) */
            return ((P2-gamma*P1)-(b2-gamma*b1))/(1.0-gamma);
        }
        else if (sys==SYS_IRN) { /* L5-S */
            gamma=SQR(FREQ5/FREQ9);
            return (P2-gamma*P1)/(1.0-gamma);
        }
    }
    else { /* single-freq (L1/E1/B1) */
        *var=SQR(ERR_CBIAS);
        
        if (sys==SYS_GPS||sys==SYS_QZS) { /* L1 */
            b1=gettgd(sat,nav,0); /* TGD (m) */
            return P1-b1;
        }
        else if (sys==SYS_GLO) { /* G1 */
            gamma=SQR(FREQ1_GLO/FREQ2_GLO);
            b1=gettgd(sat,nav,0); /* -dtaun (m) */
            return P1-b1/(gamma-1.0);
        }
        else if (sys==SYS_GAL) { /* E1 */
            if (getseleph(SYS_GAL)) b1=gettgd(sat,nav,0); /* BGD_E1E5a */
            else                    b1=gettgd(sat,nav,1); /* BGD_E1E5b */
            return P1-b1;
        }
        else if (sys==SYS_CMP) { /* B1I/B1Cp/B1Cd */
            if      (obs->code[0]==CODE_L2I) b1=gettgd(sat,nav,0); /* TGD_B1I */
            else if (obs->code[0]==CODE_L1P) b1=gettgd(sat,nav,2); /* TGD_B1Cp */
            else b1=gettgd(sat,nav,2)+gettgd(sat,nav,4); /* TGD_B1Cp+ISC_B1Cd */
            return P1-b1;
        }
        else if (sys==SYS_IRN) { /* L5 */
            gamma=SQR(FREQ9/FREQ5);
            b1=gettgd(sat,nav,0); /* TGD (m) */
            return P1-gamma*b1;
        }
    }
    return P1;
}
/* ionospheric correction ------------------------------------------------------
* compute ionospheric correction
* args   : gtime_t time     I   time
*          nav_t  *nav      I   navigation data
*          int    sat       I   satellite number
*          double *pos      I   receiver position {lat,lon,h} (rad|m)
*          double *azel     I   azimuth/elevation angle {az,el} (rad)
*          int    ionoopt   I   ionospheric correction option (IONOOPT_???)
*          double *ion      O   ionospheric delay (L1) (m)
*          double *var      O   ionospheric delay (L1) variance (m^2)
* return : status(1:ok,0:error)
*-----------------------------------------------------------------------------*/
extern int ionocorr(gtime_t time, const nav_t *nav, int sat, const double *pos,
                    const double *azel, int ionoopt, double *ion, double *var)
{
    trace(4,"ionocorr: time=%s opt=%d sat=%2d pos=%.3f %.3f azel=%.3f %.3f\n",
          time_str(time,3),ionoopt,sat,pos[0]*R2D,pos[1]*R2D,azel[0]*R2D,
          azel[1]*R2D);
    
    /* GPS broadcast ionosphere model */
    if (ionoopt==IONOOPT_BRDC) {
        *ion=ionmodel(time,nav->ion_gps,pos,azel);
        *var=SQR(*ion*ERR_BRDCI);
        return 1;
    }
    /* SBAS ionosphere model */
    if (ionoopt==IONOOPT_SBAS) {
        return sbsioncorr(time,nav,pos,azel,ion,var);
    }
    /* IONEX TEC model */
    if (ionoopt==IONOOPT_TEC) {
        return iontec(time,nav,pos,azel,1,ion,var);
    }
    /* QZSS broadcast ionosphere model */
    if (ionoopt==IONOOPT_QZS&&norm(nav->ion_qzs,8)>0.0) {
        *ion=ionmodel(time,nav->ion_qzs,pos,azel);
        *var=SQR(*ion*ERR_BRDCI);
        return 1;
    }
    *ion=0.0;
    *var=ionoopt==IONOOPT_OFF?SQR(ERR_ION):0.0;
    return 1;
}
/* tropospheric correction -----------------------------------------------------
* compute tropospheric correction
* args   : gtime_t time     I   time
*          nav_t  *nav      I   navigation data
*          double *pos      I   receiver position {lat,lon,h} (rad|m)
*          double *azel     I   azimuth/elevation angle {az,el} (rad)
*          int    tropopt   I   tropospheric correction option (TROPOPT_???)
*          double *trp      O   tropospheric delay (m)
*          double *var      O   tropospheric delay variance (m^2)
* return : status(1:ok,0:error)
*-----------------------------------------------------------------------------*/
extern int tropcorr(gtime_t time, const nav_t *nav, const double *pos,
                    const double *azel, int tropopt, double *trp, double *var)
{
    trace(4,"tropcorr: time=%s opt=%d pos=%.3f %.3f azel=%.3f %.3f\n",
          time_str(time,3),tropopt,pos[0]*R2D,pos[1]*R2D,azel[0]*R2D,
          azel[1]*R2D);
    
    /* Saastamoinen model */
    if (tropopt==TROPOPT_SAAS||tropopt==TROPOPT_EST||tropopt==TROPOPT_ESTG) {
        *trp=tropmodel(time,pos,azel,REL_HUMI);
        *var=SQR(ERR_SAAS/(sin(azel[1])+0.1));
        return 1;
    }
    /* SBAS (MOPS) troposphere model */
    if (tropopt==TROPOPT_SBAS) {
        *trp=sbstropcorr(time,pos,azel,var);
        return 1;
    }
    /* no correction */
    *trp=0.0;
    *var=tropopt==TROPOPT_OFF?SQR(ERR_TROP):0.0;
    return 1;
}
/* pseudorange residuals -----------------------------------------------------*/
/*
 * x={x，y，z，0，0，0，0，0} 传入前赋值了接收机ecef的坐标
 * v=mat(n+4,1);
 * H=mat(NX,n+4);矩阵应当是 NX行 n+4列  但是实际构建的时候构建的是H的转置
 * var=mat(n+4,1);伪距测量误差的方差，对流层、电离层、高度角、测量误差等引起的误差的方差
 *
 * 迭代次数、OBS、OBS数量、卫星位置和速度
 * 卫星钟差、卫星位置和时间引起误差的方差、卫星健康标志
 * nav、系统状态量、opt
 * 残差矩阵（未进行权值改正）、系统观测矩阵（未进行权值改正）、伪距测量误差的方差、方位角高度角、观测值可用性
 * 伪距残差、经过筛选剔除后的实际可用卫星数
*/
static int rescode(int iter, const obsd_t *obs, int n, const double *rs,
                   const double *dts, const double *vare, const int *svh,
                   const nav_t *nav, const double *x, const prcopt_t *opt,
                   double *v, double *H, double *var, double *azel, int *vsat,
                   double *resp, int *ns)
{
    gtime_t time;
    double r,freq,dion,dtrp,vmeas,vion,vtrp,rr[3],pos[3],dtr,e[3],P;
    /* r,卫星和接收机的几何距离（进行了sagnac改正）
     * freq,频率
     * dion,电离层延迟
     * dtrp,对流层延迟
     * vmeas,伪距码偏差标准差，双频IONOOPT_IFLC模式为0，单频为ERR_CBIAS*ERR_CBIAS =0.09     code bias error Std (m)
     * vion,电离层延迟误差
     * vtrp,对流层延迟误差
     * rr[3],接收机 ECEF坐标
     * pos[3],接收机 纬度lat、经度lon、大地高h
     * dtr=x[3],x的第四个元素是GPS系统钟差
     * e[3],接收机指向卫星的单位向量（ECEF坐标系下）
     * P;经过码偏差改正后的伪距（双频模型、频间DCB，单频改正），*/

    int i,j,nv=0,sat,sys,mask[NX-3]={0};
    /* nv 构建的伪距观测值残差的数量，因为把多系统的钟差分别作为参数来估计，为了保证构建的矩阵满秩，需要再多构建一些方程来约束，所以和实际使用的卫星数ns有区别
     * sat卫星的编号范围1-MAX(区别于 prn prn是系统内的编号)
     * sys卫星系统
     * mask表示系统的标志，值为1表示有这个系统，01234分别表示GRECI五个系统
    */
    
    trace(3,"resprng : n=%d\n",n);
    
    for (i=0;i<3;i++) rr[i]=x[i];
    dtr=x[3];
    
    ecef2pos(rr,pos);
    
    for (i=*ns=0;i<n&&i<MAXOBS;i++) {    //对所有的n颗卫星进行筛选，剔除重复数据、判断卫星是否可用、 ns表示可用的卫星数量
        vsat[i]=0; azel[i*2]=azel[1+i*2]=resp[i]=0.0;
        time=obs[i].time;
        sat=obs[i].sat;
        if (!(sys=satsys(sat,NULL))) continue;
        
        /* reject duplicated observation data *///剔除重复的卫星观测值数据
        if (i<n-1&&i<MAXOBS-1&&sat==obs[i+1].sat) {
            trace(2,"duplicated obs data %s sat=%d\n",time_str(time,3),sat);
            i++;
            continue;
        }
        /* excluded satellite? */
        if (satexclude(sat,vare[i],svh[i],opt)) continue;//根据 1、opt的设置来加入卫星和剔除卫星，2、卫星健康标志vsh来删除卫星 3、卫星历元误差的方差vare与宏定义的值MAX_VAR_EPH进行比较剔除卫星
        
        /* geometric distance */
        if ((r=geodist(rs+i*6,rr,e))<=0.0) continue;//根据卫星位置和接收机的预设值/前一次迭代值 来得到几何距离r（r除计算几何距离外还进行了sagnac改正）和接收机指向卫星的单位向量e
        
        if (iter>0) {//iter是传入的参数 指当前迭代的次数，即 从第二次迭代才进入这个函数处理，（第一次处理先得到一个初始值才能计算这些改正值？）
            /* test elevation mask */
            if (satazel(pos,e,azel+i*2)<opt->elmin) continue;
            
            /* test SNR mask */
            if (!snrmask(obs+i,azel+i*2,opt)) continue;
            
            /* ionospheric correction */
            if (!ionocorr(time,nav,sat,pos,azel+i*2,opt->ionoopt,&dion,&vion)) {
                continue;
            }
            if ((freq=sat2freq(sat,obs[i].code[0],nav))==0.0) continue;
            dion*=SQR(FREQ1/freq);
            vion*=SQR(FREQ1/freq);
            
            /* tropospheric correction */
            if (!tropcorr(time,nav,pos,azel+i*2,opt->tropopt,&dtrp,&vtrp)) {
                continue;
            }
        }
        /* psendorange with code bias correction */
        if ((P=prange(obs+i,nav,opt,&vmeas))==0.0) continue;//如果没有成功得到改正后的伪距，导致伪距值为0，则跳过这颗卫星
        
        /* pseudorange residual */
        v[nv]=P-(r+dtr-CLIGHT*dts[i*2]+dion+dtrp);//根据改正后的伪距值得到伪距残差v  dtr是GPS系统的钟差 如果是其他系统，则再进行一个改正，引入一个新参数估计
        
        /* design matrix */
        for (j=0;j<NX;j++) {
            H[j+nv*NX]=j<3?-e[j]:(j==3?1.0:0.0);
        }
        /* time system offset and receiver bias correction *///根据不同的系统，对伪距残差v 再进行一次修正，减去系统间的钟差引起的距离误差
        if      (sys==SYS_GLO) {v[nv]-=x[4]; H[4+nv*NX]=1.0; mask[1]=1;}
        else if (sys==SYS_GAL) {v[nv]-=x[5]; H[5+nv*NX]=1.0; mask[2]=1;}
        else if (sys==SYS_CMP) {v[nv]-=x[6]; H[6+nv*NX]=1.0; mask[3]=1;}
        else if (sys==SYS_IRN) {v[nv]-=x[7]; H[7+nv*NX]=1.0; mask[4]=1;}
#if 0 /* enable QZS-GPS time offset estimation */
        else if (sys==SYS_QZS) {v[nv]-=x[8]; H[8+nv*NX]=1.0; mask[5]=1;}
#endif
        else mask[0]=1;
        
        vsat[i]=1; resp[i]=v[nv]; (*ns)++;
        
        /* variance of pseudorange error *///伪距误差的方差=opt人为预设的测量误差方差+卫星伪距的测量误差的方差+伪距码偏差误差方差+电离层误差方差+对流层误差方差
        var[nv++]=varerr(opt,azel[1+i*2],sys)+vare[i]+vmeas+vion+vtrp;
        
        trace(4,"sat=%2d azel=%5.1f %4.1f res=%7.3f sig=%5.3f\n",obs[i].sat,
              azel[i*2]*R2D,azel[1+i*2]*R2D,resp[i],sqrt(var[nv-1]));
    }
    /* constraint to avoid rank-deficient *///约束避免秩亏，保证满秩
    for (i=0;i<NX-3;i++) {
        if (mask[i]) continue;//如果所有卫星系统都用上了，构建的H就满秩，不需要再添加约束，nv=ns  如果有x个没用上的系统则再构建x行  最多4个系统没用上,这也解释了为什么最多有n+4个v、var和H，
        v[nv]=0.0;
        for (j=0;j<NX;j++) H[j+nv*NX]=j==i+3?1.0:0.0;//新构建一组值 令v[]=0.0 var[]=0.001  H`={0，0，0，G，R，E，C，I} (哪个系统没用上，哪个值为1），最多4个系统没用上（也就是只用一个系统，需要再添加4个约束）
        var[nv++]=0.01;
    }
    return nv;
}
/* validate solution ---------------------------------------------------------*/
static int valsol(const double *azel, const int *vsat, int n,
                  const prcopt_t *opt, const double *v, int nv, int nx,
                  char *msg)
{
    double azels[MAXOBS*2],dop[4],vv;
    int i,ns;
    
    trace(3,"valsol  : n=%d nv=%d\n",n,nv);
    
    /* Chi-square validation of residuals */
    vv=dot(v,v,nv);
    if (nv>nx&&vv>chisqr[nv-nx-1]) {
        sprintf(msg,"chi-square error nv=%d vv=%.1f cs=%.1f",nv,vv,chisqr[nv-nx-1]);
        return 0;
    }
    /* large GDOP check */
    for (i=ns=0;i<n;i++) {
        if (!vsat[i]) continue;
        azels[  ns*2]=azel[  i*2];
        azels[1+ns*2]=azel[1+i*2];
        ns++;
    }
    dops(ns,azels,opt->elmin,dop);
    if (dop[0]<=0.0||dop[0]>opt->maxgdop) {
        sprintf(msg,"gdop error nv=%d gdop=%.1f",nv,dop[0]);
        return 0;
    }
    return 1;
}
/* estimate receiver position ------------------------------------------------*/
/*
 * obs观测值、卫星数、卫星的位置和速度、卫星钟差
 * 卫星位置和时间引起误差的方差、卫星健康标志、广播星历
 * 解算参数、解、方位高度角、观测值的可用性（用sat编号）
 * 伪距残差 residuals of pseudorange (m)、错误信息
*/
static int estpos(const obsd_t *obs, int n, const double *rs, const double *dts,
                  const double *vare, const int *svh, const nav_t *nav,
                  const prcopt_t *opt, sol_t *sol, double *azel, int *vsat,
                  double *resp, char *msg)
{


    double x[NX]={0},dx[NX],Q[NX*NX],*v,*H,*var,sig;
    /* x[NX]={0},系统状态量 ECEFx、y、z、钟差（换算成距离）、R、E、C、I导航系统的时间偏移量
     * dx[NX],状态量的估计误差，最小二乘法求得的改正值
     * Q[NX*NX],
     * *v,残差矩阵，伪距减去几何距离、钟差、电离层对流层改正后的残余值，并按照伪距测量误差的std来定权
     * *H,系统观测矩阵   ECEF坐标系下接收机指向卫星的单位向量Vx、Vy、Vz，并按照伪距测量误差的std来定权
     * *var,伪距测量误差的方差，对流层、电离层、高度角、测量误差等引起的误差的方差
     * sig;伪距测量误差的标准差 sqrt（var）
     *
     * 对于包含噪声的状态观测方程 Y=Ax+n
     * 用LS求解 x=(A`A)'A`Y   在这里用v=H`x表示，其中H`是经过线性化的A,则LS表示 dx=(HH`)'Hv    (`表示转置,'表示逆）
    */
    int i,j,k,info,stat,nv,ns;
    /* i,j,k,
     * info,
     * stat,表示解算的结果的状态 1表示正常 0表示可靠性检验失败（卡方检验或GDOP超过预设值）
     * nv,构建的伪距观测值残差的数量，因为把多系统的钟差分别作为参数来估计，为了保证构建的矩阵满秩，需要再多构建一些方程来约束，所以和实际使用的卫星数ns有区别
     *     nv=伪距观测值残差的数量ns（一颗可用卫星一个）+为了保证满秩所附加的约束的数量（GRECI五个系统少使用一个卫星系统就要加一个），
     * ns;经过筛选剔除后的实际可用卫星数      区别与nv ns 所有观测值数量n 及他们的关系
*/
    
    trace(3,"estpos  : n=%d\n",n);
    
    v=mat(n+4,1); H=mat(NX,n+4); var=mat(n+4,1);
    
    for (i=0;i<3;i++) x[i]=sol->rr[i];//给x赋予前一次解算的ECEF坐标x、y、z
    
    for (i=0;i<MAXITR;i++) {//开始迭代  程序设置迭代最大次数MAXITR 宏定义为10
        
        /* pseudorange residuals (m) */
        nv=rescode(i,obs,n,rs,dts,vare,svh,nav,x,opt,v,H,var,azel,vsat,resp,
                   &ns);//x、v、H、var
        
        if (nv<NX) {
            sprintf(msg,"lack of valid sats ns=%d",nv);
            break;
        }
        /* weighted by Std */   //std定权
        for (j=0;j<nv;j++) {
            sig=sqrt(var[j]);
            v[j]/=sig;
            for (k=0;k<NX;k++) H[k+j*NX]/=sig;
        }
        /* least square estimation */  //最小二乘估计
        if ((info=lsq(H,v,NX,nv,dx,Q))) {
            sprintf(msg,"lsq error info=%d",info);
            break;
        }
        for (j=0;j<NX;j++) {
            x[j]+=dx[j];//用估计误差修正系统状态量的误差  用这次解算得到的改正值来改正上一次迭代的值
        }
        if (norm(dx,NX)<1E-4) {  //计算估计误差的的范数（可以理解为NX维向量的绝对距离），判断当两次迭代的结果差异小于1E-4,可认为结果收敛，结束迭代
            sol->type=0;//ECEF坐标系
            sol->time=timeadd(obs[0].time,-x[3]/CLIGHT);
            sol->dtr[0]=x[3]/CLIGHT; /* receiver clock bias (s) */
            sol->dtr[1]=x[4]/CLIGHT; /* GLO-GPS time offset (s) */
            sol->dtr[2]=x[5]/CLIGHT; /* GAL-GPS time offset (s) */
            sol->dtr[3]=x[6]/CLIGHT; /* BDS-GPS time offset (s) */
            sol->dtr[4]=x[7]/CLIGHT; /* IRN-GPS time offset (s) */
            for (j=0;j<6;j++) sol->rr[j]=j<3?x[j]:0.0;//ECEF下的XYZ，速度在
            for (j=0;j<3;j++) sol->qr[j]=(float)Q[j+j*NX];//ECEF下XYZ的方差
            sol->qr[3]=(float)Q[1];    /* cov xy *///协方差
            sol->qr[4]=(float)Q[2+NX]; /* cov yz */
            sol->qr[5]=(float)Q[2];    /* cov zx */
            sol->ns=(uint8_t)ns;//解算实际使用的卫星数
            sol->age=sol->ratio=0.0;//差分年龄和AR-ratio 在差分模式和模糊度固定才有
            
            /* validate solution */ //卡方检验大于卡方分布表 和 GDOP值检验大于opt设置的预设值，则返回0 表示解不可靠（sol->stat的在pntpos函数开始时设置为了SOLQ_NONE）
            if ((stat=valsol(azel,vsat,n,opt,v,nv,NX,msg))) {
                sol->stat=opt->sateph==EPHOPT_SBAS?SOLQ_SBAS:SOLQ_SINGLE;
            }
            free(v); free(H); free(var);
            return stat;
        }
    }
    if (i>=MAXITR) sprintf(msg,"iteration divergent i=%d",i);//迭代超过MAXITR次没有收敛，表示结果发散
    
    free(v); free(H); free(var);
    return 0;
}
//接收机自主完好性监测（RAIM: Receiver Autonomous Integrity Monitoring）
/* RAIM FDE (failure detection and exclution) -------------------------------*/
static int raim_fde(const obsd_t *obs, int n, const double *rs,
                    const double *dts, const double *vare, const int *svh,
                    const nav_t *nav, const prcopt_t *opt, sol_t *sol,
                    double *azel, int *vsat, double *resp, char *msg)
{
    obsd_t *obs_e;
    sol_t sol_e={{0}};
    char tstr[32],name[16],msg_e[128];
    double *rs_e,*dts_e,*vare_e,*azel_e,*resp_e,rms_e,rms=100.0;
    int i,j,k,nvsat,stat=0,*svh_e,*vsat_e,sat=0;
    
    trace(3,"raim_fde: %s n=%2d\n",time_str(obs[0].time,0),n);
    
    if (!(obs_e=(obsd_t *)malloc(sizeof(obsd_t)*n))) return 0;
    rs_e = mat(6,n); dts_e = mat(2,n); vare_e=mat(1,n); azel_e=zeros(2,n);
    svh_e=imat(1,n); vsat_e=imat(1,n); resp_e=mat(1,n); 
    
    for (i=0;i<n;i++) {
        
        /* satellite exclution */
        for (j=k=0;j<n;j++) {
            if (j==i) continue;
            obs_e[k]=obs[j];
            matcpy(rs_e +6*k,rs +6*j,6,1);
            matcpy(dts_e+2*k,dts+2*j,2,1);
            vare_e[k]=vare[j];
            svh_e[k++]=svh[j];
        }
        /* estimate receiver position without a satellite */
        if (!estpos(obs_e,n-1,rs_e,dts_e,vare_e,svh_e,nav,opt,&sol_e,azel_e,
                    vsat_e,resp_e,msg_e)) {
            trace(3,"raim_fde: exsat=%2d (%s)\n",obs[i].sat,msg);
            continue;
        }
        for (j=nvsat=0,rms_e=0.0;j<n-1;j++) {
            if (!vsat_e[j]) continue;
            rms_e+=SQR(resp_e[j]);
            nvsat++;
        }
        if (nvsat<5) {
            trace(3,"raim_fde: exsat=%2d lack of satellites nvsat=%2d\n",
                  obs[i].sat,nvsat);
            continue;
        }
        rms_e=sqrt(rms_e/nvsat);
        
        trace(3,"raim_fde: exsat=%2d rms=%8.3f\n",obs[i].sat,rms_e);
        
        if (rms_e>rms) continue;
        
        /* save result */
        for (j=k=0;j<n;j++) {
            if (j==i) continue;
            matcpy(azel+2*j,azel_e+2*k,2,1);
            vsat[j]=vsat_e[k];
            resp[j]=resp_e[k++];
        }
        stat=1;
        *sol=sol_e;
        sat=obs[i].sat;
        rms=rms_e;
        vsat[i]=0;
        strcpy(msg,msg_e);
    }
    if (stat) {
        time2str(obs[0].time,tstr,2); satno2id(sat,name);
        trace(2,"%s: %s excluded by raim\n",tstr+11,name);
    }
    free(obs_e);
    free(rs_e ); free(dts_e ); free(vare_e); free(azel_e);
    free(svh_e); free(vsat_e); free(resp_e);
    return stat;
}
/* range rate residuals ------------------------------------------------------*/
static int resdop(const obsd_t *obs, int n, const double *rs, const double *dts,
                  const nav_t *nav, const double *rr, const double *x,
                  const double *azel, const int *vsat, double err, double *v,
                  double *H)
{
    double freq,rate,pos[3],E[9],a[3],e[3],vs[3],cosel,sig;
    int i,j,nv=0;
    
    trace(3,"resdop  : n=%d\n",n);
    
    ecef2pos(rr,pos); xyz2enu(pos,E);
    
    for (i=0;i<n&&i<MAXOBS;i++) {
        
        freq=sat2freq(obs[i].sat,obs[i].code[0],nav);
        
        if (obs[i].D[0]==0.0||freq==0.0||!vsat[i]||norm(rs+3+i*6,3)<=0.0) {
            continue;
        }
        /* LOS (line-of-sight) vector in ECEF */
        cosel=cos(azel[1+i*2]);
        a[0]=sin(azel[i*2])*cosel;
        a[1]=cos(azel[i*2])*cosel;
        a[2]=sin(azel[1+i*2]);
        matmul("TN",3,1,3,1.0,E,a,0.0,e);
        
        /* satellite velocity relative to receiver in ECEF */
        for (j=0;j<3;j++) {
            vs[j]=rs[j+3+i*6]-x[j];
        }
        /* range rate with earth rotation correction */
        rate=dot(vs,e,3)+OMGE/CLIGHT*(rs[4+i*6]*rr[0]+rs[1+i*6]*x[0]-
                                      rs[3+i*6]*rr[1]-rs[  i*6]*x[1]);
        
        /* Std of range rate error (m/s) */
        sig=(err<=0.0)?1.0:err*CLIGHT/freq;
        
        /* range rate residual (m/s) */
        v[nv]=(-obs[i].D[0]*CLIGHT/freq-(rate+x[3]-CLIGHT*dts[1+i*2]))/sig;
        
        /* design matrix */
        for (j=0;j<4;j++) {
            H[j+nv*4]=((j<3)?-e[j]:1.0)/sig;
        }
        nv++;
    }
    return nv;
}
/* estimate receiver velocity ------------------------------------------------*/
static void  estvel(const obsd_t *obs, int n, const double *rs, const double *dts,
                   const nav_t *nav, const prcopt_t *opt, sol_t *sol,
                   const double *azel, const int *vsat)
{
    double x[4]={0},dx[4],Q[16],*v,*H;
    double err=opt->err[4]; /* Doppler error (Hz) *///opt里的预设值 默认1hz
    int i,j,nv;
    
    trace(3,"estvel  : n=%d\n",n);
    
    v=mat(n,1); H=mat(4,n);
    
    for (i=0;i<MAXITR;i++) {
        
        /* range rate residuals (m/s) */
        if ((nv=resdop(obs,n,rs,dts,nav,sol->rr,x,azel,vsat,err,v,H))<4) {
            break;
        }
        /* least square estimation */
        if (lsq(H,v,4,nv,dx,Q)) break;
        
        for (j=0;j<4;j++) x[j]+=dx[j];
        
        if (norm(dx,4)<1E-6) {
            matcpy(sol->rr+3,x,3,1);
            sol->qv[0]=(float)Q[0];  /* xx */
            sol->qv[1]=(float)Q[5];  /* yy */
            sol->qv[2]=(float)Q[10]; /* zz */
            sol->qv[3]=(float)Q[1];  /* xy */
            sol->qv[4]=(float)Q[6];  /* yz */
            sol->qv[5]=(float)Q[2];  /* zx */
            break;
        }
    }
    free(v); free(H);
}
/* single-point positioning ----------------------------------------------------
* compute receiver position, velocity, clock bias by single-point positioning
* with pseudorange and doppler observables
* args   : obsd_t *obs      I   observation data
*          int    n         I   number of observation data
*          nav_t  *nav      I   navigation data
*          prcopt_t *opt    I   processing options
*          sol_t  *sol      IO  solution
*          double *azel     IO  azimuth/elevation angle (rad) (NULL: no output)
*          ssat_t *ssat     IO  satellite status              (NULL: no output)
*          char   *msg      O   error message for error exit
* return : status(1:ok,0:error)
*-----------------------------------------------------------------------------*/
extern int pntpos(const obsd_t *obs, int n, const nav_t *nav,
                  const prcopt_t *opt, sol_t *sol, double *azel, ssat_t *ssat,
                  char *msg)
{
    prcopt_t opt_=*opt;
    double *rs,*dts,*var,*azel_,*resp;
    //卫星ECEF坐标系下的位置和速度rs 6行n列  {x y z vx vy vz} (m|m/s)
    //卫星钟差dts                 2行n列  {bias,drift}     (s|s/s)
    //卫星位置和时间引起误差的方差var        1行n列               (m^2)   如果有精密星历那就用精密星历的值，如果没有就用默认值SQR(STD_BRDCCLK)
    //azel  2行n列0矩阵     方位角、高度角（rad） azimuth/elevation angles {az,el} (rad)
    //resp  1行n列0矩阵      伪距残差 residuals of pseudorange (m) */


    int i,stat,vsat[MAXOBS]={0},svh[MAXOBS];
    //
    //stat 表示解算的结果的状态 1表示正常 0表示可靠性检验失败（卡方检验或GDOP超过预设值）
    //vsat 观测值的可用性 用sat编号 1可用 0 不可用 （是否可用除了取决于是否有有效obs数据，还取决于解算模式时单频还是双频）
    //卫星健康标志 svh    -1 表示不可用
    
    trace(3,"pntpos  : tobs=%s n=%d\n",time_str(obs[0].time,3),n);
    
    sol->stat=SOLQ_NONE;
    
    if (n<=0) {
        strcpy(msg,"no observation data");
        return 0;
    }
    sol->time=obs[0].time;    //解的时间和obs一致
    msg[0]='\0';
    
    rs=mat(6,n); dts=mat(2,n); var=mat(1,n); azel_=zeros(2,n); resp=mat(1,n);//分配空间
    
    if (opt_.mode!=PMODE_SINGLE) { /* for precise positioning *///非单点定位模式调用单点定位函数的时候，在单点定位解算的时候暂时将ion和trop模型改为brdc和saas（如RTK模式基准站坐标选择average single会用到单点定位）
        opt_.ionoopt=IONOOPT_BRDC;
        opt_.tropopt=TROPOPT_SAAS;
    }
    /* satellite positons, velocities and clocks */
    satposs(sol->time,obs,n,nav,opt_.sateph,rs,dts,var,svh);
    //输入obs的时间，观测值、观测值个数、星历、使用的星历模式(EPHOPT_???)，得到卫星位置速度、钟差、位置和时间误差的方差、卫星健康标志
    
    /* estimate receiver position with pseudorange */
    stat=estpos(obs,n,rs,dts,var,svh,nav,&opt_,sol,azel_,vsat,resp,msg);
    //obs观测值、卫星数、位置速度、钟差、卫星位置时间误差、卫星健康标志、广播星历、解算参数、解、方位高度角、观测值的可用性（用sat编号）、伪距残差、错误信息
    
    /* RAIM FDE */
    if (!stat&&n>=6&&opt->posopt[4]) {//如果stat=0即LS解的结果验证不通过、观测值数量大于6、opt开启了RAIM FDE检验(posopt5=1），则进行接收机自主完好性监测和错误剔除
        stat=raim_fde(obs,n,rs,dts,var,svh,nav,&opt_,sol,azel_,vsat,resp,msg);//Receiver Autonomous Integrity Monitoring failure detection and exclution
    }
    /* estimate receiver velocity with Doppler */
    if (stat) {
        estvel(obs,n,rs,dts,nav,&opt_,sol,azel_,vsat);
    }
    if (azel) {
        for (i=0;i<n*2;i++) azel[i]=azel_[i];
    }
    if (ssat) {
        for (i=0;i<MAXSAT;i++) { //ssat所有值初始化为0
            ssat[i].vs=0;
            ssat[i].azel[0]=ssat[i].azel[1]=0.0;
            ssat[i].resp[0]=ssat[i].resc[0]=0.0;
            ssat[i].snr[0]=0;
        }
        for (i=0;i<n;i++) {  //将azel和SNR按照sat（卫星编号区别于PRN）填入 ssat[sat-1]
            ssat[obs[i].sat-1].azel[0]=azel_[  i*2];
            ssat[obs[i].sat-1].azel[1]=azel_[1+i*2];
            ssat[obs[i].sat-1].snr[0]=obs[i].SNR[0];
            if (!vsat[i]) continue;//如果卫星可用，填入可用标志vs和伪距残差resp
            ssat[obs[i].sat-1].vs=1;
            ssat[obs[i].sat-1].resp[0]=resp[i];
        }
    }
    free(rs); free(dts); free(var); free(azel_); free(resp);
    return stat;
}
