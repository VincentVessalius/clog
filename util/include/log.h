//
// Created by cyz on 5/5/18.
//

#ifndef CLOG_LOG_H
#define CLOG_LOG_H

#include <string>
#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "Vin_Tools.h"
//TODO

extern int FILEMODE;
extern int DIRMODE;

extern std::string logDir;
extern FILE *logErrF, *logWarnF, *logInfoF;


extern int InitLog(std::string tmpDir);

extern time_t curtime;

#define LOG_ERROR(fmt, args...) {do{flockfile(logErrF);time(&curtime);fprintf(logErrF, "[%s:PID:%d|" fmt"|]\n",\
                                asctime(localtime(&curtime)),getpid(),##args);funlockfile(logErrF);}while(0);}

#define LOG_WARN(fmt, args...) {do{flockfile(logWarnF);time(&curtime);fprintf(logWarnF, "[%s:PID:%d|" fmt"|]\n",\
                                asctime(localtime(&curtime)),getpid(),##args);funlockfile(logWarnF);}while(0);}

#define LOG_INFO(fmt, args...) {do{flockfile(logInfoF);time(&curtime);fprintf(logInfoF, "[%s:PID:%d|" fmt"|]\n",\
                                asctime(localtime(&curtime)),getpid(),##args);funlockfile(logInfoF);}while(0);}

#endif //CLOG_LOG_H
