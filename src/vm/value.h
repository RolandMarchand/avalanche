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

#pragma once

#include <stdint.h>

#define GET_VALUE_NUMBER(num)					\
	((struct value){					\
		.as.number = (num),				\
		.type = VALUE_NUMBER				\
	})
#define GET_VALUE_BOOL(boolean)					\
	((struct value) {					\
		.as.bool = !!(boolean),				\
		.type = VALUE_BOOL				\
	})

enum value_type {
	VALUE_NUMBER = 0,
	VALUE_BOOL,
	VALUE_NIL
};

struct value {
	union {
		double number;
		uint8_t bool;
		void *structure;
	} as;
	enum value_type type;
};

struct value value_negate(struct value val);
struct value value_logical_not(struct value val);
struct value value_add(struct value val1, struct value val2);
struct value value_substract(struct value val1, struct value val2);
struct value value_multiply(struct value val1, struct value val2);
struct value value_divide(struct value val1, struct value val2);
struct value value_modulo(struct value val1, struct value val2);
struct value value_greater(struct value val1, struct value val2);
struct value value_greater_or_equal(struct value val1, struct value val2);
struct value value_less(struct value val1, struct value val2);
struct value value_less_or_equal(struct value val1, struct value val2);
struct value value_equal(struct value val1, struct value val2);
struct value value_not_equal(struct value val1, struct value val2);
