#ifndef MMPY_FORMAT_H
#define MMPY_FORMAT_H

#include "helpers.h"

#define MMPY_FORMAT_PTR0(MMPY, PTR)\
template<typename T>\
std::string MMPY##_format(T PTR v) {\
	return *v;\
}\

#define MMPY_FORMAT_PTR(PTR)\
MMPY_FORMAT_PTR0(mm, PTR)\
MMPY_FORMAT_PTR0(py, PTR)\

MMPY_FORMAT_PTR(*)
MMPY_FORMAT_PTR(const*)

#define MMPY_FORMAT_SIMPLE0(MMPY, T, REF, VALUE)\
std::string MMPY##_format(T REF v) {\
	return VALUE;\
}\

#define MMPY_FORMAT_SIMPLE1(MMPY, T, LV, RV)\
MMPY_FORMAT_SIMPLE0(MMPY, T, const&, LV)\
MMPY_FORMAT_SIMPLE0(MMPY, T, &&, RV)\

#define MMPY_FORMAT_SIMPLE(T, LV, RV)\
MMPY_FORMAT_SIMPLE1(mm, T, LV, RV)\
MMPY_FORMAT_SIMPLE1(py, T, LV, RV)\

#define MMPY_FORMAT_SCALAR(T, VALUE) MMPY_FORMAT_SIMPLE(T, VALUE, VALUE)

#define MMPY_FORMAT_NUM(T) MMPY_FORMAT_SCALAR(T, std::to_string(v))

MMPY_FORMAT_NUM(short)
MMPY_FORMAT_NUM(unsigned short)
MMPY_FORMAT_NUM(int)
MMPY_FORMAT_NUM(unsigned)
MMPY_FORMAT_NUM(long)
MMPY_FORMAT_NUM(unsigned long)
MMPY_FORMAT_NUM(long long)
MMPY_FORMAT_NUM(unsigned long long)

MMPY_FORMAT_NUM(float)
MMPY_FORMAT_NUM(double)
MMPY_FORMAT_NUM(long double)

MMPY_FORMAT_SCALAR(bool, v ? "True" : "False")

#define CASE(VALUE, BODY) case VALUE: BODY; break;

// Modified from https://stackoverflow.com/a/7369771
namespace {
	std::string unescape_char(char ch) {
		if (isprint(ch) && ch != '\'' && ch != '\"' && ch != '\\' && ch != '\?') {
			return std::string{ch};
		}

		switch (ch) {
			CASE('\a', return "\\a")
				CASE('\b', return "\\b")
				CASE('\f', return "\\f")
				CASE('\n', return "\\n")
				CASE('\r', return "\\r")
				CASE('\t', return "\\t")
				CASE('\v', return "\\v")
				CASE('\\', return "\\\\")
				CASE('\'', return "\\'")
				CASE('\"', return "\\\"")
				CASE('\?', return "\\\?")
		default:
			return std::string{ch};
		}
	}

	template<typename STRING>
	std::string unescape(STRING&& str) {
		std::string result;
		for (char ch : str) {
			result += unescape_char(ch);
		}
		return result;
	}
}

#define MMPY_FORMAT_STRING0(MMPY, T, REF, DELIM, VALUE)\
std::string MMPY##_format(T REF v) {\
	std::string str = DELIM;\
	str += unescape(VALUE);\
	str += DELIM;\
	return str;\
}\

#define MMPY_FORMAT_STRING1(MMPY, T, DELIM, LV, RV)\
MMPY_FORMAT_STRING0(MMPY, T, const&, DELIM, LV)\
MMPY_FORMAT_STRING0(MMPY, T, &&, DELIM, RV)\

#define MMPY_FORMAT_STRING(T, LV, RV)\
MMPY_FORMAT_STRING1(mm, T, "\"", LV, RV)\
MMPY_FORMAT_STRING1(py, T, "'", LV, RV)\

MMPY_FORMAT_STRING(char, std::string{v}, std::string{v})
MMPY_FORMAT_STRING(char*, std::string(v), std::string(v))
MMPY_FORMAT_STRING(char const*, std::string(v), std::string(v))
MMPY_FORMAT_STRING(std::string, v, std::move(v))

#define MMPY_FORMAT_ITERABLE0(MMPY, TEMPLATE, TYPE, REF, PRE, START, DELIM, POST, END)\
template<TEMPLATE>\
std::string MMPY##_format(TYPE REF v) {\
	PRE;\
	std::string str = START;\
	bool begin = true;\
	for (auto const& e : v) {\
		if (begin) {\
			begin = false;\
		}\
		else {\
			str += DELIM;\
		}\
		str += MMPY##_format(e);\
	}\
	POST;\
	str += END;\
	return str;\
}\

