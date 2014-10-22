#include "WNCore/inc/WNTypes.h"
#include "WNScripting/inc/WNTypeManager.h"
#include "WNScripting/inc/WNScriptingInterop.h"

struct GenerateStringIDOperator: public WNScripting::GenerateIDAccessOperation {
    GenerateStringIDOperator(WNScripting::WNScriptType _scriptType, WNScripting::WNScriptType _intType, WNScripting::WNScriptType _functionType): 
        mArrayOperator(_scriptType, _intType, _functionType),
        mScriptType(_scriptType),
        mIntType(_intType),
        mFunctionType(_functionType) {
    }
    
    virtual eWNTypeError Execute(llvm::IRBuilderBase* _builder, llvm::Value* _expr1, llvm::Value* _expr2, WN_CHAR* _id,  WNScripting::WNScriptType &_outType, llvm::Value*& _outValue, llvm::Value*& _outLocation) const {
        return(mArrayOperator.Execute(_builder, _expr1, _expr2, _id, _outType, _outValue, _outLocation)); 
    }
private:
    WNScripting::WNScriptType mScriptType;
    WNScripting::WNScriptType mIntType;
    WNScripting::WNScriptType mFunctionType;
    WNScripting::GenerateArrayIDOperator mArrayOperator;
};

WN_VOID AppendString(WNScripting::WNScriptingArrayRef<WN_CHAR>* _inRef, WNScripting::WNScriptingArrayRef<WN_CHAR>* _toAppend) {
    WNScripting::WNScriptingArray<WN_CHAR> inArray(_inRef);
    WNScripting::WNScriptingArray<WN_CHAR> appendArray(_toAppend);
    if(!inArray|| !appendArray) {
        return;
    }
    inArray.Resize(inArray.GetLength() + appendArray.GetLength());
    memcpy(&inArray[inArray.GetLength()], &appendArray[0], appendArray.GetLength());
}

WNScripting::WNScriptingArrayRef<WNScripting::WNScriptingArray<WN_CHAR> >* Split(WNScripting::WNScriptingArrayRef<WN_CHAR>* _string, WN_CHAR _split, WN_BOOL _ignoreEmpty) {
    WNScripting::WNScriptingArray<WN_CHAR> string(_string);
    if(!string) {
        return(WN_NULL);
    }
    WNScripting::WNScriptType _arrayArrayType =  string.GetType()->mScriptingEngine->GetExistingArrayOf(string.GetType());
    WNScripting::WNScriptType _stringType = string.GetType();
    const WNScripting::WNScriptingEngine* engine = string.GetType()->mScriptingEngine;
    if(!_arrayArrayType) {
        return(WN_NULL);
    }
    WN_SIGNED_T splitCount= 0;
    WN_SIGNED_T index = 0;
    WN_SIGNED_T previousIndex = -1;
    WN_SIGNED_T strLen = string.GetLength();
    for(WN_SIGNED_T i = 0; i < strLen; ++i){
        if(string[i] == _split) {
            if((i - previousIndex) > 1 || !_ignoreEmpty){
                splitCount += 1;
            }
            previousIndex = i;
        }
    }
    if(previousIndex < strLen - 1 || !_ignoreEmpty) {
        splitCount += 1;
    }
    previousIndex = -1;
    //now we have the number of splits we need time to allocate that array
    WN_VOID* outPtr;
    engine->ConstructScriptingArray(_arrayArrayType, splitCount, outPtr);
    WNScripting::WNScriptingArray<WNScripting::WNScriptingArray<WN_CHAR> > newArray(outPtr);

    for(WN_SIGNED_T i = 0; i < strLen; ++i){
        if(string[i] == _split) {
            if((i - previousIndex) > 1 || !_ignoreEmpty){
                WN_SIGNED_T len = (i - previousIndex - 1);
                WN_VOID* dat;
                engine->ConstructScriptingArray(_stringType, len, dat);
                newArray[index] = WNScripting::WNScriptingArray<WN_CHAR>(dat);
                if(len > 0) {
                    memcpy(&newArray[index][0], &string[previousIndex + 1], len);
                }
                index += 1;
            }
            previousIndex = i;
        }
    }
    
    if(previousIndex < strLen - 1 || !_ignoreEmpty) {
        WN_SIZE_T len = (strLen - previousIndex - 1);
        WN_VOID* dat;
        engine->ConstructScriptingArray(_stringType, len, dat);
        newArray[index] = WNScripting::WNScriptingArray<WN_CHAR>(dat);
        if(len > 0) {
            memcpy(&newArray[index][0], &string[previousIndex + 1], len);
        }
    }
    return(newArray.DetachOut());
}

