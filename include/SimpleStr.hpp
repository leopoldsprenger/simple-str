#pragma once

#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace SimpleStr {

struct Str {
  std::string s;

  // Constructors
  Str() = default;
  Str(std::string_view sv) : s(sv) {}

  // Conversion operators
  operator std::string_view() const { return s; }
  const std::string &str() const { return s; }

  // ---------------- Basic string checks ----------------

  inline bool startswith(std::string_view prefix) const {
    return s.size() >= prefix.size() && s.substr(0, prefix.size()) == prefix;
  }

  inline bool endswith(std::string_view suffix) const {
    return s.size() >= suffix.size() &&
           s.substr(s.size() - suffix.size()) == suffix;
  }

  // ---------------- Split & Join ----------------

  inline std::vector<std::string_view> split(char delim) const {
    std::vector<std::string_view> result;
    size_t start = 0;
    for (size_t i = 0; i < s.size(); ++i) {
      if (s[i] != delim)
        continue;
      result.push_back(s.substr(start, i - start));
      start = i + 1;
    }
    if (start <= s.size())
      result.push_back(s.substr(start));
    return result;
  }

  // Python-style default split (whitespace)
  inline std::vector<std::string_view> split() const {
    std::vector<std::string_view> result;
    size_t start = 0;
    auto is_space = [](char c) {
      return c == ' ' || c == '\t' || c == '\n' || c == '\r';
    };

    while (start < s.size() && is_space(s[start]))
      ++start;
    for (size_t i = start; i < s.size(); ++i) {
      if (!is_space(s[i]))
        continue;
      if (i > start)
        result.push_back(s.substr(start, i - start));
      start = i + 1;
      while (start < s.size() && is_space(s[start]))
        ++start;
      i = start - 1;
    }
    if (start < s.size())
      result.push_back(s.substr(start));
    return result;
  }

  inline static std::string join(const std::vector<std::string_view> &parts,
                                 std::string_view sep = "") {
    std::string joined;
    size_t total_size = 0;
    for (auto &p : parts)
      total_size += p.size();
    total_size += sep.size() * (parts.size() ? parts.size() - 1 : 0);
    joined.reserve(total_size);
    for (size_t i = 0; i < parts.size(); ++i) {
      joined += parts[i];
      if (i < parts.size() - 1)
        joined += sep;
    }
    return joined;
  }

  // Member join for chaining
  inline Str join(const std::vector<Str> &parts,
                  std::string_view sep = "") const {
    std::vector<std::string_view> views;
    views.reserve(parts.size());
    for (auto &p : parts)
      views.push_back(p.s);
    return Str(join(views, sep));
  }

  // ---------------- Slice ----------------

  inline Str slice(int start, int end, int step = 1) const {
    assert(step != 0);
    const int n = static_cast<int>(s.size());

    // normalize negative indices
    if (start < 0)
      start += n;
    if (end < 0)
      end += n;

    // clamp to bounds [0, n]
    start = std::clamp(start, 0, n);
    end = std::clamp(end, 0, n);

    std::string result;
    if (step > 0) {
      if (start >= end)
        return Str("");
      result.reserve((end - start + step - 1) / step);
      for (int i = start; i < end; i += step)
        result += s[i];
    } else {
      if (start <= end)
        return Str("");
      result.reserve((start - end + (-step - 1)) / -step);
      for (int i = start; i > end; i += step)
        result += s[i];
    }
    return Str(result);
  }

  // ---------------- Replace ----------------

  inline Str replace(std::string_view from, std::string_view to) const {
    if (from.empty())
      return Str(s);

    std::string result;
    size_t pos = 0;

    // reserve estimate
    size_t occurrences = 0;
    size_t tmp_pos = 0;
    while (true) {
      size_t match = s.find(from, tmp_pos);
      if (match == s.npos)
        break;
      ++occurrences;
      tmp_pos = match + from.size();
    }
    result.reserve(s.size() + occurrences * to.size());

    while (true) {
      size_t match = s.find(from, pos);
      if (match == s.npos)
        break;
      result += s.substr(pos, match - pos);
      result += to;
      pos = match + from.size();
    }

    result += s.substr(pos);
    return Str(result);
  }

  // ---------------- Strip Functions ----------------

  inline Str lstrip() const {
    size_t start = 0;
    while (start < s.size() && (s[start] == ' ' || s[start] == '\t' ||
                                s[start] == '\n' || s[start] == '\r')) {
      ++start;
    }
    return slice(static_cast<int>(start), static_cast<int>(s.size()));
  }

  inline Str rstrip() const {
    size_t end = s.size();
    while (end > 0 && (s[end - 1] == ' ' || s[end - 1] == '\t' ||
                       s[end - 1] == '\n' || s[end - 1] == '\r')) {
      --end;
    }
    return slice(0, static_cast<int>(end));
  }

  inline Str strip() const { return lstrip().rstrip(); }

  // ---------------- Lower, Upper and Capitalize ----------------

  inline Str lower() const {
    std::string result;
    result.reserve(s.size());
    for (char c : s) {
      result += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return Str(result);
  }

  inline Str upper() const {
    std::string result;
    result.reserve(s.size());
    for (char c : s) {
      result += static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    }
    return Str(result);
  }

  inline Str capitalize() const {
    if (s.empty())
      return Str("");
    std::string result;
    result.reserve(s.size());
    result += static_cast<char>(std::toupper(static_cast<unsigned char>(s[0])));
    for (size_t i = 1; i < s.size(); ++i) {
      result +=
          static_cast<char>(std::tolower(static_cast<unsigned char>(s[i])));
    }
    return Str(result);
  }

  // ---------------- Remove ----------------

  inline Str remove(std::string_view sub) const { return replace(sub, ""); }

  // ---------------- Count ----------------

  inline int count(std::string_view sub) const {
    if (sub.empty())
      return 0;
    int occurrences = 0;
    size_t pos = 0;
    while (true) {
      size_t match = s.find(sub, pos);
      if (match == s.npos)
        break;
      ++occurrences;
      pos = match + sub.size();
    }
    return occurrences;
  }

  // ---------------- Contains ----------------

  inline bool contains(std::string_view sub) const {
    return s.find(sub) != std::string::npos;
  }

  // ---------------- Repeat ----------------

  inline Str repeat(int n) const {
    if (n <= 0 || s.empty())
      return Str("");
    std::string result;
    result.reserve(s.size() * n);
    for (int i = 0; i < n; ++i)
      result += s;
    return Str(result);
  }

  // Operator * for repeat
  inline Str operator*(int n) const { return repeat(n); }

  // ---------------- Character Set Checks ----------------

  inline bool isAlpha() const {
    if (s.empty())
      return false;
    for (char c : s)
      if (!std::isalpha(static_cast<unsigned char>(c)))
        return false;
    return true;
  }

  inline bool isDigit() const {
    if (s.empty())
      return false;
    for (char c : s)
      if (!std::isdigit(static_cast<unsigned char>(c)))
        return false;
    return true;
  }

  inline bool isAlnum() const {
    if (s.empty())
      return false;
    for (char c : s)
      if (!std::isalnum(static_cast<unsigned char>(c)))
        return false;
    return true;
  }
};

} // namespace SimpleStr
