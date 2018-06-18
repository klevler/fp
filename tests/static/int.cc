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

#include <cstdint>
#include <limits>
#include <fp/fp.hpp>

using namespace fp;
using namespace fp::constants;

/********** fp instances must be standard layout iff base_type is standard layout *************************************/
static_assert(std::is_standard_layout<fp_t<uint32_t,0>>::value, "fp_t<uint32_t> must be standard layout");
/**********************************************************************************************************************/

/********************** Construction and cast tests *******************************************************************/
#define TEST(T,E,V) static_assert(decltype(V)(fp_t<T,E>(V)) == V, "")
TEST(uint32_t, 16, uint32_t(std::numeric_limits<uint8_t>::min()));
TEST(uint32_t, 16, uint32_t(std::numeric_limits<uint8_t>::max()));
TEST(uint32_t, 16, uint32_t(std::numeric_limits<int8_t>::max()));
TEST(uint64_t, 16, uint32_t(std::numeric_limits<uint16_t>::min()));
TEST(uint64_t, 16, uint32_t(std::numeric_limits<uint16_t>::max()));
TEST(uint32_t, 16, uint32_t(std::numeric_limits<int16_t>::max()));
TEST(uint64_t, 16, uint32_t(std::numeric_limits<uint32_t>::min()));
TEST(uint64_t, 16, uint64_t(std::numeric_limits<uint32_t>::max()));
TEST(uint64_t, 16, uint64_t(std::numeric_limits<int32_t>::max()));

TEST(uint32_t, 16, uint32_t(std::numeric_limits<int8_t>::max()));
TEST(uint32_t, 16, uint32_t(std::numeric_limits<int16_t>::max()));
TEST(uint32_t, 16, uint32_t(std::numeric_limits<int16_t>::max()));

TEST(uint16_t, 8, 1.375f);
TEST(uint16_t, 8, 2.625f);
TEST(uint16_t, 8, 0.625);
TEST(uint16_t, 8, 4.25);
TEST(uint16_t, 8, 6.5l);
TEST(uint16_t, 8, 8.125l);

TEST(uint8_t, -4, 1024);
#undef TEST
/**********************************************************************************************************************/

/******* Relational operators tests ***********************************************************************************/
static_assert(fp_t<uint16_t,10>(8.25) == fp_t<uint16_t,8>(8.25), "");
static_assert(fp_t<uint16_t,10>(8.25) == fp_t<uint32_t,16>(8.25), "");
static_assert(fp_t<uint16_t,10>(8.25) != fp_t<uint32_t,16>(8.26), "");
static_assert(fp_t<uint16_t,10>(8.25) < fp_t<uint32_t,16>(10.25), "");
static_assert(fp_t<uint32_t,10>(23.5) < fp_t<uint32_t,16>(23.6), "");
static_assert(fp_t<uint16_t,10>(16.5) > fp_t<uint32_t,16>(10.25), "");
static_assert(fp_t<uint64_t,10>(4.375) <= fp_t<uint32_t,16>(4.375), "");
static_assert(fp_t<uint64_t,10>(4.250) <= fp_t<uint32_t,16>(4.375), "");
static_assert(fp_t<uint64_t,10>(4.750) >= fp_t<uint32_t,16>(4.375), "");
static_assert(fp_t<uint64_t,10>(4.750) >= fp_t<uint32_t,16>(4.750), "");
/**********************************************************************************************************************/

/**************** Unary operators tests *******************************************************************************/
static_assert(+fp_t<uint64_t,10>(4.750) == fp_t<uint32_t,16>(4.750), "");
static_assert(~fp_t<uint32_t,0>(0xDEADBEEF) == fp_t<uint64_t,-8>(~0xDEADBEEF), "");
static_assert(decltype(+fp_t<uint64_t,10>(0))::exp{} == 10, "");
static_assert(decltype(~fp_t<uint32_t,-5>(0))::exp{} == -5, "");
static_assert(std::is_same<decltype(+fp_t<uint8_t,8>())::base_type, decltype(+uint8_t())>::value, "");
/**********************************************************************************************************************/

/*********************** Binary operators tests ***********************************************************************/
static_assert(fp_t<uint32_t,16>(5.25) + fp_t<uint32_t,8>(4.75) == fp_t<uint32_t>(10), "");
static_assert(fp_t<uint32_t>(7) == fp_t<uint32_t>(1) + fp_t<uint32_t,16>(6), "");
static_assert(fp_t<uint32_t,16>(5.25) - fp_t<uint32_t,8>(4.75) == fp_t<uint8_t,4>(0.5), "");
static_assert(fp_t<uint32_t,14>(7) - fp_t<uint32_t,12>(5) == fp_t<uint32_t>(2.0), "");
static_assert(fp_t<uint32_t,4>(4.75) * fp_t<uint32_t,8>(5.25) == fp_t<uint32_t,10>(24.9375), "");
static_assert(fp_t<uint32_t,16>(24.9375) / fp_t<uint32_t,8>(5.25) == fp_t<uint32_t,4>(4.75), "");
static_assert(fp_t<uint32_t,16>(18.5) % fp_t<uint32_t,8>(4.25) == fp_t<uint32_t,2>(1.5), "");

