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

#include "scanner.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GET_TOKEN(KEYWORD)				\
	(struct token){					\
		.type = KEYWORD,			\
			.lexeme = (struct substring)	\
			{				\
				.start = start,		\
				.end = current		\
			},				\
			.line = line			\
			}
#define IS_DIGIT(d) (d >= '0' && d <= '9')
#define IS_ALPHA(c) ((c >= 'A' && c <= 'z') || c == '_')

static int line = 1;
static char *start = 0;
static char *current = 0;
static char valid_input = 1;

static void scan_tokens(struct token_vector *ta);
static struct token get_token();
static char advance();

static struct token string();
static struct token digit();
static struct token identifier();
static enum token_type keywordcmp(int offset, const char* str, enum token_type t);
static enum token_type get_keyword_type(const char *str);

struct scan *scan_init(const char *filename)
{
	struct source *src = source_new(filename);
	current = src->string;
	struct token_vector *ta = token_vector_init();

	scan_tokens(ta);

	if (!valid_input) exit(1);

	struct scan *s = malloc(sizeof(struct scan));

	if (s == NULL) {
		fprintf(stderr,
			"Failed to allocate %zu bytes in scan_init.",
			sizeof(struct scan));
		exit(1);
	}

	*s = (struct scan){.source = src, .tokens = ta};

	return s;
}

void scan_del(struct scan *s)
{
	source_close(s->source);
	token_vector_del(s->tokens);
	free(s);
}

static void scan_tokens(struct token_vector *ta)
{
	struct token t;
	do {
		start = current;
		t = get_token();
		token_vector_add(ta, t);
	} while (t.type != TOKEN_END_OF_FILE);
}

static struct token get_token()
{
	switch (current[0]) {
		/* single-character tokens */
	case '(': advance(); return GET_TOKEN(TOKEN_LEFT_PAREN);
	case ')': advance(); return GET_TOKEN(TOKEN_RIGHT_PAREN);
	case '{': advance(); return GET_TOKEN(TOKEN_LEFT_BRACE);
	case '}': advance(); return GET_TOKEN(TOKEN_RIGHT_PAREN);
	case '[': advance(); return GET_TOKEN(TOKEN_LEFT_SQUARE);
	case ']': advance(); return GET_TOKEN(TOKEN_RIGHT_SQUARE);
	case ',': advance(); return GET_TOKEN(TOKEN_COMMA);
	case '.': advance(); return GET_TOKEN(TOKEN_DOT);
	case '+': advance(); return GET_TOKEN(TOKEN_PLUS);
	case '*': advance(); return GET_TOKEN(TOKEN_STAR);
	case '%': advance(); return GET_TOKEN(TOKEN_MODULO);
	case '/': advance(); return GET_TOKEN(TOKEN_SLASH);
	case '\t': advance(); return GET_TOKEN(TOKEN_TAB);
	case '\0': return GET_TOKEN(TOKEN_END_OF_FILE);
	case '\n':
		line++;
		advance();
		return GET_TOKEN(TOKEN_NEWLINE);
		/* Comment */
	case '#':
		advance();
		while (current[0] != '\n' && current[0] != '\0') advance();
		start = current;
		return get_token();

		/* one or two character tokens */
	case '-':
		advance();
		if (current[0] == '>') {
			advance();
			return GET_TOKEN(TOKEN_ARROW); 
		}
		return GET_TOKEN(TOKEN_MINUS);
	case ':':
		advance();
		if (current[0] == ':') {
			advance();
			return GET_TOKEN(TOKEN_COLON_COLON);
		}
		return GET_TOKEN(TOKEN_COLON);
	case '!':
		advance();
		if (current[0] == '=') {
			advance();
			return GET_TOKEN(TOKEN_BANG_EQUAL);
		}
		return GET_TOKEN(TOKEN_BANG);
	case '=':
		advance();
		if (current[0] == '=') {
			advance();
			return GET_TOKEN(TOKEN_EQUAL_EQUAL);
		}
		return GET_TOKEN(TOKEN_EQUAL);
	case '>':
		advance();
		if (current[0] == '=') {
			advance();
			return GET_TOKEN(TOKEN_GREATER_EQUAL);
		}
		return GET_TOKEN(TOKEN_GREATER);
	case '<':
		advance();
		if (current[0] == '=') {
			advance();
			return GET_TOKEN(TOKEN_LESS_EQUAL);
		}
		return GET_TOKEN(TOKEN_LESS);

		/* ignored */
	case ' ':
	case '\f':
	case '\v':
	case '\r':
		advance();
		start = current;
		return get_token();

		/* literals */
	case '"': advance(); return string();

	default:
		if (IS_DIGIT(current[0])) return digit();
		if (IS_ALPHA(current[0])) return identifier();

		fprintf(stderr,
			"Unexpected character %c at line %d.\n",
			current[0], line);

		/* invalid */
		valid_input = 0;
		advance();
		return GET_TOKEN(TOKEN_INVALID);
	}
}

static char advance()
{
	if (current[0] == '\0') return '\0';

	current++;

	return current[-1];
}

static struct token string()
{
	/* used to print error message */
	int line_begin = line; 

	while (current[0] != '"') {
		if (current[0] == '\n') line++;
		if (current[0] == '\\') advance();

		if (current[0] != '\0') {
			advance();
			continue;
		}

		fprintf(stderr, "Unterminated string at line %d.", line_begin);
		return GET_TOKEN(TOKEN_INVALID);
	}

