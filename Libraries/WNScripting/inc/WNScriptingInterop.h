// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_SCRIPTING_INTEROP_H__
#define __WN_SCRIPTING_INTEROP_H__
#include "WNCore/inc/WNBase.h"
#include "WNScripting/inc/WNScriptingConfig.h"
#include "WNScripting/inc/WNCodeModule.h"
#include "WNScripting/inc/WNScriptingInteropForward.h"
#include "WNScripting/inc/WNScriptingEngine.h"
#include <algorithm>
namespace WNScripting {
    typedef void(*destrType)(void*);
    WN_INLINE wn_void FreeStruct(wn_void* str) {
        StructInternalType* sIp = *reinterpret_cast<StructInternalType**>(str);
        if(sIp) {
            if(sIp->owner == str && 0 != sIp->structLoc) {
                (**(destrType**)(sIp->structLoc))(sIp);
                free(sIp->structLoc);
            }
            if ((++sIp->refCount) == 0)
            {
                free(sIp);
            }
        }
    }

    WN_INLINE wn_void FreeArray(wn_void* _array) {
        StructInternalType* sIp = *reinterpret_cast<StructInternalType**>(_array);
        if(sIp) {
            if(sIp->owner == _array && 0 != sIp->structLoc) {
                wn_size_t curSize = *(reinterpret_cast<wn_size_t*>(sIp->structLoc));
                WNScriptType type = *reinterpret_cast<WNScriptType*>((reinterpret_cast<wn_size_t>(sIp->structLoc) + sizeof(wn_size_t)));
                WNScriptType nType = type->mArrayType;
                if(nType->mStoredVTable || nType->mArrayType) {

                    if(nType->mStoredVTable) {
                        for(wn_size_t i = 0; i < curSize; ++i) {
                            FreeStruct(reinterpret_cast<wn_char*>(sIp->structLoc) + sizeof(wn_size_t) * 2 + i * nType->mTypeSize);
                        }
                    } else {
                        for(wn_size_t i = 0; i < curSize; ++i) {
                            FreeArray(reinterpret_cast<wn_char*>(sIp->structLoc) + sizeof(wn_size_t) * 2 + i * nType->mTypeSize);
                        }
                    }
                }
                free(sIp->structLoc);
            }
            if ((++sIp->refCount) == 0)
            {
                free(sIp);
            }
        }
    }

    WN_INLINE wn_void Resize(wn_void* _array, wn_size_t _arraySize) {
        StructInternalType* sIp = reinterpret_cast<StructInternalType*>(_array);
        if(sIp == wn_nullptr) {
            return;
        }
        if(sIp->structLoc == wn_nullptr) {
            return;
        }
        wn_size_t curSize = *(reinterpret_cast<wn_size_t*>(sIp->structLoc));
        WNScriptType type = *reinterpret_cast<WNScriptType*>((reinterpret_cast<wn_size_t>(sIp->structLoc) + sizeof(wn_size_t)));
        WNScriptType nType = type->mArrayType;
        if(nType->mStoredVTable || nType->mArrayType) {

            if(nType->mStoredVTable) {
                for(wn_size_t i = _arraySize; i < curSize; ++i) {
                    FreeStruct(reinterpret_cast<wn_char*>(sIp->structLoc) + sizeof(wn_size_t) * 2 + i * nType->mTypeSize);
                }
            } else {
                for(wn_size_t i = _arraySize; i < curSize; ++i) {
                    FreeArray(reinterpret_cast<wn_char*>(sIp->structLoc) + sizeof(wn_size_t) * 2 + i * nType->mTypeSize);
                }
            }
            for(wn_size_t i = 0; i < ((curSize < _arraySize)? curSize : _arraySize); ++i) {
                StructInternalType** sp = reinterpret_cast<StructInternalType**>(reinterpret_cast<wn_char*>(sIp->structLoc) + sizeof(wn_size_t) * 2 + i * nType->mTypeSize);
                if((*sp)->owner == sp) {
                    (*sp)->owner = 0;
                }
            }
            sIp->structLoc = wn::memory::realloc(sIp->structLoc, nType->mTypeSize*_arraySize + sizeof(wn_size_t) * 2);
            for(wn_size_t i = 0; i < ((curSize < _arraySize)? curSize : _arraySize); ++i) {
                StructInternalType** sp = reinterpret_cast<StructInternalType**>(reinterpret_cast<wn_char*>(sIp->structLoc) + sizeof(wn_size_t) * 2 + i * nType->mTypeSize);
                if((*sp)->owner == 0) {
                    (*sp)->owner = sp;
                }
            }
            for(wn_size_t i = curSize; i < _arraySize; ++i) {
                StructInternalType* sp = reinterpret_cast<StructInternalType*>(malloc(sizeof(StructInternalType)));
                *reinterpret_cast<StructInternalType**>(reinterpret_cast<wn_char*>(sIp->structLoc) + sizeof(wn_size_t) * 2 + i * nType->mTypeSize) = sp;
                sp->owner = reinterpret_cast<StructInternalType**>(reinterpret_cast<wn_char*>(sIp->structLoc) + sizeof(wn_size_t) * 2 + i * nType->mTypeSize);
                sp->refCount = 1;
                sp->structLoc = wn_nullptr;
            }
        } else {
            sIp->structLoc = wn::memory::realloc(sIp->structLoc, nType->mTypeSize*_arraySize + sizeof(wn_size_t) * 2);
            if(_arraySize > curSize) {
                wn::memory::memset(reinterpret_cast<wn_char*>(sIp->structLoc) + sizeof(wn_size_t) * 2 + curSize * nType->mTypeSize, 0x00, (_arraySize - curSize) * nType->mTypeSize);
            }
        }
        *(reinterpret_cast<wn_size_t*>(sIp->structLoc)) = _arraySize;
    }

    class WNScriptingEngine;

    template<typename T, typename PT>
    class WNMemberVariablePointer;

   template<typename T, typename T_Parent = DummyType<0>>
   class WNScriptingObjectRef;

   template<typename T, typename T_Parent = DummyType<0>>
    class WNScriptingObject {
    public:
        typedef T_Parent ParentType;
        WNScriptingObject() : refData(wn_nullptr) {
        }
        WNScriptingObject(const WNScriptingObject<T, T_Parent>& _other) {
            refData = _other.refData;
            refData->refCount++;
        }
        WNScriptingObject(WNScriptingObject<T, T_Parent>&& _other) {
            refData = _other.refData;
            _other.refData = wn_nullptr;
        }
        ~WNScriptingObject() {
            Destroy();
        }

