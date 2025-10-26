/**
 * @file parse_tree_printer.hpp
 * @brief AST generation and printing for parser combinator library
 *
 * This file provides functionality to build a runtime AST from parsed elements
 * and print the parse tree structure to stdout with proper indentation and
 * type information.
 *
 * @copyright Copyright (c) 2025 David Mott
 * @license Distributed under the Boost Software License, Version 1.0.
 *          (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef STLX_PARSE_TREE_PRINTER_HPP
#define STLX_PARSE_TREE_PRINTER_HPP

#include "stlx/parser.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <typeinfo>
#include <vector>

namespace stlx {
namespace parse {

// Forward declaration
template <typename Iterator> class parse_node;

// Parse node representing a node in the AST
template <typename Iterator>
class parse_node {
public:
    using iterator_type = Iterator;
    using children_type = std::vector<std::shared_ptr<parse_node>>;
    using string_type = std::string;

    const std::type_info& type_info;
    string_type rule_name;
    string_type matched_text;
    size_t start_pos;
    size_t end_pos;
    children_type children;

    template<typename Iter>
    parse_node(const std::type_info& ti, const string_type& name,
               const Iter& start, const Iter& end)
        : type_info(ti)
        , rule_name(name)
        , matched_text(start, end)
        , start_pos(0)
        , end_pos(0)
        , children() {}

    void add_child(std::shared_ptr<parse_node> child) {
        if (child) {
            children.push_back(child);
        }
    }

    [[nodiscard]] size_t size() const { return children.size(); }
};

// Template class for generating and printing AST
template <typename Iterator, typename GrammarRule>
class parse_tree_printer {
public:
    using iterator_type = Iterator;
    using grammar_type = GrammarRule;
    using node_type = parse_node<Iterator>;
    using node_ptr = std::shared_ptr<node_type>;
    using context_type = context<Iterator>;

    parse_tree_printer() = default;

    // Parse input and generate AST
    [[nodiscard]] node_ptr parse_to_ast(const std::string& input) {
        grammar_type grammar;
        // Create a mutable copy for iterators
        std::string mutable_input = input;
        typename std::string::iterator begin = mutable_input.begin();
        typename std::string::iterator end = mutable_input.end();
        context_type ctx(begin, end, false);  // Don't ignore whitespace for AST

        if (grammar.parse(ctx, begin, end)) {
            // Create a simple AST node for the root with proper name from parser
            std::string rule_name = grammar.name();
            return std::make_shared<node_type>(typeid(grammar), rule_name,
                                                mutable_input.begin(), begin);
        }
        return nullptr;
    }

    // Extract readable rule name from type_info
    [[nodiscard]] static std::string
    extract_rule_name(const std::type_info& ti) {
        std::string name = ti.name();

        // Demangle C++ type names (simplified)
        // Remove common prefixes
        size_t pos = name.find_last_of("::");
        if (pos != std::string::npos) {
            name = name.substr(pos + 1);
        }

        // Remove template parameters
        pos = name.find('<');
        if (pos != std::string::npos) {
            name = name.substr(0, pos);
        }

        return name;
    }

    // Print AST to stdout with indentation
    void print_ast(const node_ptr& node, int indent = 0,
                    const std::string& prefix = "") {
        if (!node) {
            return;
        }

        // Print current node
        std::string indent_str(indent * 2, ' ');
        std::cout << prefix << "`- " << node->rule_name;

        // Print matched text if it's short enough
        if (!node->matched_text.empty() && node->matched_text.length() <= 60) {
            // Escape special characters for display
            std::string display_text = node->matched_text;
            std::replace(display_text.begin(), display_text.end(), '\n', '\\');
            std::replace(display_text.begin(), display_text.end(), '\t', '\\');

            std::cout << " [" << display_text << "]";
        }

        std::cout << "\n";

        // Print children
        for (size_t i = 0; i < node->children.size(); ++i) {
            bool is_last = (i == node->children.size() - 1);
            std::string child_prefix = prefix + (is_last ? "    " : "|   ");
            std::string next_prefix = prefix + (is_last ? "    " : "|   ");
            print_ast(node->children[i], indent + 2, child_prefix);
        }
    }

    // Print AST in a compact format
    void print_ast_compact(const node_ptr& node, int depth = 0) {
        if (!node) {
            return;
        }

        // Indentation
        std::string indent(depth * 2, ' ');

        // Print node
        std::cout << indent << node->rule_name;

        // Print matched text in quotes if short
        if (!node->matched_text.empty() && node->matched_text.length() <= 40) {
            std::string display_text = node->matched_text;
            std::replace(display_text.begin(), display_text.end(), '\n', '\\');
            std::replace(display_text.begin(), display_text.end(), '\t', '\\');
            std::cout << ": \"" << display_text << "\"";
        }

        std::cout << "\n";

        // Print children
        for (const auto& child : node->children) {
            print_ast_compact(child, depth + 1);
        }
    }

    // Print parse statistics
    void print_statistics(const node_ptr& node) {
        if (!node) {
            std::cout << "No AST generated.\n";
            return;
        }

        std::cout << "=== Parse Tree Statistics ===\n";
        std::cout << "Total nodes: " << count_nodes(node) << "\n";
        std::cout << "Max depth: " << max_depth(node) << "\n";
        std::cout << "Root type: " << node->rule_name << "\n";
        std::cout << "Matched text length: " << node->matched_text.length()
                  << " characters\n";
        std::cout << "Children: " << node->children.size() << "\n";
        std::cout << "============================\n";
    }

private:
    [[nodiscard]] size_t count_nodes(const node_ptr& node) const {
        if (!node) {
            return 0;
        }
        size_t count = 1;
        for (const auto& child : node->children) {
            count += count_nodes(child);
        }
        return count;
    }

    [[nodiscard]] size_t max_depth(const node_ptr& node) const {
        if (!node) {
            return 0;
        }
        if (node->children.empty()) {
            return 1;
        }
        size_t max = 0;
        for (const auto& child : node->children) {
            size_t depth = max_depth(child);
            if (depth > max) {
                max = depth;
            }
        }
        return max + 1;
    }

    [[nodiscard]] node_ptr build_ast_from_child(
        const std::shared_ptr<rule_base<Iterator>>& child,
        const iterator_type& start, const iterator_type& end) {
        // This is a simplified version - in a full implementation,
        // you'd need to properly track the parse position for each child
        std::string name = extract_rule_name(child->type());
        return std::make_shared<node_type>(child->type(), name, start, end);
    }
};

// Helper function to parse and print AST in one call
template <typename Iterator, typename GrammarRule>
void print_parse_tree(const std::string& input) {
    parse_tree_printer<Iterator, GrammarRule> printer;
    auto ast = printer.parse_to_ast(input);

    if (ast) {
        std::cout << "\n=== Parse Tree ===\n\n";
        printer.print_ast(ast);
        std::cout << "\n=== Compact Format ===\n\n";
        printer.print_ast_compact(ast);
        std::cout << "\n";
        printer.print_statistics(ast);
    } else {
        std::cout << "Parse failed - no AST generated.\n";
    }
}

}  // namespace parse
}  // namespace stlx

#endif  // STLX_PARSE_TREE_PRINTER_HPP

