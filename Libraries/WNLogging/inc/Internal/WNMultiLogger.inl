////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __WN_MULTI_LOGGER_INL__
#define __WN_MULTI_LOGGER_INL__
#include "WNLogging/inc/WNMultiLogger.h"

template<typename T_Logger1, typename T_Logger2, int T_BufferSize>
WNLogging::WNMultiLogger<T_Logger1, T_Logger2, T_BufferSize>::~WNMultiLogger() {
}

template<typename T_Logger1, typename T_Logger2, int T_BufferSize>
WN_VOID WNLogging::WNMultiLogger<T_Logger1, T_Logger2, T_BufferSize>::FlushExternal(const WN_CHAR* _buffer, WN_SIZE_T _bufferSize, std::vector<WNLogColorElement>* _colors) {
    mLogger1.FlushExternal(_buffer, _bufferSize, _colors);
    mLogger2.FlushExternal(_buffer, _bufferSize, _colors);
}
#endif//__WN_MULTI_LOGGER_INL__
