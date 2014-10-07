////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_NETWORKING_NETWORK_MANAGER_WINDOWS_H__
#define __WN_NETWORKING_NETWORK_MANAGER_WINDOWS_H__

#include "WNNetworking/inc/WNNetworkManager.h"
#include "WNConcurrency/inc/WNSpinLock.h"

#include <vector>

namespace WNConcurrency {
    template <typename Return>
    class WNThread;
}

namespace WNNetworking {
    class WNListenConnectionWindows;
    class WNInConnectionWindows;
    class WNConnectionWindows;
    class WNOutConnectionWindows;

    class WNNetworkManagerWindows : public WNNetworkManager {
    public:
        WNNetworkManagerWindows();
        virtual ~WNNetworkManagerWindows();

        virtual WNNetworkManagerReturnCode::Type Initialize(WN_UINT32 _numWorkerThreads);
        virtual WNNetworkManagerReturnCode::Type ConnectTo(WNConnection*& _outHandle, WNConnectionType::Type _type, const WN_CHAR* _target, WN_UINT16 _port);
        virtual WNNetworkManagerReturnCode::Type CreateListener(WNConnection*& _outHandle, WNConnectionType::Type _type, WN_UINT16 _port, const WNConnectedCallback& _callback);
        virtual WN_VOID Cleanup();
        virtual WN_VOID DestroyConnection(WNConnection* _connection);

    private:
        WN_VOID IOCPThread();
        WN_VOID ListenThread();

        enum eWNWindowsInitializationState {
            eWNNotStarted,
            eWNWSAStartup,
            eWNAcceptEventCreated,
            eWNShutdownEventCreated,
            eWNIOCPCreated,
            eWNThreadsCreated,
            eWNInitializationCompleted
        };

        WN_UINT32 mMaxThreads;
        HANDLE mIOCP;
        HANDLE mAcceptEvent;
        HANDLE mShutdownEvent;
        eWNWindowsInitializationState mInitializationState;
        WNConcurrency::WNThread<WN_VOID>* mListenThread;
        WNConcurrency::WNSpinLock mListenerMutex;
        WNConcurrency::WNSpinLock mRecievedMutex;
        WNConcurrency::WNSpinLock mInvalidMutex;
        WNConcurrency::WNSpinLock mOutgoingMutex;

        std::vector<WNConcurrency::WNThread<WN_VOID>*> mThreads;
        std::list<WNListenConnectionWindows*> mIncommingConnections;
        std::list<WNOutConnectionWindows*> mOutgoingConnections;
        std::list<WNInConnectionWindows*> mReceivedConnections;
        std::list<WNConnection*> mInvalidConnections;
    };
};

#endif // __WN_NETWORKING_NETWORK_MANAGER_WINDOWS_H__
