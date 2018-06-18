/*
 * Copyright (c) 2018, Matheus Izvekov <mizvekov@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#pragma once

#include <functional>
#include <type_traits>
#include <limits>
#include <algorithm>

#include <fp/fp.hpp>

namespace stdx {
	using namespace fp::operators;
#define DECL template<class T, class U> constexpr
	struct shift_right { DECL auto operator()(const T& x, const U& y) { return x >> y; } };
	struct shift_left  { DECL auto operator()(const T& x, const U& y) { return x << y; } };
#undef DECL
};

template<typename A> struct is_ranged : std::false_type {};

template<class T, T LOW, T MAX> struct ranged_t; //forward declare

template<class T, T LOW = std::numeric_limits<T>::lowest(), T MAX = std::numeric_limits<T>::max()>
static constexpr auto ranged(const T &a) { return ranged_t<T, LOW, MAX>(a); }

template<class T, T V>
static constexpr auto ranged(const std::integral_constant<T,V> &a) { return ranged_t<T, V, V>(a); }

class empty {};

template<class T, T LOW = std::numeric_limits<T>::lowest(),
                  T MAX = std::numeric_limits<T>::max()> struct ranged_t :
	          std::conditional_t<LOW == MAX, std::integral_constant<T,LOW>, empty> {

	using base_type = std::conditional_t<LOW != MAX, T, std::integral_constant<T,LOW>>;

	static_assert(std::numeric_limits<T>::is_specialized, "invalid type");
	static_assert(LOW <= MAX, "please fix your low and max values");
	static_assert(LOW >= std::numeric_limits<T>::lowest(), "low overflow");
	static_assert(MAX <= std::numeric_limits<T>::max(), "max overflow");

	// Public Constructors
	constexpr ranged_t() : base{} {};
	template<class A> constexpr ranged_t(const A &a) : base(a) {
		// could assert here in case 'a' was out of range
		// but I dont see how to preserve constexpr'ness
	}
	template<class A, A A_LOW, A A_MAX> constexpr ranged_t(const ranged_t<A,A_LOW,A_MAX> &a) : ranged_t(a.base) {
		static_assert(A_LOW >= lo, "");
		static_assert(A_MAX <= hi, "");
	}
	//

	// Operators Implementation
	template<class U, class = std::enable_if_t<!is_ranged<U>{}>>
	explicit constexpr operator U() const {
		static_assert(!std::numeric_limits<U>::is_specialized || std::numeric_limits<U>::lowest() <= LOW, "");
		static_assert(!std::numeric_limits<U>::is_specialized || std::numeric_limits<U>::max() >= MAX, "");
		return U(base);
	}

	template<class B> constexpr auto& operator =(const B &b) { return base = ranged_t(b).base, *this; }

#define DECL template<class B, class = std::enable_if_t< is_ranged<B>{} || fp::detail::is_std_int_const<B>{} > > constexpr
	DECL auto operator+(const B &b) const { return bin_op<rplus      , std::plus      <>>(*this, proxy(b)); }
	DECL auto operator-(const B &b) const { return bin_op<rminus     , std::minus     <>>(*this, proxy(b)); }
	DECL auto operator*(const B &b) const { return bin_op<rmultiplies, std::multiplies<>>(*this, proxy(b)); }
	DECL auto operator/(const B &b) const { return bin_op<rdivides   , std::divides   <>>(*this, proxy(b)); }
//	DECL auto operator%(const B &b) { return ; }
//	DECL auto operator&(const B &b) { return ; }
//	DECL auto operator|(const B &b) { return ; }
//	DECL auto operator^(const B &b) { return ; }

	DECL auto operator>>(const B &b) const { return bin_op<rshift_right, stdx::shift_right>(*this, proxy(b)); }
	DECL auto operator<<(const B &b) const { return bin_op<rshift_left , stdx::shift_left >(*this, proxy(b)); }

	DECL bool operator==(const B &b) const { return cmp_op<requal, std::equal_to<>>(*this, proxy(b)); }
	DECL bool operator <(const B &b) const { return cmp_op<rless , std::less    <>>(*this, proxy(b)); }

	DECL bool operator<=(const B &b) const { return *this == b || *this <  b; }
	DECL bool operator!=(const B &b) const { return !(*this == b); }
	DECL bool operator >(const B &b) const { return !(*this <= b); }
	DECL bool operator>=(const B &b) const { return !(*this <  b); }
#undef DECL

	constexpr auto operator+() const { return ranged_t<decltype(+base), +lo, +hi>(+base); }
	constexpr auto operator-() const { return ranged_t<decltype(-base), -hi, -lo>(-base); }
//	constexpr auto operator~() const { return ; }
	//

	// These implement the remaining operators based on the previous ones
	template<class B> constexpr auto& operator+=(const B &b) { return *this = *this + b; }
	template<class B> constexpr auto& operator-=(const B &b) { return *this = *this - b; }
	template<class B> constexpr auto& operator*=(const B &b) { return *this = *this * b; }
	template<class B> constexpr auto& operator/=(const B &b) { return *this = *this / b; }
	template<class B> constexpr auto& operator%=(const B &b) { return *this = *this % b; }

	template<class B> constexpr auto& operator<<=(const B &b) { return *this = *this << b; }
	template<class B> constexpr auto& operator>>=(const B &b) { return *this = *this >> b; }

	constexpr auto& operator++()    { return *this += 1; }
	constexpr auto& operator--()    { return *this -= 1; }
	constexpr auto  operator++(int) { auto ret = *this; return *this += 1, ret; }
	constexpr auto  operator--(int) { auto ret = *this; return *this -= 1, ret; }
	//
private:
	static constexpr auto lo = LOW, hi = MAX;
	template<class A, A, A> friend struct ranged_t;

	template<class B, class = std::enable_if_t< is_ranged<B>{} > >
	static constexpr const B& proxy(const B &b) { return b; }

	template<class B, class = std::enable_if_t< fp::detail::is_std_int_const<B>{} > >
	static constexpr auto proxy(const B &b) { return ranged(b); }

	// Binary Operator Range Operations
#define DECL template<class A, class B, class R> struct
	DECL rplus       { static constexpr R min = A::lo + B::lo, max = A::hi + B::hi; };
	DECL rminus      { static constexpr R min = A::lo - B::hi, max = A::hi - B::lo; };
	DECL rmultiplies {
		static constexpr R min = std::min(std::initializer_list<R>{
			A::lo * B::lo, A::lo * B::hi, A::hi * B::lo, A::hi * B::hi
		});
		static constexpr R max = std::max(std::initializer_list<R>{
			A::lo * B::lo, A::lo * B::hi, A::hi * B::lo, A::hi * B::hi
		});
	};
	DECL rdivides {
		static_assert(B::hi > typename B::base_type(0) || B::hi < typename B::base_type(0),
		              "divisor range may not contain zero");
		static constexpr R min = std::min(std::initializer_list<R>{
			A::lo / B::lo, A::lo / B::hi, A::hi / B::lo, A::hi / B::hi
		});
		static constexpr R max = std::max(std::initializer_list<R>{
			A::lo / B::lo, A::lo / B::hi, A::hi / B::lo, A::hi / B::hi
		});
	};
	DECL rshift_right {
		static constexpr R min = fp::detail::const_shift<-intmax_t(B::hi)>(A::lo),
		                   max = fp::detail::const_shift<-intmax_t(B::lo)>(A::hi);
	};
	DECL rshift_left  {
		static constexpr R min = fp::detail::const_shift<+intmax_t(B::lo)>(A::lo),
		                   max = fp::detail::const_shift<+intmax_t(B::hi)>(A::hi);
	};
#undef DECL
	// Compare Range Operations
#define DECL template<class A, class B> struct
	DECL requal {
		static constexpr bool always_true  = A::lo == A::hi && B::lo == B::hi && A::lo == B::lo,
	                              always_false = A::lo > B::hi || A::hi < B::lo;
	};
	DECL rless  { static constexpr bool always_true = A::hi < B::lo, always_false = A::lo >= B::hi; };
#undef DECL
	//

	// Generic Binary Operator Implementation
	template<template<class,class,class> class ROP, class BOP,
		class A, A A_LOW, A A_MAX, class B, B B_LOW, B B_MAX>
	static constexpr auto bin_op(const ranged_t<A, A_LOW, A_MAX> &a, const ranged_t<B, B_LOW, B_MAX> &b) {
		auto res = BOP{}(a.base, b.base);
		using rop = ROP<std::decay_t<decltype(a)>, std::decay_t<decltype(b)>, decltype(res)>;
		return ranged_t<decltype(res), rop::min, rop::max>(res);
	}

	template<template<class,class> class ROP, class BOP,
		class A, A A_LOW, A A_MAX, class B, B B_LOW, B B_MAX>
	static constexpr bool cmp_op(const ranged_t<A, A_LOW, A_MAX> &a, const ranged_t<B, B_LOW, B_MAX> &b) {
		using rop = ROP<std::decay_t<decltype(a)>, std::decay_t<decltype(b)>>;
		return rop::always_true || (!rop::always_false && BOP{}(a.base, b.base));
	}
	//

	base_type base;
};

namespace std {
template<class A, A A_LOW, A A_MAX> struct numeric_limits<ranged_t<A,A_LOW,A_MAX>> {
	using base = numeric_limits<A>;

	static constexpr bool is_specialized = base::is_specialized;

	static constexpr auto min()    noexcept {
		using t = ::std::integral_constant<A, std::max(A_LOW, base::min())>;
		return ranged_t<A, t{}, t{}>{};
	}
	static constexpr auto max()    noexcept { return ranged_t<A, A_MAX, A_MAX>{}; }
	static constexpr auto lowest() noexcept { return ranged_t<A, A_LOW, A_LOW>{}; }

	static constexpr int  digits       = base::digits;
	static constexpr int  digits10     = base::digits10;
	static constexpr int  max_digits10 = base::max_digits10;
	static constexpr bool is_signed    = base::is_signed;
	static constexpr bool is_integer   = base::is_integer;
	static constexpr bool is_exact     = base::is_exact;
	static constexpr int  radix        = base::radix;

	static constexpr auto epsilon()     noexcept { return base::epsilon(); }
	static constexpr auto round_error() noexcept { return base::round_error(); }

	static constexpr int  min_exponent   = base::min_exponent;
	static constexpr int  min_exponent10 = base::min_exponent10;
	static constexpr int  max_exponent   = base::max_exponent;
	static constexpr int  max_exponent10 = base::max_exponent10;

	static constexpr bool has_infinity      = base::has_infinity;
	static constexpr bool has_quiet_NaN     = base::has_quiet_NaN;
	static constexpr bool has_signaling_NaN = base::has_signaling_NaN;
	static constexpr float_denorm_style has_denorm = base::has_denorm;
	static constexpr bool has_denorm_loss = base::has_denorm_loss;

	static constexpr A infinity()      noexcept { return base::infinity(); }
	static constexpr A quiet_NaN()     noexcept { return base::quiet_NaN(); }
	static constexpr A signaling_NaN() noexcept { return base::signaling_NaN(); }
	static constexpr A denorm_min()    noexcept { return base::denorm_min(); }

	static constexpr bool is_iec559  = base::is_iec559;
	static constexpr bool is_bounded = base::is_bounded;
	static constexpr bool is_modulo  = base::is_modulo;

	static constexpr bool traps = base::traps;
	static constexpr bool tinyness_before = base::tinyness_before;
	static constexpr float_round_style round_style = base::round_style;
};
}

template<class A, A A_LOW, A A_MAX> struct is_ranged<ranged_t<A, A_LOW, A_MAX>> : std::true_type {};
