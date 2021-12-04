#include "korolib.h"

Korolib::Korolib()
{
}




int char2arg(char* str, int* argc, char** argv, int number)
{
    char *p;
    int num=0;
    int word_start = 1;

    if(argc == NULL || argv == NULL)
        return -1;

    p=str;

    while(*p){
        if((*p == '\r') || (*p == '\n')){
            *p = '\0';
            break;
        }
        if((*p == ' ') || (*p == '\t')){
            word_start = 1;
            *p = '\0';
            p++;
            continue;
        }
        if(num >= number)
            break;

        if(word_start){
            argv[num++] = p;
            word_start = 0;
        }
        p++;
    }

    *argc = num;

    return 0;
}


