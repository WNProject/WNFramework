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
    #pragma clang diagnostic ignored "-Wc++11-narrowing"
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
VOID_TYPE: 'Void';
INT_TYPE:  'Int';
FLOAT_TYPE: 'Float';
BOOL_TYPE: 'Bool';
STRING_TYPE: 'String';
CHAR_TYPE: 'Char';
QUESTION: '?';
WEAK_REF:     'weak';
SHARED_REF:   'shared';

BOOL :  'true' | 'false';

ID  :    ('a'..'z') ('a'..'z'|'A'..'Z'|'0'..'9'|'_')*
    ;

TYPE  :    ('A'..'Z') ('a'..'z'|'A'..'Z'|'0'..'9'|'_')*
    ;

INT :    ('-')?('0'..'9')+
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

objectType returns[scripting::type* node]
@init {
    node = nullptr;
}
    :   TYPE    { node = m_allocator->construct<scripting::type>(m_allocator, $TYPE.text.c_str()); SET_LOCATION(node, $TYPE); }
    |   STRING_TYPE { node = m_allocator->construct<scripting::type>(m_allocator, scripting::type_classification::string_type); SET_LOCATION(node, $STRING_TYPE); }
    ;

scalarType returns[scripting::type* node]
@init {
    node = nullptr;
}
    :   VOID_TYPE { node = m_allocator->construct<scripting::type>(m_allocator, scripting::type_classification::void_type); SET_LOCATION(node, $VOID_TYPE); }
    |   INT_TYPE { node = m_allocator->construct<scripting::type>(m_allocator, scripting::type_classification::int_type); SET_LOCATION(node, $INT_TYPE); }
    |   FLOAT_TYPE { node = m_allocator->construct<scripting::type>(m_allocator, scripting::type_classification::float_type); SET_LOCATION(node, $FLOAT_TYPE); }
    |   CHAR_TYPE { node = m_allocator->construct<scripting::type>(m_allocator, scripting::type_classification::char_type); SET_LOCATION(node, $CHAR_TYPE); }
    |   BOOL_TYPE { node = m_allocator->construct<scripting::type>(m_allocator, scripting::type_classification::bool_type); SET_LOCATION(node, $BOOL_TYPE); }
    ;

compoundType returns[scripting::type* node]
@init {
    node = nullptr;
}
    : scalarType { node = $scalarType.node; SET_LOCATION_FROM_NODE(node, $scalarType.node); }
           (LSQBRACKET RSQBRACKET {
                   SET_END_LOCATION(node, $RSQBRACKET);
                   node = m_allocator->construct<scripting::array_type>(m_allocator, node);
                   SET_LOCATION(node, $LSQBRACKET);
                   SET_END_LOCATION(node, $RSQBRACKET);
           })+
    | objectType { node = $objectType.node; }
    ;

type    returns[scripting::type* node]
@init {
    node = nullptr;
}
    : compoundType { node = $compoundType.node; }
    | scalarType   { node = $scalarType.node; }
    ;

param returns[scripting::parameter* node]
@init {
    node = nullptr;
}
    :    scalarType a=ID {
            node = m_allocator->construct<scripting::parameter>(m_allocator, $scalarType.node, $a.text.c_str()); SET_LOCATION_FROM_NODE(node, $scalarType.node); SET_END_LOCATION(node, $a); }
    |    bb=compoundType aa=ID {
            $bb.node->set_reference_type(scripting::reference_type::unique);
            node = m_allocator->construct<scripting::parameter>(m_allocator, $bb.node, $aa.text.c_str()); SET_LOCATION_FROM_NODE(node, $bb.node); SET_END_LOCATION(node, $aa); }
    |    SHARED_REF b=compoundType c=ID {
           $b.node->set_reference_type(scripting::reference_type::shared);
           node = m_allocator->construct<scripting::parameter>(m_allocator, $b.node, $c.text.c_str()); SET_LOCATION(node, $SHARED_REF); SET_END_LOCATION(node, $c);
         }
    |    WEAK_REF d=compoundType  e=ID {
           $d.node->set_reference_type(scripting::reference_type::weak);
           node = m_allocator->construct<scripting::parameter>(m_allocator, $d.node, $e.text.c_str()); SET_LOCATION(node, $WEAK_REF); SET_END_LOCATION(node, $e);
         }
    |    f=compoundType QUESTION  g=ID {
           $f.node->set_reference_type(scripting::reference_type::nullable);
           node = m_allocator->construct<scripting::parameter>(m_allocator, $f.node, $g.text.c_str()); SET_LOCATION_FROM_NODE(node, $f.node); SET_END_LOCATION(node, $g);
         }
    ;

