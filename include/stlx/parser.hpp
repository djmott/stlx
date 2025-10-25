/** @file
text parsing and AST generation
@copyright David Mott (c) 2025.
Distributed under the Boost Software License Version 1.0.
See LICENSE.md or http://boost.org/LICENSE_1_0.txt for details.
 */

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <regex>
#include <typeinfo>
#include <string_view>
#include <cctype>
#include <algorithm>

namespace stlx {

namespace parse {

// Forward declarations
template<typename Iterator>
class rule_base;

// Skip whitespace helper function
template<typename Iterator>
void skip_ws(Iterator& begin, Iterator& end, bool ignore_whitespace) {
  if (!ignore_whitespace) return;
  while (begin != end && (*begin == ' ' || *begin == '\t' || *begin == '\n' || *begin == '\r')) {
    ++begin;
  }
}

// Parse error structure
template <typename iterator_t>
struct parse_error {
  using iterator_type = iterator_t;
  using ptr = std::shared_ptr<parse_error>;
  using vector = std::vector<ptr>;
  const std::type_info& failed_rule;
  const iterator_type position;
  std::string expected;
  std::string found;

  parse_error(const std::type_info& failRule, const iterator_type pos,
    std::string expected_msg = "", std::string found_msg = "")
    : failed_rule(failRule), position(pos),
    expected(std::move(expected_msg)), found(std::move(found_msg)) {
  }
};

// Context structure
template <typename iterator_t>
struct context {
  using iterator_type = iterator_t;
  iterator_type begin;
  iterator_type end;
  bool ignore_whitespace;
  std::shared_ptr<rule_base<iterator_t>> start_rule;
  typename parse_error<iterator_t>::vector parse_errors;

  context(iterator_type b, iterator_type e, bool ignore_ws = true)
    : begin(b), end(e), ignore_whitespace(ignore_ws) {
  }
};

// Rule base class with proper iterator type handling
template<typename Iterator>
class rule_base : public std::vector<std::shared_ptr<rule_base<Iterator>>>,
                 public std::enable_shared_from_this<rule_base<Iterator>> {
public:
  using iterator_type = Iterator;
  using pointer_type = std::shared_ptr<rule_base<Iterator>>;
  using weak_ptr_t = std::weak_ptr<rule_base<Iterator>>;
  using super_t = std::vector<pointer_type>;

  template <typename ... _child_ts>
  rule_base(_child_ts&& ... oChildRules)
    : super_t{ std::forward<_child_ts>(oChildRules)... },
    _parent() {
  }

  virtual ~rule_base() = default;

  virtual bool isa(const std::type_info& oType) const = 0;
  virtual const std::type_info& type() const = 0;

  pointer_type parent() { return _parent.lock(); }

  virtual bool parse(context<iterator_type>& ctx, iterator_type& begin, iterator_type& end) = 0;

  virtual std::string_view get_text() const { return ""; }

protected:
  void set_parent(weak_ptr_t oParent) {
    _parent = oParent;
    auto oThis = shared_from_this();
    for (auto& oChild : static_cast<super_t&>(*this)) {
      oChild->set_parent(oThis);
    }
  }
private:
  std::weak_ptr<rule_base<Iterator>> _parent;
};

// Helper to clone rules for AST building
template <typename RuleT>
std::shared_ptr<RuleT> clone_rule(const std::shared_ptr<RuleT>& src) {
  auto clone = std::make_shared<RuleT>(*src);
  return clone;
}

// Rule template using CRTP with proper iterator type handling
template <typename Iterator, typename _decl_t, typename _impl_t = _decl_t>
class rule : public rule_base<Iterator> {
public:
  using iterator_type = Iterator;
  using decl_type = _decl_t;
  using impl_type = _impl_t;
  using rule_type = rule<Iterator, _decl_t, _impl_t>;

  template <typename ... _child_rule_ts>
  explicit rule(_child_rule_ts&& ... oChildRules)
    : rule_base<Iterator>(std::forward<_child_rule_ts>(oChildRules)...) {
  }

  virtual ~rule() = default;