	advance();
	return GET_TOKEN(TOKEN_STRING);
}

static struct token digit()
{
	while (IS_DIGIT(current[0])) advance();

	if (current[0] != '.') goto return_digit;

	if (!IS_DIGIT(current[1])) goto return_invalid;

	while (IS_DIGIT(current[0])) advance();

return_digit:
	return GET_TOKEN(TOKEN_NUMBER);

return_invalid:
	fprintf(stderr,
		"Expected %c at line %d to be a number after the '.'.",
		current[1], line);
	while (IS_ALPHA(current[0]) || IS_DIGIT(current[0])) advance();
	return GET_TOKEN(TOKEN_INVALID);
}

static struct token identifier()
{
	while (IS_ALPHA(current[0]) || IS_DIGIT(current[0])) advance();

	struct substring *sbstr = &(struct substring){.start=start, .end=current};
	char str[SUBSTRING_LENGTH(*sbstr)];
	sbstrcpy(sbstr, str);

	enum token_type t = get_keyword_type(str);
	return GET_TOKEN(t);
}

static enum token_type keywordcmp(int offset, const char* str, enum token_type t)
{
	struct substring *sbstr = &(struct substring){
		.start = start,
		.end = current
	};
	char s_sbstr[SUBSTRING_LENGTH(*sbstr)];
	sbstrcpy(sbstr, s_sbstr);
	
	if (strcmp(s_sbstr + offset, str) == 0) return t;
	return TOKEN_IDENTIFIER;
}

/* trie to the lexeme and to return the right token */
static enum token_type get_keyword_type(const char *str) {
	switch(str[0]) {
	case 'a':
		switch (str[1]) {
		case 'n': return keywordcmp(2, "d", TOKEN_AND);
		case 'r': return keywordcmp(2, "ray", TOKEN_ARRAY);
		case 's': if (str[2] == '\0') return TOKEN_AS;
		default: return TOKEN_IDENTIFIER;
		}
	case 'b':
		switch (str[1]) {
		case 'o': return keywordcmp(2, "ol", TOKEN_BOOL);
		case 'r': return keywordcmp(2, "eak", TOKEN_BREAK);
		case 'y': return keywordcmp(2, "te", TOKEN_BYTE);
		default: return TOKEN_IDENTIFIER;
		}
	case 'c':
		/* compare 'on' in 'const' and 'continue' */
		if (strncmp(start + 1, str + 1, 2) != 0) return TOKEN_IDENTIFIER;

		switch (str[3]) {
		case 's': return keywordcmp(4, "t", TOKEN_CONST);
		case 't': return keywordcmp(4, "inue", TOKEN_CONTINUE);
		default: return TOKEN_IDENTIFIER;
		}
	case 'e':
		switch (str[1]) {
		case 'l': return keywordcmp(2, "se", TOKEN_ELSE);
		case 'n': return keywordcmp(2, "um", TOKEN_ENUM);
		default: return TOKEN_IDENTIFIER;
		}
	case 'f':
		switch (str[1]) {
		case 'a': return keywordcmp(2, "lse", TOKEN_FALSE);
		case 'l': return keywordcmp(2, "oat", TOKEN_FLOAT);
		case 'u': return keywordcmp(2, "nc", TOKEN_FUNC);
		default: return TOKEN_IDENTIFIER;
		}
	case 'i':
		switch (str[1]) {
		case 'n': return keywordcmp(2, "t", TOKEN_FUNC);
		case 'f': if (str[2] == '\0') return TOKEN_IF;
		default: return TOKEN_IDENTIFIER;
		}
	case 'm': return keywordcmp(1, "ap", TOKEN_MAP);
	case 'o': return keywordcmp(1, "r", TOKEN_OR);
	case 'p':
		switch (str[1]) {
		case 'a': return keywordcmp(2, "ss", TOKEN_PASS);
		case 'r':
			/* compare 'int' in 'print' and 'print_err' */
			if (strncmp(start + 2, str + 2, 3) != 0) return TOKEN_IDENTIFIER;
			if (str[5] == '\0') return TOKEN_PRINT;
			return keywordcmp(5, "_err", TOKEN_PRINT_ERR);
		default: return TOKEN_IDENTIFIER;
		}
	case 'r':
		if (str[1] != 'e') return TOKEN_IDENTIFIER;
		switch (str[2]) {
		case 'c': return keywordcmp(3, "ipe", TOKEN_RECIPE);
		case 't': return keywordcmp(3, "urn", TOKEN_RETURN);
		case 'f': if (str[3] == '\0') return TOKEN_REF;
		default: return TOKEN_IDENTIFIER;
		}
	case 's':
		switch (str[1]) {
		case 'b': return keywordcmp(2, "yte", TOKEN_BYTE);
		case 't': return keywordcmp(2, "r", TOKEN_STR);
		default: return TOKEN_IDENTIFIER;
		}
	case 't': return keywordcmp(1, "rue", TOKEN_TRUE);
	case 'u': return keywordcmp(1, "int", TOKEN_UINT);
	case 'w': return keywordcmp(1, "hile", TOKEN_WHILE);
	default:
		return TOKEN_IDENTIFIER;
	}
}
