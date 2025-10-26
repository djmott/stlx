/**
 * @file parser_examples.cpp
 * @brief Parser combinator examples and demonstrations
 *
 * This file demonstrates various uses of the STLX parser combinator library
 * including expression parsing, JSON-like structures, and regex patterns.
 *
 * @copyright Copyright (c) 2025 David Mott
 * @license Distributed under the Boost Software License, Version 1.0.
 *          (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */

#include "stlx/parser.hpp"
#include <iostream>
#include <string>
#include <vector>

using namespace stlx::parse;

// Expression parsing example
namespace expression_example {
// Define basic terminals
using digit = characters<std::string::const_iterator, '0', '9'>;
using plus = character<std::string::const_iterator, '+'>;
using minus = character<std::string::const_iterator, '-'>;
using mult = character<std::string::const_iterator, '*'>;
using div = character<std::string::const_iterator, '/'>;
using lparen = character<std::string::const_iterator, '('>;
using rparen = character<std::string::const_iterator, ')'>;

// Define non-terminals
using number = one_or_more_<std::string::const_iterator, digit>;
using term = or_<std::string::const_iterator, number,
                 and_<std::string::const_iterator, lparen, number, rparen>>;
using expression = and_<
    std::string::const_iterator, term,
    zero_or_more_<std::string::const_iterator,
                  and_<std::string::const_iterator, plus, term>>>; // Simplified

void demonstrate_expression_parsing() {
  std::cout << "\n=== Expression Parsing Example ===" << std::endl;

  std::vector<std::string> test_expressions = {"123", "45+67", "1+2+3",
                                               "invalid"};

  for (const auto &expr : test_expressions) {
    std::cout << "Parsing: '" << expr << "'" << std::endl;

    auto begin = expr.begin();
    auto end = expr.end();
    context<std::string::const_iterator> ctx(begin, end);

    expression parser;
    bool success = parser.parse(ctx, begin, end);

    std::cout << "Result: " << (success ? "SUCCESS" : "FAILED") << std::endl;
    if (!success) {
      std::cout << "Errors: " << ctx.parse_errors.size() << std::endl;
    }
    std::cout << std::endl;
  }
}
} // namespace expression_example

// Statement parsing example
namespace statement_example {
using letter = characters<std::string::const_iterator, 'a', 'z'>;
using digit = characters<std::string::const_iterator, '0', '9'>;
using equals = character<std::string::const_iterator, '='>;
using semicolon = character<std::string::const_iterator, ';'>;
using space = character<std::string::const_iterator, ' '>;

using identifier = one_or_more_<std::string::const_iterator, letter>;
using number = one_or_more_<std::string::const_iterator, digit>;
using assignment =
    and_<std::string::const_iterator, identifier, equals, number, semicolon>;

void demonstrate_statement_parsing() {
  std::cout << "\n=== Statement Parsing Example ===" << std::endl;

  std::vector<std::string> test_statements = {"x=42;", "y=123;", "invalid",
                                              "a=;"};

  for (const auto &stmt : test_statements) {
    std::cout << "Parsing: '" << stmt << "'" << std::endl;

    auto begin = stmt.begin();
    auto end = stmt.end();
    context<std::string::const_iterator> ctx(begin, end);

    assignment parser;
    bool success = parser.parse(ctx, begin, end);

    std::cout << "Result: " << (success ? "SUCCESS" : "FAILED") << std::endl;
    if (!success) {
      std::cout << "Errors: " << ctx.parse_errors.size() << std::endl;
    }
    std::cout << std::endl;
  }
}
} // namespace statement_example

// JSON-like parsing example
namespace json_example {
using quote = character<std::string::const_iterator, '"'>;
using colon = character<std::string::const_iterator, ':'>;
using comma = character<std::string::const_iterator, ','>;
using lbrace = character<std::string::const_iterator, '{'>;
using rbrace = character<std::string::const_iterator, '}'>;
using lbracket = character<std::string::const_iterator, '['>;
using rbracket = character<std::string::const_iterator, ']'>;
using letter = characters<std::string::const_iterator, 'a', 'z'>;
using digit = characters<std::string::const_iterator, '0', '9'>;

using string_char = or_<std::string::const_iterator, letter, digit,
                        character<std::string::const_iterator, ' '>>;
using json_string =
    and_<std::string::const_iterator, quote,
         zero_or_more_<std::string::const_iterator, string_char>, quote>;
using json_number = one_or_more_<std::string::const_iterator, digit>;
using json_value = or_<std::string::const_iterator, json_string, json_number>;

void demonstrate_json_parsing() {
  std::cout << "\n=== JSON-like Parsing Example ===" << std::endl;

  std::vector<std::string> test_json = {"\"hello\"", "123", "\"test value\"",
                                        "invalid"};

  for (const auto &json : test_json) {
    std::cout << "Parsing: '" << json << "'" << std::endl;

    auto begin = json.begin();
    auto end = json.end();
    context<std::string::const_iterator> ctx(begin, end);

    json_value parser;
    bool success = parser.parse(ctx, begin, end);

    std::cout << "Result: " << (success ? "SUCCESS" : "FAILED") << std::endl;
    if (!success) {
      std::cout << "Errors: " << ctx.parse_errors.size() << std::endl;
    }
    std::cout << std::endl;
  }
}
} // namespace json_example

