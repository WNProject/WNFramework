////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
    WN_INLINE WN_VOID FreeStruct(WN_VOID* str) {
        StructInternalType* sIp = *reinterpret_cast<StructInternalType**>(str);
        if(sIp) {
            if(sIp->owner == str && 0 != sIp->structLoc) {
                (**(destrType**)(sIp->structLoc))(sIp);
                free(sIp->structLoc);
            }
            if(0 == WNConcurrency::WNAtomicDecrement(reinterpret_cast<WN_ATOM_T*>(&sIp->refCount)))
            {
                free(sIp);
            }
        }
    }

    WN_INLINE WN_VOID FreeArray(WN_VOID* _array) {
        StructInternalType* sIp = *reinterpret_cast<StructInternalType**>(_array);
        if(sIp) {
            if(sIp->owner == _array && 0 != sIp->structLoc) {
                WN_SIZE_T curSize = *(reinterpret_cast<WN_SIZE_T*>(sIp->structLoc));
                WNScriptType type = *reinterpret_cast<WNScriptType*>((reinterpret_cast<WN_SIZE_T>(sIp->structLoc) + sizeof(WN_SIZE_T)));
                WNScriptType nType = type->mArrayType;
                if(nType->mStoredVTable || nType->mArrayType) {
        
                    if(nType->mStoredVTable) {
                        for(WN_SIZE_T i = 0; i < curSize; ++i) {
                            FreeStruct(reinterpret_cast<WN_CHAR*>(sIp->structLoc) + sizeof(WN_SIZE_T) * 2 + i * nType->mTypeSize);
                        }
                    } else {
                        for(WN_SIZE_T i = 0; i < curSize; ++i) {
                            FreeArray(reinterpret_cast<WN_CHAR*>(sIp->structLoc) + sizeof(WN_SIZE_T) * 2 + i * nType->mTypeSize);
                        }
                    }
                }
                free(sIp->structLoc);
            }
            if(0 == WNConcurrency::WNAtomicDecrement(reinterpret_cast<WN_ATOM_T*>(&sIp->refCount)))
            {
                free(sIp);
            }
        }
    }

    WN_INLINE WN_VOID Resize(WN_VOID* _array, WN_SIZE_T _arraySize) {
        StructInternalType* sIp = reinterpret_cast<StructInternalType*>(_array);
        if(sIp == WN_NULL) {
            return;
        }
        if(sIp->structLoc == WN_NULL) {
            return;
        }
        WN_SIZE_T curSize = *(reinterpret_cast<WN_SIZE_T*>(sIp->structLoc));
        WNScriptType type = *reinterpret_cast<WNScriptType*>((reinterpret_cast<WN_SIZE_T>(sIp->structLoc) + sizeof(WN_SIZE_T)));
        WNScriptType nType = type->mArrayType;
        if(nType->mStoredVTable || nType->mArrayType) {
        
            if(nType->mStoredVTable) {
                for(WN_SIZE_T i = _arraySize; i < curSize; ++i) {
                    FreeStruct(reinterpret_cast<WN_CHAR*>(sIp->structLoc) + sizeof(WN_SIZE_T) * 2 + i * nType->mTypeSize);
                }
            } else {
                for(WN_SIZE_T i = _arraySize; i < curSize; ++i) {
                    FreeArray(reinterpret_cast<WN_CHAR*>(sIp->structLoc) + sizeof(WN_SIZE_T) * 2 + i * nType->mTypeSize);
                }
            }
            for(WN_SIZE_T i = 0; i < ((curSize < _arraySize)? curSize : _arraySize); ++i) {
                StructInternalType** sp = reinterpret_cast<StructInternalType**>(reinterpret_cast<WN_CHAR*>(sIp->structLoc) + sizeof(WN_SIZE_T) * 2 + i * nType->mTypeSize);
                if((*sp)->owner == sp) {
                    (*sp)->owner = 0;
                }
            }
            sIp->structLoc = WNMemory::WNRealloc(sIp->structLoc, nType->mTypeSize*_arraySize + sizeof(WN_SIZE_T) * 2);
            for(WN_SIZE_T i = 0; i < ((curSize < _arraySize)? curSize : _arraySize); ++i) {
                StructInternalType** sp = reinterpret_cast<StructInternalType**>(reinterpret_cast<WN_CHAR*>(sIp->structLoc) + sizeof(WN_SIZE_T) * 2 + i * nType->mTypeSize);
                if((*sp)->owner == 0) {
                    (*sp)->owner = sp;
                }
            }
            for(WN_SIZE_T i = curSize; i < _arraySize; ++i) {
                StructInternalType* sp = reinterpret_cast<StructInternalType*>(malloc(sizeof(StructInternalType)));
                *reinterpret_cast<StructInternalType**>(reinterpret_cast<WN_CHAR*>(sIp->structLoc) + sizeof(WN_SIZE_T) * 2 + i * nType->mTypeSize) = sp;
                sp->owner = reinterpret_cast<StructInternalType**>(reinterpret_cast<WN_CHAR*>(sIp->structLoc) + sizeof(WN_SIZE_T) * 2 + i * nType->mTypeSize);
                sp->refCount = 1;
                sp->structLoc = WN_NULL;
            }
        } else {
            sIp->structLoc = WNMemory::WNRealloc(sIp->structLoc, nType->mTypeSize*_arraySize + sizeof(WN_SIZE_T) * 2);
            if(_arraySize > curSize) {
                WNMemory::WNMemSet(reinterpret_cast<WN_CHAR*>(sIp->structLoc) + sizeof(WN_SIZE_T) * 2 + curSize * nType->mTypeSize, 0x00, (_arraySize - curSize) * nType->mTypeSize);
            }
        }
        *(reinterpret_cast<WN_SIZE_T*>(sIp->structLoc)) = _arraySize;
    }

    class WNScriptingEngine;

    template<typename T, typename PT>
    class WNMemberVariablePointer;

   template<typename T, typename T_Parent = DummyType<0>>
    class WNScriptingObject {
    public:
        typedef T_Parent ParentType;
        WNScriptingObject() : refData(WN_NULL) {
        }
        WNScriptingObject(const WNScriptingObject<T, T_Parent>& _other) {
            refData = _other.refData;
            WNConcurrency::WNAtomicIncrement(reinterpret_cast<WN_ATOM_T*>(&refData->refCount));
        }
        WNScriptingObject(WNScriptingObject<T, T_Parent>&& _other) {
            refData = _other.refData;
            _other.refData = WN_NULL;
        }
        ~WNScriptingObject() {
            Destroy();
        }

        WNScriptingObject<T, T_Parent>& operator=(WNScriptingObject<T, T_Parent>&& _other) {
            Destroy();
            refData = _other.refData;
            if(refData->owner == &_other) {
                refData->owner = this;
            }
            _other.refData = WN_NULL;
            return(*this);
        }
        
        WNScriptingObject<T, T_Parent>& operator=(WNScriptingObject<T, T_Parent>& _other) {
            Destroy();
            refData = _other.refData;
            WNConcurrency::WNAtomicIncrement(reinterpret_cast<WN_ATOM_T*>(&refData->refCount));
            return(*this);
        }
        void Own(WNScriptingObject<T, T_Parent>& _other) {
            Destroy();
            refData = _other.refData;
            WNConcurrency::WNAtomicIncrement(reinterpret_cast<WN_ATOM_T*>(&refData->refCount));
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

        WNScriptingObject(void* _ptr) {
            refData = reinterpret_cast<StructInternalType*>(_ptr);
            if(refData->owner == WN_NULL) {
                refData->owner = this;
            }
            WNConcurrency::WNAtomicIncrement(reinterpret_cast<WN_ATOM_T*>(&refData->refCount));
        }
        
        WN_VOID Destroy() {
            if(refData) {
                if(refData->owner == this) {
                    (**(destrType**)(refData->structLoc))(refData);
                    free(refData->structLoc);
                    refData->structLoc = 0;
                }
                if(refData) {
                    if(0 == WNConcurrency::WNAtomicDecrement(reinterpret_cast<WN_ATOM_T*>(&refData->refCount)))
                    {
                        free(refData);
                    }
                }
                refData = WN_NULL;
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
            void* v =   reinterpret_cast<void*>(reinterpret_cast<WN_SIZE_T>(x.refData->structLoc) + mOffset);
            return(*reinterpret_cast<T*>(v));
        }
        WNMemberVariablePointer(WN_CHAR* _name) {
            mVariableName = WNStrings::WNStrNDup(_name, 1024);
        }
        WN_VOID SetEngine(WNScriptingEngine* _engine) {
            mEngine = _engine;
        }
        WNMemberVariablePointer(WN_CHAR* _name, WNScriptingEngine* _engine) : mOffset(-1) {
            mEngine = _engine;
            mVariableName = WNStrings::WNStrNDup(_name, 1024);
            mResolution = eUnresolved;
            TryToResolve();
        }
    private:
        WN_VOID TryToResolve() {
            if(mOffset >= 0 || mVariableName == WN_NULL || mEngine == WN_NULL){
                return;
            }
            if(mVariableName) {
                WNScriptType type = mEngine->GetTypeByName(TypeMapping<PT>::GetTypeName());

                if(type != 0) {
                    std::vector<WNContainedStructType>::iterator i = std::find_if(type->mStructTypes.begin(), type->mStructTypes.end(), 
                        [this](const WNContainedStructType& sType) { return(WNStrings::WNStrCmp(mVariableName, sType.mName) == 0); }
                    );
                    WNMemory::WNFreeT(mVariableName);
                    mVariableName = WN_NULL;
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
        WN_INT32 mOffset;
        WN_CHAR* mVariableName;
        WNScriptingEngine* mEngine;
        eResolutionState mResolution;
        template<typename TP, typename TS>
        friend class WNScriptingObject;
    };

    template<typename T>
    class WNScriptingArray {
    public:
        WNScriptingArray() : refData(WN_NULL) {
        }
        WNScriptingArray(const WNScriptingArray <T>& _other) {
            refData = _other.refData;
            WNConcurrency::WNAtomicIncrement(reinterpret_cast<WN_ATOM_T*>(&refData->refCount));
        }
        WNScriptingArray(WNScriptingArray <T>&& _other) {
            refData = _other.refData;
            _other.refData = WN_NULL;
        }
        ~WNScriptingArray() {
            Destroy();
        }
        WN_VOID Resize(WN_SIZE_T _newLength) {
            WNScripting::Resize(refData, _newLength);
        }
        WNScriptingArray<T>& operator=(WNScriptingArray<T>&& _other) {
            Destroy();
            refData = _other.refData;
            if(refData->owner == &_other) {
                refData->owner = this;
            }
            _other.refData = WN_NULL;
            return(*this);
        }
        
        WNScriptingArray<T>& operator=(WNScriptingArray<T>& _other) {
            Destroy();
            refData = _other.refData;
            WNConcurrency::WNAtomicIncrement(reinterpret_cast<WN_ATOM_T*>(&refData->refCount));
            return(*this);
        }
        void Own(WNScriptingArray<T>& _other) {
            Destroy();
            refData = _other.refData;
            WNConcurrency::WNAtomicIncrement(reinterpret_cast<WN_ATOM_T*>(&refData->refCount));
            refData->owner = this;
        }

        WNScriptingArray(void* _ptr) {
            refData = reinterpret_cast<StructInternalType*>(_ptr);
            if(refData->owner == WN_NULL) {
                refData->owner = this;
            }
            WNConcurrency::WNAtomicIncrement(reinterpret_cast<WN_ATOM_T*>(&refData->refCount));
        }
        
        T& operator[](WN_SIZE_T i) {
            void* v = reinterpret_cast<void*>(reinterpret_cast<WN_SIZE_T>(refData->structLoc) + 2 * sizeof(WN_SIZE_T) + (i * sizeof(T)));
            return(*reinterpret_cast<T*>(v));
        }
        WN_SIZE_T GetLength() {
            if(refData) {
                if(refData->structLoc) {
                    return(*reinterpret_cast<WN_SIZE_T*>(refData->structLoc));
                }
            }
            return(0);
        }
        WN_VOID Destroy() {
            if(refData) {
                if(refData->owner == this) {
                    WN_SIZE_T length = *reinterpret_cast<WN_SIZE_T*>(refData->structLoc);
                    if(length > 0) {
                        T* startT = reinterpret_cast<T*>(reinterpret_cast<WN_SIZE_T>(refData->structLoc) + sizeof(WN_SIZE_T));
                        for(WN_SIZE_T i = 0; i < length; ++i) {
                            startT[i].~T();
                        }
                    }
                    free(refData->structLoc);
                    refData->structLoc = 0;
                }
                if(refData) {
                    if(0 == WNConcurrency::WNAtomicDecrement(reinterpret_cast<WN_ATOM_T*>(&refData->refCount)))
                    {
                        free(refData);
                    }
                }
                refData = WN_NULL;
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
        static const WN_CHAR * GetTypeName() { return TypeMapping<T>::GetTypeName(); }
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
            refData->structLoc = WN_NULL;
            refData->owner = this;
        }
        ScriptPointer(StructInternalType* _ptr) {
            refData = _ptr;
        }
        ScriptPointer(const ScriptPointer<T, T_Parent>& _other) {
            refData = _other.refData;
            WNConcurrency::WNAtomicIncrement(reinterpret_cast<WN_ATOM_T*>(&refData->refCount));
        }
        ScriptPointer(ScriptPointer<T, T_Parent>&& _other) {
            refData = _other.refData;
            _other.refData = WN_NULL;
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
                refData->structLoc = WN_NULL;
                refData->owner = this;
                if(0 == WNConcurrency::WNAtomicDecrement(reinterpret_cast<WN_ATOM_T*>(&refData->refCount)))
                {
                    free(refData);
                }
                refData = NULL;
                return(tDat);
            }
            return(WN_NULL);
        }

        operator T*() {
            return(reinterpret_cast<T*>(refData->structLoc));
        }

        T* operator->() {
            return(reinterpret_cast<T*>(refData->structLoc));
        }

        WN_VOID Destroy() {
            if(refData) {
                if(refData->owner == this && refData->structLoc) {
                    reinterpret_cast<T*>(refData->structLoc)->~T();
                    free(refData->structLoc);
                    refData->structLoc = 0;
                }
                if(refData) {
                    if(0 == WNConcurrency::WNAtomicDecrement(reinterpret_cast<WN_ATOM_T*>(&refData->refCount)))
                    {
                        free(refData);
                    }
                }
                refData = WN_NULL;
            }
        }
        ScriptPointer<T, T_Parent>& operator=(ScriptPointer<T, T_Parent>&& _other) {
            Destroy();
            refData = _other.refData;
            if(refData->owner == &_other || refData->owner == WN_NULL) {
                refData->owner = this;
            }
            _other.refData = WN_NULL;
            return(*this);
        }
        
        ScriptPointer<T, T_Parent>& operator=(ScriptPointer<T, T_Parent>& _other) {
            Destroy();
            refData = _other.refData;
            if(refData->owner == WN_NULL) {
                refData->owner = this;
            }
            WNConcurrency::WNAtomicIncrement(reinterpret_cast<WN_ATOM_T*>(&refData->refCount));
            return(*this);
        }
        void Own(ScriptPointer<T, T_Parent>& _other) {
            Destroy();
            refData = _other.refData;
            WNConcurrency::WNAtomicIncrement(reinterpret_cast<WN_ATOM_T*>(&refData->refCount));
            refData->owner = this;
        }
        
        bool operator !() {
            return(refData == WN_NULL || refData->structLoc == WN_NULL);
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
            static const WN_CHAR * GetTypeName() { return #_name; } \
            static WNScriptType GetModifiedType(WNScriptType t, WNScriptingEngine*){return t; } \
        }; \
        } 

DEFINE_TEMPLATE_MAPPING(WN_INT32, Int);
DEFINE_TEMPLATE_MAPPING(WN_FLOAT32, Float);
DEFINE_TEMPLATE_MAPPING(WN_CHAR, Char);
DEFINE_TEMPLATE_MAPPING(WN_BOOL, Bool);
DEFINE_TEMPLATE_MAPPING(WN_VOID, Void);
DEFINE_TEMPLATE_MAPPING(WN_VOID*, -Ptr);
DEFINE_TEMPLATE_MAPPING(WN_SIZE_T, -SizeT);

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
    struct OutCaster<WN_VOID> {
        typedef WN_VOID* InType;
        static const WN_VOID ConvertOut(const InType&) {
            return;
        }
    };

    template<typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
    class WNFunctionPointer {
    public:
        WNFunctionPointer() :mReturnType(WN_NULL), callPt(WN_NULL){
        }
        WN_VOID Assign(R (*_callPt)()) {
            WN_STATIC_ASSERT(IsDummy<T1>::val == 1);
            callPt = reinterpret_cast<void*>(_callPt);
        }
        WN_VOID Assign(R (*_callPt)(T1)) {
            callPt = reinterpret_cast<void*>(_callPt);
        }
        WN_VOID Assign(R (*_callPt)(T1, T2)) {
            callPt = reinterpret_cast<void*>(_callPt);
        }
        WN_VOID Assign(R (*_callPt)(T1, T2, T3)) {
            callPt = reinterpret_cast<void*>(_callPt);
        }
        WN_VOID Assign(R (*_callPt)(T1, T2, T3, T4)) {
            callPt = reinterpret_cast<void*>(_callPt);
        }
        WN_VOID Assign(R (*_callPt)(T1, T2, T3, T4, T5)) {
            callPt = reinterpret_cast<void*>(_callPt);
        }
        WN_VOID Assign(R (*_callPt)(T1, T2, T3, T4, T5, T6)) {
            WN_STATIC_ASSERT(IsDummy<T6>::val == 0);
            callPt = reinterpret_cast<void*>(_callPt);
        }

        WN_VOID SetOutType(WNScriptType _type) {
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
        WNMemberFunctionPointer() :mReturnType(WN_NULL), callPt(WN_NULL), mVirtualIndex(-1){
        }

        WN_VOID Assign(WN_INT32 _virtualIndex) {
            mVirtualIndex = _virtualIndex;    
        }

        WN_VOID Assign(R (*_callPt)(T)) {
            WN_STATIC_ASSERT(IsDummy<T1>::val == 1);
            callPt = reinterpret_cast<void*>(_callPt);
        }
        WN_VOID Assign(R (*_callPt)(T, T1)) {
            WN_STATIC_ASSERT(IsDummy<T1>::val == 0 && IsDummy<T2>::val == 1);
            callPt = reinterpret_cast<void*>(_callPt);
        }
        WN_VOID Assign(R (*_callPt)(T, T1, T2)) {
            WN_STATIC_ASSERT(IsDummy<T2>::val == 0 && IsDummy<T3>::val == 1);
            callPt = reinterpret_cast<void*>(_callPt);
        }
        WN_VOID Assign(R (*_callPt)(T, T1, T2, T3)) {
            WN_STATIC_ASSERT(IsDummy<T3>::val == 0 && IsDummy<T4>::val == 1);
            callPt = reinterpret_cast<void*>(_callPt);
        }
        WN_VOID Assign(R (*_callPt)(T, T1, T2, T3, T4)) {
            WN_STATIC_ASSERT(IsDummy<T4>::val == 0 && IsDummy<T5>::val == 1);
            callPt = reinterpret_cast<void*>(_callPt);
        }
        WN_VOID Assign(R (*_callPt)(T, T1, T2, T3, T4, T5)) {
            WN_STATIC_ASSERT(IsDummy<T5>::val == 0 && IsDummy<T6>::val == 1);
            callPt = reinterpret_cast<void*>(_callPt);
        }
        WN_VOID Assign(R (*_callPt)(T, T1, T2, T3, T4, T5, T6)) {
            WN_STATIC_ASSERT(IsDummy<T6>::val == 0);
            callPt = reinterpret_cast<void*>(_callPt);
        }

        WN_VOID SetOutType(WNScriptType _type) {
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
                    WN_RELEASE_ASSERT(eWNOK == def->mScriptingEngine->GetFunctionPointer(def->mContainedFile->mFileName, def->mName, def->mReturn, def->mTypes, pt));
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
                    WN_RELEASE_ASSERT(eWNOK == def->mScriptingEngine->GetFunctionPointer(def->mContainedFile->mFileName, def->mName, def->mReturn, def->mTypes, pt));
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
                    WN_RELEASE_ASSERT(eWNOK == def->mScriptingEngine->GetFunctionPointer(def->mContainedFile->mFileName, def->mName, def->mReturn, def->mTypes, pt));
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
                    WN_RELEASE_ASSERT(eWNOK == def->mScriptingEngine->GetFunctionPointer(def->mContainedFile->mFileName, def->mName, def->mReturn, def->mTypes, pt));
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
                    WN_RELEASE_ASSERT(eWNOK == def->mScriptingEngine->GetFunctionPointer(def->mContainedFile->mFileName, def->mName, def->mReturn, def->mTypes, pt));
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
                    WN_RELEASE_ASSERT(eWNOK == def->mScriptingEngine->GetFunctionPointer(def->mContainedFile->mFileName, def->mName, def->mReturn, def->mTypes, pt));
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
                    WN_RELEASE_ASSERT(eWNOK == def->mScriptingEngine->GetFunctionPointer(def->mContainedFile->mFileName, def->mName, def->mReturn, def->mTypes, pt));
                    (*(void***)(m->structLoc))[2 + 2*mVirtualIndex] = pt;
                }
                return(OutCaster<R>::ConvertOut(reinterpret_cast<typename OutCaster<R>::InType (*)(typename InCaster<T>::OutType, typename InCaster<T1>::OutType, typename InCaster<T2>::OutType, typename InCaster<T3>::OutType, typename InCaster<T4>::OutType, typename InCaster<T5>::OutType, typename InCaster<T6>::OutType)>(pt)(InCaster<T>::ConvertOut(t), InCaster<T1>::ConvertOut(t1), InCaster<T2>::ConvertOut(t2), InCaster<T3>::ConvertOut(t3), InCaster<T4>::ConvertOut(t4), InCaster<T5>::ConvertOut(t5), InCaster<T6>::ConvertOut(t6))));
            }
        }
    private:
        void* callPt;
        WN_INT32 mVirtualIndex;
        WNScriptType mReturnType;
    };
}

#define DEFINE_TYPED_SCRIPT_CHILD(_name, _parent) \
    namespace WNScripting {\
        struct ScriptTypePT_##_name; \
        typedef  WNScriptingObject<ScriptTypePT_##_name*, ScriptType_##_parent> ScriptType_##_name; \
        template<> \
        struct TypeMapping<ScriptType_##_name> { \
        static const WN_CHAR * GetTypeName() { return #_name; } \
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
        typedef  WNScriptingObject<ScriptTypePT_##_name*> ScriptType_##_name; \
        template<> \
        struct TypeMapping<ScriptType_##_name> { \
        static const WN_CHAR * GetTypeName() { return #_name; } \
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
        typedef ScriptPointer<_type> ScriptPointer_##_type;  \
        template<> \
        struct TypeMapping<_type> { \
            static const WN_CHAR * GetTypeName() { return #_type; } \
            static WNScriptType GetModifiedType(WNScriptType t, WNScriptingEngine*){return t; } \
        };\
        template<> \
            struct TypeMapping<ScriptPointer_##_type> { \
            static const WN_CHAR * GetTypeName() { return #_type; } \
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
        typedef ScriptPointer<_type, ScriptPointer_##_parent> ScriptPointer_##_type;  \
        template<> \
        struct TypeMapping<_type> { \
            static const WN_CHAR * GetTypeName() { return #_type; } \
            static WNScriptType GetModifiedType(WNScriptType t, WNScriptingEngine*){return t; } \
        };\
        template<> \
            struct TypeMapping<ScriptPointer_##_type> { \
            static const WN_CHAR * GetTypeName() { return #_type; } \
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
