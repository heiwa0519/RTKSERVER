#ifndef KOROLIB_H
#define KOROLIB_H

#include "korolib_global.h"
#include "stdio.h"

#ifdef WIN_DLL
#define EXPORT __declspec(dllexport) /* for Windows DLL */
#else
#define EXPORT
#endif

#define MAX_CMD_ARGV  50




class KOROLIB_EXPORT Korolib
{
public:
    Korolib();
};


EXPORT int char2arg(char* str, int* argc, char** argv, int number);//字符串转argc和argv   number=MAX_CMD_ARGV


#endif // KOROLIB_H
