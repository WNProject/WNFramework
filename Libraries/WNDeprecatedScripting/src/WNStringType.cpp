#include "WNCore/inc/WNTypes.h"
#include "WNDeprecatedScripting/inc/WNScriptingInterop.h"
#include "WNDeprecatedScripting/inc/WNTypeManager.h"

struct GenerateStringIDOperator
    : public WNScripting::GenerateIDAccessOperation {
  GenerateStringIDOperator(WNScripting::WNScriptType _scriptType,
      WNScripting::WNScriptType _intType,
      WNScripting::WNScriptType _functionType)
    : mArrayOperator(_scriptType, _intType, _functionType),
      mScriptType(_scriptType),
      mIntType(_intType),
      mFunctionType(_functionType) {}

  virtual eWNTypeError Execute(llvm::IRBuilderBase* _builder,
      llvm::Value* _expr1, llvm::Value* _expr2, char* _id,
      WNScripting::WNScriptType& _outType, llvm::Value*& _outValue,
      llvm::Value*& _outLocation) const {
    return (mArrayOperator.Execute(
        _builder, _expr1, _expr2, _id, _outType, _outValue, _outLocation));
  }

private:
  WNScripting::WNScriptType mScriptType;
  WNScripting::WNScriptType mIntType;
  WNScripting::WNScriptType mFunctionType;
  WNScripting::GenerateArrayIDOperator mArrayOperator;
};

void AppendString(WNScripting::WNScriptingArrayRef<char>* _inRef,
    WNScripting::WNScriptingArrayRef<char>* _toAppend) {
  WNScripting::WNScriptingArray<char> inArray(_inRef);
  WNScripting::WNScriptingArray<char> appendArray(_toAppend);
  if (!inArray || !appendArray) {
    return;
  }
  inArray.Resize(inArray.GetLength() + appendArray.GetLength());
  memcpy(
      &inArray[inArray.GetLength()], &appendArray[0], appendArray.GetLength());
}

WNScripting::WNScriptingArrayRef<WNScripting::WNScriptingArray<char>>* Split(
    WNScripting::WNScriptingArrayRef<char>* _string, char _split,
    bool _ignoreEmpty) {
  WNScripting::WNScriptingArray<char> string(_string);
  if (!string) {
    return (nullptr);
  }
  WNScripting::WNScriptType _arrayArrayType =
      string.GetType()->mScriptingEngine->get_existing_array_of(
          string.GetType());
  WNScripting::WNScriptType _stringType = string.GetType();
  const WNScripting::WNScriptingEngine* engine =
      string.GetType()->mScriptingEngine;
  if (!_arrayArrayType) {
    return (nullptr);
  }
  signed_t splitCount = 0;
  signed_t index = 0;
  signed_t previousIndex = -1;
  signed_t strLen = string.GetLength();
  for (signed_t i = 0; i < strLen; ++i) {
    if (string[i] == _split) {
      if ((i - previousIndex) > 1 || !_ignoreEmpty) {
        splitCount += 1;
      }
      previousIndex = i;
    }
  }
  if (previousIndex < strLen - 1 || !_ignoreEmpty) {
    splitCount += 1;
  }
  previousIndex = -1;
  // now we have the number of splits we need time to allocate that array
  void* outPtr;
  engine->construct_scripting_array(_arrayArrayType, splitCount, outPtr);
  WNScripting::WNScriptingArray<WNScripting::WNScriptingArray<char>> newArray(
      outPtr);

  for (signed_t i = 0; i < strLen; ++i) {
    if (string[i] == _split) {
      if ((i - previousIndex) > 1 || !_ignoreEmpty) {
        signed_t len = (i - previousIndex - 1);
        void* dat;
        engine->construct_scripting_array(_stringType, len, dat);
        newArray[index] = WNScripting::WNScriptingArray<char>(dat);
        if (len > 0) {
          memcpy(&newArray[index][0], &string[previousIndex + 1], len);
        }
        index += 1;
      }
      previousIndex = i;
    }
  }

  if (previousIndex < strLen - 1 || !_ignoreEmpty) {
    size_t len = (strLen - previousIndex - 1);
    void* dat;
    engine->construct_scripting_array(_stringType, len, dat);
    newArray[index] = WNScripting::WNScriptingArray<char>(dat);
    if (len > 0) {
      memcpy(&newArray[index][0], &string[previousIndex + 1], len);
    }
  }
  return (newArray.DetachOut());
}

