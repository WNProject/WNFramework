////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_PLATFORM_SURFACE_INL__
#define __WN_PLATFORM_SURFACE_INL__

#ifndef __WN_PLATFORM_SURFACE_H__
    #error "WNSurface.inl should never be included directly. Please include WNSurface.h instead"
#endif

namespace WNPlatform {
    WN_INLINE WNSurface::WNSurface() :
        WNResourceBase() {
        for(WN_SIZE_T i = 0; i < eWNRDTMAX; ++i) {
            mRegisteredTypes[i] = WN_NULL;
        }
    }

    WN_INLINE WNSurface::~WNSurface() {
        FireCallback(eWNSETDestroyed);
    }

    WN_INLINE WN_VOID WNSurface::RegisterCallback(WNSurfaceEventType _type, const WNSurfaceEventCallback& _callback) {
        mEventCallbacks[_type].push_back(_callback);
    }

    WN_INLINE WN_VOID WNSurface::FireCallback(WNSurfaceEventType _type) {
        for (std::list<WNSurfaceEventCallback>::iterator i = mEventCallbacks[_type].begin(); i != mEventCallbacks[_type].end(); ++i) {
            (*i).Execute(_type, this);
        }
    }

    WN_INLINE WN_VOID WNSurface::RegisterData(WNRegisteredDataTypes _type, WN_VOID* _data) {
        WN_RELEASE_ASSERT(_type < eWNRDTMAX);
        WN_RELEASE_ASSERT(mRegisteredTypes[_type] == WN_NULL);

        mRegisteredTypes[_type] = _data;
    }
}

#endif // __WN_PLATFORM_SURFACE_INL__