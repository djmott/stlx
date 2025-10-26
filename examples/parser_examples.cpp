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
using lparen = character<std::string::const_iterator, '('>;
using rparen = character<std::string::const_iterator, ')'>;

// Define non-terminals using CRTP pattern
struct number_rule : rule<std::string::const_iterator, number_rule,
                         one_or_more_<std::string::const_iterator, digit>> {};

struct term_rule : rule<std::string::const_iterator, term_rule,
                       or_<std::string::const_iterator, number_rule,
                           and_<std::string::const_iterator, lparen, number_rule, rparen>>> {};

struct expression_rule : rule<std::string::const_iterator, expression_rule,
                             and_<std::string::const_iterator, term_rule,
                                 zero_or_more_<std::string::const_iterator,
                                             and_<std::string::const_iterator, plus, term_rule>>>> {};

void demonstrate_expression_parsing() {
  std::cout << "\n=== Expression Parsing Example ===" << std::endl;

  std::vector<std::string> test_expressions = {"123", "45+67", "1+2+3",
                                               "invalid"};

  for (const auto &expr : test_expressions) {
    std::cout << "Parsing: '" << expr << "'" << std::endl;

    std::shared_ptr<expression_rule> ast;
    bool success = stlx::parser<expression_rule>::parse(expr.cbegin(), expr.cend(), ast, false);

    std::cout << "Result: " << (success ? "SUCCESS" : "FAILED") << std::endl;
    if (!success) {
      std::cout << "Errors encountered during parsing" << std::endl;
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

// Define using CRTP pattern
struct identifier_rule : rule<std::string::const_iterator, identifier_rule,
                              one_or_more_<std::string::const_iterator, letter>> {};

struct number_rule : rule<std::string::const_iterator, number_rule,
                         one_or_more_<std::string::const_iterator, digit>> {};

struct assignment_rule : rule<std::string::const_iterator, assignment_rule,
                             and_<std::string::const_iterator, identifier_rule,
                                 equals, number_rule, semicolon>> {};

void demonstrate_statement_parsing() {
  std::cout << "\n=== Statement Parsing Example ===" << std::endl;

  std::vector<std::string> test_statements = {"x=42;", "y=123;", "invalid",
                                              "a=;"};

  for (const auto &stmt : test_statements) {
    std::cout << "Parsing: '" << stmt << "'" << std::endl;

    std::shared_ptr<assignment_rule> ast;
    bool success = stlx::parser<assignment_rule>::parse(stmt.cbegin(), stmt.cend(), ast, false);

    std::cout << "Result: " << (success ? "SUCCESS" : "FAILED") << std::endl;
    if (!success) {
      std::cout << "Errors encountered during parsing" << std::endl;
    }
    std::cout << std::endl;
  }
}
} // namespace statement_example

// JSON-like parsing example
namespace json_example {
using quote = character<std::string::const_iterator, '"'>;
using letter = characters<std::string::const_iterator, 'a', 'z'>;
using digit = characters<std::string::const_iterator, '0', '9'>;

// Define using CRTP pattern
struct string_char_rule : rule<std::string::const_iterator, string_char_rule,
                               or_<std::string::const_iterator, letter, digit,
                                   character<std::string::const_iterator, ' '>>> {};

struct json_string_rule : rule<std::string::const_iterator, json_string_rule,
                              and_<std::string::const_iterator, quote,
                                  zero_or_more_<std::string::const_iterator, string_char_rule>,
                                  quote>> {};

struct json_number_rule : rule<std::string::const_iterator, json_number_rule,
                              one_or_more_<std::string::const_iterator, digit>> {};

struct json_value_rule : rule<std::string::const_iterator, json_value_rule,
                             or_<std::string::const_iterator, json_string_rule, json_number_rule>> {};

void demonstrate_json_parsing() {
  std::cout << "\n=== JSON-like Parsing Example ===" << std::endl;

  std::vector<std::string> test_json = {"\"hello\"", "123", "\"test value\"",
                                        "invalid"};

  for (const auto &json : test_json) {
    std::cout << "Parsing: '" << json << "'" << std::endl;

    std::shared_ptr<json_value_rule> ast;
    bool success = stlx::parser<json_value_rule>::parse(json.cbegin(), json.cend(), ast, false);

    std::cout << "Result: " << (success ? "SUCCESS" : "FAILED") << std::endl;
    if (!success) {
      std::cout << "Errors encountered during parsing" << std::endl;
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
    std::shared_ptr<email_regex> ast;
    bool success = stlx::parser<email_regex>::parse(email.cbegin(), email.cend(), ast, false);
    std::cout << "  '" << email << "': " << (success ? "MATCH" : "NO MATCH")
              << std::endl;
  }

  // Test phone regex
  std::cout << "\nTesting phone regex:" << std::endl;
  for (const auto &phone : test_phones) {
    std::shared_ptr<phone_regex> ast;
    bool success = stlx::parser<phone_regex>::parse(phone.cbegin(), phone.cend(), ast, false);
    std::cout << "  '" << phone << "': " << (success ? "MATCH" : "NO MATCH")
              << std::endl;
  }

  // Test IP regex
  std::cout << "\nTesting IP regex:" << std::endl;
  for (const auto &ip : test_ips) {
    std::shared_ptr<ip_regex> ast;
    bool success = stlx::parser<ip_regex>::parse(ip.cbegin(), ip.cend(), ast, false);
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

// Define using CRTP pattern
struct identifier_char_rule : rule<std::string::const_iterator, identifier_char_rule,
                                    or_<std::string::const_iterator, letter, digit, underscore>> {};

struct identifier_rule : rule<std::string::const_iterator, identifier_rule,
                              and_<std::string::const_iterator, letter,
                                  zero_or_more_<std::string::const_iterator, identifier_char_rule>>> {};

void demonstrate_combinator_parsing() {
  std::cout << "\n=== Combinator Parsing Example ===" << std::endl;

  std::vector<std::string> test_identifiers = {"variable", "var123", "my_var",
                                               "123invalid", "_private"};

  for (const auto &ident : test_identifiers) {
    std::cout << "Parsing identifier: '" << ident << "'" << std::endl;

    std::shared_ptr<identifier_rule> ast;
    bool success = stlx::parser<identifier_rule>::parse(ident.cbegin(), ident.cend(), ast, false);

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