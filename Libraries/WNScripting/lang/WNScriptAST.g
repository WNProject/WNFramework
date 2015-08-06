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
}

@lexer::traits
{
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

}

@parser::context {
private:
  wn::memory::allocator* m_allocator;
public:
  void set_allocator(wn::memory::allocator* _allocator) {
    m_allocator = _allocator;
  }
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

scalarType returns[scripting::type_node* node]
@init {
    node = wn_nullptr;
}
    :   TYPE    { node = m_allocator->make_allocated<scripting::type_node>(m_allocator, $TYPE.text.c_str()); SET_LOCATION(node, $TYPE); }
    ;

type    returns[scripting::type_node* node]
@init {
    node = wn_nullptr;
}
    :    scalarType { node = $scalarType.node; SET_LOCATION_FROM_NODE(node, $scalarType.node); }
                (LSQBRACKET RSQBRACKET { node->add_array_level(); SET_END_LOCATION(node, $RSQBRACKET); })*
    ;

decl returns[scripting::declaration* node]
@init {
    node = wn_nullptr;
}
    :    type ID {node = m_allocator->make_allocated<scripting::declaration>(m_allocator, $type.node, $ID.text.c_str()); SET_LOCATION_FROM_NODE(node, $type.node); SET_END_LOCATION(node, $ID); }
    ;

declList returns[scripting::decl_list* node]
@init {
    node = wn_nullptr;
}
    :    a=decl { node = m_allocator->make_allocated<scripting::decl_list>(m_allocator, $a.node); SET_LOCATION_FROM_NODE(node, $a.node); }
        (COMMA b=decl { $node->add_declaration($b.node); SET_END_LOCATION_FROM_NODE(node, $b.node); })*
    ;

parameterList returns[scripting::decl_list* node]
@init {
    node = wn_nullptr;
}
    :    LBRACKET declList RBRACKET { node = $declList.node; SET_LOCATION(node, $LBRACKET); SET_END_LOCATION(node, $RBRACKET); }
        |    LBRACKET RBRACKET      { node = wn_nullptr; }
    ;


assign_type returns[scripting::assign_type node]
@init {
    node = scripting::assign_type::equal;
}
    :    '='  { node = scripting::assign_type::equal; }
    |    '+=' { node = scripting::assign_type::add_equal; }
    |    '-=' { node = scripting::assign_type::sub_equal; }
    |    '*=' { node = scripting::assign_type::mult_equal; }
    |    '/=' { node = scripting::assign_type::div_equal; }
    |    '%=' { node = scripting::assign_type::mod_equal; }
    |    '<==' { node = scripting::assign_type::change_owner; }
    ;

lvalue returns[scripting::lvalue* node]
@init {
    node = wn_nullptr;
}
    :    unary_ex { node = m_allocator->make_allocated<scripting::lvalue>(m_allocator, $unary_ex.node); SET_LOCATION_FROM_NODE(node, $unary_ex.node); }
    ;

arglist returns[scripting::arg_list* node]
@init {
    node = m_allocator->make_allocated<scripting::arg_list>(m_allocator);
}
        :   (
                    (a=expr { node->add_expression($a.node); SET_LOCATION_FROM_NODE(node, $a.node); })
                |   ('<==' b=expr {node->add_expression($b.node, wn_true); SET_LOCATION_FROM_NODE(node, $b.node); })
            )

            (','
                (
                    ( '<==' c=expr { node->add_expression($c.node); SET_END_LOCATION_FROM_NODE(node, $c.node); } ) |
                    ( d=expr { node->add_expression($d.node, wn_true); SET_END_LOCATION_FROM_NODE(node, $d.node); } ) |
                )
            )*
        ;

expr returns[scripting::expression* node]
@init {
    node = wn_nullptr;
}
        :    cond_ex { node = $cond_ex.node; }
        ;

cond_ex returns[scripting::expression* node]
@init {
    node = wn_nullptr;
}
        :    or_ex  { node = $or_ex.node; }

                ('?' b=expr ':' c=cond_ex { node = m_allocator->make_allocated<scripting::cond_expression>(m_allocator, node, $b.node, $c.node);  SET_LOCATION_FROM_NODE(node, $or_ex.node); SET_END_LOCATION_FROM_NODE(node, $c.node); }  )?
        ;

or_ex    returns[scripting::expression* node]
@init {
    node = wn_nullptr;
}
        :    a=and_ex { node = $a.node; }
            ('||' b=and_ex { node = m_allocator->make_allocated<scripting::ss_expression>(m_allocator, scripting::short_circuit_type::or_operator, node, $b.node); SET_LOCATION_FROM_NODE(node, $a.node); SET_END_LOCATION_FROM_NODE(node, $b.node); })*
        ;

and_ex  returns[scripting::expression* node]
@init {
    node = wn_nullptr;
}
        :    a=eq_ex { node = $a.node; }
            ('&&' b=eq_ex {node = m_allocator->make_allocated<scripting::ss_expression>(m_allocator, scripting::short_circuit_type::and_operator, node, $b.node); SET_LOCATION_FROM_NODE(node, $a.node); SET_END_LOCATION_FROM_NODE(node, $b.node); })*;
			
eq_ex    returns[scripting::expression* node]
@init {
    node = wn_nullptr;
}
        :    a=rel_ex { node = $a.node; }
            (
                ('==' b=rel_ex {node = m_allocator->make_allocated<scripting::binary_expression>(m_allocator, scripting::arithmetic_type::arithmetic_equal, node, $b.node);  SET_LOCATION_FROM_NODE(node, $a.node); SET_END_LOCATION_FROM_NODE(node, $b.node); } )
              | ('!=' c=rel_ex {node = m_allocator->make_allocated<scripting::binary_expression>(m_allocator, scripting::arithmetic_type::arithmetic_not_equal, node, $c.node); SET_LOCATION_FROM_NODE(node, $a.node); SET_END_LOCATION_FROM_NODE(node, $c.node); } )
            )*
        ;

rel_ex  returns[scripting::expression* node]
@init {
    node = wn_nullptr;
}
        :    a=add_ex { node=$a.node; }
            (
                ( '<' b=add_ex {node = m_allocator->make_allocated<scripting::binary_expression>(m_allocator, scripting::arithmetic_type::arithmetic_less_than, node, $b.node);   SET_LOCATION_FROM_NODE(node, $a.node); SET_END_LOCATION_FROM_NODE(node, $b.node); } )
              | ( '>' c=add_ex {node = m_allocator->make_allocated<scripting::binary_expression>(m_allocator, scripting::arithmetic_type::arithmetic_greater_than, node, $c.node);   SET_LOCATION_FROM_NODE(node, $a.node); SET_END_LOCATION_FROM_NODE(node, $c.node); } )
              | ( '<=' d=add_ex {node = m_allocator->make_allocated<scripting::binary_expression>(m_allocator, scripting::arithmetic_type::arithmetic_less_than_or_equal, node, $d.node);   SET_LOCATION_FROM_NODE(node, $a.node); SET_END_LOCATION_FROM_NODE(node, $d.node); } )
              | ( '>=' e=add_ex {node = m_allocator->make_allocated<scripting::binary_expression>(m_allocator, scripting::arithmetic_type::arithmetic_greater_than_or_equal, node, $e.node);   SET_LOCATION_FROM_NODE(node, $a.node); SET_END_LOCATION_FROM_NODE(node, $e.node); } )
            )*
        ;

add_ex     returns[scripting::expression* node]
@init {
    node = wn_nullptr;
}
        :    a=mult_ex {node = $a.node; }
            (
                ('+' b=mult_ex {node = m_allocator->make_allocated<scripting::binary_expression>(m_allocator, scripting::arithmetic_type::arithmetic_add, node, $b.node);   SET_LOCATION_FROM_NODE(node, $a.node); SET_END_LOCATION_FROM_NODE(node, $b.node); })
              | ('-' c=mult_ex {node = m_allocator->make_allocated<scripting::binary_expression>(m_allocator, scripting::arithmetic_type::arithmetic_sub, node, $c.node);   SET_LOCATION_FROM_NODE(node, $a.node); SET_END_LOCATION_FROM_NODE(node, $c.node); })
            )*
        ;

mult_ex    returns[scripting::expression* node]
@init {
    node = wn_nullptr;
}
        :    a=unary_ex { node = $a.node; }
            (
                    ('*' b=unary_ex {node = m_allocator->make_allocated<scripting::binary_expression>(m_allocator, scripting::arithmetic_type::arithmetic_mult, node, $b.node);   SET_LOCATION_FROM_NODE(node, $a.node); SET_END_LOCATION_FROM_NODE(node, $b.node); })
                |   ('/' c=unary_ex {node = m_allocator->make_allocated<scripting::binary_expression>(m_allocator, scripting::arithmetic_type::arithmetic_div, node, $c.node);   SET_LOCATION_FROM_NODE(node, $a.node); SET_END_LOCATION_FROM_NODE(node, $c.node); })
                |   ('%' d=unary_ex {node = m_allocator->make_allocated<scripting::binary_expression>(m_allocator, scripting::arithmetic_type::arithmetic_mod, node, $d.node);   SET_LOCATION_FROM_NODE(node, $a.node); SET_END_LOCATION_FROM_NODE(node, $d.node); })
            )*
        ;

unary_ex returns[scripting::expression* node]
@init {
    node = wn_nullptr;
}
    :    a=post_ex { node = $a.node; }
    |    '++' b=unary_ex { scripting::node* t = node; node = m_allocator->make_allocated<scripting::unary_expression>(m_allocator, scripting::unary_type::pre_increment, $b.node);   SET_LOCATION_FROM_NODE(node, t); SET_END_LOCATION_FROM_NODE(node, $b.node); }
    |    '--' c=unary_ex { scripting::node* t = node; node = m_allocator->make_allocated<scripting::unary_expression>(m_allocator, scripting::unary_type::pre_decrement, $c.node);   SET_LOCATION_FROM_NODE(node, t); SET_END_LOCATION_FROM_NODE(node, $c.node); }
    |    '-' d=unary_ex  { scripting::node* t = node; node = m_allocator->make_allocated<scripting::unary_expression>(m_allocator, scripting::unary_type::negation, $d.node);   SET_LOCATION_FROM_NODE(node, t); SET_END_LOCATION_FROM_NODE(node, $d.node); }
        ;


post_ex_proper returns[scripting::post_expression* node]
@init {
    node = wn_nullptr;
}
    :    d=LSQBRACKET a=expr e=RSQBRACKET { node = m_allocator->make_allocated<scripting::array_access_expression>(m_allocator, $a.node); SET_LOCATION(node, $d); SET_END_LOCATION(node, $e); }
    |    f=LBRACKET g=RBRACKET              { node = m_allocator->make_allocated<scripting::function_call_expression>(m_allocator); SET_LOCATION(node, $f); SET_END_LOCATION(node, $g); }
    |    h=LBRACKET b = arglist i=RBRACKET  { node = m_allocator->make_allocated<scripting::function_call_expression>(m_allocator, $b.node); SET_LOCATION(node, $h); SET_END_LOCATION(node, $i); }
    |   '.' c=ID              { node = m_allocator->make_allocated<scripting::member_access_expression>(m_allocator, $c.text.c_str()); SET_LOCATION(node, $c); }
    |     DOUBINC                  { node = m_allocator->make_allocated<scripting::post_unary_expression>(m_allocator, scripting::post_unary_type::post_increment); SET_LOCATION(node, $DOUBINC);}
    |   DOUBDEC                  { node = m_allocator->make_allocated<scripting::post_unary_expression>(m_allocator, scripting::post_unary_type::post_decrement); SET_LOCATION(node, $DOUBDEC);}
    ;

post_ex    returns[scripting::expression* node]
@init {
    node = wn_nullptr;
}
        :   prim_ex { node = $prim_ex.node; }
            (a=post_ex_proper {$a.node->add_base_expression(node); SET_END_LOCATION_FROM_NODE($a.node, node); node = $a.node;  } )* ;

assignment returns[scripting::assignment_instruction* node]
@init {
    node = wn_nullptr;
}
    :    lvalue { node = m_allocator->make_allocated<scripting::assignment_instruction>(m_allocator, $lvalue.node); SET_LOCATION_FROM_NODE(node, $lvalue.node); }
        (assign_type expr { node->add_value($assign_type.node, $expr.node); SET_END_LOCATION_FROM_NODE(node, $expr.node); }  )?
    ;
constant returns[scripting::constant_expression* node]
@init {
    node = wn_nullptr;
}
    :    INT    { node = m_allocator->make_allocated<scripting::constant_expression>(m_allocator, scripting::type_classification::int_type, $INT.text.c_str()); SET_LOCATION(node, $INT); }
    |    FLOAT  { node = m_allocator->make_allocated<scripting::constant_expression>(m_allocator, scripting::type_classification::float_type, $FLOAT.text.c_str()); SET_LOCATION(node, $FLOAT); }
    |    CHAR   { node = m_allocator->make_allocated<scripting::constant_expression>(m_allocator, scripting::type_classification::char_type, $CHAR.text.c_str()); SET_LOCATION(node, $CHAR);}
    |    STRING { node = m_allocator->make_allocated<scripting::constant_expression>(m_allocator, scripting::type_classification::string_type, $STRING.text.c_str()); SET_LOCATION(node, $STRING); }
    |   BOOL   { node = m_allocator->make_allocated<scripting::constant_expression>(m_allocator, scripting::type_classification::bool_type, $BOOL.text.c_str()); SET_LOCATION(node, $BOOL); }
    ;

prim_ex returns[scripting::expression * node]
@init {
    node = wn_nullptr;
}
    :    ID { node = m_allocator->make_allocated<scripting::id_expression>(m_allocator, $ID.text.c_str()); SET_LOCATION(node, $ID);}
    |    ba=LBRACKET a=expr bb=RBRACKET {node = $a.node; SET_LOCATION(node, $ba); SET_END_LOCATION(node, $bb); }
    |    b=constant  {node = $b.node; }
    |   c=scalarType
          (
              ( e=structInit ) { $e.node->set_type($c.node); node=$e.node; SET_START_LOCATION_FROM_NODE(node, $c.node); }
            | ( f=arrayInit )  { $f.node->set_type($c.node); node=$f.node; SET_START_LOCATION_FROM_NODE(node, $c.node); }
          )
    |   d=NULLTOK { node = m_allocator->make_allocated<scripting::null_allocation_expression>(m_allocator); SET_LOCATION(node, $NULLTOK); }
    ;

structInit returns[scripting::struct_allocation_expression* node]
@init {
    node = wn_nullptr;
}
    : a=LBRACKET b=RBRACKET { node = m_allocator->make_allocated<scripting::struct_allocation_expression>(m_allocator); SET_LOCATION(node, $a); SET_END_LOCATION(node, $b); }
    | c=LBRACKET d=expr e=RBRACKET { node = m_allocator->make_allocated<scripting::struct_allocation_expression>(m_allocator); node->set_copy_initializer(d); SET_LOCATION(node, $c); SET_END_LOCATION(node, $e); }
    ;

arrayInit returns[scripting::array_allocation_expression* node]
@init {
    node = m_allocator->make_allocated<scripting::array_allocation_expression>(m_allocator);
}
    :   (
            (
                (LSQBRACKET e=expr RSQBRACKET { node->add_expression($e.node);} )+
                (LSQBRACKET RSQBRACKET { node->add_level(); } )*
            )
            |   (LSQBRACKET RSQBRACKET { node->add_level(); } )+
        )
        (
            ( LBRACKET a=RBRACKET { SET_LOCATION(node, $a); } )
            |
            ( LBRACKET b=expr c=RBRACKET { node->set_copy_initializer(b); SET_LOCATION(node, $c); } )
        )

    ;

declaration returns[scripting::declaration* node]
@init {
    node = wn_nullptr;
}
    :    a=decl { node = $a.node; }
            (
                ('=' ( (c=expr) { node->add_expression_initializer($c.node); SET_END_LOCATION_FROM_NODE(node, $c.node); } ) )
            |   ('<==' ( (d=expr) { node->add_expression_initializer($d.node, true); SET_END_LOCATION_FROM_NODE(node, $d.node); } ) )
            )
    ;

instructionScalar returns[scripting::instruction* node]
@init {
    node = wn_nullptr;
}
    :    declaration {node = $declaration.node; }
    |    assignment  {node = $assignment.node; }
    ;

returnInst returns[scripting::return_instruction* node]
@init {
    node = wn_nullptr;
}
    :   a=RETURN expr b=SEMICOLON { node = m_allocator->make_allocated<scripting::return_instruction>(m_allocator, $expr.node); SET_LOCATION(node, $a); SET_END_LOCATION(node, $b);}
    |   c=RETURN d=SEMICOLON { node = m_allocator->make_allocated<scripting::return_instruction>(m_allocator); SET_LOCATION(node, $c); SET_END_LOCATION(node, $d); }
    |   e=RETURN_OWN expr f=SEMICOLON { node = m_allocator->make_allocated<scripting::return_instruction>(m_allocator, $expr.node, true); SET_LOCATION(node, $e); SET_END_LOCATION(node, $f); }
    ;

whileInst     returns[scripting::instruction* node]
@init {
    node = wn_nullptr;
}
        :    WHILE LBRACKET expr RBRACKET body { node = m_allocator->make_allocated<scripting::while_instruction>(m_allocator, $expr.node, $body.node); SET_LOCATION(node, $WHILE); SET_END_LOCATION_FROM_NODE(node, $body.node); }
        ;
doInst    returns[scripting::instruction* node]
@init {
    node = wn_nullptr;
}
    :    DO body WHILE LBRACKET expr RBRACKET SEMICOLON {node = m_allocator->make_allocated<scripting::do_instruction>(m_allocator, $expr.node, $body.node); SET_LOCATION(node, $DO); SET_END_LOCATION(node, $SEMICOLON); }
    ;

forInst    returns[scripting::for_instruction* node]
@init{
    node =  m_allocator->make_allocated<scripting::for_instruction>(m_allocator);
}
    :    FOR LBRACKET  { SET_LOCATION(node, $FOR); }
                    (a=instructionScalar {node->add_initializer($a.node);})? SEMICOLON
                    (b=expr         {node->add_condition($b.node); })? SEMICOLON
                    (c=instructionScalar {node->add_post_op($c.node); })? RBRACKET
                    (d=body                 {node->add_body($d.node); SET_END_LOCATION_FROM_NODE(node, $d.node); })
    ;


elsemiddle returns[scripting::else_if_instruction* node]
@init {
    node = wn_nullptr;
}
    :    ELSE IF LBRACKET expr RBRACKET body { node = m_allocator->make_allocated<scripting::else_if_instruction>(m_allocator, $expr.node, $body.node); SET_LOCATION(node, $ELSE); SET_END_LOCATION_FROM_NODE(node, $body.node); }
    ;

endif    returns[scripting::instruction_list* node]
@init {
    node = wn_nullptr;
}
    :    'else' body {node = $body.node; }
    ;

ifInst returns[scripting::if_instruction* node]
@init {
    node = wn_nullptr;
}
    :    IF LBRACKET expr RBRACKET body {node = m_allocator->make_allocated<scripting::if_instruction>(m_allocator, $expr.node, $body.node); SET_LOCATION(node, $IF); SET_END_LOCATION_FROM_NODE(node, $body.node); }
            (elsemiddle {node->add_else_if($elsemiddle.node); SET_END_LOCATION_FROM_NODE(node, $elsemiddle.node); } )*
            (endif {node->add_else($endif.node); SET_LOCATION(node, $IF); SET_END_LOCATION_FROM_NODE(node, $endif.node); } )?
    ;

instruction returns [scripting::instruction* node]
@init {
    node = wn_nullptr;
}
    :    ifInst         { node = $ifInst.node; }
    |    whileInst     {node = $whileInst.node; }
    |    doInst        {node = $doInst.node; }
    |    forInst        {node = $forInst.node; }
    |    instructionScalar ';' {node = $instructionScalar.node;}
    |    returnInst {node = $returnInst.node; }
    ;

instruction_list returns[scripting::instruction_list* node]
@init {
    node = wn_nullptr;
}
    :    a=instruction  {node = m_allocator->make_allocated<scripting::instruction_list>(m_allocator, $a.node); SET_LOCATION_FROM_NODE(node, $a.node); }
        (b=instruction {node->add_instruction($b.node); SET_END_LOCATION_FROM_NODE(node, $b.node); })*
    ;

body returns[scripting::instruction_list* node]
@init {
    node = wn_nullptr;
}
    :    a=LBRACE b=RBRACE { node = m_allocator->make_allocated<scripting::instruction_list>(m_allocator); SET_LOCATION(node, $a); SET_END_LOCATION(node, $b); }
    |    d=LBRACE instruction_list e=RBRACE {node = $instruction_list.node; SET_LOCATION(node, $d); SET_END_LOCATION(node, $e); }
    ;

function returns[scripting::function* node]
@init {
    node = wn_nullptr;
}
    :    decl parameterList body { node = m_allocator->make_allocated<scripting::function>(m_allocator, $decl.node, $parameterList.node, $body.node); SET_LOCATION_FROM_NODE(node, $decl.node); SET_END_LOCATION_FROM_NODE(node, $body.node); }
    ;

structDecl returns[scripting::struct_definition* node]
@init {
    node = wn_nullptr;
}
    :    STRUCT TYPE {node= m_allocator->make_allocated<scripting::struct_definition>(m_allocator, $TYPE.text.c_str()); SET_LOCATION(node, $STRUCT); }
            LBRACE (a=declaration { node->add_struct_elem($a.node);} SEMICOLON )* RBRACE  { SET_END_LOCATION(node, $RBRACE); }
    ;

classDecl returns[scripting::struct_definition* node]
@init {
    node = wn_nullptr;
}
    :
        (
            (f=CLASS c=TYPE {node= m_allocator->make_allocated<scripting::struct_definition>(m_allocator, $c.text.c_str(), true); SET_LOCATION(node, $f); })
          | (g=CLASS d=TYPE COLON e=TYPE {node= m_allocator->make_allocated<scripting::struct_definition>(m_allocator, $d.text.c_str(), true, $e.text.c_str()); SET_LOCATION(node, $g); })
        )
            LBRACE
                (
                    (a=declaration { node->add_struct_elem($a.node);} SEMICOLON)
                  | (b=function    { node->add_function($b.node); })
                  | (VIRTUAL h=function { node->add_function($h.node); $h.node->set_is_virtual(wn_true); })
                  | (OVERRIDE i=function { node->add_function($i.node); $i.node->set_is_override(wn_true); })
                )*
            RBRACE  { SET_END_LOCATION(node, $RBRACE); }
    ;

inc returns[const wn_char* file]
@init {
    file = wn_nullptr;
}
    :   INCLUDE STRING { file = $STRING.text.c_str(); }
    ;

program returns[scripting::script_file* node]
@init{
    node = m_allocator->make_allocated<scripting::script_file>(m_allocator);
}
    :   (
                function   { node->add_function($function.node); }
            |   structDecl { node->add_struct($structDecl.node); }
            |   classDecl  { node->add_struct($classDecl.node); }
            |   inc        { node->add_include($inc.file); }
        )*
    ;

