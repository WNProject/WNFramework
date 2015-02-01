grammar WNScriptAST;

options {
    language=Cpp;
}

tokens
{
    LSQBRACKET;
    RSQBRACKET;
    LBRACKET;
    RBRACKET;
    LBRACE;
    RBRACE;
    COMMA;
    DOUBINC;
    DOUBDEC;
    RETURN;
    RETURN_OWN;
    SEMICOLON;
    COLON;
    WHILE;
    DO;
    IF;
    ELSE;
    FOR;    
    STRUCT; 
    CLASS;  
    INCLUDE;
    ID;
    INT;
    FLOAT;
    COMMENT;
    WS;
    STRING;
    CHAR;
    NULLTOK;
    CHOWN;
    VIRTUAL;
    OVERRIDE;
}


@header {
    #include "WNCore/inc/WNBase.h"
#ifdef _WN_MSVC
    #pragma warning(disable: 4703)
#endif
    #include "WNScripting/src/WNScriptASTLexer.hpp"
    #include "WNScripting/inc/WNScriptingAllocations.h"
    #include "WNScripting/inc/WNArgList.h"
    #include "WNScripting/inc/WNArrayAccessExpression.h"
    #include "WNScripting/inc/WNArrayAllocation.h"
    #include "WNScripting/inc/WNAssignment.h"
    #include "WNScripting/inc/WNBinExpression.h"
    #include "WNScripting/inc/WNCondExpression.h"
    #include "WNScripting/inc/WNConstantExpression.h"
    #include "WNScripting/inc/WNDeclaration.h"
    #include "WNScripting/inc/WNDeclList.h"
    #include "WNScripting/inc/WNDoInstruction.h"
    #include "WNScripting/inc/WNExpression.h"
    #include "WNScripting/inc/WNForInstruction.h"
    #include "WNScripting/inc/WNFunction.h"
    #include "WNScripting/inc/WNFunctionCallExpression.h"
    #include "WNScripting/inc/WNIDExpression.h"
    #include "WNScripting/inc/WNIfInstruction.h"
    #include "WNScripting/inc/WNInstruction.h"
    #include "WNScripting/inc/WNInstructionList.h"
    #include "WNScripting/inc/WNLValue.h"
    #include "WNScripting/inc/WNMemberAccessExpression.h"
    #include "WNScripting/inc/WNNode.h"
    #include "WNScripting/inc/WNNullAllocation.h"
    #include "WNScripting/inc/WNPostExpression.h"
    #include "WNScripting/inc/WNPostUNExpression.h"
    #include "WNScripting/inc/WNScriptFile.h"
    #include "WNScripting/inc/WNSSExpression.h"
    #include "WNScripting/inc/WNStruct.h"
    #include "WNScripting/inc/WNStructAllocation.h"
    #include "WNScripting/inc/WNTypeNode.h"
    #include "WNScripting/inc/WNUnExpression.h"
    #include "WNScripting/inc/WNWhileInstruction.h"
    #include "WNScripting/inc/WNReturn.h"
    using namespace WNScripting;
    //
    //

    #ifndef SetLocation 
    #define SetLocation(node, tok) { WNScriptLocation loc; \
                                     loc.mStartIndex = tok->get_startIndex();\
                                     loc.mEndIndex = tok->get_stopIndex();\
                                     loc.mLineStart = tok->get_lineStart();\
                                     loc.mLineNumber = tok->get_line();\
                                     loc.mCharNumber = tok->get_charPositionInLine();\
                                     node->SetStartPosition(loc); \
                                   }
    #define SetLocationFromNode(node, node2) { \
                                        if(node2) { \
                                        node->SetStartPosition(node2->GetStartPosition()); \
                                        } \
                                   }
    #define SetStartLocationFromNode(node, node2) { \
                                        if(node2) { \
                                        WNScriptLocation loc = node->GetStartPosition(); \
                                        WNScriptLocation loc2 = node2->GetStartPosition(); \
                                        loc.mStartIndex = loc2.mStartIndex; \
                                        loc.mLineStart = loc2.mLineStart; \
                                        loc.mLineNumber = loc2.mLineNumber; \
                                        loc.mCharNumber = loc2.mCharNumber; \
                                        node->SetStartPosition(loc); \
                                        } \
                                    }
    #define SetEndLocation(node, tok) { \
                                        node->SetEndIndex(tok->get_stopIndex()); \
                                      }
    #define SetEndLocationFromNode(node, node2) { \
                                            if(node2) { \
                                            node->SetEndIndex(node2->GetStartPosition().mEndIndex); \
                                            } \
                                            }
    #endif
}