WNScripting::WNScriptingArrayRef<WNScripting::WNScriptingArray<char>>*
SplitFrom(WNScripting::WNScriptingArrayRef<char>* _string,
    WNScripting::WNScriptingArrayRef<char>* _splitString, bool _ignoreEmpty) {
  WNScripting::WNScriptingArray<char> string(_string);
  WNScripting::WNScriptingArray<char> splitString(_splitString);
  if (!string || !splitString) {
    return (nullptr);
  }
  WNScripting::WNScriptType _arrayArrayType =
      string.GetType()->mScriptingEngine->get_existing_array_of(
          string.GetType());
  WNScripting::WNScriptType _stringType = string.GetType();
  const WNScripting::WNScriptingEngine* engine =
      string.GetType()->mScriptingEngine;
  if (!_arrayArrayType) {
    return (nullptr);
  }
  signed_t splitLength = splitString.GetLength();
  signed_t splitCount = 0;
  signed_t index = 0;
  signed_t previousIndex = -1;
  signed_t strLen = string.GetLength();
  for (signed_t i = 0; i < strLen; ++i) {
    for (signed_t j = 0; j < splitLength; ++j) {
      if (string[i] == splitString[j]) {
        if ((i - previousIndex) > 1 || !_ignoreEmpty) {
          splitCount += 1;
        }
        previousIndex = i;
        break;
      }
    }
  }
  if (previousIndex < strLen - 1 || !_ignoreEmpty) {
    splitCount += 1;
  }
  previousIndex = -1;
  // now we have the number of splits we need time to allocate that array
  void* outPtr;
  engine->construct_scripting_array(_arrayArrayType, splitCount, outPtr);
  WNScripting::WNScriptingArray<WNScripting::WNScriptingArray<char>> newArray(
      outPtr);

  for (signed_t i = 0; i < strLen; ++i) {
    for (signed_t j = 0; j < splitLength; ++j) {
      if (string[i] == splitString[j]) {
        if ((i - previousIndex) > 1 || !_ignoreEmpty) {
          signed_t len = (i - previousIndex - 1);
          void* dat;
          engine->construct_scripting_array(_stringType, len, dat);
          newArray[index] = WNScripting::WNScriptingArray<char>(dat);
          if (len > 0) {
            memcpy(&newArray[index][0], &string[previousIndex + 1], len);
          }
          index += 1;
        }
        previousIndex = i;
        break;
      }
    }
  }

  if (previousIndex < strLen - 1 || !_ignoreEmpty) {
    size_t len = (strLen - previousIndex - 1);
    void* dat;
    engine->construct_scripting_array(_stringType, len, dat);
    newArray[index] = WNScripting::WNScriptingArray<char>(dat);
    if (len > 0) {
      memcpy(&newArray[index][0], &string[previousIndex + 1], len);
    }
  }
  return (newArray.DetachOut());
}

void ToLower(WNScripting::WNScriptingArrayRef<char>* _string) {
  WNScripting::WNScriptingArray<char> string(_string);
  if (!string) {
    return;
  }
  for (size_t i = 0; i < string.GetLength(); ++i) {
    string[i] = static_cast<char>(::tolower(string[i]));
  }
  return;
}

void ToUpper(WNScripting::WNScriptingArrayRef<char>* _string) {
  WNScripting::WNScriptingArray<char> string(_string);
  if (!string) {
    return;
  }
  for (size_t i = 0; i < string.GetLength(); ++i) {
    string[i] = static_cast<char>(::toupper(string[i]));
  }
  return;
}

