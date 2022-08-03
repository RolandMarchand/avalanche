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

#include "parser.h"
#include "error.h"
#include "src/value.h"
#include "src/macros.h"

#include <stdlib.h>
#include <stdio.h>

/* defined in parser.c */
extern struct parser parser;

struct value value_negate(const struct value *val)
{
	switch (val->type) {
	case VALUE_INT: return GET_VALUE_INT(-val->as.integer);
	case VALUE_FLOAT: return GET_VALUE_FLOAT(-val->as.float_p);
	/* TODO: complete error reporting for other types */
	default:
		COMPILER_REPORT(parser.current_token->line,
				"Negation invalid, value is not a number.");
		return (struct value){};
	}
}

struct value value_logical_not(const struct value *val)
{
	switch (val->type) {
	case VALUE_INT: return GET_VALUE_INT(!val->as.integer);
	case VALUE_FLOAT: return GET_VALUE_FLOAT(!val->as.float_p);
	case VALUE_BOOL: return GET_VALUE_BOOL(!val->as.bool);
	/* TODO: complete error reporting for other types */
	default:
		COMPILER_REPORT(parser.current_token->line,
				"Logical not operation impossible.");
		return (struct value){};
	}
}

struct value value_add(const struct value *val1, const struct value *val2)
{
	switch (val1->type) {
	case VALUE_INT:
		if (val2->type == VALUE_INT) {
			return GET_VALUE_INT(val1->as.integer
					+ val2->as.integer);
		}
		if (val2->type == VALUE_FLOAT) {
			return GET_VALUE_FLOAT(val1->as.integer
					+ val2->as.float_p); 
		}
	case VALUE_FLOAT: return GET_VALUE_FLOAT(!val->as.float_p);
	case VALUE_BOOL: return GET_VALUE_BOOL(!val->as.bool);
	/* TODO: complete error reporting for other types */
	default:
		COMPILER_REPORT(parser.current_token->line,
				"Value not a number.");
		return (struct value){};
	}
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
