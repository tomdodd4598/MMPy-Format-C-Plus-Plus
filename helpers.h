#ifndef MMPY_HELPERS_H
#define MMPY_HELPERS_H

#include <array>
#include <concepts>
#include <cstddef>
#include <functional>
#include <list>
#include <map>
#include <set>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#define L_PAR (
#define R_PAR )

#define EXPAND(...) __VA_ARGS__
#define EXPAND2(...) EXPAND L_PAR __VA_ARGS__ R_PAR
#define EXPAND3(...) EXPAND2 L_PAR __VA_ARGS__ R_PAR
#define EXPAND4(...) EXPAND3 L_PAR __VA_ARGS__ R_PAR
#define EXPAND5(...) EXPAND4 L_PAR __VA_ARGS__ R_PAR
#define EXPAND6(...) EXPAND5 L_PAR __VA_ARGS__ R_PAR
#define EXPAND7(...) EXPAND6 L_PAR __VA_ARGS__ R_PAR
#define EXPAND8(...) EXPAND7 L_PAR __VA_ARGS__ R_PAR
#define EXPAND9(...) EXPAND8 L_PAR __VA_ARGS__ R_PAR

template<typename FUNCTION, typename TUPLE>
constexpr decltype(auto) apply_ordered(FUNCTION&& f, TUPLE&& t) {
	std::apply([&](auto const&... x) {(..., f(x));}, t);
}

#endif
