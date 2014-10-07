////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

WN_BOOL WNInConnectionLinux::Initialize(WN_INT32 _yourSocket, sockaddr& _addr, socklen_t addrLen) {
    
    mSockFD = _yourSocket;

    WN_CHAR name[NI_MAXHOST];
    WN_CHAR port[NI_MAXSERV];

    WN_BOOL validName = (getnameinfo(&_addr, addrLen, name, NI_MAXHOST, port, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV) == 0);
    WN_INT32 flags;

    flags = fcntl(mSockFD, F_GETFL, 0);
    if(flags == -1) {
        //NOTIFY ERROR
        return(WN_FALSE);
    }
#ifdef _WN_ANDROID 
    flags |= O_NONBLOCK; //Android does not define SOCK_NONBLOCK
#else
    flags |= SOCK_NONBLOCK;
#endif
    if(fcntl (mSockFD, F_SETFL, flags) == -1) {
        //NOTIFY ERROR
        return(WN_FALSE);
    } 
    sockaddr_in* sin = reinterpret_cast<sockaddr_in*>(&_addr);
    WN_SIZE_T length = WN_SNPRINTF(NULL,0, "%s:%d", inet_ntoa(sin->sin_addr), ntohs(sin->sin_port));
    
    mConnectionName = WNMemory::WNMallocT<WN_CHAR>(length + 1);
    WN_SNPRINTF(mConnectionName,length+1, "%s:%d", inet_ntoa(sin->sin_addr), ntohs(sin->sin_port));

    return(WN_TRUE);
}
