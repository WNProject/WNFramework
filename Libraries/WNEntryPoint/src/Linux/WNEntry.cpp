#include "WNCore/inc/WNTypes.h"
#include "WNUtils/inc/WNCrashHandler.h"

extern wn_int32 wn_main(wn_int32 _argc, wn_char* _argv[]);

int main(int _argc, char** _argv) {
    WNUtils::InitializeCrashHandler();

    return(wn_main(_argc, _argv));
}