@lexer::traits 
{
    #include "WNCore/inc/WNBase.h"
#ifdef _WN_MSVC
    #pragma warning(disable: 4100) 
#endif
#ifdef _WN_CLANG
    #pragma clang diagnostic ignored "-Wparentheses-equality"
    #pragma clang diagnostic ignored "-Wtautological-compare"
#endif
    //class WNScriptASTLexer; 
    //class WNScriptASTParser; 

    //template<class ImplTraits>
    //class UserTraits : public antlr3::CustomTraitsBase<ImplTraits>
    //{
    //public:
        //for using the token stream which deleted the tokens, once it is reduced to a rule
        //but it leaves the start and stop tokens. So they can be accessed as usual
    //    static const bool TOKENS_ACCESSED_FROM_OWNING_RULE = true;
    //};

    //typedef antlr3::Traits< WNScriptASTLexer, WNScriptASTParser, UserTraits > WNScriptASTLexerTraits;
    //typedef WNScriptASTLexerTraits WNScriptASTParserTraits;

    // If you don't want the override it is like this.
       class WNScriptASTLexer;
       class WNScriptASTParser;
       typedef antlr3::Traits< WNScriptASTLexer, WNScriptASTParser > WNScriptASTLexerTraits;
       typedef WNScriptASTLexerTraits WNScriptASTParserTraits;
    
}

LSQBRACKET: '[';
RSQBRACKET: ']';   
LBRACKET: '(';
RBRACKET: ')';
LBRACE:   '{';
RBRACE:   '}';
COMMA:    ',';
DOUBINC:   '++';
DOUBDEC:   '--';
RETURN:    'return';
RETURN_OWN: 'return<==';
SEMICOLON: ';';
COLON:     ':';
WHILE:     'while';
DO:        'do';
IF:        'if';
ELSE:      'else';
FOR:       'for';
STRUCT:    'struct';
CLASS:     'class';
VIRTUAL:   'virtual';
OVERRIDE:  'override';
INCLUDE:   '#include';

BOOL :  'true' | 'false';

ID  :    ('a'..'z'|'_') ('a'..'z'|'A'..'Z'|'0'..'9'|'_')*
    ;

TYPE  :    ('A'..'Z') ('a'..'z'|'A'..'Z'|'0'..'9'|'_')*
    ;
    
INT :    '0'..'9'+
    ;

FLOAT
    :   ('0'..'9')+ '.' ('0'..'9')* EXPONENT?
    |   '.' ('0'..'9')+ EXPONENT?
    |   ('0'..'9')+ EXPONENT
    ; 

COMMENT
    :   '//' ~('\n'|'\r')* '\r'? '\n' {$channel=HIDDEN;}
    |   '/*' ( options {greedy=false;} : . )* '*/' {$channel=HIDDEN;}
    ;

WS  :   ( ' '
        | '\t'
        | '\r'
        | '\n'
        ) {$channel=HIDDEN;}
    ;

STRING
    :  '"' ( ESC_SEQ | ~('\\'|'"') )* '"'
    ;

CHAR:  '\'' ( ESC_SEQ | ~('\''|'\\') ) '\''
    ;

NULLTOK: '~'
    ;

CHOWN: '<=='
    ;

fragment
EXPONENT : ('e'|'E') ('+'|'-')? ('0'..'9')+ ;

fragment
HEX_DIGIT : ('0'..'9'|'a'..'f'|'A'..'F') ;

fragment
ESC_SEQ
    :       UNICODE_ESC
    |   OCTAL_ESC
    |   '\\' ('b'|'t'|'n'|'f'|'r'|'\"'|'\''|'\\')
    ;

fragment
OCTAL_ESC
    :   '\\' ('0'..'3') ('0'..'7') ('0'..'7')
    |   '\\' ('0'..'7') ('0'..'7')
    |   '\\' ('0'..'7')
    ;

