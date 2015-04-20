#include "WNCore/inc/WNMacros.h"
#include "WNCore/inc/WNTypes.h"
#include "WNNetworking/inc/WNNetworkFactory.h"
#include "WNNetworking/inc/WNNetworkManager.h"
#include "WNNetworking/inc/WNConnection.h"
#include "WNNetworking/inc/WNNetworkWriteBuffer.h"
#include "WNNetworking/inc/WNNetworkReadBuffer.h"
#include "WNConcurrency/inc/WNMutex.h"
#include "WNConcurrency/inc/WNThread.h"
#include "WNContainers/inc/WNSerializer.h"

#include <mutex>

using namespace WNConcurrency;
using namespace WNContainers;
using namespace WNNetworking;

#define WN_PORT 60553
#define WN_ADDRESS "localhost"

static wn::mutex gPrintMutex;
static std::atomic<wn_int32> gAtomVal({0});

template <wn_uint32 Number>
static wn_void Message(WNConnection* _connection, WNNetworkReadBuffer& _buffer) {
    WN_UNUSED_ARGUMENT(_connection);

    wn_uint64 val;

    _buffer.serialize(wn::containers::serializer<wn_uint64>(val), 0);

    {
        if ((++gAtomVal) % 1000 == 0) {
            const std::lock_guard<wn::mutex> lockGuard(gPrintMutex);

            ::printf("Value is now %d\n", static_cast<wn_int32>(gAtomVal));
        }
    }
}

static wn_bool ConnectedCallback(const wn_char* _name) {
    {
        const std::lock_guard<wn::mutex> lockGuard(gPrintMutex);

        ::printf("Received Connection from %s\n", _name);
    }

    return(wn_true);
}

wn_int32 wn_main(wn_int32 _argc, wn_char* _argv[]) {
    WN_UNUSED_ARGUMENT(_argc);
    WN_UNUSED_ARGUMENT(_argv);

    WNNetworkManager* manager = WNNetworkFactory::CreateNetworkManager();
    WNNetworkManager* manager2 = WNNetworkFactory::CreateNetworkManager();

    manager->Initialize(2);
    manager2->Initialize(2);

    WNConnection* conn = wn_nullptr;
    WNConnection* conn1 = wn_nullptr;
    WNConnection* conn2 = wn_nullptr;

    manager->CreateListener(conn, WNConnectionType::eWNReliable, static_cast<wn_uint16>(WN_PORT), WNConnectedCallback(&ConnectedCallback));
    manager2->ConnectTo(conn1, WNConnectionType::eWNReliable, WN_ADDRESS, WN_PORT);
    manager2->ConnectTo(conn2, WNConnectionType::eWNReliable, WN_ADDRESS, WN_PORT);
    manager->SetCallback(1, WNMessageCallback(&Message<1>));
    manager->SetCallback(2, WNMessageCallback(&Message<2>));

    WNNetworkWriteBuffer buff(*manager2, 1);
    WNNetworkWriteBuffer buff2(*manager2, 2);
    wn_uint64 number = 32;

    buff.serialize(wn::containers::serializer<wn_uint64>(number), 0);
    buff2.serialize(wn::containers::serializer<wn_uint64>(number), 0);

    for (wn_size_t i = 0; i < 10000; ++i) {
        conn1->SendBuffer(buff);
        conn2->SendBuffer(buff2);
    }

    while (gAtomVal != 20000) {
        wn::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    wn::memory::destroy(manager);

    return(0);
}
