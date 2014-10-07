////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_CONCURRENCY_H__
#define __WN_CONCURRENCY_H__

#include "WNConcurrency/inc/WNAtomic.h"
#include "WNConcurrency/inc/WNCallbackJob.h"
#include "WNConcurrency/inc/WNCallOnce.h"
#include "WNConcurrency/inc/WNConditionVariable.h"
#include "WNConcurrency/inc/WNLockGuard.h"
#include "WNConcurrency/inc/WNMutex.h"
#include "WNConcurrency/inc/WNReadWriteLock.h"
#include "WNConcurrency/inc/WNRecursiveMutex.h"
#include "WNConcurrency/inc/WNRecursiveSpinLock.h"
#include "WNConcurrency/inc/WNResourceBase.h"
#include "WNConcurrency/inc/WNResourcePointer.h"
#include "WNConcurrency/inc/WNSemaphore.h"
#include "WNConcurrency/inc/WNSpinLock.h"
#include "WNConcurrency/inc/WNThread.h"
#include "WNConcurrency/inc/WNThreadJob.h"
#include "WNConcurrency/inc/WNThreadPool.h"

#endif // __WN_CONCURRENCY_H__