        WNScriptingObjectRef<T, T_Parent>* DetachOut() {
            WNScriptingObject<T, T_Parent>* ptr = reinterpret_cast<WNScriptingObject<T, T_Parent>*>(refData);
            refData->refCount--;
            refData = wn_nullptr;
            return(ptr);
        }
        WNScriptingObject<T, T_Parent>& operator=(WNScriptingObject<T, T_Parent>&& _other) {
            Destroy();
            refData = _other.refData;
            if(refData->owner == &_other) {
                refData->owner = this;
            }
            _other.refData = wn_nullptr;
            return(*this);
        }

        WNScriptingObject<T, T_Parent>& operator=(WNScriptingObject<T, T_Parent>& _other) {
            Destroy();
            refData = _other.refData;
            refData->refCount++;
            return(*this);
        }
        void Own(WNScriptingObject<T, T_Parent>& _other) {
            Destroy();
            refData = _other.refData;
            refData->refCount++;
            refData->owner = this;
        }

        template<typename Q, typename Q2>
        Q& operator->*(WNMemberVariablePointer<Q, Q2>& mQ) {
            return(mQ.GetValue(*this));
        }

        operator ParentType() {
            return(ParentType(refData));
        }
        operator typename ParentType::ParentType() {
            return(ParentType::ParentType(refData));
        }
        operator typename ParentType::ParentType::ParentType() {
            return(ParentType::ParentType::ParentType(refData));
        }
        operator typename ParentType::ParentType::ParentType::ParentType() {
            return(ParentType::ParentType::ParentType::ParentType(refData));
        }
        operator typename ParentType::ParentType::ParentType::ParentType::ParentType() {
            return(ParentType::ParentType::ParentType::ParentType::ParentType(refData));
        }
        operator typename ParentType::ParentType::ParentType::ParentType::ParentType::ParentType() {
            return(ParentType::ParentType::ParentType::ParentType::ParentType::ParentType(refData));
        }
        operator typename ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType() {
            return(ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType(refData));
        }
        operator typename ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType() {
            return(ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType(refData));
        }
        operator typename ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType() {
            return(ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType(refData));
        }
        operator typename ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType() {
            return(ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType(refData));
        }
        operator typename ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType() {
            return(ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType(refData));
        }

        WNScriptingObject(WNScriptingObjectRef<T, T_Parent>* _ptr) {
            refData = reinterpret_cast<StructInternalType*>(_ptr);
            if(refData->owner == wn_nullptr) {
                refData->owner = this;
            }
            refData->refCount++;
        }
        WNScriptingObject(void* _ptr) {
            refData = reinterpret_cast<StructInternalType*>(_ptr);
            if(refData->owner == wn_nullptr) {
                refData->owner = this;
            }
            refData->refCount++;
        }

        wn_void Destroy() {
            if(refData) {
                if(refData->owner == this) {
                    (**(destrType**)(refData->structLoc))(refData);
                    free(refData->structLoc);
                    refData->structLoc = 0;
                }
                if(refData) {
                    if ((--refData->refCount)==0)
                    {
                        free(refData);
                    }
                }
                refData = wn_nullptr;
            }
        }
        StructInternalType* refData;
        friend class WNScripting::WNScriptingEngine;

        template<typename T2, typename T3>
        friend class WNScriptingObject;

        friend struct TypeMapping<WNScriptingObject>;
        friend struct WNScripting::InCaster<WNScriptingObject<T, T_Parent>>;
        friend struct WNScripting::OutCaster<WNScriptingObject<T, T_Parent>>;
        template<typename TR, typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
        friend class WNScripting::WNMemberFunctionPointer;
    };
    enum eResolutionState {
        eUnresolved,
        eResolved,
        eFailedResolution
    };

    template<typename T, typename PT>
    class WNMemberVariablePointer {
    public:
        eResolutionState GetResolution() {
            TryToResolve();
            return(mResolution);
        }
        T& GetValue(const PT& x) {
            void* v =   reinterpret_cast<void*>(reinterpret_cast<wn_size_t>(x.refData->structLoc) + mOffset);
            return(*reinterpret_cast<T*>(v));
        }
        WNMemberVariablePointer(const wn_char* _name) :
            mEngine(wn_nullptr) {
            mVariableName = WNStrings::WNStrNDup(_name, 1024);
        }
        wn_void SetEngine(WNScriptingEngine* _engine) {
            mEngine = _engine;
        }
        WNMemberVariablePointer(const wn_char* _name, WNScriptingEngine* _engine) : mOffset(-1) {
            mEngine = _engine;
            mVariableName = WNStrings::WNStrNDup(_name, 1024);
            mResolution = eUnresolved;
            TryToResolve();
        }
    private:
        wn_void TryToResolve() {
            if(mOffset >= 0 || mVariableName == wn_nullptr || mEngine == wn_nullptr){
                return;
            }
            if(mVariableName) {
                WNScriptType type = mEngine->GetTypeByName(TypeMapping<PT>::GetTypeName());

                if(type != 0) {
                    std::vector<WNContainedStructType>::iterator i = std::find_if(type->mStructTypes.begin(), type->mStructTypes.end(),
                        [this](const WNContainedStructType& sType) { return(WNStrings::WNStrCmp(mVariableName, sType.mName) == 0); }
                    );
                    wn::memory::free(mVariableName);
                    mVariableName = wn_nullptr;
                    if(i == type->mStructTypes.end()) {
                        mResolution = eFailedResolution;
                        return;
                    }
                    if(i->mType != mEngine->GetTypeByName(TypeMapping<T>::GetTypeName())) {
                        mResolution = eFailedResolution;
                        return;
                    }
                    mOffset = i->mCppOffset;
                    mResolution = eResolved;
                } else {
                    mResolution = eUnresolved;
                }
            }
        }
        wn_int32 mOffset;
        wn_char* mVariableName;
        WNScriptingEngine* mEngine;
        eResolutionState mResolution;
        template<typename TP, typename TS>
        friend class WNScriptingObject;
    };

    template<typename T>
    class WNScriptingArrayRef;
    template<typename T>
    class WNScriptingArray {
    public:
        WNScriptingArray() : refData(wn_nullptr) {
        }
        WNScriptingArray(const WNScriptingArray <T>& _other) {
            refData = _other.refData;
            refData->refCount++;
        }
        WNScriptingArray(WNScriptingArray <T>&& _other) {
            refData = _other.refData;
            _other.refData = wn_nullptr;
        }