// Regex parsing example
namespace regex_example {
REGEX(email_regex, R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
REGEX(phone_regex, R"(\(\d{3}\)\s\d{3}-\d{4})");
REGEX(ip_regex, R"(\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3})");

void demonstrate_regex_parsing() {
  std::cout << "\n=== Regex Parsing Example ===" << std::endl;

  std::vector<std::string> test_emails = {"user@example.com", "invalid-email",
                                          "test@domain.org"};

  std::vector<std::string> test_phones = {"(555) 123-4567", "invalid-phone",
                                          "(999) 888-7777"};

  std::vector<std::string> test_ips = {"192.168.1.1", "invalid-ip", "10.0.0.1"};

  // Test email regex
  std::cout << "Testing email regex:" << std::endl;
  for (const auto &email : test_emails) {
    auto begin = email.begin();
    auto end = email.end();
    context<std::string::const_iterator> ctx(begin, end);

    email_regex parser;
    bool success = parser.parse(ctx, begin, end);
    std::cout << "  '" << email << "': " << (success ? "MATCH" : "NO MATCH")
              << std::endl;
  }

  // Test phone regex
  std::cout << "\nTesting phone regex:" << std::endl;
  for (const auto &phone : test_phones) {
    auto begin = phone.begin();
    auto end = phone.end();
    context<std::string::const_iterator> ctx(begin, end);

    phone_regex parser;
    bool success = parser.parse(ctx, begin, end);
    std::cout << "  '" << phone << "': " << (success ? "MATCH" : "NO MATCH")
              << std::endl;
  }

  // Test IP regex
  std::cout << "\nTesting IP regex:" << std::endl;
  for (const auto &ip : test_ips) {
    auto begin = ip.begin();
    auto end = ip.end();
    context<std::string::const_iterator> ctx(begin, end);

    ip_regex parser;
    bool success = parser.parse(ctx, begin, end);
    std::cout << "  '" << ip << "': " << (success ? "MATCH" : "NO MATCH")
              << std::endl;
  }
}
} // namespace regex_example

// Combinator examples
namespace combinator_example {
using letter = characters<std::string::const_iterator, 'a', 'z'>;
using digit = characters<std::string::const_iterator, '0', '9'>;
using underscore = character<std::string::const_iterator, '_'>;

using identifier_char =
    or_<std::string::const_iterator, letter, digit, underscore>;
using identifier =
    and_<std::string::const_iterator, letter,
         zero_or_more_<std::string::const_iterator, identifier_char>>;

void demonstrate_combinator_parsing() {
  std::cout << "\n=== Combinator Parsing Example ===" << std::endl;

  std::vector<std::string> test_identifiers = {"variable", "var123", "my_var",
                                               "123invalid", "_private"};

  for (const auto &ident : test_identifiers) {
    std::cout << "Parsing identifier: '" << ident << "'" << std::endl;

    auto begin = ident.begin();
    auto end = ident.end();
    context<std::string::const_iterator> ctx(begin, end);

    identifier parser;
    bool success = parser.parse(ctx, begin, end);

    std::cout << "Result: " << (success ? "VALID" : "INVALID") << std::endl;
    std::cout << std::endl;
  }
}
} // namespace combinator_example

int main() {
  std::cout << "STLX Parser Examples" << std::endl;
  std::cout << "===================" << std::endl;

  try {
    expression_example::demonstrate_expression_parsing();
    statement_example::demonstrate_statement_parsing();
    json_example::demonstrate_json_parsing();
    regex_example::demonstrate_regex_parsing();
    combinator_example::demonstrate_combinator_parsing();

    std::cout << "\nAll examples completed successfully!" << std::endl;
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}