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
#include "src/macros.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GET_TOKEN(KEYWORD)				\
	(struct token){					\
		.type = KEYWORD,			\
			.lexeme = (struct substring)	\
			{				\
				.start = scanner.start,	\
				.end = scanner.current	\
			},				\
			.line = scanner.line		\
			}
#define IS_DIGIT(d) (d >= '0' && d <= '9')
#define IS_ALPHA(c) ((c >= 'A' && c <= 'z') || c == '_')

struct scanner {
	/* start of the lexeme to be included in the next token
	 * reset at every token */
	char *start;
	/* pointer to current character of the source code */
	char *current;
	/* line counter */
	int line;
};

static struct scanner scanner = (struct scanner){
	.start = 0,
	.current = 0,
	.line = 0,
};

/* fill up `ta` with the tokens from the source file
 * interface for `get_token()` */
static void scan_tokens(struct token_vector *ta);
/* get the next token, heart of the scanner */
static struct token get_token();
/* interface for advancing `current`
 * return the character that was advanced */
static char advance();

/* return the next string and avance `current` */
static struct token string();
/* return the next digit and avance `scanner.current` */
static struct token digit();
/* return the next identifier or keyword and avance `scanner.current` */
static struct token identifier();
/* trie to the lexeme and to return the right token */
static enum token_type get_keyword_type(const char *str);
/* compare the current lexeme with `str`, only used by `get_keyword_type()`
 * supports sub-string comparisons for usage in a trie
 * return the token_type `t` if there is a match
 * return TOKEN_IDENTIFIER if there is no match */
static enum token_type keywordcmp(int offset, const char* str, enum token_type t);

struct scan *scan_init(const char *filename)
{
	struct source *src = source_new(filename);
	scanner.current = src->string;
	struct token_vector *ta = token_vector_init();

	scan_tokens(ta);

	struct scan *s = malloc(sizeof(struct scan));

	ASSERT(s != NULL, "Failed to allocate %zu bytes in scan_init.",
		sizeof(struct scan));

	*s = (struct scan){.source = src, .tokens = ta};

	return s;
}

void scan_free(struct scan *s)
{
	source_close(s->source);
	token_vector_del(s->tokens);
	free(s);
}

static void scan_tokens(struct token_vector *ta)
{
	struct token t;
	do {
		scanner.start = scanner.current;
		t = get_token();
		token_vector_add(ta, t);
	} while (t.type != TOKEN_END_OF_FILE);
}

static struct token get_token()
{
	switch (scanner.current[0]) {
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
	case '%': advance(); return GET_TOKEN(TOKEN_PERCENT);
	case '/': advance(); return GET_TOKEN(TOKEN_SLASH);
	case '\t': advance(); return GET_TOKEN(TOKEN_TAB);
	case '\0': return GET_TOKEN(TOKEN_END_OF_FILE);
	case '\n':
		scanner.line++;
		advance();
		return GET_TOKEN(TOKEN_NEWLINE);
		/* Comment */
	case '#':
		advance();
		while (scanner.current[0] != '\n' && scanner.current[0] != '\0') advance();
		scanner.start = scanner.current;
		return get_token();

		/* one or two character tokens */
	case '-':
		advance();
		if (scanner.current[0] == '>') {
			advance();
			return GET_TOKEN(TOKEN_ARROW);
		}
		return GET_TOKEN(TOKEN_MINUS);
	case ':':
		advance();
		if (scanner.current[0] == ':') {
			advance();
			return GET_TOKEN(TOKEN_COLON_COLON);
		}
		return GET_TOKEN(TOKEN_COLON);
	case '!':
		advance();
		if (scanner.current[0] == '=') {
			advance();
			return GET_TOKEN(TOKEN_BANG_EQUAL);
		}
		return GET_TOKEN(TOKEN_BANG);
	case '=':
		advance();
		if (scanner.current[0] == '=') {
			advance();
			return GET_TOKEN(TOKEN_EQUAL_EQUAL);
		}
		return GET_TOKEN(TOKEN_EQUAL);
	case '>':
		advance();
		if (scanner.current[0] == '=') {
			advance();
			return GET_TOKEN(TOKEN_GREATER_EQUAL);
		}
		return GET_TOKEN(TOKEN_GREATER);
	case '<':
		advance();
		if (scanner.current[0] == '=') {
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
		scanner.start = scanner.current;
		return get_token();

		/* literals */
	case '"': advance(); return string();

	default:
		if (IS_DIGIT(scanner.current[0])) return digit();
		if (IS_ALPHA(scanner.current[0])) return identifier();

		fprintf(stderr,
			"Unexpected character %c at line %d.\n",
			scanner.current[0], scanner.line);

		/* invalid */
		advance();
		return GET_TOKEN(TOKEN_INVALID);
	}
}

static char advance()
{
	if (scanner.current[0] == '\0') return '\0';

	scanner.current++;

	return scanner.current[-1];
}

static struct token string()
{
	/* used to print error message */
	int line_begin = scanner.line;

