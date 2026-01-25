#include <algorithm>
#include <cctype>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include "SimpleStr.hpp"

using namespace SimpleStr;

// ---------------- Timer ----------------
template <typename Func> double timeFuncMedian(Func f, int runs = 30) {
  std::vector<double> times;
  times.reserve(runs);

  // warm-up
  f();

  for (int i = 0; i < runs; ++i) {
    auto start = std::chrono::steady_clock::now();
    f();
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double, std::milli> dt = end - start;
    times.push_back(dt.count());
  }

  std::sort(times.begin(), times.end());
  return times[runs / 2];
}

// ---------------- Python runner ----------------
std::map<std::string, double> runPythonBenchmark() {
  std::map<std::string, double> results;
  FILE *pipe = popen("python3 bench/benchmark_python.py", "r");
  if (!pipe)
    return results;

  char buf[128];
  while (fgets(buf, sizeof(buf), pipe)) {
    std::string line(buf);
    auto pos = line.find(':');
    if (pos != std::string::npos)
      results[line.substr(0, pos)] = std::stod(line.substr(pos + 1));
  }
  pclose(pipe);
  return results;
}

// ---------------- Main ----------------
int main() {
  constexpr size_t DATA_SIZE = 10'000'000;

  std::string base(DATA_SIZE, 'A');
  for (size_t i = 0; i < DATA_SIZE; i += 10)
    base[i] = ' ';

  std::string csv = base;
  for (size_t i = 0; i < DATA_SIZE; i += 15)
    csv[i] = ',';

  Str s(base);
  Str s_csv(csv);

  // pre-split data (critical)
  auto ss_split_csv = s_csv.split(',');
  auto ss_split_ws = s.split();

  std::vector<std::string_view> std_split_csv;
  {
    size_t start = 0;
    for (size_t i = 0; i < csv.size(); ++i) {
      if (csv[i] == ',') {
        std_split_csv.emplace_back(csv.data() + start, i - start);
        start = i + 1;
      }
    }
    std_split_csv.emplace_back(csv.data() + start, csv.size() - start);
  }

  std::vector<std::string_view> std_split_ws;
  {
    size_t i = 0;
    while (i < base.size()) {
      while (i < base.size() && base[i] == ' ')
        ++i;
      size_t start = i;
      while (i < base.size() && base[i] != ' ')
        ++i;
      if (start < i)
        std_split_ws.emplace_back(base.data() + start, i - start);
    }
  }

  std::map<std::string, double> ssTimes;
  std::map<std::string, double> stdTimes;
  std::map<std::string, double> pyTimes = runPythonBenchmark();

  // ---------------- SimpleStr ----------------
  ssTimes["splitChar"] = timeFuncMedian([&] {
    auto v = s_csv.split(',');
    volatile size_t n = v.size();
    (void)n;
  });

  ssTimes["splitWS"] = timeFuncMedian([&] {
    auto v = s.split();
    volatile size_t n = v.size();
    (void)n;
  });

  ssTimes["join"] = timeFuncMedian([&] {
    auto out = Str::join(ss_split_csv, "-");
    volatile size_t n = out.size();
    (void)n;
  });

  ssTimes["replace"] = timeFuncMedian([&] {
    auto r = s.replace("A", "B");
    volatile size_t n = r.s.size();
    (void)n;
  });

  ssTimes["lower"] = timeFuncMedian([&] {
    auto r = s.lower();
    volatile size_t n = r.s.size();
    (void)n;
  });

  ssTimes["upper"] = timeFuncMedian([&] {
    auto r = s.upper();
    volatile size_t n = r.s.size();
    (void)n;
  });

  ssTimes["strip"] = timeFuncMedian([&] {
    auto r = s.strip();
    volatile size_t n = r.s.size();
    (void)n;
  });

  ssTimes["count"] = timeFuncMedian([&] {
    volatile int c = s.count("A");
    (void)c;
  });

  ssTimes["contains"] = timeFuncMedian([&] {
    volatile bool b = s.contains("XYZ");
    (void)b;
  });

  ssTimes["repeat"] = timeFuncMedian([&] {
    auto r = Str("abc").repeat(100000);
    volatile size_t n = r.s.size();
    (void)n;
  });

  // ---------------- std::string ----------------
  stdTimes["splitChar"] = timeFuncMedian([&] {
    std::vector<std::string_view> v;
    size_t start = 0;
    for (size_t i = 0; i < csv.size(); ++i) {
      if (csv[i] == ',') {
        v.emplace_back(csv.data() + start, i - start);
        start = i + 1;
      }
    }
    v.emplace_back(csv.data() + start, csv.size() - start);
    volatile size_t n = v.size();
    (void)n;
  });

  stdTimes["splitWS"] = timeFuncMedian([&] {
    std::vector<std::string_view> v;
    size_t i = 0;
    while (i < base.size()) {
      while (i < base.size() && base[i] == ' ')
        ++i;
      size_t start = i;
      while (i < base.size() && base[i] != ' ')
        ++i;
      if (start < i)
        v.emplace_back(base.data() + start, i - start);
    }
    volatile size_t n = v.size();
    (void)n;
  });

  stdTimes["join"] = timeFuncMedian([&] {
    std::string out;
    for (size_t i = 0; i < std_split_csv.size(); ++i) {
      if (i)
        out += '-';
      out.append(std_split_csv[i]);
    }
    volatile size_t n = out.size();
    (void)n;
  });

  stdTimes["replace"] = timeFuncMedian([&] {
    std::string out = base;
    for (char &c : out)
      if (c == 'A')
        c = 'B';
    volatile size_t n = out.size();
    (void)n;
  });

  stdTimes["lower"] = timeFuncMedian([&] {
    std::string out = base;
    for (char &c : out)
      if (c >= 'A' && c <= 'Z')
        c += 32;
    volatile size_t n = out.size();
    (void)n;
  });

  stdTimes["upper"] = timeFuncMedian([&] {
    std::string out = base;
    for (char &c : out)
      if (c >= 'a' && c <= 'z')
        c -= 32;
    volatile size_t n = out.size();
    (void)n;
  });

  stdTimes["strip"] = timeFuncMedian([&] {
    size_t l = base.find_first_not_of(" ");
    size_t r = base.find_last_not_of(" ");
    std::string out = base.substr(l, r - l + 1);
    volatile size_t n = out.size();
    (void)n;
  });

  stdTimes["count"] = timeFuncMedian([&] {
    int c = 0;
    for (char ch : base)
      if (ch == 'A')
        ++c;
    volatile int x = c;
    (void)x;
  });

  stdTimes["contains"] = timeFuncMedian([&] {
    volatile bool b = base.find("XYZ") != std::string::npos;
    (void)b;
  });

  stdTimes["repeat"] = timeFuncMedian([&] {
    std::string out;
    out.reserve(300000);
    for (int i = 0; i < 100000; ++i)
      out += "abc";
    volatile size_t n = out.size();
    (void)n;
  });

  // ---------------- Output ----------------
  std::vector<std::string> ops = {"splitChar", "splitWS", "join",  "replace",
                                  "lower",     "upper",   "strip", "count",
                                  "contains",  "repeat"};

  std::cout << std::setw(14) << "Operation" << std::setw(6) << "Mark"
            << std::setw(12) << "SStr(ms)" << std::setw(14) << "vs Python"
            << std::setw(14) << "vs std\n";

  auto fmtDiff = [](double d) {
    std::ostringstream o;
    o << (d >= 0 ? "+" : "") << std::fixed << std::setprecision(2) << d;
    return o.str();
  };

  double tolerance = 0.05;

  for (const auto &op : ops) {
    double tSS = ssTimes[op];
    double tPy = pyTimes.count(op) ? pyTimes[op] : 0.0;
    double tStd = stdTimes[op];

    bool pass = ((tPy == 0.0) || (tSS <= tPy * (1.0 + tolerance))) &&
                (tSS <= tStd * (1.0 + tolerance));

    std::cout << std::setw(14) << op << std::setw(6) << (pass ? "✔" : "✘")
              << std::setw(12) << std::fixed << std::setprecision(2) << tSS
              << std::setw(14) << (tPy ? fmtDiff(tSS - tPy) : "   n/a")
              << std::setw(14) << fmtDiff(tSS - tStd) << "\n";
  }
}
