#include <iostream>
#include "util/include/log.h"

int main() {
    std::string tmpDir="/tmp/clog";
    InitLog(tmpDir);

    std::cout << "Hello, World!" << std::endl;
    return 0;
}