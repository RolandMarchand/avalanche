/*
 * Copyright (c) 2022, Roland Marchand <roland.marchand@protonmail.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "value.h"
#include "src/macros.h"

#include <stdlib.h>
#include <stdio.h>

struct value value_negate(struct value val)
{
	ASSERT(val.type == VALUE_NUMBER, "Value not a number.");
	val.as.number = -val.as.number;
	return val;
}

struct value value_logical_not(struct value val)
{
	ASSERT(val.type == VALUE_NUMBER, "Value not a number.");
	val.as.number = !val.as.number;
	return val;
}

struct value value_add(struct value val1, struct value val2)
{
	ASSERT(val1.type == VALUE_NUMBER
		&& val2.type == VALUE_NUMBER, "Value not a number.");
	val1.as.number += val2.as.number;
	return val1;
}

struct value value_substract(struct value val1, struct value val2)
{
	ASSERT(val1.type == VALUE_NUMBER
		&& val2.type == VALUE_NUMBER, "Value not a number.");
	val1.as.number -= val2.as.number;
	return val1;
}

struct value value_multiply(struct value val1, struct value val2)
{
	ASSERT(val1.type == VALUE_NUMBER
		&& val2.type == VALUE_NUMBER, "Value not a number.");
	val1.as.number *= val2.as.number;
	return val1;
}

struct value value_divide(struct value val1, struct value val2)
{
	ASSERT(val1.type == VALUE_NUMBER
		&& val2.type == VALUE_NUMBER, "Value not a number.");
	val1.as.number /= val2.as.number;
	return val1;
}

struct value value_modulo(struct value val1, struct value val2)
{
	ASSERT(val1.type == VALUE_NUMBER
		&& val2.type == VALUE_NUMBER, "Value not a number.");
	val1.as.number = (int)val1.as.number % (int)val2.as.number;
	return val1;
}

struct value value_greater(struct value val1, struct value val2)
{
	ASSERT(val1.type == VALUE_NUMBER
		&& val2.type == VALUE_NUMBER, "Value not a number.");
	return GET_VALUE_BOOL(val1.as.number > val2.as.number);
}

struct value value_greater_or_equal(struct value val1, struct value val2)
{
	ASSERT(val1.type == VALUE_NUMBER
		&& val2.type == VALUE_NUMBER, "Value not a number.");
	return GET_VALUE_BOOL(val1.as.number >= val2.as.number);
}

struct value value_less(struct value val1, struct value val2)
{
	ASSERT(val1.type == VALUE_NUMBER
		&& val2.type == VALUE_NUMBER, "Value not a number.");
	return GET_VALUE_BOOL(val1.as.number < val2.as.number);
}

struct value value_less_or_equal(struct value val1, struct value val2)
{
	ASSERT(val1.type == VALUE_NUMBER
		&& val2.type == VALUE_NUMBER, "Value not a number.");
	return GET_VALUE_BOOL(val1.as.number <= val2.as.number);
}

struct value value_equal(struct value val1, struct value val2)
{
	ASSERT((val1.type == VALUE_NUMBER
		&& val2.type == VALUE_NUMBER
			|| (val1.type == VALUE_BOOL
				&& val2.type == VALUE_BOOL)),
		"Value not a boolean.");
	if (val1.type == VALUE_NUMBER) {
		return GET_VALUE_BOOL(val1.as.number == val2.as.number);
	}

	return GET_VALUE_BOOL(val1.as.bool == val2.as.bool);
}

struct value value_not_equal(struct value val1, struct value val2)
{
	ASSERT((val1.type == VALUE_NUMBER
		&& val2.type == VALUE_NUMBER
			|| (val1.type == VALUE_BOOL
				&& val2.type == VALUE_BOOL)),
		"Value not a boolean.");
	if (val1.type == VALUE_NUMBER) {
		return GET_VALUE_BOOL(val1.as.number != val2.as.number);
	}

	return GET_VALUE_BOOL(val1.as.bool != val2.as.bool);
}