WNScripting::WNScriptingArrayRef<WNScripting::WNScriptingArray<WN_CHAR> >* SplitFrom(WNScripting::WNScriptingArrayRef<WN_CHAR>* _string, WNScripting::WNScriptingArrayRef<WN_CHAR>* _splitString, WN_BOOL _ignoreEmpty) {
    WNScripting::WNScriptingArray<WN_CHAR> string(_string);
    WNScripting::WNScriptingArray<WN_CHAR> splitString(_splitString);
    if(!string || !splitString) {
        return(WN_NULL);
    }
    WNScripting::WNScriptType _arrayArrayType =  string.GetType()->mScriptingEngine->GetExistingArrayOf(string.GetType());
    WNScripting::WNScriptType _stringType = string.GetType();
    const WNScripting::WNScriptingEngine* engine = string.GetType()->mScriptingEngine;
    if(!_arrayArrayType) {
        return(WN_NULL);
    }
    WN_SIGNED_T splitLength = splitString.GetLength();
    WN_SIGNED_T splitCount= 0;
    WN_SIGNED_T index = 0;
    WN_SIGNED_T previousIndex = -1;
    WN_SIGNED_T strLen = string.GetLength();
    for(WN_SIGNED_T i = 0; i < strLen; ++i){
        for(WN_SIGNED_T j = 0; j < splitLength; ++j) {
            if(string[i] == splitString[j]) {
                if((i - previousIndex) > 1 || !_ignoreEmpty){
                    splitCount += 1;
                }
                previousIndex = i;
                break;
            }
        }
    }
    if(previousIndex < strLen - 1 || !_ignoreEmpty) {
        splitCount += 1;
    }
    previousIndex = -1;
    //now we have the number of splits we need time to allocate that array
    WN_VOID* outPtr;
    engine->ConstructScriptingArray(_arrayArrayType, splitCount, outPtr);
    WNScripting::WNScriptingArray<WNScripting::WNScriptingArray<WN_CHAR> > newArray(outPtr);

    for(WN_SIGNED_T i = 0; i < strLen; ++i){
        for(WN_SIGNED_T j = 0; j < splitLength; ++j) {
            if(string[i] == splitString[j]) {
                if((i - previousIndex) > 1 || !_ignoreEmpty){
                    WN_SIGNED_T len = (i - previousIndex - 1);
                    WN_VOID* dat;
                    engine->ConstructScriptingArray(_stringType, len, dat);
                    newArray[index] = WNScripting::WNScriptingArray<WN_CHAR>(dat);
                    if(len > 0) {
                        memcpy(&newArray[index][0], &string[previousIndex + 1], len);
                    }
                    index += 1;
                }
                previousIndex = i;
                break;
            }
        }
    }
    
    if(previousIndex < strLen - 1 || !_ignoreEmpty) {
        WN_SIZE_T len = (strLen - previousIndex - 1);
        WN_VOID* dat;
        engine->ConstructScriptingArray(_stringType, len, dat);
        newArray[index] = WNScripting::WNScriptingArray<WN_CHAR>(dat);
        if(len > 0) {
            memcpy(&newArray[index][0], &string[previousIndex + 1], len);
        }
    }
    return(newArray.DetachOut());
}

WN_VOID ToLower(WNScripting::WNScriptingArrayRef<WN_CHAR>* _string) {
    WNScripting::WNScriptingArray<WN_CHAR> string(_string);
    if(!string) {
        return;
    }
    for(WN_SIZE_T i = 0; i < string.GetLength(); ++i) {
        string[i] = WNStrings::WNToLower(string[i]);
    }
    return;
}

WN_VOID ToUpper(WNScripting::WNScriptingArrayRef<WN_CHAR>* _string) {
    WNScripting::WNScriptingArray<WN_CHAR> string(_string);
    if(!string) {
        return;
    }
    for(WN_SIZE_T i = 0; i < string.GetLength(); ++i) {
        string[i] = WNStrings::WNToUpper(string[i]);
    }
    return;
}