  bool isa(const std::type_info& oType) const override {
    return (typeid(rule) == oType) || (typeid(decl_type) == oType) ||
      (typeid(impl_type) == oType) || (typeid(rule_base<Iterator>) == oType);
  }

  const std::type_info& type() const override {
    return typeid(_decl_t);
  }

  bool parse(context<iterator_type>& ctx, iterator_type& begin, iterator_type& end) override {
    auto backup = begin;
    if (ctx.ignore_whitespace) skip_ws(begin, end, ctx.ignore_whitespace);
    for (auto& child : *this) {
      if (!child->parse(ctx, begin, end)) {
        ctx.parse_errors.emplace_back(std::make_shared<parse_error<iterator_type>>(
          this->type(), backup, "Sequence element failed",
          begin != end ? std::string(1, *begin) : "EOF"));
        begin = backup;
        return false;
      }
    }
    return true;
  }
};

// NOT combinator with proper template handling
template <typename Iterator, typename ...> class not_;

template <typename Iterator> 
class not_<Iterator> : public rule<Iterator, not_<Iterator>> {
public:
  using _super_t = rule<Iterator, not_<Iterator>>;
  template <typename ... _child_rule_ts>
  explicit not_(_child_rule_ts&& ... oChildRules) : _super_t(std::forward<_child_rule_ts>(oChildRules)...) {}

  bool parse(context<iterator_type>& ctx, iterator_type& begin, iterator_type& end) override {
    auto backup = begin;
    if (ctx.ignore_whitespace) skip_ws(begin, end, ctx.ignore_whitespace);
    if (this->empty() || !this->front()->parse(ctx, begin, end)) {
      // NOT succeeds when child fails
      begin = backup;
      return true;
    }
    // NOT fails when child succeeds
    ctx.parse_errors.emplace_back(std::make_shared<parse_error<iterator_type>>(
      this->type(), backup, "Negative assertion failed",
      begin != end ? std::string(1, *begin) : "EOF"));
    begin = backup;
    return false;
  }
};

template <typename Iterator, typename _head_t, typename ... _tail_ts>
class not_<Iterator, _head_t, _tail_ts...> : public rule<Iterator, not_<Iterator, _head_t, _tail_ts...>> {
public:
  using _super_t = rule<Iterator, not_<Iterator, _head_t, _tail_ts...>>;
  template <typename ... _child_rule_ts>
  explicit not_(_child_rule_ts&& ... oChildRules) : _super_t(std::forward<_child_rule_ts>(oChildRules)...) {}

  bool parse(context<iterator_type>& ctx, iterator_type& begin, iterator_type& end) override {
    auto backup = begin;
    if (ctx.ignore_whitespace) skip_ws(begin, end, ctx.ignore_whitespace);

    auto temp_begin = begin;
    bool all_matched = true;
    for (auto& child : *this) {
      if (!child->parse(ctx, temp_begin, end)) {
        all_matched = false;
        break;
      }
    }

    if (all_matched) {
      ctx.parse_errors.emplace_back(std::make_shared<parse_error<iterator_type>>(
        this->type(), backup, "Negative assertion failed",
        begin != end ? std::string(1, *begin) : "EOF"));
      begin = backup;
      return false;
    }

    begin = backup;
    return true;
  }
};

// AND combinator (uses default sequence implementation)
template <typename Iterator, typename ...> class and_;
template <typename Iterator> class and_<Iterator> : public rule<Iterator, and_<Iterator>> {};
template <typename Iterator, typename _head_t, typename ... _tail_ts>
class and_<Iterator, _head_t, _tail_ts...> : public rule<Iterator, and_<Iterator, _head_t, _tail_ts...>> {};

// OR combinator with proper template handling
template <typename Iterator, typename ...> class or_;

template <typename Iterator> 
class or_<Iterator> : public rule<Iterator, or_<Iterator>> {
public:
  using _super_t = rule<Iterator, or_<Iterator>>;
  template <typename ... _child_rule_ts>
  explicit or_(_child_rule_ts&& ... oChildRules) : _super_t(std::forward<_child_rule_ts>(oChildRules)...) {}

