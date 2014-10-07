////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "WNNetworking/inc/Internal/Windows/WNInConnectionWindows.h"
#include "WNMemory/inc/WNManipulation.h"
#include "WNStrings/inc/WNStrings.h"

using namespace WNNetworking;

WNInConnectionWindows::WNInConnectionWindows(WNNetworkManager& _manager) :
    WNConnectionWindows(_manager) {
}

WNNetworkManagerReturnCode::Type WNInConnectionWindows::Initialize(SOCKET _listenSocket, WNConnectedCallback _callback) {
    sockaddr_in address;
    int sockAddrSize = sizeof(address);

    WNMemory::WNMemClrT(&address);

    mSocket = accept(_listenSocket, (sockaddr*)&address, &sockAddrSize);

    if (INVALID_SOCKET == mSocket) {
        return(WNNetworkManagerReturnCode::eWNCannotCreateSocket);
    }

    WN_SIZE_T length = WN_SNPRINTF(NULL, 0, "%s:%d", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

    mConnectionName = WNMemory::WNMallocT<WN_CHAR>(length + 1);

    WN_SNPRINTF(mConnectionName, length + 1, "%s:%d", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

    mCallback = _callback;

    return(WNNetworkManagerReturnCode::eWNOK);
}

WN_BOOL WNInConnectionWindows::FireCallback() {
    return(mCallback.Execute(mConnectionName));
}