int32_t FindFirstIn(WNScripting::WNScriptingArrayRef<char>* _string,
    WNScripting::WNScriptingArrayRef<char>* _findee) {
  WNScripting::WNScriptingArray<char> string(_string);
  WNScripting::WNScriptingArray<char> findee(_findee);
  if (!string || !findee) {
    return (-1);
  }
  size_t searchLen = findee.GetLength();
  for (size_t i = 0; i < string.GetLength(); ++i) {
    for (size_t j = 0; j < searchLen; ++j) {
      if (string[i] == findee[j]) {
        return (static_cast<int32_t>(i));
      }
    }
  }
  return (-1);
}

int32_t FindFirstNotIn(WNScripting::WNScriptingArrayRef<char>* _string,
    WNScripting::WNScriptingArrayRef<char>* _findee) {
  WNScripting::WNScriptingArray<char> string(_string);
  WNScripting::WNScriptingArray<char> findee(_findee);
  if (!string || !findee) {
    return (-1);
  }
  size_t searchLen = findee.GetLength();
  for (size_t i = 0; i < string.GetLength(); ++i) {
    size_t j = 0;
    for (j = 0; j < searchLen; ++j) {
      if (string[i] == findee[j]) {
        break;
      }
    }
    if (j == searchLen) {
      return (static_cast<int32_t>(i));
    }
  }
  return (-1);
}

int32_t FindFirstOf(
    WNScripting::WNScriptingArrayRef<char>* _string, char _char) {
  WNScripting::WNScriptingArray<char> string(_string);
  if (!string) {
    return (-1);
  }
  for (size_t i = 0; i < string.GetLength(); ++i) {
    if (string[i] == _char) {
      return (static_cast<int32_t>(i));
    }
  }
  return (-1);
}

int32_t FindLastIn(WNScripting::WNScriptingArrayRef<char>* _string,
    WNScripting::WNScriptingArrayRef<char>* _findee) {
  WNScripting::WNScriptingArray<char> string(_string);
  WNScripting::WNScriptingArray<char> findee(_findee);
  if (!string || !findee) {
    return (-1);
  }
  int32_t searchLen = static_cast<int32_t>(findee.GetLength());
  for (int32_t i = static_cast<int32_t>(string.GetLength()) - 1; i >= 0; --i) {
    for (int32_t j = 0; j < searchLen; ++j) {
      if (string[i] == findee[j]) {
        return (static_cast<int32_t>(i));
      }
    }
  }
  return (-1);
}

int32_t FindLastNotIn(WNScripting::WNScriptingArrayRef<char>* _string,
    WNScripting::WNScriptingArrayRef<char>* _findee) {
  WNScripting::WNScriptingArray<char> string(_string);
  WNScripting::WNScriptingArray<char> findee(_findee);
  if (!string || !findee) {
    return (-1);
  }
  size_t searchLen = findee.GetLength();
  for (size_t i = string.GetLength() - 1; i != 0; --i) {
    size_t j = 0;
    for (j = 0; j < searchLen; ++j) {
      if (string[i] == findee[j]) {
        break;
      }
    }
    if (j == searchLen) {
      return (static_cast<int32_t>(i));
    }
  }
  return (-1);
}

int32_t FindLastOf(
    WNScripting::WNScriptingArrayRef<char>* _string, char _char) {
  WNScripting::WNScriptingArray<char> string(_string);
  if (!string) {
    return (-1);
  }
  for (int32_t i = static_cast<int32_t>(string.GetLength()) - 1; i >= 0; --i) {
    if (string[i] == _char) {
      return (static_cast<int32_t>(i));
    }
  }
  return (-1);
}

int32_t FindNextIn(WNScripting::WNScriptingArrayRef<char>* _string,
    WNScripting::WNScriptingArrayRef<char>* _findee, int32_t index) {
  WNScripting::WNScriptingArray<char> string(_string);
  WNScripting::WNScriptingArray<char> findee(_findee);
  if (!string || !findee || index < 0) {
    return (-1);
  }
  size_t searchLen = findee.GetLength();
  for (size_t i = index; i < string.GetLength(); ++i) {
    for (size_t j = 0; j < searchLen; ++j) {
      if (string[i] == findee[j]) {
        return (static_cast<int32_t>(i));
      }
    }
  }
  return (-1);
}

