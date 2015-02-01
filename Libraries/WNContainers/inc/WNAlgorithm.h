// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_ALGORITHM_H__
#define __WN_ALGORITHM_H__
#include "WNCore/inc/WNTypes.h"

namespace WNContainers {
    
    template<typename T, typename Pred>
    WN_BOOL ContainerEqualIf(T i1, T i1end, T i2, T i2end, Pred _pr) {
        while(i1 != i1end && i2 != i2end) {
            if(!_pr(*i1, *i2))
            {
                return(false);
            }
            ++i1;
            ++i2;
        }
        return(i1 == i1end && i2 == i2end);
    }
    
    template<typename T>
    WN_BOOL ContainerEqual(T i1, T i1end, T i2, T i2end) {
        while(i1 != i1end && i2 != i2end) {
            if(*i1 != *i2)
            {
                return(false);
            }
            ++i1;
            ++i2;
        }
        return(i1 == i1end && i2 == i2end);
    }
}

#endif//__WN_ALGORITHM_H__
