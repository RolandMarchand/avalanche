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

#include "compiler.h"
#include "src/value.h"
#include "type.h"

#include <stdio.h>
#include <stdlib.h>

#define CURRENT_TOKEN_IS(...)						\
	__TOKEN_IS__(parser.current_token, (enum token_type[]){__VA_ARGS__, -1})

struct parser parser;

static struct token *advance();

static struct value expression();
static struct value equality();
static struct value comparison();
static struct value term();
static struct value factor();
static struct value unary();
static struct value primary();

static int __TOKEN_IS__(const struct token *tok, const enum token_type type[]);

void parse(struct scan *sc)
{
	parser.current_token = sc->tokens->array;
	parser.panic = 0;
	struct value val = expression();
	printf("%g\n", val.as.float_p);
}

static struct token *advance() {
	if (CURRENT_TOKEN_IS(TOKEN_END_OF_FILE)) {
		return parser.current_token;
	}
	return parser.current_token++;
}

static struct value expression()
{
	return equality();
}

static struct value equality()
{
	struct value val = comparison();

	if (CURRENT_TOKEN_IS(TOKEN_EQUAL_EQUAL, TOKEN_BANG_EQUAL)) {
		switch (advance()->type) {
		case TOKEN_EQUAL_EQUAL:
			return value_equal(val, comparison());
		case TOKEN_BANG_EQUAL:
			return value_not_equal(val, comparison());
		default: break;
		}
	}
	return val;
}

static struct value comparison()
{
	struct value val = term();

	while (CURRENT_TOKEN_IS(TOKEN_GREATER, TOKEN_GREATER_EQUAL,
			TOKEN_LESS, TOKEN_LESS_EQUAL)) {
		switch (advance()->type) {
		case TOKEN_GREATER:
			return value_greater(val, term());
		case TOKEN_GREATER_EQUAL:
			return value_greater_or_equal(val, term());
		case TOKEN_LESS:
			return value_less(val, term());
		case TOKEN_LESS_EQUAL:
			return value_less_or_equal(val, term());
		default: break;
		}
	}
	return val;
}

static struct value term()
{
	struct value val = factor();

	while (CURRENT_TOKEN_IS(TOKEN_PLUS, TOKEN_MINUS)) {
		switch (advance()->type) {
		case TOKEN_PLUS:
			return value_add(val, factor());
		case TOKEN_MINUS:
			return value_substract(val, factor());
		default: break;	/* should not reach here */
		}
	}
	return val;
}

static struct value factor()
{
	struct value val = unary();

	while (CURRENT_TOKEN_IS(TOKEN_STAR, TOKEN_SLASH, TOKEN_PERCENT)) {
		switch (advance()->type) {
		case TOKEN_STAR:
			return value_multiply(val, unary());
		case TOKEN_SLASH:
			return value_divide(val, unary());
		case TOKEN_PERCENT:
			return value_modulo(val, unary());
		default: break;	/* should not reach here */ 
		}
	}
	return val;
}

static struct value unary()
{
	if (CURRENT_TOKEN_IS(TOKEN_MINUS, TOKEN_BANG)) {
		switch (advance()->type) {
		case TOKEN_MINUS:
			return value_negate(unary());
		case TOKEN_BANG:
			return value_logical_not(unary());
		default: break;	/* should not reach here */
		}
	}
	return primary();
}

static struct value primary()
{
	struct token* t = advance();

        switch (t->type) {
	case TOKEN_CONSTANT_INT:
		return GET_VALUE_INT(atoi(sbstr2str(&t->lexeme)));
	case TOKEN_CONSTANT_FLOAT:
		return GET_VALUE_FLOAT(atof(sbstr2str(&t->lexeme)));
	case TOKEN_LEFT_PAREN: {
		/* advance the current token
		 * `t` is now obsolete */
		struct value value = expression(); 	
		if (advance()->type != TOKEN_RIGHT_PAREN) {
			COMPILER_REPORT(parser.current_token->line,
				"Expected ')' after expression.");
		}
		return value;
	}
	default:
		COMPILER_REPORT(t->line, "No expression found.");
	}
	return (struct value){};
}

static int __TOKEN_IS__(const struct token *tok, const enum token_type type[])
{
	int NOT_A_TOKEN = -1;
	for (int i = 0; type[i] != NOT_A_TOKEN; i++) {
		if (tok->type == type[i]) return 1;
	}
        return 0;
}
