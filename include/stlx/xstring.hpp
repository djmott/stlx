/** @file
  specializations of std::basic_string for advanced and common string handling
  @copyright David Mott (c) 2016. Distributed under the Boost Software License Version 1.0. See LICENSE.md or http://boost.org/LICENSE_1_0.txt for details.

*/

#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <functional>
#include <algorithm>
#include <cctype>
#include <cstring>
#include <locale>
#include <sstream>

// Define forever macro for infinite loops
#define forever for(;;)

namespace stlx{

    #if defined(_UNICODE) || defined(UNICODE)
      using tchar = wchar_t;
    #else
      using tchar = char;
    #endif

  template <typename _ChT> class xstring;

  using string = xstring<char>;
  using wstring = xstring<wchar_t>;
  using tstring = xstring<tchar>;

  /** Extends std::string with some added functionality
   @tparam _ChT character type
   */
  template <typename _ChT> class xstring : public std::basic_string<_ChT>{
  public:
    using _super_t = std::basic_string<_ChT>;
    using size_type = typename _super_t::size_type;

    ///Generic constructor forwards everything to the base class
    template <typename ... _ArgsT>
    xstring(_ArgsT&&...oArgs)
      : _super_t(std::forward<_ArgsT>(oArgs)...){}

    bool ends_with(const stlx::string& suffix) const{
      if (suffix.empty()) return true;
      if (_super_t::size() < suffix.size()) return false;
      
      auto sDest = _super_t::rbegin();
      auto sSrc = suffix.rbegin();
      for (; suffix.rend() != sSrc && sDest != _super_t::rend(); ++sSrc, ++sDest){
        if (*sSrc != *sDest) return false;
      }
      return (sSrc == suffix.rend());
    }

    /**
    Type safe formatting - simplified version
    @param oArgs variable elements appended together
     */
    static xstring format(){
      return xstring();
    }

    template <typename _Ty, typename ... _ArgsT>
    static xstring format(const _Ty & val, _ArgsT&&...oArgs){
      xstring sRet = to_string(val);
      if constexpr (sizeof...(oArgs) > 0) {
        sRet += format(std::forward<_ArgsT>(oArgs)...);
      }
      return sRet;
    }

    // Helper function to convert various types to string
    template<typename T>
    static xstring to_string(const T& val) {
      if constexpr (std::is_same_v<T, const char*>) {
        return xstring(val);
      } else if constexpr (std::is_same_v<T, char*>) {
        return xstring(val);
      } else if constexpr (std::is_same_v<T, std::string>) {
        return xstring(val);
      } else if constexpr (std::is_same_v<T, xstring>) {
        return val;
      } else if constexpr (std::is_integral_v<T>) {
        return xstring(std::to_string(val));
      } else if constexpr (std::is_floating_point_v<T>) {
        // Use a more controlled format for floating point numbers
        std::ostringstream oss;
        oss << val;
        return xstring(oss.str());
      } else {
        // Fallback for other types
        return xstring("(unknown)");
      }
    }

    // Specialization for string literals
    template<size_t N>
    static xstring to_string(const char (&val)[N]) {
      return xstring(val, N - 1); // Remove null terminator
    }

#if (stlx_OS_WINDOWS & stlx_OS)
    static xstring from_resource(uint32_t resid) {
      xstring sRet(10, 0);
      for(;;) {
        auto iRet = LoadString(GetModuleHandle(nullptr), resid, &sRet[0], sRet.size());
        if (!iRet) return xstring();
        if (sRet.size() < static_cast<size_t>(iRet)) {
          sRet.resize(iRet);
          return sRet;
        }
        sRet.resize(2 * sRet.size());
      }
    }
#endif

    xstring& reverse(){
      std::reverse(_super_t::begin(), _super_t::end());
      return *this;
    }
    
    /**
    Converts the string to lower case
    @param loc The locale to use during conversion
     */
    xstring& to_lower(const std::locale& loc){
      for (auto & ch : *this){
        ch = std::tolower(ch, loc);
      }
      return *this;
    }

    /**
    Converts the string to upper case
    @param loc The locale to use during conversion
     */
    xstring& to_upper(const std::locale& loc){
      for (auto & ch : *this){
        ch = std::toupper(ch, loc);
      }
      return *this;
    }

    ///Trim leading whitespace
    xstring& ltrim(){
      auto oBegin = _super_t::begin();
      if (oBegin >= _super_t::end()){
        return *this;
      }
      for(;oBegin != _super_t::end() ; ++oBegin){
        if (!std::isspace(*oBegin)){
          break;
        }
      }
      _super_t::erase(_super_t::begin(), oBegin);
      return *this;
    }

    ///Trim trailing whitespace
    xstring& rtrim(){
      if (_super_t::empty()) {
        return *this;
      }
      
      auto it = _super_t::end();
      --it;
      
      // Find the last non-whitespace character
      while (it != _super_t::begin() && std::isspace(*it)) {
        --it;
      }
      
      // If we found a non-whitespace character, erase everything after it
      if (!std::isspace(*it)) {
        _super_t::erase(it + 1, _super_t::end());
      } else {
        // All characters are whitespace
        _super_t::clear();
      }
      
      return *this;
    }

    //Trim leading and trailing whitespace
    xstring& trim(){
      ltrim();
      return rtrim();
    }

    //replaces all occurrences of the characters in the oItems list with a specified character
    xstring& replace(std::initializer_list<_ChT> oItems, _ChT chReplace) {
      for (auto & oCh : *this) {
        bool bFound = false;
        for (const auto & oFind : oItems) {
          if (oFind == oCh) {
            bFound = true;
            break;
          }
        }
        if (bFound) {
          oCh = chReplace;
        }
      }
      return *this;
    }

    ///replaces all instances of a sub-string with a character
    xstring& replace(const xstring& src, _ChT chReplace) {
      size_type pos = 0;
      while ((pos = _super_t::find(src, pos)) != _super_t::npos) {
        _super_t::erase(pos, src.size());
        _super_t::insert(pos, 1, chReplace);
        pos += 1; // Move past the replacement character
      }
      return *this;
    }

    ///removes all occurrences of a list
    xstring& remove(const std::initializer_list<_ChT>& chars) {
      auto it = std::remove_if(_super_t::begin(), _super_t::end(), 
        [&chars](_ChT ch) {
          return std::find(chars.begin(), chars.end(), ch) != chars.end();
        });
      _super_t::erase(it, _super_t::end());
      return *this;
    }

    ///finds the first occurrence of any item
    size_type find_first_of(const std::initializer_list<_ChT>& delimiters, size_type pos = 0) const{
      size_type iRet = _super_t::npos;
      for (const _ChT ch : delimiters){
        auto x = _super_t::find_first_of(ch, pos);
        if ((_super_t::npos != x) && (_super_t::npos == iRet || x < iRet)){
          iRet = x;
        }
      }
      return iRet;
    }
    
    ///finds the first occurrence of an item from a user-defined visitor
    size_type find_first_of(const std::function<bool(_ChT)>& VisitorFN, size_type pos = 0) const{
      for (size_type i = pos; i < _super_t::size(); ++i){
        if (VisitorFN(_super_t::at(i))) return i;
      }
      return _super_t::npos;
    }

    size_type find_last_of(const std::initializer_list<_ChT>& delimiters, size_type pos = _super_t::npos) const {
      size_type iRet = _super_t::npos;
      for (const _ChT ch : delimiters){
        auto x = _super_t::find_last_of(ch, pos);
        if ((_super_t::npos != x) && (_super_t::npos == iRet || x > iRet)){
          iRet = x;
        }
      }
      return iRet;
    }

    ///splits the string by the specified delmiters into constituent elements
    std::vector<xstring<_ChT>> split(const std::initializer_list<_ChT>& delimiters, bool trimEmpty = false) const {
      using container_t = std::vector<xstring<_ChT>>;
      container_t oRet;
      using _my_t = xstring<_ChT>;
      size_type pos;
      size_type lastPos = 0;

      using value_type = typename container_t::value_type;

      forever{
        pos = find_first_of(delimiters, lastPos);
        if (pos == _my_t::npos) {
        pos = _super_t::length();
        if (pos != lastPos || !trimEmpty) {
          oRet.push_back(value_type(_super_t::data() + lastPos, (pos - lastPos)));
        }
        break;
      }
      else if (pos != lastPos || !trimEmpty) {
        oRet.push_back(value_type(_super_t::data() + lastPos , (pos - lastPos)));
      }

      lastPos = pos + 1;
      }
      return oRet;
    }

    ///splits the string by the specified string into constituent elements
    std::vector<xstring<_ChT>> split(const xstring<_ChT>& delim, bool trimEmpty = false) const {
      using container_t = std::vector<xstring<_ChT>>;
      container_t oRet;
      using _my_t = xstring<_ChT>;
      size_type pos;
      size_type lastPos = 0;

      using value_type = typename container_t::value_type;

      forever{
        pos = _super_t::find(delim, lastPos);
        if (pos == _my_t::npos) {
          pos = _super_t::length();
          if (pos != lastPos || !trimEmpty) {
            oRet.push_back(value_type(_super_t::data() + lastPos, (pos - lastPos)));
          }
          break;
        }
        else if (pos != lastPos || !trimEmpty) {
          oRet.push_back(value_type(_super_t::data() + lastPos , (pos - lastPos)));
        }

        lastPos = pos + delim.size();
      }
      return oRet;
    }

    ///splits the string by the user supplied unary function
    std::vector<xstring<_ChT>> split(const std::function<bool(_ChT)>& VisitorFN) const{
      using container_t = std::vector<xstring<_ChT>>;
      using value_type = typename container_t::value_type;
      container_t oRet;
      using _my_t = xstring<_ChT>;
      size_t iLast = 0;
      
      for (size_t iCurr = 0; iCurr < _my_t::size(); ++iCurr){
        if (VisitorFN((*this)[iCurr])){
          if (iCurr > iLast) {
            oRet.push_back(value_type(_super_t::data() + iLast, iCurr - iLast));
          }
          iLast = iCurr + 1;
        }
      }
      
      // Add the last part if there's anything left
      if (iLast < _my_t::size()){
        oRet.push_back(value_type(_super_t::data() + iLast, _my_t::size() - iLast));
      }
      
      return oRet;
    }
  };

}

namespace std{
  template <typename _ChT> struct iterator_traits<stlx::xstring<_ChT>> : std::iterator_traits<std::basic_string<_ChT>>{};
}