  bool parse(context<iterator_type>& ctx, iterator_type& begin, iterator_type& end) override {
    auto backup = begin;
    if (ctx.ignore_whitespace) skip_ws(begin, end, ctx.ignore_whitespace);
    typename parse_error<iterator_type>::vector branch_errors;
    for (auto& child : *this) {
      auto child_backup = begin;
      if (child->parse(ctx, begin, end)) return true;
      branch_errors.emplace_back(std::make_shared<parse_error<iterator_type>>(
        child->type(), child_backup, "Alternative branch failed",
        child_backup != end ? std::string(1, *child_backup) : "EOF"));
      begin = backup;
    }
    ctx.parse_errors.insert(ctx.parse_errors.end(), branch_errors.begin(), branch_errors.end());
    begin = backup;
    return false;
  }
};

template <typename Iterator, typename _head_t, typename ... _tail_ts>
class or_<Iterator, _head_t, _tail_ts...> : public rule<Iterator, or_<Iterator, _head_t, _tail_ts...>> {};

// ONE_OR_MORE combinator with proper template handling
template <typename Iterator, typename _ty>
class one_or_more_ : public rule<Iterator, one_or_more_<Iterator, _ty>> {
public:
  using _super_t = rule<Iterator, one_or_more_<Iterator, _ty>>;
  template <typename ... _child_rule_ts>
  explicit one_or_more_(_child_rule_ts&& ... oChildRules) : _super_t(std::forward<_child_rule_ts>(oChildRules)...) {}

  bool parse(context<iterator_type>& ctx, iterator_type& begin, iterator_type& end) override {
    auto backup = begin;
    if (ctx.ignore_whitespace) skip_ws(begin, end, ctx.ignore_whitespace);
    if (this->empty()) {
      ctx.parse_errors.emplace_back(std::make_shared<parse_error<iterator_type>>(
        this->type(), backup, "One_or_more rule has no child", ""));
      begin = backup;
      return false;
    }
    auto child = this->front();

    if (!child->parse(ctx, begin, end)) {
      ctx.parse_errors.emplace_back(std::make_shared<parse_error<iterator_type>>(
        this->type(), backup, "One_or_more requires at least one match",
        begin != end ? std::string(1, *begin) : "EOF"));
      begin = backup;
      return false;
    }
    this->push_back(clone_rule(child));

    while (true) {
      auto loop_backup = begin;
      if (ctx.ignore_whitespace) skip_ws(begin, end, ctx.ignore_whitespace);
      if (begin == end || !child->parse(ctx, begin, end)) {
        begin = loop_backup;
        break;
      }
      // Prevent infinite loop: if no progress was made, break
      if (begin == loop_backup) break;
      this->push_back(clone_rule(child));
    }
    return true;
  }
};

// ZERO_OR_MORE combinator with proper template handling
template <typename Iterator, typename _ty>
class zero_or_more_ : public rule<Iterator, zero_or_more_<Iterator, _ty>> {
public:
  using _super_t = rule<Iterator, zero_or_more_<Iterator, _ty>>;
  template <typename ... _child_rule_ts>
  explicit zero_or_more_(_child_rule_ts&& ... oChildRules) : _super_t(std::forward<_child_rule_ts>(oChildRules)...) {}

  bool parse(context<iterator_type>& ctx, iterator_type& begin, iterator_type& end) override {
    if (this->empty()) return true;
    auto child = this->front();
    while (true) {
      auto backup = begin;
      if (ctx.ignore_whitespace) skip_ws(begin, end, ctx.ignore_whitespace);
      if (begin == end || !child->parse(ctx, begin, end)) {
        begin = backup;
        break;
      }
      // Prevent infinite loop: if no progress was made, break
      if (begin == backup) break;
      this->push_back(clone_rule(child));
    }
    return true;
  }
};

// ZERO_OR_ONE combinator with proper template handling
template <typename Iterator, typename _ty>
class zero_or_one_ : public rule<Iterator, zero_or_one_<Iterator, _ty>> {
public:
  using _super_t = rule<Iterator, zero_or_one_<Iterator, _ty>>;
  template <typename ... _child_rule_ts>
  explicit zero_or_one_(_child_rule_ts&& ... oChildRules) : _super_t(std::forward<_child_rule_ts>(oChildRules)...) {}