WN_INT32 FindFirstIn(WNScripting::WNScriptingArrayRef<WN_CHAR>* _string, WNScripting::WNScriptingArrayRef<WN_CHAR>* _findee) {
    WNScripting::WNScriptingArray<WN_CHAR> string(_string);
    WNScripting::WNScriptingArray<WN_CHAR> findee(_findee);
    if(!string || !findee) {
        return(-1);
    }
    WN_SIZE_T searchLen = findee.GetLength();
    for(WN_SIZE_T i = 0; i < string.GetLength(); ++i) {
        for(WN_SIZE_T j = 0; j < searchLen; ++j) {
            if(string[i] == findee[j]) {
                return(i);
            }
        }
    }
    return(-1);
}

WN_INT32 FindFirstNotIn(WNScripting::WNScriptingArrayRef<WN_CHAR>* _string, WNScripting::WNScriptingArrayRef<WN_CHAR>* _findee) {
    WNScripting::WNScriptingArray<WN_CHAR> string(_string);
    WNScripting::WNScriptingArray<WN_CHAR> findee(_findee);
    if(!string || !findee) {
        return(-1);
    }
    WN_SIZE_T searchLen = findee.GetLength();
    for(WN_SIZE_T i = 0; i < string.GetLength(); ++i) {
        WN_SIZE_T j = 0;
        for(j = 0; j < searchLen; ++j) {
            if(string[i] == findee[j]) {
               break; 
            }
        }
        if(j == searchLen) {
            return(i);
        }
    }
    return(-1);
}

WN_INT32 FindFirstOf(WNScripting::WNScriptingArrayRef<WN_CHAR>* _string, WN_CHAR _char) {
    WNScripting::WNScriptingArray<WN_CHAR> string(_string);
    if(!string) {
        return(-1);
    }
    for(WN_SIZE_T i = 0; i < string.GetLength(); ++i) {
        if(string[i] == _char) {
            return(i);
        }
    }
    return(-1);
}

WN_INT32 FindLastIn(WNScripting::WNScriptingArrayRef<WN_CHAR>* _string, WNScripting::WNScriptingArrayRef<WN_CHAR>* _findee) {
    WNScripting::WNScriptingArray<WN_CHAR> string(_string);
    WNScripting::WNScriptingArray<WN_CHAR> findee(_findee);
    if(!string || !findee) {
        return(-1);
    }
    WN_INT32 searchLen = static_cast<WN_INT32>(findee.GetLength());
    for(WN_INT32 i = static_cast<WN_INT32>(string.GetLength()) - 1; i >= 0; --i) {
        for(WN_INT32 j = 0; j < searchLen; ++j) {
            if(string[i] == findee[j]) {
                return(i);
            }
        }
    }
    return(-1);
}

WN_INT32 FindLastNotIn(WNScripting::WNScriptingArrayRef<WN_CHAR>* _string, WNScripting::WNScriptingArrayRef<WN_CHAR>* _findee) {
    WNScripting::WNScriptingArray<WN_CHAR> string(_string);
    WNScripting::WNScriptingArray<WN_CHAR> findee(_findee);
    if(!string || !findee) {
        return(-1);
    }
    WN_SIZE_T searchLen = findee.GetLength();
    for(WN_SIZE_T i = string.GetLength() - 1; i >= 0; --i) {
        WN_SIZE_T j = 0;
        for(j = 0; j < searchLen; ++j) {
            if(string[i] == findee[j]) {
                break;
            }
        }
        if(j == searchLen) {
            return(i);
        }
    }
    return(-1);
}

WN_INT32 FindLastOf(WNScripting::WNScriptingArrayRef<WN_CHAR>* _string, WN_CHAR _char) {
    WNScripting::WNScriptingArray<WN_CHAR> string(_string);
    if(!string) {
        return(-1);
    }
    for(WN_INT32 i = static_cast<WN_INT32>(string.GetLength()) - 1; i >= 0; --i) {
        if(string[i] == _char) {
            return(i);
        }
    }
    return(-1);
}

WN_INT32 FindNextIn(WNScripting::WNScriptingArrayRef<WN_CHAR>* _string, WNScripting::WNScriptingArrayRef<WN_CHAR>* _findee, WN_INT32 index) {
    WNScripting::WNScriptingArray<WN_CHAR> string(_string);
    WNScripting::WNScriptingArray<WN_CHAR> findee(_findee);
    if(!string || !findee || index < 0) {
        return(-1);
    }
    WN_SIZE_T searchLen = findee.GetLength();
    for(WN_SIZE_T i = index; i < string.GetLength(); ++i) {
        for(WN_SIZE_T j = 0; j < searchLen; ++j) {
            if(string[i] == findee[j]) {
                return(i);
            }
        }
    }
    return(-1);
}