fragment
UNICODE_ESC 
    :   '\\' 'u' HEX_DIGIT HEX_DIGIT HEX_DIGIT HEX_DIGIT
    ;
    
scalarType returns[WNTypeNode* node]
@init {
    node = WN_NULL;
}
    :   TYPE    { node = WN_SCRIPTNODE_NEW(WNTypeNode($TYPE.text.c_str())); SetLocation(node, $TYPE); }
    ; 
  
type    returns[WNTypeNode* node]
@init {
    node = WN_NULL;
}
    :    scalarType { node = $scalarType.node; SetLocationFromNode(node, $scalarType.node); }
                (LSQBRACKET RSQBRACKET { node->AddArrayLevel(); SetEndLocation(node, $RSQBRACKET); })*
    ;

decl returns[WNDeclaration* node]
@init {
    node = WN_NULL;
}
    :    type ID {node = WN_SCRIPTNODE_NEW(WNDeclaration($type.node, $ID.text.c_str())); SetLocationFromNode(node, $type.node); SetEndLocation(node, $ID); }
    ;

declList returns[WNDeclList* node]
@init {
    node = WN_NULL;
}
    :    a=decl { node = WN_SCRIPTNODE_NEW(WNDeclList($a.node)); SetLocationFromNode(node, $a.node); }
        (COMMA b=decl { $node->AddDeclaration($b.node); SetEndLocationFromNode(node, $b.node); })*
    ;

parameterList returns[WNDeclList* node]
@init {
    node = WN_NULL;
}
    :    LBRACKET declList RBRACKET { node = $declList.node; SetLocation(node, $LBRACKET); SetEndLocation(node, $RBRACKET); }
        |    LBRACKET RBRACKET      { node = WN_NULL; }
    ;


assign_op returns[WNAssignType node]
@init {
    node = AT_EQ;
}
    :    '='  { node = AT_EQ; }
    |    '+=' { node = AT_ADD_EQ; }
    |    '-=' { node = AT_SUB_EQ; }
    |    '*=' { node = AT_MULT_EQ; }
    |    '/=' { node = AT_DIV_EQ; }
    |    '%=' { node = AT_MOD_EQ; }
    |    '<==' { node = AT_CHOWN; }
    ;
    
lvalue returns[WNLValue* node]    
@init {
    node = WN_NULL;
}
    :    unary_ex { node = WN_SCRIPTNODE_NEW(WNLValue($unary_ex.node)); SetLocationFromNode(node, $unary_ex.node); }
    ;

arglist returns[WNArgList* node]
@init {
    node = WN_SCRIPTNODE_NEW(WNArgList());
}
        :   ( 
                    (a=expression { node->AddExpression($a.node); SetLocationFromNode(node, $a.node); }) 
                |   ('<==' b=expression {node->AddExpression($b.node, WN_TRUE); SetLocationFromNode(node, $b.node); })
            )

            (',' 
                (
                    ( '<==' c=expression { node->AddExpression($c.node); SetEndLocationFromNode(node, $c.node); } ) |
                    ( d=expression { node->AddExpression($d.node, WN_TRUE); SetEndLocationFromNode(node, $d.node); } ) |
                )
            )*
        ;
         
expression returns[WNExpression* node]
@init {
    node = WN_NULL;
}
        :    cond_ex { node = $cond_ex.node; }
        ;
        
cond_ex returns[WNExpression* node]
@init {
    node = WN_NULL;
}
        :    or_ex  { node = $or_ex.node; }

                ('?' b=expression ':' c=cond_ex { node = WN_SCRIPTNODE_NEW(WNCondExpression(node, $b.node, $c.node));  SetLocationFromNode(node, $or_ex.node); SetEndLocationFromNode(node, $c.node); }  )?
        ;
        
or_ex    returns[WNExpression* node]
@init {
    node = WN_NULL;
}
        :    a=and_ex { node = $a.node; }
            ('||' b=and_ex { node = WN_SCRIPTNODE_NEW(WNSSExpression(ST_OR, node, $b.node)); SetLocationFromNode(node, $a.node); SetEndLocationFromNode(node, $b.node); })*
        ;
        
