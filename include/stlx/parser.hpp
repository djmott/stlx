/**
 * @file parser.hpp
 * @brief Parser combinator library for C++23
 *
 * This file provides a comprehensive parser combinator library that allows
 * building complex parsers from simple, composable components. It includes
 * support for character parsing, string matching, regex patterns, and various
 * combinators like AND, OR, NOT, and quantifiers (zero_or_more, one_or_more,
 * etc.).
 *
 * @copyright Copyright (c) 2025 David Mott
 * @license Distributed under the Boost Software License, Version 1.0.
 *          (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */

 #ifndef STLX_PARSER_HPP
 #define STLX_PARSER_HPP
 
 #include <algorithm>
 #include <cctype>
 #include <memory>
 #include <regex>
 #include <string>
 #include <string_view>
 #include <typeinfo>
 #include <vector>
 
 namespace stlx {
 
 namespace parse {
 
 // Forward declarations
 template <typename Iterator> class rule_base;
 
 // Helper function to demangle and prettify type names
 inline std::string demangle_type_name(const char* mangled_name) {
     std::string name(mangled_name);
     
     // Handle GCC/Clang mangled names (N...E format for nested types)
     if (!name.empty() && name[0] == 'N' && name.back() == 'E') {
         name = name.substr(1);  // Remove leading N
         name.pop_back();        // Remove trailing E
         
         // Now parse the structure: <len><name><len><name>...
         // Numbers indicate the length of the following name
         std::string result;
         std::string::size_type i = 0;
         
         while (i < name.size()) {
             // Extract number (length)
             std::string num_str;
             while (i < name.size() && std::isdigit(name[i])) {
                 num_str += name[i];
                 ++i;
             }
             
             if (!num_str.empty() && !result.empty()) {
                 // Insert :: before this component
                 result += "::";
             }
             
             // Extract name of given length
             if (!num_str.empty()) {
                 int len = std::stoi(num_str);
                 if (i + len <= name.size()) {
                     result += name.substr(i, len);
                     i += len;
                 } else {
                     break;
                 }
             } else {
                 break;
             }
         }
         
         return result.empty() ? name : result;
     }
     
     // For simple names, just clean them up
     // Remove template parameters
     size_t pos = name.find('<');
     if (pos != std::string::npos) {
         name = name.substr(0, pos);
     }
     
     // Remove namespace markers at the end
     pos = name.find_last_of(':');
     if (pos != std::string::npos && pos < name.size() - 1 && name[pos + 1] == ':') {
         name = name.substr(pos + 2);
     }
     
     return name;
 }
 
 // Skip whitespace helper function
 template <typename Iterator>
 void skip_ws(Iterator &begin, Iterator &end, bool ignore_whitespace) {
   if (!ignore_whitespace) {
     return;
   }
   while (begin != end && (*begin == ' ' || *begin == '\t' || *begin == '\n' ||
                           *begin == '\r')) {
     ++begin;
   }
 }
 
 // Parse error structure
 template <typename iterator_t> struct parse_error {
   using iterator_type = iterator_t;
   using ptr = std::shared_ptr<parse_error>;
   using vector = std::vector<ptr>;
   const std::type_info &failed_rule;
   const iterator_type position;
   std::string expected;
   std::string found;
 
   parse_error(const std::type_info &failRule, const iterator_type pos,
               std::string expected_msg = "", std::string found_msg = "")
       : failed_rule(failRule), position(pos), expected(std::move(expected_msg)),
         found(std::move(found_msg)) {}
 };
 
 // Context structure
 template <typename iterator_t> struct context {
   using iterator_type = iterator_t;
   iterator_type begin;
   iterator_type end;
   bool ignore_whitespace;
   std::shared_ptr<rule_base<iterator_t>> start_rule;
   typename parse_error<iterator_t>::vector parse_errors;
 
   context(iterator_type begin_iter, iterator_type end_iter,
           bool ignore_ws = true)
       : begin(begin_iter), end(end_iter), ignore_whitespace(ignore_ws) {}
 };
 
// Rule base class with proper iterator type handling
template <typename Iterator>
class rule_base : public std::vector<std::shared_ptr<rule_base<Iterator>>>,
                  public std::enable_shared_from_this<rule_base<Iterator>> {
protected:
  Iterator _matched_begin;
  Iterator _matched_end;

public:
  using iterator_type = Iterator;
  using pointer_type = std::shared_ptr<rule_base<Iterator>>;
  using weak_ptr_t = std::weak_ptr<rule_base<Iterator>>;
  using super_t = std::vector<pointer_type>;

  // Constructor: accepts iterator range for matched text
  template <typename... _child_ts>
  rule_base(Iterator matched_begin, Iterator matched_end, _child_ts &&...oChildRules)
    : super_t{std::forward<_child_ts>(oChildRules)...}, 
      _matched_begin(matched_begin), 
      _matched_end(matched_end),
      _parent() {}

  // Default constructor for combinators that don't match text
  rule_base()
    : super_t{}, _parent() {}

  virtual ~rule_base() = default;

  [[nodiscard]] virtual bool isa(const std::type_info &oType) const = 0;
  [[nodiscard]] virtual const std::type_info &type() const = 0;
  [[nodiscard]] virtual std::string name() const = 0;

  pointer_type parent() { return _parent.lock(); }

  virtual std::string_view get_text() const {
    if (_matched_begin != _matched_end) {
      return std::string_view(&*_matched_begin, std::distance(_matched_begin, _matched_end));
    }
    return "";
  }
 
   // Virtual size() method that can be overridden by quantifier combinators
   [[nodiscard]] virtual size_t size() const { return super_t::size(); }
 
 protected:
   void set_parent(weak_ptr_t oParent) {
     _parent = oParent;
     auto oThis = this->shared_from_this();
     for (auto &oChild : static_cast<super_t &>(*this)) {
       oChild->set_parent(oThis);
     }
   }
 
 private:
   std::weak_ptr<rule_base<Iterator>> _parent;
 };
 
 // Helper to clone rules for AST building
 template <typename RuleT>
 std::shared_ptr<RuleT> clone_rule(const std::shared_ptr<RuleT> &src) {
   if constexpr (std::is_abstract_v<RuleT>) {
     // For abstract types, we can't clone directly
     // This is a limitation of the current design
     return nullptr;
   } else {
     auto clone = std::make_shared<RuleT>(*src);
     return clone;
   }
 }
 
// Rule template using CRTP with proper iterator type handling
template <typename Iterator, typename _decl_t, typename _impl_t = _decl_t>
class rule : public rule_base<Iterator> {
public:
  using iterator_type = Iterator;
  using decl_type = _decl_t;
  using impl_type = _impl_t;
  using rule_type = rule<Iterator, _decl_t, _impl_t>;

  // Constructor for terminals with iterators
  rule(Iterator matched_begin, Iterator matched_end)
    : rule_base<Iterator>(matched_begin, matched_end) {}
  
  // Constructor for combinators without iterators
  rule()
    : rule_base<Iterator>() {}

  virtual ~rule() = default;
 
   bool isa(const std::type_info &oType) const override {
     return (typeid(rule) == oType) || (typeid(decl_type) == oType) ||
            (typeid(impl_type) == oType) ||
            (typeid(rule_base<Iterator>) == oType);
   }
 
   const std::type_info &type() const override { return typeid(_decl_t); }
 
   std::string name() const override { return demangle_type_name(typeid(_decl_t).name()); }
 
   static bool parse(context<iterator_type> &ctx, iterator_type &begin,
                     iterator_type &end,
                     std::shared_ptr<rule_base<iterator_type>>& ast) {
     // Always delegate to impl_type first (the actual combinator implementation)
     return impl_type::parse(ctx, begin, end, ast);
   }
 };
 
 // NOT combinator with proper template handling
 template <typename Iterator, typename...> class not_;
 
template <typename Iterator>
class not_<Iterator> : public rule<Iterator, not_<Iterator>> {
public:
  using iterator_type = Iterator;
  using _super_t = rule<Iterator, not_<Iterator>>;

  // Default constructor for combinators
  not_() : rule<Iterator, not_<Iterator>>() {}
};
 
template <typename Iterator, typename _head_t, typename... _tail_ts>
class not_<Iterator, _head_t, _tail_ts...>
    : public rule<Iterator, not_<Iterator, _head_t, _tail_ts...>> {
public:
  using iterator_type = Iterator;
  using _super_t = rule<Iterator, not_<Iterator, _head_t, _tail_ts...>>;

  // Constructor for combinators with matched text range
  not_(Iterator matched_begin, Iterator matched_end)
    : rule<Iterator, not_<Iterator, _head_t, _tail_ts...>>(matched_begin, matched_end) {}

  static bool parse(context<iterator_type> &ctx, iterator_type &begin,
                    iterator_type &end,
                    std::shared_ptr<rule_base<Iterator>>& ast) {
    auto backup = begin;
    
    // Try to parse child - if it succeeds, NOT fails
    std::shared_ptr<rule_base<Iterator>> test_ast;
    if (_head_t::parse(ctx, begin, end, test_ast)) {
      // Child parsed successfully - NOT fails
      ctx.parse_errors.emplace_back(
          std::make_shared<parse_error<iterator_type>>(
              typeid(not_<Iterator, _head_t, _tail_ts...>), backup,
              "Negative assertion failed",
              begin != end ? std::string(1, *begin) : "EOF"));
      begin = backup;
      return false;
    }
    
    // Child did NOT parse - NOT succeeds (empty match: backup to backup)
    begin = backup;
    ast = std::make_shared<not_<Iterator, _head_t, _tail_ts...>>(backup, backup);
    return true;
  }
};
 
// AND combinator (sequence implementation)
template <typename Iterator, typename...> class and_;
template <typename Iterator>
class and_<Iterator> : public rule<Iterator, and_<Iterator>> {
public:
  using iterator_type = Iterator;
  using _super_t = rule<Iterator, and_<Iterator>>;

  // Default constructor for combinators
  and_() : rule<Iterator, and_<Iterator>>() {}
};

// Helper function to parse tail types recursively
// Base case: no tail types
template <typename Iterator>
bool parse_tail(context<Iterator> &ctx, Iterator &begin, Iterator &end,
                 std::vector<std::shared_ptr<rule_base<Iterator>>> &child_asts) {
  return true; // All tail types parsed successfully
}

// Recursive case: parse first tail type, then remaining
template <typename Iterator, typename HeadTail, typename... RestTail>
bool parse_tail(context<Iterator> &ctx, Iterator &begin, Iterator &end,
                 std::vector<std::shared_ptr<rule_base<Iterator>>> &child_asts) {
  std::shared_ptr<rule_base<Iterator>> tail_ast;
  if (!HeadTail::parse(ctx, begin, end, tail_ast)) {
    return false;
  }
  child_asts.push_back(tail_ast);
  
  // Recursively parse remaining tail types
  return parse_tail<Iterator, RestTail...>(ctx, begin, end, child_asts);
}

template <typename Iterator, typename _head_t, typename... _tail_ts>
class and_<Iterator, _head_t, _tail_ts...>
    : public rule<Iterator, and_<Iterator, _head_t, _tail_ts...>> {
public:
  using iterator_type = Iterator;
  using _super_t = rule<Iterator, and_<Iterator, _head_t, _tail_ts...>>;

  // Constructor for combinators with matched text range
  and_(Iterator matched_begin, Iterator matched_end)
    : rule<Iterator, and_<Iterator, _head_t, _tail_ts...>>(matched_begin, matched_end) {}

  static bool parse(context<iterator_type> &ctx, iterator_type &begin,
                    iterator_type &end,
                    std::shared_ptr<rule_base<Iterator>>& ast) {
    auto backup = begin;
    std::vector<std::shared_ptr<rule_base<Iterator>>> child_asts;

    // Parse first child (_head_t)
    std::shared_ptr<rule_base<Iterator>> head_ast;
    if (!_head_t::parse(ctx, begin, end, head_ast)) {
      begin = backup;
      return false;
    }
    child_asts.push_back(head_ast);

    // Parse remaining children (_tail_ts) recursively
    if (!parse_tail<Iterator, _tail_ts...>(ctx, begin, end, child_asts)) {
      begin = backup;
      return false;
    }

    // All children parsed - create instance with matched text range
    Iterator match_start = backup;
    Iterator match_end = begin;
    ast = std::make_shared<and_<Iterator, _head_t, _tail_ts...>>(match_start, match_end);
    for (auto& child : child_asts) {
      ast->push_back(child);
    }
    return true;
  }
};
 
// OR combinator with proper template handling
template <typename Iterator, typename...> class or_;

// Helper function to try parsing tail types for OR
// Base case: no tail types left to try
template <typename Iterator>
bool try_parse_tail_or(context<Iterator> &ctx, Iterator &begin, Iterator &end,
                       std::shared_ptr<rule_base<Iterator>> &tail_ast) {
  return false; // No more alternatives to try
}

// Recursive case: try first tail type, then remaining
template <typename Iterator, typename HeadTail, typename... RestTail>
bool try_parse_tail_or(context<Iterator> &ctx, Iterator &begin, Iterator &end,
                       std::shared_ptr<rule_base<Iterator>> &tail_ast) {
  // Try to parse this tail type
  if (HeadTail::parse(ctx, begin, end, tail_ast)) {
    return true; // Success!
  }
  
  // This tail type failed - try remaining ones
  return try_parse_tail_or<Iterator, RestTail...>(ctx, begin, end, tail_ast);
}

template <typename Iterator>
class or_<Iterator> : public rule<Iterator, or_<Iterator>> {
public:
  using iterator_type = Iterator;
  using _super_t = rule<Iterator, or_<Iterator>>;

  // Default constructor for combinators
  or_() : rule<Iterator, or_<Iterator>>() {}
};

template <typename Iterator, typename _head_t, typename... _tail_ts>
class or_<Iterator, _head_t, _tail_ts...>
    : public rule<Iterator, or_<Iterator, _head_t, _tail_ts...>> {
public:
  using iterator_type = Iterator;
  using _super_t = rule<Iterator, or_<Iterator, _head_t, _tail_ts...>>;

  // Constructor for combinators with matched text range
  or_(Iterator matched_begin, Iterator matched_end)
    : rule<Iterator, or_<Iterator, _head_t, _tail_ts...>>(matched_begin, matched_end) {}

  static bool parse(context<iterator_type> &ctx, iterator_type &begin,
                    iterator_type &end,
                    std::shared_ptr<rule_base<Iterator>>& ast) {
    auto backup = begin;
    typename parse_error<iterator_type>::vector branch_errors;

    // Try to parse first child (_head_t)
    std::shared_ptr<rule_base<Iterator>> head_ast;
    auto head_backup = begin;
    if (_head_t::parse(ctx, begin, end, head_ast)) {
      // Success - return this child with matched text range
      Iterator match_start = backup;
      Iterator match_end = begin;
      ast = std::make_shared<or_<Iterator, _head_t, _tail_ts...>>(match_start, match_end);
      ast->push_back(head_ast);
      return true;
    }
    
    branch_errors.emplace_back(std::make_shared<parse_error<iterator_type>>(
        typeid(_head_t), head_backup, "Alternative branch failed",
        head_backup != end ? std::string(1, *head_backup) : "EOF"));
    begin = backup;

    // Try remaining children (_tail_ts) recursively
    std::shared_ptr<rule_base<Iterator>> tail_ast;
    if (try_parse_tail_or<Iterator, _tail_ts...>(ctx, begin, end, tail_ast)) {
      Iterator match_start = backup;
      Iterator match_end = begin;
      ast = std::make_shared<or_<Iterator, _head_t, _tail_ts...>>(match_start, match_end);
      ast->push_back(tail_ast);
      return true;
    }

    // All alternatives failed
    ctx.parse_errors.insert(ctx.parse_errors.end(), branch_errors.begin(),
                            branch_errors.end());
    return false;
  }
};
 
// ONE_OR_MORE combinator with static parsing
template <typename Iterator, typename _ty>
class one_or_more_ : public rule<Iterator, one_or_more_<Iterator, _ty>> {
public:
  using iterator_type = Iterator;
  using _super_t = rule<Iterator, one_or_more_<Iterator, _ty>>;

  // Constructor for combinators with matched text range
  one_or_more_(Iterator matched_begin, Iterator matched_end)
    : rule<Iterator, one_or_more_<Iterator, _ty>>(matched_begin, matched_end) {}

  static bool parse(context<iterator_type> &ctx, iterator_type &begin,
                    iterator_type &end,
                    std::shared_ptr<rule_base<Iterator>>& ast) {
    auto backup = begin;
    std::vector<std::shared_ptr<rule_base<Iterator>>> child_asts;

    // Must match at least once
    std::shared_ptr<rule_base<Iterator>> first_ast;
    if (!_ty::parse(ctx, begin, end, first_ast)) {
      ctx.parse_errors.emplace_back(
          std::make_shared<parse_error<iterator_type>>(
              typeid(one_or_more_<Iterator, _ty>), backup,
              "One_or_more requires at least one match",
              begin != end ? std::string(1, *begin) : "EOF"));
      begin = backup;
      return false;
    }
    child_asts.push_back(first_ast);

    // Try to match more
    while (true) {
      auto loop_backup = begin;
      if (ctx.ignore_whitespace)
        skip_ws(begin, end, ctx.ignore_whitespace);
      
      std::shared_ptr<rule_base<Iterator>> child_ast;
      if (begin == end || !_ty::parse(ctx, begin, end, child_ast)) {
        begin = loop_backup;
        break;
      }
      
      // Prevent infinite loop: if no progress was made, break
      if (begin == loop_backup) {
        begin = loop_backup;
        break;
      }
      
      child_asts.push_back(child_ast);
    }

    // Create instance with all matched children and matched text range
    Iterator match_start = backup;
    Iterator match_end = begin;
    ast = std::make_shared<one_or_more_<Iterator, _ty>>(match_start, match_end);
    for (auto& child : child_asts) {
      ast->push_back(child);
    }
    return true;
  }

  // Override size() to return the number of matches (children count)
  [[nodiscard]] size_t size() const override { 
    return rule_base<Iterator>::size(); // Returns number of child ASTs matched
  }
};
 
// ZERO_OR_MORE combinator with static parsing
template <typename Iterator, typename _ty>
class zero_or_more_ : public rule<Iterator, zero_or_more_<Iterator, _ty>> {
public:
  using iterator_type = Iterator;
  using _super_t = rule<Iterator, zero_or_more_<Iterator, _ty>>;

  // Constructor for combinators with matched text range
  zero_or_more_(Iterator matched_begin, Iterator matched_end)
    : rule<Iterator, zero_or_more_<Iterator, _ty>>(matched_begin, matched_end) {}

  static bool parse(context<iterator_type> &ctx, iterator_type &begin,
                    iterator_type &end,
                    std::shared_ptr<rule_base<Iterator>>& ast) {
    auto backup = begin;
    std::vector<std::shared_ptr<rule_base<Iterator>>> child_asts;

    // Try to match as many as possible (zero or more)
    while (true) {
      auto loop_backup = begin;
      if (ctx.ignore_whitespace)
        skip_ws(begin, end, ctx.ignore_whitespace);
      
      std::shared_ptr<rule_base<Iterator>> child_ast;
      if (begin == end || !_ty::parse(ctx, begin, end, child_ast)) {
        begin = loop_backup;
        break;
      }
      
      // Prevent infinite loop: if no progress was made, break
      if (begin == loop_backup) {
        begin = loop_backup;
        break;
      }
      
      child_asts.push_back(child_ast);
    }

    // Create instance with all matched children (may be empty) and matched text range
    Iterator match_start = backup;
    Iterator match_end = begin;
    ast = std::make_shared<zero_or_more_<Iterator, _ty>>(match_start, match_end);
    for (auto& child : child_asts) {
      ast->push_back(child);
    }
    return true;
  }

  // Override size() to return the number of matches (children count)
  [[nodiscard]] size_t size() const override { 
    return rule_base<Iterator>::size(); // Returns number of child ASTs matched
  }
};
 
// ZERO_OR_ONE combinator with static parsing
template <typename Iterator, typename _ty>
class zero_or_one_ : public rule<Iterator, zero_or_one_<Iterator, _ty>> {
public:
  using iterator_type = Iterator;
  using _super_t = rule<Iterator, zero_or_one_<Iterator, _ty>>;

  // Constructor for combinators with matched text range
  zero_or_one_(Iterator matched_begin, Iterator matched_end)
    : rule<Iterator, zero_or_one_<Iterator, _ty>>(matched_begin, matched_end) {}

  static bool parse(context<iterator_type> &ctx, iterator_type &begin,
                    iterator_type &end,
                    std::shared_ptr<rule_base<Iterator>>& ast) {
    auto backup = begin;
    std::vector<std::shared_ptr<rule_base<Iterator>>> child_asts;

    // Try to match once (zero or one)
    if (ctx.ignore_whitespace)
      skip_ws(begin, end, ctx.ignore_whitespace);
    
    std::shared_ptr<rule_base<Iterator>> child_ast;
    if (begin != end && _ty::parse(ctx, begin, end, child_ast)) {
      child_asts.push_back(child_ast);
    } else {
      begin = backup; // Reset if didn't match
    }

    // Create instance (may have zero or one child) with matched text range
    Iterator match_start = backup;
    Iterator match_end = begin;
    ast = std::make_shared<zero_or_one_<Iterator, _ty>>(match_start, match_end);
    for (auto& child : child_asts) {
      ast->push_back(child);
    }
    return true;
  }

  // Override size() to return the number of matches (children count)
  [[nodiscard]] size_t size() const override { 
    return rule_base<Iterator>::size(); // Returns number of child ASTs matched (0 or 1)
  }
};
 
// STRING terminal with iterator-based construction
template <typename Iterator, size_t _len, char (&_str)[_len]>
class string : public rule<Iterator, string<Iterator, _len, _str>> {
public:
  using iterator_type = Iterator;
  using _super_t = rule<Iterator, string<Iterator, _len, _str>>;
  static constexpr size_t length = _len - 1;

  // Constructor: accepts iterators pointing to the matched text range
  string(Iterator matched_begin, Iterator matched_end)
    : rule<Iterator, string<Iterator, _len, _str>>(matched_begin, matched_end) {}

  static bool parse(context<iterator_type> &ctx, iterator_type &begin,
                    iterator_type &end,
                    std::shared_ptr<rule_base<Iterator>>& ast) {
    auto backup = begin;
    if (ctx.ignore_whitespace)
      skip_ws(begin, end, ctx.ignore_whitespace);
    
    // Store the start of the matched text
    Iterator match_start = begin;
    
    constexpr const char *lit = _str;
    for (size_t i = 0; i < length; ++i) {
      if (begin == end || *begin != lit[i]) {
        ctx.parse_errors.emplace_back(
            std::make_shared<parse_error<iterator_type>>(
                typeid(string<Iterator, _len, _str>), backup, 
                std::string("Expected string: ") + lit,
                begin != end ? std::string(1, *begin) : "EOF"));
        begin = backup;
        return false;
      }
      ++begin;
    }
    
    // Parse succeeded - capture end of iterator range
    Iterator match_end = begin;
    
    // Create instance NOW, after successful parse
    ast = std::make_shared<string<Iterator, _len, _str>>(match_start, match_end);
    return true;
  }

  std::string_view get_text() const override {
    return std::string_view(&*this->_matched_begin, 
                            std::distance(this->_matched_begin, this->_matched_end));
  }
};
 
// CHARACTER terminal with iterator-based construction
template <typename Iterator, char _value>
class character : public rule<Iterator, character<Iterator, _value>> {
public:
  using iterator_type = Iterator;
  
  // Constructor: accepts iterators pointing to the matched text range
  character(Iterator matched_begin, Iterator matched_end)
    : rule<Iterator, character<Iterator, _value>>(matched_begin, matched_end) {}

  static bool parse(context<iterator_type> &ctx, iterator_type &begin,
                    iterator_type &end,
                    std::shared_ptr<rule_base<Iterator>>& ast) {
    auto backup = begin;
    if (ctx.ignore_whitespace)
      skip_ws(begin, end, ctx.ignore_whitespace);
    if (begin == end || *begin != _value) {
      ctx.parse_errors.emplace_back(
          std::make_shared<parse_error<iterator_type>>(
              typeid(character<Iterator, _value>), backup,
              std::string("Expected character: '") + _value + "'",
              begin != end ? std::string(1, *begin) : "EOF"));
      begin = backup;
      return false;
    }
    
    // Parse succeeded - capture iterator range
    Iterator match_start = begin;
    ++begin;
    Iterator match_end = begin;
    
    // Create instance NOW, after successful parse
    ast = std::make_shared<character<Iterator, _value>>(match_start, match_end);
    return true;
  }

  std::string_view get_text() const override {
    return std::string_view(&*this->_matched_begin, 
                            std::distance(this->_matched_begin, this->_matched_end));
  }
};
 
// CHARACTERS range terminal with iterator-based construction
template <typename Iterator, char _first, char _last>
class characters : public rule<Iterator, characters<Iterator, _first, _last>> {
public:
  using iterator_type = Iterator;
  using _super_t = rule<Iterator, characters<Iterator, _first, _last>>;

  // Constructor: accepts iterators pointing to the matched text range
  characters(Iterator matched_begin, Iterator matched_end)
    : rule<Iterator, characters<Iterator, _first, _last>>(matched_begin, matched_end) {}

  static bool parse(context<iterator_type> &ctx, iterator_type &begin,
                    iterator_type &end,
                    std::shared_ptr<rule_base<Iterator>>& ast) {
    auto backup = begin;
    if (ctx.ignore_whitespace)
      skip_ws(begin, end, ctx.ignore_whitespace);
    if (begin == end) {
      ctx.parse_errors.emplace_back(
          std::make_shared<parse_error<iterator_type>>(
              typeid(characters<Iterator, _first, _last>), backup, 
              "Expected character in range", "EOF"));
      begin = backup;
      return false;
    }

    char c = *begin;
    if constexpr (_first <= _last) {
      if (c < _first || c > _last) {
        ctx.parse_errors.emplace_back(
            std::make_shared<parse_error<iterator_type>>(
                typeid(characters<Iterator, _first, _last>), backup,
                std::string("Expected character in range [") + _first + "-" +
                    _last + "]",
                std::string(1, c)));
        begin = backup;
        return false;
      }
    } else {
      if (c > _first || c < _last) {
        ctx.parse_errors.emplace_back(
            std::make_shared<parse_error<iterator_type>>(
                typeid(characters<Iterator, _first, _last>), backup,
                std::string("Expected character in range [") + _last + "-" +
                    _first + "]",
                std::string(1, c)));
        begin = backup;
        return false;
      }
    }

    // Parse succeeded - capture iterator range
    Iterator match_start = begin;
    ++begin;
    Iterator match_end = begin;
    
    // Create instance NOW, after successful parse
    ast = std::make_shared<characters<Iterator, _first, _last>>(match_start, match_end);
    return true;
  }

  std::string_view get_text() const override {
    return std::string_view(&*this->_matched_begin, 
                            std::distance(this->_matched_begin, this->_matched_end));
  }
};
 
// REGEX terminal with iterator-based construction
template <typename Iterator, size_t _len, char (&_str)[_len]>
class regex : public rule<Iterator, regex<Iterator, _len, _str>> {
private:
  static std::regex &get_pattern() {
    static std::regex pattern(_str);
    return pattern;
  }

public:
  using iterator_type = Iterator;
  using _super_t = rule<Iterator, regex<Iterator, _len, _str>>;
  static constexpr size_t length = _len - 1;

  // Constructor: accepts iterators pointing to the matched text range
  regex(Iterator matched_begin, Iterator matched_end)
    : rule<Iterator, regex<Iterator, _len, _str>>(matched_begin, matched_end) {}

  static bool parse(context<iterator_type> &ctx, iterator_type &begin,
                    iterator_type &end,
                    std::shared_ptr<rule_base<Iterator>>& ast) {
    auto backup = begin;
    if (ctx.ignore_whitespace)
      skip_ws(begin, end, ctx.ignore_whitespace);

    if (begin == end) {
      ctx.parse_errors.emplace_back(
          std::make_shared<parse_error<iterator_type>>(
              typeid(regex<Iterator, _len, _str>), backup, 
              "Expected regex match", "EOF"));
      begin = backup;
      return false;
    }

    // Store the start of the matched text
    Iterator match_start = begin;

    std::match_results<iterator_type> matches;
    auto &pattern = get_pattern();

    if (!std::regex_search(begin, end, matches, pattern,
                           std::regex_constants::match_continuous)) {
      ctx.parse_errors.emplace_back(
          std::make_shared<parse_error<iterator_type>>(
              typeid(regex<Iterator, _len, _str>), backup, 
              std::string("Expected regex: ") + _str,
              begin != end ? std::string(begin, begin + std::min<size_t>(
                                                            10, end - begin)) +
                                 "..."
                           : "EOF"));
      begin = backup;
      return false;
    }

    if (matches.empty() || matches.position(0) != 0) {
      ctx.parse_errors.emplace_back(
          std::make_shared<parse_error<iterator_type>>(
              typeid(regex<Iterator, _len, _str>), backup, 
              "Regex match not at beginning",
              std::string(begin, begin + std::min<size_t>(10, end - begin)) +
                  "..."));
      begin = backup;
      return false;
    }

    // Parse succeeded - calculate end position
    auto matched_length = matches.str(0).length();
    Iterator match_end = begin + matched_length;
    begin = match_end;
    
    // Create instance NOW, after successful parse
    ast = std::make_shared<regex<Iterator, _len, _str>>(match_start, match_end);
    return true;
  }

  std::string_view get_text() const override {
    return std::string_view(&*this->_matched_begin, 
                            std::distance(this->_matched_begin, this->_matched_end));
  }
};
 
 // WHITESPACE definition
 template <char... _chs> class whitespace {
 public:
   using whitespace_type = whitespace<_chs...>;
 };
 
 // Macros for convenience - fixed to use proper template parameters
 #define STRING_(x)                                                             \
   namespace _ {                                                                \
   char x[] = #x;                                                               \
   }                                                                            \
   using x =                                                                    \
       stlx::parse::string<std::string::const_iterator, sizeof(_::x), _::x>;
 
 #define STRING(_name, _value)                                                  \
   namespace _ {                                                                \
   char _name[] = _value;                                                       \
   }                                                                            \
   using _name = stlx::parse::string<std::string::const_iterator,               \
                                     sizeof(_::_name), _::_name>;
 
 #define CHARACTER_(_name, _value)                                              \
   using _name = stlx::parse::character<std::string::const_iterator, _value>;
 
 #define CHARACTERS_(_name, _first, _last)                                      \
   using _name =                                                                \
       stlx::parse::characters<std::string::const_iterator, _first, _last>;
 
 #define REGEX(_name, _value)                                                   \
   namespace _ {                                                                \
   char _name[] = _value;                                                       \
   }                                                                            \
   using _name = stlx::parse::regex<std::string::const_iterator,                \
                                    sizeof(_::_name), _::_name>;
 
// End of stream/input rule - matches when iterator has reached the end
template <typename Iterator>
class eof : public rule<Iterator, eof<Iterator>> {
public:
  using iterator_type = Iterator;
  using _super_t = rule<Iterator, eof<Iterator>>;

  // Constructor: eof has no matched text
  eof()
    : rule<Iterator, eof<Iterator>>() {}

  static bool parse(context<iterator_type> &ctx, iterator_type &begin,
                    iterator_type &end,
                    std::shared_ptr<rule_base<Iterator>>& ast) {
    // For eof, we don't need to skip whitespace - we just check if we're at the end
    if (begin == end) {
      // Create instance - but eof has no matched text
      ast = std::make_shared<eof<Iterator>>();
      return true;
    }
    
    ctx.parse_errors.emplace_back(
        std::make_shared<parse_error<iterator_type>>(
            typeid(eof<Iterator>), begin, "Expected end of input",
            begin != end ? std::string(begin, begin + std::min<size_t>(10, end - begin)) +
                "..."
            : "EOF"));
    return false;
  }

  std::string_view get_text() const override {
    return "";
  }
};
 
 } // namespace parse
 
 // Main parser class with proper template handling
 template <typename _rule_t, bool _ignore_case = false,
           typename _whitespace_t = parse::whitespace<' ', '\t', '\n', '\r'>>
 class parser {
 public:
  template <typename _iterator_t>
  static bool parse(_iterator_t begin, _iterator_t end,
                    std::shared_ptr<_rule_t> &ast,
                    typename parse::parse_error<_iterator_t>::vector &errors,
                    bool require_full_consumption = false) {
    parse::context<_iterator_t> ctx(begin, end, true);
    
    std::shared_ptr<parse::rule_base<_iterator_t>> ast_base;
    bool success = _rule_t::parse(ctx, begin, end, ast_base);
    
    if (success) {
      // Cast the base AST to the concrete rule type
      ast = std::static_pointer_cast<_rule_t>(ast_base);
    } else {
      // Copy errors from context only on failure
      errors = ctx.parse_errors;
    }
    
    return success;
  }

  template <typename _iterator_t>
  static bool parse(_iterator_t begin, _iterator_t end,
                    std::shared_ptr<_rule_t> &ast,
                    bool require_full_consumption = false) {
    typename parse::parse_error<_iterator_t>::vector errors;
    return parse(begin, end, ast, errors, require_full_consumption);
  }
 };
 
 } // namespace stlx
 
 #endif // STLX_PARSER_HPP