WN_INT32 FindNextNotIn(WNScripting::WNScriptingArrayRef<WN_CHAR>* _string, WNScripting::WNScriptingArrayRef<WN_CHAR>* _findee, WN_INT32 index) {
    WNScripting::WNScriptingArray<WN_CHAR> string(_string);
    WNScripting::WNScriptingArray<WN_CHAR> findee(_findee);
    if(!string || !findee || index < 0) {
        return(-1);
    }
    WN_SIZE_T searchLen = findee.GetLength();
    for(WN_SIZE_T i = index; i < string.GetLength(); ++i) {
        WN_SIZE_T j = 0;
        for(j = 0; j < searchLen; ++j) {
            if(string[i] == findee[j]) {
                break;
            }
        }
        if(j == searchLen) {
            return(i);
        }
    }
    return(-1);
}
WN_INT32 FindNextOf(WNScripting::WNScriptingArrayRef<WN_CHAR>* _string, WN_CHAR _char, WN_INT32 index) {
    WNScripting::WNScriptingArray<WN_CHAR> string(_string);
    if(!string || index < 0) {
        return(-1);
    }
    for(WN_SIZE_T i = index; i < string.GetLength(); ++i) {
        if(string[i] == _char) {
            return(i);
        }
    }
    return(-1);
}

WN_INT32 FindNextLastIn(WNScripting::WNScriptingArrayRef<WN_CHAR>* _string, WNScripting::WNScriptingArrayRef<WN_CHAR>* _findee, WN_INT32 index) {
    WNScripting::WNScriptingArray<WN_CHAR> string(_string);
    WNScripting::WNScriptingArray<WN_CHAR> findee(_findee);
    if(!string || !findee || static_cast<WN_SIZE_T>(index) > string.GetLength()) {
        return(-1);
    }
    WN_SIZE_T searchLen = findee.GetLength();
    for(WN_SIZE_T i = index; i >= 0; --i) {
        for(WN_SIZE_T j = 0; j < searchLen; ++j) {
            if(string[i] == findee[j]) {
                return(i);
            }
        }
    }
    return(-1);
}

WN_INT32 FindNextLastNotIn(WNScripting::WNScriptingArrayRef<WN_CHAR>* _string, WNScripting::WNScriptingArrayRef<WN_CHAR>* _findee, WN_INT32 index) {
    WNScripting::WNScriptingArray<WN_CHAR> string(_string);
    WNScripting::WNScriptingArray<WN_CHAR> findee(_findee);
    if(!string || !findee || static_cast<WN_SIZE_T>(index) > string.GetLength()) {
        return(-1);
    }
    WN_SIZE_T searchLen = findee.GetLength();
    for(WN_SIZE_T i = index; i >= 0; --i) {
        WN_SIZE_T j = 0;
        for(j = 0; j < searchLen; ++j) {
            if(string[i] == findee[j]) {
                break;
            }
        }
        if(j == searchLen) {
            return(i);
        }
    }
    return(-1);
}

WN_INT32 FindNextLastOf(WNScripting::WNScriptingArrayRef<WN_CHAR>* _string, WN_CHAR _char, WN_INT32 index) {
    WNScripting::WNScriptingArray<WN_CHAR> string(_string);
    if(!string || static_cast<WN_SIZE_T>(index) > string.GetLength()) {
        return(-1);
    }
    for(int i = index; i >= 0; --i) {
        if(string[i] == _char) {
            return(i);
        }
    }
    return(-1);
}

WN_INT32 FindString(WNScripting::WNScriptingArrayRef<WN_CHAR>* _string, WNScripting::WNScriptingArrayRef<WN_CHAR>* _substr) {
    WNScripting::WNScriptingArray<WN_CHAR> string(_string);
    WNScripting::WNScriptingArray<WN_CHAR> substr(_substr);
    if(!string || !substr) {
        return(-1);
    }

    for(WN_INT32 i = 0; i < static_cast<WN_INT32>(string.GetLength()) - static_cast<WN_INT32>(substr.GetLength()); ++i){
        if(WNStrings::WNStrNCmp(&string[i], &substr[0], substr.GetLength()) == 0) {
            return(i);
        }
    }

    return(-1);
}

