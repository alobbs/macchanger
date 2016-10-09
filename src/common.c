/*
 * common.c
 *
 * Copyright (C) 2016 - Victor Hugo Santos Pucheta <vctrhgsp@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include "common.h"

char *
format_msg (const char *format, va_list va)
{
	size_t size;
	char *msg;
	va_list va_copy;

	va_copy (va_copy, va);
	size = vsnprintf (NULL, 0, format, va_copy) + 1;
	va_end (va_copy);

	msg = xmalloc (size);
	vsnprintf (msg, size, format, va);
	return msg;
}

void
error(const char *format, ...)
{
	va_list va;
	char *msg;

	va_start(va, format);
	msg = format_msg(format, va);
	va_end(va);

	fprintf(stderr, "[ERROR]: %s\n", msg);
	free(msg);
}

void
warning(const char *format, ...)
{
	va_list va;
	char *msg;

	va_start(va, format);
	msg = format_msg(format, va);
	va_end(va);

	fprintf(stderr, "[WARNING]: %s\n", msg);
	free(msg);
}

void
fatal(const char *format, ...)
{
	va_list va;
	char *msg;

	va_start(va, format);
	msg = format_msg(format, va);
	va_end(va);

	fprintf(stderr, "[FATAL_ERROR]: %s\n", msg);
	free(msg);
	exit(EXIT_FAILURE);
}

void *
xmalloc(size_t size)
{
	void *ptr;

	if (size == 0)
		fatal("Zero size");

	ptr = malloc(size);
	if (ptr == NULL)
		fatal("Can't allocate memory!");
	return ptr;
}

void *
xcalloc(size_t nmemb, size_t size)
{
	void *ptr;

	if (size == 0 || nmemb == 0)
		fatal("Zero size");

	ptr = calloc(nmemb, size);
	if (ptr == NULL)
		fatal("Can't allocate memory!");
	return ptr;
}

