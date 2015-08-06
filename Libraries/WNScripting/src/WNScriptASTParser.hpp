/** \file
 *  This C++ header file was generated by $ANTLR version 3.5.3-SNAPSHOT
 *
 *     -  From the grammar source file : C:\\Users\\Andre\\Documents\\Development\\WNFramework\\Libraries\\WNScripting\\lang\\WNScriptAST.g
 *     -                            On : 2015-08-03 21:13:57
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
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef	_WNScriptASTParser_H
#define _WNScriptASTParser_H
/* =============================================================================
 * Standard antlr3 C++ runtime definitions
 */
#include <antlr3.hpp>

/* End of standard antlr 3 runtime definitions
 * =============================================================================
 */




    #include "WNCore/inc/WNBase.h"
#ifdef _WN_MSVC
    #pragma warning(disable: 4100)
    #pragma warning(disable: 4459)
    #pragma warning(disable: 4703)
#endif
    #include "WNMemory/inc/WNAllocator.h"
	#include "WNScripting/src/WNScriptASTLexer.hpp"
    #include "WNScripting/inc/WNNodeTypes.h"
	
    using namespace wn;
    //
    //

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
// Disable: Unreferenced parameter,							- Rules with parameters that are not used
//          constant conditional,							- ANTLR realizes that a prediction is always true (synpred usually)
//          initialized but unused variable					- tree rewrite variables declared but not needed
//          Unreferenced local variable						- lexer rule declares but does not always use _type
//          potentially unitialized variable used			- retval always returned from a rule
//			unreferenced local function has been removed	- susually getTokenNames or freeScope, they can go without warnigns
//
// These are only really displayed at warning level /W4 but that is the code ideal I am aiming at
// and the codegen must generate some of these warnings by necessity, apart from 4100, which is
// usually generated when a parser rule is given a parameter that it does not use. Mostly though
// this is a matter of orthogonality hence I disable that one.
//
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
	/** Symbolic definitions of all the tokens that the parser will work with.
	 *
	 * Antlr will define EOF, but we can't use that as it it is too common in
	 * in C header files and that would be confusing. There is no way to filter this out at the moment
	 * so we just undef it here for now. That isn't the value we get back from C recognizers
	 * anyway. We are looking for ANTLR_TOKEN_EOF.
	 */
	enum Tokens 
	{
		EOF_TOKEN = WNScriptASTParserImplTraits::CommonTokenType::TOKEN_EOF
		, T__43 = 43 
		, T__44 = 44 
		, T__45 = 45 
		, T__46 = 46 
		, T__47 = 47 
		, T__48 = 48 
		, T__49 = 49 
		, T__50 = 50 
		, T__51 = 51 
		, T__52 = 52 
		, T__53 = 53 
		, T__54 = 54 
		, T__55 = 55 
		, T__56 = 56 
		, T__57 = 57 
		, T__58 = 58 
		, T__59 = 59 
		, T__60 = 60 
		, T__61 = 61 
		, T__62 = 62 
		, T__63 = 63 
		, BOOL = 4 
		, CHAR = 5 
		, CHOWN = 6 
		, CLASS = 7 
		, COLON = 8 
		, COMMA = 9 
		, COMMENT = 10 
		, DO = 11 
		, DOUBDEC = 12 
		, DOUBINC = 13 
		, ELSE = 14 
		, ESC_SEQ = 15 
		, EXPONENT = 16 
		, FLOAT = 17 
		, FOR = 18 
		, HEX_DIGIT = 19 
		, ID = 20 
		, IF = 21 
		, INCLUDE = 22 
		, INT = 23 
		, LBRACE = 24 
		, LBRACKET = 25 
		, LSQBRACKET = 26 
		, NULLTOK = 27 
		, OCTAL_ESC = 28 
		, OVERRIDE = 29 
		, RBRACE = 30 
		, RBRACKET = 31 
		, RETURN = 32 
		, RETURN_OWN = 33 
		, RSQBRACKET = 34 
		, SEMICOLON = 35 
		, STRING = 36 
		, STRUCT = 37 
		, TYPE = 38 
		, UNICODE_ESC = 39 
		, VIRTUAL = 40 
		, WHILE = 41 
		, WS = 42 
	};

};

/** Context tracking structure for WNScriptASTParser
 */
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


     scripting::type_node* scalarType (); 
     scripting::type_node* type (); 
     scripting::declaration* decl (); 
     scripting::decl_list* declList (); 
     scripting::decl_list* parameterList (); 
     scripting::assign_type assign_type (); 
     scripting::lvalue* lvalue (); 
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
     scripting::assignment_instruction* assignment (); 
     scripting::constant_expression* constant (); 
     scripting::expression * prim_ex (); 
     scripting::struct_allocation_expression* structInit (); 
     scripting::array_allocation_expression* arrayInit (); 
     scripting::declaration* declaration (); 
     scripting::instruction* instructionScalar (); 
     scripting::return_instruction* returnInst (); 
     scripting::instruction* whileInst (); 
     scripting::instruction* doInst (); 
     scripting::for_instruction* forInst (); 
     scripting::else_if_instruction* elsemiddle (); 
     scripting::instruction_list* endif (); 
     scripting::if_instruction* ifInst (); 
     scripting::instruction* instruction (); 
     scripting::instruction_list* instruction_list (); 
     scripting::instruction_list* body (); 
     scripting::function* function (); 
     scripting::struct_definition* structDecl (); 
     scripting::struct_definition* classDecl (); 
     const wn_char* inc (); 
     scripting::script_file* program (); 
    // Delegated rules

    const char *    getGrammarFileName();
    void            reset();
    ~WNScriptASTParser();

};

// Function protoypes for the constructor functions that external translation units
// such as delegators and delegates may wish to call.
//

/* End of token definitions for WNScriptASTParser
 * =============================================================================
 */


#endif

/* END - Note:Keep extra line feed to satisfy UNIX systems */