WN_INT32 FindLastString(WNScripting::WNScriptingArrayRef<WN_CHAR>* _string, WNScripting::WNScriptingArrayRef<WN_CHAR>* _substr) {
    WNScripting::WNScriptingArray<WN_CHAR> string(_string);
    WNScripting::WNScriptingArray<WN_CHAR> substr(_substr);
    if(!string || !substr) {
        return(-1);
    }

    for(WN_INT32 i = static_cast<WN_INT32>(string.GetLength()) - static_cast<WN_INT32>(substr.GetLength()) - 1; i >= 0 ; --i){
        if(WNStrings::WNStrNCmp(&string[i], &substr[0], substr.GetLength()) == 0) {
            return(i);
        }
    }
    return(-1);
}

WNScripting::WNScriptingArrayRef<WN_CHAR>* ConcatenateStrings(WNScripting::WNScriptingArrayRef<WN_CHAR>* _string1, WNScripting::WNScriptingArrayRef<WN_CHAR>* _string2) {
    WNScripting::WNScriptingArray<WN_CHAR> string1(_string1);
    WNScripting::WNScriptingArray<WN_CHAR> string2(_string2);
    if(!string1 || !string2) {
        return(WN_NULL);
    }
    void* outPtr;
    string1.GetType()->mScriptingEngine->ConstructScriptingArray(string1.GetType(), string1.GetLength() + string2.GetLength(), outPtr);
    WNScripting::WNScriptingArray<WN_CHAR> outArray(outPtr);
    memcpy(&outArray[0], &string1[0], string1.GetLength());
    memcpy(&outArray[string1.GetLength()], &string2[0], string2.GetLength());
    return(outArray.DetachOut());
}

WNScripting::WNScriptingArrayRef<WN_CHAR>* CopyString(WNScripting::WNScriptingArrayRef<WN_CHAR>* _string1) {
    WNScripting::WNScriptingArray<WN_CHAR> string1(_string1);
    if(!string1) {
        return(WN_NULL);
    }
    void* outPtr;
    string1.GetType()->mScriptingEngine->ConstructScriptingArray(string1.GetType(), string1.GetLength(), outPtr);
    WNScripting::WNScriptingArray<WN_CHAR> outArray(outPtr);
    memcpy(&outArray[0], &string1[0], string1.GetLength());
    return(outArray.DetachOut());
}