and_ex  returns[WNExpression* node]
@init {
    node = WN_NULL;
}
        :    a=eq_ex { node = $a.node; }
            ('&&' b=eq_ex {node = WN_SCRIPTNODE_NEW(WNSSExpression(ST_AND, node, $b.node)); SetLocationFromNode(node, $a.node); SetEndLocationFromNode(node, $b.node); })*;
eq_ex    returns[WNExpression* node]
@init {
    node = WN_NULL;
}
        :    a=rel_ex { node = $a.node; }
            (    
                ('==' b=rel_ex {node = WN_SCRIPTNODE_NEW(WNBinExpression(AR_EQ, node, $b.node));  SetLocationFromNode(node, $a.node); SetEndLocationFromNode(node, $b.node); } )
              | ('!=' c=rel_ex {node = WN_SCRIPTNODE_NEW(WNBinExpression(AR_NEQ, node, $c.node)); SetLocationFromNode(node, $a.node); SetEndLocationFromNode(node, $c.node); } )
            )*
        ;
            
rel_ex  returns[WNExpression* node]
@init {
    node = WN_NULL;
}
        :    a=add_ex { node=$a.node; }
            (
                ( '<' b=add_ex {node = WN_SCRIPTNODE_NEW(WNBinExpression(AR_LT, node, $b.node));   SetLocationFromNode(node, $a.node); SetEndLocationFromNode(node, $b.node); } )
              | ( '>' c=add_ex {node = WN_SCRIPTNODE_NEW(WNBinExpression(AR_GT, node, $c.node));   SetLocationFromNode(node, $a.node); SetEndLocationFromNode(node, $c.node); } )
              | ( '<=' d=add_ex {node = WN_SCRIPTNODE_NEW(WNBinExpression(AR_LEQ, node, $d.node));   SetLocationFromNode(node, $a.node); SetEndLocationFromNode(node, $d.node); } )
              | ( '>=' e=add_ex {node = WN_SCRIPTNODE_NEW(WNBinExpression(AR_GEQ, node, $e.node));   SetLocationFromNode(node, $a.node); SetEndLocationFromNode(node, $e.node); } )
            )*
        ;

add_ex     returns[WNExpression* node]
@init {
    node = WN_NULL;
}
        :    a=mult_ex {node = $a.node; } 
            (
                ('+' b=mult_ex {node = WN_SCRIPTNODE_NEW(WNBinExpression(AR_ADD, node, $b.node));   SetLocationFromNode(node, $a.node); SetEndLocationFromNode(node, $b.node); })
              | ('-' c=mult_ex {node = WN_SCRIPTNODE_NEW(WNBinExpression(AR_SUB, node, $c.node));   SetLocationFromNode(node, $a.node); SetEndLocationFromNode(node, $c.node); })
            )*
        ;

mult_ex    returns[WNExpression* node]
@init {
    node = WN_NULL;
}
        :    a=unary_ex { node = $a.node; }
            (
                    ('*' b=unary_ex {node = WN_SCRIPTNODE_NEW(WNBinExpression(AR_MULT, node, $b.node));   SetLocationFromNode(node, $a.node); SetEndLocationFromNode(node, $b.node); })
                |   ('/' c=unary_ex {node = WN_SCRIPTNODE_NEW(WNBinExpression(AR_DIV, node, $c.node));   SetLocationFromNode(node, $a.node); SetEndLocationFromNode(node, $c.node); })
                |   ('%' d=unary_ex {node = WN_SCRIPTNODE_NEW(WNBinExpression(AR_MOD, node, $d.node));   SetLocationFromNode(node, $a.node); SetEndLocationFromNode(node, $d.node); })
            )*
        ; 
        
unary_ex returns[WNExpression* node]
@init {
    node = WN_NULL;
}
    :    a=post_ex { node = $a.node; }
    |    '++' b=unary_ex { WNNode* t = node; node = WN_SCRIPTNODE_NEW(WNUNExpression(UN_PREINC, $b.node));   SetLocationFromNode(node, t); SetEndLocationFromNode(node, $b.node); }
    |    '--' c=unary_ex { WNNode* t = node; node = WN_SCRIPTNODE_NEW(WNUNExpression(UN_PREDEC, $c.node));   SetLocationFromNode(node, t); SetEndLocationFromNode(node, $c.node); }
    |    '-' d=unary_ex  { WNNode* t = node; node = WN_SCRIPTNODE_NEW(WNUNExpression(UN_NEG, $d.node));   SetLocationFromNode(node, t); SetEndLocationFromNode(node, $d.node); }
        ;
        

