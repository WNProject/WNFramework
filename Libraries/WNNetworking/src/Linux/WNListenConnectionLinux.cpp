// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNNetworking/inc/Internal/Linux/WNListenConnectionLinux.h"
#include "WNNetworking/inc/Internal/Linux/WNInConnectionLinux.h"
#include "WNMemory/inc/WNManipulation.h"

#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#ifdef _WN_ANDROID
    #include <netinet/in.h>
#endif

using namespace WNNetworking;

WNListenConnectionLinux::WNListenConnectionLinux(WNNetworkManager& _manager, WNConnectionType::Type _type, WN_UINT16 _port, WNConnectedCallback _connected) :
    WNConnectionLinux(_manager),
    mPort(_port),
    mType(_type),
    mConnectedCallback(_connected) {
}

WNNetworkManagerReturnCode::Type WNListenConnectionLinux::Initialize() {
    WN_RELEASE_ASSERT_DESC(mType == WNConnectionType::eWNReliable, "WNNetworking does not support unreliable connections .... YET");

    struct addrinfo hints;
    struct addrinfo *result, *rp;
    WN_INT32 sock;

    WNMemory::WNMemClr(&hints, sizeof(struct addrinfo));

    struct sockaddr_in serv_addr;

    WNMemory::WNMemClr(&serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port=htons(mPort);

    mSockFD = socket(AF_INET, SOCK_STREAM, 0);

    if (mSockFD == -1) {
        return(WNNetworkManagerReturnCode::eWNCannotAssociate);
    }

    struct linger flush;

    flush.l_onoff= 1;
    flush.l_linger=0;

    const int err = setsockopt(mSockFD, SOL_SOCKET, SO_LINGER, &flush, sizeof(flush));

    if (err != 0) {
        return(WNNetworkManagerReturnCode::eWNCannotAssociate);
    }

    sock = ::bind(mSockFD, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    if (sock != 0) {
        return(WNNetworkManagerReturnCode::eWNCannotAssociate);
    }

    WN_INT32 flags;

    flags = fcntl(mSockFD, F_GETFL, 0);

    if (flags == -1) {
        return(WNNetworkManagerReturnCode::eWNCannotListen);
    }

#ifdef _WN_ANDROID 
    flags |= O_NONBLOCK; //Android does not define SOCK_NONBLOCK
#else
    flags |= SOCK_NONBLOCK;
#endif

    if (fcntl(mSockFD, F_SETFL, flags) == -1) {
        return(WNNetworkManagerReturnCode::eWNCannotListen);
    }

    if (listen(mSockFD, SOMAXCONN) == -1) {
        return(WNNetworkManagerReturnCode::eWNCannotListen);
    }

    const int nameLen = WN_SNPRINTF(NULL, 0, "Listen:%d", mPort);
    WN_CHAR* name = WNMemory::WNCallocT<WN_CHAR>(nameLen + 1);

    WN_SNPRINTF(name, nameLen + 1, "Listen:%d", mPort);

    mConnectionName = name;

    return(WNNetworkManagerReturnCode::eWNOK);
}
WNInConnectionLinux* WNListenConnectionLinux::ReceiveConnection() {
    struct sockaddr addr;
    socklen_t len = sizeof(sockaddr);
    WN_INT32 sockFD;

    sockFD = accept(mSockFD, &addr, &len);

    if (sockFD == -1){
        if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
            return(WN_NULL);
        } else {
            char* s = strerror(errno);

            perror(NULL);
            //REPORT THE ERROR OR SOMETHING.. COULD NOT ACCEPT CONNECTION
            return(WN_NULL);
        }
    }

    WNInConnectionLinux * conn = WN_NEW WNInConnectionLinux(mManager);

    conn->Initialize(sockFD, addr, len);

    if (!mConnectedCallback.Execute(conn->GetName())) {
        WN_DELETE(conn);
    }

    return(conn);
}