paramList returns[scripting::parameter_list* node]
@init {
    node = nullptr;
}
    :    a=param { node = m_allocator->construct<scripting::parameter_list>(m_allocator, $a.node); SET_LOCATION_FROM_NODE(node, $a.node); }
        (COMMA b=param { $node->add_parameter($b.node); SET_END_LOCATION_FROM_NODE(node, $b.node); })*
    ;

parameterList returns[scripting::parameter_list* node]
@init {
    node = nullptr;
}
    :    LBRACKET paramList RBRACKET { node = $paramList.node; SET_LOCATION(node, $LBRACKET); SET_END_LOCATION(node, $RBRACKET); }
        |    LBRACKET RBRACKET      { node = nullptr; }
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
    ;

arglist returns[scripting::arg_list* node]
@init {
    node = m_allocator->construct<scripting::arg_list>(m_allocator);
}
        :   (
                    (a=expr { node->add_expression($a.node); SET_LOCATION_FROM_NODE(node, $a.node); })
            )

            (','
                (
                    ( d=expr { node->add_expression($d.node, true); SET_END_LOCATION_FROM_NODE(node, $d.node); } ) |
                )
            )*
        ;

expr returns[scripting::expression* node]
@init {
    node = nullptr;
}
        :    cond_ex { node = $cond_ex.node; }
        ;

cond_ex returns[scripting::expression* node]
@init {
    node = nullptr;
}
        :    or_ex  { node = $or_ex.node; }

                ('?' b=expr ':' c=cond_ex { node = m_allocator->construct<scripting::cond_expression>(m_allocator, node, $b.node, $c.node);  SET_LOCATION_FROM_NODE(node, $or_ex.node); SET_END_LOCATION_FROM_NODE(node, $c.node); }  )?
        ;

or_ex    returns[scripting::expression* node]
@init {
    node = nullptr;
}
        :    a=and_ex { node = $a.node; }
            ('||' b=and_ex { node = m_allocator->construct<scripting::short_circuit_expression>(m_allocator, scripting::short_circuit_type::or_operator, node, $b.node); SET_LOCATION_FROM_NODE(node, $a.node); SET_END_LOCATION_FROM_NODE(node, $b.node); })*
        ;

and_ex  returns[scripting::expression* node]
@init {
    node = nullptr;
}
        :    a=eq_ex { node = $a.node; }
            ('&&' b=eq_ex {node = m_allocator->construct<scripting::short_circuit_expression>(m_allocator, scripting::short_circuit_type::and_operator, node, $b.node); SET_LOCATION_FROM_NODE(node, $a.node); SET_END_LOCATION_FROM_NODE(node, $b.node); })*;

eq_ex    returns[scripting::expression* node]
@init {
    node = nullptr;
}
        :    a=rel_ex { node = $a.node; }
            (
                ('==' b=rel_ex {node = m_allocator->construct<scripting::binary_expression>(m_allocator, scripting::arithmetic_type::arithmetic_equal, node, $b.node);  SET_LOCATION_FROM_NODE(node, $a.node); SET_END_LOCATION_FROM_NODE(node, $b.node); } )
              | ('!=' c=rel_ex {node = m_allocator->construct<scripting::binary_expression>(m_allocator, scripting::arithmetic_type::arithmetic_not_equal, node, $c.node); SET_LOCATION_FROM_NODE(node, $a.node); SET_END_LOCATION_FROM_NODE(node, $c.node); } )
            )*
        ;

