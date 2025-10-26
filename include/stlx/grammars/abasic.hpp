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

}  // namespace basic
}  // namespace stlx::grammars

#endif  // INCLUDE_STLX_GRAMMARS_ABASIC_HPP_
