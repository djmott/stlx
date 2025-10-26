// Copyright (c) 2025 David Mott
// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

#ifndef INCLUDE_STLX_GRAMMARS_ABASIC_HPP_
#define INCLUDE_STLX_GRAMMARS_ABASIC_HPP_

#include "stlx/parser.hpp"

namespace stlx::grammars {
namespace basic {

using namespace stlx::parse;

// ============================================================================
// Terminal Rules (Compile-time constants for lexing)
// ============================================================================

// Characters
using digit = characters<std::string::iterator, '0', '9'>;
using letter_lower = characters<std::string::iterator, 'a', 'z'>;
using letter_upper = characters<std::string::iterator, 'A', 'Z'>;
using letter = or_<std::string::iterator, letter_lower, letter_upper>;

// Operators
using plus = character<std::string::iterator, '+'>;
using minus = character<std::string::iterator, '-'>;
using mult = character<std::string::iterator, '*'>;
using div_op = character<std::string::iterator, '/'>;
using eq = character<std::string::iterator, '='>;
using lt = character<std::string::iterator, '<'>;
using gt = character<std::string::iterator, '>'>;
using comma = character<std::string::iterator, ','>;
using semicolon = character<std::string::iterator, ';'>;

// Delimiters
using lparen = character<std::string::iterator, '('>;
using rparen = character<std::string::iterator, ')'>;
using whitespace_char = character<std::string::iterator, ' '>;
using whitespace = zero_or_more_<std::string::iterator, whitespace_char>;

// Literals
using number_literal = one_or_more_<std::string::iterator, digit>;
using identifier_char = or_<std::string::iterator, letter_lower, letter_upper, digit>;
using identifier_literal = and_<std::string::iterator, 
                                 letter, 
                                 zero_or_more_<std::string::iterator, identifier_char>>;

// String literals
using quote = character<std::string::iterator, '"'>;
using string_char = not_<std::string::iterator, quote>;
using string_literal = and_<std::string::iterator, 
                            quote, 
                            zero_or_more_<std::string::iterator, string_char>, 
                            quote>;

// Keywords (BASIC language)
STRING(_let, "LET");
STRING(_print, "PRINT");
STRING(_input, "INPUT");
STRING(_if, "IF");
STRING(_then, "THEN");
STRING(_else, "ELSE");
STRING(_endif, "ENDIF");
STRING(_for, "FOR");
STRING(_to, "TO");
STRING(_step, "STEP");
STRING(_next, "NEXT");
STRING(_while, "WHILE");
STRING(_wend, "WEND");
STRING(_goto, "GOTO");
STRING(_gosub, "GOSUB");
STRING(_return, "RETURN");
STRING(_end, "END");
STRING(_true, "TRUE");
STRING(_false, "FALSE");

// ============================================================================
// Non-Terminal Rules (Runtime evaluation - rule specializations)
// ============================================================================

// Forward declarations
struct expression;
struct additive_expression;
struct multiplicative_expression;
struct unary_expression;
struct primary_expression;

// Expression grammar
// Expression precedence: Additive -> Multiplicative -> Unary -> Primary

struct primary_expression
    : rule<std::string::iterator, 
           or_<std::string::iterator,
               number_literal,
               identifier_literal,
               string_literal,
               and_<std::string::iterator, lparen, expression, rparen>>> {
};

struct unary_expression
    : rule<std::string::iterator,
           or_<std::string::iterator,
               primary_expression,
               and_<std::string::iterator, minus, primary_expression>>> {
};

struct multiplicative_expression
    : rule<std::string::iterator,
           or_<std::string::iterator,
               unary_expression,
               and_<std::string::iterator, multiplicative_expression, mult, unary_expression>,
               and_<std::string::iterator, multiplicative_expression, div_op, unary_expression>>> {
};

struct additive_expression
    : rule<std::string::iterator,
           or_<std::string::iterator,
               multiplicative_expression,
               and_<std::string::iterator, additive_expression, plus, multiplicative_expression>,
               and_<std::string::iterator, additive_expression, minus, multiplicative_expression>>> {
};

struct expression
    : rule<std::string::iterator, additive_expression> {
};

// Comparison operators
using eq_op = and_<std::string::iterator, eq, eq>;
using ne_op = and_<std::string::iterator, character<std::string::iterator, '!'>, eq>;
using le_op = and_<std::string::iterator, lt, eq>;
using ge_op = and_<std::string::iterator, gt, eq>;

struct comparison_expression
    : rule<std::string::iterator,
           or_<std::string::iterator,
               expression,
               and_<std::string::iterator, expression, eq_op, expression>,
               and_<std::string::iterator, expression, ne_op, expression>,
               and_<std::string::iterator, expression, lt, expression>,
               and_<std::string::iterator, expression, gt, expression>,
               and_<std::string::iterator, expression, le_op, expression>,
               and_<std::string::iterator, expression, ge_op, expression>>> {
};

// Statements
struct assignment_statement;
struct print_statement;
struct input_statement;
struct if_statement;
struct for_statement;
struct while_statement;
struct goto_statement;
struct gosub_statement;
struct return_statement;
struct end_statement;

// Forward declaration for statement sequence
struct statement_sequence;

// Assignment: LET identifier = expression
struct assignment_statement
    : rule<std::string::iterator,
           and_<std::string::iterator,
                whitespace,
                _let, whitespace,
                identifier_literal, whitespace,
                eq, whitespace,
                expression>> {
};

// PRINT expression
struct print_statement
    : rule<std::string::iterator,
           and_<std::string::iterator,
                whitespace,
                _print, whitespace,
                expression>> {
};

// INPUT identifier
struct input_statement
    : rule<std::string::iterator,
           and_<std::string::iterator,
                whitespace,
                _input, whitespace,
                identifier_literal>> {
};

// IF comparison THEN ... ELSE ... ENDIF
struct if_statement
    : rule<std::string::iterator,
           and_<std::string::iterator,
                whitespace,
                _if, whitespace,
                comparison_expression, whitespace,
                _then, whitespace,
                statement_sequence, whitespace,
                _else, whitespace,
                statement_sequence, whitespace,
                _endif>> {
};

// FOR identifier = expression TO expression STEP expression ... NEXT
struct for_statement
    : rule<std::string::iterator,
           and_<std::string::iterator,
                whitespace,
                _for, whitespace,
                identifier_literal, whitespace,
                eq, whitespace,
                expression, whitespace,
                _to, whitespace,
                expression, whitespace,
                _step, whitespace,
                expression, whitespace,
                statement_sequence, whitespace,
                _next, whitespace,
                identifier_literal>> {
};

// WHILE comparison ... WEND
struct while_statement
    : rule<std::string::iterator,
           and_<std::string::iterator,
                whitespace,
                _while, whitespace,
                comparison_expression, whitespace,
                statement_sequence, whitespace,
                _wend>> {
};

// GOTO identifier
struct goto_statement
    : rule<std::string::iterator,
           and_<std::string::iterator,
                whitespace,
                _goto, whitespace,
                identifier_literal>> {
};

// GOSUB identifier
struct gosub_statement
    : rule<std::string::iterator,
           and_<std::string::iterator,
                whitespace,
                _gosub, whitespace,
                identifier_literal>> {
};

// RETURN
struct return_statement
    : rule<std::string::iterator,
           and_<std::string::iterator,
                whitespace,
                _return>> {
};

// END
struct end_statement
    : rule<std::string::iterator,
           and_<std::string::iterator,
                whitespace,
                _end>> {
};

// Statement sequence (one or more statements)
struct statement_sequence
    : rule<std::string::iterator,
           one_or_more_<std::string::iterator,
                        or_<std::string::iterator,
                            assignment_statement,
                            print_statement,
                            input_statement,
                            if_statement,
                            for_statement,
                            while_statement,
                            goto_statement,
                            gosub_statement,
                            return_statement,
                            end_statement>>> {
};

// Program (statement sequence with optional trailing whitespace)
struct basic_program
    : rule<std::string::iterator,
           and_<std::string::iterator,
                statement_sequence,
                whitespace>> {
};

}  // namespace basic
}  // namespace stlx::grammars

#endif  // INCLUDE_STLX_GRAMMARS_ABASIC_HPP_
