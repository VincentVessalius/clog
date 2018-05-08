//
// Created by cyz on 5/5/18.
//

#ifndef CLOG_TOOLS_H
#define CLOG_TOOLS_H

#include <string>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

int mkdirs(std::string dir, mode_t mode);

#endif //CLOG_TOOLS_H