#define MMPY_FORMAT_ITERABLE1(MMPY, TEMPLATE, TYPE, PRE, START, DELIM, POST, END)\
MMPY_FORMAT_ITERABLE0(MMPY, TEMPLATE, TYPE, const&, PRE, START, DELIM, POST, END)\
MMPY_FORMAT_ITERABLE0(MMPY, TEMPLATE, TYPE, &&, PRE, START, DELIM, POST, END)\

#define MMPY_FORMAT_ITERABLE2(MMPY, TYPE, PRE, START, DELIM, POST, END)\
MMPY_FORMAT_ITERABLE1(MMPY, typename T, TYPE<T>, PRE, START, DELIM, POST, END)\

#define MMPY_FORMAT_LIST(MMPY, PRE, START, DELIM, POST, END)\
MMPY_FORMAT_ITERABLE1(MMPY, EXPAND2(typename T, size_t N), std::array<EXPAND2(T, N)>, PRE, START, DELIM, POST, END)\
MMPY_FORMAT_ITERABLE2(MMPY, std::vector, PRE, START, DELIM, POST, END)\
MMPY_FORMAT_ITERABLE2(MMPY, std::list, PRE, START, DELIM, POST, END)\

MMPY_FORMAT_LIST(mm, {}, "{", ",", {}, "}")
MMPY_FORMAT_LIST(py, {}, "(", ", ", if (v.size() == 1) str += ",", ")")

#define MMPY_FORMAT_SET(MMPY, PRE, START, DELIM, POST, END)\
MMPY_FORMAT_ITERABLE2(MMPY, std::set, PRE, START, DELIM, POST, END)\
MMPY_FORMAT_ITERABLE2(MMPY, std::unordered_set, PRE, START, DELIM, POST, END)\

MMPY_FORMAT_SET(mm, {}, "{", ",", {}, "}")
MMPY_FORMAT_SET(py, if (v.empty()) return "set()", "{", ", ", {}, "}")

#define MMPY_FORMAT_MAP0(MMPY, TYPE, REF, PRE, START, DELIM, SEP, POST, END)\
template<typename K, typename V>\
std::string MMPY##_format(TYPE<K, V> REF v) {\
	std::string str = START;\
	bool begin = true;\
	for (auto const& e : v) {\
		if (begin) {\
			begin = false;\
		}\
		else {\
			str += DELIM;\
		}\
		str += MMPY##_format(e.first);\
		str += SEP;\
		str += MMPY##_format(e.second);\
	}\
	str += END;\
	return str;\
}\

#define MMPY_FORMAT_MAP1(MMPY, TYPE, PRE, START, DELIM, SEP, POST, END)\
MMPY_FORMAT_MAP0(MMPY, TYPE, const&, PRE, START, DELIM, SEP, POST, END)\
MMPY_FORMAT_MAP0(MMPY, TYPE, &&, PRE, START, DELIM, SEP, POST, END)\

#define MMPY_FORMAT_MAP(MMPY, PRE, START, DELIM, SEP, POST, END)\
MMPY_FORMAT_MAP1(MMPY, std::map, PRE, START, DELIM, SEP, POST, END)\
MMPY_FORMAT_MAP1(MMPY, std::unordered_map, PRE, START, DELIM, SEP, POST, END)\

MMPY_FORMAT_MAP(mm, {}, "<|", ",", "->", {}, "|>")
MMPY_FORMAT_MAP(py, {}, "{", ", ", ": ", {}, "}")

#define MMPY_FORMAT_TUPLE0(MMPY, REF, PRE, START, DELIM, POST, END)\
template<typename... T>\
std::string MMPY##_format(std::tuple<T...> REF v) {\
	PRE;\
	std::string str = START;\
	bool begin = true;\
	auto f = [&](auto const& e) {\
		if (begin) {\
			begin = false;\
		}\
		else {\
			str += DELIM;\
		}\
		str += MMPY##_format(e);\
	};\
	apply_ordered([&](auto const& e) {f(e);}, v);\
	POST;\
	str += END;\
	return str;\
}\

#define MMPY_FORMAT_TUPLE(MMPY, PRE, START, DELIM, POST, END)\
MMPY_FORMAT_TUPLE0(MMPY, const&, PRE, START, DELIM, POST, END)\
MMPY_FORMAT_TUPLE0(MMPY, &&, PRE, START, DELIM, POST, END)\

MMPY_FORMAT_TUPLE(mm, {}, "{", ",", {}, "}")
MMPY_FORMAT_TUPLE(py, {}, "(", ", ", if (std::tuple_size<std::tuple<T...>>::value == 1) str += ",", ")")

#endif
