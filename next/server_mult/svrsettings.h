#ifndef SVRSETTINGS_H
#define SVRSETTINGS_H

#include"rtklib.h"
#include"server_mult.h"

/* global setting ----------------------------------------------------------*/

//basic conf path:
#define DEFAULT_SETTING  "D:/RTKSERVER/conf/server_mult/default.ini"




extern const prcopt_t svr_prcopt_defult={ /* defaults processing options */
    PMODE_KINEMA,   /* positioning mode (PMODE_???) */
    0,              /* solution type (0:forward,1:backward,2:combined) */
    2,              /* number of frequencies (1:L1,2:L1+L2,3:L1+L2+L5) */
    SYS_GPS|SYS_CMP,/* navigation system (SYS_???|SYS_???)*/

    15.0*D2R,       /* elevation mask angle (rad) */
    {{0,0}},        /* SNR mask */

    EPHOPT_BRDC,              /* satellite ephemeris/clock (EPHOPT_???) */
    3,              /* AR mode (0:off,1:continuous,2:instantaneous,3:fix and hold,4:ppp-ar) */
    0,              /* GLONASS AR mode (0:off,1:on,2:auto cal,3:ext cal) */
    1,              /* BeiDou AR mode (0:off,1:on) */

    5,              /* obs outage count to reset bias */
    0,              /* min lock count to fix ambiguity */
    10,             /* min fix count to hold ambiguity */
    1,              /* max iteration to resolve ambiguity */

    IONOOPT_OFF,    /* ionosphere option (IONOOPT_???) */
    TROPOPT_OFF,    /* troposphere option (TROPOPT_???) */
    0,              /* dynamics model (0:none,1:velociy,2:accel) */
    0,              /* earth tide correction (0:off,1:solid,2:solid+otl+pole) */

    1,              /* number of filter iteration */
    0,              /* code smoothing window size (0:none) */
    0,              /* interpolate reference obs (for post mission) */
    0,              /* SBAS correction options */
    0,              /* SBAS satellite selection (0:all) */

    0,              /* rover position for fixed mode */
    0,              /* base position for relative mode */
                    /* (0:pos in prcopt,  1:average of single pos, */
                    /*  2:read from file, 3:rinex header, 4:rtcm pos) */

    {100.0,100.0},  /* code/phase error ratio */

    {100.0,0.003,0.003,0.0,1.0},
                    /* measurement error factor */
                    /* [0]:reserved */
                    /* [1-3]:error factor a/b/c of phase (m) */
                    /* [4]:doppler frequency (hz) */

    {30.0,0.03,0.3},/* initial-state std [0]bias,[1]iono [2]trop */

    {1E-4,1E-3,1E-4,1E-1,1E-2,0.0},
                    /* process-noise std [0]bias,[1]iono [2]trop [3]acch [4]accv [5] pos */

    5E-12,          /* satellite clock stability (sec/sec) */

    {3.0,0.9999,0.25,0.1,0.05},/* AR validation threshold */

    0.0,            /* elevation mask of AR for rising satellite (deg) */
    0.0,            /* elevation mask to hold ambiguity (deg) */
    0.05,           /* slip threshold of geometry-free phase (m) */

    30.0,           /* max difference of time (sec) */
    30.0,           /* reject threshold of innovation (m) */
    30.0,           /* reject threshold of gdop */

    {0},            /* baseline length constraint {const,sigma} (m) */
    {0},            /* base position for relative mode {x,y,z} (ecef) (m) */
    {0},            /* antenna types {rover,base} */

    {"",""},        /* antenna types {rover,base} */

    {{0}},          /* antenna delta {{rov_e,rov_n,rov_u},{ref_e,ref_n,ref_u}} */
    {{0}},          /* receiver antenna parameters {rov,base} */
    {0}             /* excluded satellites (1:excluded,2:included) */
};




extern const solopt_t svr_solopt_default={ /* defaults solution output options */
    SOLF_LLH,TIMES_GPST,1,3,    /* posf,times,timef,timeu */
    0,1,0,0,0,0,0,              /* degf,outhead,outopt,outvel,datum,height,geoid */
    0,0,0,                      /* solstatic,sstat,trace */
    {0.0,0.0},                  /* nmeaintv */
    " ",""                      /* separator/program name */
};





extern const int strtype[8]={                  /* stream types */
    STR_NONE,STR_NONE,STR_NONE,STR_NONE,STR_NONE,STR_NONE,STR_NONE,STR_NONE
};
char strpath[8][MAXSTR]={""};    /* stream paths */






extern const int svrcycle     =10;            /* server cycle (ms) */

extern const int nmeacycle    =5000;          /* nmea request cycle (ms) */
extern const int nmeareq      =0;             /* nmea request type (0:off,1:lat/lon,2:single) */
extern const double nmeapos[3] ={0,0};         /* nmea position (lat/lon) (deg) */

extern const int buffsize     =32768;         /* input buffer size (bytes) */

extern const int strfmt[8]={                   /* stream formats */
    STRFMT_RTCM3,STRFMT_RTCM3,STRFMT_RTCM3,SOLF_LLH,SOLF_NMEA,0,0,0
};

extern const int navsel=0;                    /* ephemeris select (0:all,1:rover,2:base,3:corr) */





extern const int timeout      =10000;         /* timeout time (ms) */
extern const int reconnect    =10000;         /* reconnect interval (ms) */



extern const int fswapmargin  =30;            /* file swap marign (s) */

extern const int navmsgsel    =0;             /* navigation mesaage select */


extern char proxyaddr[MAXSTR]="";              /* proxy address */

extern char *paths[8]={strpath[0],strpath[1],strpath[2],strpath[3],
                strpath[4],strpath[5],strpath[6],strpath[7]};

extern char*cmds[3]={0,0,0};
extern char*cmds_periodic[3]= {0,0,0};
extern char*rcvopts[3]={"","",""};

extern char errmsg[20148];




//moni setting

extern int moniport=0;            //0:关闭
extern int monitype=STR_TCPSVR;
extern int monimode=STR_MODE_RW;
extern char *monipath={""};



extern const svrio_t svrio_default{
{STR_NONE,STR_NONE,STR_NONE,STR_NONE,STR_NONE,STR_NONE,STR_NONE,STR_NONE},
{STRFMT_RTCM3,STRFMT_RTCM3,STRFMT_RTCM3,SOLF_LLH,SOLF_NMEA,0,0,0},
{""},
1,
{0.0,0.0,0.0}
};


#endif // SVRSETTINGS_H









