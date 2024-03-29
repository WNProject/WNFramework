// clang-format off
/** \file
 *  This C++ header file was generated by $ANTLR version 3.5.2
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


    #include "core/inc/base.h"
#ifdef _WN_MSVC
    #pragma warning(disable: 4100)
    #pragma warning(disable: 4459)
    #pragma warning(disable: 4703)
    #pragma warning(disable: 4114)
#endif
#ifdef _WN_CLANG
    #pragma clang diagnostic ignored "-Wparentheses-equality"
    #pragma clang diagnostic ignored "-Wtautological-undefined-compare"
    #pragma clang diagnostic ignored "-Wunused-variable"
    #pragma clang diagnostic ignored "-Wtautological-compare"
    #pragma clang diagnostic ignored "-Wc++11-narrowing"
#endif
#ifdef _WN_GCC
    #pragma GCC diagnostic ignored "-Wunused-variable"
    #pragma GCC diagnostic ignored "-Wsign-compare"
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
		, T__71 = 71
		, T__72 = 72
		, T__73 = 73
		, T__74 = 74
		, T__75 = 75
		, T__76 = 76
		, T__77 = 77
		, T__78 = 78
		, T__79 = 79
		, T__80 = 80
		, T__81 = 81
		, T__82 = 82
		, T__83 = 83
		, T__84 = 84
		, T__85 = 85
		, T__86 = 86
		, T__87 = 87
		, T__88 = 88
		, T__89 = 89
		, T__90 = 90
		, T__91 = 91
		, T__92 = 92
		, ACTION = 4
		, ACTOR = 5
		, AT = 6
		, BACKGROUND = 7
		, BOOL = 8
		, BOOL_TYPE = 9
		, BREAK = 10
		, CHAR = 11
		, CHAR_TYPE = 12
		, CLASS = 13
		, COLON = 14
		, COMMA = 15
		, COMMENT = 16
		, CONTINUE = 17
		, CPTR_TYPE = 18
		, DEFAULT = 19
		, DEFER = 20
		, DLSQBRACKET = 21
		, DO = 22
		, DOUBDEC = 23
		, DOUBINC = 24
		, DRSQBRACKET = 25
		, ELSE = 26
		, ESC_SEQ = 27
		, EXPONENT = 28
		, FLOAT = 29
		, FLOAT_TYPE = 30
		, FOR = 31
		, HEX_DIGIT = 32
		, ID = 33
		, IF = 34
		, INCLUDE = 35
		, INT = 36
		, INT_TYPE = 37
		, LBRACE = 38
		, LBRACKET = 39
		, LENGTH = 40
		, LOW_PRIORITY = 41
		, LSQBRACKET = 42
		, NULLTOK = 43
		, OCTAL_ESC = 44
		, OVERRIDE = 45
		, QUESTION = 46
		, RBRACE = 47
		, RBRACKET = 48
		, RESOURCE = 49
		, RETURN = 50
		, RSQBRACKET = 51
		, SEMICOLON = 52
		, SHARED_REF = 53
		, SIZE = 54
		, SIZE_TYPE = 55
		, SLICETOK = 56
		, SSOR = 57
		, STRING = 58
		, STRING_TYPE = 59
		, STRUCT = 60
		, SYNCHRONIZED = 61
		, THIS_TOK = 62
		, TYPE = 63
		, T_TYPE = 64
		, UNICODE_ESC = 65
		, VIRTUAL = 66
		, VOID_TYPE = 67
		, WEAK_REF = 68
		, WHILE = 69
		, WS = 70
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



    void  mT__71( );
    void  mT__72( );
    void  mT__73( );
    void  mT__74( );
    void  mT__75( );
    void  mT__76( );
    void  mT__77( );
    void  mT__78( );
    void  mT__79( );
    void  mT__80( );
    void  mT__81( );
    void  mT__82( );
    void  mT__83( );
    void  mT__84( );
    void  mT__85( );
    void  mT__86( );
    void  mT__87( );
    void  mT__88( );
    void  mT__89( );
    void  mT__90( );
    void  mT__91( );
    void  mT__92( );
    void  mLSQBRACKET( );
    void  mRSQBRACKET( );
    void  mDLSQBRACKET( );
    void  mDRSQBRACKET( );
    void  mSLICETOK( );
    void  mSSOR( );
    void  mLENGTH( );
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
    void  mCPTR_TYPE( );
    void  mFLOAT_TYPE( );
    void  mBOOL_TYPE( );
    void  mSTRING_TYPE( );
    void  mCHAR_TYPE( );
    void  mSIZE_TYPE( );
    void  mQUESTION( );
    void  mWEAK_REF( );
    void  mSHARED_REF( );
    void  mDEFAULT( );
    void  mNULLTOK( );
    void  mAT( );
    void  mDEFER( );
    void  mLOW_PRIORITY( );
    void  mBACKGROUND( );
    void  mTHIS_TOK( );
    void  mSYNCHRONIZED( );
    void  mACTION( );
    void  mACTOR( );
    void  mBOOL( );
    void  mID( );
    void  mRESOURCE( );
    void  mTYPE( );
    void  mINT( );
    void  mSIZE( );
    void  mFLOAT( );
    void  mCOMMENT( );
    void  mWS( );
    void  mSTRING( );
    void  mCHAR( );
    void  mT_TYPE( );
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