int32_t FindNextNotIn(WNScripting::WNScriptingArrayRef<char>* _string,
    WNScripting::WNScriptingArrayRef<char>* _findee, int32_t index) {
  WNScripting::WNScriptingArray<char> string(_string);
  WNScripting::WNScriptingArray<char> findee(_findee);
  if (!string || !findee || index < 0) {
    return (-1);
  }
  size_t searchLen = findee.GetLength();
  for (size_t i = index; i < string.GetLength(); ++i) {
    size_t j = 0;
    for (j = 0; j < searchLen; ++j) {
      if (string[i] == findee[j]) {
        break;
      }
    }
    if (j == searchLen) {
      return (static_cast<int32_t>(i));
    }
  }
  return (-1);
}
int32_t FindNextOf(WNScripting::WNScriptingArrayRef<char>* _string, char _char,
    int32_t index) {
  WNScripting::WNScriptingArray<char> string(_string);
  if (!string || index < 0) {
    return (-1);
  }
  for (size_t i = index; i < string.GetLength(); ++i) {
    if (string[i] == _char) {
      return (static_cast<int32_t>(i));
    }
  }
  return (-1);
}

int32_t FindNextLastIn(WNScripting::WNScriptingArrayRef<char>* _string,
    WNScripting::WNScriptingArrayRef<char>* _findee, int32_t index) {
  WNScripting::WNScriptingArray<char> string(_string);
  WNScripting::WNScriptingArray<char> findee(_findee);
  if (!string || !findee || static_cast<size_t>(index) > string.GetLength()) {
    return (-1);
  }
  size_t searchLen = findee.GetLength();
  for (int32_t i = index; i >= 0; --i) {
    for (size_t j = 0; j < searchLen; ++j) {
      if (string[i] == findee[j]) {
        return (static_cast<int32_t>(i));
      }
    }
  }
  return (-1);
}

int32_t FindNextLastNotIn(WNScripting::WNScriptingArrayRef<char>* _string,
    WNScripting::WNScriptingArrayRef<char>* _findee, int32_t index) {
  WNScripting::WNScriptingArray<char> string(_string);
  WNScripting::WNScriptingArray<char> findee(_findee);
  if (!string || !findee || static_cast<size_t>(index) > string.GetLength()) {
    return (-1);
  }
  size_t searchLen = findee.GetLength();
  for (int32_t i = index; i >= 0; --i) {
    size_t j = 0;
    for (j = 0; j < searchLen; ++j) {
      if (string[i] == findee[j]) {
        break;
      }
    }
    if (j == searchLen) {
      return (static_cast<int32_t>(i));
    }
  }
  return (-1);
}

int32_t FindNextLastOf(WNScripting::WNScriptingArrayRef<char>* _string,
    char _char, int32_t index) {
  WNScripting::WNScriptingArray<char> string(_string);
  if (!string || static_cast<size_t>(index) > string.GetLength()) {
    return (-1);
  }
  for (int32_t i = index; i >= 0; --i) {
    if (string[i] == _char) {
      return (static_cast<int32_t>(i));
    }
  }
  return (-1);
}

int32_t FindString(WNScripting::WNScriptingArrayRef<char>* _string,
    WNScripting::WNScriptingArrayRef<char>* _substr) {
  WNScripting::WNScriptingArray<char> string(_string);
  WNScripting::WNScriptingArray<char> substr(_substr);
  if (!string || !substr) {
    return (-1);
  }

  for (int32_t i = 0; i < static_cast<int32_t>(string.GetLength()) -
                              static_cast<int32_t>(substr.GetLength());
       ++i) {
    if (wn::memory::strncmp(&string[i], &substr[0], substr.GetLength()) == 0) {
      return (static_cast<int32_t>(i));
    }
  }

  return (-1);
}

int32_t FindLastString(WNScripting::WNScriptingArrayRef<char>* _string,
    WNScripting::WNScriptingArrayRef<char>* _substr) {
  WNScripting::WNScriptingArray<char> string(_string);
  WNScripting::WNScriptingArray<char> substr(_substr);
  if (!string || !substr) {
    return (-1);
  }

  for (int32_t i = static_cast<int32_t>(string.GetLength()) -
                   static_cast<int32_t>(substr.GetLength()) - 1;
       i >= 0; --i) {
    if (wn::memory::strncmp(&string[i], &substr[0], substr.GetLength()) == 0) {
      return (static_cast<int32_t>(i));
    }
  }
  return (-1);
}