  bool parse(context<iterator_type>& ctx, iterator_type& begin, iterator_type& end) override {
    if (this->empty()) return true;
    auto backup = begin;
    if (ctx.ignore_whitespace) skip_ws(begin, end, ctx.ignore_whitespace);
    auto child = this->front();
    if (child->parse(ctx, begin, end)) {
      this->push_back(clone_rule(child));
      return true;
    }
    begin = backup;
    return true;
  }
};

// STRING terminal with proper template handling
template <typename Iterator, size_t _len, char(&_str)[_len]>
class string : public rule<Iterator, string<Iterator, _len, _str>> {
private:
  std::string _matched;

public:
  using _super_t = rule<Iterator, string<Iterator, _len, _str>>;
  static constexpr size_t length = _len - 1;
  string() = default;

  bool parse(context<iterator_type>& ctx, iterator_type& begin, iterator_type& end) override {
    auto backup = begin;
    if (ctx.ignore_whitespace) skip_ws(begin, end, ctx.ignore_whitespace);
    constexpr const char* lit = _str;
    for (size_t i = 0; i < length; ++i) {
      if (begin == end || *begin != lit[i]) {
        ctx.parse_errors.emplace_back(std::make_shared<parse_error<iterator_type>>(
          this->type(), backup, std::string("Expected string: ") + lit,
          begin != end ? std::string(1, *begin) : "EOF"));
        begin = backup;
        return false;
      }
      ++begin;
    }
    _matched = std::string(lit, length);
    return true;
  }

  std::string_view get_text() const override { return _matched; }
};

// CHARACTER terminal with proper template handling
template <typename Iterator, char _value>
class character : public rule<Iterator, character<Iterator, _value>> {
private:
  char _matched = 0;

public:
  bool parse(context<iterator_type>& ctx, iterator_type& begin, iterator_type& end) override {
    auto backup = begin;
    if (ctx.ignore_whitespace) skip_ws(begin, end, ctx.ignore_whitespace);
    if (begin == end || *begin != _value) {
      ctx.parse_errors.emplace_back(std::make_shared<parse_error<iterator_type>>(
        this->type(), backup, std::string("Expected character: '") + _value + "'",
        begin != end ? std::string(1, *begin) : "EOF"));
      begin = backup;
      return false;
    }
    _matched = *begin;
    ++begin;
    return true;
  }

  std::string_view get_text() const override { return std::string_view(&_matched, 1); }
};

// CHARACTERS range terminal with proper template handling
template <typename Iterator, char _first, char _last>
class characters : public rule<Iterator, characters<Iterator, _first, _last>> {
private:
  char _matched = 0;

public:
  using _super_t = rule<Iterator, characters<Iterator, _first, _last>>;

  bool parse(context<iterator_type>& ctx, iterator_type& begin, iterator_type& end) override {
    auto backup = begin;
    if (ctx.ignore_whitespace) skip_ws(begin, end, ctx.ignore_whitespace);
    if (begin == end) {
      ctx.parse_errors.emplace_back(std::make_shared<parse_error<iterator_type>>(
        this->type(), backup, "Expected character in range",
        "EOF"));
      begin = backup;
      return false;
    }

    char c = *begin;
    if constexpr (_first <= _last) {
      if (c < _first || c > _last) {
        ctx.parse_errors.emplace_back(std::make_shared<parse_error<iterator_type>>(
          this->type(), backup,
          std::string("Expected character in range [") + _first + "-" + _last + "]",
          std::string(1, c)));
        begin = backup;
        return false;
      }
    }
    else {
      if (c > _first || c < _last) {
        ctx.parse_errors.emplace_back(std::make_shared<parse_error<iterator_type>>(
          this->type(), backup,
          std::string("Expected character in range [") + _last + "-" + _first + "]",
          std::string(1, c)));
        begin = backup;
        return false;
      }
    }

    _matched = c;
    ++begin;
    return true;
  }