        ~WNScriptingArray() {
            Destroy();
        }
        wn_void Resize(wn_size_t _newLength) {
            WNScripting::Resize(refData, _newLength);
        }
        WNScriptingArrayRef<T>* DetachOut() {
            if(refData) {
                WNScriptingArrayRef<T>* ptr = reinterpret_cast<WNScriptingArrayRef<T>*>(refData);
                refData->refCount--;
                refData = wn_nullptr;
                return(ptr);
            }
            return(wn_nullptr);
        }
        WNScriptingArray<T>& ReleaseOwnership() {
            if(refData) {
                refData->owner = reinterpret_cast<void*>(0);
            }
            return(*this);
        }
        WNScriptingArray<T>& operator=(WNScriptingArray<T>&& _other) {
            Destroy();
            refData = _other.refData;
            if(refData->owner == &_other) {
                refData->owner = this;
            }
            _other.refData = wn_nullptr;
            return(*this);
        }
        bool operator!() {
            return(refData && refData->structLoc);
        }
        WNScriptingArray<T>& operator=(WNScriptingArray<T>& _other) {
            Destroy();
            refData = _other.refData;
            refData->refCount++;
            return(*this);
        }
        void Own(WNScriptingArray<T>& _other) {
            Destroy();
            refData = _other.refData;
            refData->refCount++;
            refData->owner = this;
        }

        WNScriptingArray(WNScriptingArrayRef<T>* _ptr) {
            refData = reinterpret_cast<StructInternalType*>(_ptr);
            if(refData->owner == wn_nullptr) {
                refData->owner = this;
            }
            refData->refCount++;
        }

        WNScriptingArray(void* _ptr) {
            refData = reinterpret_cast<StructInternalType*>(_ptr);
            if(refData->owner == wn_nullptr) {
                refData->owner = this;
            }
            refData->refCount++;
        }

        WNScriptType GetType() {
            if(wn_nullptr == refData || wn_nullptr == refData->structLoc) {
                return(wn_nullptr);;
            }
            WNScriptType type = *reinterpret_cast<WNScriptType*>((reinterpret_cast<wn_size_t>(refData->structLoc) + sizeof(wn_size_t)));
            return(type);
        }


        T& operator[](wn_size_t i) {
            void* v = reinterpret_cast<void*>(reinterpret_cast<wn_size_t>(refData->structLoc) + 2 * sizeof(wn_size_t) + (i * sizeof(T)));
            return(*reinterpret_cast<T*>(v));
        }
        wn_size_t GetLength() {
            if(refData) {
                if(refData->structLoc) {
                    return(*reinterpret_cast<wn_size_t*>(refData->structLoc));
                }
            }
            return(0);
        }
        wn_void Destroy() {
            if(refData) {
                if(refData->owner == this) {
                    wn_size_t length = *reinterpret_cast<wn_size_t*>(refData->structLoc);
                    if(length > 0) {
                        T* startT = reinterpret_cast<T*>(reinterpret_cast<wn_size_t>(refData->structLoc) + sizeof(wn_size_t));
                        for(wn_size_t i = 0; i < length; ++i) {
                            startT[i].~T();
                        }
                    }
                    free(refData->structLoc);
                    refData->structLoc = 0;
                }
                if(refData) {
                    if ((refData->refCount--) == 0)
                    {
                        free(refData);
                    }
                }
                refData = wn_nullptr;
            }
        }
        StructInternalType* refData;
        friend class WNScripting::WNScriptingEngine;
        friend struct TypeMapping<WNScriptingArray<T>>;
        friend struct WNScripting::InCaster<WNScriptingArray<T>>;
        friend struct WNScripting::OutCaster<WNScriptingArray<T>>;
    };


    template<typename T>
    struct TypeMapping<WNScriptingArray<T>> {
        static const wn_bool RawInputType = wn_false;
        static const wn_bool RawOutputType = wn_true;
        static const wn_char * GetTypeName() { return TypeMapping<T>::GetTypeName(); }
        static WNScriptType GetModifiedType(WNScriptType t, WNScriptingEngine* e){ return e->GetArrayOf(TypeMapping<T>::GetModifiedType(t, e)); }
    };

    template<typename T>
    struct TypeMapping<WNScriptingArrayRef<T>*> {
        static const wn_bool RawInputType = wn_true;
        static const wn_bool RawOutputType = wn_false;
        static const wn_char * GetTypeName() { return TypeMapping<T>::GetTypeName(); }
        static WNScriptType GetModifiedType(WNScriptType t, WNScriptingEngine* e){ return e->GetArrayOf(TypeMapping<T>::GetModifiedType(t, e)); }
    };

    template<typename T>
    struct InCaster<WNScriptingArray<T>> {
        typedef void* OutType;
        static OutType ConvertOut(const WNScriptingArray<T>& tVal) {
            return(tVal.refData);
        }
    };

    template<typename T>
    struct OutCaster<WNScriptingArray<T>> {
        typedef void* InType;
        static WNScriptingArray<T> ConvertOut(void* tVal) {
            return(WNScriptingArray<T>(tVal));
        }
    };

    template<typename T, typename T_Parent = DummyType<0>>
    class ScriptPointerRef;

    template<typename T, typename T_Parent = DummyType<0>>
    class ScriptPointer {
    public:
        typedef T_Parent ParentType;
        ScriptPointer(T* _ptr) {
            refData = new StructInternalType();
            refData->refCount = 1;
            refData->structLoc = _ptr;
            refData->owner = this;
        }
        ScriptPointer() {
            refData = new StructInternalType();
            refData->refCount = 1;
            refData->structLoc = wn_nullptr;
            refData->owner = this;
        }
        ScriptPointer(StructInternalType* _ptr) {
            refData = _ptr;
        }
        ScriptPointer(ScriptPointerRef<T, T_Parent>* _ptr) {
            refData = reinterpret_cast<StructInternalType*>(_ptr);
            refData->refCount++;
            if(refData->owner == wn_nullptr) {
                refData->owner = this;
            }
        }
        ScriptPointer(const ScriptPointer<T, T_Parent>& _other) {
            refData = _other.refData;
            refData->refCount++;
        }
        ScriptPointer(ScriptPointer<T, T_Parent>&& _other) {
            refData = _other.refData;
            _other.refData = wn_nullptr;
        }
        ScriptPointerRef<T, T_Parent>* DetachOut() {
            ScriptPointerRef<T, T_Parent>* ptr = reinterpret_cast<ScriptPointerRef<T, T_Parent>*>(refData);
            refData->refCount--;
            refData = wn_nullptr;
            return(ptr);
        }
        ~ScriptPointer() {
            Destroy();
        }
        ScriptPointer<T, T_Parent>& ReleaseOwnership() {
            if(refData) {
                refData->owner = reinterpret_cast<void*>(0);
            }
            return(*this);
        }
        const ScriptPointer<T, T_Parent>& operator=(T* t) {
            Destroy();
            refData = new StructInternalType();
            refData->owner = this;
            refData->refCount = 1;
            refData->structLoc = reinterpret_cast<void*>(t);
            return(*this);
        }
        T* Detach() {
            if(refData) {
                T* tDat = reinterpret_cast<T*>(refData->structLoc);
                refData->structLoc = wn_nullptr;
                refData->owner = this;
                if ((refData->refCount--) == 0)
                {
                    free(refData);
                }
                refData = NULL;
                return(tDat);
            }
            return(wn_nullptr);
        }