eWNTypeError RegisterExternalString(WNScripting::WNScriptingEngine* _engine, WNScripting::WNTypeManager& _manager, void*) {
    WNScripting::WNScriptType _charArray;
    WNScripting::WNScriptType _char;
    WNScripting::WNScriptType _myType;
    WNScripting::WNScriptType _intType;
    WNScripting::WNScriptType _nullType;
    WNScripting::WNScriptType _boolType;
    WNScripting::WNScriptType _voidType;
    WNScripting::WNScriptType _functionType;
    WNScripting::WNScriptType _arrayOfArrayOfString;
    if(eWNOK != _manager.GetTypeByName("Char", _char)) {
        return(eWNError);
    }
    if(eWNOK != _manager.GetTypeByName("Int", _intType)) {
        return(eWNError);
    }
    if(eWNOK != _manager.GetTypeByName("-Null", _nullType)) {
        return(eWNError);
    }
    if(eWNOK != _manager.GetTypeByName("Bool", _boolType)) {
        return(eWNError);
    }
    if(eWNOK != _manager.GetTypeByName("-Function", _functionType)) {
        return(eWNError);
    }
    if(eWNOK != _manager.GetTypeByName("Void", _voidType)) {
        return(eWNError);
    }
    if(eWNOK != _manager.GetArrayOf(_char, _charArray)) {
        return(eWNError);
    }
    
    if(eWNOK != _manager.GetArrayOf(_charArray, _arrayOfArrayOfString)) { //We use this here so that is
                                                                          //is populated so we can get it through
                                                                          //a const ScriptingEngine later
        return(eWNError);
    }

    if(eWNOK != _manager.RegisterAliasedStruct("String", _charArray, _myType)) {
        return(eWNError);
    }
    
    _manager.RegisterCastingOperator(_charArray, _myType, WN_NEW WNScripting::GenerateReinterpretCastOperation(_myType));
    _manager.RegisterCastingOperator(_myType, _charArray, WN_NEW WNScripting::GenerateReinterpretCastOperation(_charArray));
    _manager.RegisterArrayAccessOperator(_myType, _intType, WN_NEW WNScripting::GenerateDefaultArrayAccessOperation(_myType));
    _manager.RegisterDestructionOperator(_myType, WN_NEW WNScripting::GenerateArrayDestruction());
    _manager.RegisterAllocationOperator(_myType, WN_NEW WNScripting::GenerateDefaultAllocation());
    _manager.RegisterAssignmentOperator(_myType, AT_EQ, WN_NEW WNScripting::GenerateStructAssignment());
    _manager.RegisterAssignmentOperator(_myType, AT_CHOWN, WN_NEW WNScripting::GenerateStructTransfer());
    _manager.RegisterCastingOperator(_nullType, _myType, WN_NEW WNScripting::GenerateReinterpretCastOperation(_myType));
    _manager.RegisterCastingOperator(_myType, _nullType, WN_NEW WNScripting::GenerateReinterpretCastOperation(_nullType));
    _manager.RegisterArithmeticOperator(AR_EQ, _myType, _myType, WN_NEW WNScripting::GenerateStructCompare(_boolType, WN_TRUE));
    _manager.RegisterArithmeticOperator(AR_NEQ, _myType, _myType, WN_NEW WNScripting::GenerateStructCompare(_boolType, WN_FALSE));
    _manager.RegisterIDAccessOperator(_myType, WN_NEW WNScripting::GenerateArrayIDOperator(_myType, _intType, _functionType));

    std::vector<WNScripting::WNScriptType> params;
    params.push_back(_charArray);
    _engine->RegisterFunction("$ToLower", _voidType, params, reinterpret_cast<void*>(&ToLower));
    _engine->RegisterFunction("$ToUpper", _voidType, params, reinterpret_cast<void*>(&ToUpper));
    params.push_back(_charArray);
    _engine->RegisterFunction("$AppendString", _voidType, params, reinterpret_cast<void*>(&AppendString));
    _engine->RegisterFunction("$ConcatenateString", _charArray, params, reinterpret_cast<void*>(&ConcatenateStrings));
    _engine->RegisterFunction("$FindString", _intType, params, reinterpret_cast<void*>(&FindString));
    _engine->RegisterFunction("$FindLastString", _intType, params, reinterpret_cast<void*>(&FindLastString));
    _engine->RegisterFunction("$CopyString", _intType, params, reinterpret_cast<void*>(&CopyString));
    _engine->RegisterFunction("$FindFirstNotIn", _intType, params, reinterpret_cast<void*>(&FindFirstNotIn));
    _engine->RegisterFunction("$FindLastNotIn", _intType, params, reinterpret_cast<void*>(&FindLastNotIn));
    _engine->RegisterFunction("$FindFirstIn", _intType, params, reinterpret_cast<void*>(&FindFirstIn));
    _engine->RegisterFunction("$FindLastIn", _intType, params, reinterpret_cast<void*>(&FindLastIn));
    _engine->RegisterFunction("$SplitFrom", _arrayOfArrayOfString, params, reinterpret_cast<void*>(&FindLastOf));
    params.push_back(_intType);
    _engine->RegisterFunction("$FindNextIn", _intType, params, reinterpret_cast<void*>(&FindNextIn));
    _engine->RegisterFunction("$FindNextLastIn", _intType, params, reinterpret_cast<void*>(&FindNextLastIn));
    _engine->RegisterFunction("$FindNextNotIn", _intType, params, reinterpret_cast<void*>(&FindNextNotIn));
    _engine->RegisterFunction("$FindNextLastNotIn", _intType, params, reinterpret_cast<void*>(&FindNextLastNotIn));
    params.pop_back();
    params.pop_back();
    params.push_back(_char);
    _engine->RegisterFunction("$FindFirstOf", _intType, params, reinterpret_cast<void*>(&FindFirstOf));
    _engine->RegisterFunction("$FindLastOf", _intType, params, reinterpret_cast<void*>(&FindLastOf));
    _engine->RegisterFunction("$Split", _arrayOfArrayOfString, params, reinterpret_cast<void*>(&FindLastOf));
    params.push_back(_intType);
    _engine->RegisterFunction("$FindNextOf", _intType, params, reinterpret_cast<void*>(&FindNextOf));
    _engine->RegisterFunction("$FindNextLastOf", _intType, params, reinterpret_cast<void*>(&FindNextLastOf));

    return(eWNOK);
}
