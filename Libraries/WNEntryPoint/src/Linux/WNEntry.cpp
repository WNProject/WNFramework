#include "WNCore/inc/WNTypes.h"
#include "WNUtils/inc/WNCrashHandler.h"

extern WN_INT32 WNMain(WN_INT32 _argc, WN_CHAR* _argv[]);

int main(int _argc, char** _argv) {
    WNUtils::InitializeCrashHandler();

    return(WNMain(_argc, _argv)); 
}