WNScripting::WNScriptingArrayRef<char>* ConcatenateStrings(
    WNScripting::WNScriptingArrayRef<char>* _string1,
    WNScripting::WNScriptingArrayRef<char>* _string2) {
  WNScripting::WNScriptingArray<char> string1(_string1);
  WNScripting::WNScriptingArray<char> string2(_string2);
  if (!string1 || !string2) {
    return (nullptr);
  }
  void* outPtr;
  string1.GetType()->mScriptingEngine->construct_scripting_array(
      string1.GetType(), string1.GetLength() + string2.GetLength(), outPtr);
  WNScripting::WNScriptingArray<char> outArray(outPtr);
  memcpy(&outArray[0], &string1[0], string1.GetLength());
  memcpy(&outArray[string1.GetLength()], &string2[0], string2.GetLength());
  return (outArray.DetachOut());
}

WNScripting::WNScriptingArrayRef<char>* CopyString(
    WNScripting::WNScriptingArrayRef<char>* _string1) {
  WNScripting::WNScriptingArray<char> string1(_string1);
  if (!string1) {
    return (nullptr);
  }
  void* outPtr;
  string1.GetType()->mScriptingEngine->construct_scripting_array(
      string1.GetType(), string1.GetLength(), outPtr);
  WNScripting::WNScriptingArray<char> outArray(outPtr);
  memcpy(&outArray[0], &string1[0], string1.GetLength());
  return (outArray.DetachOut());
}

