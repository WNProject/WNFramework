// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.


#ifndef __WN_SCRIPTING_ENGINE_H__
#define __WN_SCRIPTING_ENGINE_H__

#include "WNDeprecatedScripting/inc/WNScriptingEngine.h"
#include <vector>

using WNScripting::WNScriptType;
namespace wn {
	namespace scripting {
		class scripting_engine {
		public:
			virtual ~scripting_engine() {}

			virtual eWNTypeError get_function_pointer(const wn_char* _file, const wn_char* _functionName, WNScriptType& _retParam, const std::vector<WNScriptType>& _params, void*& _ptr);
			virtual wn_int32 get_virtual_function_index(const wn_char* _functionName, WNScriptType& _type, const std::vector<WNScriptType>& _params);
			virtual eWNTypeError initialize();
			virtual WNScriptType get_type_by_name(const wn_char* _typeName) const;
			virtual WNScriptType get_array_of(WNScriptType _type);
			virtual WNScriptType get_existing_array_of(WNScriptType _type) const;
			virtual eWNTypeError register_function(const wn_char* _functionName, const WNScriptType _retParam, const std::vector<WNScriptType>& _params, void* _ptr);
			virtual eWNTypeError register_member_function(const wn_char* _functionName, const WNScriptType _thisType, const WNScriptType _retParam, const std::vector<WNScriptType>& _params, void* _ptr);
			virtual eWNTypeError register_member(const wn_char* _varName, const WNScriptType _thisType, const WNScriptType _varType, wn_size_t _offset);
			virtual void* get_registered_function(const wn_char* _functionName) const;
			virtual void set_internal_log_level(WNLogging::WNLogLevel);
			virtual void set_log_level(WNLogging::WNLogLevel);
			virtual void set_compilation_log(WNLogging::WNLog*);
			virtual eWNTypeError register_external_type(const wn_char* _typeName, void(*ptr)(void*));
			virtual eWNTypeError register_external_type(const wn_char* _typeName, const wn_char* _parentType, void(*ptr)(void*));
			virtual eWNTypeError compile_file(const wn_char* _file);
			virtual eWNTypeError construct_scripting_object(WNScriptType _type, wn_void*& _retVal) const;
			virtual eWNTypeError construct_scripting_array(WNScriptType _type, wn_size_t _size, wn_void*& _retVal) const;
		};
	}
}
#include "WNFunctionTemplates.inl"
#endif//__WN_SCRIPTING_ENGINE_H__
