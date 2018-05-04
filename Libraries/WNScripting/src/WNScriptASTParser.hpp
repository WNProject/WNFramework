/** \file
 *  This C++ header file was generated by $ANTLR version 3.5.3-SNAPSHOT
 *
 *     -                for the parser : WNScriptASTParserParser
 *
 * Editing it, at least manually, is not wise.
 *
 * C++ language generator and runtime by Gokulakannan Somasundaram ( heavy lifting from C Run-time by Jim Idle )
 *
 *
 * The parser WNScriptASTParser has the callable functions (rules) shown below,
 * which will invoke the code for the associated rule in the source grammar
 * assuming that the input stream is pointing to a token/text stream that could begin
 * this rule.
 *
 * For instance if you call the first (topmost) rule in a parser grammar, you will
 * get the results of a full parse, but calling a rule half way through the grammar will
 * allow you to pass part of a full token stream to the parser, such as for syntax checking
 * in editors and so on.
 *
 */
// [The "BSD license"]
// Copyright (c) 2005-2009 Gokulakannan Somasundaram.
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. The name of the author may not be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF

// clang-format off
#ifndef	_WNScriptASTParser_H
#define _WNScriptASTParser_H



#include <antlr3.hpp>








    #include "WNCore/inc/base.h"
#ifdef _WN_MSVC
    #pragma warning(disable: 4100)
    #pragma warning(disable: 4459)
    #pragma warning(disable: 4703)
#endif
#ifdef _WN_CLANG
    #pragma clang diagnostic ignored "-Wunused-function"
#endif
#ifdef _WN_GCC
    #pragma GCC diagnostic ignored "-Wunused-function"
    #pragma GCC diagnostic ignored "-Wunused-variable"
    #pragma GCC diagnostic ignored "-Wunused-but-set-variable"
    #pragma GCC diagnostic ignored "-Wsign-compare"
#endif
    #include "WNMemory/inc/allocator.h"
    #include "WNScripting/src/WNScriptASTLexer.hpp"
    #include "WNScripting/inc/WNNodeTypes.h"

    using namespace wn;

    #ifndef SET_LOCATION
    #define SET_LOCATION(node, tok) { scripting::source_location loc; \
                                     loc.m_start_index = tok->get_startIndex();\
                                     loc.m_end_index = tok->get_stopIndex();\
                                     loc.m_line_start = tok->get_lineStart();\
                                     loc.m_line_number = tok->get_line();\
                                     loc.m_char_number = tok->get_charPositionInLine();\
                                     node->set_start_location(loc); \
                                   }

    #define SET_LOCATION_FROM_NODE(node, node2) { \
                                        if(node2) { \
                                        node->set_start_location(node2->get_start_location()); \
                                        } \
                                   }
    #define SET_START_LOCATION_FROM_NODE(node, node2) { \
                                        if(node2) { \
                                        scripting::source_location loc = node->get_start_location(); \
                                        scripting::source_location loc2 = node2->get_start_location(); \
                                        loc.m_start_index = loc2.m_start_index; \
                                        loc.m_line_start = loc2.m_line_start; \
                                        loc.m_line_number = loc2.m_line_number; \
                                        loc.m_char_number = loc2.m_char_number; \
                                        node->set_start_location(loc); \
                                        } \
                                    }
    #define SET_END_LOCATION(node, tok) { \
                                        node->set_end_index(tok->get_stopIndex()); \
                                      }
    #define SET_END_LOCATION_FROM_NODE(node, node2) { \
                                            if(node2) { \
                                            node->set_end_index(node2->get_start_location().m_end_index); \
                                            } \
                                            }
    #endif


#ifdef	WIN32
#pragma warning( disable : 4100 )
#pragma warning( disable : 4101 )
#pragma warning( disable : 4127 )
#pragma warning( disable : 4189 )
#pragma warning( disable : 4505 )
#pragma warning( disable : 4701 )
#endif

typedef WNScriptASTParserTraits WNScriptASTParserImplTraits;


class WNScriptASTParserTokens
{
public:







	enum Tokens
	{
		EOF_TOKEN = WNScriptASTParserImplTraits::CommonTokenType::TOKEN_EOF
		, T__56 = 56
		, T__57 = 57
		, T__58 = 58
		, T__59 = 59
		, T__60 = 60
		, T__61 = 61
		, T__62 = 62
		, T__63 = 63
		, T__64 = 64
		, T__65 = 65
		, T__66 = 66
		, T__67 = 67
		, T__68 = 68
		, T__69 = 69
		, T__70 = 70
		, T__71 = 71
		, T__72 = 72
		, T__73 = 73
		, T__74 = 74
		, T__75 = 75
		, BOOL = 4
		, BOOL_TYPE = 5
		, BREAK = 6
		, CHAR = 7
		, CHAR_TYPE = 8
		, CLASS = 9
		, COLON = 10
		, COMMA = 11
		, COMMENT = 12
		, CONTINUE = 13
		, DEFAULT = 14
		, DLSQBRACKET = 15
		, DO = 16
		, DOUBDEC = 17
		, DOUBINC = 18
		, DRSQBRACKET = 19
		, ELSE = 20
		, ESC_SEQ = 21
		, EXPONENT = 22
		, FLOAT = 23
		, FLOAT_TYPE = 24
		, FOR = 25
		, HEX_DIGIT = 26
		, ID = 27
		, IF = 28
		, INCLUDE = 29
		, INT = 30
		, INT_TYPE = 31
		, LBRACE = 32
		, LBRACKET = 33
		, LENGTH = 34
		, LSQBRACKET = 35
		, NULLTOK = 36
		, OCTAL_ESC = 37
		, OVERRIDE = 38
		, QUESTION = 39
		, RBRACE = 40
		, RBRACKET = 41
		, RETURN = 42
		, RSQBRACKET = 43
		, SEMICOLON = 44
		, SHARED_REF = 45
		, STRING = 46
		, STRING_TYPE = 47
		, STRUCT = 48
		, TYPE = 49
		, UNICODE_ESC = 50
		, VIRTUAL = 51
		, VOID_TYPE = 52
		, WEAK_REF = 53
		, WHILE = 54
		, WS = 55
	};

};



class WNScriptASTParser : public WNScriptASTParserImplTraits::BaseParserType, public WNScriptASTParserTokens
{
public:
	typedef WNScriptASTParserImplTraits ImplTraits;
	typedef WNScriptASTParser ComponentType;
	typedef ComponentType::StreamType StreamType;
	typedef WNScriptASTParserImplTraits::BaseParserType BaseType;
	typedef ImplTraits::RecognizerSharedStateType<StreamType> RecognizerSharedStateType;
	typedef StreamType InputType;
	static const bool IsFiltered = false;


private:
public:
    WNScriptASTParser(InputType* instream);
    WNScriptASTParser(InputType* instream, RecognizerSharedStateType* state);

    void init(InputType* instream  );


    private:
      wn::memory::allocator* m_allocator;
    public:
      void set_allocator(wn::memory::allocator* _allocator) {
        m_allocator = _allocator;
      }


     scripting::type* objectType ();
     scripting::type* scalarType ();
     scripting::type* arrayType ();
     scripting::type* dynamicArrayType ();
     scripting::type* compoundType ();
     scripting::type* nonArrayType ();
     scripting::type* type ();
     scripting::parameter* param ();
     scripting::parameter_list* paramList ();
     scripting::parameter_list* parameterList ();
     scripting::assign_type assign_type ();
     scripting::arg_list* arglist ();
     scripting::expression* expr ();
     scripting::expression* cond_ex ();
     scripting::expression* or_ex ();
     scripting::expression* and_ex ();
     scripting::expression* eq_ex ();
     scripting::expression* rel_ex ();
     scripting::expression* add_ex ();
     scripting::expression* mult_ex ();
     scripting::expression* unary_ex ();
     scripting::post_expression* post_ex_proper ();
     scripting::expression* post_ex ();
     scripting::instruction* assignment_or_expression ();
     scripting::constant_expression* constant ();
     scripting::builtin_unary_expression* builtin_unary ();
     scripting::expression * prim_ex ();
     scripting::cast_expression* cast ();
     scripting::struct_allocation_expression* structInit ();
     scripting::array_allocation_expression* arrayInit ();
     scripting::dynamic_array_allocation_expression* dynamicArrayInit ();
     scripting::declaration* declaration ();
     scripting::declaration* inherited_declaration ();
     scripting::instruction* instructionScalar ();
     scripting::continue_instruction* continueInst ();
     scripting::break_instruction* breakInst ();
     scripting::return_instruction* returnInst ();
     scripting::instruction* whileInst ();
     scripting::instruction* doInst ();
     scripting::for_instruction* forInst ();
     scripting::else_if_instruction* elsemiddle ();
     scripting::instruction_list* endif ();
     scripting::if_instruction* ifInst ();
     scripting::instruction* instruction ();
     scripting::instruction_list* instruction_list ();
     scripting::function* function ();
     scripting::struct_definition* structDecl ();
     scripting::struct_definition* classDecl ();
     const char* inc ();
     scripting::script_file* program ();

    const char *    getGrammarFileName();
    void            reset();
    ~WNScriptASTParser();

};







#endif


