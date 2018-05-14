//
// Created by cyz on 5/6/18.
//

#include "util/include/tools.h"

int mkdirs(std::string dir, mode_t mode) {
    if (dir.size() == 0 || mode < 0 || mode > 0777)
        return -1;
    std::string now;
    for (int i = 0; i < dir.size(); i++) {
        if (dir[i] == '/') {
            now = dir.substr(0, i + 1);
            if (NULL == opendir(now.c_str())) {
                mkdir(now.c_str(), mode);
            }
        }
    }
    if (NULL == opendir(dir.c_str()))
        mkdir(dir.c_str(), mode);
    return 0;
}