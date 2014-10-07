////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "WNNetworking/inc/Internal/Windows/WNListenConnectionWindows.h"
#include "WNNetworking/inc/Internal/Windows/WNInConnectionWindows.h"
#include "WNNetworking/inc/WNNetworkManager.h"
#include "WNMemory/inc/WNManipulation.h"

using namespace WNNetworking;

WNListenConnectionWindows::WNListenConnectionWindows(WNNetworkManager& _manager, WNConnectionType::Type _type, WN_UINT16 _port, WNConnectedCallback _connected) :
    WNConnectionWindows(_manager),
    mPort(_port),
    mConnectedCallback(_connected),
    mType(_type),
    mInitialized(WN_FALSE) {
    WN_RELEASE_ASSERT_DESC(_type == WNConnectionType::eWNReliable, "We do not support unreliable connections yet");
}

WNNetworkManagerReturnCode::Type WNListenConnectionWindows::Initialize() {
    mSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

    if (INVALID_SOCKET == mSocket) {
        return(WNNetworkManagerReturnCode::eWNCannotCreateSocket);
    }

    sockaddr_in ServerAddress;

    WNMemory::WNMemClrT(&ServerAddress);

    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_addr.s_addr = INADDR_ANY;
    ServerAddress.sin_port = htons(mPort);

    if (SOCKET_ERROR == bind(mSocket, (struct sockaddr *) &ServerAddress, sizeof(ServerAddress))) {
        closesocket(mSocket);

        return(WNNetworkManagerReturnCode::eWNCannotCreateSocket);
    }

    if (SOCKET_ERROR == listen(mSocket, SOMAXCONN)) {
        closesocket(mSocket);

        return(WNNetworkManagerReturnCode::eWNCannotCreateSocket);
    }

    const WN_UINT32 nameLen = WN_SNPRINTF(NULL, 0, "Listen:%d", mPort);
    WN_CHAR* name = WNMemory::WNCallocT<WN_CHAR>(nameLen + 1);

    WN_SNPRINTF(name, nameLen + 1, "Listen:%d", mPort);

    mConnectionName = name;
    mInitialized = WN_TRUE;

    return(WNNetworkManagerReturnCode::eWNOK);
}

WNInConnectionWindows* WNListenConnectionWindows::AcceptConnection() {
    WNInConnectionWindows* connection = WN_NEW WNInConnectionWindows(mManager);

    if (connection->Initialize(mSocket, mConnectedCallback) != WNNetworkManagerReturnCode::eWNOK) {
        WN_DELETE(connection);
    }

    return(connection);
}
