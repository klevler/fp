/*
 * Copyright (c) 2014, Matheus Izvekov <mizvekov@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "fp.hpp"
#include "static_tests_int.hpp"
#include "static_tests_float.hpp"
#include "static_tests_ranged.hpp"
#include "static_tests_ranged_fp.hpp"
#include "static_tests_totem.hpp"

TEST_CASE( "fp tests", "[fp]" ) {
	fp<uint16_t,8> a = 1.25;
	REQUIRE( double(a) == 1.250 );

	a += fp<uint16_t,8>(0.25);
	REQUIRE( double(a) == 1.500 );

	a += fp<uint16_t,4>(0.125);
	REQUIRE( double(a) == 1.625 );

	--a;
	REQUIRE( double(a) == 0.625 );

	fp<int16_t,8> b;
	b = 0.75;
	REQUIRE( double(b) == 0.750 );

	b -= fp<int16_t,8>(1.25);
	REQUIRE( double(b) == -0.500 );

	b++;
	REQUIRE( double(b) == 0.500 );

	auto c = a * b;
	REQUIRE( double(c) == 0.3125 );

	REQUIRE( fabs(double(M_PI) - double(fp<uint64_t,32>(M_PI))) <= 2e-10 );
}