rel_ex  returns[scripting::expression* node]
@init {
    node = nullptr;
}
        :    a=add_ex { node=$a.node; }
            (
                ( '<' b=add_ex {node = m_allocator->construct<scripting::binary_expression>(m_allocator, scripting::arithmetic_type::arithmetic_less_than, node, $b.node);   SET_LOCATION_FROM_NODE(node, $a.node); SET_END_LOCATION_FROM_NODE(node, $b.node); } )
              | ( '>' c=add_ex {node = m_allocator->construct<scripting::binary_expression>(m_allocator, scripting::arithmetic_type::arithmetic_greater_than, node, $c.node);   SET_LOCATION_FROM_NODE(node, $a.node); SET_END_LOCATION_FROM_NODE(node, $c.node); } )
              | ( '<=' d=add_ex {node = m_allocator->construct<scripting::binary_expression>(m_allocator, scripting::arithmetic_type::arithmetic_less_than_or_equal, node, $d.node);   SET_LOCATION_FROM_NODE(node, $a.node); SET_END_LOCATION_FROM_NODE(node, $d.node); } )
              | ( '>=' e=add_ex {node = m_allocator->construct<scripting::binary_expression>(m_allocator, scripting::arithmetic_type::arithmetic_greater_than_or_equal, node, $e.node);   SET_LOCATION_FROM_NODE(node, $a.node); SET_END_LOCATION_FROM_NODE(node, $e.node); } )
            )*
        ;

add_ex     returns[scripting::expression* node]
@init {
    node = nullptr;
}
        :    a=mult_ex {node = $a.node; }
            (
                ('+' b=mult_ex {node = m_allocator->construct<scripting::binary_expression>(m_allocator, scripting::arithmetic_type::arithmetic_add, node, $b.node);   SET_LOCATION_FROM_NODE(node, $a.node); SET_END_LOCATION_FROM_NODE(node, $b.node); })
              | ('-' c=mult_ex {node = m_allocator->construct<scripting::binary_expression>(m_allocator, scripting::arithmetic_type::arithmetic_sub, node, $c.node);   SET_LOCATION_FROM_NODE(node, $a.node); SET_END_LOCATION_FROM_NODE(node, $c.node); })
            )*
        ;

mult_ex    returns[scripting::expression* node]
@init {
    node = nullptr;
}
        :    a=unary_ex { node = $a.node; }
            (
                    ('*' b=unary_ex {node = m_allocator->construct<scripting::binary_expression>(m_allocator, scripting::arithmetic_type::arithmetic_mult, node, $b.node);   SET_LOCATION_FROM_NODE(node, $a.node); SET_END_LOCATION_FROM_NODE(node, $b.node); })
                |   ('/' c=unary_ex {node = m_allocator->construct<scripting::binary_expression>(m_allocator, scripting::arithmetic_type::arithmetic_div, node, $c.node);   SET_LOCATION_FROM_NODE(node, $a.node); SET_END_LOCATION_FROM_NODE(node, $c.node); })
                |   ('%' d=unary_ex {node = m_allocator->construct<scripting::binary_expression>(m_allocator, scripting::arithmetic_type::arithmetic_mod, node, $d.node);   SET_LOCATION_FROM_NODE(node, $a.node); SET_END_LOCATION_FROM_NODE(node, $d.node); })
            )*
        ;

unary_ex returns[scripting::expression* node]
@init {
    node = nullptr;
}
    :    a=post_ex { node = $a.node; }
    |    '++' b=unary_ex { scripting::node* t = node; node = m_allocator->construct<scripting::unary_expression>(m_allocator, scripting::unary_type::pre_increment, $b.node);   SET_LOCATION_FROM_NODE(node, t); SET_END_LOCATION_FROM_NODE(node, $b.node); }
    |    '--' c=unary_ex { scripting::node* t = node; node = m_allocator->construct<scripting::unary_expression>(m_allocator, scripting::unary_type::pre_decrement, $c.node);   SET_LOCATION_FROM_NODE(node, t); SET_END_LOCATION_FROM_NODE(node, $c.node); }
    |    '-' d=unary_ex  { scripting::node* t = node; node = m_allocator->construct<scripting::unary_expression>(m_allocator, scripting::unary_type::negation, $d.node);   SET_LOCATION_FROM_NODE(node, t); SET_END_LOCATION_FROM_NODE(node, $d.node); }
        ;