eWNTypeError RegisterExternalString(WNScripting::WNScriptingEngine* _engine,
    WNScripting::WNTypeManager& _manager, void*) {
  WNScripting::WNScriptType _charArray;
  WNScripting::WNScriptType _char;
  WNScripting::WNScriptType _myType;
  WNScripting::WNScriptType _intType;
  WNScripting::WNScriptType _nullType;
  WNScripting::WNScriptType _boolType;
  WNScripting::WNScriptType _voidType;
  WNScripting::WNScriptType _functionType;
  WNScripting::WNScriptType _arrayOfArrayOfString;
  if (ok != _manager.get_type_by_name("Char", _char)) {
    return (error);
  }
  if (ok != _manager.get_type_by_name("Int", _intType)) {
    return (error);
  }
  if (ok != _manager.get_type_by_name("-Null", _nullType)) {
    return (error);
  }
  if (ok != _manager.get_type_by_name("Bool", _boolType)) {
    return (error);
  }
  if (ok != _manager.get_type_by_name("-Function", _functionType)) {
    return (error);
  }
  if (ok != _manager.get_type_by_name("Void", _voidType)) {
    return (error);
  }
  if (ok != _manager.get_array_of(_char, _charArray)) {
    return (error);
  }

  if (ok !=
      _manager.get_array_of(
          _charArray, _arrayOfArrayOfString)) {  // We use this here so that is
    // is populated so we can get it through
    // a const ScriptingEngine later
    return (error);
  }

  if (ok != _manager.RegisterAliasedStruct("String", _charArray, _myType)) {
    return (error);
  }

  _manager.RegisterCastingOperator(_charArray, _myType,
      wn::memory::construct<WNScripting::GenerateReinterpretCastOperation>(
                                       _myType));
  _manager.RegisterCastingOperator(_myType, _charArray,
      wn::memory::construct<WNScripting::GenerateReinterpretCastOperation>(
                                       _charArray));
  _manager.RegisterArrayAccessOperator(_myType, _intType,
      wn::memory::construct<WNScripting::GenerateDefaultArrayAccessOperation>(
                                           _myType));
  _manager.RegisterDestructionOperator(
      _myType, wn::memory::construct<WNScripting::GenerateArrayDestruction>());
  _manager.RegisterAllocationOperator(
      _myType, wn::memory::construct<WNScripting::GenerateDefaultAllocation>());
  _manager.RegisterAssignmentOperator(_myType, AT_EQ,
      wn::memory::construct<WNScripting::GenerateStructAssignment>());
  _manager.RegisterAssignmentOperator(_myType, AT_CHOWN,
      wn::memory::construct<WNScripting::GenerateStructTransfer>());
  _manager.RegisterCastingOperator(_nullType, _myType,
      wn::memory::construct<WNScripting::GenerateReinterpretCastOperation>(
                                       _myType));
  _manager.RegisterCastingOperator(_myType, _nullType,
      wn::memory::construct<WNScripting::GenerateReinterpretCastOperation>(
                                       _nullType));
  _manager.RegisterArithmeticOperator(AR_EQ, _myType, _myType,
      wn::memory::construct<WNScripting::GenerateStructCompare>(
                                          _boolType, true));
  _manager.RegisterArithmeticOperator(AR_NEQ, _myType, _myType,
      wn::memory::construct<WNScripting::GenerateStructCompare>(
                                          _boolType, false));
  _manager.RegisterIDAccessOperator(
      _myType, wn::memory::construct<WNScripting::GenerateArrayIDOperator>(
                   _myType, _intType, _functionType));

  std::vector<WNScripting::WNScriptType> params;
  params.push_back(_charArray);
  _engine->register_function(
      "$ToLower", _voidType, params, reinterpret_cast<void*>(&ToLower));
  _engine->register_function(
      "$ToUpper", _voidType, params, reinterpret_cast<void*>(&ToUpper));
  params.push_back(_charArray);
  _engine->register_function("$AppendString", _voidType, params,
      reinterpret_cast<void*>(&AppendString));
  _engine->register_function("$ConcatenateString", _charArray, params,
      reinterpret_cast<void*>(&ConcatenateStrings));
  _engine->register_function(
      "$FindString", _intType, params, reinterpret_cast<void*>(&FindString));
  _engine->register_function("$FindLastString", _intType, params,
      reinterpret_cast<void*>(&FindLastString));
  _engine->register_function(
      "$CopyString", _intType, params, reinterpret_cast<void*>(&CopyString));
  _engine->register_function("$FindFirstNotIn", _intType, params,
      reinterpret_cast<void*>(&FindFirstNotIn));
  _engine->register_function("$FindLastNotIn", _intType, params,
      reinterpret_cast<void*>(&FindLastNotIn));
  _engine->register_function(
      "$FindFirstIn", _intType, params, reinterpret_cast<void*>(&FindFirstIn));
  _engine->register_function(
      "$FindLastIn", _intType, params, reinterpret_cast<void*>(&FindLastIn));
  _engine->register_function("$SplitFrom", _arrayOfArrayOfString, params,
      reinterpret_cast<void*>(&FindLastOf));
  params.push_back(_intType);
  _engine->register_function(
      "$FindNextIn", _intType, params, reinterpret_cast<void*>(&FindNextIn));
  _engine->register_function("$FindNextLastIn", _intType, params,
      reinterpret_cast<void*>(&FindNextLastIn));
  _engine->register_function("$FindNextNotIn", _intType, params,
      reinterpret_cast<void*>(&FindNextNotIn));
  _engine->register_function("$FindNextLastNotIn", _intType, params,
      reinterpret_cast<void*>(&FindNextLastNotIn));
  params.pop_back();
  params.pop_back();
  params.push_back(_char);
  _engine->register_function(
      "$FindFirstOf", _intType, params, reinterpret_cast<void*>(&FindFirstOf));
  _engine->register_function(
      "$FindLastOf", _intType, params, reinterpret_cast<void*>(&FindLastOf));
  _engine->register_function("$Split", _arrayOfArrayOfString, params,
      reinterpret_cast<void*>(&FindLastOf));
  params.push_back(_intType);
  _engine->register_function(
      "$FindNextOf", _intType, params, reinterpret_cast<void*>(&FindNextOf));
  _engine->register_function("$FindNextLastOf", _intType, params,
      reinterpret_cast<void*>(&FindNextLastOf));

  return (ok);
}
