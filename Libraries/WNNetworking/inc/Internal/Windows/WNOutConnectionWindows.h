////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_NETWORKING_WINDOWS_OUT_CONNECTION_H__
#define __WN_NETWORKING_WINDOWS_OUT_CONNECTION_H__

#include "WNNetworking/inc/Internal/Windows/WNConnectionWindows.h"
#include "WNNetworking/inc/WNNetworkManager.h"
#include "WNCore/inc/WNTypes.h"

namespace WNNetworking {
    class WNOutConnectionWindows : public WNConnectionWindows {
    public:
        WNOutConnectionWindows(WNNetworkManager& _manager);
        virtual WN_FORCE_INLINE ~WNOutConnectionWindows () {}

        WNNetworkManagerReturnCode::Type Initialize(WNConnectionType::Type& _type, const WN_CHAR* _target, WN_UINT16 _port);

    private:
        WNConnectedCallback mCallback;
    };
};

#endif // __WN_NETWORKING_WINDOWS_OUT_CONNECTION_H__