  std::string_view get_text() const override { return std::string_view(&_matched, 1); }
};

// REGEX terminal with proper template handling
template <typename Iterator, size_t _len, char(&_str)[_len]>
class regex : public rule<Iterator, regex<Iterator, _len, _str>> {
private:
  std::string _matched;
  static std::regex& get_pattern() {
    static std::regex pattern(_str);
    return pattern;
  }

public:
  using _super_t = rule<Iterator, regex<Iterator, _len, _str>>;
  static constexpr size_t length = _len - 1;

  bool parse(context<iterator_type>& ctx, iterator_type& begin, iterator_type& end) override {
    auto backup = begin;
    if (ctx.ignore_whitespace) skip_ws(begin, end, ctx.ignore_whitespace);

    if (begin == end) {
      ctx.parse_errors.emplace_back(std::make_shared<parse_error<iterator_type>>(
        this->type(), backup, "Expected regex match", "EOF"));
      begin = backup;
      return false;
    }

    std::match_results<iterator_type> matches;
    auto& pattern = get_pattern();

    if (!std::regex_search(begin, end, matches, pattern,
      std::regex_constants::match_continuous)) {
      ctx.parse_errors.emplace_back(std::make_shared<parse_error<iterator_type>>(
        this->type(), backup, std::string("Expected regex: ") + _str,
        begin != end ? std::string(begin, begin + std::min<size_t>(10, end - begin)) + "..." : "EOF"));
      begin = backup;
      return false;
    }

    if (matches.empty() || matches.position(0) != 0) {
      ctx.parse_errors.emplace_back(std::make_shared<parse_error<iterator_type>>(
        this->type(), backup, "Regex match not at beginning",
        std::string(begin, begin + std::min<size_t>(10, end - begin)) + "..."));
      begin = backup;
      return false;
    }

    _matched = matches.str(0);
    begin += _matched.length();
    return true;
  }

  std::string_view get_text() const override { return _matched; }
};

// WHITESPACE definition
template <char..._chs>
class whitespace {
public:
  using whitespace_type = whitespace<_chs...>;
};

// Macros for convenience - fixed to use proper template parameters
#define STRING_(x) \
        namespace _ { char x[] = #x; } \
        using x = stlx::parse::string<std::string::const_iterator, sizeof(_::x), _::x>;

#define STRING(_name, _value) \
        namespace _ { char _name[] = _value; } \
        using _name = stlx::parse::string<std::string::const_iterator, sizeof(_::_name), _::_name>;

#define CHARACTER_(_name, _value) \
        using _name = stlx::parse::character<std::string::const_iterator, _value>;

#define CHARACTERS_(_name, _first, _last) \
        using _name = stlx::parse::characters<std::string::const_iterator, _first, _last>;

#define REGEX(_name, _value) \
        namespace _ { char _name[] = _value; } \
        using _name = stlx::parse::regex<std::string::const_iterator, sizeof(_::_name), _::_name>;

// End of file marker
struct EndOfFile {
  using impl_type = EndOfFile;
};

} // namespace parse

// Main parser class with proper template handling
template <typename _rule_t, bool _ignore_case = false, typename _whitespace_t = parse::whitespace<' ', '\t', '\n', '\r'>>
class parser {
public:
  template <typename _iterator_t>
  static bool parse(_iterator_t begin, _iterator_t end,
    std::shared_ptr<_rule_t>& ast,
    typename parse::parse_error<_iterator_t>::vector& errors) {
    parse::context<_iterator_t> ctx(begin, end, true);
    auto start_rule = std::make_shared<_rule_t>();

    if (!start_rule->parse(ctx, begin, end)) {
      errors = ctx.parse_errors;
      return false;
    }

    if (begin != end) {
      errors.emplace_back(std::make_shared<parse::parse_error<_iterator_t>>(
        typeid(_rule_t), begin, "Expected end of input",
        std::string(begin, begin + std::min<size_t>(10, end - begin)) + "..."));
      return false;
    }

    ast = start_rule;
    return true;
  }

  template <typename _iterator_t>
  static bool parse(_iterator_t begin, _iterator_t end, std::shared_ptr<_rule_t>& ast) {
    typename parse::parse_error<_iterator_t>::vector errors;
    return parse(begin, end, ast, errors);
  }
};

} // namespace stlx