post_ex_proper returns[scripting::post_expression* node]
@init {
    node = nullptr;
}
    :    d=LSQBRACKET a=expr e=RSQBRACKET { node = m_allocator->construct<scripting::array_access_expression>(m_allocator, $a.node); SET_LOCATION(node, $d); SET_END_LOCATION(node, $e); }
    |    f=LBRACKET g=RBRACKET              { node = m_allocator->construct<scripting::function_call_expression>(m_allocator); SET_LOCATION(node, $f); SET_END_LOCATION(node, $g); }
    |    h=LBRACKET b = arglist i=RBRACKET  { node = m_allocator->construct<scripting::function_call_expression>(m_allocator, $b.node); SET_LOCATION(node, $h); SET_END_LOCATION(node, $i); }
    |   '.' c=ID              { node = m_allocator->construct<scripting::member_access_expression>(m_allocator, $c.text.c_str()); SET_LOCATION(node, $c); }
    |     DOUBINC                  { node = m_allocator->construct<scripting::post_unary_expression>(m_allocator, scripting::post_unary_type::post_increment); SET_LOCATION(node, $DOUBINC);}
    |   DOUBDEC                  { node = m_allocator->construct<scripting::post_unary_expression>(m_allocator, scripting::post_unary_type::post_decrement); SET_LOCATION(node, $DOUBDEC);}
    ;

post_ex    returns[scripting::expression* node]
@init {
    node = nullptr;
}
        :   prim_ex { node = $prim_ex.node; }
            (a=post_ex_proper {$a.node->add_base_expression(node); SET_END_LOCATION_FROM_NODE($a.node, node); node = $a.node;  } )* ;

assignment_or_expression returns[scripting::instruction* node]
@init {
    node = nullptr;
}
    :    a=expr
           (
              (b=assign_type c=expr {
                  scripting::lvalue* lval = m_allocator->construct<scripting::lvalue>(m_allocator, $a.node);
                  SET_LOCATION_FROM_NODE(lval, $c.node);

                  scripting::assignment_instruction* assign = m_allocator->construct<scripting::assignment_instruction>(m_allocator, lval);
                  SET_LOCATION_FROM_NODE(assign , lval);

                  assign->add_value($b.node, $c.node);
                  SET_END_LOCATION_FROM_NODE(assign, $c.node);
                  node = assign;
                })
              | ({
                  node = m_allocator->construct<scripting::expression_instruction>(m_allocator, $a.node);
                  SET_LOCATION_FROM_NODE(node, $a.node);
                })
           )
    ;

constant returns[scripting::constant_expression* node]
@init {
    node = nullptr;
}
    :    a=INT    { scripting::type* type_node = m_allocator->construct<scripting::type>(m_allocator, scripting::type_classification::int_type); SET_LOCATION(type_node, $a); node = m_allocator->construct<scripting::constant_expression>(m_allocator, type_node, $a.text.c_str()); SET_LOCATION(node, $a); }
    |    b=FLOAT  { scripting::type* type_node = m_allocator->construct<scripting::type>(m_allocator, scripting::type_classification::float_type); SET_LOCATION(type_node, $b); node = m_allocator->construct<scripting::constant_expression>(m_allocator, type_node, $b.text.c_str()); SET_LOCATION(node, $b); }
    |    c=CHAR   { scripting::type* type_node = m_allocator->construct<scripting::type>(m_allocator, scripting::type_classification::char_type); SET_LOCATION(type_node, $c); node = m_allocator->construct<scripting::constant_expression>(m_allocator, type_node, $c.text.c_str()); SET_LOCATION(node, $c);}
    |    d=STRING { scripting::type* type_node = m_allocator->construct<scripting::type>(m_allocator, scripting::type_classification::string_type); SET_LOCATION(type_node, $d); node = m_allocator->construct<scripting::constant_expression>(m_allocator, type_node, $d.text.c_str()); SET_LOCATION(node, $d); }
    |    e=BOOL   { scripting::type* type_node = m_allocator->construct<scripting::type>(m_allocator, scripting::type_classification::bool_type); SET_LOCATION(type_node, $e); node = m_allocator->construct<scripting::constant_expression>(m_allocator, type_node, $e.text.c_str()); SET_LOCATION(node, $e); }
    |    f=scalarType g=LBRACE h=STRING i=RBRACE { node = m_allocator->construct<scripting::constant_expression>(m_allocator, $f.node, $h.text.c_str()); SET_LOCATION_FROM_NODE(node, $f.node); SET_END_LOCATION(node, $i);}
    ;

