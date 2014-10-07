////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_NETWORKING_NETWORK_MANAGER_LINUX_H__
#define __WN_NETWORKING_NETWORK_MANAGER_LINUX_H__

#include "WNNetworking/inc/WNNetworkManager.h"
#include "WNConcurrency/inc/WNSpinLock.h"

#include <vector>

namespace WNConcurrency {
    template <typename Return>
    class WNThread;
}

namespace WNNetworking {
    class WNConnectionLinux;
    class WNListenConnectionLinux;

    class WNNetworkManagerLinux : public WNNetworkManager {
    public:
        WNNetworkManagerLinux();
        virtual ~WNNetworkManagerLinux();

        virtual WNNetworkManagerReturnCode::Type Initialize(WN_UINT32 _numWorkerThreads);
        virtual WNNetworkManagerReturnCode::Type ConnectTo(WNConnection*& _outHandle, WNConnectionType::Type _type, const WN_CHAR* _target, WN_UINT16 _port);
        virtual WNNetworkManagerReturnCode::Type CreateListener(WNConnection*& _outHandle, WNConnectionType::Type _type, WN_UINT16 _port, const WNConnectedCallback& _callback);
        virtual WN_VOID Cleanup();
        virtual WN_VOID DestroyConnection(WNConnection* _connection);

    private:
        enum eWNLinuxInitializationState {
            eWNNotStarted,
            eWNEPollWriteCreated,
            eWNWriteThreadsCreated,
            eWNEPollReadCreated,
            eWNReadThreadsCreated,
            eWNEPollListenCreated,
            eWNListenThreadCreated,
            eWNInitializationComplete
        };

    private:
        WN_BOOL AddToReadEPoll(WNConnectionLinux* _conn);
        WN_BOOL AddToWriteEPoll(WNConnectionLinux* _conn);
        WN_VOID CleanAllConnections();
        WN_VOID ReadThread();
        WN_VOID WriteThread();
        WN_VOID ListenThread();

    private:
        WNConcurrency::WNSpinLock mListenMutex;
        WNConcurrency::WNSpinLock mIncommingMutex;
        WNConcurrency::WNSpinLock mOutgoingMutex;
        WNConcurrency::WNSpinLock mInvalidMutex;

        eWNLinuxInitializationState mInitializationState;
        WN_INT32 mWriteEPollInstance;
        WN_INT32 mReadEPollInstance;
        WN_INT32 mListenEPollInstance;
        WNConcurrency::WNThread<WN_VOID>* mListenThread;
        WN_BOOL mShuttingDown;
        std::vector<WNConcurrency::WNThread<WN_VOID>*> mReadThreads;
        std::vector<WNConcurrency::WNThread<WN_VOID>*> mWriteThreads;
        std::list<WNListenConnectionLinux*> mListenConnections;
        std::list<WNConnection*> mInvalidConnections;
        std::list<WNConnectionLinux*> mIncommingConnections;
        std::list<WNConnectionLinux*> mOutgoingConnections;
    };
};

#endif // __WN_NETWORKING_NETWORK_MANAGER_WINDOWS_H__
