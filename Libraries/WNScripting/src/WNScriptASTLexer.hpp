/** \file
 *  This C++ header file was generated by $ANTLR version 3.5.3-SNAPSHOT
 *
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

// clang-format off
#ifndef	_WNScriptASTLexer_H
#define _WNScriptASTLexer_H



#include <antlr3.hpp>








#ifdef	WIN32
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
    #pragma clang diagnostic ignored "-Wc++11-narrowing"
#endif

    template<class ImplTraits>
    class UserTraits : public antlr3::CustomTraitsBase<ImplTraits>
    {
      public:
    };


       class WNScriptASTLexer;
       class WNScriptASTParser;
       typedef antlr3::Traits< WNScriptASTLexer, WNScriptASTParser > WNScriptASTLexerTraits;
       typedef WNScriptASTLexerTraits WNScriptASTParserTraits;


typedef WNScriptASTLexerTraits WNScriptASTLexerImplTraits;


class WNScriptASTLexerTokens
{
public:







	enum Tokens
	{
		EOF_TOKEN = WNScriptASTLexerImplTraits::CommonTokenType::TOKEN_EOF
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
		, T__70 = 70
		, T__71 = 71
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
		, DO = 14
		, DOUBDEC = 15
		, DOUBINC = 16
		, ELSE = 17
		, ESC_SEQ = 18
		, EXPONENT = 19
		, FLOAT = 20
		, FLOAT_TYPE = 21
		, FOR = 22
		, HEX_DIGIT = 23
		, ID = 24
		, IF = 25
		, INCLUDE = 26
		, INT = 27
		, INT_TYPE = 28
		, LBRACE = 29
		, LBRACKET = 30
		, LSQBRACKET = 31
		, NULLTOK = 32
		, OCTAL_ESC = 33
		, OVERRIDE = 34
		, QUESTION = 35
		, RBRACE = 36
		, RBRACKET = 37
		, RETURN = 38
		, RSQBRACKET = 39
		, SEMICOLON = 40
		, SHARED_REF = 41
		, STRING = 42
		, STRING_TYPE = 43
		, STRUCT = 44
		, TYPE = 45
		, UNICODE_ESC = 46
		, VIRTUAL = 47
		, VOID_TYPE = 48
		, WEAK_REF = 49
		, WHILE = 50
		, WS = 51
	};

};



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
    void  mT__70( );
    void  mT__71( );
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
    void  mBREAK( );
    void  mCONTINUE( );
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
    void  mQUESTION( );
    void  mWEAK_REF( );
    void  mSHARED_REF( );
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







#endif