        operator T*() {
            return(reinterpret_cast<T*>(refData->structLoc));
        }

        T* operator->() {
            return(reinterpret_cast<T*>(refData->structLoc));
        }

        wn_void Destroy() {
            if(refData) {
                if(refData->owner == this && refData->structLoc) {
                    reinterpret_cast<T*>(refData->structLoc)->~T();
                    free(refData->structLoc);
                    refData->structLoc = 0;
                }
                if(refData) {
                    if(0 == WNConcurrency::WNAtomicDecrement(reinterpret_cast<wn_atom_t*>(&refData->refCount)))
                    {
                        free(refData);
                    }
                }
                refData = wn_nullptr;
            }
        }
        ScriptPointer<T, T_Parent>& operator=(ScriptPointer<T, T_Parent>&& _other) {
            Destroy();
            refData = _other.refData;
            if(refData->owner == &_other || refData->owner == wn_nullptr) {
                refData->owner = this;
            }
            _other.refData = wn_nullptr;
            return(*this);
        }

        ScriptPointer<T, T_Parent>& operator=(ScriptPointer<T, T_Parent>& _other) {
            Destroy();
            refData = _other.refData;
            if(refData->owner == wn_nullptr) {
                refData->owner = this;
            }
            WNConcurrency::WNAtomicIncrement(reinterpret_cast<wn_atom_t*>(&refData->refCount));
            return(*this);
        }
        void Own(ScriptPointer<T, T_Parent>& _other) {
            Destroy();
            refData = _other.refData;
            WNConcurrency::WNAtomicIncrement(reinterpret_cast<wn_atom_t*>(&refData->refCount));
            refData->owner = this;
        }

        bool operator !() {
            return(refData == wn_nullptr || refData->structLoc == wn_nullptr);
        }
        operator ParentType() {
            return(ParentType(refData));
        }
        operator typename ParentType::ParentType() {
            return(ParentType::ParentType(refData));
        }
        operator typename ParentType::ParentType::ParentType() {
            return(ParentType::ParentType::ParentType(refData));
        }
        operator typename ParentType::ParentType::ParentType::ParentType() {
            return(ParentType::ParentType::ParentType::ParentType(refData));
        }
        operator typename ParentType::ParentType::ParentType::ParentType::ParentType() {
            return(ParentType::ParentType::ParentType::ParentType::ParentType(refData));
        }
        operator typename ParentType::ParentType::ParentType::ParentType::ParentType::ParentType() {
            return(ParentType::ParentType::ParentType::ParentType::ParentType::ParentType(refData));
        }
        operator typename ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType() {
            return(ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType(refData));
        }
        operator typename ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType() {
            return(ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType(refData));
        }
        operator typename ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType() {
            return(ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType(refData));
        }
        operator typename ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType() {
            return(ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType(refData));
        }
        operator typename ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType() {
            return(ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType::ParentType(refData));
        }
        typedef T CType;
    private:
        StructInternalType* refData;

