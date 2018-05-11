//
// Created by cyz on 5/6/18.
//

#include "util/include/log.h"

int FILEMODE = 0660;
int DIRMODE = 0755;

std::string logDir;
FILE *logErrF, *logWarnF, *logInfoF;

time_t curtime;

int InitLog(std::string tmpDir) {
    logDir = tmpDir + "/log";
    if (mkdirs(logDir, DIRMODE))
        return -1;
    if (!(logErrF = fopen((logDir + "/ERROR").c_str(), "a+")))
        return -1;
    if (!(logWarnF = fopen((logDir + "/WARN").c_str(), "a+")))
        return -1;
    if (!(logInfoF = fopen((logDir + "/INFO").c_str(), "a+")))
        return -1;
    setvbuf(logErrF, NULL, _IOLBF, 512);
    setvbuf(logWarnF, NULL, _IOLBF, 512);
    setvbuf(logInfoF, NULL, _IOLBF, 512);
    return 0;
}