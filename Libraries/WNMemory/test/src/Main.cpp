#include "WNMemory/test/inc/Common.h"

#include <vector>

wn_int32 wn_main(wn_int32 _argc, wn_char* _argv[]) {
    testing::InitGoogleTest(&_argc, _argv);

    const wn_uint32 result = RUN_ALL_TESTS();

    return(result);
}