prim_ex returns[scripting::expression * node]
@init {
    node = nullptr;
}
    :    ID { node = m_allocator->construct<scripting::id_expression>(m_allocator, $ID.text.c_str()); SET_LOCATION(node, $ID);}
    |    ba=LBRACKET a=expr bb=RBRACKET {node = $a.node; SET_LOCATION(node, $ba); SET_END_LOCATION(node, $bb); }
    |    b=constant  {node = $b.node; }
    |   c=scalarType
          (   ( f=cast) { $f.node->set_type($c.node); node=$f.node; SET_START_LOCATION_FROM_NODE(node, $c.node); }
            | ( g=arrayInit )  { $g.node->set_type($c.node); node=$g.node; SET_START_LOCATION_FROM_NODE(node, $c.node); }
          )
    |   d=NULLTOK { node = m_allocator->construct<scripting::null_allocation_expression>(m_allocator); SET_LOCATION(node, $NULLTOK); }
    |   e=objectType h=structInit { $e.node->set_reference_type(scripting::reference_type::unique); $h.node->set_type($e.node); node=$h.node; SET_START_LOCATION_FROM_NODE(node, $e.node); }
    |   SHARED_REF i=objectType j=structInit { $i.node->set_reference_type(scripting::reference_type::shared); $j.node->set_type($i.node); node=$j.node; SET_START_LOCATION_FROM_NODE(node, $i.node); }
    ;

cast returns[scripting::cast_expression* node]
@init {
  node = nullptr;
}
    : a=LBRACKET b=expr c=RBRACKET { node = m_allocator->construct<scripting::cast_expression>(m_allocator, $b.node); SET_LOCATION(node, $a); SET_END_LOCATION(node, $c); }
    ;

structInit returns[scripting::struct_allocation_expression* node]
@init {
    node = nullptr;
}
    : a=LBRACKET b=RBRACKET { node = m_allocator->construct<scripting::struct_allocation_expression>(m_allocator); SET_LOCATION(node, $a); SET_END_LOCATION(node, $b); }
    ;

arrayInit returns[scripting::array_allocation_expression* node]
@init {
    node = m_allocator->construct<scripting::array_allocation_expression>(m_allocator);
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
    node =  m_allocator->construct<scripting::declaration>(m_allocator);
}
    :    a=param { node->set_parameter($a.node); SET_LOCATION_FROM_NODE(node, $a.node);}
            (
                ('=' ( (c=expr) { node->add_expression_initializer($c.node); SET_END_LOCATION_FROM_NODE(node, $c.node);} ) )
            )
    ;

instructionScalar returns[scripting::instruction* node]
@init {
    node = nullptr;
}
    :    declaration {node = $declaration.node; }
    |    assignment_or_expression  {node = $assignment_or_expression.node; }
    ;

returnInst returns[scripting::return_instruction* node]
@init {
    node = nullptr;
}
    :   a=RETURN expr b=SEMICOLON { node = m_allocator->construct<scripting::return_instruction>(m_allocator, $expr.node); SET_LOCATION(node, $a); SET_END_LOCATION(node, $b);}
    |   c=RETURN d=SEMICOLON { node = m_allocator->construct<scripting::return_instruction>(m_allocator); SET_LOCATION(node, $c); SET_END_LOCATION(node, $d); };

whileInst     returns[scripting::instruction* node]
@init {
    node = nullptr;
}
        :    WHILE LBRACKET expr RBRACKET instruction_list { node = m_allocator->construct<scripting::while_instruction>(m_allocator, $expr.node, $instruction_list.node); SET_LOCATION(node, $WHILE); SET_END_LOCATION_FROM_NODE(node, $instruction_list.node); }
        ;
doInst    returns[scripting::instruction* node]
@init {
    node = nullptr;
}
    :    DO instruction_list WHILE LBRACKET expr RBRACKET SEMICOLON {node = m_allocator->construct<scripting::do_instruction>(m_allocator, $expr.node, $instruction_list.node); SET_LOCATION(node, $DO); SET_END_LOCATION(node, $SEMICOLON); }
    ;

forInst    returns[scripting::for_instruction* node]
@init{
    node =  m_allocator->construct<scripting::for_instruction>(m_allocator);
}
    :    FOR LBRACKET  { SET_LOCATION(node, $FOR); }
                    (a=instructionScalar {node->add_initializer($a.node);})? SEMICOLON
                    (b=expr         {node->add_condition($b.node); })? SEMICOLON
                    (c=instructionScalar {node->add_post_op($c.node); })? RBRACKET
                    (d=instruction_list                 {node->add_body($d.node); SET_END_LOCATION_FROM_NODE(node, $d.node); })
    ;