        template<typename T2, typename T3>
        friend class ScriptPointer;
        friend struct TypeMapping<ScriptPointer>;
        friend struct WNScripting::InCaster<ScriptPointer<T, T_Parent>>;
        friend struct WNScripting::InCaster<T>;
        friend struct WNScripting::OutCaster<ScriptPointer<T, T_Parent>>;
        template<typename T2, typename T3>
        friend class WNMemberVariablePointer;
    };
}
    #define DEFINE_TEMPLATE_MAPPING(_type, _name) \
        namespace WNScripting { \
        template<> \
        struct TypeMapping<_type> { \
            static const wn_bool RawInputType = wn_true; \
            static const wn_bool RawOutputType = wn_true; \
            static const wn_char * GetTypeName() { return #_name; } \
            static WNScriptType GetModifiedType(WNScriptType t, WNScriptingEngine*){return t; } \
        }; \
        }

DEFINE_TEMPLATE_MAPPING(wn_int32, Int);
DEFINE_TEMPLATE_MAPPING(wn_float32, Float);
DEFINE_TEMPLATE_MAPPING(wn_char, Char);
DEFINE_TEMPLATE_MAPPING(wn_bool, Bool);
DEFINE_TEMPLATE_MAPPING(wn_void, Void);
DEFINE_TEMPLATE_MAPPING(wn_void*, -Ptr);
DEFINE_TEMPLATE_MAPPING(wn_size_t, -SizeT);

template<char const* T>
struct ScriptObjectType {
    static const char* GetStr() {
        return T;
    }
};

namespace WNScripting{
    template<typename T>
    struct IsDummy {
        const static int val = 0;
    };
    template<>
    struct IsDummy<DummyType<0>> {
        const static int val = 1;
    };

    template<typename InType>
    struct InCaster {
        typedef InType OutType;
        static const OutType& ConvertOut(const InType& tVal) {
            return(tVal);
        }
    };

    template<typename OutType>
    struct OutCaster {
        typedef OutType InType;
        static const OutType& ConvertOut(const InType& tVal) {
            return(tVal);
        }
    };

    template<>
    struct OutCaster<wn_void> {
        typedef wn_void* InType;
        static const wn_void ConvertOut(const InType&) {
            return;
        }
    };

    template<typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
    class WNFunctionPointer {
    public:
        WNFunctionPointer() :mReturnType(wn_nullptr), callPt(wn_nullptr){
        }
        wn_void Assign(R (*_callPt)()) {
            WN_STATIC_ASSERT(IsDummy<T1>::val == 1);
            callPt = reinterpret_cast<void*>(_callPt);
        }
        wn_void Assign(R (*_callPt)(T1)) {
            callPt = reinterpret_cast<void*>(_callPt);
        }
        wn_void Assign(R (*_callPt)(T1, T2)) {
            callPt = reinterpret_cast<void*>(_callPt);
        }
        wn_void Assign(R (*_callPt)(T1, T2, T3)) {
            callPt = reinterpret_cast<void*>(_callPt);
        }
        wn_void Assign(R (*_callPt)(T1, T2, T3, T4)) {
            callPt = reinterpret_cast<void*>(_callPt);
        }
        wn_void Assign(R (*_callPt)(T1, T2, T3, T4, T5)) {
            callPt = reinterpret_cast<void*>(_callPt);
        }
        wn_void Assign(R (*_callPt)(T1, T2, T3, T4, T5, T6)) {
            WN_STATIC_ASSERT(IsDummy<T6>::val == 0);
            callPt = reinterpret_cast<void*>(_callPt);
        }

        wn_void SetOutType(WNScriptType _type) {
            mReturnType = _type;
        }

        R operator()() {
            WN_STATIC_ASSERT(IsDummy<T1>::val == 1);
            return(OutCaster<R>::ConvertOut(reinterpret_cast<typename OutCaster<R>::InType (*)()>(callPt)()));
        }
        R  operator()(const T1 & t1) {
            WN_STATIC_ASSERT(IsDummy<T1>::val == 0 && IsDummy<T2>::val == 1);
            return(OutCaster<R>::ConvertOut(reinterpret_cast<typename OutCaster<R>::InType (*)(typename InCaster<T1>::OutType)>(callPt)(InCaster<T1>::ConvertOut(t1))));
        }
        R  operator()(const T1& t1, const T2& t2) {
            WN_STATIC_ASSERT(IsDummy<T2>::val == 0 && IsDummy<T3>::val == 1);
            return(OutCaster<R>::ConvertOut(reinterpret_cast<typename OutCaster<R>::InType (*)(typename InCaster<T1>::OutType, typename InCaster<T2>::OutType)>(callPt)(InCaster<T1>::ConvertOut(t1), InCaster<T2>::ConvertOut(t2))));
        }
        R  operator()(const T1& t1, const T2& t2, const T3& t3) {
            WN_STATIC_ASSERT(IsDummy<T3>::val == 0 && IsDummy<T4>::val == 1);
            return(OutCaster<R>::ConvertOut(reinterpret_cast<typename OutCaster<R>::InType (*)(typename InCaster<T1>::OutType, typename InCaster<T2>::OutType, typename InCaster<T3>::OutType)>(callPt)(InCaster<T1>::ConvertOut(t1), InCaster<T2>::ConvertOut(t2), InCaster<T3>::ConvertOut(t3))));
        }
        R  operator()(const T1& t1, const T2& t2, const T3& t3, const T4& t4) {
            WN_STATIC_ASSERT(IsDummy<T4>::val == 0 && IsDummy<T5>::val == 1);
            return(OutCaster<R>::ConvertOut(reinterpret_cast<typename OutCaster<R>::InType (*)(typename InCaster<T1>::OutType, typename InCaster<T2>::OutType, typename InCaster<T3>::OutType, typename InCaster<T4>::OutType)>(callPt)(InCaster<T1>::ConvertOut(t1), InCaster<T2>::ConvertOut(t2), InCaster<T3>::ConvertOut(t3), InCaster<T4>::ConvertOut(t4))));
        }
        R  operator()(const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5) {
            WN_STATIC_ASSERT(IsDummy<T5>::val == 0 && IsDummy<T6>::val == 1);
            return(OutCaster<R>::ConvertOut(reinterpret_cast<typename OutCaster<R>::InType (*)(typename InCaster<T1>::OutType, typename InCaster<T2>::OutType, typename InCaster<T3>::OutType, typename InCaster<T4>::OutType, typename InCaster<T5>::OutType)>(callPt)(InCaster<T1>::ConvertOut(t1), InCaster<T2>::ConvertOut(t2), InCaster<T3>::ConvertOut(t3), InCaster<T4>::ConvertOut(t4), InCaster<T5>::ConvertOut(t5))));
        }
        R  operator()(const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T6& t5, const T6& t6) {
            WN_STATIC_ASSERT(IsDummy<T6>::val == 0);
            return(OutCaster<R>::ConvertOut(reinterpret_cast<typename OutCaster<R>::InType (*)(typename InCaster<T1>::OutType, typename InCaster<T2>::OutType, typename InCaster<T3>::OutType, typename InCaster<T4>::OutType, typename InCaster<T5>::OutType, typename InCaster<T6>::OutType)>(callPt)(InCaster<T1>::ConvertOut(t1), InCaster<T2>::ConvertOut(t2), InCaster<T3>::ConvertOut(t3), InCaster<T4>::ConvertOut(t4), InCaster<T5>::ConvertOut(t5), InCaster<T6>::ConvertOut(t6))));
        }
    private:
        void* callPt;
        WNScriptType mReturnType;
    };

    template<typename T, typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
    class WNMemberFunctionPointer {
    public:
        WNMemberFunctionPointer() :mReturnType(wn_nullptr), callPt(wn_nullptr), mVirtualIndex(-1){
        }

        wn_void Assign(wn_int32 _virtualIndex) {
            mVirtualIndex = _virtualIndex;
        }

        wn_void Assign(R (*_callPt)(T)) {
            WN_STATIC_ASSERT(IsDummy<T1>::val == 1);
            callPt = reinterpret_cast<void*>(_callPt);
        }
        wn_void Assign(R (*_callPt)(T, T1)) {
            WN_STATIC_ASSERT(IsDummy<T1>::val == 0 && IsDummy<T2>::val == 1);
            callPt = reinterpret_cast<void*>(_callPt);
        }
        wn_void Assign(R (*_callPt)(T, T1, T2)) {
            WN_STATIC_ASSERT(IsDummy<T2>::val == 0 && IsDummy<T3>::val == 1);
            callPt = reinterpret_cast<void*>(_callPt);
        }
        wn_void Assign(R (*_callPt)(T, T1, T2, T3)) {
            WN_STATIC_ASSERT(IsDummy<T3>::val == 0 && IsDummy<T4>::val == 1);
            callPt = reinterpret_cast<void*>(_callPt);
        }
        wn_void Assign(R (*_callPt)(T, T1, T2, T3, T4)) {
            WN_STATIC_ASSERT(IsDummy<T4>::val == 0 && IsDummy<T5>::val == 1);
            callPt = reinterpret_cast<void*>(_callPt);
        }
        wn_void Assign(R (*_callPt)(T, T1, T2, T3, T4, T5)) {
            WN_STATIC_ASSERT(IsDummy<T5>::val == 0 && IsDummy<T6>::val == 1);
            callPt = reinterpret_cast<void*>(_callPt);
        }
        wn_void Assign(R (*_callPt)(T, T1, T2, T3, T4, T5, T6)) {
            WN_STATIC_ASSERT(IsDummy<T6>::val == 0);
            callPt = reinterpret_cast<void*>(_callPt);
        }

        wn_void SetOutType(WNScriptType _type) {
            mReturnType = _type;
        }

        R operator()(const T& t) {
            WN_STATIC_ASSERT(IsDummy<T1>::val == 1);
            WN_RELEASE_ASSERT(callPt || mVirtualIndex >= 0);
            if(callPt) {
                return(OutCaster<R>::ConvertOut(reinterpret_cast<typename OutCaster<R>::InType (*)(typename InCaster<T>::OutType)>(callPt)(InCaster<T>::ConvertOut(t))));
            } else {
                StructInternalType* m = t.refData;
                void* pt = (*(void***)(m->structLoc))[2 + 2*mVirtualIndex];
                if(!pt) {
                    WNFunctionDefinition* def = reinterpret_cast<WNFunctionDefinition*>((*(void***)(m->structLoc))[2 + 2*mVirtualIndex + 1]);
                    WN_RELEASE_ASSERT(ok == def->mScriptingEngine->GetFunctionPointer(def->mContainedFile->mFileName, def->mName, def->mReturn, def->mTypes, pt));
                    (*(void***)(m->structLoc))[2 + 2*mVirtualIndex] = pt;
                }
                return(OutCaster<R>::ConvertOut((reinterpret_cast<typename OutCaster<R>::InType(*)(typename InCaster<T>::OutType)>(pt))(InCaster<T>::ConvertOut(t))));
            }
        }
        R  operator()(const T& t, const T1 & t1) {
            WN_STATIC_ASSERT(IsDummy<T1>::val == 0 && IsDummy<T2>::val == 1);
            WN_RELEASE_ASSERT(callPt || mVirtualIndex >= 0);
            if(callPt) {
                return(OutCaster<R>::ConvertOut(reinterpret_cast<typename OutCaster<R>::InType (*)(typename InCaster<T>::OutType, typename InCaster<T1>::OutType)>(callPt)(InCaster<T>::ConvertOut(t), InCaster<T1>::ConvertOut(t1))));
            } else {
                StructInternalType* m = t.refData;
                void* pt = (*(void***)(m->structLoc))[2 + 2*mVirtualIndex];
                if(!pt) {
                    WNFunctionDefinition* def = reinterpret_cast<WNFunctionDefinition*>((*(void***)(m->structLoc))[2 + 2*mVirtualIndex + 1]);
                    WN_RELEASE_ASSERT(ok == def->mScriptingEngine->GetFunctionPointer(def->mContainedFile->mFileName, def->mName, def->mReturn, def->mTypes, pt));
                    (*(void***)(m->structLoc))[2 + 2*mVirtualIndex] = pt;
                }
                return(OutCaster<R>::ConvertOut(reinterpret_cast<typename OutCaster<R>::InType (*)(typename InCaster<T>::OutType, typename InCaster<T1>::OutType)>(callPt)(InCaster<T>::ConvertOut(t), InCaster<T1>::ConvertOut(t1))));
            }
        }
        R  operator()(const T& t, const T1& t1, const T2& t2) {
            WN_STATIC_ASSERT(IsDummy<T2>::val == 0 && IsDummy<T3>::val == 1);
            WN_RELEASE_ASSERT(callPt || mVirtualIndex >= 0);
            if(callPt) {
                return(OutCaster<R>::ConvertOut(reinterpret_cast<typename OutCaster<R>::InType (*)(typename InCaster<T>::OutType, typename InCaster<T1>::OutType, typename InCaster<T2>::OutType)>(callPt)(InCaster<T>::ConvertOut(t), InCaster<T1>::ConvertOut(t1), InCaster<T2>::ConvertOut(t2))));
            } else {
                StructInternalType* m = t.refData;
                void* pt = (*(void***)(m->structLoc))[2 + 2*mVirtualIndex];
                if(!pt) {
                    WNFunctionDefinition* def = reinterpret_cast<WNFunctionDefinition*>((*(void***)(m->structLoc))[2 + 2*mVirtualIndex + 1]);
                    WN_RELEASE_ASSERT(ok == def->mScriptingEngine->GetFunctionPointer(def->mContainedFile->mFileName, def->mName, def->mReturn, def->mTypes, pt));
                    (*(void***)(m->structLoc))[2 + 2*mVirtualIndex] = pt;
                }
                return(OutCaster<R>::ConvertOut(reinterpret_cast<typename OutCaster<R>::InType (*)(typename InCaster<T>::OutType, typename InCaster<T1>::OutType, typename InCaster<T2>::OutType)>(pt)(InCaster<T>::ConvertOut(t), InCaster<T1>::ConvertOut(t1), InCaster<T2>::ConvertOut(t2))));
            }
        }
        R  operator()(const T& t, const T1& t1, const T2& t2, const T3& t3) {
            WN_STATIC_ASSERT(IsDummy<T3>::val == 0 && IsDummy<T4>::val == 1);
            WN_RELEASE_ASSERT(callPt || mVirtualIndex >= 0);
            if(callPt) {
                return(OutCaster<R>::ConvertOut(reinterpret_cast<typename OutCaster<R>::InType (*)(typename InCaster<T>::OutType, typename InCaster<T1>::OutType, typename InCaster<T2>::OutType, typename InCaster<T3>::OutType)>(callPt)(InCaster<T>::ConvertOut(t), InCaster<T1>::ConvertOut(t1), InCaster<T2>::ConvertOut(t2), InCaster<T3>::ConvertOut(t3))));
            } else {
                StructInternalType* m = t.refData;
                void* pt = (*(void***)(m->structLoc))[2 + 2*mVirtualIndex];
                if(!pt) {
                    WNFunctionDefinition* def = reinterpret_cast<WNFunctionDefinition*>((*(void***)(m->structLoc))[2 + 2*mVirtualIndex + 1]);
                    WN_RELEASE_ASSERT(ok == def->mScriptingEngine->GetFunctionPointer(def->mContainedFile->mFileName, def->mName, def->mReturn, def->mTypes, pt));
                    (*(void***)(m->structLoc))[2 + 2*mVirtualIndex] = pt;
                }
                return(OutCaster<R>::ConvertOut(reinterpret_cast<typename OutCaster<R>::InType (*)(typename InCaster<T>::OutType, typename InCaster<T1>::OutType, typename InCaster<T2>::OutType, typename InCaster<T3>::OutType)>(pt)(InCaster<T>::ConvertOut(t), InCaster<T1>::ConvertOut(t1), InCaster<T2>::ConvertOut(t2), InCaster<T3>::ConvertOut(t3))));
            }
        }
        R  operator()(const T& t, const T1& t1, const T2& t2, const T3& t3, const T4& t4) {
            WN_STATIC_ASSERT(IsDummy<T4>::val == 0 && IsDummy<T5>::val == 1);
            WN_RELEASE_ASSERT(callPt || mVirtualIndex >= 0);
            if(callPt) {
                return(OutCaster<R>::ConvertOut(reinterpret_cast<typename OutCaster<R>::InType (*)(typename InCaster<T>::OutType, typename InCaster<T1>::OutType, typename InCaster<T2>::OutType, typename InCaster<T3>::OutType, typename InCaster<T4>::OutType)>(callPt)(InCaster<T>::ConvertOut(t), InCaster<T1>::ConvertOut(t1), InCaster<T2>::ConvertOut(t2), InCaster<T3>::ConvertOut(t3), InCaster<T4>::ConvertOut(t4))));
            } else {
                StructInternalType* m = t.refData;
                void* pt = (*(void***)(m->structLoc))[2 + 2*mVirtualIndex];
                if(!pt) {
                    WNFunctionDefinition* def = reinterpret_cast<WNFunctionDefinition*>((*(void***)(m->structLoc))[2 + 2*mVirtualIndex + 1]);
                    WN_RELEASE_ASSERT(ok == def->mScriptingEngine->GetFunctionPointer(def->mContainedFile->mFileName, def->mName, def->mReturn, def->mTypes, pt));
                    (*(void***)(m->structLoc))[2 + 2*mVirtualIndex] = pt;
                }
                return(OutCaster<R>::ConvertOut(reinterpret_cast<typename OutCaster<R>::InType (*)(typename InCaster<T>::OutType, typename InCaster<T1>::OutType, typename InCaster<T2>::OutType, typename InCaster<T3>::OutType, typename InCaster<T4>::OutType)>(pt)(InCaster<T>::ConvertOut(t), InCaster<T1>::ConvertOut(t1), InCaster<T2>::ConvertOut(t2), InCaster<T3>::ConvertOut(t3), InCaster<T4>::ConvertOut(t4))));
            }
        }
        R  operator()(const T& t, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5) {
            WN_STATIC_ASSERT(IsDummy<T5>::val == 0 && IsDummy<T6>::val == 1);
            WN_RELEASE_ASSERT(callPt || mVirtualIndex >= 0);
            if(callPt) {
                return(OutCaster<R>::ConvertOut(reinterpret_cast<typename OutCaster<R>::InType (*)(typename InCaster<T>::OutType, typename InCaster<T1>::OutType, typename InCaster<T2>::OutType, typename InCaster<T3>::OutType, typename InCaster<T4>::OutType, typename InCaster<T5>::OutType)>(callPt)(InCaster<T>::ConvertOut(t), InCaster<T1>::ConvertOut(t1), InCaster<T2>::ConvertOut(t2), InCaster<T3>::ConvertOut(t3), InCaster<T4>::ConvertOut(t4), InCaster<T5>::ConvertOut(t5))));
            } else {
                StructInternalType* m = t.refData;
                void* pt = (*(void***)(m->structLoc))[2 + 2*mVirtualIndex];
                if(!pt) {
                    WNFunctionDefinition* def = reinterpret_cast<WNFunctionDefinition*>((*(void***)(m->structLoc))[2 + 2*mVirtualIndex + 1]);
                    WN_RELEASE_ASSERT(ok == def->mScriptingEngine->GetFunctionPointer(def->mContainedFile->mFileName, def->mName, def->mReturn, def->mTypes, pt));
                    (*(void***)(m->structLoc))[2 + 2*mVirtualIndex] = pt;
                }
                return(OutCaster<R>::ConvertOut(reinterpret_cast<typename OutCaster<R>::InType (*)(typename InCaster<T>::OutType, typename InCaster<T1>::OutType, typename InCaster<T2>::OutType, typename InCaster<T3>::OutType, typename InCaster<T4>::OutType, typename InCaster<T5>::OutType)>(pt)(InCaster<T>::ConvertOut(t), InCaster<T1>::ConvertOut(t1), InCaster<T2>::ConvertOut(t2), InCaster<T3>::ConvertOut(t3), InCaster<T4>::ConvertOut(t4), InCaster<T5>::ConvertOut(t5))));
            }
        }
        R  operator()(const T& t, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T6& t5, const T6& t6) {
            WN_STATIC_ASSERT(IsDummy<T6>::val == 0);
            WN_RELEASE_ASSERT(callPt || mVirtualIndex >= 0);
            if(callPt) {
                return(OutCaster<R>::ConvertOut(reinterpret_cast<typename OutCaster<R>::InType (*)(typename InCaster<T>::OutType, typename InCaster<T1>::OutType, typename InCaster<T2>::OutType, typename InCaster<T3>::OutType, typename InCaster<T4>::OutType, typename InCaster<T5>::OutType, typename InCaster<T6>::OutType)>(callPt)(InCaster<T>::ConvertOut(t), InCaster<T1>::ConvertOut(t1), InCaster<T2>::ConvertOut(t2), InCaster<T3>::ConvertOut(t3), InCaster<T4>::ConvertOut(t4), InCaster<T5>::ConvertOut(t5), InCaster<T6>::ConvertOut(t6))));
            } else {
                StructInternalType* m = t.refData;
                void* pt = (*(void***)(m->structLoc))[2 + 2*mVirtualIndex];
                if(!pt) {
                    WNFunctionDefinition* def = reinterpret_cast<WNFunctionDefinition*>((*(void***)(m->structLoc))[2 + 2*mVirtualIndex + 1]);
                    WN_RELEASE_ASSERT(ok == def->mScriptingEngine->GetFunctionPointer(def->mContainedFile->mFileName, def->mName, def->mReturn, def->mTypes, pt));
                    (*(void***)(m->structLoc))[2 + 2*mVirtualIndex] = pt;
                }
                return(OutCaster<R>::ConvertOut(reinterpret_cast<typename OutCaster<R>::InType (*)(typename InCaster<T>::OutType, typename InCaster<T1>::OutType, typename InCaster<T2>::OutType, typename InCaster<T3>::OutType, typename InCaster<T4>::OutType, typename InCaster<T5>::OutType, typename InCaster<T6>::OutType)>(pt)(InCaster<T>::ConvertOut(t), InCaster<T1>::ConvertOut(t1), InCaster<T2>::ConvertOut(t2), InCaster<T3>::ConvertOut(t3), InCaster<T4>::ConvertOut(t4), InCaster<T5>::ConvertOut(t5), InCaster<T6>::ConvertOut(t6))));
            }
        }
    private:
        void* callPt;
        wn_int32 mVirtualIndex;
        WNScriptType mReturnType;
    };
}

#define DEFINE_TYPED_SCRIPT_CHILD(_name, _parent) \
    namespace WNScripting {\
        struct ScriptTypePT_##_name; \
        typedef  WNScriptingObjectRef<ScriptTypePT_##_name*, ScriptType_##_parent>* ScriptType_##_name##_ref; \
        typedef  WNScriptingObject<ScriptTypePT_##_name*, ScriptType_##_parent> ScriptType_##_name; \
        template<> \
        struct TypeMapping<ScriptType_##_name> { \
            static const wn_bool RawOutputType = wn_true; \
            static const wn_bool RawInputType = wn_false; \
            static const wn_char * GetTypeName() { return #_name; } \
            static WNScriptType GetModifiedType(WNScriptType t, WNScriptingEngine*){return t; } \
        }; \
        template<> \
        struct TypeMapping<ScriptType_##_name##_ref> { \
            static const wn_bool RawInputType = wn_true; \
            static const wn_bool RawOutputType = wn_false; \
            static const wn_char * GetTypeName() { return #_name; } \
            static WNScriptType GetModifiedType(WNScriptType t, WNScriptingEngine*){return t; } \
        }; \
        template<> \
        struct InCaster<ScriptType_##_name> { \
            typedef void* OutType; \
            static OutType ConvertOut(const ScriptType_##_name& tVal) { \
                return(tVal.refData); \
            } \
        }; \
        template<>\
        struct OutCaster<ScriptType_##_name> { \
            typedef void* InType;\
            static ScriptType_##_name ConvertOut(void* tVal) {\
                return(ScriptType_##_name(tVal));\
            }\
        };\
    }

