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

#include "source.h"
#include "src/macros.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

struct source *source_new(const char *file)
{
	struct stat sb;
	ASSERT(stat(file, &sb) != -1, "Invalid input file.");

	int fd = open(file, O_RDONLY);
	char *code = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

	struct source *sf = malloc(sizeof(struct source));

	ASSERT(fd != -1 && code != MAP_FAILED && sf != NULL,
	       "Failed to allocate source file.");

	*sf = (struct source){
		.string = code,
		.file_name = file,
		.size = sb.st_size,
		.file_descriptor = fd
	};

	return sf;
}

void source_close(struct source *sf)
{
	ASSERT(munmap(sf->string, sf->size) != -1 &&
	       close(sf->file_descriptor) != -1, "Unable to close source file.");

	free(sf);
	sf = NULL;
}
