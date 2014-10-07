#include "WNCore/inc/WNTypes.h"
#include "WNScripting/inc/WNTypeManager.h"



eWNTypeError RegisterExternalString(WNScripting::WNTypeManager& _manager, void*) {
    WNScripting::WNScriptType _charArray;
    WNScripting::WNScriptType _char;
    WNScripting::WNScriptType _myType;
    WNScripting::WNScriptType _intType;
    WNScripting::WNScriptType _nullType;
    WNScripting::WNScriptType _boolType;
    WNScripting::WNScriptType _functionType;
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
        
    }
    if(eWNOK != _manager.GetArrayOf(_char, _charArray)) {
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
    return(eWNOK);
}