#define DEFINE_TYPED_SCRIPT_OBJECT(_name) \
    namespace WNScripting {\
        struct ScriptTypePT_##_name; \
        typedef  WNScriptingObjectRef<ScriptTypePT_##_name>* ScriptType_##_name##_ref; \
        typedef  WNScriptingObject<ScriptTypePT_##_name*> ScriptType_##_name; \
        template<> \
        struct TypeMapping<ScriptType_##_name> { \
            static const wn_bool RawInputType = wn_false; \
            static const wn_bool RawOutputType = wn_true; \
            static const wn_char * GetTypeName() { return #_name; } \
            static WNScriptType GetModifiedType(WNScriptType t, WNScriptingEngine*){return t; } \
        }; \
        template<> \
        struct TypeMapping<ScriptType_##_name##_ref> { \
            static const wn_bool RawInputType = wn_true; \
            static const wn_bool RawOutputType = wn_false; \
            static const wn_char * GetTypeName() { return #_name; } \
            static WNScriptType GetModifiedType(WNScriptType t, WNScriptingEngine*){return t; } \
        }; \
        template<> \
        struct InCaster<ScriptType_##_name> { \
            typedef void* OutType; \
            static OutType ConvertOut(const ScriptType_##_name& tVal) { \
                return(tVal.refData); \
            } \
        }; \
        template<>\
        struct OutCaster<ScriptType_##_name> { \
            typedef void* InType;\
            static ScriptType_##_name ConvertOut(void* tVal) {\
                return(ScriptType_##_name(tVal));\
            }\
        };\
    }

