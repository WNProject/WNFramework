/** \file
 *  This C++ header file was generated by $ANTLR version 3.5.3-SNAPSHOT
 *
 *     -  From the grammar source file : C:\\Users\\Andre\\Documents\\Development\\WNFramework\\Libraries\\WNScripting\\lang\\WNScriptAST.g
 *     -                            On : 2015-12-10 22:42:01
 *     -                 for the lexer : WNScriptASTLexerLexer
 *
 * Editing it, at least manually, is not wise.
 *
 * C++ language generator and runtime by Gokulakannan Somasundaram ( heavy lifting from C Run-time by Jim Idle )
 *
 *
 * The lexer WNScriptASTLexer has the callable functions (rules) shown below,
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

#ifndef	_WNScriptASTLexer_H
#define _WNScriptASTLexer_H
/* =============================================================================
 * Standard antlr3 C++ runtime definitions
 */
#include <antlr3.hpp>

/* End of standard antlr 3 runtime definitions
 * =============================================================================
 */




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


    #include "WNCore/inc/WNBase.h"
#ifdef _WN_MSVC
    #pragma warning(disable: 4100)
    #pragma warning(disable: 4459)
    #pragma warning(disable: 4703)
#endif
#ifdef _WN_CLANG
    #pragma clang diagnostic ignored "-Wparentheses-equality"
    #pragma clang diagnostic ignored "-Wtautological-compare"
#endif
    //class WNScriptASTLexer;
    //class WNScriptASTParser;

    template<class ImplTraits>
    class UserTraits : public antlr3::CustomTraitsBase<ImplTraits>
    {
      public:
        //for using the token stream which deleted the tokens, once it is reduced to a rule
        //but it leaves the start and stop tokens. So they can be accessed as usual
    //    static const bool TOKENS_ACCESSED_FROM_OWNING_RULE = true;
    };

    //typedef antlr3::Traits< WNScriptASTLexer, WNScriptASTParser, UserTraits > WNScriptASTLexerTraits;
    //typedef WNScriptASTLexerTraits WNScriptASTParserTraits;

    // If you don't want the override it is like this.
       class WNScriptASTLexer;
       class WNScriptASTParser;
       typedef antlr3::Traits< WNScriptASTLexer, WNScriptASTParser > WNScriptASTLexerTraits;
       typedef WNScriptASTLexerTraits WNScriptASTParserTraits;


typedef WNScriptASTLexerTraits WNScriptASTLexerImplTraits;


class WNScriptASTLexerTokens
{
public:
	/** Symbolic definitions of all the tokens that the lexer will work with.
	 *
	 * Antlr will define EOF, but we can't use that as it it is too common in
	 * in C header files and that would be confusing. There is no way to filter this out at the moment
	 * so we just undef it here for now. That isn't the value we get back from C recognizers
	 * anyway. We are looking for ANTLR_TOKEN_EOF.
	 */
	enum Tokens 
	{
		EOF_TOKEN = WNScriptASTLexerImplTraits::CommonTokenType::TOKEN_EOF
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
		, T__64 = 64 
		, T__65 = 65 
		, T__66 = 66 
		, T__67 = 67 
		, T__68 = 68 
		, T__69 = 69 
		, BOOL = 4 
		, BOOL_TYPE = 5 
		, CHAR = 6 
		, CHAR_TYPE = 7 
		, CHOWN = 8 
		, CLASS = 9 
		, COLON = 10 
		, COMMA = 11 
		, COMMENT = 12 
		, DO = 13 
		, DOUBDEC = 14 
		, DOUBINC = 15 
		, ELSE = 16 
		, ESC_SEQ = 17 
		, EXPONENT = 18 
		, FLOAT = 19 
		, FLOAT_TYPE = 20 
		, FOR = 21 
		, HEX_DIGIT = 22 
		, ID = 23 
		, IF = 24 
		, INCLUDE = 25 
		, INT = 26 
		, INT_TYPE = 27 
		, LBRACE = 28 
		, LBRACKET = 29 
		, LSQBRACKET = 30 
		, NULLTOK = 31 
		, OCTAL_ESC = 32 
		, OVERRIDE = 33 
		, RBRACE = 34 
		, RBRACKET = 35 
		, RETURN = 36 
		, RETURN_OWN = 37 
		, RSQBRACKET = 38 
		, SEMICOLON = 39 
		, STRING = 40 
		, STRING_TYPE = 41 
		, STRUCT = 42 
		, TYPE = 43 
		, UNICODE_ESC = 44 
		, VIRTUAL = 45 
		, VOID_TYPE = 46 
		, WHILE = 47 
		, WS = 48 
	};

};

