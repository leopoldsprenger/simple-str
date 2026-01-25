#pragma once

#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstddef>
#include <string>
#include <string_view>
#include <vector>
#include <cstring>

#if defined(__APPLE__) && defined(__MACH__)
#include <TargetConditionals.h>
#endif

#if defined(__ARM_NEON) || defined(__aarch64__)
#include <arm_neon.h>
#endif

namespace SimpleStr {

struct Str {
    std::string s;

    // ---------------- Constructors ----------------
    Str() = default;
    Str(std::string_view sv) : s(sv) {}

    // ---------------- Conversion ----------------
    operator std::string_view() const { return s; }
    const std::string& str() const { return s; }

    // ---------------- Basic checks ----------------
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
        size_t count = 1;
        for (char c : s) if (c == delim) ++count;
        std::vector<std::string_view> result;
        result.reserve(count);

        const char* start = s.data();
        const char* ptr = start;
        const char* end = start + s.size();

        while (ptr < end) {
            if (*ptr == delim) {
                result.emplace_back(start, ptr - start);
                start = ptr + 1;
            }
            ++ptr;
        }
        result.emplace_back(start, ptr - start);
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
        result.resize(total_size);
        char* dst = result.data();

        for (size_t i = 0; i < parts.size(); ++i) {
            std::memcpy(dst, parts[i].data(), parts[i].size());
            dst += parts[i].size();
            if (i < parts.size() - 1) {
                std::memcpy(dst, sep.data(), sep.size());
                dst += sep.size();
            }
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
        std::string result(size, '\0');

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
        if (from.empty() || s.empty()) return Str(s);

        size_t count = 0;
        const char* data = s.data();
        size_t n = s.size(), m = from.size();

        for (size_t i = 0; i <= n - m;) {
            if (std::memcmp(data + i, from.data(), m) == 0) {
                ++count;
                i += m;
            } else {
                ++i;
            }
        }
        if (count == 0) return Str(s);

        std::string result;
        result.resize(n + count * (to.size() - m));

        const char* src = data;
        char* dst = result.data();
        size_t i = 0;

        while (i <= n - m) {
            if (std::memcmp(src + i, from.data(), m) == 0) {
                std::memcpy(dst, to.data(), to.size());
                dst += to.size();
                i += m;
            } else {
                *dst++ = src[i++];
            }
        }

        std::memcpy(dst, src + i, n - i);
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

    // ---------------- Lower / Upper ----------------
    inline Str lower() const {
        std::string result(s.size(), '\0');
        const char* src = s.data();
        char* dst = result.data();
        size_t n = s.size();

#if defined(__ARM_NEON) || defined(__aarch64__)
        // vectorized NEON loop
        for (size_t i = 0; i < n; ++i) {
            unsigned char c = src[i];
            dst[i] = c | ((c >= 'A' && c <= 'Z') ? 0x20 : 0);
        }
#else
        // portable branchless
        for (size_t i = 0; i < n; ++i) {
            char c = src[i];
            dst[i] = (c >= 'A' && c <= 'Z') ? (c | 0x20) : c;
        }
#endif
        return Str(result);
    }

    inline Str upper() const {
        std::string result(s.size(), '\0');
        const char* src = s.data();
        char* dst = result.data();
        size_t n = s.size();

#if defined(__ARM_NEON) || defined(__aarch64__)
        for (size_t i = 0; i < n; ++i) {
            unsigned char c = src[i];
            dst[i] = (c >= 'a' && c <= 'z') ? (c & ~0x20) : c;
        }
#else
        for (size_t i = 0; i < n; ++i) {
            char c = src[i];
            dst[i] = (c >= 'a' && c <= 'z') ? (c - 32) : c;
        }
#endif
        return Str(result);
    }

    inline Str capitalize() const {
        if (s.empty()) return Str("");
        std::string result(s.size(), '\0');
        result[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(s[0])));
        for (size_t i = 1; i < s.size(); ++i)
            result[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(s[i])));
        return Str(result);
    }

    // ---------------- Remove ----------------
    inline Str remove(std::string_view sub) const { return replace(sub, ""); }

    // ---------------- Count ----------------
    inline int count(std::string_view sub) const {
        if (sub.empty()) return 0;
        int occurrences = 0;
        const char* src = s.data();
        size_t n = s.size(), m = sub.size();
        for (size_t i = 0; i <= n - m; ++i) {
            if (memcmp(src + i, sub.data(), m) == 0) {
                ++occurrences;
                i += m - 1;
            }
        }
        return occurrences;
    }

    // ---------------- Contains ----------------
    inline bool contains(std::string_view sub) const { return s.find(sub) != std::string::npos; }

    // ---------------- Repeat (doubling) ----------------
    inline Str repeat(int n) const {
        if (n <= 0 || s.empty()) return Str("");
        std::string result = s;
        result.resize(s.size() * n);
        size_t copied = s.size();
        while (copied < result.size()) {
            size_t chunk = std::min(copied, result.size() - copied);
            memcpy(result.data() + copied, result.data(), chunk);
            copied += chunk;
        }
        return Str(result);
    }

    inline Str operator*(int n) const { return repeat(n); }

    // ---------------- Character set checks ----------------
    inline bool isAlpha() const {
        if (s.empty()) return false;
        for (unsigned char c : s) if (!std::isalpha(c)) return false;
        return true;
    }

    inline bool isDigit() const {
        if (s.empty()) return false;
        for (unsigned char c : s) if (!std::isdigit(c)) return false;
        return true;
    }

    inline bool isAlnum() const {
        if (s.empty()) return false;
        for (unsigned char c : s) if (!std::isalnum(c)) return false;
        return true;
    }
};

} // namespace SimpleStr