static_assert(fp_t<uint32_t,10>(8.0) == fp_t<uint16_t,4>(80) / fp_t<uint32_t,6>(10), "");
static_assert(fp_t<uint32_t,16>(18.5) == fp_t<uint32_t,16>(185) / fp_t<uint32_t>(10), "");

static_assert((fp_t<uint32_t,8>(0xDEAD) & fp_t<uint32_t>(0x7777)) == fp_t<uint32_t>(0x5625), "");
static_assert((fp_t<uint16_t,8>(9.640625) & fp_t<uint16_t,8>(0xFF, {})) == fp_t<uint16_t,8>(0.640625), "");
static_assert((fp_t<uint32_t,8>(0x5625) | fp_t<uint32_t>(0x8888)) == fp_t<uint32_t,8>(0xDEAD), "");
static_assert((fp_t<uint32_t,8>(0xDEAD) ^ fp_t<uint32_t>(0x6042)) == fp_t<uint32_t,8>(0xBEEF), "");

static_assert(fp_t<uint32_t,8>(33) == (fp_t<uint32_t,8>(8.25) << 2), "");
static_assert((fp_t<uint32_t,8>(33) >> 2) == fp_t<uint32_t,8>(8.25), "");

static_assert( (fp_t<uint32_t,8>(8.25) << int_<2>) == fp_t<uint32_t,4>(33), "");
static_assert( (fp_t<uint32_t,8>(33  ) >> int_<2>) == fp_t<uint32_t,4>(8.25), "");

static_assert(std::is_same<decltype(
	fp_t<uint64_t,8>() + fp_t<uint8_t,8>())::base_type,
	decltype(uint64_t()+uint8_t())
	>::value, "");
static_assert(std::is_same< decltype(fp_t<uint16_t,8>() << 2ULL)::base_type, decltype(uint16_t() << 2ULL)>::value, "");

static_assert(decltype(fp_t<uint64_t,10>(1) + fp_t<uint8_t, 8>(2))::exp{} == 10, "");
static_assert(decltype(fp_t<uint64_t,12>(1) - fp_t<uint8_t, 6>(2))::exp{} == 12, "");
static_assert(decltype(fp_t<uint64_t,12>(1) * fp_t<uint8_t, 6>(2))::exp{} == 18, "");
static_assert(decltype(fp_t<uint64_t,12>(1) / fp_t<uint8_t, 4>(2))::exp{} ==  8, "");
static_assert(decltype(fp_t<uint64_t,12>(1) % fp_t<uint8_t, 6>(2))::exp{} == 12, "");
static_assert(decltype(fp_t<uint64_t, 8>(1) & fp_t<uint8_t, 6>(2))::exp{} ==  8, "");
static_assert(decltype(fp_t<uint64_t, 5>(1) | fp_t<uint8_t,13>(2))::exp{} == 13, "");
static_assert(decltype(fp_t<uint64_t,-3>(1) ^ fp_t<uint8_t,-2>(2))::exp{} == -2, "");

static_assert(decltype(fp_t<uint64_t,12>(1) << 2)::exp{} == 12, "");
static_assert(decltype(fp_t<uint64_t,10>(1) >> 2)::exp{} == 10, "");
/**********************************************************************************************************************/

class int_test {
	static constexpr fp_t<uint16_t,10> t1 = 1;
	static constexpr fp_t<uint32_t,12> t2 = 2;
	static constexpr fp_t<uint64_t,16> t3 = 7.25;
	static constexpr auto t4 = fp_t<uint16_t>(3) + t1 + t3 - t2;
	static_assert(t4 == fp_t<uint32_t,16>(9.25), "");
	static_assert(std::is_same<decltype(t4)::base_type, uint64_t>::value, "");
	static_assert(decltype(t4)::exp{} == 16, "");

	static constexpr auto t5 = fp_t<uint32_t,2>(2.75) * fp_t<uint32_t,8>(10.25);
	static_assert(std::is_same<decltype(t5)::base_type, uint32_t>::value, "");
	static_assert(decltype(t5)::exp{} == 10, "");
	static_assert(t5 == fp_t<uint32_t,10>(2.75 * 10.25), "");
};