/** Context tracking structure for WNScriptASTLexer
 */
class WNScriptASTLexer : public WNScriptASTLexerImplTraits::BaseLexerType, public WNScriptASTLexerTokens
{
public:
	typedef WNScriptASTLexerImplTraits ImplTraits;
	typedef WNScriptASTLexer ComponentType;
	typedef ComponentType::StreamType StreamType;
	typedef WNScriptASTLexerImplTraits::BaseLexerType BaseType;
	typedef ImplTraits::RecognizerSharedStateType<StreamType> RecognizerSharedStateType;
	typedef StreamType InputType;
	static const bool IsFiltered = false;


private:	
public:
    WNScriptASTLexer(InputType* instream);
    WNScriptASTLexer(InputType* instream, RecognizerSharedStateType* state);

    void init(InputType* instream  );


    
    void  mT__49( );
    void  mT__50( );
    void  mT__51( );
    void  mT__52( );
    void  mT__53( );
    void  mT__54( );
    void  mT__55( );
    void  mT__56( );
    void  mT__57( );
    void  mT__58( );
    void  mT__59( );
    void  mT__60( );
    void  mT__61( );
    void  mT__62( );
    void  mT__63( );
    void  mT__64( );
    void  mT__65( );
    void  mT__66( );
    void  mT__67( );
    void  mT__68( );
    void  mT__69( );
    void  mLSQBRACKET( );
    void  mRSQBRACKET( );
    void  mLBRACKET( );
    void  mRBRACKET( );
    void  mLBRACE( );
    void  mRBRACE( );
    void  mCOMMA( );
    void  mDOUBINC( );
    void  mDOUBDEC( );
    void  mRETURN( );
    void  mRETURN_OWN( );
    void  mSEMICOLON( );
    void  mCOLON( );
    void  mWHILE( );
    void  mDO( );
    void  mIF( );
    void  mELSE( );
    void  mFOR( );
    void  mSTRUCT( );
    void  mCLASS( );
    void  mVIRTUAL( );
    void  mOVERRIDE( );
    void  mINCLUDE( );
    void  mVOID_TYPE( );
    void  mINT_TYPE( );
    void  mFLOAT_TYPE( );
    void  mBOOL_TYPE( );
    void  mSTRING_TYPE( );
    void  mCHAR_TYPE( );
    void  mBOOL( );
    void  mID( );
    void  mTYPE( );
    void  mINT( );
    void  mFLOAT( );
    void  mCOMMENT( );
    void  mWS( );
    void  mSTRING( );
    void  mCHAR( );
    void  mNULLTOK( );
    void  mCHOWN( );
    void  mEXPONENT( );
    void  mHEX_DIGIT( );
    void  mESC_SEQ( );
    void  mOCTAL_ESC( );
    void  mUNICODE_ESC( );
    void  mTokens( );
    const char *    getGrammarFileName();
    void            reset();
    ~WNScriptASTLexer();

};

// Function protoypes for the constructor functions that external translation units
// such as delegators and delegates may wish to call.
//

/* End of token definitions for WNScriptASTLexer
 * =============================================================================
 */


#endif

/* END - Note:Keep extra line feed to satisfy UNIX systems */