////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "WNNetworking/inc/WNConnection.h"
#include "WNMemory/inc/WNMemory.h"

WNNetworking::WNConnection::WNConnection() :
    mConnectionName(WN_NULL),
    mIsValid(WN_TRUE) {
}

const WN_CHAR* WNNetworking::WNConnection::GetName() {
    return(mConnectionName);
}

WNNetworking::WNConnection::~WNConnection() {
    if (mConnectionName != WN_NULL) {
        WN_DELETE(mConnectionName);

        mConnectionName = WN_NULL;
    }
}

WN_VOID WNNetworking::WNConnection::Invalidate() {
    mIsValid = WN_FALSE;
}

WN_BOOL WNNetworking::WNConnection::IsValid() {
    return(mIsValid);
}