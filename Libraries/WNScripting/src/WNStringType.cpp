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

    virtual eWNTypeError Execute(llvm::IRBuilderBase* _builder, llvm::Value* _expr1, llvm::Value* _expr2, wn_char* _id,  WNScripting::WNScriptType &_outType, llvm::Value*& _outValue, llvm::Value*& _outLocation) const {
        return(mArrayOperator.Execute(_builder, _expr1, _expr2, _id, _outType, _outValue, _outLocation));
    }
private:
    WNScripting::WNScriptType mScriptType;
    WNScripting::WNScriptType mIntType;
    WNScripting::WNScriptType mFunctionType;
    WNScripting::GenerateArrayIDOperator mArrayOperator;
};

wn_void AppendString(WNScripting::WNScriptingArrayRef<wn_char>* _inRef, WNScripting::WNScriptingArrayRef<wn_char>* _toAppend) {
    WNScripting::WNScriptingArray<wn_char> inArray(_inRef);
    WNScripting::WNScriptingArray<wn_char> appendArray(_toAppend);
    if(!inArray|| !appendArray) {
        return;
    }
    inArray.Resize(inArray.GetLength() + appendArray.GetLength());
    memcpy(&inArray[inArray.GetLength()], &appendArray[0], appendArray.GetLength());
}