post_ex_proper returns[WNPostExpression* node]
@init {
    node = WN_NULL;
}
    :    d=LSQBRACKET a=expression e=RSQBRACKET { node = WN_SCRIPTNODE_NEW(WNArrayAccessExpr($a.node)); SetLocation(node, $d); SetEndLocation(node, $e); }
    |    f=LBRACKET g=RBRACKET              { node = WN_SCRIPTNODE_NEW(WNFunctionCallExpr()); SetLocation(node, $f); SetEndLocation(node, $g); }
    |    h=LBRACKET b = arglist i=RBRACKET  { node = WN_SCRIPTNODE_NEW(WNFunctionCallExpr($b.node)); SetLocation(node, $h); SetEndLocation(node, $i); }
    |   '.' c=ID              { node = WN_SCRIPTNODE_NEW(WNMemberAccessExpr($c.text.c_str())); SetLocation(node, $c); }
    |     DOUBINC                  { node = WN_SCRIPTNODE_NEW(WNPostUNExpression(UN_POSTINC)); SetLocation(node, $DOUBINC);}
    |   DOUBDEC                  { node = WN_SCRIPTNODE_NEW(WNPostUNExpression(UN_POSTDEC)); SetLocation(node, $DOUBDEC);}
    ;
    
post_ex    returns[WNExpression* node]
@init {
    node = WN_NULL;
}
        :   prim_ex { node = $prim_ex.node; }
            (a=post_ex_proper {$a.node->AddBaseExpr(node); SetEndLocationFromNode($a.node, node); node = $a.node;  } )* ;

assignment returns[WNAssignment* node]
@init {
    node = WN_NULL;
}
    :    lvalue { node = WN_SCRIPTNODE_NEW(WNAssignment($lvalue.node)); SetLocationFromNode(node, $lvalue.node); }
        (assign_op expression { node->AddValue($assign_op.node, $expression.node); SetEndLocationFromNode(node, $expression.node); }  )?
    ;
constant returns[WNConstantExpression* node]
@init {
    node = WN_NULL;
}
    :    INT    { node = WN_SCRIPTNODE_NEW(WNConstantExpression(SC_INT, $INT.text.c_str())); SetLocation(node, $INT); }
    |    FLOAT  { node = WN_SCRIPTNODE_NEW(WNConstantExpression(SC_FLOAT, $FLOAT.text.c_str())); SetLocation(node, $FLOAT); }
    |    CHAR   { node = WN_SCRIPTNODE_NEW(WNConstantExpression(SC_CHAR, $CHAR.text.c_str())); SetLocation(node, $CHAR);}
    |    STRING { node = WN_SCRIPTNODE_NEW(WNConstantExpression(SC_STRING, $STRING.text.c_str())); SetLocation(node, $STRING); }
    |   BOOL   { node = WN_SCRIPTNODE_NEW(WNConstantExpression(SC_BOOL, $BOOL.text.c_str())); SetLocation(node, $BOOL); }
    ;

prim_ex returns[WNExpression * node]
@init {
    node = WN_NULL;
}
    :    ID { node = WN_SCRIPTNODE_NEW(WNIDExpression($ID.text.c_str())); SetLocation(node, $ID);}
    |    ba=LBRACKET a=expression bb=RBRACKET {node = $a.node; SetLocation(node, $ba); SetEndLocation(node, $bb); }
    |    b=constant  {node = $b.node; }
    |   c=scalarType 
          (
              ( e=structInit ) { $e.node->SetType($c.node); node=$e.node; SetStartLocationFromNode(node, $c.node); }
            | ( f=arrayInit )  { $f.node->SetType($c.node); node=$f.node; SetStartLocationFromNode(node, $c.node); }
          )
    |   d=NULLTOK { node= WN_SCRIPTNODE_NEW(WNNullAllocation()); SetLocation(node, $NULLTOK); }
    ;

