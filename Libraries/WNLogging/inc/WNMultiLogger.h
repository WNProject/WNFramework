////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __WN_MULTI_LOGGER_H__ 
#define __WN_MULTI_LOGGER_H__ 
#include "WNLogging/inc/WNLog.h"
namespace WNLogging {
    
    template<typename T_Logger1, typename T_Logger2>
    class WNMultiLogger: public WNLogger {
    public:
        ~WNMultiLogger();
        WN_VOID FlushBuffer(const WN_CHAR* _buffer, WN_SIZE_T bufferSize, const std::vector<WNLogColorElement>* mColors);
    private:
        T_Logger1 mLogger1;
        T_Logger2 mLogger2;
    };
};

#include "WNLogging/inc/Internal/WNMultiLogger.inl"
#endif