	while (scanner.current[0] != '"') {
		if (scanner.current[0] == '\n') scanner.line++;
		if (scanner.current[0] == '\\') advance();

		if (scanner.current[0] != '\0') {
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
	int max_digit_size = 1024;
	while (IS_DIGIT(scanner.current[0])) {
		advance();
                max_digit_size--;
	}

	if (scanner.current[0] != '.') goto return_digit;

	/* disallow defining a float as 'n.' */
	if (!IS_DIGIT(scanner.current[1])) goto return_invalid;

	while (IS_DIGIT(scanner.current[0])) {
		advance();
		max_digit_size--;
	}

return_digit:
	/* max identifier size of 1024 */
	if (max_digit_size <= 0) {
		fprintf(stderr,
			"[line %d] ERROR: Digit exceeds 1024 characters.\n",
			scanner.line);
		return GET_TOKEN(TOKEN_INVALID);
	}

	return GET_TOKEN(TOKEN_NUMBER);

return_invalid:
	fprintf(stderr,
		"Trailing period not allowed at line %d.\n",
		scanner.line);
	/* eat garbage input */
	while (IS_ALPHA(scanner.current[0]) || IS_DIGIT(scanner.current[0])) advance();
	return GET_TOKEN(TOKEN_INVALID);
}

static struct token identifier()
{
	
	int max_id_size = 1024;
	while (IS_ALPHA(scanner.current[0]) || IS_DIGIT(scanner.current[0])) {
		advance();
		max_id_size--;
	}

	/* max identifier size of 1024 */
	if (max_id_size <= 0) {
		fprintf(stderr,
			"[line %d] ERROR: Identifier exceeds 1024 characters.\n",
			scanner.line);
		return GET_TOKEN(TOKEN_INVALID);
	}

	char *string = sbstr2str(&(struct substring){.start=scanner.start, .end=scanner.current});

	enum token_type t = get_keyword_type(string);
	return GET_TOKEN(t);
}

static enum token_type get_keyword_type(const char *str) {
	switch(str[0]) {
	case 'a':
		switch (str[1]) {
		case 'n': return keywordcmp(2, "d", TOKEN_AND); /* and */
		case 'r': return keywordcmp(2, "ray", TOKEN_ARRAY); /* array */
		case 's': if (str[2] == '\0') return TOKEN_AS; /* as */
			/* fall through */
		default: return TOKEN_IDENTIFIER;
		}
	case 'b':
		switch (str[1]) {
		case 'o': return keywordcmp(2, "ol", TOKEN_BOOL); /* bool */
		case 'r': return keywordcmp(2, "eak", TOKEN_BREAK); /* break */
		case 'y': return keywordcmp(2, "te", TOKEN_BYTE); /* byte */
		default: return TOKEN_IDENTIFIER;
		}
	case 'c':
		/* compare 'on' in 'const' and 'continue' */
		if (strncmp(scanner.start + 1, str + 1, 2) != 0) return TOKEN_IDENTIFIER;

		switch (str[3]) {
		case 's': return keywordcmp(4, "t", TOKEN_CONST); /* const */
		case 't': return keywordcmp(4, "inue", TOKEN_CONTINUE); /* continue */
		default: return TOKEN_IDENTIFIER;
		}
	case 'e':
		switch (str[1]) {
		case 'l': return keywordcmp(2, "se", TOKEN_ELSE); /* else */
		case 'n': return keywordcmp(2, "um", TOKEN_ENUM); /* enum */
		default: return TOKEN_IDENTIFIER;
		}
	case 'f':
		switch (str[1]) {
		case 'a': return keywordcmp(2, "lse", TOKEN_FALSE); /* false */
		case 'l': return keywordcmp(2, "oat", TOKEN_FLOAT); /* float */
		case 'u': return keywordcmp(2, "nc", TOKEN_FUNC); /* func */
		default: return TOKEN_IDENTIFIER;
		}
	case 'i':
		switch (str[1]) {
		case 'n': return keywordcmp(2, "t", TOKEN_FUNC); /* int */
		case 'f': if (str[2] == '\0') return TOKEN_IF; /* if */
			 /* fall through */
		default: return TOKEN_IDENTIFIER;
		}
	case 'm': return keywordcmp(1, "ap", TOKEN_MAP); /* map */
	case 'o': return keywordcmp(1, "r", TOKEN_OR);	 /* or */
	case 'p':
		switch (str[1]) {
		case 'a': return keywordcmp(2, "ss", TOKEN_PASS); /* pass */
		case 'r':
			/* compare 'int' in 'print' and 'print_err' */
			if (strncmp(scanner.start + 2, str + 2, 3) != 0) return TOKEN_IDENTIFIER;
			if (str[5] == '\0') return TOKEN_PRINT; /* print */
			return keywordcmp(5, "_err", TOKEN_PRINT_ERR); /* print_err */
		default: return TOKEN_IDENTIFIER;
		}
	case 'r':
		if (str[1] != 'e') return TOKEN_IDENTIFIER;
		switch (str[2]) {
		case 'c': return keywordcmp(3, "ipe", TOKEN_RECIPE); /* recipe */
		case 't': return keywordcmp(3, "urn", TOKEN_RETURN); /* return */
		case 'f': if (str[3] == '\0') return TOKEN_REF; /* ref */
			/* fall through */
		default: return TOKEN_IDENTIFIER;
		}
	case 's':
		switch (str[1]) {
		case 'b': return keywordcmp(2, "yte", TOKEN_BYTE); /* sbyte */
		case 't': return keywordcmp(2, "r", TOKEN_STR); /* str */
		default: return TOKEN_IDENTIFIER;
		}
	case 't': return keywordcmp(1, "rue", TOKEN_TRUE); /* true */
	case 'u': return keywordcmp(1, "int", TOKEN_UINT); /* uint */
	case 'w': return keywordcmp(1, "hile", TOKEN_WHILE); /* while */
	default:
		return TOKEN_IDENTIFIER;
	}
}

static enum token_type keywordcmp(int offset, const char* str, enum token_type t)
{
	/* convert the current lexeme substring into a string able to
	 * be compared by `strcmp()` */
	struct substring *sbstr = &(struct substring){
		.start = scanner.start,
		.end = scanner.current
	};

	if (strcmp(sbstr2str(sbstr) + offset, str) == 0) return t;
	return TOKEN_IDENTIFIER;
}