structInit returns[WNStructAllocation* node] 
    : a=LBRACKET b=RBRACKET { node = WN_SCRIPTNODE_NEW(WNStructAllocation()); SetLocation(node, $a); SetEndLocation(node, $b); }
    | c=LBRACKET d=expression e=RBRACKET { node = WN_SCRIPTNODE_NEW(WNStructAllocation()); node->SetCopyInitializer(d); SetLocation(node, $c); SetEndLocation(node, $e); }
    ;

arrayInit returns[WNArrayAllocation* node]
@init {
    node = WN_SCRIPTNODE_NEW(WNArrayAllocation()); 
}
    :   (
            (
                (LSQBRACKET e=expression RSQBRACKET { node->AddExpression($e.node);} )+
                (LSQBRACKET RSQBRACKET { node->AddLevel(); } )*
            )
            |   (LSQBRACKET RSQBRACKET { node->AddLevel(); } )+
        )
        (   
            ( LBRACKET a=RBRACKET { SetLocation(node, $a); } )
            |
            ( LBRACKET b=expression c=RBRACKET { node->SetCopyInitializer(b); SetLocation(node, $c); } )
        )

    ;

declaration returns[WNDeclaration* node]
@init {
    node = WN_NULL;
}
    :    a=decl { node = $a.node; }
            (
                ('=' ( (c=expression) { node->AddExpressionInitializer($c.node); SetEndLocationFromNode(node, $c.node); } ) )
            |   ('<==' ( (d=expression) { node->AddExpressionInitializer($d.node, true); SetEndLocationFromNode(node, $d.node); } ) ) 
            )
    ;

instructionScalar returns[WNInstruction* node]
@init {
    node = WN_NULL;
}
    :    declaration {node = $declaration.node; }
    |    assignment  {node = $assignment.node; }
    ;

returnInst returns[WNReturn* node]
@init {
    node = WN_NULL;
}
    :   a=RETURN expression b=SEMICOLON { node = WN_SCRIPTNODE_NEW(WNReturn($expression.node)); SetLocation(node, $a); SetEndLocation(node, $b);}
    |   c=RETURN d=SEMICOLON { node = WN_SCRIPTNODE_NEW(WNReturn()); SetLocation(node, $c); SetEndLocation(node, $d); }
    |   e=RETURN_OWN expression f=SEMICOLON { node = WN_SCRIPTNODE_NEW(WNReturn($expression.node, true)); SetLocation(node, $e); SetEndLocation(node, $f); }
    ;
    
whileInst     returns[WNInstruction* node]
@init {
    node = WN_NULL;
}        
        :    WHILE LBRACKET expression RBRACKET body { node = WN_SCRIPTNODE_NEW(WNWhileInstruction($expression.node, $body.node)); SetLocation(node, $WHILE); SetEndLocationFromNode(node, $body.node); }
        ;
doInst    returns[WNInstruction* node]
@init {
    node = WN_NULL;
}
    :    DO body WHILE LBRACKET expression RBRACKET SEMICOLON {node = WN_SCRIPTNODE_NEW(WNDoInstruction($expression.node, $body.node)); SetLocation(node, $DO); SetEndLocation(node, $SEMICOLON); }
    ;

forInst    returns[WNInstruction* node]
@init{
    WNForInstruction* forNode = WN_SCRIPTNODE_NEW(WNForInstruction());
    node = forNode;
}
    :    FOR LBRACKET  { SetLocation(forNode, $FOR); }
                    (a=instructionScalar {forNode->AddInitializer($a.node);})? SEMICOLON
                    (b=expression         {forNode->AddCondition($b.node); })? SEMICOLON 
                    (c=instructionScalar {forNode->AddPostOp($c.node); })? RBRACKET 
                    (d=body                 {forNode->AddBody($d.node); SetEndLocationFromNode(forNode, $d.node); })
    ;
    
    
elsemiddle returns[WNElseIf* node] 
@init {
    node = WN_NULL;
}
    :    ELSE IF LBRACKET expression RBRACKET body { node = WN_SCRIPTNODE_NEW(WNElseIf($expression.node, $body.node)); SetLocation(node, $ELSE); SetEndLocationFromNode(node, $body.node); }
    ;
     
