///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_FILE_SYSTEM_FILE_BUFFER_H__
#define __WN_FILE_SYSTEM_FILE_BUFFER_H__

#include "WNContainers/inc/WNDataBuffer.h"

namespace WNFileSystem {
    enum WNFileBufferType {
        eWNBinary,
        eWNText
    };

    class WNFileBuffer : public wn::containers::data_buffer {
    public:
        WNFileBuffer(WNFileBufferType _type);

    protected:
        WNFileBufferType mType;
    };
 }

#endif // __WN_FILE_SYSTEM_FILE_BUFFER_H__