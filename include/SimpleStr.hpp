#pragma onc #pragma once

#include <algorithm>
#include <cassert>
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
      result.reserve((start - end - step - 1) / (-step));
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
};

} // namespace SimpleStr