endif    returns[WNInstructionList* node] 
@init {
    node = WN_NULL;
}
    :    'else' body {node = $body.node; }
    ;
    
ifInst returns[WNInstruction* node]
@init {
    WNIFInstruction* inst;
    node = WN_NULL;
}
    :    IF LBRACKET expression RBRACKET body {inst = WN_SCRIPTNODE_NEW(WNIFInstruction($expression.node, $body.node)); node = inst; SetLocation(node, $IF); SetEndLocationFromNode(node, $body.node); } 
            (elsemiddle {inst->AddElseIf($elsemiddle.node); SetEndLocationFromNode(node, $elsemiddle.node); } )* 
            (endif {inst->AddElse($endif.node); SetLocation(node, $IF); SetEndLocationFromNode(node, $endif.node); } )?
    ;

instruction returns [WNInstruction* node]
@init {
}
    :    ifInst         { node = $ifInst.node; }
    |    whileInst     {node = $whileInst.node; }
    |    doInst        {node = $doInst.node; }
    |    forInst        {node = $forInst.node; }
    |    instructionScalar ';' {node = $instructionScalar.node;}
    |    returnInst {node = $returnInst.node; }
    ;
    
instructionList returns[WNInstructionList* node]
@init {
}
    :    a=instruction  {node = WN_SCRIPTNODE_NEW(WNInstructionList($a.node)); SetLocationFromNode(node, $a.node); }
        (b=instruction {node->AddInstruction($b.node); SetEndLocationFromNode(node, $b.node); })*
    ;

body returns[WNInstructionList* node]
@init {
}
    :    a=LBRACE b=RBRACE { node = WN_SCRIPTNODE_NEW(WNInstructionList()); SetLocation(node, $a); SetEndLocation(node, $b); }
    |    d=LBRACE instructionList e=RBRACE {node = $instructionList.node; SetLocation(node, $d); SetEndLocation(node, $e); }
    ;

function returns[WNFunction* node]
@init {
}
    :    decl parameterList body { node = WN_SCRIPTNODE_NEW(WNFunction($decl.node, $parameterList.node, $body.node)); SetLocationFromNode(node, $decl.node); SetEndLocationFromNode(node, $body.node); }
    ;
    
structDecl returns[WNStruct* node]
    :    STRUCT TYPE {node= WN_SCRIPTNODE_NEW(WNStruct($TYPE.text.c_str())); SetLocation(node, $STRUCT); }
            LBRACE (a=declaration { node->AddStructElem($a.node);} SEMICOLON )* RBRACE  { SetEndLocation(node, $RBRACE); }
    ;

classDecl returns[WNStruct* node]
    :    
        ( 
            (f=CLASS c=TYPE {node= WN_SCRIPTNODE_NEW(WNStruct($c.text.c_str(), true)); SetLocation(node, $f); })
          | (g=CLASS d=TYPE COLON e=TYPE {node= WN_SCRIPTNODE_NEW(WNStruct($d.text.c_str(), true, $e.text.c_str())); SetLocation(node, $g); })
        )
            LBRACE 
                (
                    (a=declaration { node->AddStructElem($a.node);} SEMICOLON)
                  | (b=function    { node->AddFunction($b.node); })
                  | (VIRTUAL h=function { node->AddFunction($h.node); $h.node->SetVirtual(); })
                  | (OVERRIDE i=function { node->AddFunction($i.node); $i.node->SetOverride(); })
                )* 
            RBRACE  { SetEndLocation(node, $RBRACE); }
    ;

inc returns[WN_CHAR* file]
    :   INCLUDE STRING { COPY_STRING($STRING.text.c_str(), file); }
    ;

program returns[WNScriptFile* node]
@init{
    node = WN_SCRIPTNODE_NEW(WNScriptFile());
}
    :   (
                function   { node->AddFunction($function.node); }
            |   structDecl { node->AddStruct($structDecl.node); }
            |   classDecl  { node->AddStruct($classDecl.node); }
            |   inc        { node->AddInclude($inc.file); }
        )*
    ;
    