#define DEFINE_CPP_TYPE(_type) \
    namespace WNScripting {\
        typedef ScriptPointerRef<_type> ScriptPointer_##_type##_ref; \
        typedef ScriptPointer<_type> ScriptPointer_##_type;  \
        template<> \
        struct TypeMapping<_type> { \
            static const wn_bool RawInputType = wn_false; \
            static const wn_bool RawOutputType = wn_false; \
            static const wn_char * GetTypeName() { return #_type; } \
            static WNScriptType GetModifiedType(WNScriptType t, WNScriptingEngine*){return t; } \
        };\
        template<> \
        struct TypeMapping<ScriptPointer_##_type> { \
            static const wn_bool RawInputType = wn_false; \
            static const wn_bool RawOutputType = wn_true; \
            static const wn_char * GetTypeName() { return #_type; } \
            static WNScriptType GetModifiedType(WNScriptType t, WNScriptingEngine*){return t; } \
        };\
        template<> \
            struct TypeMapping<ScriptPointer_##_type##_ref> { \
            static const wn_bool RawInputType = wn_true; \
            static const wn_bool RawOutputType = wn_false; \
            static const wn_char * GetTypeName() { return #_type; } \
            static WNScriptType GetModifiedType(WNScriptType t, WNScriptingEngine*){return t; } \
        };\
        template<> \
        struct InCaster<ScriptPointer_##_type> { \
            typedef void* OutType; \
            static OutType ConvertOut(const ScriptPointer_##_type& tVal) { \
                return(tVal.refData); \
            } \
        }; \
        template<>\
        struct OutCaster<ScriptPointer_##_type> { \
            typedef void* InType;\
            static ScriptPointer_##_type ConvertOut(void* tVal) {\
                return(ScriptPointer_##_type(reinterpret_cast<StructInternalType*>(tVal)));\
            }\
        };\
    }