WNScripting::WNScriptingArrayRef<WNScripting::WNScriptingArray<wn_char> >* Split(WNScripting::WNScriptingArrayRef<wn_char>* _string, wn_char _split, wn_bool _ignoreEmpty) {
    WNScripting::WNScriptingArray<wn_char> string(_string);
    if(!string) {
        return(wn_nullptr);
    }
    WNScripting::WNScriptType _arrayArrayType =  string.GetType()->mScriptingEngine->GetExistingArrayOf(string.GetType());
    WNScripting::WNScriptType _stringType = string.GetType();
    const WNScripting::WNScriptingEngine* engine = string.GetType()->mScriptingEngine;
    if(!_arrayArrayType) {
        return(wn_nullptr);
    }
    wn_signed_t splitCount= 0;
    wn_signed_t index = 0;
    wn_signed_t previousIndex = -1;
    wn_signed_t strLen = string.GetLength();
    for(wn_signed_t i = 0; i < strLen; ++i){
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
    wn_void* outPtr;
    engine->ConstructScriptingArray(_arrayArrayType, splitCount, outPtr);
    WNScripting::WNScriptingArray<WNScripting::WNScriptingArray<wn_char> > newArray(outPtr);

    for(wn_signed_t i = 0; i < strLen; ++i){
        if(string[i] == _split) {
            if((i - previousIndex) > 1 || !_ignoreEmpty){
                wn_signed_t len = (i - previousIndex - 1);
                wn_void* dat;
                engine->ConstructScriptingArray(_stringType, len, dat);
                newArray[index] = WNScripting::WNScriptingArray<wn_char>(dat);
                if(len > 0) {
                    memcpy(&newArray[index][0], &string[previousIndex + 1], len);
                }
                index += 1;
            }
            previousIndex = i;
        }
    }

    if(previousIndex < strLen - 1 || !_ignoreEmpty) {
        wn_size_t len = (strLen - previousIndex - 1);
        wn_void* dat;
        engine->ConstructScriptingArray(_stringType, len, dat);
        newArray[index] = WNScripting::WNScriptingArray<wn_char>(dat);
        if(len > 0) {
            memcpy(&newArray[index][0], &string[previousIndex + 1], len);
        }
    }
    return(newArray.DetachOut());
}

WNScripting::WNScriptingArrayRef<WNScripting::WNScriptingArray<wn_char> >* SplitFrom(WNScripting::WNScriptingArrayRef<wn_char>* _string, WNScripting::WNScriptingArrayRef<wn_char>* _splitString, wn_bool _ignoreEmpty) {
    WNScripting::WNScriptingArray<wn_char> string(_string);
    WNScripting::WNScriptingArray<wn_char> splitString(_splitString);
    if(!string || !splitString) {
        return(wn_nullptr);
    }
    WNScripting::WNScriptType _arrayArrayType =  string.GetType()->mScriptingEngine->GetExistingArrayOf(string.GetType());
    WNScripting::WNScriptType _stringType = string.GetType();
    const WNScripting::WNScriptingEngine* engine = string.GetType()->mScriptingEngine;
    if(!_arrayArrayType) {
        return(wn_nullptr);
    }
    wn_signed_t splitLength = splitString.GetLength();
    wn_signed_t splitCount= 0;
    wn_signed_t index = 0;
    wn_signed_t previousIndex = -1;
    wn_signed_t strLen = string.GetLength();
    for(wn_signed_t i = 0; i < strLen; ++i){
        for(wn_signed_t j = 0; j < splitLength; ++j) {
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
    wn_void* outPtr;
    engine->ConstructScriptingArray(_arrayArrayType, splitCount, outPtr);
    WNScripting::WNScriptingArray<WNScripting::WNScriptingArray<wn_char> > newArray(outPtr);

    for(wn_signed_t i = 0; i < strLen; ++i){
        for(wn_signed_t j = 0; j < splitLength; ++j) {
            if(string[i] == splitString[j]) {
                if((i - previousIndex) > 1 || !_ignoreEmpty){
                    wn_signed_t len = (i - previousIndex - 1);
                    wn_void* dat;
                    engine->ConstructScriptingArray(_stringType, len, dat);
                    newArray[index] = WNScripting::WNScriptingArray<wn_char>(dat);
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
        wn_size_t len = (strLen - previousIndex - 1);
        wn_void* dat;
        engine->ConstructScriptingArray(_stringType, len, dat);
        newArray[index] = WNScripting::WNScriptingArray<wn_char>(dat);
        if(len > 0) {
            memcpy(&newArray[index][0], &string[previousIndex + 1], len);
        }
    }
    return(newArray.DetachOut());
}

wn_void ToLower(WNScripting::WNScriptingArrayRef<wn_char>* _string) {
    WNScripting::WNScriptingArray<wn_char> string(_string);
    if(!string) {
        return;
    }
    for(wn_size_t i = 0; i < string.GetLength(); ++i) {
      string[i] = static_cast<wn_char>(::tolower(string[i]));
    }
    return;
}

wn_void ToUpper(WNScripting::WNScriptingArrayRef<wn_char>* _string) {
    WNScripting::WNScriptingArray<wn_char> string(_string);
    if(!string) {
        return;
    }
    for(wn_size_t i = 0; i < string.GetLength(); ++i) {
      string[i] = static_cast<wn_char>(::toupper(string[i]));
    }
    return;
}

wn_int32 FindFirstIn(WNScripting::WNScriptingArrayRef<wn_char>* _string, WNScripting::WNScriptingArrayRef<wn_char>* _findee) {
    WNScripting::WNScriptingArray<wn_char> string(_string);
    WNScripting::WNScriptingArray<wn_char> findee(_findee);
    if(!string || !findee) {
        return(-1);
    }
    wn_size_t searchLen = findee.GetLength();
    for(wn_size_t i = 0; i < string.GetLength(); ++i) {
        for(wn_size_t j = 0; j < searchLen; ++j) {
            if(string[i] == findee[j]) {
                return(static_cast<wn_int32>(i));
            }
        }
    }
    return(-1);
}

wn_int32 FindFirstNotIn(WNScripting::WNScriptingArrayRef<wn_char>* _string, WNScripting::WNScriptingArrayRef<wn_char>* _findee) {
    WNScripting::WNScriptingArray<wn_char> string(_string);
    WNScripting::WNScriptingArray<wn_char> findee(_findee);
    if(!string || !findee) {
        return(-1);
    }
    wn_size_t searchLen = findee.GetLength();
    for(wn_size_t i = 0; i < string.GetLength(); ++i) {
        wn_size_t j = 0;
        for(j = 0; j < searchLen; ++j) {
            if(string[i] == findee[j]) {
               break;
            }
        }
        if(j == searchLen) {
            return(static_cast<wn_int32>(i));
        }
    }
    return(-1);
}

wn_int32 FindFirstOf(WNScripting::WNScriptingArrayRef<wn_char>* _string, wn_char _char) {
    WNScripting::WNScriptingArray<wn_char> string(_string);
    if(!string) {
        return(-1);
    }
    for(wn_size_t i = 0; i < string.GetLength(); ++i) {
        if(string[i] == _char) {
            return(static_cast<wn_int32>(i));
        }
    }
    return(-1);
}

wn_int32 FindLastIn(WNScripting::WNScriptingArrayRef<wn_char>* _string, WNScripting::WNScriptingArrayRef<wn_char>* _findee) {
    WNScripting::WNScriptingArray<wn_char> string(_string);
    WNScripting::WNScriptingArray<wn_char> findee(_findee);
    if(!string || !findee) {
        return(-1);
    }
    wn_int32 searchLen = static_cast<wn_int32>(findee.GetLength());
    for(wn_int32 i = static_cast<wn_int32>(string.GetLength()) - 1; i >= 0; --i) {
        for(wn_int32 j = 0; j < searchLen; ++j) {
            if(string[i] == findee[j]) {
                return(static_cast<wn_int32>(i));
            }
        }
    }
    return(-1);
}

wn_int32 FindLastNotIn(WNScripting::WNScriptingArrayRef<wn_char>* _string, WNScripting::WNScriptingArrayRef<wn_char>* _findee) {
    WNScripting::WNScriptingArray<wn_char> string(_string);
    WNScripting::WNScriptingArray<wn_char> findee(_findee);
    if(!string || !findee) {
        return(-1);
    }
    wn_size_t searchLen = findee.GetLength();
    for(wn_size_t i = string.GetLength() - 1; i != 0; --i) {
        wn_size_t j = 0;
        for(j = 0; j < searchLen; ++j) {
            if(string[i] == findee[j]) {
                break;
            }
        }
        if(j == searchLen) {
            return(static_cast<wn_int32>(i));
        }
    }
    return(-1);
}

wn_int32 FindLastOf(WNScripting::WNScriptingArrayRef<wn_char>* _string, wn_char _char) {
    WNScripting::WNScriptingArray<wn_char> string(_string);
    if(!string) {
        return(-1);
    }
    for(wn_int32 i = static_cast<wn_int32>(string.GetLength()) - 1; i >= 0; --i) {
        if(string[i] == _char) {
            return(static_cast<wn_int32>(i));
        }
    }
    return(-1);
}

wn_int32 FindNextIn(WNScripting::WNScriptingArrayRef<wn_char>* _string, WNScripting::WNScriptingArrayRef<wn_char>* _findee, wn_int32 index) {
    WNScripting::WNScriptingArray<wn_char> string(_string);
    WNScripting::WNScriptingArray<wn_char> findee(_findee);
    if(!string || !findee || index < 0) {
        return(-1);
    }
    wn_size_t searchLen = findee.GetLength();
    for(wn_size_t i = index; i < string.GetLength(); ++i) {
        for(wn_size_t j = 0; j < searchLen; ++j) {
            if(string[i] == findee[j]) {
                return(static_cast<wn_int32>(i));
            }
        }
    }
    return(-1);
}

wn_int32 FindNextNotIn(WNScripting::WNScriptingArrayRef<wn_char>* _string, WNScripting::WNScriptingArrayRef<wn_char>* _findee, wn_int32 index) {
    WNScripting::WNScriptingArray<wn_char> string(_string);
    WNScripting::WNScriptingArray<wn_char> findee(_findee);
    if(!string || !findee || index < 0) {
        return(-1);
    }
    wn_size_t searchLen = findee.GetLength();
    for(wn_size_t i = index; i < string.GetLength(); ++i) {
        wn_size_t j = 0;
        for(j = 0; j < searchLen; ++j) {
            if(string[i] == findee[j]) {
                break;
            }
        }
        if(j == searchLen) {
            return(static_cast<wn_int32>(i));
        }
    }
    return(-1);
}
wn_int32 FindNextOf(WNScripting::WNScriptingArrayRef<wn_char>* _string, wn_char _char, wn_int32 index) {
    WNScripting::WNScriptingArray<wn_char> string(_string);
    if(!string || index < 0) {
        return(-1);
    }
    for(wn_size_t i = index; i < string.GetLength(); ++i) {
        if(string[i] == _char) {
            return(static_cast<wn_int32>(i));
        }
    }
    return(-1);
}

wn_int32 FindNextLastIn(WNScripting::WNScriptingArrayRef<wn_char>* _string, WNScripting::WNScriptingArrayRef<wn_char>* _findee, wn_int32 index) {
    WNScripting::WNScriptingArray<wn_char> string(_string);
    WNScripting::WNScriptingArray<wn_char> findee(_findee);
    if(!string || !findee || static_cast<wn_size_t>(index) > string.GetLength()) {
        return(-1);
    }
    wn_size_t searchLen = findee.GetLength();
    for(wn_int32 i = index; i >= 0; --i) {
        for(wn_size_t j = 0; j < searchLen; ++j) {
            if(string[i] == findee[j]) {
                return(static_cast<wn_int32>(i));
            }
        }
    }
    return(-1);
}

wn_int32 FindNextLastNotIn(WNScripting::WNScriptingArrayRef<wn_char>* _string, WNScripting::WNScriptingArrayRef<wn_char>* _findee, wn_int32 index) {
    WNScripting::WNScriptingArray<wn_char> string(_string);
    WNScripting::WNScriptingArray<wn_char> findee(_findee);
    if(!string || !findee || static_cast<wn_size_t>(index) > string.GetLength()) {
        return(-1);
    }
    wn_size_t searchLen = findee.GetLength();
    for(wn_int32 i = index; i >= 0; --i) {
        wn_size_t j = 0;
        for(j = 0; j < searchLen; ++j) {
            if(string[i] == findee[j]) {
                break;
            }
        }
        if(j == searchLen) {
            return(static_cast<wn_int32>(i));
        }
    }
    return(-1);
}

wn_int32 FindNextLastOf(WNScripting::WNScriptingArrayRef<wn_char>* _string, wn_char _char, wn_int32 index) {
    WNScripting::WNScriptingArray<wn_char> string(_string);
    if(!string || static_cast<wn_size_t>(index) > string.GetLength()) {
        return(-1);
    }
    for(wn_int32 i = index; i >= 0; --i) {
        if(string[i] == _char) {
            return(static_cast<wn_int32>(i));
        }
    }
    return(-1);
}

wn_int32 FindString(WNScripting::WNScriptingArrayRef<wn_char>* _string, WNScripting::WNScriptingArrayRef<wn_char>* _substr) {
    WNScripting::WNScriptingArray<wn_char> string(_string);
    WNScripting::WNScriptingArray<wn_char> substr(_substr);
    if(!string || !substr) {
        return(-1);
    }

    for(wn_int32 i = 0; i < static_cast<wn_int32>(string.GetLength()) - static_cast<wn_int32>(substr.GetLength()); ++i){
        if(wn::memory::strncmp(&string[i], &substr[0], substr.GetLength()) == 0) {
            return(static_cast<wn_int32>(i));
        }
    }

    return(-1);
}

wn_int32 FindLastString(WNScripting::WNScriptingArrayRef<wn_char>* _string, WNScripting::WNScriptingArrayRef<wn_char>* _substr) {
    WNScripting::WNScriptingArray<wn_char> string(_string);
    WNScripting::WNScriptingArray<wn_char> substr(_substr);
    if(!string || !substr) {
        return(-1);
    }

    for(wn_int32 i = static_cast<wn_int32>(string.GetLength()) - static_cast<wn_int32>(substr.GetLength()) - 1; i >= 0 ; --i){
        if(wn::memory::strncmp(&string[i], &substr[0], substr.GetLength()) == 0) {
            return(static_cast<wn_int32>(i));
        }
    }
    return(-1);
}

WNScripting::WNScriptingArrayRef<wn_char>* ConcatenateStrings(WNScripting::WNScriptingArrayRef<wn_char>* _string1, WNScripting::WNScriptingArrayRef<wn_char>* _string2) {
    WNScripting::WNScriptingArray<wn_char> string1(_string1);
    WNScripting::WNScriptingArray<wn_char> string2(_string2);
    if(!string1 || !string2) {
        return(wn_nullptr);
    }
    void* outPtr;
    string1.GetType()->mScriptingEngine->ConstructScriptingArray(string1.GetType(), string1.GetLength() + string2.GetLength(), outPtr);
    WNScripting::WNScriptingArray<wn_char> outArray(outPtr);
    memcpy(&outArray[0], &string1[0], string1.GetLength());
    memcpy(&outArray[string1.GetLength()], &string2[0], string2.GetLength());
    return(outArray.DetachOut());
}

WNScripting::WNScriptingArrayRef<wn_char>* CopyString(WNScripting::WNScriptingArrayRef<wn_char>* _string1) {
    WNScripting::WNScriptingArray<wn_char> string1(_string1);
    if(!string1) {
        return(wn_nullptr);
    }
    void* outPtr;
    string1.GetType()->mScriptingEngine->ConstructScriptingArray(string1.GetType(), string1.GetLength(), outPtr);
    WNScripting::WNScriptingArray<wn_char> outArray(outPtr);
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
    if(ok != _manager.GetTypeByName("Char", _char)) {
        return(error);
    }
    if(ok != _manager.GetTypeByName("Int", _intType)) {
        return(error);
    }
    if(ok != _manager.GetTypeByName("-Null", _nullType)) {
        return(error);
    }
    if(ok != _manager.GetTypeByName("Bool", _boolType)) {
        return(error);
    }
    if(ok != _manager.GetTypeByName("-Function", _functionType)) {
        return(error);
    }
    if(ok != _manager.GetTypeByName("Void", _voidType)) {
        return(error);
    }
    if(ok != _manager.GetArrayOf(_char, _charArray)) {
        return(error);
    }

    if(ok != _manager.GetArrayOf(_charArray, _arrayOfArrayOfString)) { //We use this here so that is
                                                                          //is populated so we can get it through
                                                                          //a const ScriptingEngine later
        return(error);
    }

    if(ok != _manager.RegisterAliasedStruct("String", _charArray, _myType)) {
        return(error);
    }

    _manager.RegisterCastingOperator(_charArray, _myType, wn::memory::construct<WNScripting::GenerateReinterpretCastOperation>(_myType));
    _manager.RegisterCastingOperator(_myType, _charArray, wn::memory::construct<WNScripting::GenerateReinterpretCastOperation>(_charArray));
    _manager.RegisterArrayAccessOperator(_myType, _intType, wn::memory::construct<WNScripting::GenerateDefaultArrayAccessOperation>(_myType));
    _manager.RegisterDestructionOperator(_myType, wn::memory::construct<WNScripting::GenerateArrayDestruction>());
    _manager.RegisterAllocationOperator(_myType, wn::memory::construct<WNScripting::GenerateDefaultAllocation>());
    _manager.RegisterAssignmentOperator(_myType, AT_EQ, wn::memory::construct<WNScripting::GenerateStructAssignment>());
    _manager.RegisterAssignmentOperator(_myType, AT_CHOWN, wn::memory::construct<WNScripting::GenerateStructTransfer>());
    _manager.RegisterCastingOperator(_nullType, _myType, wn::memory::construct<WNScripting::GenerateReinterpretCastOperation>(_myType));
    _manager.RegisterCastingOperator(_myType, _nullType, wn::memory::construct<WNScripting::GenerateReinterpretCastOperation>(_nullType));
    _manager.RegisterArithmeticOperator(AR_EQ, _myType, _myType, wn::memory::construct<WNScripting::GenerateStructCompare>(_boolType, wn_true));
    _manager.RegisterArithmeticOperator(AR_NEQ, _myType, _myType, wn::memory::construct<WNScripting::GenerateStructCompare>(_boolType, wn_false));
    _manager.RegisterIDAccessOperator(_myType, wn::memory::construct<WNScripting::GenerateArrayIDOperator>(_myType, _intType, _functionType));

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

    return(ok);
}
