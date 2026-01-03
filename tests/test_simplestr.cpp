#include "SimpleStr.hpp"
#include <catch2/catch_test_macros.hpp>
#include <string_view>
#include <vector>

using SimpleStr::Str;

// ---------------- Constructors & Conversion ----------------

TEST_CASE("Construction and conversion", "[Str]") {
  Str a;
  REQUIRE(std::string_view(a) == "");

  Str b("hello");
  REQUIRE(b.str() == "hello");
  REQUIRE(std::string_view(b) == "hello");
}

// ---------------- startswith / endswith ----------------

TEST_CASE("Startswith / Endswith", "[Str]") {
  Str s("hello_world");

  REQUIRE(s.startswith("hello"));
  REQUIRE_FALSE(s.startswith("world"));

  REQUIRE(s.endswith("world"));
  REQUIRE_FALSE(s.endswith("hello"));
}

// ---------------- Split ----------------

TEST_CASE("Split by character", "[Str][Split]") {
  Str s("a,b,c");
  auto parts = s.split(',');

  REQUIRE(parts.size() == 3);
  REQUIRE(parts[0] == "a");
  REQUIRE(parts[1] == "b");
  REQUIRE(parts[2] == "c");
}

TEST_CASE("Split whitespace (Python-style)", "[Str][Split]") {
  Str s("  hello   world \n test ");
  auto parts = s.split();

  REQUIRE(parts.size() == 3);
  REQUIRE(parts[0] == "hello");
  REQUIRE(parts[1] == "world");
  REQUIRE(parts[2] == "test");
}

// ---------------- Join ----------------

TEST_CASE("Static join", "[Str][Join]") {
  std::vector<std::string_view> parts = {"a", "b", "c"};
  auto joined = Str::join(parts, ",");

  REQUIRE(joined == "a,b,c");
}

TEST_CASE("Member join", "[Str][Join]") {
  Str sep(",");
  std::vector<Str> parts = {Str("x"), Str("y"), Str("z")};

  auto result = sep.join(parts, ",");
  REQUIRE(result.str() == "x,y,z");
}

// ---------------- Slice ----------------

TEST_CASE("Slice basic", "[Str][Slice]") {
  Str s("abcdef");

  REQUIRE(s.slice(0, 3).str() == "abc");
  REQUIRE(s.slice(1, 5).str() == "bcde");
}

TEST_CASE("Slice with negative indices", "[Str][Slice]") {
  Str s("abcdef");

  REQUIRE(s.slice(-3, -1).str() == "de");
}

TEST_CASE("Slice with step", "[Str][Slice]") {
  Str s("abcdef");

  REQUIRE(s.slice(0, 6, 2).str() == "ace");
  REQUIRE(s.slice(5, 0, -2).str() == "fdb");
}

// ---------------- Replace / Remove ----------------

TEST_CASE("Replace", "[Str]") {
  Str s("one two two three");

  auto r = s.replace("two", "2");
  REQUIRE(r.str() == "one 2 2 three");
}

TEST_CASE("Remove", "[Str]") {
  Str s("banana");

  REQUIRE(s.remove("na").str() == "ba");
}

// ---------------- Strip ----------------

TEST_CASE("Strip functions", "[Str]") {
  Str s("  \t hello world \n ");

  REQUIRE(s.lstrip().str() == "hello world \n ");
  REQUIRE(s.rstrip().str() == "  \t hello world");
  REQUIRE(s.strip().str() == "hello world");
}

// ---------------- Case operations ----------------

TEST_CASE("Lower / Upper / Capitalize", "[Str]") {
  Str s("hELLo");

  REQUIRE(s.lower().str() == "hello");
  REQUIRE(s.upper().str() == "HELLO");
  REQUIRE(s.capitalize().str() == "Hello");
}

TEST_CASE("Capitalize empty", "[Str]") {
  Str s("");
  REQUIRE(s.capitalize().str() == "");
}

// ---------------- Count / Contains ----------------

TEST_CASE("Count", "[Str]") {
  Str s("abababa");

  REQUIRE(s.count("aba") == 2);
  REQUIRE(s.count("x") == 0);
  REQUIRE(s.count("") == 0);
}

TEST_CASE("Contains", "[Str]") {
  Str s("hello world");

  REQUIRE(s.contains("world"));
  REQUIRE_FALSE(s.contains("planet"));
}

// ---------------- Repeat ----------------

TEST_CASE("Repeat", "[Str]") {
  Str s("ab");

  REQUIRE(s.repeat(3).str() == "ababab");
  REQUIRE((s * 2).str() == "abab");
  REQUIRE(s.repeat(0).str() == "");
}

// ---------------- Character class checks ----------------

TEST_CASE("isAlpha", "[Str]") {
  REQUIRE(Str("abc").isAlpha());
  REQUIRE_FALSE(Str("abc123").isAlpha());
  REQUIRE_FALSE(Str("").isAlpha());
}

TEST_CASE("isDigit", "[Str]") {
  REQUIRE(Str("12345").isDigit());
  REQUIRE_FALSE(Str("12a").isDigit());
  REQUIRE_FALSE(Str("").isDigit());
}

TEST_CASE("isAlnum", "[Str]") {
  REQUIRE(Str("abc123").isAlnum());
  REQUIRE_FALSE(Str("abc!").isAlnum());
  REQUIRE_FALSE(Str("").isAlnum());
}
