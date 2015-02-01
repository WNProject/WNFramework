// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNNetworking/inc/Internal/Linux/WNInConnectionLinux.h"
#include "WNMemory/inc/WNManipulation.h"

#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>

using namespace WNNetworking;

WNInConnectionLinux::WNInConnectionLinux(WNNetworkManager& _manager) :
    WNConnectionLinux(_manager) {
}

wn_bool WNInConnectionLinux::Initialize(wn_int32 _yourSocket, sockaddr& _addr, socklen_t addrLen) {

    mSockFD = _yourSocket;

    wn_char name[NI_MAXHOST];
    wn_char port[NI_MAXSERV];

    wn_bool validName = (getnameinfo(&_addr, addrLen, name, NI_MAXHOST, port, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV) == 0);
    wn_int32 flags;

    flags = fcntl(mSockFD, F_GETFL, 0);
    if(flags == -1) {
        //NOTIFY ERROR
        return(wn_false);
    }
#ifdef _WN_ANDROID
    flags |= O_NONBLOCK; //Android does not define SOCK_NONBLOCK
#else
    flags |= SOCK_NONBLOCK;
#endif
    if(fcntl (mSockFD, F_SETFL, flags) == -1) {
        //NOTIFY ERROR
        return(wn_false);
    }
    sockaddr_in* sin = reinterpret_cast<sockaddr_in*>(&_addr);
    wn_size_t length = WN_SNPRINTF(NULL,0, "%s:%d", inet_ntoa(sin->sin_addr), ntohs(sin->sin_port));

    mConnectionName = wn::malloc<wn_char>(length + 1);
    WN_SNPRINTF(mConnectionName,length+1, "%s:%d", inet_ntoa(sin->sin_addr), ntohs(sin->sin_port));

    return(wn_true);
}
