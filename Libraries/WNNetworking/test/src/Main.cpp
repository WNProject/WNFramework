#include "WNCore/inc/WNMacros.h"
#include "WNCore/inc/WNTypes.h"
#include "WNNetworking/inc/WNNetworkFactory.h"
#include "WNNetworking/inc/WNNetworkManager.h"
#include "WNNetworking/inc/WNConnection.h"
#include "WNNetworking/inc/WNNetworkWriteBuffer.h"
#include "WNNetworking/inc/WNNetworkReadBuffer.h"
#include "WNConcurrency/inc/WNMutex.h"
#include "WNConcurrency/inc/WNLockGuard.h"
#include "WNConcurrency/inc/WNThread.h"
#include "WNContainers/inc/WNSerializer.h"

using namespace WNConcurrency;
using namespace WNContainers;
using namespace WNNetworking;

#define WN_PORT 60553
#define WN_ADDRESS "localhost"

static WNMutex gPrintMutex;
static WN_ATOM_T gAtomVal = 0;

template <WN_UINT32 Number>
static WN_VOID Message(WNConnection* _connection, WNNetworkReadBuffer& _buffer) {
    WN_UNUSED_ARG(_connection);

    WN_UINT64 val;

    _buffer.Serialize(0, WNSerializer<WN_UINT64>(val));

    {
        if (WNAtomicIncrement(&gAtomVal) % 1000 == 0) {
            WN_UINT32 atomVal = static_cast<WN_UINT32>(gAtomVal);
            WNLockGuard<WNMutex> lockGuard(gPrintMutex);

            WN_PRINTF("Value is now %d\n", atomVal);
        }
    }
}

static WN_BOOL ConnectedCallback(const WN_CHAR* _name) {
    {
        WNLockGuard<WNMutex> lockGuard(gPrintMutex);

        WN_PRINTF("Received Connection from %s\n", _name);
    }

    return(WN_TRUE);
}

WN_INT32 WNMain(WN_INT32 _argc, WN_CHAR* _argv[]) {
    WN_UNUSED_ARG(_argc);
    WN_UNUSED_ARG(_argv);

    WNNetworkManager* manager = WNNetworkFactory::CreateNetworkManager();
    WNNetworkManager* manager2 = WNNetworkFactory::CreateNetworkManager();

    manager->Initialize(2);
    manager2->Initialize(2);

    WNConnection* conn = WN_NULL;
    WNConnection* conn1 = WN_NULL;
    WNConnection* conn2 = WN_NULL;

    manager->CreateListener(conn, WNConnectionType::eWNReliable, static_cast<WN_UINT16>(WN_PORT), WNConnectedCallback(&ConnectedCallback));
    manager2->ConnectTo(conn1, WNConnectionType::eWNReliable, WN_ADDRESS, WN_PORT);
    manager2->ConnectTo(conn2, WNConnectionType::eWNReliable, WN_ADDRESS, WN_PORT);
    manager->SetCallback(1, WNMessageCallback(&Message<1>));
    manager->SetCallback(2, WNMessageCallback(&Message<2>));

    WNNetworkWriteBuffer buff(*manager2, 1);
    WNNetworkWriteBuffer buff2(*manager2, 2);
    WN_UINT64 number = 32;

    buff.Serialize(0, WNSerializer<WN_UINT64>(number));
    buff2.Serialize(0, WNSerializer<WN_UINT64>(number));

    for (WN_SIZE_T i = 0; i < 10000; ++i) {
        conn1->SendBuffer(buff);
        conn2->SendBuffer(buff2);
    }

    while (gAtomVal != 20000) {
        WNThreadSleep(1);
    }

    WN_DELETE(manager);
    return(0);
}
