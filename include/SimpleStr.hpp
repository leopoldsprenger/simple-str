#pragma once

#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstddef>
#include <string>
#include <string_view>
#include <vector>
#include <cstring>

namespace SimpleStr {

struct Str {
    std::string s;

    // ---------------- Constructors ----------------
    Str() = default;
    Str(std::string_view sv) : s(sv) {}

    // ---------------- Conversion operators ----------------
    operator std::string_view() const { return s; }
    const std::string& str() const { return s; }

    // ---------------- Basic string checks ----------------
    inline bool startswith(std::string_view prefix) const {
        return s.size() >= prefix.size() &&
               std::memcmp(s.data(), prefix.data(), prefix.size()) == 0;
    }

    inline bool endswith(std::string_view suffix) const {
        return s.size() >= suffix.size() &&
               std::memcmp(s.data() + s.size() - suffix.size(), suffix.data(), suffix.size()) == 0;
    }

    // ---------------- Split & Join ----------------
    inline std::vector<std::string_view> split(char delim) const {
        std::vector<std::string_view> result;
        size_t start = 0;

        for (size_t i = 0; i < s.size(); ++i) {
            if (s[i] == delim) {
                result.emplace_back(s.data() + start, i - start);
                start = i + 1;
            }
        }

        result.emplace_back(s.data() + start, s.size() - start);
        return result;
    }

    inline std::vector<std::string_view> split() const {
        std::vector<std::string_view> result;
        size_t i = 0, n = s.size();

        auto is_space = [](char c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r'; };

        while (i < n) {
            while (i < n && is_space(s[i])) ++i;
            if (i >= n) break;
            size_t start = i;
            while (i < n && !is_space(s[i])) ++i;
            result.emplace_back(s.data() + start, i - start);
        }
        return result;
    }

    inline static std::string join(const std::vector<std::string_view>& parts, std::string_view sep = "") {
        size_t total_size = 0;
        for (auto& p : parts) total_size += p.size();
        total_size += sep.size() * (parts.size() ? parts.size() - 1 : 0);

        std::string result;
        result.reserve(total_size);

        for (size_t i = 0; i < parts.size(); ++i) {
            result.append(parts[i]);
            if (i < parts.size() - 1) result.append(sep);
        }
        return result;
    }

    inline Str join(const std::vector<Str>& parts, std::string_view sep = "") const {
        std::vector<std::string_view> views;
        views.reserve(parts.size());
        for (auto& p : parts) views.push_back(p.s);
        return Str(join(views, sep));
    }

    // ---------------- Slice ----------------
    inline Str slice(int start, int end, int step = 1) const {
        assert(step != 0);
        const int n = static_cast<int>(s.size());

        if (start < 0) start += n;
        if (end < 0) end += n;
        start = std::clamp(start, 0, n);
        end = std::clamp(end, 0, n);

        if ((step > 0 && start >= end) || (step < 0 && start <= end))
            return Str("");

        int size = (step > 0) ? (end - start + step - 1) / step : (start - end - step - 1) / (-step);
        std::string result;
        result.resize(size);

        size_t idx = 0;
        if (step > 0) {
            for (int i = start; i < end; i += step) result[idx++] = s[i];
        } else {
            for (int i = start; i > end; i += step) result[idx++] = s[i];
        }
        return Str(result);
    }

    // ---------------- Replace ----------------
    inline Str replace(std::string_view from, std::string_view to) const {
        if (from.empty()) return Str(s);

        std::string result;
        result.reserve(s.size()); // rough estimate

        size_t pos = 0;
        while (true) {
            size_t match = s.find(from, pos);
            if (match == s.npos) break;

            result.append(s.data() + pos, match - pos);
            result.append(to);
            pos = match + from.size();
        }
        result.append(s.data() + pos, s.size() - pos);
        return Str(result);
    }

    // ---------------- Strip ----------------
    inline Str lstrip() const {
        size_t start = 0, n = s.size();
        while (start < n && std::isspace(static_cast<unsigned char>(s[start]))) ++start;
        return Str(std::string_view(s.data() + start, n - start));
    }

    inline Str rstrip() const {
        size_t end = s.size();
        while (end > 0 && std::isspace(static_cast<unsigned char>(s[end - 1]))) --end;
        return Str(std::string_view(s.data(), end));
    }

    inline Str strip() const {
        size_t start = 0, end = s.size();
        while (start < end && std::isspace(static_cast<unsigned char>(s[start]))) ++start;
        while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) --end;
        return Str(std::string_view(s.data() + start, end - start));
    }

    // ---------------- Lower, Upper, Capitalize ----------------
    inline Str lower() const {
        std::string result(s.size(), '\0');
        std::transform(s.begin(), s.end(), result.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        return Str(result);
    }

    inline Str upper() const {
        std::string result(s.size(), '\0');
        std::transform(s.begin(), s.end(), result.begin(),
                       [](unsigned char c) { return std::toupper(c); });
        return Str(result);
    }

    inline Str capitalize() const {
        if (s.empty()) return Str("");
        std::string result(s.size(), '\0');
        result[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(s[0])));
        std::transform(s.begin() + 1, s.end(), result.begin() + 1,
                       [](unsigned char c) { return std::tolower(c); });
        return Str(result);
    }

    // ---------------- Remove ----------------
    inline Str remove(std::string_view sub) const { return replace(sub, ""); }

    // ---------------- Count ----------------
    inline int count(std::string_view sub) const {
        if (sub.empty()) return 0;
        int occurrences = 0;
        size_t pos = 0;
        while ((pos = s.find(sub, pos)) != std::string::npos) {
            ++occurrences;
            pos += sub.size();
        }
        return occurrences;
    }

    // ---------------- Contains ----------------
    inline bool contains(std::string_view sub) const { return s.find(sub) != std::string::npos; }

    // ---------------- Repeat ----------------
    inline Str repeat(int n) const {
        if (n <= 0 || s.empty()) return Str("");
        std::string result;
        result.resize(s.size() * n);
        char* out = result.data();
        for (int i = 0; i < n; ++i) {
            memcpy(out, s.data(), s.size());
            out += s.size();
        }
        return Str(result);
    }

    inline Str operator*(int n) const { return repeat(n); }

    // ---------------- Character Set Checks ----------------
    inline bool isAlpha() const {
        if (s.empty()) return false;
        return std::all_of(s.begin(), s.end(), [](unsigned char c) { return std::isalpha(c); });
    }

    inline bool isDigit() const {
        if (s.empty()) return false;
        return std::all_of(s.begin(), s.end(), [](unsigned char c) { return std::isdigit(c); });
    }

    inline bool isAlnum() const {
        if (s.empty()) return false;
        return std::all_of(s.begin(), s.end(), [](unsigned char c) { return std::isalnum(c); });
    }
};

} // namespace SimpleStr