elsemiddle returns[scripting::else_if_instruction* node]
@init {
    node = nullptr;
}
    :    ELSE IF LBRACKET expr RBRACKET instruction_list { node = m_allocator->construct<scripting::else_if_instruction>(m_allocator, $expr.node, $instruction_list.node); SET_LOCATION(node, $ELSE); SET_END_LOCATION_FROM_NODE(node, $instruction_list.node); }
    ;

endif    returns[scripting::instruction_list* node]
@init {
    node = nullptr;
}
    :    'else' instruction_list {node = $instruction_list.node; }
    ;

ifInst returns[scripting::if_instruction* node]
@init {
    node = nullptr;
}
    :    IF LBRACKET expr RBRACKET instruction_list {node = m_allocator->construct<scripting::if_instruction>(m_allocator, $expr.node, $instruction_list.node); SET_LOCATION(node, $IF); SET_END_LOCATION_FROM_NODE(node, $instruction_list.node); }
            (elsemiddle {node->add_else_if($elsemiddle.node); SET_END_LOCATION_FROM_NODE(node, $elsemiddle.node); } )*
            (endif {node->add_else($endif.node); SET_LOCATION(node, $IF); SET_END_LOCATION_FROM_NODE(node, $endif.node); } )?
    ;

instruction returns [scripting::instruction* node]
@init {
    node = nullptr;
}
    :    ifInst         { node = $ifInst.node; }
    |    whileInst     {node = $whileInst.node; }
    |    doInst        {node = $doInst.node; }
    |    forInst        {node = $forInst.node; }
    |    instructionScalar ';' {node = $instructionScalar.node;}
    |    returnInst {node = $returnInst.node; }
    |    instruction_list { node = $instruction_list.node; }
    ;

instruction_list returns[scripting::instruction_list* node]
@init {
    node = m_allocator->construct<scripting::instruction_list>(m_allocator);
}
    :    lb=LBRACE
            (b=instruction {node->add_instruction($b.node);})*
         rb=RBRACE {
          SET_LOCATION(node, $lb); SET_END_LOCATION(node, $rb);
         }
    ;

function returns[scripting::function* node]
@init {
    node = nullptr;
}
    :    param parameterList instruction_list { node = m_allocator->construct<scripting::function>(m_allocator, $param.node, $parameterList.node, $instruction_list.node); SET_LOCATION_FROM_NODE(node, $param.node); SET_END_LOCATION_FROM_NODE(node, $instruction_list.node); }
    ;

structDecl returns[scripting::struct_definition* node]
@init {
    node = nullptr;
}
    :    STRUCT TYPE {node= m_allocator->construct<scripting::struct_definition>(m_allocator, $TYPE.text.c_str()); SET_LOCATION(node, $STRUCT); }
            LBRACE (a=declaration { node->add_struct_elem($a.node);} SEMICOLON )* RBRACE  { SET_END_LOCATION(node, $RBRACE); }
    ;

classDecl returns[scripting::struct_definition* node]
@init {
    node = nullptr;
}
    :
        (
            (f=CLASS c=TYPE {node= m_allocator->construct<scripting::struct_definition>(m_allocator, $c.text.c_str(), true); SET_LOCATION(node, $f); })
          | (g=CLASS d=TYPE COLON e=TYPE {node= m_allocator->construct<scripting::struct_definition>(m_allocator, $d.text.c_str(), true, $e.text.c_str()); SET_LOCATION(node, $g); })
        )
            LBRACE
                (
                    (a=declaration { node->add_struct_elem($a.node);} SEMICOLON)
                  | (b=function    { node->add_function($b.node); })
                  | (VIRTUAL h=function { node->add_function($h.node); $h.node->set_is_virtual(true); })
                  | (OVERRIDE i=function { node->add_function($i.node); $i.node->set_is_override(true); })
                )*
            RBRACE  { SET_END_LOCATION(node, $RBRACE); }
    ;

inc returns[const char* file]
@init {
    file = nullptr;
}
    :   INCLUDE STRING { file = $STRING.text.c_str(); }
    ;

program returns[scripting::script_file* node]
@init{
    node = m_allocator->construct<scripting::script_file>(m_allocator);
}
    :   (
                function   { node->add_function($function.node); }
            |   structDecl { node->add_struct($structDecl.node); }
            |   classDecl  { node->add_struct($classDecl.node); }
            |   inc        { node->add_include($inc.file); }
        )*
    ;