#define DEFINE_CPP_TYPE_CHILD(_type, _parent) \
    namespace WNScripting {\
        typedef ScriptPointerRef<_type, ScriptPointer_##_parent> ScriptPointer_##_type##_ref; \
        typedef ScriptPointer<_type, ScriptPointer_##_parent> ScriptPointer_##_type;  \
        template<> \
        struct TypeMapping<_type> { \
            static const wn_bool RawInputType = wn_false; \
            static const wn_bool RawOutputType = wn_false; \
            static const wn_char * GetTypeName() { return #_type; } \
            static WNScriptType GetModifiedType(WNScriptType t, WNScriptingEngine*){return t; } \
        };\
        template<> \
        struct TypeMapping<ScriptPointer_##_type> { \
            static const wn_bool RawInputType = wn_false; \
            static const wn_bool RawOutputType = wn_true; \
            static const wn_char * GetTypeName() { return #_type; } \
            static WNScriptType GetModifiedType(WNScriptType t, WNScriptingEngine*){return t; } \
        };\
        template<> \
        struct TypeMapping<ScriptPointer_##_type##_ref> { \
            static const wn_bool RawInputType = wn_true; \
            static const wn_bool RawOutputType = wn_false; \
            static const wn_char * GetTypeName() { return #_type; } \
            static WNScriptType GetModifiedType(WNScriptType t, WNScriptingEngine*){return t; } \
        };\
        template<> \
        struct InCaster<ScriptPointer_##_type> { \
            typedef void* OutType; \
            static OutType ConvertOut(const ScriptPointer_##_type& tVal) { \
                return(tVal.refData); \
            } \
        }; \
        template<>\
        struct OutCaster<ScriptPointer_##_type> { \
            typedef void* InType;\
            static ScriptPointer_##_type ConvertOut(void* tVal) {\
                return(ScriptPointer_##_type(reinterpret_cast<StructInternalType*>(tVal)));\
            }\
        };\
    }
#endif
